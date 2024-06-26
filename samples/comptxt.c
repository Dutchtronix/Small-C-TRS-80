#include <stdio/h>

#define MAXSTR 250

FILE *inf1, *inf2;
char line1[MAXSTR], line2[MAXSTR];
int lineno;
int f1,f2;

/*  compare -- compare two files for equality */
compare()
{
    int feq;
    feq = TRUE;
    lineno = 0;
    do {
        lineno = lineno + 1;
        f1 = fgets(line1, MAXSTR, inf1);
        f2 = fgets(line2, MAXSTR, inf2);
        if (f1 & f2) {
            if (strcmp(line1, line2) != 0) {
                feq = FALSE;
				printf("%d:\n%s\n%s\n", lineno, line1, line2);
			}
		}
	}
	while (f1 & f2);
    if (f2 && !f1) {
        printf("comptxt: end of file on file 1\n");
	}
    else if (f1 && !f2) {
        printf("comptxt: end of file on file 2\n");
	}
    else {
       if (feq == TRUE) 
        printf("comptxt: files are equal\n");
       else
        printf("comptxt: files are different\n");
	}
}

main(argc, argv) int argc;  char* argv[];
{
	int size1, size2;
	int stat;
	
    printf("comptxt v1.0\n");
	if (argc < 3) {
		fprintf(stderr, "usage: compbin file1 file2\n");
		exit(1);
	}
    inf1=fopen(argv[1],"r");
    if(inf1==NULL) {
		fprintf(stderr,"Cannot open file %s\n",argv[1]);
		exit(1);
    }
    inf2=fopen(argv[2],"r");
    if(inf2==NULL) {
		fprintf(stderr,"Cannot open file %s\n",argv[2]);
		fclose(inf1);
	    exit(1);
    }
	compare();
	fclose(inf1);
	fclose(inf2);
}
 