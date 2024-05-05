/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I V2.1B Dutchtronix 2024
*/

/*
** These functions are in front, since they will
** be used as buffer space after execution
*/
/*
** get run options
*/
ask(argc, argv) char *argv[]; {
	int i;
	char letter;
	i = 0;
	line = mline;
askon:
	while (i < argc) {
		cptr = argv[++i];
		if (cptr == NULL)
			break;
		if (*cptr != '-') continue;
		strcopy(cptr, mline);
		letter = upper(line[1]);
		if (line[2] <= ' ') {
			switch (letter) {
				case 'T': { ctext = !ctext; goto askon; }
				case 'H': { headers = !headers;  goto askon; }
				case 'P': { pause = !pause;  goto askon; }
#ifdef OTHERASM
				case 'M': { outform = M80; strcopy(DEFOUTEXT, assext);  goto askon; }
				case 'E': { outform = EDTASM; strcopy(DEFAOUTEXT, assext);  goto askon; }
				case 'I': { outform = INSASM; strcopy(DEFAOUTEXT, assext);  goto askon; }
#endif
				case '@': { escape = '@';  goto askon; }
				case '%': { fdebug = TRUE; goto askon; }
			}
		}
		if (letter == 'Z') {
			bump(0); bump(2);
			if (number(&maxzero)) continue;
		}
#ifdef OTHERASM
		if (letter == 'L') {
			bump(0); bump(2);
			if (number(&nxtlab)) {
				newnum(nxtlab); continue;
			}
		}
#endif
		if (letter == 'S') {
			bump(0); bump(2);
			if (number(&numglbs)) continue;
		}
		if (letter == 'D') {
			bump(0); bump(2);
			if (number(&numdefs)) continue;
		}
#ifdef OTHERASM
		sout("usage: cc out = in..[-hptmei@][-zlsd#]\n", stderr);
#else
		sout("usage: cc out = in..[-hpt@][-ZSD#]\n", stderr);
#endif
		exit(ERRCODE);
	}
}

#ifdef OTHERASM
newnum(val) int val;
{
	char *ptr;
	ptr = &linenum[5];
	if (val < 0) val = - val;
	while (val) {
		*--ptr = (val % 10) + '\260';
		val /= 10;
	}
}
#endif

/*
** get first output file specification from pline
**		output = input
** leave pline with input; store output in mline
*/
void getout(str) char *str;
{
	char ch;
	while (ch = *str++) {
		if (ch == '=') { 		/* don't copy '=' */
			outfile = TRUE; *--str = '\0';
			strcopy(pline, mline);	/* copy outfile to mline */
			strcopy(++str, pline);	/* copy infile to begin of pline */
			break;
		}
	}
}

/*
** open output file. First openin() has copied eventual
** output file name to mline. Just '=' means no output.
** flag outfile has been set. Not set implies that
** same name as first input file will be taken
*/
void openout()
{
	char ch, *ptr1, *ptr2;
	if (!outfile) { 		/* no output specification */
		ptr1 = mline; ptr2 = pline;
		while (ch = *ptr1++ = *ptr2++) {
			if (ch == EXTMARKER) {	/* do not copy extension */
				--ptr1;			/* remove slash */
				while (an(*ptr2++)) ; /* skip extension */
				--ptr2;			/* set to non-an */
			}
		}
	}
	outfile = TRUE;			/* prevent getout() calls */
	/* if first char = NULL then output=0 (init value) */
	if (*mline) {
		extension(mline, assext);
		if ((output = fopen(mline, "w")) == NULL) {
			sout("cannot open: ", stderr); sout(mline, stderr);
			sout("\n", stderr); exit(ERRCODE);
		}
	}
}

void setops()
{
  op2[0]=     op[0]=  zor;  /* hier5 */
  op2[1]=     op[1]= zxor;  /* hier6 */
  op2[2]=     op[2]= zand;  /* hier7 */
  op2[3]=     op[3]=  zeq;  /* hier8 */
  op2[4]=     op[4]=  zne;
  op2[5]=ule; op[5]=  zle;  /* hier9 */
  op2[6]=uge; op[6]=  zge;
  op2[7]=ult; op[7]=  zlt;
  op2[8]=ugt; op[8]=  zgt;
 /* potential bug. In cc11.c op2[9] is set to uasr */
  op2[9]=     op[9]= zasr;  /* hier10 */
  op2[10]=     op[10]= zasl;
  op2[11]=     op[11]= zadd;  /* hier11 */
  op2[12]=     op[12]= zsub;
  op2[13]=     op[13]=zmult;  /* hier12 */
  op2[14]=     op[14]= zdiv;
  op2[15]=     op[15]= zmod;
}

