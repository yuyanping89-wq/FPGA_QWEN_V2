#!/usr/bin/env python3
"""Local Qwen tokenizer + raw lwIP client. Inference runs entirely on the FPGA.

No model weights are loaded on the PC. Binary profiles never infer DDR time
from beats or treat an unavailable compute-active counter as zero work.
"""
from __future__ import annotations

import argparse
import csv
import json
import socket
import statistics
import struct
import sys
import time
from pathlib import Path

MAGIC = 0x324E5751
MAX_SEQUENCE = 2048
KINDS = ["ip", "cdma", "sd_read", "cpu_copy", "cache"]
PHASES = ["load", "prefill", "decode", "debug"]
STATUS = {0: "OK", 1: "bad argument", 2: "address/size range", 3: "alignment",
          4: "CDMA error", 5: "timeout", 6: "IP status/rows/job error",
          7: "IP build mismatch", 8: "asset missing", 9: "asset I/O",
          12: "PL DDR probe", 13: "internal", 14: "reset PL/PS required", 15: "protocol"}


def label_record(r):
    r["kind_name"] = KINDS[r["kind"]]
    r["phase_name"] = PHASES[r["phase"]]
    m, role = r["module"], r["role"]
    r["module_name"] = f"M{m:02}" if m else "system"
    r["role_name"] = str(role)
    if r["kind"] == 0:
        if m == 6:
            r["role_name"] = ["attn_norm", "ffn_norm", "final_norm"][role]
        elif m == 12:
            r["role_name"] = ["attn_residual", "ffn_residual"][role]
    elif r["kind"] == 1:
        r["role_name"] = ["PS_to_PL", "PL_to_PS"][role]
    elif r["kind"] == 4:
        r["role_name"] = ["flush", "invalidate"][role]
    caps = r["capabilities"]
    cycles = r.get("hw_cycles", 0)
    r["hw_total_us"] = cycles / 100.0 if caps & 1 else None
    # Current image ties compute_active low for every operator.
    r["pure_compute_us"] = r.get("compute_cycles", 0) / 100.0 if caps & 8 else None
    r["ip_internal_ddr_us"] = None
    r["beats_semantics"] = "measured_handshakes" if caps & 2 else "logical" if caps & 4 else "unavailable"
    r["read_bytes"] = r["read_beats"] * 16 if caps & 6 else None
    r["write_bytes"] = r["write_beats"] * 16 if caps & 6 else None
    us = r.get("total_us", r.get("duration_us", 0))
    # Numeric bytes/us equals decimal MB/s; physical beat totals include padding/reloads.
    r["effective_MBps"] = r["bytes"] / us if r["kind"] in (1, 2) and us else None
    r["axi_achieved_MBps"] = ((r["read_beats"] + r["write_beats"]) * 16 /
                                  r["hw_total_us"]) if caps & 3 == 3 and cycles else None
    if r["layer"] == 0xFFFFFFFF:
        r["layer"] = -1
    return r


