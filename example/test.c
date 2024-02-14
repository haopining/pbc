#include <iostream>
#include <time.h>
#include <pbc/pbc.h>

int main() {
    // 定义配对环境
    clock_t start, end;
    pbc_param_t params;

    pairing_t pairing;
    element_t g, h, gh;

    // Initialize pairing and elements
    start = clock();
    pbc_param_init_a_gen(params, 160, 512);
    pairing_init_pbc_param(pairing, params);
        
    element_t g1;
    element_init_G1(g1, pairing);

    // 随机生成G1上的元素和标量
    element_random(g1);
    mpz_t scalar;
    mpz_init(scalar);
    // mpz_urandomb(scalar, pairing->rbits);
    // 计算标量乘法
    element_t result;
    element_init_G1(result, pairing);
    element_pow_zn(result, g1, scalar);
    end = clock();
    printf("Tsm exec time: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}