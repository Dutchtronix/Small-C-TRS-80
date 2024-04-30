/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I Mikrorie - Jan de Rie
**
** Part 2
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
	assext[4], ctext, escape;

extern int
	numglbs, numdefs, nch, iflevel, skiplevel, pptr;

extern char
	*stage, *symtab, *litq, *macn, *macq, *pline,
	*mline, *line, *lptr, *glbptr, *locptr, *stagenext,
	*stagelast, *cptr, *cptr2, *symptr, ssname[NAMESIZE],
	msname[NAMESIZE];

extern int						/* from part 0 */
	nogo, noloc, op[16], op2[16], swactive,
	swdefault, *swnext, *swend, *wq,
	*wqptr, litbound, ch, declared, nxtlab,
	litlab, csp, ncmp, errflag, curlevel, outfile,
	eofstatus, files, filearg, lastst, macidx, maxzero,
	gsymend, msymend, errcount;

extern int16_t* iptr;

extern FILE						/* from part 0 */
	*input[MAXLEVEL],
	*output,
	*errfile;

#ifdef MSC
extern int
	ASSERT(), gsearch(), astreq(), an(), gch(), alpha(), white(),
	streq(), match(), msearch(), peephole(), hash(), putmac();


extern void
	asmstart(), asmline(), asmfinish(), openin(), chkasm(), opnerr(),
	sout(), newerror(), bump(), noiferr(), preprocess(), keepch(),
	addmac(), doasm(), blanks(), putint(), doinclude(),  outbyte(),
	outstr(), cout(), xout(), nl(), checkmax(), printerror();
#endif

#ifdef MSC
	extern void initptrmaptable(void);
	extern int16_t mapptr2idx(char* syptr);
	extern char* mapidx2ptr(int16_t idx);
#endif

#include "cc21.c"
#include "cc22.c"
