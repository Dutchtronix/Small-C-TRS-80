/*
 * compress/decompress a text file
 *    Mikrorie Software
 */

#include <stdio/h>

char *tabl1 = {" etaoinshrdlu()+"},
     *tabl2 = {" etaoin"};
char ch, *cptr, *cptr2, *cptr3,
     line[132], line2[132];
int code;

main (argc, argv) int argc, *argv[];
{
  int direction;
  if (argc == 1)
    fputs("comprs: supply direction  c or d\n", STDERR);
  else {
    direction = (*argv[1] == 'C');
    while (fgets(line, 132, STDIN)) {
      if (direction)
        compress(line, line2);
      else
        decompress(line, line2);
      fputs(line2, STDOUT);
    }
    fclose(STDOUT);
  }
}

map(ch, tabel) char ch, *tabel;
{
  cptr = tabel;
  while (*cptr) {
    if (*cptr++ == ch)
      return (cptr-tabel-1);
  }
  return -1;
}

compress(s1, s2) char *s1, *s2;
{
  int p1,p2;
  cptr2 = s1; cptr3 = s2;
  while (ch = *cptr2++) {
    if ((p1 = map(ch, tabl1)) != -1)
      if ((p2 = map(*cptr2, tabl2)) != -1) {
        ch = (p1 << 3) + p2 + 128;
        ++cptr2;
      }
    *cptr3++ = ch;
  }
  *cptr3 = 0;
}

decompress(s1, s2) char *s1, *s2;
{
  cptr2 = s1; cptr3 = s2;
  while ((code =*cptr2++ & 255)) {
    if (code > 127) {
      code &= 127;
      *cptr3++ = tabl1[ (code >> 3) & 15 ];
      *cptr3++ = tabl2[ code & 7 ];
      continue;
    }
    *cptr3++ = code;
  }
  *cptr3 = 0;
}
 