/*
** print all assembler info before any code is generated
*/
void doheader()
{
#ifdef OTHERASM
	if (outform != M80)
		outstr("\323        ");	/* header */
	prnum();
#endif
	outstr(";Dutchtronix - TRS-80 Small-C V");
	outstr(cversie); outbyte('\n');
#ifdef OTHERASM
	if (outform == M80)
#endif
	{
		outstr("\t.REQUEST\tCCLIB\n");
		outstr("\tEXT\tCCMAIN\n");
	}
#ifdef OTHERASM
	if ((outform == EDTASM) && (getlabel() < 3)) {
		prnum(); outstr("\t6C00H\n");	/* versie 2.0 */
	}
	if (outform == INSASM) {
		prnum(); outstr("\tJP\t&CCMAI\n");
	}
#endif
}

void initExprArgs()
{
	argsf1[ARG4] = eq0;
	argsf1[ARG5] = ne0;
	argsf1[ARG6] = hier6;
	argsf1[ARG7] = hier7;
	argsf1[ARG8] = hier8;
	argsf1[ARG9] = hier9;
	argsf1[ARG10] = hier10;
	argsf1[ARG11] = hier11;
	argsf1[ARG12] = hier12;
	argsf1[ARG13] = hier13;
	argsf2[ARG4] = hier4;
	argsf2[ARG5] = hier5;
}

/*
** execution begins here
*/
void main(argc, argv) int argc;  char* argv[];
{
	char *s;
	int i;
	argcs = argc;
	argvs = argv;
/*	initheap(); now called from calloc() */
	initExprArgs();

	fputs("CC ", stderr);
	fputs(cversie, stderr);
	fputc('\n', stderr);
/*
** allocate these buffers before ask() etc.
*/
	mline = ccalloc(LINESIZE);
	pline = ccalloc(LINESIZE);
#ifdef MSC
	ask(argc, argv);			/* get user options */
#else
	ask(argc, &argv);			/* get user options */
#endif
	openin();					/* initial input file */
	if (eofstatus == TRUE) {
		newerror(48); exit(ERRCODE);
	}
	openout();					/* open output file */
	setops();					/* set values in op array */
	
	swnext=CCALLOC(SWTABSZ);
	swend=swnext+((SWTABSZ-SWSIZ)>>1);

#ifdef MSC
	stage = &stagebuffer[0];
#else
	stage = ask;				/* init code, min. STAGESIZE bytes needed */
#endif

	stagelast=stage+STAGELIMIT;
	stagemax = stage;

	wq=CCALLOC(WQTABSZ*BPW);
	wqptr=wq;					/* clear while queue */

	litq=CCALLOC(LITABSZ);

	macn=CCALLOC(MACNSIZE);
#ifdef MSC
	int a = (int)macn + MACNSIZE - 1;
	msymend = (char *)a;
#else
	msymend = macn + MACNSIZE - 1;
#endif

	macq=CCALLOC(MACQSIZE);
	s = "BEGIN"; msearch(s);
	strcopy(s, symptr);
	*(iptr = &symptr[NAMESIZE]) = 0;
	*macq = '{'; macq[1] = NULL;
	s = "END"; msearch(s);
	strcopy(s, symptr);
	*(iptr = &symptr[NAMESIZE]) = 2;
	macq[2] = '}'; macq[3] = NULL;
	macidx = 4;

	symtab=CCALLOC(SYMTBSZ);
#ifdef MSC
	a = (int)symtab + (NUMLOCS * SYMAVG);
	glbptr = (char*)a;
	a = (int)glbptr + (numglbs * SYMMAX - 1);
	gsymend = (char*)a;
#else
	glbptr = symtab + (NUMLOCS * SYMAVG);
	gsymend = glbptr + (numglbs * SYMMAX - 1);
#endif

#ifdef MSC
	initptrmaptable();
#endif

	stagenext = 0;				/* direct output mode */
	doheader();					/* intro code */
	preprocess();				/* fetch first line */
	/*
	** end of initialization code
	*/
#ifndef MSC
#ifdef DEBUG
#asm
	JR	_MAIN
	DB	'ENDINIT'
_MAIN:
#endasm
#endif
#endif
	parse();					/* process ALL input */
	outside();					/* verify outside any function */
	trailer();					/* follow-up code */

	if (output) fclose(output);
	if (errfile) fclose(errfile);
	if (errcount == 0) fputs("no ", stderr);
	else {
		myitoa(errcount, mline);
		fputs(mline, stderr);
	}
	fputs(" error(s) detected\n", stderr);

#ifdef MSC
	i = (stagemax - stage);
	myitoa(i, mline);
	fputs(mline, stderr);
	fputs(" bytes in stagebuffer used\n", stderr);
#endif
}

