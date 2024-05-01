/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I  V2.1B Dutchtronix 2024
**
** Part 2
*/

findglb(sname)  char *sname; {
	if(gsearch(sname))
		return symptr;
	return 0;
}

findloc(sname)  char *sname;  {
	cptr = locptr - 1;  /* search backward for block locals */
	while(cptr > STARTLOC) {
		cptr = cptr - *cptr;
		if(astreq(sname, cptr, NAMEMAX)) return (cptr - NAME);
		cptr = cptr - NAME - 1;
	}
	return 0;
}

addsym(sname, id, typ, value, lgptrptr, varclass)
  char *sname, id, typ;  int value, *lgptrptr, varclass;
{
	char *cptr3;

	if(lgptrptr == &glbptr) {
		if (gsearch(sname))
			return symptr;
		/* search on this symbol has just been done */
		if(symptr==0) {
			newerror(27);
			return 0;
		}
		cptr = symptr;
	}
	else {						/* local symbol */
		if(locptr > (ENDLOC-SYMMAX)) {
			newerror(28);
			exit(ERRCODE);
		}
		cptr = *lgptrptr;
	}
	cptr[IDENT]=id;
	cptr[TYPE]=typ;
	cptr[CLASS]=varclass;
	putint(value, cptr+OFFSET, OFFSIZE);
	cptr3 = cptr2 = cptr + NAME;
	while(an(*sname)) *cptr2++ = *sname++;

	if(lgptrptr == &locptr) {
		*cptr2 = cptr2 - cptr3;	/* set length */
		*lgptrptr = ++cptr2;
	}
	return cptr;
}

/*
** used for local variables with goto statement
*/
nextsym(entry) char *entry;
{
	entry = entry + NAME;
	while (*entry++ >= ' ') ;	/* find length byte */
	return entry;
}

/*
** the next two functions are nice but slow up
** compilation. Pointers to integers can do the job
** faster, so we will use them.
**
** get integer of length len from address addr
** (byte sequence set by "putint")
*/
getint(addr, len) char *addr; int len; {
	int i;
	i = *(addr + --len);  /* high order byte sign extended */
	while(len--) i = (i << 8) | *(addr+len)&255;
	return i;
}

/*
** put integer i of length len into address addr
** (low byte first)
*/
void putint(i, addr, len) char *addr; int i, len; {
	while(len--) {
		*addr++ = i;
		i = i>>8;
    }
}

/*
** test if next input string is legal symbol name
*/
#ifdef BUGFIXES
symname(sname) char* sname;
#else
symname(sname, ucase) char *sname; int ucase;
#endif
{

	blanks();
#ifdef UNCLEARBUGFIXES
	if (alpha(ch) == 0) return (*sname = 0);	/* causes failure */
#else
	if(alpha(ch)==0) return 0;
#endif
	cptr = sname;
	while(an(ch)) {
			*cptr++=gch();
		if((cptr-sname) > NAMEMAX) --cptr;	/* correct if too long */
    }
	*cptr=0;
	return TRUE;
}

/*
**
*/
void blanks()
{
	do {
		while (ch) {
			if (white()) gch();
			else return;
		}
		if (line != pline) return;		/* preprocessing */
		preprocess();
	}
	while (!eofstatus);
}

/*
**
*/
junk()
{
	if (an(gch()))
		while (an(ch)) gch();
	else while (an(ch) == 0) {
		if (ch == 0) break;
		gch();
	}
	blanks();
}
/*
**
*/
endst() {
	blanks();
	return ((streq(lptr,";")|(ch==0)));
}

illname() {
	newerror(29);
	junk();
}

multidef(sname)  char *sname; {
	newerror(30);
}

needtoken(str)  char *str; {
	if (match(str)==0) newerror(47, str);
}

needlval() {
	newerror(31);
}
/*
** force upper case alphabetics
*/
#ifdef MSC
upper(c)  char c;
{
	if((c >= 'a') & (c <= 'z')) return (c - 32);
	else return c;
}
#else
upper(c)  char c;
{
#asm
POP	BC
POP	HL
PUSH HL
PUSH BC
	LD	A,L
; TODO call CIOUPP::
	CP	'a'
	JR	C,UPPR10
	CP	'z' + 1
	JR	NC,UPPR10
	SUB	32
UPPR10:
	JP	CCC5##; CCSXT
#endasm
}
#endif

/*
** return next avail internal label number
*/
getlabel() {
	return(++nxtlab);
}