def decode_profile(data: bytes):
    if len(data) < 160:
        raise ValueError("truncated performance header")
    h = struct.unpack_from("<16I6Q12I", data)
    if h[:5] != (0x32465250, 1, 160, 112, 112):
        raise ValueError("unsupported performance format")
    ns, nt, nk = h[5:8]
    if ns > 1536 or nt > 2048 or nk > 2048 or len(data) != 160 + (ns + nt) * 112 + nk * 8:
        raise ValueError("invalid performance record counts/length")
    names = ["total_us", "prefill_us", "decode_us", "ttft_us", "load_us", "profiler_overhead_us"]
    result = dict(zip(names, h[16:22]))
    result.update(trace_dropped=h[8], stats_dropped=h[9], flags=h[10], prefill_rows=h[11],
                  prompt_tokens=h[12], generated_tokens=h[13], status=struct.unpack("<i", struct.pack("<I", h[14]))[0],
                  accelerator_hz=h[15])
    if h[15] != 100_000_000:
        raise ValueError("client expects this image's 100 MHz operator clock")
    result["last"] = dict(zip(["module", "layer", "hw_status", "build", "ap_ctrl", "job",
                               "cdma_sr", "cdma_source", "cdma_destination", "cdma_bytes", "fault_latched"], h[22:33]))
    stat_names = ["kind", "phase", "module", "layer", "role", "calls", "failures", "capabilities",
                  "total_us", "min_us", "max_us", "hw_cycles", "compute_cycles", "read_beats", "write_beats", "bytes",
                  "hw_samples"]
    trace_names = ["kind", "phase", "module", "layer", "role", "job", "status", "capabilities",
                   "rows", "position", "hw_status", "build", "crc", "source", "destination", "bytes",
                   "start_us", "duration_us", "hw_cycles", "compute_cycles", "read_beats", "write_beats"]
    offset = 160
    result["stats"] = []
    for _ in range(ns):
        v = struct.unpack_from("<8I8QIIQ", data, offset)
        result["stats"].append(label_record(dict(zip(stat_names, v[:17]))))
        offset += 112
    result["trace"] = []
    for _ in range(nt):
        v = list(struct.unpack_from("<16I6Q", data, offset))
        v[6] = struct.unpack("<i", struct.pack("<I", v[6]))[0]
        result["trace"].append(label_record(dict(zip(trace_names, v))))
        offset += 112
    result["token_latencies_us"] = list(struct.unpack_from(f"<{nk}Q", data, offset))
    result["prefill_tokens_per_s"] = h[12] * 1e6 / result["prefill_us"] if result["prefill_us"] else None
    result["decode_tokens_per_s"] = (h[13] - 1) * 1e6 / result["decode_us"] if h[13] > 1 and result["decode_us"] else None
    result["notes"] = ["prefill_us includes final norm and first LM-head (device TTFT); excludes PC tokenization/network",
                       "IP total includes computation, memory access and stalls; pure compute and internal DDR time unavailable",
                       "M06/M09 beats are logical; other eight IPs count actual AXI handshakes",
                       "load statistics are retained from boot; do not add them to request inference latency"]
    return result


def recv_exact(sock, length):
    data = bytearray()
    while len(data) < length:
        chunk = sock.recv(length - len(data))
        if not chunk:
            raise ConnectionError("board closed connection before a complete response")
        data.extend(chunk)
    return bytes(data)


def exchange(sock, tokens, args, request_id=1, command=1):
    flags = int(args.trace) | (int(args.crc) << 1) | (int(args.uart_progress) << 2)
    max_new = args.dma_bytes if command == 3 else args.max_new_tokens
    eos = args.dma_iterations if command == 3 else (0xFFFFFFFF if args.no_eos else 151645)
    packet = struct.pack("<IHHIIIIfIII", MAGIC, 2, command, request_id, len(tokens), max_new,
                         eos, args.repetition_penalty, flags, args.prefill_rows, args.trace_limit)
    packet += struct.pack(f"<{len(tokens)}I", *tokens)
    begin = time.perf_counter()
    sock.sendall(packet)
    h = struct.unpack("<IHH6I", recv_exact(sock, 32))
    if h[0] != MAGIC or h[1] != 2 or h[3] != request_id or h[4] > 2048 or h[8] > 600_000:
        raise ValueError("invalid response header; confirm matching PS source and bitstream")
    generated = list(struct.unpack(f"<{h[4]}I", recv_exact(sock, h[4] * 4)))
    profile = decode_profile(recv_exact(sock, h[8])) if h[8] else None
    return {"request_id": request_id, "status": h[2], "status_text": STATUS.get(h[2], "unknown"),
            "generated_ids": generated, "cache_length": h[5], "failed_module": h[6],
            "failed_layer": -1 if h[7] == 0xFFFFFFFF else h[7], "profile": profile,
            "network_roundtrip_s": time.perf_counter() - begin}


class TextCodec:
    def __init__(self, directory):
        try:
            from tokenizers import Tokenizer
            from jinja2.sandbox import SandboxedEnvironment
        except ImportError as exc:
            raise RuntimeError("install tools/requirements-client.txt first") from exc
        directory = Path(directory)
        self.tokenizer = Tokenizer.from_file(str(directory / "tokenizer.json"))
        config = json.loads((directory / "tokenizer_config.json").read_text(encoding="utf-8"))
        env = SandboxedEnvironment(trim_blocks=True, lstrip_blocks=True)
        self.template = env.from_string(config["chat_template"])
        self.config = config

    def encode(self, messages, chat=True):
        text = self.template.render(messages=messages, tools=None, add_generation_prompt=True,
                                    **{k: v for k, v in self.config.items() if k.endswith("_token")}) if chat else messages[-1]["content"]
        ids = self.tokenizer.encode(text, add_special_tokens=False).ids
        if any(i >= 151936 for i in ids):
            raise ValueError("tokenizer vocabulary does not match current hardware")
        return ids, text

    def decode(self, ids):
        return self.tokenizer.decode(ids, skip_special_tokens=True)


