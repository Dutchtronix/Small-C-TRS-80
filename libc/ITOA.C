/*
* itoa(n, s) - Convert n to characters in s
* error: value -32768 gets a wrong string
*/
itoa (n, s) char *s; int n; begin
  int sign;
  char *ptr;
  ptr = s;
  if ((sign = n) < 0)      /* record sign */
    n = -n;                /* make n positive */
  do {     /* generate digits in revers order */
    *ptr++ = n % 10 + '0'; /* get next digit */
  }
  while ((n /= 10) > 0);   /* delete it */
  if (sign < 0) *ptr++ = '-';
  *ptr = '\0';
  reverse(s);
}
 