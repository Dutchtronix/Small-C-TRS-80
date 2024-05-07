/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I  V2.1B Dutchtronix 2024 
**
** Part 3
*/

/*
** lval[LSYMPTR] - symbol table address, else 0 for constant
** lval[INDTYPE] - type of indirect obj to fetch, else 0 for static
** lval[PATYPE] - type of pointer or array, else 0 for all other
** lval[CEXPR] - true if constant expression
** lval[CVALUE] - value of constant expression
** lval[USEDE] - true if secondary register altered
** lval[LBINOP] - function address of highest/last binary operator
** lval[OPER0] - stage address of "oper 0" code, else 0
*/


/*
** skim over terms adjoining || and && operators
** TODO Boolean result value must be set in A, followed by AND A. Now in HL.
*/
skim(argid, lval) int argid, lval[];
{
	int k, hits, droplab, endlab;
	hits = 0;
	while (TRUE) {
		k = plung1(argsf2[argid], lval);
		if (nextop(argstrings[argid])) {
			bump(opsize);
			if (hits == 0) {
				hits = 1;
				droplab = getlabel();
			}
			dropout(k, argsf1[argid], droplab, lval);
		}
		else if (hits) {
			dropout(k, argsf1[argid], droplab, lval);
			doconst(argsendval[argid]);
			jump(endlab = getlabel());
			postlabel(droplab, TRUE);
			doconst(argsdropval[argid]);
			postlabel(endlab, TRUE);
			lval[INDTYPE] = lval[PATYPE] = lval[CEXPR] = lval[OPER0] = 0;
			return 0;
		}
		else return k;
	}
}

/*
** test for early dropout from || or && evaluations
*/
void dropout(k, testfunc, exit1, lval) int k, (*testfunc)(), exit1, lval[];
{
  if(k) rvalue(lval);
  else if(lval[CEXPR]) doconst(lval[CVALUE]);
	(*testfunc)(exit1); /* jumps on false */
}

/*
** plunge to a lower level
*/
plunge(argid, lval) int argid, lval[];
{
	int k, lval2[LVALSIZE];
	k = plung1(argsf1[argid], lval);
	if (nextop(argstrings[argid]) == 0) return k;
	if (k) rvalue(lval);
	while (TRUE) {
		if (nextop(argstrings[argid])) {
			bump(opsize);
			opindex += argsoff[argid];
			plung2(op[opindex], op2[opindex], argsf1[argid], lval, lval2);
		}
		else return 0;
	}
}

/*
** unary plunge to lower level
** "plung1" to have
** first 6 chars unique for M80 assembler.
*/
plung1(hier, lval) int (*hier)(), lval[];
{
	char *before, *start;
	int k;
	if (!(before = stagenext)) {
		newerror(49); setstage(&before, &start);
	}
	k = (*hier)(lval);
	if(lval[CEXPR])
		stagenext = before; 	/* purge code, load constant later */
	return k;
}

/*
** binary plunge to lower level
** "plung2" to make first 6 chars unique for M80 assembler.
*/
void plung2(oper, oper2, hier, lval, lval2)
	int (*oper)(), (*oper2)(), (*hier)(), lval[], lval2[];
{
	char *before, *start;
	setstage(&before, &start);	/* value of start needed */
	lval[USEDE]=TRUE;  		    /* flag secondary register used */
	lval[OPER0]=0;    			/* flag as not "... oper 0" syntax */
	if(lval[CEXPR]) { 			/* constant on left side not yet loaded */
		if(plung1(hier, lval2))
			rvalue(lval2);
		if(lval[CVALUE]==0)
			lval[OPER0]=stagenext;
		do2const(lval[CVALUE]<<dbltest(lval2, lval));
	}
	else {              		/* non-constant on left side */
		push();
		if(plung1(hier, lval2))
			rvalue(lval2);
		if(lval2[CEXPR]) {    	/* constant on right side */
			if(lval2[CVALUE]==0)
				lval[OPER0]=start;
			if(oper==zadd) { 	/* may test other commutative operators */
				csp=csp+2;
				stagenext = before;	/* purge code */
				do2const(lval2[CVALUE]<<dbltest(lval, lval2));   /* load secondary */
			}
			else {
				doconst(lval2[CVALUE]<<dbltest(lval, lval2));    /* load primary */
				smartpop(lval2, start);
			}
		}
		else {            		/* non-constants on both sides */
			smartpop(lval2, start);
			if((oper==zadd)|(oper==zsub)) {
				if(dbltest(lval,lval2)) doublereg();
				if(dbltest(lval2,lval)) {
					swap();
					doublereg();
					if(oper==zsub) swap();
				}
			}
		}
	}
	if(oper) {
		if(lval[CEXPR]=lval[CEXPR]&lval2[CEXPR]) {
		  lval[CVALUE]=calc(lval[CVALUE], oper, lval2[CVALUE]);
		  stagenext = before;	/* purge code */  
		  lval[USEDE]=0;
		}
		else {
			if((lval[PATYPE]==0)&&(lval2[PATYPE]==0)) {
				(*oper)();
				lval[LBINOP]= oper;  /* identify the operator */
			}
			else {
				(*oper2)();
				lval[LBINOP]= oper2; /* identify the operator */
			}
		}
		if(oper==zsub) {
			if((lval[PATYPE]==CINT)&(lval2[PATYPE]==CINT)) {
				swap();
				doconst(1);
				zasr();  		/** div by 2 **/
			}
		}
		if((oper==zsub) || (oper==zadd))
			result(lval, lval2); /* determine type of action */
	}
}

