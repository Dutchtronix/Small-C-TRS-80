/* File STRING2/C */
/*
 * strncat(s, t, n) - concatenate n bytes max from
 *                    t to end of s.
 *                    s must be large enough.
 */
strncat(s, t, n) char *s, *t; {
  char *d;
  d = s;
  --s;
  while (*++s) ;
  while (n--) {
    if (*s++ = *t++) continue;
    return d;
  }
  *s = '\0';
  return d;
}

/*
 * strncmp(s, t, n) - compare 2 strings for at most n
 *                    characters and returns an integer
 *                    >0, =0, <0 as s is >t, =t, <t.
 */
strncmp(s, t, n) char *s, *t; {
  while (n && *s==*t) {
    if (*s == '\0') return 0;
    ++s; ++t; --n;
  }
  if (n)
    return (*s - *t);
  return 0;
}

/*
 * Strncpy(t, s, n) - Copy n chars from s to t, null padding
 */
strncpy(t, s, n) char *s, *t; {
  char *d;
  d = t;
  while (n-- > 0) {
    if (*d++ = *s++) continue;
    while (n-- > 0) *d++ = '\0';
  }
  *d = 0;
  return t;
}
 