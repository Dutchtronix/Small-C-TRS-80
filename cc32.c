/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I  V2.1B Dutchtronix 2024
**
** Part 3
*/

hier13(lval)  int lval[];
{
	int k;
	char *symptr;
	if(match("++")) {                   /* ++lval */
		if(hier13(lval)==0) {
			needlval();
			return 0;
		}
		step(inc, lval);
		return 0;
	}
	else if(match("--")) {              /* --lval */
		if(hier13(lval)==0) {
			needlval();
			return 0;
		}
		step(dec, lval);
		return 0;
	}
	else if (match("~")) {              /* ~ */
		if(hier13(lval)) rvalue(lval);
		com();
		lval[CVALUE] = ~lval[CVALUE];
		return 0;
	}
	else if (match("!")) {              /* ! */
		if(hier13(lval)) rvalue(lval);
		lneg();
		lval[CVALUE] = !lval[CVALUE];
		return 0;
	}
	else if (match("-")) {              /* unary - */
		if(hier13(lval)) rvalue(lval);
		neg();
		lval[CVALUE] = -lval[CVALUE];
		return 0;
	}
	else if(match("*")) {               /* unary * */
		if(hier13(lval))
			rvalue(lval);
		if(symptr=lval[LSYMPTR]) lval[INDTYPE]=symptr[TYPE];
		else lval[INDTYPE]=CINT;
		lval[PATYPE]=0;  				/* flag as not pointer or array */
		lval[CEXPR]=0;  				/* flag as not constant */
		return 1;
	}
	else if (match("&")) {				/* unary & */
		if (hier13(lval) == 0) {
			needlval();
			return 0;
		}
		symptr = lval[LSYMPTR];
		lval[PATYPE] = symptr[TYPE];
		if (lval[INDTYPE]) return 0;
		/* global & non-array */
		address(symptr);
		lval[INDTYPE] = symptr[TYPE];
		return 0;
	}
	else {
		k = hier14(lval);
		if (match("++")) {		/* lval++ */
			if (k == 0) {
				needlval();
				return 0;
			}
			step(inc, lval);
			dec(lval[PATYPE] >> 2);
			return 0;
		}
		else if (match("--")) {            /* lval-- */
			if (k == 0) {
				needlval();
				return 0;
			}
			step(dec, lval);
			inc(lval[PATYPE] >> 2);
			return 0;
		}
		else return k;
	}
}

hier14(lval)  int lval[];
{
	int k, lval2[LVALSIZE];
	char* symptr, * before;
	k = primary(lval);
	symptr = lval[LSYMPTR];
	blanks();
	if ((ch == '[') | (ch == '(')) {
		lval[USEDE] = TRUE;			/* secondary register will be used */
		while (TRUE) {
			if (match("[") || match("(.")) { /* [subscript] */
				if (symptr == 0) {
					newerror(41);
					junk();
					if ((match("]") == 0) && (match(".)") == 0))
						needtoken("]");
					return 0;
				}
				else if (symptr[IDENT] == POINTER) rvalue(lval);
				else if (symptr[IDENT] != ARRAY) {
					newerror(41);
					k = 0;
				}
				before = stagenext;
				lval2[CEXPR] = 0;
				plung2(0, 0, hier1, lval2, lval2); /* lval2 deadend */
				if ((match("]") == 0) && (match(".)") == 0))
					needtoken("]");
				if (lval2[CEXPR]) {
					stagenext = before;	/* purge code */
					if (lval2[CVALUE]) {
						if (symptr[TYPE] == CINT)
							do2const(lval2[CVALUE] << LBPW);
						else
							do2const(lval2[CVALUE]);
						zadd();
					}
				}
				else {
					if (symptr[TYPE] == CINT) doublereg();
					zadd();
				}
				/* Listing shows bug fix 15 */
				lval[PATYPE] = 0;
				lval[INDTYPE] = symptr[TYPE];
				k = 1;
			}
			else if (match("(")) {           	/* function(...) */
#ifndef XBUGFIXES
				if (symptr == 0) callfunction(0);		/* non-standard use */
				else
#endif
					if (symptr[IDENT] != FUNCTION) {
						/* listing shows bug fix 13,14 */
						if (k && (symptr[IDENT] == POINTER) && (lval[PATYPE] != 0)) {
							rvalue(lval);
						}
						else if (k && (symptr[IDENT] == VARIABLE)) {
							rvalue(lval);
						}
					callfunction(0);
				}
				else callfunction(symptr);
				k=lval[LSYMPTR]=lval[CEXPR]=0;
			}
			else return k;
		}
	}
	if(symptr==0) return k;
	if(symptr[IDENT]==FUNCTION) {
		address(symptr);
		lval[LSYMPTR] = 0;
		return 0;
	}
	return k;
}

