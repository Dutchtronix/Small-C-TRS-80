/* print profile result */
#include <stdio/h>

#define TABLESIZE 256
FILE f;
int *highlim, *lowlim; /* unsigned int */
int window;
int scale, size, count;
char *defname = "profile/prf";

main(argc, argv) char *argv[]; {
 int  i, junk;
 if (argc==1)
    f=fopen(defname,"r");
 else
    f=fopen(*++argv,"r");
 if (f==0) {
    fputs("cannot open ",STDERR);
    if (argc==1)
        fputs(defname,STDERR);
    else
        fputs(*argv,STDERR);
    fputs("\n",STDERR);
    exit(0);
 }
 highlim= getw(f); lowlim= getw(f);
 scale = getw(f);
 /* size = 2**scale */
 size = 1;
 do
    size <<= 1;
 while (--scale);
/* size >>= 1; /* to correct the pointer arithmetic */
 for (i=4; i<= 128; ++i)
    junk = getw(f);
 junk = 0;
 for (window=lowlim, i=1;
          (i <= TABLESIZE) && (window <= highlim);
          ++i, window += size) {
    if (junk==4) {
       junk = 0;
       printf("\n");
    }
    count = getw(f);
    printf("   $%x:%6d", window, count);
    ++junk;
 }
 printf("\n\f");
}
 