#include "swiglu_v2.h"

#include "common_numeric_v2.h"
#include "silu_lut.h"

namespace {

// CRC, measured performance and strict PS validation match M05/M07/M11/M12.
// Snapshot and reserved flag bits fail before any data access.
static const uint32_t SUPPORTED_FLAGS_MASK = 0xBu;
static const int OUTPUT_GROUP = 32;
static const int BEATS_PER_OUTPUT_GROUP =
    OUTPUT_GROUP / SWIGLU_V2_BF16_PER_BEAT;
static const int HIDDEN_BEATS =
    SWIGLU_V2_HIDDEN_SIZE / SWIGLU_V2_BF16_PER_BEAT;
static const int MID_TILES =
    SWIGLU_V2_INTERMEDIATE_SIZE / SWIGLU_V2_MID_TILE;

// The Vitis HLS 2021.1 FP32 feedback path is five cycles.  These five
// independent partial sums restore the recurrence-hiding schedule used by
// the original II=1 kernels.
static const int ACC_LANES = 5;
static const uint64_t PROJECTION_ISSUE_CYCLES_PER_TILE =
    (uint64_t)(SWIGLU_V2_MID_TILE / OUTPUT_GROUP) *
    (uint64_t)SWIGLU_V2_HIDDEN_SIZE *
    (uint64_t)BEATS_PER_OUTPUT_GROUP;
static const uint64_t DOWN_ISSUE_CYCLES_PER_TILE =
    (uint64_t)(SWIGLU_V2_HIDDEN_SIZE / OUTPUT_GROUP) *
    (uint64_t)SWIGLU_V2_MID_TILE *
    (uint64_t)BEATS_PER_OUTPUT_GROUP;

// ABI PAR=8, ROW_TILE=4, packed partials, one intermediate tile, LUT SiLU=1, revision=4.
static const uint32_t BUILD_CONFIG =
    (uint32_t)COMMON_PAR_CODE_8 |
    ((uint32_t)SWIGLU_V2_ROW_TILE << 4) |
    (64u << 10) |
    (1u << 18) |
    ((uint32_t)COMMON_CLOCK_CODE_100_MHZ << 22) |
    ((uint32_t)SWIGLU_V2_MODULE_REVISION << 28);

static uint16_t get_bf16_lane(const swiglu_axi128_t &word, int lane) {
#pragma HLS INLINE
    return (uint16_t)(word >> (lane * 16));
}

static void set_bf16_lane(
    swiglu_axi128_t &word,
    int lane,
    uint16_t value) {
#pragma HLS INLINE
    word |= swiglu_axi128_t(value) << (lane * 16);
}

static float swiglu_silu(float value) {
#pragma HLS INLINE
    if (common_numeric_v2::fp32_is_nan(value)) {
        return common_numeric_v2::canonical_nan_fp32(
            common_numeric_v2::fp32_signbit(value));
    }
    if (common_numeric_v2::fp32_is_inf(value)) {
        return common_numeric_v2::fp32_signbit(value)
                   ? common_numeric_v2::signed_zero_fp32(true)
                   : value;
    }
    if (value <= SILU_LUT_MIN) return 0.0f;
    if (value >= SILU_LUT_MAX) return value;

    float pos = (value - SILU_LUT_MIN) * SILU_INV_STEP;
    int index = (int)pos;
    float fraction = pos - (float)index;
    float a = SILU_LUT[index];
    float b = SILU_LUT[index + 1];
    return a + fraction * (b - a);
}

static uint32_t validate_job(
    uint32_t run_mode,
    uint32_t valid_rows,
    uint32_t hidden_size,
    uint32_t intermediate_size,
    uint32_t layout_id,
    uint32_t abi_version,
    uint32_t flags) {
#pragma HLS INLINE
    if (abi_version != SWIGLU_V2_ABI_VERSION) {
        return SWIGLU_V2_BAD_ABI;
    }
    if (layout_id != SWIGLU_V2_LAYOUT_W_OG32_I_L32) {
        return SWIGLU_V2_BAD_LAYOUT;
    }
    if (hidden_size != SWIGLU_V2_HIDDEN_SIZE ||
        intermediate_size != SWIGLU_V2_INTERMEDIATE_SIZE) {
        return SWIGLU_V2_BAD_DIMENSION;
    }
    if (run_mode > SWIGLU_V2_DECODE || valid_rows == 0u ||
        valid_rows > SWIGLU_V2_MAX_ROWS ||
        (run_mode == SWIGLU_V2_DECODE && valid_rows != 1u)) {
        return SWIGLU_V2_BAD_DIMENSION;
    }
    if ((flags & ~SUPPORTED_FLAGS_MASK) != 0u) {
        return SWIGLU_V2_BAD_FLAGS;
    }
    return SWIGLU_V2_OK;
}

}  // namespace