/*
** copy str1 to str2
*/
void strcopy(str1, str2) char *str1, *str2;	/* *str2??? */
{
#ifdef MSC
	strcpy(str2, str1);			/* dest, src */
/*	while (*str2++ = *str1++); */
#else
#asm
	pop	bc
	pop	de	;str1
	pop	hl	;str2
	push hl
	push de
	push bc
str88:
	ld	a,(de)
	ld	(hl),a
	or	a
	ret	z
	inc	hl
	inc	de
	jr	str88
#endasm
#endif
}

/*
* itoa(n, s) - Convert postive n to characters in s
*/
myitoa(n, s) char* s; int n;
{
	char* ptr;
	char* ptr2;
	int l, c;
	l = 0;
	ptr = s;
	do {     /* generate digits in revers order */
		*ptr++ = n % 10 + '0'; /* get next digit */
		++l;
	} while ((n /= 10) > 0);   /* delete it */
	*ptr = '\0';
	ptr2 = s + l - 1;
	while (s < ptr2) {
		c = *s;
		*s++ = *ptr2;
		*ptr2-- = c;
	}
}

/*
** insert default extension into filename
*/
void extension(filename, defaultext) char *filename, *defaultext;
{
	char buf[20];
	if (*filename != '*') {			/* no extension for devices */
		while (an(*filename++)) ;	/* skip letters/digits */
		if (*--filename != EXTMARKER) {	/* there is no extension */
			strcopy(filename, buf);	/* copy rest to buf, maybe empty */
			while (*filename++ = *defaultext++) ;
			strcopy(buf, --filename); /* copy rest back */
		}
	}
}

/*
** file cannot be opened
*/
opnerr(s) char *s;
{
	sout("cannot open: ", stderr); sout(s, stderr);
	sout("\n", stderr); exit(ERRCODE);
}

/*
** check if new input file is OTHERASM format
*/
chkasm()
{
#ifdef OTHERASM
#ifdef MSC
	char c; int i; FILE* fptr;
	fptr = input[curlevel];
	c = fgetc(fptr);
	if ((c & 255) == 0xD3) {
		for (i = 1; i++ <= 6; fgetc(fptr));
	}
	else {
		ungetc(c, fptr);
	}
#else
input[curlevel];			/* to HL */
#asm
	LD	E, (HL)
	INC	HL
	LD	D, (HL)
	LD	HL, 51FFH
	RST	18H
	RET	NC
	CALL 13H; getbyte
	CP	0D3H
	JR	Z, _CHK1; TODO: JP NZ, 443Fh
	CALL 443FH; reset TODO : CALL + RET = JMP
	RET
	_CHK1:
	LD	B, 6
	_CHK2:
	CALL 13H
	DJNZ _CHK2
	RET
#endasm
#endif
#endif
}

/*
** get next input file
*/
void openin()
{
	while (++filearg < argcs) {
		if (*(cptr = argvs[filearg]) == '-') continue;
		strcopy(cptr, pline);			/* copy argument */
		if (!outfile)
			getout(pline);				/* remove eventual output file */
		extension(pline, DEFSRCEXT);	/* default extension */
		if (curlevel != 0) {
			newerror(2); exit(ERRCODE);
		}
		if ((input[0] = fopen(pline, "r")) == NULL)
			opnerr(pline);
		chkasm();
		if (files == TRUE) kill();		/* not first time */
		files = TRUE;
		return;
	}
	eofstatus = TRUE; kill();
}

/*
** process all input text
**
** At this level, only static declarations
**		and function definitions are legal
*/
void parse()
{
	while (eofstatus == 0) {
		if (amatch("EXTERN", 6))	dodeclare(EXTERNAL);
		else if (dodeclare(STATIC)) ;
		else
			newfunc();
		blanks();				/* force eof if pending */
	}
}

#ifdef MSC
char* ptrmaptable[1000];
/*
** init ptrmaptable
*/
void initptrmaptable(void)
{
	for (int i = 0; i < 1000; ++i) {
		ptrmaptable[i] = (char *)-1;
	}
}