/*
** scan line for operator in list
*/
nextop(list) char *list;
{
	char op[4];
	opindex=0;
	blanks();
	while(TRUE) {
		opsize=0;
		while(*list > ' ') op[opsize++] = *list++;
		op[opsize]=0;
		if(opsize=streq(lptr, op))
			if((*(lptr+opsize) != '=')&
				(*(lptr+opsize) != *(lptr+opsize-1)))
				return 1;
		if(*list) {
			++list;
			++opindex;
		}
		else return 0;
	}
}

calc(left, oper, right) int left, (*oper)(), right;
{
       if(oper ==  zor) return (left  |  right);
  else if(oper == zxor) return (left  ^  right);
  else if(oper == zand) return (left  &  right);
  else if(oper ==  zeq) return (left  == right);
  else if(oper ==  zne) return (left  != right);
  else if(oper ==  zle) return (left  <= right);
  else if(oper ==  zge) return (left  >= right);
  else if(oper ==  zlt) return (left  <  right);
  else if(oper ==  zgt) return (left  >  right);
  else if(oper == zasr) return (left  >> right);
  else if(oper == zasl) return (left  << right);
  else if(oper == zadd) return (left  +  right);
  else if(oper == zsub) return (left  -  right);
  else if(oper ==zmult) return (left  *  right);
  else if(oper == zdiv) return (left  /  right);
  else if(oper == zmod) return (left  %  right);
  else return 0;
  }

/*
** Expression parser using recursive calls.
** Unfortunately, each recursive call to expression uses 596 bytes of stack space (on a Z80)
** Complex expressions like func1(func2(func3(a))) use 3 levels of recursion and exhaust the
** stack space quickly.
**
** arguments to skim() and plunge() take a lot of space on the stack but are mostly
** identical between calls to we'll use an index into static arrays to pass as an argument.
*/

expression(pconst, val) int *pconst, *val;  {
	int lval[LVALSIZE];					/* MSC: fields used as numbers and pointers */
	memset(lval, 0, LVALSIZE * BPI);
	if(hier1(lval))
		rvalue(lval);
	if(lval[CEXPR]) {
		*pconst=TRUE;
		*val=lval[CVALUE];
	}
	else *pconst=0;
}

hier1(lval)  int lval[];
{
#ifdef MSC
	int k,lval2[LVALSIZE], (*oper)();
#else
	/*
	** Cannot delcare a local variable to be a function in Small-C
	*/
	int k, lval2[LVALSIZE];
	int oper;
#endif
	k=plung1(hier3, lval);
	if(lval[CEXPR]) doconst(lval[CVALUE]);
	if(match("|="))  oper= zor;
	else if(match("^="))  oper= zxor;
	else if(match("&="))  oper= zand;
	else if(match("+="))  oper= zadd;
	else if(match("-="))  oper= zsub;
	else if(match("*="))  oper= zmult;
	else if(match("/="))  oper= zdiv;
	else if(match("%="))  oper= zmod;
	else if(match(">>=")) oper= zasr;
	else if(match("<<=")) oper= zasl;
	else if(match("="))   oper= 0;
	else return k;
	if(k==0) {
		needlval();
		return 0;
	}
	memset(lval2, 0, LVALSIZE * BPI);
	if(lval[INDTYPE]) {
		if(oper) {
			push();
			rvalue(lval);
		}
		/*
		** unsigned support.
		** TODO this is marked as not working
		*/
		if (oper != zasr)
			plung2(oper, oper, hier1, lval, lval2);
		else
			plung2(zasr, uasr, hier1, lval, lval2);
		if(oper) pop();
	}
	else {
		if(oper) {
			rvalue(lval);
			plung2(oper, oper, hier1, lval, lval2);
		}
		else {
			if(hier1(lval2)) rvalue(lval2);
			lval[USEDE]=lval2[USEDE];
		}
	}
	store(lval);
	return 0;
}

hier3(lval)  int lval[];
{
	return skim(ARG4, lval);
}

hier4(lval)  int lval[];
{
	return skim(ARG5, lval);
}

hier5(lval)  int lval[];
{
	return plunge(ARG6, lval);
}

hier6(lval)  int lval[];
{
	return plunge(ARG7, lval);
}

hier7(lval)  int lval[];
{
	return plunge(ARG8, lval);
}

hier8(lval)  int lval[];
{
	return plunge(ARG9, lval);
}

hier9(lval)  int lval[];
{
	return plunge(ARG10, lval);
}

hier10(lval)  int lval[];
{
	return plunge(ARG11, lval);
}

hier11(lval)  int lval[];
{
	return plunge(ARG12, lval);
}

hier12(lval)  int lval[];
{
	return plunge(ARG13, lval);
}

