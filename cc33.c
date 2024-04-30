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
** true if val1 -> int pointer or int array and val2 not ptr or array
** val1 and val2 are lval arrays 
*/
dbltest(val1,val2) int val1[], val2[];
{
	if(val1[PATYPE]!=CINT) return 0;
	if(val2[PATYPE]) return 0;
	return 1;
}

/*
** determine type of binary operation
*/
void result(lval, lval2) int lval[], lval2[];
{
	if((lval[PATYPE]!=0)&(lval2[PATYPE]!=0)) {
		lval[PATYPE]=0;
	}
	else if(lval2[PATYPE]) {
		lval[LSYMPTR]=lval2[LSYMPTR];
		lval[INDTYPE]=lval2[INDTYPE];
		lval[PATYPE]=lval2[PATYPE];
    }
}

void step(oper, lval) int (*oper)(), lval[];
{
	if(lval[INDTYPE]) {
		if(lval[USEDE]) {
			push();
			rvalue(lval);
			(*oper)(lval[PATYPE] >> 2);
			pop();
			store(lval);
			return;
		}
		else {
			move();
			lval[USEDE]=1;
		}
	}
	rvalue(lval);
	(*oper)(lval[PATYPE] >> 2);
	store(lval);
}

void store(lval)  int lval[];
{
  if(lval[INDTYPE]) putstk(lval);
  else        putmem(lval);
}

void rvalue(lval) int lval[];
{
  if ((lval[LSYMPTR]!=0)&&(lval[INDTYPE]==0))
	  getmem(lval);
  else
	  indirect(lval);
}

void dotest(labelnr, parens)  int labelnr, parens;
{
	int lval[LVALSIZE];
	char *before;
#ifdef MSC
	int (*oper)();
#else
	int oper;			/* function address */
#endif

	memset(lval, 0, LVALSIZE * BPI);
	if(parens) needtoken("(");
	do {						/* staging buffer control in statement */	
		before = stagenext;
		if(hier1(lval))
			rvalue(lval);
	}
	while (match(","));
	if (parens)
		needtoken(")");
	if (lval[CEXPR]) {  				/* constant expression */
		stagenext = before;				/* remove generated code */
		if(lval[CVALUE]) return;
		jump(labelnr);
		return;
	}
	if (lval[OPER0]) {  				/* stage address of "oper 0" code */
		oper=lval[LBINOP];				/* operator function address */
		if((oper==zeq) || (oper==ule))
			zerojump(eq0, labelnr, lval);
		else if((oper==zne)|(oper==ugt))
			zerojump(ne0, labelnr, lval);
		else if (oper==zgt)
			zerojump(gt0, labelnr, lval);
		else if (oper==zge)
			zerojump(ge0, labelnr, lval);
		else if (oper==uge)
			stagenext = lval[OPER0];	 /* purge oper 0 code */
		else if (oper==zlt)
			zerojump(lt0, labelnr, lval);
		else if (oper==ult)
			zerojump(ult0, labelnr, lval);
		else if (oper==zle)
			zerojump(le0, labelnr, lval);
		else
			testjump(labelnr);
	}
	else {
		testjump(labelnr);
	}
}

constexpr(val) int *val;
{
	int iconst;
	char *before, *start;
	setstage(&before, &start);
	expression(&iconst, val);
	stagenext = before; 				/* scratch generated code */
	if(iconst==0)
		newerror(43);
	return iconst;
}

constant(lval)  int lval[];
{
	int iret;
	iret = TRUE;						/* assume return value == 1 */
	lval=lval+3;						/* point to &lval[CEXPR] */
	*lval=1;       						/* assume it will be a constant */
	if (number(++lval)) doconst(*lval);	/* now point to &lval[CVALUE] */
	else if (pstr(lval)) doconst(*lval);/* single quote constant */
	else if (qstr(lval)) {				/* string constant */
		*(lval-1)=0; 					/* nope, it's a string address: lval[CEXPR] */
		strconst(litlab, *lval);
	}
	else iret = FALSE;					/* not a constant */
	return iret;
}

number(val)  int val[];
{
	int minus, radix, k, digit;
	k=minus=0;
	radix = 10;
	if(match("+")) ;
	else if(match("-")) minus=TRUE;
	if(numeric(ch)==0) return 0;
	if (match("0")) {
		if (match("X")) radix = 16;
		else radix = 8;
	}
	while (TRUE) {
		digit = upper(ch) - '0';
		if (digit >= 10)
			if (digit < 17) digit = -1;
			else digit -= 7;
		if ((digit >= 0) && (digit < radix)) {
			gch();
			k = k * radix + digit;
		}
		else break;
	}
	if (minus) k=(-k);
	val[0]=k;
	return TRUE;
}



pstr(val)  int *val;
{
	int k;
	k=0;
	if (!match("'")) return FALSE;
	while(ch!=39)
		k= ((k&255) << 8)  + (litchar()&255);
	++lptr;
	*val = k;
	return TRUE;
}

qstr(val)  int *val;
{
	if (!match(quote)) return FALSE;
	*val=litidx;
	while (ch!='"') {
		if(ch==0) break;
		stowlit(litchar(), 1);
	}
	gch();
	litq[litidx++]=0;
	return TRUE;
}

void stowlit(value, size) int value, size;
{
	if((litidx+size) >= litbound) {
		newerror(44);
		exit(ERRCODE);
    }
	putint(value, litq+litidx, size);
	litidx=litidx+size;
}

/*
** return current literal char & bump lptr
*/
litchar()
{
	int i, oct;
	if((ch!=escape) || (nch==0)) return gch();
	gch();
	if(upper(ch)=='N') {gch(); return 13;} /* CR */
	if(upper(ch)=='T') {gch(); return  9;} /* HT */
	if(upper(ch)=='B') {gch(); return  8;} /* BS */
	if(upper(ch)=='F') {gch(); return 12;} /* FF */
	i=3; oct=0;
	while(((i--)>0) && (ch>='0') && (ch<='7'))
		oct=(oct<<3)+gch()-'0';
	if(i==2)
		return gch();
	else
		return oct;
}
