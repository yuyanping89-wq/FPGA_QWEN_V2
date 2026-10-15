#!/usr/bin/env python3
"""Export Qwen2.5-0.5B BF16 tensors into the V2 PS/Vivado asset layout.

The output directory is directly consumable by qwen_assets_fatfs.c. The
script reads Safetensors headers itself; NumPy is the only Python dependency.
It preserves original BF16 bits and applies the same OG32 packing used by the
M07/M11 weight packer and the active M13 W_OG32_I_L32 interface.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path
from typing import Any

import numpy as np

HIDDEN = 896
INTERMEDIATE = 4864
VOCAB = 151936
LAYERS = 24
LANES = 32


class SafeTensorReader:
    def __init__(self, model_path: Path):
        if model_path.is_file():
            shards = [model_path]
            self.model_dir = model_path.parent
        else:
            self.model_dir = model_path
            shards = sorted(model_path.glob("*.safetensors"))
        if not shards:
            raise FileNotFoundError(f"no .safetensors files in {model_path}")

        self.index: dict[str, tuple[Path, int, dict[str, Any]]] = {}
        for shard in shards:
            with shard.open("rb") as stream:
                header_size_raw = stream.read(8)
                if len(header_size_raw) != 8:
                    raise ValueError(f"truncated Safetensors header: {shard}")
                header_size = struct.unpack("<Q", header_size_raw)[0]
                header = json.loads(stream.read(header_size).decode("utf-8"))
            data_start = 8 + header_size
            for name, metadata in header.items():
                if name != "__metadata__":
                    self.index[name] = (shard, data_start, metadata)

    def metadata(self, name: str) -> dict[str, Any]:
        try:
            return self.index[name][2]
        except KeyError as error:
            raise KeyError(f"missing tensor: {name}") from error

    def read(self, name: str) -> np.ndarray:
        shard, data_start, metadata = self.index[name]
        if metadata.get("dtype") != "BF16":
            raise ValueError(f"{name}: expected BF16, got {metadata.get('dtype')}")
        start, stop = (int(value) for value in metadata["data_offsets"])
        with shard.open("rb") as stream:
            stream.seek(data_start + start)
            raw = stream.read(stop - start)
        if len(raw) != stop - start:
            raise EOFError(f"truncated tensor data: {name}")
        return np.frombuffer(raw, dtype="<u2").reshape(metadata["shape"]).copy()

    def copy_raw(self, name: str, destination: Path) -> int:
        shard, data_start, metadata = self.index[name]
        if metadata.get("dtype") != "BF16":
            raise ValueError(f"{name}: expected BF16, got {metadata.get('dtype')}")
        start, stop = (int(value) for value in metadata["data_offsets"])
        destination.parent.mkdir(parents=True, exist_ok=True)
        with shard.open("rb") as source, destination.open("wb") as output:
            source.seek(data_start + start)
            remaining = stop - start
            while remaining:
                chunk = source.read(min(8 * 1024 * 1024, remaining))
                if not chunk:
                    raise EOFError(f"truncated tensor data: {name}")
                output.write(chunk)
                remaining -= len(chunk)
        return stop - start


def require_shape(reader: SafeTensorReader, name: str, expected: tuple[int, ...]) -> None:
    metadata = reader.metadata(name)
    actual = tuple(int(value) for value in metadata["shape"])
    if actual != expected:
        raise ValueError(f"{name}: expected shape {expected}, got {actual}")
    if metadata.get("dtype") != "BF16":
        raise ValueError(f"{name}: expected BF16, got {metadata.get('dtype')}")


def pack_og32(weight: np.ndarray) -> np.ndarray:
    """Pack [O,I] as [ceil(O/32),I,32], matching layout_id 6."""
    outputs, inputs = weight.shape
    groups = (outputs + LANES - 1) // LANES
    padded = np.zeros((groups * LANES, inputs), dtype=np.uint16)
    padded[:outputs, :] = weight
    return np.ascontiguousarray(padded.reshape(groups, LANES, inputs).transpose(0, 2, 1))


def write_bf16(path: Path, values: np.ndarray) -> dict[str, Any]:
    path.parent.mkdir(parents=True, exist_ok=True)
    np.asarray(values, dtype="<u2").tofile(path)
    return {"file": path.name, "bytes": path.stat().st_size, "sha256": sha256(path)}


def fp32_to_bf16(values: np.ndarray) -> np.ndarray:
    """Convert finite FP32 values to BF16 with round-to-nearest, ties-to-even."""
    bits = np.asarray(values, dtype="<f4").view("<u4")
    rounding = np.uint32(0x7FFF) + ((bits >> 16) & np.uint32(1))
    return ((bits + rounding) >> 16).astype("<u2")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(8 * 1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def parse_layers(spec: str) -> list[int]:
    layers: list[int] = []
    for part in spec.split(","):
        if "-" in part:
            first, last = (int(value) for value in part.split("-", 1))
            layers.extend(range(first, last + 1))
        else:
            layers.append(int(part))
    if len(set(layers)) != len(layers) or any(layer < 0 or layer >= LAYERS for layer in layers):
        raise ValueError(f"layer selection must be unique IDs in 0..{LAYERS - 1}")
    return layers


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("model", type=Path, help="Qwen2.5-0.5B-Instruct Safetensors file or directory")
    parser.add_argument("output", type=Path, help="Empty or dedicated output directory")
    parser.add_argument("--layers", default="0-23", help="Layer list/ranges, e.g. 0-23 or 0,2-4")
    args = parser.parse_args()

    args.output.mkdir(parents=True, exist_ok=True)
    if any(args.output.iterdir()):
        raise ValueError(f"output directory must be empty: {args.output}")

    config_path = args.model / "config.json" if args.model.is_dir() else args.model.parent / "config.json"
    config = json.loads(config_path.read_text(encoding="utf-8"))
    expected_config = {
        "hidden_size": HIDDEN,
        "intermediate_size": INTERMEDIATE,
        "num_hidden_layers": LAYERS,
        "num_attention_heads": 14,
        "num_key_value_heads": 2,
        "vocab_size": VOCAB,
        "rms_norm_eps": 1e-6,
        "rope_theta": 1e6,
        "tie_word_embeddings": True,
    }
    for key, expected in expected_config.items():
        if config.get(key) != expected:
            raise ValueError(f"config {key}: expected {expected!r}, got {config.get(key)!r}")

    reader = SafeTensorReader(args.model)
    manifest: dict[str, Any] = {
        "format": "QWEN_V2_PS_ASSETS",
        "dtype": "BF16 little-endian, original bits",
        "hidden_size": HIDDEN,
        "intermediate_size": INTERMEDIATE,
        "vocab_size": VOCAB,
        "layers": LAYERS,
        "rms_norm_eps": config["rms_norm_eps"],
        "weight_layout": "W_OG32_I_L32_V2 (layout_id=6)",
        "rope_layout": "ROPE_SD_V2 [2048,64], Qwen rotate-half",
        "source_model": str(args.model.resolve()),
        "shared_vocab": {},
        "layer_assets": {},
        "final_m06_gamma": {},
        "rope_tables": {},
    }

    rope_theta = np.float32(config["rope_theta"])
    pair_index = np.arange(0, 64, 2, dtype=np.float32) / np.float32(64.0)
    inverse_frequency = np.power(rope_theta, -pair_index).astype(np.float32)
    positions = np.arange(2048, dtype=np.float32)
    phase_half = positions[:, None] * inverse_frequency[None, :]
    phase = np.concatenate((phase_half, phase_half), axis=1)
    cos_path = args.output / "rope_cos.bf16.bin"
    sin_path = args.output / "rope_sin.bf16.bin"
    cos_info = write_bf16(cos_path, fp32_to_bf16(np.cos(phase)))
    sin_info = write_bf16(sin_path, fp32_to_bf16(np.sin(phase)))
    manifest["rope_tables"] = {
        "cos": {**cos_info, "shape": [2048, 64], "source": "cos(position * theta^(-2i/64)); duplicated halves"},
        "sin": {**sin_info, "shape": [2048, 64], "source": "sin(position * theta^(-2i/64)); duplicated halves"},
        "rope_theta": float(config["rope_theta"]),
    }

    tied_name = "model.embed_tokens.weight"
    require_shape(reader, tied_name, (VOCAB, HIDDEN))
    shared_path = args.output / "shared_vocab_weight.bf16.bin"
    shared_bytes = reader.copy_raw(tied_name, shared_path)
    manifest["shared_vocab"] = {
        "file": shared_path.name,
        "shape": [VOCAB, HIDDEN],
        "bytes": shared_bytes,
        "sha256": sha256(shared_path),
        "source_tensor": tied_name,
        "shared_by": ["M05 embedding", "M14 LM head"],
    }

    layer_manifest: dict[str, Any] = {}
    for layer in parse_layers(args.layers):
        root = f"model.layers.{layer}."
        attention = root + "self_attn."
        mlp = root + "mlp."
        qkv_weights = []
        qkv_biases = []
        for name, rows in (("q_proj", HIDDEN), ("k_proj", 2 * HIDDEN // 14),
                           ("v_proj", 2 * HIDDEN // 14)):
            weight_name = attention + name + ".weight"
            bias_name = attention + name + ".bias"
            require_shape(reader, weight_name, (rows, HIDDEN))
            require_shape(reader, bias_name, (rows,))
            qkv_weights.append(reader.read(weight_name))
            qkv_biases.append(reader.read(bias_name))

        qkv = np.concatenate(qkv_weights, axis=0)
        qkv_bias = np.concatenate(qkv_biases, axis=0)
        if qkv.shape != (HIDDEN + 2 * (2 * HIDDEN // 14), HIDDEN):
            raise ValueError(f"layer {layer}: unexpected fused QKV shape {qkv.shape}")

        assets: dict[str, Any] = {}
        assets["m07_wqkv"] = write_bf16(
            args.output / f"layer{layer:02d}_m07_wqkv.bin", pack_og32(qkv)
        )
        assets["m07_bqkv"] = write_bf16(
            args.output / f"layer{layer:02d}_m07_bqkv.bin", qkv_bias
        )

        wo_name = attention + "o_proj.weight"
        require_shape(reader, wo_name, (HIDDEN, HIDDEN))
        assets["m11_wo"] = write_bf16(
            args.output / f"layer{layer:02d}_m11_wo.bin", pack_og32(reader.read(wo_name))
        )

        input_norm = root + "input_layernorm.weight"
        post_norm = root + "post_attention_layernorm.weight"
        require_shape(reader, input_norm, (HIDDEN,))
        require_shape(reader, post_norm, (HIDDEN,))
        assets["m06_attn_gamma"] = write_bf16(
            args.output / f"layer{layer:02d}_m06_attn_gamma.bin", reader.read(input_norm)
        )
        assets["m06_ffn_gamma"] = write_bf16(
            args.output / f"layer{layer:02d}_m06_ffn_gamma.bin", reader.read(post_norm)
        )

        for projection, output_dim, input_dim, output_name in (
            ("gate_proj", INTERMEDIATE, HIDDEN, "m13_w_gate"),
            ("up_proj", INTERMEDIATE, HIDDEN, "m13_w_up"),
            ("down_proj", HIDDEN, INTERMEDIATE, "m13_w_down"),
        ):
            tensor_name = mlp + projection + ".weight"
            require_shape(reader, tensor_name, (output_dim, input_dim))
            assets[output_name] = write_bf16(
                args.output / f"layer{layer:02d}_{output_name}.bin",
                pack_og32(reader.read(tensor_name)),
            )

        layer_manifest[str(layer)] = assets
        print(f"exported layer {layer:02d}")

    final_norm = "model.norm.weight"
    require_shape(reader, final_norm, (HIDDEN,))
    final_path = args.output / "final_m06_gamma.bin"
    manifest["final_m06_gamma"] = write_bf16(final_path, reader.read(final_norm))
    manifest["final_m06_gamma"]["source_tensor"] = final_norm
    manifest["layer_assets"] = layer_manifest
    (args.output / "manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    print(f"wrote asset manifest: {args.output / 'manifest.json'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
