/*
 * purpose of this program:
 *      combine various files in one big file,
 *      but forget the last byte of all files
 *      except for the last one!
 */

#include <stdio/h>

#define MAXCHAR 132

int opeek;

main (argc, argv) char *argv[]; {
    char s[MAXCHAR];
    FILE *fout;
    if (argc < 2) {
        printf("usage: makelib output-file [input-files]\n");
        exit(1);
    }
    if ((fout=fopen(*++argv, "w")) == NULL) {
        printf("cannot open: %s\n", *argv);
        exit(1);
}
    if (argc > 2) {
        --argc;         /* correct output file */
        while (--argc)
            append(*++argv, fout);
    }
    else while (TRUE) {
        printf("enter file to append: "); fflush(stdout);
        fgets(s, MAXCHAR, stdin);
        if (strcmp(s, "stop\n") == 0) break;
        append(s,fout);
    }
    putc(opeek, fout); fflush(fout);    /* output last character */
    printf("append ready\n");
}

append(s, f) char *s; FILE *f; {
    FILE *finput;
    char c;
    if ((finput=fopen(s, "r")) == NULL) {
        printf("cannot open: %s\n", s);
        exit(1);
    }
    opeek = -1;         /* delete last character */
    while ((c=getc(finput)) != EOF)
        outchar(c, f);
    fflush(f);
    fclose(finput);
}

outchar(c, f) char c; FILE *f; {
    if (opeek != -1)
        putc(opeek, f);
    opeek = c & 0xff;
}
 