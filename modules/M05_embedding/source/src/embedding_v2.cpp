#include "include/embedding_v2.h"
#include <hls_stream.h>

namespace {

// FIFO 1: bit 32 = terminal; bits 31:0 = token ID or terminal status.
// FIFO 2: a 128-bit row header followed by exactly 112 raw data words.
// At a header position only, bit 32 = terminal and bits 31:0 = status.
// Payload words are NEVER inspected for control bits: every BF16 pattern is valid.
using token_task_t = ap_uint<33>;
using m05_v2::axi_word_t;

// Stage A. Emit only the valid prefix, followed by exactly one terminal task.
// No downstream process needs to guess whether another token will arrive.
void read_and_check_ids(
    const uint32_t *token_ids,
    uint32_t valid_rows,
    uint32_t vocab_size,
    hls::stream<token_task_t> &id_fifo)
{
#pragma HLS INLINE off
    uint32_t status = m05_v2::kStatusOk;

CHECK_IDS:
    for (uint32_t t = 0; t < valid_rows; ++t) {
        const uint32_t id = token_ids[t];
        if (id >= vocab_size) {
            status = m05_v2::kStatusBadToken;
            break;
        }
        token_task_t task = 0;
        task.range(31, 0) = id;
        id_fifo.write(task);
    }

    token_task_t terminal = 0;
    terminal[32] = 1;
    terminal.range(31, 0) = status;
    id_fifo.write(terminal);
}

// Stage B. Each valid task produces one row header and 112 payload words.
// A terminal task produces just one terminal header, with no table access.
void read_embedding_rows(
    const axi_word_t *table,
    hls::stream<token_task_t> &id_fifo,
    hls::stream<axi_word_t> &data_fifo)
{
#pragma HLS INLINE off

READ_TASKS:
    while (true) {
        const token_task_t task = id_fifo.read();
        const bool terminal = task[32] != 0;

        axi_word_t header = 0;
        header[32] = terminal;
        if (terminal) {
            header.range(31, 0) = task.range(31, 0);
        }
        data_fifo.write(header);
        if (terminal) {
            break;
        }

        const uint32_t id = task.range(31, 0).to_uint();
        const uint32_t src_word = id * m05_v2::kWordsPerRow;
READ_ROW:
        for (uint32_t w = 0; w < m05_v2::kWordsPerRow; ++w) {
#pragma HLS PIPELINE II=1
            const axi_word_t value = table[src_word + w];
            data_fifo.write(value);
        }
    }
}

// Stage C. Only payload words reach DDR; row/terminal headers stay internal.
// Count a row after all 112 writes have been issued. The top function completes
// only after the DATAFLOW region and the generated AXI completion logic finish.
void write_embedding_rows(
    hls::stream<axi_word_t> &data_fifo,
    axi_word_t *hidden_out,
    uint32_t &status_code,
    uint32_t &processed_rows)
{
#pragma HLS INLINE off
    uint32_t rows = 0;
    uint32_t status = m05_v2::kStatusOk;

WRITE_TASKS:
    while (true) {
        const axi_word_t header = data_fifo.read();
        if (header[32] != 0) {
            status = header.range(31, 0).to_uint();
            break;
        }

        const uint32_t dst_word = rows * m05_v2::kWordsPerRow;
WRITE_ROW:
        for (uint32_t w = 0; w < m05_v2::kWordsPerRow; ++w) {
#pragma HLS PIPELINE II=1
            const axi_word_t value = data_fifo.read();
            hidden_out[dst_word + w] = value;
        }
        ++rows;
    }

    status_code = status;
    processed_rows = rows;
}

// Keep the DATAFLOW region canonical: channel declarations and three
// unconditional process calls. Job validation stays outside this region.
// Local, fully drained streams carry no data into the next invocation.
void run_embedding_dataflow(
    const uint32_t *token_ids,
    const axi_word_t *table,
    axi_word_t *hidden_out,
    uint32_t valid_rows,
    uint32_t vocab_size,
    uint32_t &status_code,
    uint32_t &processed_rows)
{
#pragma HLS INLINE off
    hls::stream<token_task_t> id_fifo("id_fifo");
    hls::stream<axi_word_t> data_fifo("data_fifo");
#pragma HLS STREAM variable=id_fifo depth=64
#pragma HLS STREAM variable=data_fifo depth=64
#pragma HLS DATAFLOW

    read_and_check_ids(token_ids, valid_rows, vocab_size, id_fifo);
    read_embedding_rows(table, id_fifo, data_fifo);
    write_embedding_rows(data_fifo, hidden_out, status_code, processed_rows);
}

} // namespace

