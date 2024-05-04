/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I  V2.1B Dutchtronix 2024
**
** Part 4
*/

/*
** functions to output code to stage-buffer
** stage-buffer MUST be initialized (setStage)!!
*/

checkstage()
{
	if (stagenext == 0) {		/* not initialized */
		newerror(51); exit(ERRCODE);
	}
	if (stagenext >= stagelast) {
		newerror(39); exit(ERRCODE);
	}
}

/*
** update highest location in stagebuffer used
*/
checkmax()
{
	if (stagenext > stagemax) stagemax = stagenext;
}
/*
** return length (in bytes) of int_op at opsptr
*/
intlen(opsptr) char *opsptr;
{
	int int_op;
	int_op = *opsptr & 255;
	if (int_op < LDAB)
		return 1;
	if (int_op == LDAB)
		return 2;
	if (int_op == LDHLLO)
		return 5;
	return 3;
}

outcode(op) char op;
{
	checkstage();
	*stagenext++ = op;
	checkmax();
}

outcbyte(op, value) char op, value;
{
	checkstage();
	*stagenext++ = op;
	*stagenext++ = value;
	checkmax();
}

outcword(op, value) char op; int value;
{
	checkstage();
	*stagenext++ = op;
	*(iptr = stagenext) = value;
	stagenext += BPW;
	checkmax();
}

outclword(op, labelval, value) char op; int labelval, value;
{
	checkstage();
	*stagenext++ = op;
	iptr = stagenext;			/* int16_t ptr */
	*iptr = labelval;
	*++iptr = value;
	stagenext += 2 * BPW;
	checkmax();
}

outsword(op, symptr) char op; char* symptr;
{
	int16_t ptridx;
	checkstage();
	*stagenext++ = op;
	iptr = stagenext;
#ifdef MSC
	ptridx = mapptr2idx(symptr);
	*iptr = ptridx;
#else
	*iptr = symptr;;
#endif
	stagenext += BPW;
	checkmax();
}

/*
** print any assembler stuff needed at the end
*/
/* Need to mark unknown functions as AUTOEXT */
trailer()
{
	cptr = glbptr;
	while (cptr <= gsymend) {
		if ((*(cptr + IDENT) == FUNCTION) && (*(cptr + CLASS) == AUTOEXT)) {
			doextern(cptr + NAME);
		}
		cptr += SYMMAX;
	}
#ifdef OTHERASM
	if (outform == M80)
		outstr("\tEND\n");
	else
		outbyte('\032');		/* control-Z */
#else
	outstr("\tEND\n");
#endif
}

/*
** define label to assembler
*/
labelform(extrndef) int extrndef;
{
#ifdef OTHERASM
	switch (outform) {
		case M80: {
			outbyte(':');
			if (extrndef == TRUE) outbyte(':');
			break;
		}
		case EDTASM: {
			outstr("\tDEFS\t0");
			break;
		}
		case INSASM: {
			outstr("\tNOP");
			break;
		}
	}
#else
	outbyte(':');
	if (extrndef == TRUE) outbyte(':');
#endif
	nl();
}

/*
** send global symbol (var or function) to output
*/
symtoass(ptr) char *ptr;
{
	int i,len;
#ifdef OTHERASM
	if (outform == INSASM) {
		outbyte('&'); len = 5;
	}
	else
#endif
		len = 6;
	i = 0;
	while (*ptr) {
		outbyte(*ptr++);
		if (++i >= len) break;
	}
}

/*
** declare entry point
*/
entry(s, labelstate) char *s;
{
	prnum(); symtoass(s);
	if (labelstate == TRUE)
		labelform(TRUE);			/* terminate label */
	else {
#ifdef OTHERASM
		if (outform == M80)
#endif
			outstr("::");
		inlabel = TRUE;			/* set inlabel state */
	}
}

/*
** declare external reference
*/
void doextern(syname) char *syname; {
#ifdef OTHERASM
	if (outform == M80)
#endif
	{
		outstr("\tEXT ");
		symtoass(syname);
		nl();
	}
}

