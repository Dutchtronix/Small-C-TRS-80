#include <stdio/h>

extern printf(), fprintf(), fputs(), fopen(), fclose();
extern exit(), fgets(), sscanf(), sprintf(), poll();

#define LINESIZE 132
#define MARGIN1 2
#define MARGIN2 2
#define BOTTOM 64
#define PAGELEN 72
/*
** print files with header
*/

char
  time[] = {"00:00:00"},
  date[] = {"00/00/00         "},
  *months[12]= {
    "januari",
    "februari",
    "maart",
    "april",
    "mei",
    "juni",
    "juli",
    "augustus",
    "september",
    "oktober",
    "november",
    "december" };

main (argc, argv)
int argc, *argv;
{
  char name[LINESIZE], *cptr;
  int i;
  FILE *f;
  if (argc == 1) {
    fputs("usage: fprint file [file]\n", stderr);
    exit();
  }
  while (--argc) {
    if (*(cptr = *++argv) == '-') continue;
    if ((f=fopen(*argv,"r"))==0) {
      fprintf(stderr,"%s: can't open\n", *argv);
    }
    else {
      stamp();
      fprnt(*argv, f);
      fclose(f);
    }
  }
}

fprnt(sname, source) char *sname; FILE source; {
  char line[LINESIZE];
  int lineno, pageno;
  pageno = 1;
  skip(MARGIN1);
  printf("     %s Pagina %d        %s   %s\n",
    sname, pageno, date, time);
  skip(MARGIN2);
  lineno = MARGIN1+MARGIN2+1;
  while (fgets(line, LINESIZE, source)) {
    if (lineno == 0) {
      skip(MARGIN1);
      printf("     %s Pagina %d        %s   %s\n",
        sname, ++pageno, date, time);
      skip(MARGIN2);
      lineno = MARGIN1 + MARGIN2 + 1;
    }
    printf("     %s", line);
    if (++lineno >= BOTTOM) {
      skip(PAGELEN - lineno);
      lineno = 0;
    }
    poll(TRUE);
  }
  if (lineno)
    skip(PAGELEN - lineno);
}

skip(n) int n; {
  while ( n-- > 0 ) {
    printf(" \n");
  }
}

stamp() {
  int mnth, day, year;
#asm
 LD HL,TIME     ;&TIME for Instant Assembler
 CALL 446DH     ;not TRSDOS Mod III
; CALL 3036H    ;TRSDOS Mod III
 LD HL,DATE     ;&DATE for Instant Assembler
 CALL 4470H     ;not TRSDOS Mod III
; CALL 3033H    ;TRSDOS Mod III
#endasm
  sscanf(date, "%2d %*c %2d %*c %2d", &mnth, &day, &year);
  if (mnth == 0) ++mnth;
  sprintf(date, "%d %s 19%02d\0", day, months[mnth-1], year);
}
