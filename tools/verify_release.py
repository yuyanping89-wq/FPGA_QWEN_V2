#!/usr/bin/env python3
"""Verify file integrity and required artifacts in this Qwen V2 release."""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import sys
import xml.etree.ElementTree as ET

RELEASE_NAME = "qwen_v2_board_release_20261015"
MANIFEST_NAME = "release_manifest.json"
EXPECTED = {
    "vivado/artifacts/board_tested/system_wrapper_wrapper.bit": "692BEBABB4D95664B8740492CBD5CB2CDDBDAAB602A2927F29123EC9EAC80A17",
    "vivado/proj/qwen/qwen.xsa": "1C98EE866642A592316815D2E602CE0B5C82A6AFE0BE9C2800790EA0945C4246",
}
IGNORED_DIRS = {".git", ".Xil", ".cache", ".pytest_cache", "__pycache__",
                ".metadata", ".settings", "Debug", "Release", "build-review",
                "results", "qwen_model", "model_assets", "weights"}
IGNORED_SUFFIXES = {".pyc", ".jou", ".wdb", ".dcp", ".rpx", ".pb"}
REQUIRED = [
    "vivado/proj/qwen/qwen.xpr",
    "vivado/proj/qwen/qwen.srcs/sources_1/bd/system_wrapper/system_wrapper.bd",
    "vivado/ip_repo/m05_embedding/component.xml",
    "vivado/ip_repo/m06_rmsnorm/component.xml",
    "vivado/ip_repo/m07_qkv_projection/component.xml",
    "vivado/ip_repo/m08_rope_v2/component.xml",
    "vivado/ip_repo/m09_kv_cache_update/component.xml",
    "vivado/ip_repo/m10_attention_v2/component.xml",
    "vivado/ip_repo/m11_attention_out/component.xml",
    "vivado/ip_repo/m12_residual_add/component.xml",
    "vivado/ip_repo/m13_swiglu/component.xml",
    "vivado/ip_repo/m14_lm_head_v2/component.xml",
    "modules/M13_swiglu/variants/row8_candidate_unintegrated/ip/component.xml",
    "vitis/qwen_app/src/main.c",
    "vitis/host_tools/qwen_client.py",
]


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest().upper()


def files_in(root: Path):
    for path in sorted(root.rglob("*")):
        if not path.is_file() or path.name == MANIFEST_NAME:
            continue
        relative = path.relative_to(root)
        if any(part in IGNORED_DIRS or part.endswith((".runs", ".gen")) for part in relative.parts):
            continue
        if path.suffix.lower() in IGNORED_SUFFIXES:
            continue
        yield path


def build_manifest(root: Path) -> dict:
    records = []
    for path in files_in(root):
        records.append({
            "path": path.relative_to(root).as_posix(),
            "bytes": path.stat().st_size,
            "sha256": digest(path),
        })
    return {
        "release": RELEASE_NAME,
        "board_test_date": "2026-10-15",
        "target": "xczu9eg-ffvb1156-2-i",
        "vivado": "2021.1",
        "file_count": len(records),
        "critical_artifacts": EXPECTED,
        "files": records,
    }



def verify_ip_references(root: Path) -> list[str]:
    namespace = {"s": "http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009"}
    ip_dirs = sorted(p.parent for p in root.glob("vivado/ip_repo/*/component.xml"))
    ip_dirs.append(root / "modules/M13_swiglu/variants/row8_candidate_unintegrated/ip")
    errors = []
    for ip_dir in ip_dirs:
        component = ip_dir / "component.xml"
        try:
            tree = ET.parse(component)
        except (OSError, ET.ParseError) as exc:
            errors.append(f"IP_XML {component.relative_to(root).as_posix()}: {exc}")
            continue
        names = tree.findall(".//s:fileSets/s:fileSet/s:file/s:name", namespace)
        for node in names:
            if not node.text:
                continue
            path = (ip_dir / node.text).resolve()
            try:
                path.relative_to(ip_dir.resolve())
            except ValueError:
                errors.append(f"IP_PATH_ESCAPE {component.relative_to(root).as_posix()}: {node.text}")
                continue
            if not path.is_file():
                errors.append(f"IP_FILE_MISSING {component.relative_to(root).as_posix()}: {node.text}")
    return errors
def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--write-manifest", action="store_true",
                        help="refresh release_manifest.json from the files currently present")
    args = parser.parse_args()
    root = args.root.resolve()
    manifest_path = root / MANIFEST_NAME
    if args.write_manifest:
        manifest = build_manifest(root)
        manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        print(f"WROTE {manifest_path} ({manifest['file_count']} files)")
    if not manifest_path.is_file():
        print(f"Missing {manifest_path}", file=sys.stderr)
        return 2
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    errors = []
    records = {item["path"]: item for item in manifest.get("files", [])}
    for rel, record in records.items():
        path = root / Path(rel)
        if not path.is_file():
            errors.append(f"MISSING {rel}")
            continue
        if path.stat().st_size != record["bytes"]:
            errors.append(f"SIZE {rel}")
        if digest(path) != record["sha256"]:
            errors.append(f"SHA256 {rel}")
    actual = {p.relative_to(root).as_posix() for p in files_in(root)}
    listed = set(records)
    for rel in sorted(actual - listed):
        errors.append(f"UNLISTED {rel}")
    for rel in sorted(listed - actual):
        if not (root / rel).is_file():
            continue
        errors.append(f"STALE {rel}")
    for rel in REQUIRED:
        if not (root / Path(rel)).is_file():
            errors.append(f"REQUIRED {rel}")
    for rel, expected_hash in EXPECTED.items():
        path = root / Path(rel)
        if not path.is_file() or digest(path) != expected_hash:
            errors.append(f"CRITICAL_HASH {rel}")
    forbidden = []
    for path in root.rglob("*"):
        if not path.is_file():
            continue
        name = path.name.lower()
        if name.endswith(".safetensors") or name.endswith(".bf16.bin") or name.endswith(".bin"):
            forbidden.append(path.relative_to(root).as_posix())
    if forbidden:
        errors.extend(f"MODEL_ASSET {rel}" for rel in forbidden)
    errors.extend(verify_ip_references(root))
    if errors:
        print("RELEASE_VERIFY_FAILED")
        print("\n".join(errors))
        return 1
    total = sum(item["bytes"] for item in manifest["files"])
    print(f"RELEASE_VERIFY_PASS files={manifest['file_count']} bytes={total}")
    print("BIT_SHA256=" + EXPECTED["vivado/artifacts/board_tested/system_wrapper_wrapper.bit"])
    print("XSA_SHA256=" + EXPECTED["vivado/proj/qwen/qwen.xsa"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())





