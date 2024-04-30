/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I Mikrorie - Jan de Rie
**
** Part 1
*/

/*
** statement parser
**
** called whenever syntax requires a statement
**  this routine performs that statement
**  and returns a number telling which one
*/
statement() {
	if ((ch==0) & (eofstatus)) return lastst;
	else if(amatch("CHAR",4))  {declloc(CCHAR);ns();}
	else if(amatch("INT",3))   {declloc(CINT);ns();}
	else if (amatch("DOUBLE", 6)) { declloc(CDOUBLE); ns(); }
	else {
		if(declared >= 0) {
			if(ncmp > 1) nogo=declared; /* disable goto if any */
			csp=modstk(csp - declared, NO);
			declared = -1;
		}
		if(match("{"))               compound();
		else if(amatch("IF",2))      {doif();lastst=STIF;}
		else if(amatch("WHILE",5))   {dowhile();lastst=STWHILE;}
		else if(amatch("DO",2))      {dodo();lastst=STDO;}
		else if(amatch("FOR",3))     {dofor();lastst=STFOR;}
		else if(amatch("SWITCH",6))  {doswitch();lastst=STSWITCH;}
		else if(amatch("CASE",4))    {docase();lastst=STCASE;}
		else if(amatch("DEFAULT",7)) {dodefault();lastst=STDEF;}
		else if(amatch("GOTO", 4))   {dogoto(); lastst=STGOTO;}
		else if (dolabel())			 { lastst = STLABEL; } /* 45 */
		else if(amatch("RETURN",6))  {doreturn();ns();lastst=STRETURN;}
		else if(amatch("BREAK",5))   {dobreak();ns();lastst=STBREAK;}
		else if(amatch("CONTINUE",8)){docont();ns();lastst=STCONT;}
		else if(match(";"))          errflag=0;
		/*    else if (match("#asm")) { doasm(); lastst = STASM; } */
		else                         {doexpr();ns();lastst=STEXPR;}
	}
	return lastst;
}

/*
** semicolon enforcer
**
** called whenever syntax requires a semicolon
*/
void ns()
{
	if (!match(";")) newerror(16);
	else errflag=0;
}

void compound()  {
	int savcsp;
	char *savloc;
	savcsp=csp;
	savloc=locptr;
	declared=0;    				/* may now declare local variables */
	++ncmp;        				/* new level open */
	while (match("}")==0) {
		if(eofstatus) {
		  newerror(17);
		  break;
		}
		else statement();     	/* do one */
	}
	--ncmp;                 	/* close current level */
	/* delete local variable space */
	/* this prevents redundant code to be generated */
	if ((lastst != STRETURN) && (lastst != STGOTO))
		modstk(savcsp, NO);
	csp = savcsp;
	cptr=savloc;            	/* retain labels */
	while(cptr < locptr) {
		cptr2=nextsym(cptr);
		if(cptr[IDENT] == LABEL) {
			while(cptr < cptr2) *savloc++ = *cptr++;
		} else {
			cptr=cptr2;
		}
	}

	locptr=savloc;          	/* delete local symbols */
	declared = -1;          	/* may not declare variables */
}

void doif()  {
	int flab1,flab2;
	flab1=getlabel(); 			/* get label for false branch */
	dotest(flab1, YES); 			/* get expression, and branch false */
	statement();     			/* if true, do a statement */
	if (amatch("ELSE",4)==0) {  /* if...else ? */
		/* simple "if"...print false label */
		postlabel(flab1, TRUE);
		return;         		/* and exit */
	}
	flab2=getlabel();
	if((lastst != STRETURN)&&(lastst != STGOTO)) jump(flab2);
	postlabel(flab1, TRUE); 	/* print false label */
	statement();      			/* and do "else" clause */
	postlabel(flab2, TRUE);		/* print true label */
}

/*
** 
*/
void doexpr() {
	int iconst, val;
	do {
		expression(&iconst, &val);
	} while (match(","));
}

void dowhile()
{
	int wq[4];              	/* allocate local queue */
	addwhile(wq);           	/* add entry to queue for "break" */
	postlabel(wq[WQLOOP], TRUE); /* loop label */
	dotest(wq[WQEXIT], YES);  	/* see if true */
	statement();            	/* if so, do a statement */
	jump(wq[WQLOOP]);       	/* loop to label */
	postlabel(wq[WQEXIT], TRUE); /* exit label */
	delwhile();             	/* delete queue entry */
}

