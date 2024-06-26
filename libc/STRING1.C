/* File STRING1/C */
/*
* strcmp(s1, s2) - return -1, 0, +1
*                  depending on s1 <, =, > s2
*/
strcmp(s1, s2)  char *s1, *s2;  {
  while (*s1 == *s2) {
    if (*s1 == 0) return 0;
    ++s1; ++s2;
  }
  return (*s1 - *s2);
}

/*
 * strcat(s, t) - concatenate t to end of s.
 *                s must be large enough
 */
strcat(s, t) char *s, *t; {
  char *d;
  d = s;
  --s;
  while (*++s) ;
  while (*s++ = *t++) ;
  return d;
}

/*
 * strchr(s, c) - return pointer to first occurence
 *                of c in s else 0.
 */
strchr(s, c) char *s, c; {
  while (*s) {
    if (*s++ == c)
      return --s;
  }
  return 0;
}

/*
 * strcpy(s, t) - copy s to t
 */
strcpy(s, t) char *s, *t; {
  char *d;
  d = s;
  while (*s++ = *t++) ;
  return d;
}
 