/*
** test if given character is alphabetic
*/
alpha(c)  char c;
{
#ifdef MSC
	return (((c>='a')&(c<='z'))|((c>='A')&(c<='Z'))|(c=='_'));
#else
#asm
	pop	bc
	pop	hl
	push hl
	push bc
	ld	a,l
	ld	hl,1
	cp	'a'
	jr	c,alph10
	cp	'z'+1
	jr	nc,alph20
	ret
alph10:
	cp	'A'
	jr	c,alph20
	cp	'Z'+1
	ret	c
	cp	'_'
	ret	z
alph20:
	dec	hl
;	ret
#endasm
#endif
}

/*
** test if given character is numeric
*/
numeric(c)  char c;
{
#ifdef MSC
	return((c>='0')&(c<='9'));
#else
#asm
	pop	bc
	pop	hl
	push hl
	push bc
	ld	a, l
	ld	hl, 1
	cp	'0'
	jr	c,numr10
	cp	'9'+1
	ret	c
numr10:
	dec	hl
;	ret
#endasm
#endif
}

/*
** test if given character is alphanumeric
*/
an(c)  char c;
{
#ifdef MSC
	return ((alpha(c))|(numeric(c)));
#else
#asm
	pop	bc
	pop	hl
	push hl
	push bc
	push hl
	push hl
	call alpha
	pop	bc
	pop	de
	ld	a, h
	or l
	ret	nz
	push de
	call numeric
	pop	bc
;	ret
#endasm
#endif
}

/*
** test if character is white space
** test for stack/program overlap
** primary -> symname -> blanks -> white
** ccavail(): exit on stack/symbol table overflow
*/
white()
{
#ifdef MSC
	return((*lptr <= ' ') && (*lptr != 0));
#else
#asm
	call ccavail##
	ld	hl, (lptr)
	ld	a, (hl)
	ld	hl, 1
	or a
	jr	z, white1
	cp	'!'
	ret	c
white1:
	dec	hl
;	ret
#endasm
#endif
}
/*
*/
gch()
{
#ifdef MSC
	int c;
	if(c=ch) bump(1);
	return c;
#else
#asm
	ld	a,(ch)
	or	a
	jr	z,gch10
	push af
	ld	hl,1
	push hl
	call bump
	pop	bc
	pop	af
gch10:
	jp ccc5##	;sign extend
#endasm
#endif
}
/*
*/
void bump(n) int n;
{
#ifdef MSC
	if(n) lptr=lptr+n;
	else  lptr=line;
	if(ch = nch = *lptr) nch = *(lptr+1);
#else
#asm
	pop	bc
	pop	hl
	push hl
	push bc
	ld	a, h
	or l
	jr	z, bump10
	ld	de, (lptr)
	add	hl, de
	jr	bump20
bump10:
	ld	hl, (line)
bump20:
	ld	(lptr),hl
	ld	a, (hl)
	ld	(ch), a
	ld	(nch), a
	or a
	ret	z
	inc	hl
	ld	a, (hl)
	ld	(nch), a
;	ret
#endasm
#endif
}

/*
**
*/
initline()
{
	lptr = line;
	if (ch = nch=*lptr) nch = *(lptr+1);
}

kill() {
	*line=0;
	initline();
}

/*
**
*/
void doinline()
{
again:
	if (fgets(line, LINEMAX, input[curlevel]) == NULL) {
		fclose(input[curlevel]);
		input[curlevel] = EOF;	/* not needed */
		if (curlevel == 0)
			openin();
		else
			--curlevel;
		if (!eofstatus) goto again;
	}
	else {
		/*
		** skip linenumbers incl. numbers with bit 7 set.
		*/
		while (numeric(*line++ & 127)) ;
		--line;
		initline();
	}
}

void ifline() {
	while(TRUE) {
		line = mline;
		doinline();
		if(eofstatus) return;
		if(match("#ifdef") || match("#IFDEF")) {	/* todo Upper and Lower case match */
			  ++iflevel;
			  if(skiplevel) continue;
			  blanks();
			  symname(msname); /*, FALSE); */
			  if (msearch(msname) == 0) { skiplevel = iflevel; }
			  continue;
		}
		if(match("#ifndef") || match("#IFNDEF")) {
			++iflevel;
			if(skiplevel) continue;
			blanks();
			symname(msname); /*, FALSE); */
			if (msearch(msname)) { skiplevel = iflevel; }
			continue;
		}
		if(match("#else") || match("#ELSE")) {
			if(iflevel) {
				if(skiplevel==iflevel) skiplevel=0;
				else if(skiplevel==0)  skiplevel=iflevel;
			}
			else noiferr();
			continue;
		}
		if(match("#endif") || match("#ENDIF")) {
			if(iflevel) {
				if(skiplevel==iflevel) skiplevel=0;
				--iflevel;
			}
			else noiferr();
			continue;
		}
		if(skiplevel) continue;
		if (match("#asm") || match("#ASM"))
			doasm();
		else
			if (ctext) {
				sout(line, stdout);
			}
		if(ch==0) continue;
		break;
	}
}

