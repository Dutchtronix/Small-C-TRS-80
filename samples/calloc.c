#include <stdio/h>

/*
** calloc (num, size)
** return pointer to zero-d area
*/
calloc (num, size) {
  int i,j;
  char *p, *q;
  j = size * num;
  if ((p = q = malloc(j)) == 0)
    return NULL;
  for (i = 0; ++i <= j; *q++ = 0) ;
  return p;
}
/*
** abs -- returns absolute value of nbr
*/
abs(nbr)  int nbr; {
  if (nbr<0)
    return -nbr;
  else
    return nbr;
}

/*
** sign -- return -1, 0, +1 depending on the sign of nbr
*/
sign(nbr) {
  if (nbr > 0)
    return 1;
  if (nbr == 0)
    return 0;
  return -1;
}
