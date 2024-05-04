/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I  V2.1B Dutchtronix 2024
**
** Part 4
*/

void prnum()
{
#ifdef OTHERASM
	char *ptr;
	if (outform != M80) {
		for (ptr = linenum; *ptr; ) cout(*ptr++, output);
		ptr = &linenum[4];		/* now increment number */
		while ((++(*ptr) & '\377') == '\272')
			*(ptr--) = '\260';
	}
#endif
}

/*
** add primary and secundary registers (result in primary)
*/
void zadd()
{
	outcode(ADDHLDE);
}

/*
** subtract primary from secondary register (result in primary)
*/
void zsub()
{
	outcode(CCSUB);
}

/*
** multiply primary and secondary register (result in primary)
*/
void zmult()
{
	outcode(CCMULT);
}

/*
** divide secondary by primary register
** (quotient in primary, remainder in secondary)
*/
void zdiv()
{
	outcode(CCDIV);
}

/*
** remainder of secondary div primary
** (remainder in primary, quotient in secondary)
*/
void zmod()
{
	zdiv(); swap();
}

/*
** inclusive "or" primary and secondary registers
** (result in primary)
*/
void zor()
{
	outcode(CCOR);
}

/*
** exclusive "or" primary and secondary registers
** (result in primary)
*/
void zxor()
{
	outcode(CCXOR);
}

/*
** "and" primary and secondary registers
** (result in primary)
*/
void zand()
{
	outcode(CCAND);
}

/*
** logical negation of primary register
*/
void lneg()
{
	outcode(CCLNEG);
}

/*
** arithmetic shift right secondary register
** number of bits given in primary register
** (result in primary)
*/
void zasr()
{
	outcode(CCASR);
}

/*
** arithmetic shift left secondary register
** number of bits given in primary register
** (result in primary)
*/
void zasl()
{
	outcode(CCASL);
}

/*
** two's complement of primary register
*/
void neg()
{
	outcode(CCNEG);
}

/*
** one's complement of primary register
*/
void com()
{
	outcode(CCCOM);
}

/*
** increment primary register by one object of whatever sizeof
*/
void inc(n) int n;
{
	do {
		outcode(INCHL);
	}
	while (--n >= 1);
}

/*
** decrement primary register by one object of whatever sizeof
*/
void dec(n) int n;
{
	do {
		outcode(DECHL);
	}
	while (--n >= 1);
}

/*
** test for equal to
*/
void zeq()
{
	outcode(CCEQ);
}

/*
** test for equal to zero
*/
void eq0(labelnr) int labelnr;
{
	outcode(LDAH);
	outcode(ORL);
	outcword(JPNZL, labelnr);
}

/*
** test for not equal to
*/
void zne()
{
	outcode(CCNE);
}

/*
** test for not equal to zero
*/
void ne0(labelnr) int labelnr;
{
	outcode(LDAH);
	outcode(ORL);
	outcword(JPZL, labelnr);
}

/*
** test for less than (signed)
*/
void zlt()
{
	outcode(CCLT);
}

/*
** test for less than to zero
*/
void lt0(labelnr) int labelnr;
{
	outcode(XORA);
	outcode(ORH);
	outcword(JPPL, labelnr);
}

/*
** test for less than or equal to zero
*/
void zle()
{
	outcode(CCLE);
}

/*
** test for less than or equal to zero
*/
void le0(labelnr) int labelnr;
{
	int lab;
	outcode(LDAH);
	outcode(ORL);
	outcword(JRZL, (lab = getlabel()));
	outcode(XORA);
	outcode(ORH);
	outcword(JPPL, labelnr);
	outcword(DEFLAB, lab);
}

/*
** test for geater than (signed)
*/
void zgt()
{
	outcode(CCGT);
}

/*
** test for greater than to zero
*/
void gt0(labelnr) int labelnr;
{
	outcode(XORA);
	outcode(ORH);
	outcword(JPML, labelnr);
	outcode(ORL);
	outcword(JRZL, labelnr);
}

/*
** test for greater than or equal to (signed)
*/
void zge()
{
	outcode(CCGE);
}

/*
** test for greater than or equal to zero
*/
void ge0(labelnr) int labelnr;
{
	outcode(XORA);
	outcode(ORH);
	outcword(JPML, labelnr);
}

/*
** test for less than (unsigned)
*/
void ult()
{
	outcode(CCULT);
}

/*
** test for less than to zero (unsigned)
*/
void ult0(labelnr) int labelnr;
{
	outcword(JPL, labelnr);
}

/*
** test for less than or equal to (unsigned)
*/
void ule()
{
	outcode(CCULE);
}

