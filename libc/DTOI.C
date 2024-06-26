#include <stdio/h>
/*
** dtoi -- convert signed decimal string to integer nbr
**         returns field length, else ERR on error
*/
dtoi(decstr, nbr) char *decstr; int *nbr; {
  int len, s;
  if ((*decstr)=='-') {
    s=1; ++decstr;
  }
  else
    s=0;
  if ((len=toi(decstr, nbr)) < 0)
    return ERR;
  if (*nbr<0)
    return ERR;
  if (s) {
    *nbr = -*nbr;
    return ++len;
  }
  else
    return len;
}

toi(decstr, nbr) char *decstr; int *nbr; {
  int d, t;
  d = 0; *nbr = 0;
  while ((*decstr >= '0') & (*decstr <= '9')) {
    t = *nbr; t = (10 * t) + (*decstr++ - '0');
    if ((t >= 0) & (*nbr < 0)) return 0;
    ++d; *nbr = t;
  }
  return d;
}
 