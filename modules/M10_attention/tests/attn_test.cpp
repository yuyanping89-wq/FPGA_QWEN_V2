#include <stdio.h>
#include "attn.h"
#include "attn_test_BF16_00_00.h"
#include "attn_test_BF16_07_00.h"

static float apu16_to_f32(ap_uint<16> value)
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

float test_1()
{
    float res = 0.0f;
    common_result_v2 result;
    const int Q = NUM_Q_HEADS;
    const int K = NUM_KV_HEADS;
    const int D = HEAD_DIM;
    const int D_u128 = D / 8;
    const int T = 36;
    const int CL = 36;
    int layer_id = 0;
    static ap_uint<128> q_rot[Q * T * D_u128];
    static ap_uint<128> k_cache[K * CL * D_u128];
    static ap_uint<128> v_cache[K * CL * D_u128];
    static ap_uint<128> attn_out[T * Q * D_u128];
    tb_u16_to_apu128(BF16_00_00_attention_q_after_rope, q_rot, Q, T, D);
    tb_u16_to_apu128(BF16_00_00_attention_k_cache_after_update, k_cache, K, CL, D);
    tb_u16_to_apu128(BF16_00_00_attention_v_cache_after_update, v_cache, K, CL, D);
    attention_v2(
        q_rot, k_cache, v_cache, attn_out,
        0, layer_id, 0, T, 0, 0.125, 0, 2, 0, 0, &result);
    res = tb_compare(BF16_00_00_attention_output_projection_input, attn_out, T, Q, D, "attn_out");
    return res;
}

float test_2()
{
    float res = 0.0f;
    common_result_v2 result;
    const int Q = NUM_Q_HEADS;
    const int K = NUM_KV_HEADS;
    const int D = HEAD_DIM;
    const int D_u128 = D / 8;
    const int T = 1;
    const int CL = 43;
    int layer_id = 0;
    static ap_uint<128> q_rot[Q * T * D_u128];
    static ap_uint<128> k_cache[K * CL * D_u128];
    static ap_uint<128> v_cache[K * CL * D_u128];
    static ap_uint<128> attn_out[T * Q * D_u128];
    tb_u16_to_apu128(BF16_07_00_attention_q_after_rope, q_rot, Q, T, D);
    tb_u16_to_apu128(BF16_07_00_attention_k_cache_after_update, k_cache, K, CL, D);
    tb_u16_to_apu128(BF16_07_00_attention_v_cache_after_update, v_cache, K, CL, D);
    attention_v2(
        q_rot, k_cache, v_cache, attn_out,
        1, layer_id, 0, T, CL, 0.125, 0, 2, 0, 0, &result);
    res = tb_compare(BF16_07_00_attention_output_projection_input, attn_out, T, Q, D, "attn_out");
    return res;
}

int main()
{
    float res[2];
    char s[41] = "========================================";

    //    printf("\n\n%s\nBF16_00_00:\n", s);
    //    res[0] = test_1();
    printf("\n\n%s\nBF16_07_00:\n", s);
    res[1] = test_2();

    printf("\n\n%s\n", s);
    printf("Test completed.\n");
    //    printf("BF16_00_00: %f\n", res[0]);
    printf("BF16_07_00: %f\n", res[1]);

    return 0;
}
