#!/usr/bin/env python3
"""Read-only verification of the 196 exported Qwen assets, optionally on an SD card."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys


def expected_assets():
    files = {"shared_vocab_weight.bf16.bin": 272269312, "final_m06_gamma.bin": 1792,
             "rope_cos.bf16.bin": 262144, "rope_sin.bf16.bin": 262144}
    sizes = {"m07_wqkv": 2064384, "m07_bqkv": 2304, "m11_wo": 1605632,
             "m06_attn_gamma": 1792, "m06_ffn_gamma": 1792,
             "m13_w_gate": 8716288, "m13_w_up": 8716288, "m13_w_down": 8716288}
    for layer in range(24):
        files.update({f"layer{layer:02}_{name}.bin": n for name, n in sizes.items()})
    return files


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    parser.add_argument(
        "--manifest", type=Path,
        help="manifest path; defaults to <directory>/manifest.json",
    )
    parser.add_argument("--quick", action="store_true", help="check names/sizes only; skip SHA256")
    args = parser.parse_args(argv)
    root = args.directory
    manifest_path = args.manifest if args.manifest is not None else root / "manifest.json"
    errors = []
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
    except (OSError, ValueError) as error:
        print(f"FAIL manifest {manifest_path}:", error)
        return 1
    if manifest.get("format") != "QWEN_V2_PS_ASSETS" or set(manifest.get("layer_assets", {})) != {str(i) for i in range(24)}:
        errors.append("manifest must describe QWEN_V2_PS_ASSETS with all 24 layers")
    entries = {}

    def collect(node):
        if isinstance(node, dict):
            if "file" in node and "bytes" in node:
                entries[node["file"]] = node
            for value in node.values():
                collect(value)
        elif isinstance(node, list):
            for value in node:
                collect(value)

    collect(manifest)
    expected = expected_assets()
    if set(entries) != set(expected):
        errors.append("manifest asset names do not match the required 196 files")
    checked = total = 0
    for name, size in expected.items():
        path = root / name
        if not path.is_file() or path.stat().st_size != size:
            errors.append(f"missing/wrong size: {name}, expected {size}")
            continue
        entry = entries.get(name, {})
        if entry.get("bytes") != size:
            errors.append("manifest size mismatch: " + name)
        if not args.quick:
            expected_hash = entry.get("sha256", "")
            if not re.fullmatch(r"[0-9a-fA-F]{64}", expected_hash):
                errors.append("missing manifest SHA256: " + name)
            else:
                digest = hashlib.sha256()
                with path.open("rb") as stream:
                    for block in iter(lambda: stream.read(4 * 1024 * 1024), b""):
                        digest.update(block)
                if digest.hexdigest().lower() != expected_hash.lower():
                    errors.append("SHA256 mismatch: " + name)
        checked += 1
        total += size
        if checked % 24 == 0:
            print(f"checked {checked}/196")
    for error in errors:
        print("FAIL", error)
    print(f"files={checked}/196 bytes={total}/988589824 mode={'names+sizes' if args.quick else 'names+sizes+SHA256'}")
    if errors:
        return 1
    print("MODEL_ASSETS_PASS")
    return 0


if __name__ == "__main__":
    sys.exit(main())