void dodo() {
	int wq[4], top;
	addwhile(wq);
	postlabel(top=getlabel(), TRUE);
	statement();
	needtoken("WHILE");
	postlabel(wq[WQLOOP], TRUE);
	dotest(wq[WQEXIT], YES);
	jump(top);
	postlabel(wq[WQEXIT], TRUE);
	delwhile();
	ns();
}

void dofor() {
	int wq[4], lab1, lab2;
	addwhile(wq);
	lab1=getlabel();
	lab2=getlabel();
	needtoken("(");
	if(!match(";")) {
		doexpr();           	 /* expr 1 */
		ns();
	}
	postlabel(lab1), TRUE;
	if(!match(";")) {
		dotest(wq[WQEXIT], NO); 	/* expr 2 */
		ns();
	}
	jump(lab2);
	postlabel(wq[WQLOOP]), TRUE;
	if(!match(")")) {
		doexpr();            	/* expr 3 */
		needtoken(")");
	}
	jump(lab1);
	postlabel(lab2, TRUE);
	statement();
	jump(wq[WQLOOP]);
	postlabel(wq[WQEXIT], TRUE);
	delwhile();
}

void doswitch() {
	int wq[4], endlab, swact, swdef, *swnex, *swptr;
	swact=swactive;
	swdef=swdefault;
	swnex=swptr=swnext;
	addwhile(wq);
	*(wqptr + WQLOOP - WQSIZ) = 0;
	needtoken("(");
	doexpr();      				/* evaluate switch expression */
	needtoken(")");
	swdefault=0;
	swactive=1;
	jump(endlab=getlabel());
	statement();   				/* cases, etc. */
	jump(wq[WQEXIT]);
	postlabel(endlab, TRUE);
	sw();          				/* match cases */
	swtable(swptr, swnext);		/* generate switch table */
	if(swdefault) jump(swdefault);
	postlabel(wq[WQEXIT], TRUE);
	delwhile();
	swnext=swnex;
	swdefault=swdef;
	swactive=swact;
}

void docase() {
	if(swactive==0) newerror(18);
	if(swnext > swend) {
		newerror(19);
		return;
	}
	postlabel(*swnext++ = getlabel(), TRUE);
	constexpr(swnext++);
	needtoken(":");
}

void dodefault() {
	if(swactive) {
		if(swdefault) newerror(20);
	}
	else newerror(18);
	needtoken(":");
	postlabel(swdefault=getlabel(), TRUE);
}

void dogoto() {
	if(nogo > 0) newerror(22);
	else noloc = 1;
	if(symname(ssname, YES)) jump(addlabel());
	else newerror(23);
	ns();
}

dolabel() {
	char *savelptr;
	blanks();
	savelptr=lptr;
	if(symname(ssname, YES)) {
		if(gch()==':') {
			postlabel(addlabel(), TRUE);
			return 1;
		}
		else bump(savelptr-lptr);
	}
	return 0;
}

addlabel()  {
	if(cptr=findloc(ssname)) {
		if(cptr[IDENT]!=LABEL) newerror(24);
	}
	else cptr=addsym(ssname, LABEL, LABEL, getlabel(), &locptr, LABEL);
	return *(iptr = &cptr[OFFSET]);
}


void doreturn()  {
	if(endst()==0) {
		doexpr();
		modstk(0, YES);
	}
	else modstk(0, NO);
	doret();
}

void dobreak()  {
	int *ptr;
	if ((ptr=readwhile(wqptr))==0) return; /* no loops open */
	modstk((ptr[WQSP]), NO);	/* clean up stk ptr */
	jump(ptr[WQEXIT]);  		/* jump to exit label */
}

void docont()  {
	int *ptr;
	ptr = wqptr;
	while (TRUE) {
		if ((ptr=readwhile(ptr))==0) return; /* no loops open */
		if (ptr[WQLOOP]) break;
	}
	modstk((ptr[WQSP]), NO);	/* clean up stk ptr */
	jump(ptr[WQLOOP]);			/* jump to loop label */
}

void addwhile(ptr)  int ptr[]; {
	int k;
	ptr[WQSP]=csp;           	/* and stk ptr */
	ptr[WQLOOP]=getlabel();  	/* and looping label */
	ptr[WQEXIT]=getlabel();   	/* and exit label */
	if (wqptr==WQMAX) {
		newerror(24);
		exit(ERRCODE);
	}
	k=0;
	while (k<WQSIZ) *wqptr++ = ptr[k++];
}

void delwhile() {
	if(wqptr > wq) wqptr -= WQSIZ;
}

readwhile(ptr)  int *ptr; {
	if (ptr <= wq) {
		newerror(26);
		return 0;
	}
	else return (ptr-WQSIZ);
}