/*
** test for greater than (unsigned)
*/
void ugt()
{
	outcode(CCUGT);
}

/*
** test for greater than or equal to (unsigned)
*/
void uge()
{
	outcode(CCUGE);
}

/*
** shift right secondary register unsigned
*/
void uasr()
{
	outcode(CCUASR);
}

/*
** test and replace an int_ops string with wildcards
** wildcard bytes must be in same relative location as
** bytes to be copied.
*/
subst(ptraddress, source, replacement, wildok)
	char* ptraddress, * source, * replacement; int wildok;
{
	int len1, len2;
	char cs;
	cptr = ptraddress;
	cptr2 = source;
	len1 = *replacement;
	while (len1--) {
		cs = *cptr2++;			/* may be wildcard */
		if (wildok && ((cs & 255) == WILD)) {
			++cptr;
		}
		else {
			if (*cptr++ != cs)		/* 	if (*cptr++ != *cptr2++) */
				return FALSE;
		}
	}
	/* match, substitute */
	len1 = len2 = *replacement++;
	cptr2 = replacement + len1;
	for (; len1; len1--) {
		cs = *--cptr2;
		if (wildok && ((cs & 255) == WILD)) {
			--cptr;				/* leave original value in place*/
		}
		else {
			*--cptr = cs;
		}
	}
	return len2;
}

void op_error()
{
	newerror(46);
	exit(ERRCODE);
}

/*
** process int_ops with a symbol table ptr
*/
void symint_op(int_op, syptr) int int_op; char *syptr;
{
	char* lclptr;
#ifdef MSC
	/* syptr is actually an index in a table of pointers */
	lclptr = mapidx2ptr((int16_t)syptr);
#else
	lclptr = syptr;
#endif
	lclptr += NAME;				/* point to name */
	switch (int_op) {
		case LDAIS: {
			outstr("LD\tA,("); symtoass(lclptr); outstr(")"); break;
		}
		case LDIAS: {
			outstr("LD\t("); symtoass(lclptr); outstr("),A"); break;
		}
		case FCALL: {
			outstr("CALL\t"); symtoass(lclptr); break;
		}
		case LDHLIS: {
			outstr("LD\tHL,("); symtoass(lclptr); outstr(")"); break;
		}
		case LDIHLS: {
			outstr("LD\t("); symtoass(lclptr); outstr("),HL"); break;
		}
		case LDHLS: {
			outstr("LD\tHL,"); symtoass(lclptr); break;
		}
		case LDDES: {
			outstr("LD\tDE,"); symtoass(lclptr); break;
		}
		default: op_error();
	}
}

/*
** semi-static variables
*/
/* char peepch, * peepword, * peepptr; */
char peepch, *peepptr;
int16_t peepword, peeplen;