/*
** define a word to the assembler
*/
void dodefw(val) char *val;			/* unsigned */
{
	if (inlabel)
		inlabel = FALSE;
	else
		prnum();
	outstr("\tDEFW\t");
	outdec(val);
	nl();
}

/*
** define storage to the assembler
*/
void dodefs(val) char *val;			/* unsigned */
{
	if (inlabel)
		inlabel = FALSE;
	else
		prnum();
	outstr("\tDEFS\t");
	outdec(val);
	nl();
}

/*
** define a label reference to the assembler
*/
void deflabel(labelnr, value) int labelnr, value;
{
	if (inlabel)
		inlabel = FALSE;
	else
		prnum();
	outstr("\tDEFW\t");
	printlabel(labelnr);
	if (value != 0) {
		outbyte('+');
		outdec(value);
	}
	nl();
}

/*
** generate a switch table
*/
void swtable(ptr, next) int *ptr, *next;
{
	while (ptr < next) {
		outcword(DEFLWRD, *ptr++);	/* switch label */
		outcword(DEFWRD, *ptr++);	/* switch value */
	}
	outcword(DEFWRD, 0);			/* terminate table */
}

/*
** in-line assembler
*/
void asmstart()
{
	if (stagenext) {
		outcword(LITERAL, litbound); /* idx behind char */
	}
}

void asmline(line) char* line;
{
	if (stagenext) {
		while (litq[--litbound] = *line++) {
			if (litbound < litidx) {
				newerror(44);
				exit(ERRCODE);
			}
		}
		++litbound;				/* remove NULL */
	}
	else {
		prnum();
		sout(line, output);
	}
}

void asmfinish()
{
	if (stagenext)
		litq[--litbound] = 0;
}

/*
** post a label in the program
*/
void postlabel(labelnr, labelstate) int labelnr, labelstate;
{
	if (stagenext)
		outcword(DEFLAB, labelnr);
	else {
		prnum(); printlabel(labelnr);
		if (labelstate == TRUE)
			labelform(FALSE);		/* terminate label */
		else {
#ifdef OTHERASM
			if (outform == M80)
#endif
				outbyte(':');
			inlabel = TRUE;		/* set inlabel state */
		}
	}
}

/*
** post a label, outside staging buffer
** set inlabel state
*/
void setlabel(labelnr) int labelnr;
{
	prnum();
	printlabel(labelnr);
#ifdef OTHERASM
	if (outform == M80)
#endif
		outbyte(':');
	inlabel = TRUE;
}

/*
** print specified number as a label
*/
void printlabel(labelnr) int labelnr;
{
	outstr("CC");
	outdec(labelnr);
}

/*
** point to following object(s)
*/
void point()
{
	int lab;
	deflabel((lab = getlabel()), 0);
	postlabel(lab, FALSE);
}

/*
** generate a constant load in HL
*/
void doconst(val) int val;
{
	outcword(LDHLW, val);
}

/*
** generate a constant load in DE
*/
void do2const(val) int val;
{
	outcword(LDDEW, val);
}

/*
** generate a reference to a string
*/
void strconst(labelnr, val) int labelnr, val;
{
	outclword(LDHLLO, labelnr, val);
}

/*
** generate an address reference in HL
*/
void address(ptr) char *ptr;
{
	outsword(LDHLS, ptr);
}

/*
** fetch object indirect to primary register
*/
void indirect(lval) int lval[];
{
	if (lval[INDTYPE] == CCHAR)
		outcode(CCGCHAR);
	else
		outcode(CCGINT);
}

/*
** fetch a static memory cell into primary register
*/
void getmem(lval)  int lval[];
{
	char *symptr;
	symptr = lval[LSYMPTR];
	if((symptr[IDENT]!=POINTER) && (symptr[TYPE]==CCHAR)) {
		outsword(LDAIS, symptr);
		outcode(CCSXT);
    }
	else {
		outsword(LDHLIS, symptr);
    }
}

/*
** fetch addr of the specified symbol into primary register
*/
void getloc(sym)  char *sym;
{
	doconst(*(iptr = (sym+OFFSET))-csp);
	outcode(ADDHLSP);
}