def print_report(report):
    p = report["profile"]
    print(f"Status: {report['status_text']}; cache={report['cache_length']}; PC roundtrip={report['network_roundtrip_s']:.3f}s")
    if report["status"]:
        print(f"Failure: M{report['failed_module']:02}, layer={report['failed_layer']}")
    if not p:
        return
    print(f"Device total={p['total_us']/1e6:.3f}s, TTFT={p['ttft_us']/1e6:.3f}s, decode={p['decode_us']/1e6:.3f}s")
    print(f"Prefill tokens/s={p['prefill_tokens_per_s']}, decode tokens/s={p['decode_tokens_per_s']}")
    print("Phase    Module  Calls    PS total ms    HW total ms    AXI read MiB    AXI write MiB")
    groups = {}
    for s in p["stats"]:
        if s["kind"] != 0:
            continue
        key = (s["phase_name"], s["module_name"])
        a = groups.setdefault(key, [0, 0, 0, 0, 0, 0])
        for i, value in enumerate([s["calls"], s["total_us"], s["hw_total_us"] or 0,
                                    s["read_bytes"] or 0, s["write_bytes"] or 0, s["capabilities"] & 1]):
            a[i] += value
    for key, v in sorted(groups.items()):
        hw = f"{v[2]/1000:14.3f}" if v[5] else "           N/A"
        print(f"{key[0]:8} {key[1]:6} {v[0]:6} {v[1]/1000:14.3f} {hw} {v[3]/2**20:15.3f} {v[4]/2**20:16.3f}")
    transfers = {}
    for s in p["stats"]:
        if s["kind"] == 0:
            continue
        key = (s["phase_name"], s["kind_name"], s["role_name"] if s["kind"] in (1, 4) else "all")
        a = transfers.setdefault(key, [0, 0, 0])
        for i, value in enumerate([s["calls"], s["total_us"], s["bytes"]]):
            a[i] += value
    for key, (calls, us, nbytes) in sorted(transfers.items()):
        print(f"{'/'.join(key)}: calls={calls}, {us/1000:.3f} ms, {nbytes/2**20:.3f} MiB, {nbytes/us if us else 0:.2f} MB/s")
    print("M06/M09 AXI amounts are logical. Pure compute/internal DDR time: N/A for this image.")
    if p["trace_dropped"] or p["stats_dropped"]:
        print(f"Dropped: trace={p['trace_dropped']} stats={p['stats_dropped']} (aggregate stats remain available when trace fills)")
    if report["status"] or p["last"]["fault_latched"]:
        print("Registers:", json.dumps(p["last"], ensure_ascii=False))


def save_report(report, prefix):
    prefix = Path(prefix)
    prefix.parent.mkdir(parents=True, exist_ok=True)
    Path(str(prefix) + ".json").write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
    if report["profile"]:
        for name in ("stats", "trace"):
            rows = report["profile"][name]
            with Path(str(prefix) + f".{name}.csv").open("w", encoding="utf-8-sig", newline="") as f:
                if rows:
                    writer = csv.DictWriter(f, fieldnames=list(rows[0]))
                    writer.writeheader()
                    writer.writerows(rows)
        with Path(str(prefix) + ".tokens.csv").open("w", encoding="utf-8-sig", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["output_index", "token_id", "device_latency_us", "stage"])
            for i, (token, us) in enumerate(zip(report["generated_ids"], report["profile"]["token_latencies_us"])):
                writer.writerow([i, token, us, "first_token" if not i else "decode"])


def parser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--host", required=True)
    p.add_argument("--port", type=int, default=5000)
    p.add_argument("--tokenizer", default=r"D:\Qwen2.5-0.5B-Instruct")
    p.add_argument("--prompt")
    p.add_argument("--prompt-file", type=Path)
    p.add_argument("--tokens-file", type=Path, help="JSON integer array, bypass PC tokenizer")
    p.add_argument("--system", default="You are Qwen, created by Alibaba Cloud. You are a helpful assistant.")
    p.add_argument("--no-chat-template", action="store_true")
    p.add_argument("--interactive", action="store_true", help="chat history stays on PC; /reset and /quit")
    p.add_argument("--max-new-tokens", type=int, default=32)
    p.add_argument("--repetition-penalty", type=float, default=1.0)
    p.add_argument("--no-eos", action="store_true", help="fixed-length performance run, ignores EOS")
    p.add_argument("--prefill-rows", type=int, choices=range(1, 65), default=64, metavar="1..64")
    p.add_argument("--trace", action="store_true")
    p.add_argument("--trace-limit", type=int, default=2048)
    p.add_argument("--crc", action="store_true", help="changes performance; use for diagnosis")
    p.add_argument("--uart-progress", action="store_true", help="verbose UART can change performance")
    p.add_argument("--profile-only", action="store_true", help="read last report and boot load profile")
    p.add_argument("--dma-bench", action="store_true")
    p.add_argument("--dma-bytes", type=int, default=1048576)
    p.add_argument("--dma-iterations", type=int, default=10)
    p.add_argument("--repeat", type=int, default=1)
    p.add_argument("--warmup", type=int, default=0)
    p.add_argument("--timeout", type=float, default=3600)
    p.add_argument("--out", default="results/qwen")
    return p


