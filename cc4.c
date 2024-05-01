/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I V2.1B Dutchtronix 2024
**
** Part 4
*/

#include "cc.def"

#ifdef MSC
#include <stdio.h>
#include <stdlib.h>
#include "ccint.h"
#else
#include "ccint/h"
#endif

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
* stage, * symtab, * litq, * macn, * macq, * pline,
* mline, * line, * lptr, * glbptr, * locptr, * stagenext,
* stagelast, * stagemax, * cptr, * cptr2, * symptr, ssname[NAMESIZE];

extern int						/* from part 0 */
	nogo, noloc, op[16], op2[16], swactive,
	swdefault, * swnext, * swend, * wq,
	* wqptr, litidx, litbound, ch, declared, nxtlab,
	litlab, csp, ncmp, errflag, curlevel, outfile,
	eofstatus, files, filearg, lastst, macidx, maxzero,
	inlabel, gsymend;

extern char
	cversie[5];

extern int16_t
	*iptr;

extern FILE						/* from part 0 */
	*input[MAXLEVEL],
	*output;

extern char
	seq1[6], seq2[5], seq3[6], seq4[5], seq5[6], seq6[6],
	rseq1[7], rseq2[6], rseq3[7], rseq4[6], rseq5[7], rseq6[7],
	*intone[], *inttwo[], *intthree[]
#ifdef BADPEEP
	,seqw1[2], seqw2[4], rseqw1[3], rseqw2[5], seqw7[4], rseqw7[5]
#endif
	,seqw3[6], seqw4[6], seqw5[6], seqw6[6], seqw8[5], seqw9[4],
	rseqw3[7], rseqw4[7], rseqw5[7], rseqw6[7], rseqw8[6], rseqw9[5];

#ifdef MSC
extern int						/* from part 2 */
	addsym(), amatch(), blanks(), msearch(),
	initline(), bump(), clearstage(), endst(),
	newerror(), findglb(), findloc(), gch(),
	getint(), getlabel(), illname(), junk(), kill(),
	lout(), match(), multidef(), needtoken(), nextsym(),
	numeric(), preprocess(),  putint(),
	setstage(), sout(), streq(), symname(), upper(),
	an();
	
extern int	/* from part 3 */
	checkl(), constexpr(), expression(), number(),
	qstr(), stowlit();

extern int
	outstr(), outbyte(), nl(), outdec(), cout(), tab(), ol();

extern void
	printlabel(), doextern(), prnum(), outBhex(), pop(), unpush();
#endif


#ifdef MSC
	extern void initptrmaptable(void);
	extern int16_t mapptr2idx(char* syptr);
	extern char* mapidx2ptr(int16_t idx);
#endif


#include "cc41.c"
#include "cc42.c"
