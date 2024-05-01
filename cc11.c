/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I V2.1B Dutchtronix 2024
**
** Part 1
*/

/*
** dump the literal pool.
** litidx is number of bytes to dump
** TODO size arg unused.
*/
dumplits(size) int size; {
	char *highlim;
	/* litidx is index to first free */
	int i, k, l;
	highlim = litq + litidx;
	cptr = litq;
	k = litidx / 8;
	l = litidx & 7;
#ifdef MSC
	ASSERT(!stagenext, 2);
#endif
	while (k > 0) {
		outstr("\tDEFB\t");
		for (i = 0; i < 8; ++i) {
			outBhex(*cptr++);
			if (i < 7) outbyte(',');
		}
		--k;
		nl();
#ifdef MSC
		ASSERT(cptr <= highlim, 3);
#endif
	}
	if (l > 0) {
		outstr("\tDEFB\t");
		for (i = 0; i < l; ++i) {
			outBhex(*cptr++);
			if (i < (l - 1)) outbyte(',');
		}
		nl();
#ifdef MSC
		ASSERT(cptr <= highlim, 3);
#endif
	}
#ifdef MSC
	ASSERT(cptr == highlim, 3);
#endif
}

/*
** dump zeroes for default initial values
** TODO BUGFIXES coount can be < 0, not just -1
*/
void dumpzero(size, count) int size, count; {
	int i,k,l;
	int curcount;
	curcount = 0;
#ifndef BUGFIXES
	if (count == -1) return;
#endif
	if (size == 2)				/* count in words */
		count <<= 1;			/* make byte count */
	if (count > maxzero) {
		curcount = count - maxzero;
		count = maxzero;
	}
	k = count >> 3;
	l = count & 7;
	if (inlabel)
		inlabel = FALSE;
	else
		prnum();
	while (k > 0) {
		outstr("\tDEFB\t0,0,0,0,0,0,0,0");
		nl();
		--k;
	}
	if (l > 0) {
		outstr("\tDEFB\t");
		for (i = 0; i < l; ++i) {
			outbyte('0');
			if (i < (l - 1)) outbyte(',');
		}
		nl();
	}
	if (curcount) {
		dodefs(curcount);
	}
}

/*
** verify compile ends outside any function
*/
outside()  {
  if (ncmp) newerror(1);
}

/*
** test for global declarations
*/
dodeclare(varclass) int varclass; {
	if(amatch("CHAR",4)) {
		declglb(CCHAR, varclass);
		ns();
		return 1;
    }
	else if (amatch("DOUBLE", 6)) {
		declglb(CDOUBLE, varclass);
		ns();
		return 1;
	} else if((amatch("INT",3)) || (varclass==EXTERNAL)) {
    declglb(CINT, varclass);
    ns();
    return 1;
    }
  return 0;
  }

/*
** declare a static variable
*/
void declglb(vartype, varclass)  int vartype, varclass; {
	int k, j, vtyp2;
	vtyp2 = vartype;						/* original vartype */
	do {
		if(endst()) return;     		/* do line */
		/*
		** bug fix marked 3: add <|| match("(*")>
		*/
		if(match("*")) {
		  j=POINTER;
		  k=0;			/* TODO BUGFIXES 0 dimension to catch uninitialized pointers */
#ifdef XBUGFIXES
		  k = 1;		/* Failure when no initializers */
#endif
		} else {
		  j=VARIABLE;
		  k=1;
		}
#ifdef BUGFIXES
		if (symname(ssname)==0) illname();
#else
		if (symname(ssname, TRUE) == 0) illname();
#endif
		if(findglb(ssname)) multidef(ssname);
		if (match(")"));				/* 03 */
		if(match("()")) j=FUNCTION;
		if (vtyp2 == CDOUBLE) {
			j = ARRAY;
			k = BPD;
			vartype = CCHAR;
		}
		else if (match("[") || match("(.")) {
			k=needsub();    			/* get size */
			if (j==POINTER) {			/* array of pointers */
				j = PTRARRAY; vartype = CINT;
			} else {
				j=ARRAY;				/* !0=array */
			}
		}
		if(varclass==EXTERNAL) {
			doextern(ssname);			/* should be postponed */
			if (j == PTRARRAY) j = ARRAY;
		} else if (j != FUNCTION) {
			entry(ssname, FALSE);
			j=initials(vtyp2 >>2, j, k);	/* returns known ident */
		}
		addsym(ssname, j, vartype, k, &glbptr, varclass);
	} while (match(","));					/* loop if more */
}

