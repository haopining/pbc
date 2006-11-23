//requires
// * gmp.h
#ifndef RANDOM_H
#define RANDOM_H

/*@manual pbcrandom
Uses ''filename'' as a source of random bytes. For example,
on Linux one might use <filename>/dev/random</filename>.
*/
void random_set_file(char *filename);

/*@manual pbcrandom
Use a determinstic random number generator.
*/
void random_set_deterministic(void);

/*@manual pbcrandom
Use previously chosen random number generator to select
a random ''z'' that is less than ''limit''.
*/
void pbc_mpz_random(mpz_t z, mpz_t limit);

/*@manual pbcrandom
Use previously chosen random number generator to select
a random ''bits''-bit integer ''z''.
*/
void pbc_mpz_randomb(mpz_t z, unsigned int bits);

#endif //RANDOM_H