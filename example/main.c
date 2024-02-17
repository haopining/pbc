#include <gmp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_SIZE 128
clock_t start, end;

typedef mpz_t PUBLIC_PARAM;
typedef mpz_t MASTER_SECRET_KEY;

typedef struct mpk {
  PUBLIC_PARAM P;
  mpz_t P_PUB;
} MASTER_PUBLIC_KEY;

typedef struct user_key {
  mpz_t FULL_PRIVATE_KEY;
  mpz_t PUBLIC_KEY;
} USER_KEY;

typedef struct partial_private_key {
  mpz_t E_i;
  mpz_t d_i;
} PARTIAL_PRIVATE_KEY;

typedef struct signature {
  mpz_t U_i;
  mpz_t v_i;
} SIGNATURE;

void func_set_random_mpz(mpz_t result, size_t size) {
  mpz_t tmp;
  mpz_init(tmp);
  gmp_randstate_t state;
  gmp_randinit_mt(state);
  gmp_randseed_ui(state, time(NULL));
  mpz_urandomb(tmp, state, size);
  mpz_set(result, tmp);
  mpz_clear(tmp);
}

void func_add_hash(mpz_t result, mpz_t *input, size_t size) {
  mpz_set_ui(result, 0);
  for (size_t i = 0; i < size; i++)
    mpz_add(result, result, input[i]);
}

MASTER_PUBLIC_KEY func_keygen(mpz_t s, PUBLIC_PARAM p) {
  MASTER_PUBLIC_KEY mpk;
  mpz_inits(mpk.P, mpk.P_PUB, NULL);
  mpz_set(mpk.P, p);
  mpz_mul(mpk.P_PUB, s, p);
  return mpk;
}

PARTIAL_PRIVATE_KEY func_extract(MASTER_PUBLIC_KEY mpk, MASTER_SECRET_KEY msk,
                                 mpz_t X_i, mpz_t user_id) {
  PARTIAL_PRIVATE_KEY ppk;
  mpz_inits(ppk.E_i, ppk.d_i, NULL);

  mpz_t r_i, R_i, h_1;
  mpz_inits(r_i, R_i, h_1, NULL);

  func_set_random_mpz(r_i, NUM_SIZE);
  mpz_mul(R_i, r_i, mpk.P);
  mpz_add(ppk.E_i, R_i, X_i);

  mpz_t input[3];
  mpz_inits(input[0], input[1], input[2], NULL);
  mpz_set(input[0], user_id);
  mpz_set(input[1], ppk.E_i);
  mpz_set(input[2], mpk.P_PUB);
  func_add_hash(h_1, input, 3);

  mpz_mul(ppk.d_i, h_1, msk);
  mpz_add(ppk.d_i, ppk.d_i, r_i);

  return ppk;
}

SIGNATURE func_sign(MASTER_PUBLIC_KEY mpk, USER_KEY uk, mpz_t m,
                    mpz_t user_id) {
  SIGNATURE sig;
  mpz_inits(sig.U_i, sig.v_i, NULL);

  mpz_t u_i, h_2;
  mpz_inits(u_i, h_2, NULL);
  func_set_random_mpz(u_i, NUM_SIZE);
  mpz_mul(sig.U_i, u_i, mpk.P);

  mpz_t input[5];
  mpz_inits(input[0], input[1], input[2], input[3], input[4], NULL);
  mpz_set(input[0], sig.U_i);
  mpz_set(input[1], m);
  mpz_set(input[2], user_id);
  mpz_set(input[3], uk.PUBLIC_KEY);
  mpz_set(input[4], mpk.P_PUB);
  func_add_hash(h_2, input, 5);

  mpz_mul(sig.v_i, h_2, u_i);
  mpz_add(sig.v_i, sig.v_i, uk.FULL_PRIVATE_KEY);

  return sig;
}

int func_verify(MASTER_PUBLIC_KEY mpk, SIGNATURE sig, mpz_t user_public_key,
                mpz_t m, mpz_t user_id) {
  mpz_t verify_h_1, verify_h_2;
  mpz_inits(verify_h_1, verify_h_2, NULL);
  mpz_t input[3];
  mpz_inits(input[0], input[1], input[2], NULL);
  mpz_set(input[0], user_id);
  mpz_set(input[1], user_public_key);
  mpz_set(input[2], mpk.P_PUB);
  func_add_hash(verify_h_1, input, 3);

  mpz_t input2[5];
  mpz_inits(input2[0], input2[1], input2[2], input2[3], input2[4], NULL);
  mpz_set(input2[0], sig.U_i);
  mpz_set(input2[1], m);
  mpz_set(input2[2], user_id);
  mpz_set(input2[3], user_public_key);
  mpz_set(input2[4], mpk.P_PUB);
  func_add_hash(verify_h_2, input2, 5);

  mpz_t left, right;
  mpz_inits(left, right, NULL);
  mpz_mul(left, sig.v_i, mpk.P);

  mpz_mul(right, verify_h_2, sig.U_i);
  mpz_add(right, right, user_public_key);
  mpz_t tmp;
  mpz_init(tmp);
  mpz_mul(tmp, verify_h_1, mpk.P_PUB);
  mpz_add(right, right, tmp);

  if (mpz_cmp(left, right) == 0)
    return 1;
  else
    return 0;
}

int main(int argc, char const *argv[]) {
  printf("setup,master_keygen,extract,sign,verify\n");
  for (int i = 0; i < 30; i++) {
    // SETUP
    start = clock();
    PUBLIC_PARAM param;
    mpz_init(param);
    func_set_random_mpz(param, NUM_SIZE);
    end = clock();
#ifdef TIME
    printf("%f,", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
#endif

    // KGC-KEYGEN
    start = clock();
    MASTER_SECRET_KEY msk;
    mpz_init(msk);
    func_set_random_mpz(msk, NUM_SIZE);
    MASTER_PUBLIC_KEY mpk = func_keygen(msk, param);
    end = clock();
#ifdef TIME
    printf("%f,", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
#endif

    // SET-SECRET-VALUE
    start = clock();
    mpz_t x_i, X_i;
    mpz_inits(x_i, X_i, NULL);
    func_set_random_mpz(x_i, NUM_SIZE);
    mpz_mul(X_i, x_i, mpk.P);
    end = clock();

    // EXTRACT-PARTIAL-PRIVATE-KEY
    start = clock();
    mpz_t user_id;
    mpz_init(user_id);
    func_set_random_mpz(user_id, NUM_SIZE);
    PARTIAL_PRIVATE_KEY ppk = func_extract(mpk, msk, X_i, user_id);
    end = clock();
#ifdef TIME
    printf("%f,", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
#endif

    // SET-FULL-KEY
    start = clock();
    USER_KEY uk;
    mpz_inits(uk.FULL_PRIVATE_KEY, uk.PUBLIC_KEY, NULL);
    mpz_add(uk.FULL_PRIVATE_KEY, ppk.d_i, x_i);
    mpz_set(uk.PUBLIC_KEY, ppk.E_i);
    end = clock();

    // SIGN
    start = clock();
    mpz_t m;
    mpz_init(m);
    func_set_random_mpz(m, NUM_SIZE);
    SIGNATURE sig = func_sign(mpk, uk, m, user_id);
    end = clock();
#ifdef TIME
    printf("%f,", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
#endif

    // VERIFY

    start = clock();
    int result = func_verify(mpk, sig, uk.PUBLIC_KEY, m, user_id);
    end = clock();
#ifdef TIME
    printf("%f\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
#endif
  }

  return 0;
}
