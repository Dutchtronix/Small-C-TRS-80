/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod IV2.1B Dutchtronix 2024
**
** Part 1
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
	numglbs, numdefs;

extern char
	* stage, * symtab, * litq, * macn, * pline,
	* mline, * line, * lptr, * glbptr, * locptr, * stagenext,
	* cptr, * cptr2, * symptr, ssname[NAMESIZE];
	

extern int						/* from part 0 */
	nogo, noloc, op[16], op2[16], swactive,
	swdefault, * swnext, * swend, * wq, inlabel,
	* wqptr, litidx, litbound, ch, declared, nxtlab,
	litlab, csp, ncmp, errflag, curlevel, outfile,
	eofstatus, files, filearg, lastst, macidx, maxzero;

extern int16_t *iptr, *iptr2;


extern FILE						/* from part 0 */
	input[MAXLEVEL],
	output;

#ifdef MSC
extern int						/* from part 2 */
	addsym(), amatch(), blanks(), msearch(),
	initline(), bump(), clearstage(), endst(),
	newerror(), findglb(), findloc(), gch(),
	getint(), getlabel(), illname(), junk(), kill(),
	lout(), match(), multidef(), needtoken(), nextsym(),
	numeric(), preprocess(), postlabel(), putint(),
	setstage(), sout(), streq(), symname(), upper(),
	an();
	
extern int	/* from part 3 */
	checkl(), constexpr(), expression(), number(),
	qstr(), dotest(), stowlit();

extern int	/* from part 4 */
	zadd(), zand(), zasl(), zasr(), uasr(),
	dodefw(), dodefb(), dodefs(), deflabel(), swtable(),
	zdiv(), zeq(), entry(), doextern(), zge(),
	zgt(), doheader(), jump(), zle(), zlt(), zmod(),
	modstk(), zmult(), zne(), zor(), point(), doret(),
	zsub(), sw(), trailer(), uge(), ugt(), ule(), ult(),
	zxor();

extern void
	delwhile(), addwhile(), docont(), dobreak(),
	dofor(), dodo(), dowhile(), doexpr(), ns(), compound(),
	doif(), doswitch(), docase(), dodefault(), dogoto(),
	doreturn(), doargs(), init(), outstr(), outBhex(),
	outbyte(), nl(), dumpzero(), prnum(), declglb(), memset(),
	ASSERT();

extern int
	needsub(), initials(), addlabel(), dolabel(), readwhile(), statement();
#endif

#ifdef MSC
	extern void initptrmaptable(void);
	extern int16_t mapptr2idx(char* syptr);
	extern char* mapidx2ptr(int16_t idx);
#endif

#include "cc11.c"
#include "cc12.c"
