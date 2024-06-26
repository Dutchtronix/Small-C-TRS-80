/* File STRING3/C */
/*
 * Strrchr(s, c) - Search s for rightmost occurence of c
 *                 Returns pointer to rightmost c or NULL
 */
strrchr(s, c) char *s, c; {
  char *ptr;
  ptr = 0;
  while (*s) {
    if (*s==c) ptr = s;
    ++s;
  }
  return ptr;
}

/*
** Strleft -- left adjust and null terminate a string
*/
Strlft(str) char *str; {
  char *str2;
  str2=str;
  while (*str2==' ')
    ++str2;
  while (*str++ = *str2++) ;
}

/*
 * Strpad - Place n occurences of ch at dest
 */
strpad(dest, ch, n) char *dest, *n; {
  /* n is a fake unsigned integer */
  while (n--) *dest++ = ch;
}
 