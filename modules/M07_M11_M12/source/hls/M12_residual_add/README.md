
# 11 Residual Add

BF16-storage, fp32-add residual operator.

```text
Top: attention_residual_add
IP: user.org:llm:attention_residual_add:3.0
AXI/wire dtype: BF16 uint16 little-endian
Compute: fp32 add, BF16 RNE output
T=36 CSim and small RTL CoSim: bit-exact PASS
```

The raw port-50053 BF16 implementation is retained as bring-up code only.
Production integration must use the maintained `FPGA_OP_RUN_ADD` dispatch on
the shared `OFQWNRQ1` runtime. Real-board validation is pending for v3.0 BF16.