/*
** store primary register into static cell
*/
void putmem(lval)  int lval[];
{
	char *sym;
	sym = lval[LSYMPTR];
	if ((sym[IDENT]!=POINTER) && (sym[TYPE]==CCHAR)) {
		outcode(LDAL);
		outsword(LDIAS, sym);
	}
	else {
		outsword(LDIHLS, sym);
	}
}

/*
** put on the stack the type object in primary register
*/
void putstk(lval) int lval[];
{
	if(lval[INDTYPE]==CCHAR) {
		outcode(LDAL);
		outcode(LDIDA);
	}
	else
		outcode(CCPINT);
}

/*
** move primary register to secondary
*/
void move()
{
	outcode(LDDH);
	outcode(LDEL);
}

/*
** swap primary and secondary registers
*/
void swap()
{
  outcode(EXDEHL);
}

/*
** push primary register onto stack
*/
void push()
{
  outcode(PUSHHL);
  csp -= BPW;
}

/*
** unpush or pop as required
*/
void smartpop(lval, start) int lval[]; char *start;
{
	if(lval[USEDE])  pop(); 		/* secondary was used */
	else unpush(start);
}

/*
** replace a push with a swap
** Adjust all stack relative instructions following
** the replacement.
*/
void unpush(dest) char *dest;
{
	*dest = EXDEHL;
	cptr = ++dest;				/* check from next int-op on */
	while (cptr < stagenext) {
		if ((*cptr & 255) == ADDHLSP) {
			*(iptr = cptr - BPW) -= BPW; /* point to word constant */
		}
		cptr += intlen(cptr);
	}
	csp += BPW;
}

/*
** pop stack to the secondary register
*/
void pop()
{
	outcode(POPDE);
	csp += BPW;
}

/*
** swap primary register and stack
*/
void swapstk()
{
	outcode(EXSPHL);
}

/*
** process switch statement
*/
void sw()
{
	outcode(CCSWITCH);
}

/*
** call specified subroutine name
*/
void docall(sname)  char *sname;
{
	outsword(FCALL, sname);
}

/*
** return from subroutine
*/
void doret()
{
	outcode(RET);
}

/*
** perform subroutine call to value in HL => call (hl)
*/
void callstk()
{
	outcode(CCDCAL);		/* jp (hl) */
}


/*
** jump to internal label number
*/
void jump(labelnr)  int labelnr;
{
  outcword(JPL, labelnr);
}

/*
** test primary register and jump if false
** first 2 opcodes removed if preceeded by libary call.
** library calls return a TRUE or FALSE result so a FALSE value
** will result in a JMP.
*/
void testjump(labelnr)  int labelnr;
{
	outcode(LDAH);
	outcode(ORL);
	outcword(JPZL, labelnr);
}

/*
** test primary register against zero and jump if false
** removes code that resulted in a constant.
*/
zerojump(oper, llabel, lval) int (*oper)(), llabel, lval[];
{
	stagenext = lval[OPER0];		  /* purge conventional code */
	(*oper)(llabel);
}
	
/*
** modify stack pointer to value given
*/
modstk(newsp, save)  int newsp, save;
{
	int k;
	k=newsp-csp;
	if(k==0)return newsp;
	if(k>=0) {
		if(k<7) {
			if(k&1) {
				outcode(INCSP);
				k--;
			}
			while(k) {
				outcode(POPBC);
				k -= BPW;
			}
			return newsp;
		}
	}
	if(k<0) {
		if(k>-7) {
			if(k&1) {
				outcode(DECSP);
				k++;
			}
			while(k) {
				outcode(PUSHBC);
				k += BPW;
			}
			return newsp;
		}
	}
	if(save) outcode(EXDEHL);
	doconst(k);
	outcode(ADDHLSP);
	outcode(LDSPHL);
	if(save) outcode(EXDEHL);
	return newsp;
}

/*
** double primary register
*/
void doublereg()
{
	outcode(ADDHLHL);
}
