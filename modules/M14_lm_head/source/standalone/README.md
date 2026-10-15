# M14 V2 standalone board-test ELF

For the next large weight upload, use `program_board_tcp.ps1`; see
[TCP_WEIGHT_LOADING.md](TCP_WEIGHT_LOADING.md) for build, IP setup and exact
verification steps. The dedicated TCP loader passed full BF16 upload and six
fault-path board tests on 2026-10-10; the runner defaults to its hash-checked
frozen package `reports/tcp_loader_package_20261010/`. `program_board.ps1` retains the slower JTAG
fallback. The numeric test ELF below still uses JTAG mailboxes.

`run_build.ps1` creates a Cortex-A53 standalone platform from the 2021.1 XSA
and builds `build/m14_v2_test.elf` with the AArch64 command-line compiler. The
direct compiler path avoids the Vitis 2021.1 Eclipse background-build failure
mode. It does not connect to or program the board.

Before starting the ELF, the host must place the BF16 weight, one BF16 hidden
row, the optional history bitmap, and a 256-byte mailbox in DDR. The default
layout is:

| Region | Address | Bytes |
| --- | ---: | ---: |
| weight | `0x40000000` | 272269312 |
| hidden | `0x503A8000` | 1792 |
| history | `0x503A8700` | 18992 |
| logits | `0x503AD140` | 607744 |
| mailbox | `0x51000000` | 256 |

Create the mailbox with `prepare_mailbox.py`. The ELF refuses to start the IP
unless the mailbox magic/version, repeat count, job contract, and 64-byte
alignment checks pass. It then validates the expected token and, when CRC is
enabled, the expected CRC. Results and timing counters are written back to the
same mailbox and flushed from the A53 cache.

`program_board.ps1` verifies the frozen bitstream and BF16 weight hashes,
initializes the PS/DDR, programs the PL, and loads the weight once. `run_step.ps1`
loads one trace, starts the ELF, polls the mailbox, dumps optional logits, and
appends a JSON record. `run_negative.ps1` uses the mailbox V2 bypass flag only
for deliberate BAD_ABI/BAD_FLAGS/BAD_DIMENSION/NUMERIC_ERROR hardware tests.

All board scripts write their raw XSCT output and binary mailbox dumps under
`reports/board_runs/`.

The runners now default to `reports/board_package_20261009/`; pass `-PackageDir`
to select another package. The original 20260930 package is retained. The new
ELF records elapsed ticks before publishing runtime/golden failures, and
`run_step.ps1` saves a per-run decoded mailbox and aggregate record before
reporting any logits hash mismatch. `standalone/scripts/test_result_capture.ps1`
checks this failure path using a mock XSCT and never accesses the board.

The full Decode/penalty-ON acceptance campaign passed on 2026-10-10: 8-step
production/debug exact results, four invalid jobs, 30 independent timing samples
and 1,000 consecutive calls. See `reports/20261010_BOARD_ACCEPTANCE.md` for scope,
the timing distributions and remaining integration gates.

Before reusing an existing setup, run `scripts/probe_retained_state.tcl` with
XSCT. It performs only reads: no reset, PS initialization or download. Follow
the handoff approval requirement before a full program/load if state cannot be
reused. This standalone application has no Ethernet stack; ping is not a
prerequisite for its JTAG mailbox tests.

After a successful program/load, run `scripts/verify_weight_sentinels.tcl` with
the BF16 weight path. It compares both DDR endpoints with the host file without
writing to the board. Then `run_acceptance.ps1` performs 8-step production/debug
accuracy, a controlled wrong-golden-CRC timing regression, four invalid-job tests,
three warmups, 30 independent Decode/penalty-ON production samples and 1,000
consecutive calls. It checkpoints an isolated campaign JSON and stops on failure.
`scripts/summarize_acceptance.py` strictly validates that campaign. APM and other
performance modes are separate from this campaign.

To stop a campaign between completed runs, create
`reports/board_runs/STOP_REQUESTED`. The runner checkpoints state `PAUSED` and
starts no next board operation. Remove that file only when resuming authorized
work. The 2026-10-09 campaign was paused after 8-step accuracy, the controlled
golden failure regression and two negative tests; it is not full acceptance.