// Same public ABI and interface directives as the validated pre-DATAFLOW version.
// The build-identification revision requires fresh synthesis/co-simulation.
void embedding_v2(
    const uint32_t *token_ids,
    const m05_v2::axi_word_t *table,
    m05_v2::axi_word_t *hidden_out,
    uint32_t run_mode,
    uint32_t valid_rows,
    uint32_t hidden_size,
    uint32_t vocab_size,
    uint32_t abi_version,
    uint32_t job_id,
    uint32_t flags,
    common_result_v2 *result)
{
    // DDR payload interfaces. Depth counts pointer elements, not bytes.
    // Vivado may rename out/table to out_r/table_r.
#pragma HLS INTERFACE m_axi port=token_ids offset=slave bundle=ids depth=64 max_read_burst_length=16 num_read_outstanding=1
#pragma HLS INTERFACE m_axi port=table offset=slave bundle=weight depth=17016832 max_read_burst_length=64 num_read_outstanding=1
#pragma HLS INTERFACE m_axi port=hidden_out offset=slave bundle=out depth=7168 max_write_burst_length=64 num_write_outstanding=1

    // Keep the build profile consistent with the literal burst pragmas above.
    // If these limits change, update both the pragmas and the profile together.
    static_assert(m05_v2::kBuildMainTile == 64,
                  "Build profile must match the table/output burst pragmas");

    // Base addresses and scalar parameters are programmed through AXI-Lite.
#pragma HLS INTERFACE s_axilite port=token_ids bundle=ctrl
#pragma HLS INTERFACE s_axilite port=table bundle=ctrl
#pragma HLS INTERFACE s_axilite port=hidden_out bundle=ctrl
#pragma HLS INTERFACE s_axilite port=run_mode bundle=ctrl
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=ctrl
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=ctrl
#pragma HLS INTERFACE s_axilite port=vocab_size bundle=ctrl
#pragma HLS INTERFACE s_axilite port=abi_version bundle=ctrl
#pragma HLS INTERFACE s_axilite port=job_id bundle=ctrl
#pragma HLS INTERFACE s_axilite port=flags bundle=ctrl

    // Result fields are registers, not a DDR result buffer.
#pragma HLS DISAGGREGATE variable=result
#pragma HLS INTERFACE s_axilite port=result->status_code bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->processed_rows bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->perf_cycles_total bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->perf_cycles_compute bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->perf_read_beats bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->perf_write_beats bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->output_crc bundle=ctrl
#pragma HLS INTERFACE s_axilite port=result->build_config bundle=ctrl
#pragma HLS INTERFACE ap_ctrl_hs port=return
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

    // Keep the solution's 64-bit AXI address / 128-bit max data configuration.
    using namespace m05_v2;

#ifndef __SYNTHESIS__
    if (result == nullptr) {
        return;
    }
#endif

    result->status_code = kStatusOk;
    result->processed_rows = 0;
    // Performance remains unimplemented: these zeros are not measurements.
    result->perf_cycles_total = 0;
    result->perf_cycles_compute = 0;
    result->perf_read_beats = 0;
    result->perf_write_beats = 0;
    result->output_crc = 0;
    // Report the same compiled profile on success and all returned errors.
    // This result register is valid after completion; it does not yet provide
    // a reset-time identification register for M02 startup probing.
    result->build_config = kBuildConfig;
    (void)job_id;

    // Reject invalid jobs BEFORE launching any DATAFLOW process.
    if (abi_version != kAbiVersion) {
        result->status_code = kStatusBadAbi;
        return;
    }
    if (run_mode != kPrefill && run_mode != kDecode) {
        result->status_code = kStatusBadDimension;
        return;
    }
    if (valid_rows == 0 || valid_rows > kMaxRows ||
        (run_mode == kDecode && valid_rows != 1) ||
        hidden_size != kHiddenSize || vocab_size != kVocabSize) {
        result->status_code = kStatusBadDimension;
        return;
    }
    if (flags != 0) {
        result->status_code = kStatusBadFlags;
        return;
    }

    // Host-only protection. PS/M03 must validate physical ranges, alignment,
    // capacity and non-aliasing before starting the hardware.
#ifndef __SYNTHESIS__
    if (token_ids == nullptr || table == nullptr || hidden_out == nullptr) {
        result->status_code = kStatusBadAddressRange;
        return;
    }
#endif

    uint32_t completed_status;
    uint32_t completed_rows;
    run_embedding_dataflow(token_ids, table, hidden_out, valid_rows, vocab_size,
                           completed_status, completed_rows);
    result->status_code = completed_status;
    result->processed_rows = completed_rows;
    // On any error, the caller must discard this job's entire output.
}
