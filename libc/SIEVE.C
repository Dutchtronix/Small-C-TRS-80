/*
** Eratosthenes Sieve Prime Number Program in C from
** Byte January 1983, by Jim and Gary Gilbreath
** compile and link:
** cc sieve
** m80 sieve=temp
** l80 sieve,sieve-n-e
*/

#define OPTIMIZE       /* optimizations for C */

#include <stdio/h>

#define size 8190
#define NTIMES 10

char flags[size+1];
char time[] = {"00:00:00"};

#ifdef OPTIMIZE
int ii,prime, k, count, iter;
char *f;
#endif

main(argc, argv) char *argv[];
{
#ifndef OPTIMIZE
  int ii,prime, k, count, iter;
#endif
  printf("%d iterations\n",NTIMES); timestamp();
  for (iter = 1; iter <= NTIMES; iter++)  /* do NTIMES times */
    {
    printf("pass %d\n", iter);
    count = 0;                        /* prime counter */
#ifdef OPTIMIZE
    for (ii = 0, f=&flags[0]; ii++  <= size;) /* set all flags true */
        *f++ = true;
#else
    for (ii = 0; ii <= size; ii++)             /* set all flags true */
        flags[ii] = true;
#endif
#ifdef OPTIMIZE
    f = &flags[0];
#endif
    for (ii = 0 ; ii  <= size; ii++) {  /* set all flags true */
#ifdef OPTIMIZE
    if (*f++) {          /* found a prime */
#else
    if (flags[ii]) {      /* found a prime */
#endif
        prime = ii + ii + 3;
/*      printf("%d\n", prime); */
        for (k = ii + prime; k <= size; k += prime)
          flags[k] = false;           /* kill all multiples */
        ++count;                      /* primes found */
      }
    }
  }
  timestamp();
  printf("%d primes.\n", count);      /* primes found on 10th pass */
}

/*
** print current time
*/
timestamp() {
#asm
 LD HL,TIME     ;&TIME for Instant Assembler
 CALL 446DH ;not TRSDOS Mod.III
; CALL 3036H ;TRSDOS Mod.III
#endasm
  printf("tijd is: %s\n", time);
}
 