/*
** declare local variables
*/
void declloc(vartype)  int vartype;  {
	int k,j;
	int vtyp2;
	vtyp2 = vartype;						/* original vartype */

	if (swactive) {
		newerror(50);
	}

	if(noloc)
		newerror(3);

	if(declared < 0)
		newerror(4);
	do {
		if(endst()) return;
		if(match("*")) j=POINTER;
		else j=VARIABLE;
#ifdef BUGFIXES
		if (symname(ssname)==0) illname();
#else
		if (symname(ssname, TRUE) == 0) illname();
#endif
		/* no multidef check, block-locals are together */
		k=BPW;
		if (vtyp2 == CDOUBLE) {
			j = ARRAY;
			k = BPD;
			vartype = CCHAR;
		}
		if (match("[") || match("(.")) {
			if (k = needsub()) {
				j = ARRAY;
				if (vartype==CINT) k=k<<LBPW;
			} else {
				j = POINTER;
				k = BPW;
			}
		}
		else if(match("()")) j=FUNCTION;
		else if((vartype==CCHAR)&(j==VARIABLE))
			k=SBPC;
		declared += k;
		addsym(ssname, j, vartype, csp - declared, &locptr, AUTOMATIC);
	}
	while (match(","));
}

/*
** initialize global objects
*/
initials(size, varident, dim) int size, varident, dim; {
	int savedim, i;
	int16_t refs[256];
	memset(refs, -1, 256 * BPW);
	iptr2 = refs; litidx=0; litbound = LITABSZ;
	if(dim==0)
		dim = -1;				/* probably intended for [] case. Also was POINTER, unclear why */
	savedim=dim;

	if(match("=")) {
		if (match("{")) {
#ifdef MSC
			ASSERT(iptr2 == refs, 1);
#endif
			i = 0;
			while(dim) {
				if (iptr2 < &refs[256]) {
					*iptr2++ = litidx;
					++i;
				}
				else
					newerror(5);
				/* BUGFIXES init() calls expression() which may clobber iptr. Use iptr2 */
				init(size, varident, &dim);		/* TODO BUGFIXES dim not correctly updated */
#ifdef MSC
				ASSERT(iptr2 == &refs[i], 1);
#endif
				if(match(",")==0) break;
			}
			needtoken("}");
		}
		else init(size, varident, &dim);
	}
	/* dim may be < -1 now if initializers for unsized arrays present */
	if (varident == PTRARRAY) {
		int lab, dim2;
		if ((dim2 = savedim) == dim) dim = 0;	/* no initializers */
		if (dim2 < (iptr2-refs)) dim2 = iptr2-refs;
		iptr2 = refs; lab = getlabel();
		while (dim2--) {
			if (*iptr2==-1) dodefw(0);
			else deflabel(lab, *iptr2);
			++iptr2;
		}
		postlabel(lab, FALSE); varident = ARRAY;
	}
	if((dim == -1) && (dim==savedim)) {
		stowlit(0, size=BPW);
		varident=POINTER;
	}
	dumplits(size);
#ifdef BUGFIXES
	if (dim > 0) {
		dumpzero(size, dim);
	}
#ifdef MSC
	if (dim < -1) {
		if (fdebug) {
			printf("dumpzero count: %d\n", dim);
		}
	}
#endif
#else
	dumpzero(size, dim);	/* TODO BUGFIX issues when dim < -1 */
#endif
	return varident;
}

/*
** evaluate one initializer
** *dim is modified if there is an initializer, even when initial
** value was -1, causing negative values. 
*/
void init(size, varident, dim) int size, varident, *dim; {
	int value;
	if(qstr(&value)) {
#ifdef XBUGFIXES
		if ((varident == ARRAY) && (size == 2)) {
			/* array of ints. Allow pointer to string init
			** currently impossible because the string chars and
			** integer values end up mixed in the literal pool.
			*/
			point();
			*dim -= 1;
		}
		else
#endif
		{
			if ((varident == VARIABLE) | (size != 1))
				newerror(6);
#ifdef BUGFIXES
			/* BUGFIX */
			if ((varident == ARRAY) || (varident == PTRARRAY) || (varident == POINTER)) {
				*dim -= 1;
			}
			else {
				*dim -= (litidx - value);
			}
#else
			{
				*dim -= (litidx - value);
			}
#endif
			if (varident == POINTER) point();
		}
	}
	else if(constexpr(&value)) {
		if ((varident == POINTER) && (value != 0)) {
			newerror(7);
		}
		stowlit(value, size);
		*dim -= 1;
	} else {
		newerror(43);
		*dim = 0;
	}
}