/*
** Actual peephole substitutions
*/
void peepsub(ptr) char* ptr;
{
	int16_t int_op; char x;
	peepptr = ptr;
	while (TRUE) {
		x = *peepptr;
		if (x != 0) {
			int_op = x & 255;
			if (int_op == LDHLW) {
				if (peeplen = subst(peepptr, seq1, rseq1, FALSE)) goto L1;
				if (peeplen = subst(peepptr, seq2, rseq2, FALSE)) goto L1;
				if (peeplen = subst(peepptr, seq3, rseq3, FALSE)) goto L1;
				if (peeplen = subst(peepptr, seq4, rseq4, FALSE)) goto L1;
#ifdef BADPEEP
				if (peeplen = subst(peepptr, seqw2, rseqw2, FALSE)) {
#ifdef MSC
					if (fdebug) printf("seqw2 matched\n");
#endif
					goto L1;
				}
#endif
#ifdef BADPEEP
				if (peeplen = subst(peepptr, seqw7, rseqw7, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw7 matched\n");
#endif
					goto L1;
				}
#endif
				if (peeplen = subst(peepptr, seqw9, rseqw9, FALSE)) {
#ifdef MSC
					if (fdebug) printf("seqw9 matched\n");
#endif
					goto L1;
				}
				if (peeplen = subst(peepptr, seqw10, rseqw10, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw10 matched\n");
#endif
					goto L1;
				}
			}
			else if (int_op == EXDEHL) {
				if (peeplen = subst(peepptr, seqw8, rseqw8, FALSE)) {
#ifdef MSC
					if (fdebug) printf("seqw8 matched\n");
#endif
					goto L1;
				}
			}
			else if (int_op == ADDHLSP) {
				if (peeplen = subst(peepptr, seq5, rseq5, FALSE)) goto L1;
				if (peeplen = subst(peepptr, seq6, rseq6, FALSE)) goto L1;
			}
#ifdef BADPEEP
			else if (int_op == CCGCHAR) {
				if (peeplen = subst(peepptr, seqw1, rseqw1, FALSE)) {
#ifdef MSC
					if (fdebug) printf("seqw1 matched\n");
#endif
					goto L1;
				}
			}
#endif
			else if (int_op == CCEQ) {
				if (peeplen = subst(peepptr, seqw3, rseqw3, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw3 matched\n");
#endif
					goto L1;
				}
				if (peeplen = subst(peepptr, seqw4, rseqw4, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw4 matched\n");
#endif
					goto L1;
				}

			}
			else if (int_op == CCNE) {
				if (peeplen = subst(peepptr, seqw5, rseqw5, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw5 matched\n");
#endif
					goto L1;
				}
				if (peeplen = subst(peepptr, seqw6, rseqw6, TRUE)) {
#ifdef MSC
					if (fdebug) printf("seqw6 matched\n");
#endif
					goto L1;
				}

			}
			peepptr += intlen(peepptr); continue;
		L1:
			peepptr += peeplen;
		}
		else {
			break;
		}
	}
}

/*
** send int_ops in stage buffer to macro file
*/
void peephole(ptr) char *ptr;
{
	int16_t int_op;
	if (output == 0) return;
	peepsub(ptr);
	inlabel = FALSE;
	peepptr = ptr;
	while (int_op = (*peepptr++ & 255)) {	/* zero indicated end of int_ops */
		if (int_op == NOOP) continue;
		if (inlabel) {
			inlabel = FALSE;
			if ((int_op == DEFLAB) || (int_op == LITERAL)) {
				labelform(FALSE); prnum();
			}
			else {
#ifdef OTHERASM
				if (outform == M80)
#endif
					outbyte(':');
				tab();
			}
		}
		else {
			prnum();
			if ((int_op != DEFLAB) && (int_op != LITERAL)) tab();
		}
		if (int_op < NOOP) {	/* first non cclib int */
			outstr("CALL\t");
#ifdef OTHERASM
			if (outform == INSASM) outbyte('&');
#endif
			outstr("CCC");
			outdec(int_op);
#ifdef OTHERASM
			if (outform == M80)
#endif
			outstr("##");
			nl();
			/* Library calls set the flags based on HL so no need to test
			** *peepptr is next opcode.
			*/
			if ((*peepptr & 255) == LDAH)	/* remove flags test */
				peepptr += 2;				/* skip 2 int_ops. */
		}
		else if (int_op < LDAB) {
			ol(intone[int_op - NOOP]);		/* rescale int_op value */
		}
		else if (int_op == LDAB) {
			outstr("LD\tA,"); outdec(*peepptr++ & 255); nl();
		}
		else if (int_op == LDLB) {
			outstr("LD\tL,"); outdec(*peepptr++ & 255); nl();
		}
		else if (int_op == LDHB) {
			outstr("LD\tH,"); outdec(*peepptr++ & 255); nl();
		}
		else if (int_op == LDHLLO) {		/* watch max.offset!! */
			outstr("LD\tHL,"); printlabel(*(iptr = peepptr));
			++iptr;		/* point to offset */
#ifdef OTHERASM
			if ((*iptr > MAXOFFSET) && (outform == INSASM)) {
				nl(); prnum();
				outstr("\tLD\tBC,"); outdec(*iptr); nl();
				prnum(), outstr("\tADD\tHL,BC"); nl();
			}
			else
#endif
			{
				outbyte('+'); outdec(*iptr); nl();
			}
			peepptr += 2 * BPW;
		}
		else {				/* here we always have a word value */
			peepword = *(iptr = peepptr);
			peepptr += BPW;
			if (int_op < LDAIS) {
				if (int_op < LITERAL) {
					outstr(inttwo[int_op-LDHLW]);	/* STOM */
					if (int_op == DEFLWRD)
						printlabel(peepword);
					else
						outdec(peepword);
					nl();
				}
				else if (int_op == LITERAL) {
					while (TRUE) {
						--peepword;
						peepch = *(litq + peepword);
						if (!peepch) break;
						outbyte(peepch);
						if (peepch == '\n') prnum();
					}
					outstr(";\n");					/* for last line# */
				}
				else op_error();
			}
			else if (int_op >= JPL) {
				if (int_op < DEFLAB) {
					outstr(intthree[int_op - JPL]);
					printlabel(peepword); nl();
				}
				else if (int_op == DEFLAB) {
					printlabel(peepword);
					inlabel = TRUE;
				}
				else op_error();
			}
			else {				/* symbol table ptr */
				symint_op(int_op, peepword);
				nl();
			}					/* symtab ptr clause */
		}						/* word value clause */
	}							/* while loop */
}								/* peephole */

			
		