def main(argv=None):
    args = parser().parse_args(argv)
    if not 0 <= args.trace_limit <= 2048 or args.repeat < 1 or args.warmup < 0:
        raise ValueError("invalid trace limit/repetition counts")
    if not 1.0 <= args.repetition_penalty <= 16.0:
        raise ValueError("repetition penalty must be 1..16")
    command = 2 if args.profile_only else 3 if args.dma_bench else 1
    if args.dma_bench and (not 64 <= args.dma_bytes <= 1048576 or args.dma_bytes % 64 or not 1 <= args.dma_iterations <= 1000):
        raise ValueError("DMA: bytes must be 64-byte aligned, 64..1048576; iterations 1..1000")
    codec = TextCodec(args.tokenizer) if command == 1 and not args.tokens_file else None
    history = [{"role": "system", "content": args.system}] if args.system else []
    if args.prompt_file:
        args.prompt = args.prompt_file.read_text(encoding="utf-8-sig")
    if command == 1 and not args.interactive and args.prompt is None and not args.tokens_file:
        raise ValueError("provide --prompt, --prompt-file, --tokens-file or --interactive")
    times = []
    first_ids = None
    with socket.create_connection((args.host, args.port), timeout=args.timeout) as sock:
        sock.settimeout(args.timeout)
        count = 0
        while True:
            if args.interactive:
                prompt = input("You> ")
                if prompt == "/quit":
                    break
                if prompt == "/reset":
                    history = history[:1] if args.system else []
                    continue
                messages = history + [{"role": "user", "content": prompt}]
            else:
                messages = history + [{"role": "user", "content": args.prompt or ""}]
            begin = time.perf_counter()
            text = None
            tokens = []
            if command == 1:
                if args.tokens_file:
                    tokens = json.loads(args.tokens_file.read_text(encoding="utf-8-sig"))
                else:
                    tokens, text = codec.encode(messages, not args.no_chat_template)
                if not tokens or any(type(t) is not int or not 0 <= t < 151936 for t in tokens):
                    raise ValueError("invalid prompt token IDs")
                if not 1 <= args.max_new_tokens <= 2048 - len(tokens):
                    raise ValueError(f"prompt {len(tokens)} + max_new_tokens must be <=2048; use /reset for long chats")
            tokenize_s = time.perf_counter() - begin
            count += 1
            report = exchange(sock, tokens, args, count, command)
            report.update(prompt_ids=tokens, formatted_prompt=text, tokenizer_s=tokenize_s)
            report["answer"] = codec.decode(report["generated_ids"]) if codec else None
            if command == 1 and not args.interactive and not report["status"]:
                if first_ids is None:
                    first_ids = report["generated_ids"]
                report["matches_first_run"] = report["generated_ids"] == first_ids
            print("Assistant>", report["answer"] if codec else report["generated_ids"])
            print_report(report)
            save_report(report, f"{args.out}.{count:03d}")
            if report["status"]:
                return 1
            if report.get("matches_first_run") is False:
                print("Repeated identical prompt produced different token IDs; inspect CRC/trace and hardware timing.", file=sys.stderr)
                return 1
            if args.interactive:
                history = messages + [{"role": "assistant", "content": report["answer"] or ""}]
            elif count > args.warmup and report["profile"]:
                times.append(report["profile"]["total_us"] / 1e6)
            if not args.interactive and count >= args.repeat + args.warmup:
                break
    if times:
        print(f"Measured {len(times)} runs after {args.warmup} warmup: mean={statistics.mean(times):.6f}s, median={statistics.median(times):.6f}s")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as error:
        print(f"Error: {error}", file=sys.stderr)
        raise SystemExit(2)
