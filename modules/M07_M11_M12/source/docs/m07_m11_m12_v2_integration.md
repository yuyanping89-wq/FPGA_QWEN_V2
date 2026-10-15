# M07 / M11 / M12 ABI v2 integration

The architecture document numbers these operators as:

| Architecture ID | Repository implementation | v2 top function |
| --- | --- | --- |
| M07 QKV Projection | `module/04_qkv_projection` | `qkv_projection_v2_hls` |
| M11 Attention Out Projection | `module/10_attention_out_proj` | `attention_out_v2_hls` |
| M12 Residual Add | `module/11_residual_add` | `residual_add_v2_hls` |

The old top functions (`qkv_proj`, `attention_output_projection`, and
`attention_residual_add`) remain unchanged for already exported IP archives.
New integration uses `include/fpga_abi_v2.h` and passes the same
`common_job_v2` fields to every operator. Prefill accepts 1..64 rows; Decode
requires exactly one row. All external tensors are BF16 uint16 little-endian.

M07 takes one packed Q/K/V weight tensor `[1152,896]` in
`W_OG32_I_L32_V2` layout and writes Q/K/V directly in head-major layouts.
M11 consumes packed `Wo` and M12 consumes ordinary `[T,896]` activations.
The software-facing API remains `common_result_v2`. The result fields are
exposed as AXI-Lite output registers (`result_status_code` through
`result_build_config`); no short result structure is written to DDR.

## Board-less verification

```powershell
g++ -std=c++11 -O2 -I Fpga_deploy_qwen25/include `
  Fpga_deploy_qwen25/tests/test_m07_m11_m12_v2.cpp `
  Fpga_deploy_qwen25/module/04_qkv_projection/hls/qkv_kernel.cpp `
  Fpga_deploy_qwen25/module/10_attention_out_proj/hls/attention_output_kernel.cpp `
  Fpga_deploy_qwen25/module/11_residual_add/hls/residual_add_kernel.cpp `
  -o Fpga_deploy_qwen25/tests/test_m07_m11_m12_v2.exe
Fpga_deploy_qwen25/tests/test_m07_m11_m12_v2.exe

$env:PYTHONPATH = 'Fpga_deploy_qwen25'
python Fpga_deploy_qwen25/tools/test_m07_m11_m12_v2.py
```

The first test exercises the synthesizable C++ kernels. The second checks the
same BF16 packing, both run modes, packed weights, and Decode validation using
only NumPy, so it is runnable before a board or Vitis installation exists.

## Vitis HLS and IP export

Run from each module's `hls` directory with the Vitis HLS MinGW directories
before any system MinGW on `PATH`:

```powershell
$h = 'D:\Vitis_HLS\2021.1'
$env:PATH = "$h\bin;$h\tps\win64\msys64\mingw64\bin;$h\tps\win64\msys64\usr\bin;" + $env:PATH
& "$h\bin\vitis_hls.bat" -f run_hls_axu9eg_v2_nocosim_2021.tcl
```

The full-size scripts run CSim, CSynth, and export the IP. The `*_small`
scripts use reduced dimensions for practical workstation XSIM CoSim while
exercising the same ABI, AXI bundles, and result protocol. M12 full-size CoSim
passes; M07 and M11 full-size XSIM are prohibitively slow/noisy with the
Vivado 2021.1 DSP48E1 OPMODE warning, so their reduced smoke results are the
RTL regression gate.

Vivado 2021.1 generates a date-derived core revision that overflows its signed
Tcl integer after 2026. The generated `f/impl/ip/run_ippack.tcl` files pin a
legal revision before rerunning `vivado -mode batch -source run_ippack.tcl`.

The full-size CSynth utilization snapshots are: M07 16 BRAM18K / 24 DSP /
9,495 FF / 13,751 LUT; M11 14 BRAM18K / 7 DSP / 5,956 FF / 7,914 LUT; M12
14 BRAM18K / 2 DSP / 3,658 FF / 5,208 LUT. All use a 10 ns target clock and
the estimator reports 136.99 MHz Fmax. M12's compute loop reaches II=1;
M07's 8-lane weight loop is memory-port limited to II=8 and M11's to II=3.

## Vivado AXI-VIP integration

`vivado/integrate_m07_m11_m12_v2.tcl` creates a Zynq UltraScale+ block design
with AXI-Lite control, a 12-input memory SmartConnect (M07=6, M11=3, M12=3),
and AXI VIP as the DDR model. The script has been validated with Vivado 2021.1
through `validate_bd_design` and exports
`vivado/bd_out7/m07_m11_m12_v2_axi_vip.xsa`. The generated XSA is suitable for
subsequent Vitis/VIP work; final bitstream implementation, DDR calibration, and
PS cache coherency remain board-dependent. `vivado/m07_m11_m12_v2_axi_vip_tb.sv`
is the PS-style register-start smoke bench; SmartConnect maps the three control
slaves at `0x0000_0000`, `0x0001_0000`, and `0x0002_0000`.

M07, M11, and M12 keep the required unified AXI-Lite fields. Their IPs are
repackaged with legal Vivado 2021.1 core revisions (`2026092807`, `2026092808`,
and `2026092809`, respectively). M11 reduced-dimension CoSim and M12 full-size
CoSim are validated with Vivado 2021.1. The ABI-v2 implementation keeps the
required `W_OG32_I_L32_V2` packed layout and all Prefill/Decode fields. A
synthesized RTL pointer-null check was removed from the M07/M11/M12 kernel-only
paths because `result` is a local ABI object inside the top wrapper;
Vivado 2021.1 could otherwise treat that local pointer as address zero and return
`BAD_ADDRESS_RANGE` before computation. Address/range validation remains in the
PS/M02 buffer-descriptor path, while host CSim retains the defensive check.
The reduced CoSim now reports `C/RTL co-simulation finished: PASS`; the repeated
DSP48E1 OPMODE messages are simulator warnings and do not change the result.

## PS/Vitis runtime

`ps_runtime/src/fpga_hls_driver.c` contains the Xilinx standalone AXI-Lite
register path (64-bit buffer addresses, mode/row/layout fields, start/done
polling, timeout and cache maintenance). A host build uses the same validation
and a deterministic mock result, allowing the driver and TCP integration to be
compiled without `xparameters.h` or a board.