/*
** get required array size
*/
needsub()  {
	int val;
	if(match("]") || match(".)"))
		return 0; /* null size */
	if (constexpr(&val)==0) val=1;
	if (val<0) {
		newerror(8);
		val = -val;
	}
	if ((match("]")==0) && (match(".)") == 0))
		newerror(9);		/* force single dimension */
	return val;          /* and return size */
}

/*
** begin a function
**
** called from "parse" and tries to make a function
** out of the following text
**
** Patched per P.L. Woods (DDJ #52)
*/
void newfunc()  {
	char *before, *start, *ptr;
	int argstk;
	nogo  =             		/* enable goto statements */
	noloc = 0;          		/* enable block-local declarations */
	lastst=             		/* no statement yet */
	litidx=0;           		/* clear lit pool */
	litbound = LITABSZ;			/* reset boundary */
	litlab=getlabel();  		/* label next lit pool */
	locptr=STARTLOC;    		/* clear local variables */
	if (headers) {
		ptr = mline;
		while (numeric(*ptr++ & 127)) ;
		--ptr;
		sout(ptr, stderr);
	}
#ifdef BUGFIXES
	if (symname(ssname)==0)
#else
	if (symname(ssname, TRUE) == 0)
#endif
	{
		newerror(10);
		kill();					/* invalidate line */
		return;
	}
	if(ptr=findglb(ssname)) {   /* already in symbol table ? */
		if(ptr[IDENT]!=FUNCTION)       multidef(ssname);
		else
			if(ptr[OFFSET]==FUNCTION) multidef(ssname);
		else {
			/* BUGFIX 37 */
			ptr[OFFSET] = FUNCTION; /*  earlier assumed to be a function */
			ptr[CLASS] = STATIC;
		}
	} else
		addsym(ssname, FUNCTION, CINT, FUNCTION, &glbptr, STATIC);
	if(match("(")==0) newerror(11);
	entry(ssname, TRUE);
	argstk=2;               	/* stack offsets */
	while(!match(")")) {		/* then count args */
		/* any legal name bumps arg count */
#ifdef BUGFIXES
		if(symname(ssname))
#else
		if (symname(ssname, TRUE))
#endif
		{
			if(findloc(ssname)) multidef(ssname);
			else {
				addsym(ssname, VARIABLE, CINT, argstk, &locptr, AUTOMATIC);
				argstk += BPW;
			}
		} else {
			newerror(12);
			junk();
		}
		blanks();
		/* if not closing paren, should be comma */
		if(streq(lptr,")")==0) {
			if(match(",")==0) newerror(13);
		}
		if(endst()) break;
	}
	csp=0;       				/* preset stack ptr */
	while (TRUE) {
		/* now let user declare what types of things */
		/*      those arguments were */
		if(amatch("CHAR",4))     {doargs(CCHAR);ns();}
		else if(amatch("INT",3)) {doargs(CINT);ns();}
		else if (amatch("DOUBLE", 6)) { doargs(CDOUBLE); ns();}
		else break;
	}
	/*
	** this ensures that all code of a function
	** is buffered in the staging buffer
	*/
	setstage(&before, &start);
	statement();			/* 45 */
	if ((lastst != STRETURN) && (lastst != STGOTO)) {
		doret();
	}
	clearstage(before, start);
	if(litidx) {
		postlabel(litlab, FALSE);
		dumplits(1);			/* dump literals */
    }
}

/*
** declare argument types
**
** called from "newfunc" this routine adds an entry in the
** local symbol table for each named argument
**
** rewritten per P.L. Woods (DDJ #52)
*/
void doargs(t) int t; {
	int j, legalname;
	char *argptr;
	do {
		if (match("*") || match("(*")) j = POINTER;
		else j=VARIABLE;
#ifdef BUGFIXES
		if((legalname=symname(ssname))==0) illname();
#else
		if ((legalname = symname(ssname, TRUE)) == 0) illname();
#endif
		/* is it a pointer? */
		if (match(")")) ;
		if (match("()")) ;
		if (match("[") || match("(.")) {
			  /* yes, so skip stuff between "[...]" */
			while((match("]") == 0) && match(".)") == 0) {
				  bump(1);		/* to next char */
				  if(endst()) break;
			}
			if (j == VARIABLE) j = POINTER; /* add entry as pointer */
			else t = CINT;		/* *argv[] probably. make ptr to int */
		}
		if(legalname) {
			if(argptr=findloc(ssname)) {
				/* add details of type and address */
				argptr[IDENT]=j;
				argptr[TYPE]=t;
			}
			else newerror(15);
		}
	}
	while (match(","));
}