/*
** map ptr to index
*/
int16_t mapptr2idx(char *syptr)
{
	for (int i = 0; i < 1000; ++i) {
		if (ptrmaptable[i] == syptr) return (int16_t)i;
	}
	for (int i = 0; i < 1000; ++i) {
		if (ptrmaptable[i] == (char *)-1) {
			ptrmaptable[i] = syptr;
			return i;
		}
	}
	/* table full */
	newerror(53);
	exit(ERRCODE);
}

/*
** map index to ptr value
*/
char * mapidx2ptr(int16_t idx)
{
	if (idx < 1000) {
		return ptrmaptable[idx];
	}
	newerror(53);
	exit(ERRCODE);
}
#endif

#ifdef MSC
/*
** Compiler memory allocator when running on Windows
*/
ccalloc(int n)
{
	if (-1 == cchpsize) {
		cchpsize = 0;
		msccchpsize = 20000;
		ccheap = malloc(msccchpsize);
		if (NULL == ccheap) {
			newerror(54);
			exit(ERRCODE);
		}
	}
	lastp = ccheap;
	lastnheap = n;
	ccheap += n;
	cchpsize += n;
	memset(lastp, 0, n);
	return lastp;
}

ccfree(char *p)
{
	if (lastp != p) {
		sout("\nFREE MISMATCH\n", stderr);
		exit(ERRCODE);
	}
	ccheap -= lastnheap;
	cchpsize -= lastnheap;
}

#else		/* !MSC */

/*
** very simple memory allocator. Assumes LIFO (mostly)
*/
#asm
$MEMRY::
	DEFW 0
#endasm
#ifdef DEBUG
#asm
	db	'INITHEAP'
#endasm
#endif

ccalloc(n) int n;
{
	if (-1 == cchpsize) {
		cchpsize = 0;
#asm
		ld	hl, ($MEMRY)
		ld(ccheap), hl
#endasm
		foutbuf = NULL;			/* only used for 306 byte buffer */
	}
	lastp = ccheap;
	lastnheap = n;
	ccheap += n;
	cchpsize += n;
	ccavail();
	memset(lastp, 0, n);
	return lastp;
}

ccfree(p) char* p;
{
#ifdef DEBUG
	if (lastp != p) {
		sout("\nFREE MISMATCH\n", stderr);
		exit(ERRCODE);
	}
#endif
	ccheap -= lastnheap;
	cchpsize -= lastnheap;
}

malloc(n) int n;
{
	char* ptr;
	if (n == 306) {
		/*
		** called from fopen
		*/
		if (foutbuf) {
			/*
			** non-LIFO returned buffer available.
			*/
			ptr = foutbuf;
			foutbuf = NULL;
			return ptr;
		}
		else
			return ccalloc(n);
	}
#ifdef DEBUG
	else {
		sout("\nINVALID MALLOC\n", stderr);
		exit(ERRCODE);
	}
#endif
}

cfree(p) char* p;
{
	if (lastp != p) {
		/*
		** out of order ccfree(). Assume this is caused by
		** closing of the output file while more than 1
		** input file is open.
		*/
		foutbuf = p;
	}
	else
		ccfree(p);
}

ccavail()
{
#asm
	LD	HL, (ccheap)
	LD	DE, 32H
	ADD	HL, DE
	EX	DE, HL
	LD	HL, 0
	ADD	HL, SP
	CALL	CCC27##
	LD	A, H  ;test not needed
	OR	L
	RET	Z
NoMemErr:
	LD	HL, stderr
	PUSH	HL
	LD	HL, MSGNOMEM
	PUSH	HL
	CALL	FPUTS
	JP	exit	;need argument 
MSGNOMEM: db "no memory", 13, 0
#endasm
}
#endif		/* MSC */

#ifndef MSC
memset(s, c, cnt) char* s; int c, cnt;
{
	/*
	** cannot use IX, IY since fopen() uses it
	*/
#asm
; memset(void* s, char c, uint cnt)
; enter: hl = void* s
;          e = char c
;         bc = uint n
; 
	pop	hl	;return address
	ld (saveret), hl
	pop	hl	;s
	pop	de	;c
	pop	bc	;cnt

	ld a,b
	or c
	ld a,e
	ld d,h
	ld e,l
	JR	Z,outmemset
	ld(hl),a
	inc de
	dec bc
	ld a, b
	or c
	JR	Z,outmemset
	ldir
outmemset: 
	push bc
	push bc
	push bc
	ld bc,(saveret)
	push bc
	ret
saveret:
	DEFW 0
#endasm
}
#endif