primary(lval)  int *lval;
{
	char *symptr;
	char sname[NAMESIZE];

	int k;
	if(match("(")) {            /* (expression) */
		/*
		** Also catches (*func)() which causes an extra indirection.
		** need to mark this indirection so it can be undone when
		** we realize this is a ptr to function call.
		*/
		do {
			k = hier1(lval);	/* lval reused but not reinitialized */
		} while (match(","));
		needtoken(")");
		return k;
	}
	memset(lval, 0, LVALSIZE*BPI);
	if (symname(sname, YES)) {
		if (symptr = findloc(sname)) {
			if(symptr[IDENT]==LABEL) {
				experr();
				return 0;
			}
			getloc(symptr);
			lval[LSYMPTR]=symptr;	/* MSC size mismatch if int size != ptrsize */
			lval[INDTYPE]=symptr[TYPE];
			if(symptr[IDENT]==POINTER) {
				lval[INDTYPE]=CINT;
				lval[PATYPE]=symptr[TYPE];
			}
			if(symptr[IDENT]==ARRAY) {
				lval[PATYPE]=symptr[TYPE];
				return 0;
			}
			else return TRUE;
		}
		if (symptr = findglb(sname))
			if(symptr[IDENT]!=FUNCTION) {
				lval[LSYMPTR]=symptr;
				lval[INDTYPE]=0;
				if(symptr[IDENT]!=ARRAY) {
					if(symptr[IDENT]==POINTER)
						lval[PATYPE]=symptr[TYPE];
					return 1;
				}
				address(symptr);
				lval[INDTYPE]=lval[PATYPE]=symptr[TYPE];
				return 0;
			}
		symptr=addsym(sname, FUNCTION, CINT, 0, &glbptr, AUTOEXT);
		lval[LSYMPTR]=symptr;
		lval[INDTYPE]=0;
		return 0;
    }
	if(constant(lval)==0) experr();
	return 0;
}

void experr()
{
	newerror(42);
	doconst(0);
	junk();
}

/*
** call a C function. Argumnts as K&R C
*/
void callfunction(symptr)
char *symptr;	/* symbol table entry or 0 (HL is function ptr in that case */
{
	int nargs, iconst, val, curcsp, newcsp, argnum;
	char *start, *cend, *last;
	char* argptr;

	int16_t i, ilast;
	nargs=0;
	blanks();             		/* already saw open paren */
	/* 36 */
	start = stagenext;			/* address start of int_ops */
	ilast = -1;					/* end parameters chain */
								/* chain references are relative */
	while(streq(lptr,")")==0) {
		if(endst()) break;
		iptr = stagenext;
		*iptr = ilast;			/* int16 */
		i = start;
		ilast = stagenext - i;
		stagenext += BPW;
		if (symptr) {
			expression(&iconst, &val);
			push();
		}
		else {
			push();
			expression(&iconst, &val);
			swapstk();
		}
		++nargs;				/* count argument */
		if (match(",")==0) break;
	}
	needtoken(")");
	if (nargs == 1) {			/* NULL ptr in code; replace with nop */
		*start++ = NOOP;
		*start = NOOP;
	}
	else if (nargs > 1) {		/* reverse order of args on stack */
		cptr2 = start;
		i = stagenext - start;
		argptr = cptr = ccalloc(i);
		while (cptr2 < stagenext) *cptr++ = *cptr2++;	/* memcpy from start to argptr, length i */
		stagenext = start;		/* rebuild argument list */
		curcsp = 0; argnum = nargs;
		cend = cptr;
		do {
			i = ilast;
			last = argptr + i;	/* make absolute reference */
			newcsp = (curcsp++ - (--argnum)) << LBPW;
			cptr = last + BPW;
			while (cptr < cend) { /* adjust stack references */
				if ((*cptr & 255) == ADDHLSP) {
					iptr = cptr - BPW;
					*iptr += newcsp;
				}
				cptr += intlen(cptr);
			}
			cptr = last + BPW;
			while (cptr < cend) *stagenext++ = *cptr++;
			cend = last;
			iptr = last;
			ilast = *iptr;
		}
		while (ilast != -1) ;
		ccfree(argptr);
	}
	if (symptr) docall(symptr);
	else callstk();
	
	csp=modstk(csp+(nargs << 1), YES);
}
