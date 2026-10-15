# Board test results — 2026-10-15

These results were provided from the current AXU9EG board run. They validate the packaged four-row M13 bitstream, current PS application, SD weight files, network path, DMA path, and short inference prompts. They do not establish maximum-context throughput or a performance guarantee for other prompt lengths.

## Network and software preflight

- Board address: 192.168.8.190, TCP port 5000; the PHY patch uses the Marvell PHY setup.
- Offline BSP/ELF preflight: passed. The copied ELF defines main, qwen_debug_state, and qwen_phy_debug. BSP/export libraries matched; lwIP buffer settings and FILE_SYSTEM_USE_LFN=2 matched the tested build.
- The full inference client received successful responses over Ethernet. The test did not use UART.

## SD model files and loading

The host asset verifier checked 196 of 196 files, 988,589,824 bytes, with names, sizes, and SHA-256 matching the host manifest. Only the named weight files are needed on SD for this board workflow; the manifest stays on the PC for validation, and BOOT.BIN is not required on SD because Vitis/JTAG loads the bitstream and ELF.

Profile-only model loading completed successfully:

| Stage | Calls | Time | Bytes recorded | Rate |
|---|---:|---:|---:|---:|
| SD read | 1,079 | 131.231 s | 942.793 MiB | 7.53 MiB/s |
| PS-to-PL CDMA | 1,080 | 645.385 ms | 942.793 MiB | 1,531.78 MB/s |
| Cache flush | 1,081 | 58.911 ms | 942.793 MiB | 16,781.08 MB/s |
| Total device load | — | 133.855 s | — | — |

The application keeps the model weights in PL DDR after loading. CDMA moves the file contents from PS-side memory into the PL DDR window; the SD card is only the source for the initial load.

## CDMA tests

- 65,536 bytes × 3 iterations: Status OK.
- 1,048,576 bytes × 10 iterations: Status OK; the run measured 10 MiB in each direction. PS-to-PL took 6.840 ms total and PL-to-PS took 6.587 ms total, approximately 1.53 and 1.59 GB/s as printed by the client.

## End-to-end inference

| Prompt | Generated answer | Prefill | Decode | Rate |
|---|---|---:|---:|---:|
| 你好, max-new-tokens=1 | 你好 | TTFT 4.750 s | no separate decode token | one-token smoke test passed |
| 你好，请用一句话介绍自己。, max-new-tokens=32 | 你好，我是来自阿里云的Qwen，一款基于超大规模语言模型的AI助手。 | TTFT 5.409 s | 15.079 s for 20 generated tokens | 1.326 tokens/s decode |
| 请只回答：1加1等于几？, max-new-tokens=16 | 1加1等于2。 | TTFT 5.990 s | 4.513 s for 6 generated tokens | 1.330 tokens/s decode |

The max-new-tokens setting is an upper bound. The first full-answer run stopped after 20 generated tokens and the math run stopped after 6. In these tests, decode is slower than prefill because each generated token runs the complete 24-layer model sequentially and rereads large weight matrices from PL DDR; this is especially visible in M13 SwiGLU and M14 LM Head.

## Per-module profile from the 20-token answer

Times and AXI amounts are totals over the complete prefill or decode phase. AXI reads/writes are in MiB. M06 and M09 report logical AXI amounts and have no hardware-cycle timing field in this firmware profile.

| Module | Prefill HW ms | Prefill read / write MiB | Decode HW ms | Decode read / write MiB |
|---|---:|---:|---:|---:|
| M05 | 0.079 | 0.062 / 0.062 | 0.071 | 0.034 / 0.034 |
| M06 | N/A | 3.039 / 2.955 | N/A | 3.350 / 1.675 |
| M07 | 284.287 | 426.779 / 1.898 | 629.212 | 946.875 / 1.055 |
| M08 | 24.125 | 1.898 / 1.688 | 13.420 | 1.055 / 0.938 |
| M09 | N/A | 0.422 / 0.422 | N/A | 0.234 / 0.234 |
| M10 | 338.685 | 56.109 / 1.477 | 447.241 | 77.109 / 0.820 |
| M11 | 219.079 | 332.227 / 1.477 | 483.911 | 735.820 / 0.820 |
| M12 | 3.944 | 5.906 / 2.953 | 3.612 | 3.281 / 1.641 |
| M13 | 4,173.686 | 5,387.977 / 1.477 | 8,866.226 | 11,970.820 / 0.820 |
| M14 | 224.869 | 259.658 / 0.000 | 4,497.377 | 5,193.159 / 0.000 |

The M13 decode time is about 8.87 s for 20 tokens; M14 is about 4.50 s. These are the largest measured compute intervals in this run. The debug counters separate module execution from PS loading and CDMA movement, but they do not isolate pure arithmetic from DDR wait time.

## Interpretation limits

These are single-run measurements from the logs shown, not averaged benchmark campaigns. The board test proves that this image can load the provided model assets, complete the tested prompts, and report module-level timings. It does not prove maximum sequence length, stress stability, thermal limits, accuracy across a broad prompt set, or that a future rebuild will produce an identical bitstream. Re-run the host asset verifier, release checksum verifier, and board smoke test after changing any source, BSP, XSA, or bitstream.