void swiglu_v2(
    const swiglu_axi128_t *x,
    const swiglu_axi128_t *w_gate,
    const swiglu_axi128_t *w_up,
    const swiglu_axi128_t *w_down,
    swiglu_axi128_t *y,
    uint32_t run_mode,
    uint32_t valid_rows,
    uint32_t hidden_size,
    uint32_t intermediate_size,
    uint32_t layout_id,
    uint32_t abi_version,
    uint32_t job_id,
    uint32_t flags,
    uint32_t &result_status_code,
    uint32_t &result_processed_rows,
    uint64_t &result_perf_cycles_total,
    uint64_t &result_perf_cycles_compute,
    uint64_t &result_perf_read_beats,
    uint64_t &result_perf_write_beats,
    uint32_t &result_output_crc,
    uint32_t &result_build_config,
    uint32_t &result_mid_tiles,
    uint64_t &result_gate_up_active_cycles,
    uint64_t &result_down_active_cycles,
    uint64_t &result_weight_wait_cycles,
    uint64_t &result_fifo_stall_cycles) {

#pragma HLS INTERFACE m_axi port=x offset=slave bundle=x depth=7168 max_read_burst_length=256 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=w_gate offset=slave bundle=w_gate depth=544768 max_read_burst_length=256 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=w_up offset=slave bundle=w_up depth=544768 max_read_burst_length=256 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=w_down offset=slave bundle=w_down depth=544768 max_read_burst_length=256 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=y offset=slave bundle=y depth=7168 max_write_burst_length=256 num_write_outstanding=4

#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=w_gate bundle=control
#pragma HLS INTERFACE s_axilite port=w_up bundle=control
#pragma HLS INTERFACE s_axilite port=w_down bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=intermediate_size bundle=control
#pragma HLS INTERFACE s_axilite port=layout_id bundle=control
#pragma HLS INTERFACE s_axilite port=abi_version bundle=control
#pragma HLS INTERFACE s_axilite port=job_id bundle=control
#pragma HLS INTERFACE s_axilite port=flags bundle=control
#pragma HLS INTERFACE s_axilite port=result_status_code bundle=control
#pragma HLS INTERFACE s_axilite port=result_processed_rows bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_cycles_total bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_cycles_compute bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_read_beats bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_write_beats bundle=control
#pragma HLS INTERFACE s_axilite port=result_output_crc bundle=control
#pragma HLS INTERFACE s_axilite port=result_build_config bundle=control
#pragma HLS INTERFACE s_axilite port=result_mid_tiles bundle=control
#pragma HLS INTERFACE s_axilite port=result_gate_up_active_cycles bundle=control
#pragma HLS INTERFACE s_axilite port=result_down_active_cycles bundle=control
#pragma HLS INTERFACE s_axilite port=result_weight_wait_cycles bundle=control
#pragma HLS INTERFACE s_axilite port=result_fifo_stall_cycles bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    (void)job_id;

    result_status_code = validate_job(
        run_mode, valid_rows, hidden_size, intermediate_size,
        layout_id, abi_version, flags);
    result_processed_rows = 0;
    result_perf_cycles_total = 0;
    result_perf_cycles_compute = 0;
    result_perf_read_beats = 0;
    result_perf_write_beats = 0;
    result_output_crc = 0;
    result_build_config = BUILD_CONFIG;
    result_mid_tiles = 0;
    result_gate_up_active_cycles = 0;
    result_down_active_cycles = 0;
    result_weight_wait_cycles = 0;
    result_fifo_stall_cycles = 0;

    if (result_status_code != SWIGLU_V2_OK) return;

    float x_tile[SWIGLU_V2_ROW_TILE][SWIGLU_V2_HIDDEN_SIZE];
    // Five partial sums remain independent; their slot is a BRAM address,
    // rather than five tiny RAM banks with separate control/multiplexers.
    float gate_acc[SWIGLU_V2_ROW_TILE][SWIGLU_V2_MID_TILE][ACC_LANES];
    float up_acc[SWIGLU_V2_ROW_TILE][SWIGLU_V2_MID_TILE][ACC_LANES];
    float out_acc[SWIGLU_V2_ROW_TILE][SWIGLU_V2_HIDDEN_SIZE][ACC_LANES];
    float mid_full[SWIGLU_V2_ROW_TILE][SWIGLU_V2_MID_TILE];
#pragma HLS ARRAY_PARTITION variable=x_tile complete dim=1
#pragma HLS ARRAY_PARTITION variable=x_tile cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=gate_acc complete dim=1
#pragma HLS ARRAY_PARTITION variable=gate_acc cyclic factor=8 dim=2
#pragma HLS BIND_STORAGE variable=gate_acc type=ram_s2p impl=bram
#pragma HLS ARRAY_PARTITION variable=up_acc complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_acc cyclic factor=8 dim=2
#pragma HLS BIND_STORAGE variable=up_acc type=ram_s2p impl=bram
#pragma HLS ARRAY_PARTITION variable=out_acc complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_acc cyclic factor=8 dim=2
#pragma HLS BIND_STORAGE variable=out_acc type=ram_s2p impl=bram
#pragma HLS ARRAY_PARTITION variable=mid_full complete dim=1
#pragma HLS BIND_STORAGE variable=mid_full type=ram_s2p impl=bram

    // Decode explicitly selects a one-row schedule. Prefill restores the
    // weight-sharing schedule uses four rows in parallel;
    // inactive row lanes in the final block are gated.
    const int schedule_row_tile =
        run_mode == SWIGLU_V2_DECODE ? 1 : SWIGLU_V2_ROW_TILE;
    const int row_blocks =
        ((int)valid_rows + schedule_row_tile - 1) / schedule_row_tile;

ROW_BLOCK:
    for (int row_block = 0; row_block < row_blocks; ++row_block) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=16 avg=8
        const int row_base = row_block * schedule_row_tile;
        int active_rows = (int)valid_rows - row_base;
        if (active_rows > schedule_row_tile) {
            active_rows = schedule_row_tile;
        }

    LOAD_X:
        for (int row = 0; row < active_rows; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=4 avg=4
            for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
#pragma HLS PIPELINE II=1
                swiglu_axi128_t word =
                    x[(row_base + row) * HIDDEN_BEATS + beat];
                for (int lane = 0; lane < SWIGLU_V2_BF16_PER_BEAT; ++lane) {
#pragma HLS UNROLL
                    x_tile[row][beat * SWIGLU_V2_BF16_PER_BEAT + lane] =
                        common_numeric_v2::bf16_to_fp32(
                            get_bf16_lane(word, lane));
                }
            }
        }

    CLEAR_OUT:
        for (int row = 0; row < active_rows; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=4 avg=4
            for (int b=0;b<HIDDEN_BEATS*ACC_LANES;++b) {
#pragma HLS PIPELINE II=1
                for(int lane=0;lane<8;++lane) {
#pragma HLS UNROLL
                    out_acc[row][(b/ACC_LANES)*8+lane][b%ACC_LANES]=0.0f;
                }
            }
        }

    GATE_UP_ALL_TILES:
        for (int mid_tile_index = 0; mid_tile_index < MID_TILES;
             ++mid_tile_index) {
            const int mid_base = mid_tile_index * SWIGLU_V2_MID_TILE;

        CLEAR_GATE_UP:
            for (int row = 0; row < active_rows; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=4 avg=4
                for (int b=0;b<(SWIGLU_V2_MID_TILE/8)*ACC_LANES;++b) {
#pragma HLS PIPELINE II=1
                    for(int lane=0;lane<8;++lane) {
#pragma HLS UNROLL
                        const int m=(b/ACC_LANES)*8+lane,slot=b%ACC_LANES;
                        gate_acc[row][m][slot]=0.0f;
                        up_acc[row][m][slot]=0.0f;
                    }
                }
            }

        GATE_UP_GROUP:
            for (int mid_group = 0;
                 mid_group < SWIGLU_V2_MID_TILE / OUTPUT_GROUP;
                 ++mid_group) {
            GATE_UP_INPUT:
                for (int input = 0; input < SWIGLU_V2_HIDDEN_SIZE; ++input) {
                GATE_UP_BEAT:
                    for (int group_beat = 0;
                         group_beat < BEATS_PER_OUTPUT_GROUP;
                         ++group_beat) {
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=gate_acc inter RAW distance=20 true
#pragma HLS DEPENDENCE variable=up_acc inter RAW distance=20 true
                        const int output_group =
                            mid_tile_index *
                                (SWIGLU_V2_MID_TILE / OUTPUT_GROUP) +
                            mid_group;
                        const int word_index =
                            (output_group * SWIGLU_V2_HIDDEN_SIZE + input) *
                                BEATS_PER_OUTPUT_GROUP +
                            group_beat;
                        swiglu_axi128_t gate_word = w_gate[word_index];
                        swiglu_axi128_t up_word = w_up[word_index];
                        const int local_mid_base =
                            mid_group * OUTPUT_GROUP +
                            group_beat * SWIGLU_V2_BF16_PER_BEAT;

                        float input_row[SWIGLU_V2_ROW_TILE];
#pragma HLS ARRAY_PARTITION variable=input_row complete dim=1
                    LOAD_GATE_UP_INPUT:
                        for (int row = 0;
                             row < SWIGLU_V2_ROW_TILE;
                             ++row) {
#pragma HLS UNROLL
                            input_row[row] =
                                row < active_rows ? x_tile[row][input] : 0.0f;
                        }

                    GATE_UP_LANE:
                        for (int lane = 0;
                             lane < SWIGLU_V2_BF16_PER_BEAT;
                             ++lane) {
#pragma HLS UNROLL
                            float gate_weight =
                                common_numeric_v2::bf16_to_fp32(
                                    get_bf16_lane(gate_word, lane));
                            float up_weight =
                                common_numeric_v2::bf16_to_fp32(
                                    get_bf16_lane(up_word, lane));
                            const int local_mid = local_mid_base + lane;

                        GATE_UP_ROW:
                            for (int row = 0;
                                 row < SWIGLU_V2_ROW_TILE;
                                 ++row) {
#pragma HLS UNROLL
                                if (row < active_rows) {
                                    float input_value = input_row[row];
                                    float gate_product =
                                        input_value * gate_weight;
                                    float up_product = input_value * up_weight;
                                    const int slot=input%ACC_LANES;
                                    gate_acc[row][local_mid][slot] += gate_product;
                                    up_acc[row][local_mid][slot] += up_product;
                                }
                            }
                        }
                    }
                }
            }

        MAKE_MID:
            for (int row = 0; row < active_rows; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=4 avg=4
                for (int mid = 0; mid < SWIGLU_V2_MID_TILE; ++mid) {
#pragma HLS PIPELINE II=5
                    float g01 = gate_acc[row][mid][0] + gate_acc[row][mid][1];
                    float g23 = gate_acc[row][mid][2] + gate_acc[row][mid][3];
                    float u01 = up_acc[row][mid][0] + up_acc[row][mid][1];
                    float u23 = up_acc[row][mid][2] + up_acc[row][mid][3];
                    float gate_value = (g01 + g23) + gate_acc[row][mid][4];
                    float up_value = (u01 + u23) + up_acc[row][mid][4];
                    mid_full[row][mid] =
                        swiglu_silu(gate_value) * up_value;
                }
            }


        DOWN_OUTPUT_GROUP:
            for (int output_group = 0;
                 output_group < SWIGLU_V2_HIDDEN_SIZE / OUTPUT_GROUP;
                 ++output_group) {
            DOWN_MID:
                for (int mid = 0; mid < SWIGLU_V2_MID_TILE; ++mid) {
                    const int global_mid = mid_base + mid;
                DOWN_BEAT:
                    for (int group_beat = 0;
                         group_beat < BEATS_PER_OUTPUT_GROUP;
                         ++group_beat) {
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=out_acc inter RAW distance=20 true
                        const int word_index =
                            (output_group * SWIGLU_V2_INTERMEDIATE_SIZE +
                             global_mid) * BEATS_PER_OUTPUT_GROUP + group_beat;
                        swiglu_axi128_t down_word = w_down[word_index];

                        // The eight output lanes use the same activation for a
                        // given row.  Read each row exactly once and broadcast
                        // it from registers; otherwise an unrolled lane loop
                        // asks one BRAM bank for eight identical reads and HLS
                        // raises the loop II because of memory-port pressure.
                        float mid_value[SWIGLU_V2_ROW_TILE];
#pragma HLS ARRAY_PARTITION variable=mid_value complete dim=1

                    READ_MID_ROWS:
                        for (int row = 0;
                             row < SWIGLU_V2_ROW_TILE;
                             ++row) {
#pragma HLS UNROLL
                            mid_value[row] =
                                row < active_rows
                                    ? mid_full[row][mid]
                                    : 0.0f;
                        }

                    DOWN_LANE:
                        for (int lane = 0;
                             lane < SWIGLU_V2_BF16_PER_BEAT;
                             ++lane) {
#pragma HLS UNROLL
                            float down_weight =
                                common_numeric_v2::bf16_to_fp32(
                                    get_bf16_lane(down_word, lane));
                            const int output = output_group * OUTPUT_GROUP +
                                               group_beat *
                                                   SWIGLU_V2_BF16_PER_BEAT +
                                               lane;

                        DOWN_ROW:
                            for (int row = 0;
                                 row < SWIGLU_V2_ROW_TILE;
                                ++row) {
#pragma HLS UNROLL
                                if (row < active_rows) {
                                    float product =
                                        mid_value[row] * down_weight;
                                    out_acc[row][output][global_mid%ACC_LANES] += product;
                                }
                            }
                        }
                    }
                }
            }
        }

    STORE_Y:
        for (int row = 0; row < active_rows; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=4 avg=4
            for (int beat = 0; beat < HIDDEN_BEATS; ++beat) {
#pragma HLS PIPELINE II=5
                swiglu_axi128_t word = 0;
                for (int lane = 0; lane < SWIGLU_V2_BF16_PER_BEAT; ++lane) {
#pragma HLS UNROLL
                    const int output =
                        beat * SWIGLU_V2_BF16_PER_BEAT + lane;
                    float d01 =
                        out_acc[row][output][0] + out_acc[row][output][1];
                    float d23 =
                        out_acc[row][output][2] + out_acc[row][output][3];
                    uint16_t bf16 = common_numeric_v2::fp32_to_bf16_rne(
                        (d01 + d23) + out_acc[row][output][4]);
                    set_bf16_lane(word, lane, bf16);
                }
                y[(row_base + row) * HIDDEN_BEATS + beat] = word;
            }
        }
    }

    result_status_code = SWIGLU_V2_OK;
    result_processed_rows = valid_rows;
    // Actual common counters are injected into the packaged RTL. Unknown
    // stage activity and stall measurements are zero, never schedule estimates.
    result_mid_tiles = (uint32_t)(row_blocks * MID_TILES);
    if (flags & 1U) {
        uint32_t crc = 0xffffffffU;
        for (uint32_t beat = 0; beat < valid_rows * HIDDEN_BEATS; ++beat) {
            swiglu_axi128_t word = y[beat];
            for (unsigned byte = 0; byte < 16; ++byte) {
#pragma HLS PIPELINE II=1
                crc ^= (uint8_t)(word >> (byte * 8));
                for (unsigned bit = 0; bit < 8; ++bit) {
#pragma HLS UNROLL
                    crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
                }
            }
        }
        result_output_crc = ~crc;
    }
}
