#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    printf("Type Definitions Verification (on this system):\n");
    printf("--------------------------------------------------------------------------------------\n");
    printf("| %-14s | %-12s | %-20s | %-20s \n", "Type", "Size (Bytes)", "Min Value", "Max Value");
    printf("|----------------|--------------|----------------------|----------------------\n");

    // 1. 精确宽度 (Exact Width)
    printf("| int8_t         | %zu            | %-20d | %-20d \n", sizeof(int8_t), INT8_MIN, INT8_MAX);
    printf("| uint8_t        | %zu            | %-20d | %-20d \n", sizeof(uint8_t), 0, UINT8_MAX);
    printf("| int16_t        | %zu            | %-20d | %-20d \n", sizeof(int16_t), INT16_MIN, INT16_MAX);
    printf("| uint16_t       | %zu            | %-20d | %-20d \n", sizeof(uint16_t), 0, UINT16_MAX);
    printf("| int32_t        | %zu            | %-20" PRId32 " | %-20" PRId32 " \n", sizeof(int32_t), INT32_MIN, INT32_MAX);
    printf("| uint32_t       | %zu            | %-20d | %-20" PRIu32 " \n", sizeof(uint32_t), 0, UINT32_MAX);
    printf("| int64_t        | %zu            | %-20" PRId64 " | %-20" PRId64 " \n", sizeof(int64_t), INT64_MIN, INT64_MAX);
    printf("| uint64_t       | %zu            | %-20d | %-20" PRIu64 " \n", sizeof(uint64_t), 0, UINT64_MAX);

    printf("|----------------|--------------|----------------------|----------------------\n");

    // 2. 最小宽度 (Minimum Width) - 通常与精确宽度相同，除非硬件不支持
    printf("| int_least8_t   | %zu            | %-20d | %-20d \n", sizeof(int_least8_t), INT_LEAST8_MIN, INT_LEAST8_MAX);
    printf("| int_least16_t  | %zu            | %-20d | %-20d \n", sizeof(int_least16_t), INT_LEAST16_MIN, INT_LEAST16_MAX);
    printf("| int_least32_t  | %zu            | %-20" PRIdLEAST32 " | %-20" PRIdLEAST32 " \n", sizeof(int_least32_t), INT_LEAST32_MIN, INT_LEAST32_MAX);
    printf("| int_least64_t  | %zu            | %-20" PRIdLEAST64 " | %-20" PRIdLEAST64 " \n", sizeof(int_least64_t), INT_LEAST64_MIN, INT_LEAST64_MAX);

    printf("|----------------|--------------|----------------------|----------------------\n");

    // 3. 最快宽度 (Fastest Width) - 注意这里的大小可能比请求的更大
    printf("| int_fast8_t    | %zu            | %-20d | %-20d \n", sizeof(int_fast8_t), INT_FAST8_MIN, INT_FAST8_MAX);
    printf("| int_fast16_t   | %zu            | %-20" PRIdFAST16 " | %-20" PRIdFAST16 " \n", sizeof(int_fast16_t), INT_FAST16_MIN, INT_FAST16_MAX);
    printf("| int_fast32_t   | %zu            | %-20" PRIdFAST32 " | %-20" PRIdFAST32 " \n", sizeof(int_fast32_t), INT_FAST32_MIN, INT_FAST32_MAX);
    printf("| int_fast64_t   | %zu            | %-20" PRIdFAST64 " | %-20" PRIdFAST64 " \n", sizeof(int_fast64_t), INT_FAST64_MIN, INT_FAST64_MAX);

    printf("|----------------|--------------|----------------------|----------------------\n");

    // 4. 特殊用途 (Pointer & Max)
    printf("| intptr_t       | %zu            | %-20" PRIdPTR " | %-20" PRIdPTR " \n", sizeof(intptr_t), INTPTR_MIN, INTPTR_MAX);
    printf("| intmax_t       | %zu            | %-20" PRIdMAX " | %-20" PRIdMAX " \n", sizeof(intmax_t), INTMAX_MIN, INTMAX_MAX);

    return 0;
}