void noiferr()
{
	newerror(32);
	errflag=0;
}


void preprocess()
{
  int k;
  char c;
 preagain:
/*    line=mline; Now done in ifline() */
    ifline();
    if(eofstatus) return;

	pptr = -1;
	while ((ch != '\n') && ch) {
		if(white()) {
			keepch(' ');
			while(white()) gch();
		}
		if(ch=='"') {
			keepch(ch);
			gch();
			while((ch!='"') || ((*(lptr-1)==escape)&(*(lptr-2)!=escape))) {
				if(ch==0) {
					newerror(33);
					break;
				}
				keepch(gch());
			}
			keepch('"'); gch(); continue;
		}
		if(ch==39) {			/* single quote */
			keepch(39);
			gch();
			while((ch!=39) ||
					((*(lptr-1)==escape)&(*(lptr-2)!=escape))) {
				if(ch==0) {
					newerror(34);
					break;
				}
				keepch(gch());
			}
			keepch(39); gch(); continue;
		}
		if((ch=='/')&(nch=='*')) {
			bump(2);
			while (!((ch=='*') && (nch=='/'))) {
				if(ch) bump(1);
				else {
/*   				line=mline; Now done in ifline() */
					ifline();
					if(eofstatus) break;
				}
			}
			bump(2); continue;
		}
	   if(alpha(ch)) {				/* copy identifier to msname */
			cptr = msname;
			while (an(ch)) {
				if((cptr - msname) < NAMEMAX) *cptr++ = upper(ch);
				gch();
			}
			*cptr = 0;
			if(msearch(msname)) {	/* macro substitution */
				k=*(iptr = &symptr[NAMESIZE]);
				while(c=macq[k++]) keepch(c);
	      }
			else {
				cptr = msname;
				while(c=*cptr++) keepch(c);
			}
		}
	    else keepch(gch());			/* non alpha */
	}
	if(pptr>=LINEMAX) newerror(35);
	keepch(0);
	line=pline;
	initline();
	if (match("#DEFINE")) {		/* comment is removed now */
		addmac();
		goto preagain;
	}
	if (match("#INCLUDE")) {	/* comment is removed now */
		doinclude();
		goto preagain;
	}
}

void keepch(c)  char c;
{
	if(pptr<LINEMAX) pline[++pptr]=c;
}

void addmac() {
	int k;
#ifdef BUGFIXES
	if(symname(msname)==0)
#else
	if (symname(msname, FALSE) == 0)
#endif
		{
		illname();
		kill();
		return;
	}
	k=0;
	if(msearch(msname)==0) {
		if(cptr2=symptr) while(*cptr2++ = msname[k++]);
		else {
			newerror(36);
			return;
		}
	}
	/* if already defined, just new replacement */
	*(iptr = &symptr[NAMESIZE]) = macidx;
	while(white()) gch();
	while(putmac(gch()));
	if(macidx>=MACMAX) {
		newerror(37);
		exit(ERRCODE);
    }
}

putmac(c)  char c;
{
	macq[macidx]=c;
	if(macidx<MACMAX) ++macidx;
	return c;
}

void doasm()
{
	asmstart();
	while (TRUE) {
		line = mline;
		doinline();
		if (match("#endasm") || match("#ENDASM") || eofstatus)
			break;
		asmline(line);
    }
	asmfinish();
	kill();
}

/*
** open an include file
*/
void doinclude()
{
	char delim, filech;
	blanks();					/* skip over the name */
	if ((ch == '"') || (ch == '<')) {
		delim = '"';
		if (ch == '<') delim = '>';
		bump(1);
		cptr = lptr;
		while (filech = *cptr) {
			if (filech != delim) ++cptr;
			else break;
		}
		if (filech) *cptr = ' ';
	}
	if (++curlevel >= MAXLEVEL) {
		newerror(38); exit(ERRCODE);
	}
	if ((input[curlevel] = fopen(lptr, "r")) == NULL)
		opnerr(lptr);
	/* clear rest of line, so next read will
	** come from new file (if open)
	*/
	chkasm();
	kill();         /* clear rest of line */
}
