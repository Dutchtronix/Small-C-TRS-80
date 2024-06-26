/* { } \n [ ] | */
#include <stdio/h>

/*
 * Getarg(n, s, size, argc, argv)
 *        - Get command line argument
 *          Returns number of characters moved on succes
 *          else EOF
 */
getarg(n, s, size, argc, argv) char *s, *argv[]; {
  char *str;
  int i;
  if (n < 0 | n >= argc) {
    *s = NULL;
    return EOF;
  }
  i = 0;
  str = argv[n];
  while (i < size) {
    if ((s[i] = str[i]) == NULL) break;
    ++i;
  }
  s[i] = NULL;
  return i;
}
 