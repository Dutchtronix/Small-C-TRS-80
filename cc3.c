/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I Mikrorie - Jan de Rie
**
** Part 3
*/

#include "cc.def"

#ifdef MSC
#include <stdio.h>
#include <stdlib.h>
#endif

#include "ccint.h"


extern char						/* from part 0 */
	fdebug, headers, pause, 
#ifdef OTHERASM
	outform,
	linenum[5],
#endif
	assext[4], ctext;

extern int
	numglbs, numdefs, opsize, opindex;

extern char
	*stage, *symtab, *litq, *macn, *macq, *pline,
	*mline, *line, *lptr, *glbptr, *locptr, *stagenext,
	*cptr, *cptr2, *symptr, ssname[NAMESIZE],
	quote[2], escape;

extern int						/* from part 0 */
	nogo, noloc, op[16], op2[16], swactive,
	swdefault, *swnext, *swend, *wq,
	*wqptr, litidx, litbound, ch, declared, nxtlab,
	litlab, csp, ncmp, errflag, curlevel, outfile,
	eofstatus, files, filearg, lastst, macidx, maxzero,
	nch, gsymend;

extern int16_t* iptr;

extern FILE						/* from part 0 */
	*input[MAXLEVEL],
	*output;

#ifdef MSC
extern int						/* from part 2 */
	addsym(), amatch(), msearch(),
	initline(), bump(), clearstage(), endst(),
	newerror(), findglb(), findloc(), gch(),
	getint(), getlabel(), illname(), junk(), kill(),
	lout(), match(), multidef(), needtoken(), nextsym(),
	numeric(), preprocess(), postlabel(), putint(),
	setstage(), sout(), streq(), symname(), upper(),
	an(), ccalloc(), intlen(), getloc(), primary();

extern int
	number(), pstr(), qstr(), litchar(), plung1(), nextop(),
	memset(), dbltest(), calc(), hier1(), hier14(), constant(),
	modstk();

extern void
	zadd(), zand(), zasl(), zasr(), uasr(),
	deflabel(), swtable(),
	zdiv(), zeq(), entry(), doextern(), zge(),
	zgt(), doheader(), jump(), zle(), zlt(), zmod(),
	zmult(), zne(), zor(), point(), doret(),
	zsub(), sw(), trailer(), uge(), ugt(), ule(), ult(),
	zxor(), doconst(), do2const(),
	eq0(), ne0(), inc(), dec(), gt0(), ge0(), lt0(), ult0(), le0(),
	zerojump(), testjump(), strconst(), callstk(), putstk(), putmem(),
	ccfree(), docall(), move(), getmem(), indirect(), swapstk(),
	address();

extern int
	hier3(), hier4(), hier5(), hier6(), hier7(), hier8(), hier9(),
	hier10(), hier11(), hier12(), hier13();

extern void
	blanks(), callfunction(), result(), step(), store(), rvalue(), stowlit(),
	experr(), plung2(), dropout(), com(), lneg(), neg(), pop(), needlval(),
	swap(), doublereg(), smartpop(), push();
#endif

extern char* argstrings[];
extern int argsoff[], argsf1[], argsf2[], argsdropval[], argsendval[];

#ifdef MSC
	extern void initptrmaptable(void);
	extern int16_t mapptr2idx(char* syptr);
	extern char* mapidx2ptr(int16_t idx);
#endif


#include "cc31.c"
#include "cc32.c"
#include "cc33.c"
