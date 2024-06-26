/*
** compbin
** compare 2 binary files on a TRS-80 running Newdos/80
**
*/
 
#include <stdio/h>

/* within FILE structure */
#define SLFCB 	0
#define SLMODE	2
#define SLRSLT	3
#define SLFLAG	4
#define SLPUSH	5
#define SLSIZE	6

/* within Newdos/80  FCB structure */
#define MSB 13
#define MIDSB 12
#define LSB 8

getfstatus(fp) FILE *fp;
{
	char *ptr;
	ptr = fp;
	ptr += SLRSLT;
	return *ptr;
}

getffcb(fp) FILE *fp;
{
	return *fp;
}

getEOFpos(fp) FILE *fp;
{
	char *fcb;
	fcb	= getffcb(fp);
	if (fcb == NULL) return -1;
	if (*(fcb+MSB) != 0) {
		fprintf(stderr, "file size > 64k bytes. MSB = 0x%x\n", *(fcb+MSB));
		return -1;
	}
	return ((*(fcb+MIDSB) << 8) + *(fcb + LSB));
}

FILE *fp1, *fp2;
char buff1[258], buff2[258];

readcomp(size) int size;
{
	int stat,i;
	fread(buff1,1, size, fp1);
	if ((stat = getfstatus(fp1)) != 0) {
		fprintf(stderr, "Error reading file 1, status %d\n", stat);
	}
	fread(buff2,1, size, fp2);
	if ((stat = getfstatus(fp1)) != 0) {
		fprintf(stderr, "Error reading file 1, status %d\n", stat);
	}
	for (i = 0; i < size; ++i) {
		if (buff1[i] != buff2[i]) {
			return -1;
		}
	}
	return 0;
}
	
main(argc, argv) int argc;  char* argv[];
{
	int size1, size2;
	int stat;
 char *unsptr;
	
    printf("compbin v1.0\n");
	if (argc < 3) {
		printf("usage: compbin file1 file2\n");
		exit(1);
	}
    fp1=fopen(argv[1],"r");
    if(fp1==NULL) {
		fprintf(stderr,"Cannot open file %s\n",argv[1]);
		exit(1);
    }
    fp2=fopen(argv[2],"r");
    if(fp2==NULL) {
		fprintf(stderr,"Cannot open file %s\n",argv[2]);
		fclose(fp1);
	    exit(1);
    }
	stat = 0;
	while (TRUE) {
		size1 = getEOFpos(fp1);
		if (size1 == -1) {
			stat = -1;
			break;
		}
		size2 = getEOFpos(fp2);
		if (size2 == -1) {
			stat = -1;
			break;
		}

		if (size1 != size2) {
			fprintf(stderr, "file1 and file2 not same size\n");
			stat = -1;
			break;
		}
 unsptr = size1;

		while (unsptr >= 256) {
			if (readcomp(256) == -1) {
				stat = -1;
				break;
			}
			unsptr -= 256;
		}
 size1 = unsptr;
		if (stat == 0) {
			if (readcomp(size1) == -1) {
				stat = -1;
			}
		}
		break;
	}
	fclose(fp1);
	fclose(fp2);
	if (stat == -1) {
		fprintf(stderr, "Files are different\n");
	}
	else {
		fprintf(stderr, "Files are identical\n");
	}
	
}
 