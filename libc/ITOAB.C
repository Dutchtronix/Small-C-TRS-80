/* { } \n [ ]  */

/*
 * itoab(n,s,b)  Convert unsigned 'n' to chars in s
 * internal function for printf
 */
itoab(n, s, b) char *s; {
  char *ptr;
  int lowbit;
  ptr = s;
  b >>= 1;
  do {
    lowbit = n & 1;
    n = (n >> 1) & 32767;
    *ptr = ((n % b) << 1) + lowbit;
    if (*ptr < 10)
      *ptr += '0';
    else
      *ptr += '0' + ('A' - ':');
    ++ptr;
  } while (n /= b);
  *ptr = '\0';
  reverse(s);
}
 