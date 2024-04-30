/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I Mikrorie - Jan de Rie
**
** Part 2
*/

/*
** search for symbol match
** on return cptr points to slot found or empty slot
*/
gsearch(sname) char *sname;
{
	cptr=cptr2=glbptr+((hash(sname)%(numglbs-1))*SYMMAX);
	while(*cptr != 0) {
		if(astreq(sname, cptr+NAME, NAMEMAX)) {
			symptr = cptr;
			return TRUE;
		}
		if((cptr=cptr+SYMMAX) >= gsymend) cptr=glbptr;
		if(cptr == cptr2) return (symptr=cptr=0);
	}
	symptr = cptr;
	return FALSE;
}

msearch(sname) char *sname;
{
	cptr=cptr2=macn+((hash(sname)%(numdefs-1))*(NAMESIZE+2));
	while(*cptr != 0) {
		if(astreq(sname, cptr, NAMEMAX)) {
			symptr = cptr;
			return TRUE;
		}
		if((cptr=cptr+NAMESIZE+2) >= msymend) cptr=macn;
		if(cptr == cptr2) return (symptr=cptr=0);
	}
	symptr = cptr;
	return FALSE;
}
/*
** TODO add #asm version
*/
hash(sname) char *sname; {
	int i, c;
	i=0;
	while(c = *sname++) i=(i<<1)+c;
	return i;
}

void setstage(before, start) int *before, *start;
{
	if((*before=stagenext)==0)
		stagenext=stage;
	*start=stagenext;
}

void clearstage(before, start) char *before, *start;
{
	*stagenext=0;
	/* reset stagenext to previous value */
	if(stagenext=before)
		return;					/* if before != 0 */
	if(start) {
		peephole(start);
    }
}

char hexmap[16] =  {'0', '1', '2', '3', '4', '5', '6', '7',
					'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

#ifdef MSC
#define LOWNIBBLE 0x0F
#else
#define LOWNIBBLE 15
#endif

void outBhex(number) int number;
{
	char hb1, hb0;
	number &= 255;
	if (0 == number) {
		outbyte('0');
	}
	else {
		hb0 = hexmap[number & LOWNIBBLE];
		hb1 = hexmap[(number >> 4) & LOWNIBBLE];
		if (hb1 > '9') outbyte('0');
		outbyte(hb1);
		outbyte(hb0);
		outbyte('H');
	}
}


void outdec(number)  int number;
{
	int k, zs;
	char c;
	zs = 0;
	k = 10000;
	if (number + 1 == -32767) {
		outstr("-32768");
		return;
	}
	if (number < 0) {
		number = (-number);
		outbyte('-');
	}
	while (k >= 1) {
		c = number / k + '0';
		if ((c != '0') | (k == 1) | (zs)) {
			zs = 1;
			outbyte(c);
		}
		number = number % k;
		k = k / 10;
	}
}


ol(ptr)  char ptr[];
{
	outstr(ptr);
	nl();
}

ot(ptr) char ptr[];
{
	outstr(ptr);
}

/*
** send string to output
** must work with symbol table names
** terminated by length byte
** TODO add #asm version
*/
void outstr(ptr) char ptr[];
{
	while(*ptr >= '\t') outbyte(*ptr++);
}

/*
** send byte to output
** TODO add #asm version
*/
void outbyte(c) char c;
{
	if(stagenext) {
		if(stagenext==stagelast) {
			newerror(52);
			return;
		}
		else *stagenext++ = c;
		checkmax();
    }
	else cout(c,output);
}

void cout(c, fd) char c; FILE *fd;
{
	if (fd)
		if (fputc(c, fd) == EOF) xout();
}

void sout(string, fd) char *string; FILE *fd;
{
	if (fd)
		if(fputs(string, fd)==EOF) xout();
}

void lout(line, fd) char *line; FILE *fd;
{
	sout(line, fd);
	cout('\n', fd);
}

void xout() {
	fputs("output error\n", stderr);
	exit(ERRCODE);
}

void nl() {
	outbyte('\n');
}

void tab() {
  outbyte('\t');
}

void col()
{
	outbyte(':');
}

void newerror(errvalue, text) int errvalue; char *text;
{
	int k;
	if (errflag++) return;
	lout(line, stdout);
	++errcount;
	k = line+4;
	while (k++ < lptr) cout(' ', stdout);
	lout("[", stdout);
	sout("**** ", stdout);
	/* print expected token first */
	if (errvalue == 47) fputs(text, stdout);
	/* cease output generation before
	** opening error file to save
	** buffer space. Would like to re-use
	** output file buffer space for error file 
	*/
	if (output) {
		/*
		** sout("Interrupted by compilation error\n", output);
		*/
		fclose(output);
		output = NULL;
#ifdef MSC
		ASSERT(errfile == NULL);
#endif
	}
	printerror(errvalue);
#ifdef PAUSEONERROR
	if (pause) {
		int ch;
		do {
			ch = fgetc(stdin);
		}
		while ((ch != '\n') && (ch != EOF));
		if (ch == EOF) exit(ERRCODE);
	}
#endif
}

void printerror(errnum) int errnum;
{
	char s[80], *s1;
#ifdef MSC
	ASSERT(output == NULL);
#endif
	if (errnum > MAXERR) errnum = MAXERR;
	if (errfile == NULL) {
		s1 = ERRFILNAM;
		if ((errfile = fopen(s1, "r")) == 0) opnerr(s1);
	}
	/* all errors are padded with spaces to MESSSIZE */
	fseek(errfile, (errnum - 1) * MESSSIZE, 0);
	fgets(s, 80, errfile);
	fputs(&s[3], stdout);
}

/*
** some string compare functions for small-C V2
** TODO add #asm version
*/
streq(str1,str2)  char str1[],str2[];
{
	/* check for string equality over whole string */
	int k;
	k=0;
	while (str2[k]) {
		if ((str1[k])!=(str2[k])) return 0;
		++k;
	}
	return k;
}

/*
** astreq
** must detect end of symbol table names terminated by
** symbol length in binary
** TODO add #asm version
*/
astreq(str1,str2,len)  char str1[],str2[];int len;
{
	/* check for string equality over first 'len' characters */
	int k;
	k=0;
	while (k<len) {
		if ((str1[k])!=(str2[k]))break;
		if(str1[k] < ' ') break; /* or '\t' */
		if(str2[k] < ' ') break;
		++k;
	}
	if (an(str1[k]))return 0;
	if (an(str2[k]))return 0;
	return k;
}

match(lit)  char *lit;
{
	int k;
	blanks();
	if (k=streq(lptr,lit)) {
		bump(k);
		return 1;
	}
	return 0;
}

amatch(lit,len)  char *lit;int len;
{
	int k;
	blanks();
	if (k=astreq(lptr,lit,len)) {
		bump(k);
		while(an(ch)) {
			newerror(14);		/* this can't be right */
			gch();
		}
		return TRUE;
    }
  return FALSE;
}
