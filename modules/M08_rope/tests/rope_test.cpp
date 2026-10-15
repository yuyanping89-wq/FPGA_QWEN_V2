#include <stdio.h>
#include "rope.h"
#include "rope_test_BF16_00_00.h"
#include "rope_test_BF16_07_00.h"

inline float apu16_to_f32(ap_uint<16> value)
{
    bits32 bits;
    bits.u = ((uint32_t)value) << 16;
    return bits.f;
}

void tb_u16_to_apu128(const uint16_t *u16, ap_uint<128> *u128, int N, int M, int D)
{
    int num = N * M * D;
    for (int i = 0; i < num / 8; i++)
    {
        ap_uint<128> tmp = 0;
        for (int j = 0; j < 8; j++)
        {
            tmp.range(j * 16 + 15, j * 16) = (ap_uint<16>)u16[i * 8 + j];
        }
        u128[i] = tmp;
    }
}

float tb_compare(const uint16_t *u16, const ap_uint<128> *u128, int N, int M, int D, const char *name)
{
    int num = N * M * D;
    float *arr1 = (float *)malloc(num * sizeof(float));
    float *arr2 = (float *)malloc(num * sizeof(float));

    for (int i = 0; i < num / 8; i++)
    {
        ap_uint<128> tmp = u128[i];
        for (int j = 0; j < 8; j++)
        {
            ap_uint<16> t1 = tmp.range(j * 16 + 15, j * 16);
            ap_uint<16> t2 = (ap_uint<16>)u16[i * 8 + j];
            arr1[i * 8 + j] = apu16_to_f32(t1);
            arr2[i * 8 + j] = apu16_to_f32(t2);
        }
    }

    printf("========== '%s': (%d,%d,%d)\n", name, N, M, D);
    float sum = 0.0f;
    for (int itr = 0; itr < M; itr++)
    {
        printf(">> Index: %d\n", itr);
        for (int i = 0; i < N; i++)
        {
            for (int id = 0; id < D; id++)
            {
                int off = i * M * D + itr * D + id;
                printf("%.4g/%.4g ", arr1[off], arr2[off]);
                float diff = fabs(arr1[off] - arr2[off]);
                sum += diff;
            }
            printf("\n");
        }
    }
    printf("========== '%s': %f, %f\n", name, sum, sum / num);

    free(arr1);
    free(arr2);
    return sum / num;
}

int main()
{
    float res[4];
    char s[41] = "========================================";
    const int T0 = MAX_BLOCK_ROWS;
    const int Q = NUM_Q_HEADS;
    const int K = NUM_KV_HEADS;
    const int D = HEAD_DIM;
    const int D_u128 = D / 8;

    static ap_uint<128> cos_table[T0 * D_u128];
    static ap_uint<128> sin_table[T0 * D_u128];
    static ap_uint<128> q_raw[Q * T0 * D_u128];
    static ap_uint<128> q_rot[Q * T0 * D_u128];
    static ap_uint<128> k_raw[K * T0 * D_u128];
    static ap_uint<128> k_rot[K * T0 * D_u128];

    {
        common_result_v2 result;
        const int T = 36;
        tb_u16_to_apu128(BF16_00_00_attention_rope_cos, cos_table, 1, T, D);
        tb_u16_to_apu128(BF16_00_00_attention_rope_sin, sin_table, 1, T, D);
        tb_u16_to_apu128(BF16_00_00_attention_q_projection, q_raw, Q, T, D);
        tb_u16_to_apu128(BF16_00_00_attention_k_projection, k_raw, K, T, D);
        rope_v2(
            q_raw, k_raw, cos_table, sin_table, q_rot, k_rot,
            0, T, 0, T, 2, 0, 0, &result);
        printf("\n\n%s\nBF16_00_00:\n", s);
        res[0] = tb_compare(BF16_00_00_attention_q_after_rope, q_rot, Q, T, D, "rope_q_rot");
        res[1] = tb_compare(BF16_00_00_attention_k_after_rope, k_rot, K, T, D, "rope_k_rot");
    }

    {
        common_result_v2 result;
        const int T = 1;
        tb_u16_to_apu128(BF16_07_00_attention_rope_cos, cos_table, 1, T, D);
        tb_u16_to_apu128(BF16_07_00_attention_rope_sin, sin_table, 1, T, D);
        tb_u16_to_apu128(BF16_07_00_attention_q_projection, q_raw, Q, T, D);
        tb_u16_to_apu128(BF16_07_00_attention_k_projection, k_raw, K, T, D);
        rope_v2(
            q_raw, k_raw, cos_table, sin_table, q_rot, k_rot,
            1, T, 0, T, 2, 0, 0, &result);
        printf("\n\n%s\nBF16_07_00:\n", s);
        res[2] = tb_compare(BF16_07_00_attention_q_after_rope, q_rot, Q, T, D, "rope_q_rot");
        res[3] = tb_compare(BF16_07_00_attention_k_after_rope, k_rot, K, T, D, "rope_k_rot");
    }

    printf("\n\n%s\n", s);
    printf("Test completed.\n");
    printf("BF16_00_00: %f, %f\n", res[0], res[1]);
    printf("BF16_07_00: %f, %f\n", res[2], res[3]);

    return 0;
}
