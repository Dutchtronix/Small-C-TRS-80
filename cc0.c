
/*
** Small-C Compiler Version 2.0
**
** Copyright 1982 J. E. Hendrix
**
** Implemented TRS-80 Mod I V2.1B Dutchtronix 2024
**
** Global Data and Initialization Part
*/

#include "cc.def"

#ifdef MSC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccint.h"
#else
#include "ccint/h"
#endif

/*
** Global Data Storage
** options first to help customizing
*/
char
	cversie[9] = "2.1B.004\0", /* for identification */
	headers = TRUE,		/* monitor function headers? */
	pause = TRUE,		/* pause for operator on errors? */
#ifdef OTHERASM
	outform = M80,		/* assembler output format */
#endif
	assext[5] = DEFOUTEXT,	/* default output file extension */
	ctext = FALSE,			/* non-zero to output c-source */
	escape = '\\',			/* escape character */
	fdebug = FALSE			/* print debug information? */
#ifdef OTHERASM
	,
	linenum[6] = "\260\260\260\260\261\040";
#else
	;
#endif
int
	numglbs = 200,		/* maximum number of globals */
	numdefs = 215,		/* maximum number of defines */
	maxzero = 256,		/* maximum number of zero elements */
	nxtlab = 0;			/* next available label # */

/*
** miscellaneous storage
*/
char

	* stage,    /* output staging buffer */

#ifdef MSC
  stagebuffer[STAGELIMIT],		/* can't use code space */
#endif

 *symtab,   /* symbol table */
 *litq,     /* literal pool */
 *macn,     /* macro name buffer */
 *macq,     /* macro string buffer */
 *pline,    /* parsing buffer */
 *mline,    /* macro buffer */
 *line,     /* points to pline or mline */
 *lptr,     /* ptr to either */
 *glbptr,   /* ptrs to next entries */
 *locptr,   /* ptr to next local symbol */
 *stagenext,/* next addr in stage */
 *stagelast,/* last addr in stage */
 *stagemax, /* highest addr used in stage */
  quote[2] = {'"',0}, /* literal string for '"' */
  *symptr,	/* pointer to current symtab entry */
 *cptr,     /* work ptrs to any char buffer */
 *cptr2,

  msname[NAMESIZE], /* macro symbol name array */
  ssname[NAMESIZE]; /* static symbol name array */


 int
	 nogo,     /* > 0 disables goto statements */
	 noloc,    /* > 0 disables block locals */

	 /*
	 ** the following two definitions are arrays of pointers to functions
	 ** and should look like this:
	 **      (*op)()[16],
	 **      (*op2)()[16],
	 ** but small-c cheats and declares an array of ints
	 */
	 op[16],   /* function addresses of binary operators */
	 op2[16],  /* same for unsigned operators */
	 opindex,  /* index to matched operator */
	 opsize,   /* size of operator in bytes */
	 swactive = 0, /* true inside a switch */
	 swdefault,/* default label #, else 0 */
	 * swnext,   /* address of next entry */
	 * swend,    /* address of last table entry */
	 * wq,       /* while queue */

	 argcs,    /* static argc */
	 * argvs,    /* static argv (original version) */

	 * wqptr,					/* ptr to next entry */
	 litidx,					/* index to next entry in litq */
	 litbound,					/* index available literal pool */
	 macidx = 0,				/* macro buffer index */
	 pptr,						/* ptr to preprocessing buffer */
	 ch,						/* current character of line being scanned */
	 nch,						/* next character of line being scanned */
	 declared,					/* # of local bytes declared, else -1 when done */
	 iflevel = 0,				/* #if... nest level */
	 skiplevel = 0,				/* level at which #if... skipping started */
	 litlab,					/* label # assigned to literal pool */
	 inlabel = FALSE,			/* (output not in label - state */
	 csp = 0,					/* compiler relative stk ptr */
	 ncmp = 0,					/* # open compound statements */
	 errflag = 0,				/* non-zero after 1st error in statement */
	 errcount = 0,				/*count of errors */
	 eofstatus = 0,				/* set non-zero on final input eof */
	 files = FALSE,			    /* non-zero if file list specified on cmd line */
	 filearg = 0,				/* cur file arg index */
	 lastst,					/* last executed statement type */
	 gsymend,					/* end of global symbol table */
	 msymend;					/* end of macro name table */

 int16_t
	 *iptr,						/* work ptr to any int16 buffer */
	 *iptr2;					/* another ptr to any int16 buffer */

 char
	 *ccheap,					/* local heap */
	 *lastp,
	 *foutbuf;
 int
#ifdef DYNHEAP
	 cchpsize = -1,
#else
	 cchpsize = 0,
#endif
#ifdef MSC
	 msccchpsize,
#endif
	 lastnheap;

 FILE
  *input[MAXLEVEL] = {EOF, EOF, EOF},
  *output=NULL,					/* fd # for output file */
  *errfile=NULL;				/* file with error messages */

int
  curlevel=0,					/* current input file level */
  outfile=0;					/* output file recognized */
  
/*
** peephole and assembler output tables
*/

char seq1[6] = { LDHLW, 0, 0, ADDHLSP, CCGINT, EXDEHL };
char seq2[5] = { LDHLW, 0, 0, ADDHLSP, CCGINT };
char seq3[6] = { LDHLW, 2, 0, ADDHLSP, CCGINT, EXDEHL };
char seq4[5] = { LDHLW, 2, 0, ADDHLSP, CCGINT };
char seq5[6] = { ADDHLSP, LDDH, LDEL, CCGINT, INCHL, CCPINT };
char seq6[6] = { ADDHLSP, LDDH, LDEL, CCGINT, DECHL, CCPINT };
char rseq1[7] = {6, POPDE, PUSHDE, NOOP, NOOP, NOOP, NOOP };
char rseq2[6] = {5, POPHL, PUSHHL, NOOP, NOOP, NOOP };
char rseq3[7] = {6, POPBC, POPDE, PUSHDE, PUSHBC, NOOP, NOOP };
char rseq4[6] = {5, POPBC, POPHL, PUSHHL, PUSHBC, NOOP };
char rseq5[7] = {6, CCINCI, NOOP, NOOP, NOOP, NOOP, NOOP };
char rseq6[7] = {6, CCDECI, NOOP, NOOP, NOOP, NOOP, NOOP };

#ifdef BADPEEP
/* Fail. HL may be tested later */
char seqw1[2] = { CCGCHAR, LDAL };
char seqw2[4] = { LDHLW, 0, 0, LDAL };
char rseqw1[3] = { 2, NOOP, LDAM };
char rseqw2[5] = { 4, NOOP, NOOP, NOOP, XORA };
char seqw7[4] = { LDHLW, WILD, 0, LDAL };
char rseqw7[5] = { 4, LDAB, WILD, NOOP, NOOP };
#endif
char seqw3[6] = { CCEQ, LDAH, ORL, JPZL, WILD, WILD };
char seqw4[6] = { CCEQ, LDAH, ORL, JPNZL, WILD, WILD };
char seqw5[6] = { CCNE, LDAH, ORL, JPZL, WILD, WILD };
char seqw6[6] = { CCNE, LDAH, ORL, JPNZL, WILD, WILD };
char seqw8[5] = { EXDEHL, LDHLW, 255, 0, CCAND };
char seqw9[4] = { LDHLW, 255, 0, CCAND };
char seqw10[5] = {LDHLW, WILD, 0, ADDHLSP, CCGINT };

char rseqw3[7] = {6, NOOP, NOOP, CCRST18, JPNZL, WILD, WILD };
char rseqw4[7] = {6, NOOP, NOOP, CCRST18, JPZL, WILD, WILD };
char rseqw5[7] = {6, NOOP, NOOP, CCRST18, JPZL, WILD, WILD };
char rseqw6[7] = {6, NOOP, NOOP, CCRST18, JPNZL, WILD, WILD };
char rseqw8[6] = {5, LDHB, 0, NOOP, NOOP, NOOP };
char rseqw9[5] = {4, LDLE, LDHB, 0, NOOP };
char rseqw10[6] = {5, LDLB, WILD, CCGIB, NOOP, NOOP };

/*
** This table starts at intcode NOP = 128
*/
char *intone[] = {
	"NOP",
	"LD\tA,(HL)",
	"LD\tL,A",
	"LD\tA,L",
	"LD\t(DE),A",
	"LD\tA,H",
	"OR\tA",
	"OR\tL",
	"OR\tH",
	"XOR\tA",
	"ERR",
	"ERR",
	"DEC\tHL",
	"INC\tHL",
	"LD\tH,(HL)",
	"ADD\tHL,SP",
	"PUSH\tHL",
	"EX\t(SP),HL",
	"JP\t(HL)",
	"LD\tSP,HL",
	"ADD\tHL,HL",
	"ADD\tHL,DE",
	"SBC\tHL,DE",
	"ADD\tHL,BC",
	"EX\tDE,HL",
	"POP\tHL",
	"LD\tD,H",
	"LD\tE,L",
	"POP\tDE",
	"PUSH\tDE",
	"POP\tBC",
	"PUSH\tBC",
	"INC\tSP",
	"DEC\tSP",
	"RET",
	"RST 18H",
	"LD\tL,E"};

char *inttwo[] = {
	"LD\tHL,",
	"LD\tDE,",
	"LD\tBC,",
	"DEFW\t",
	"DEFW\t",
	"ERR" };

char *intthree[] = {
	"JP\t",
	"JP\tZ,",
	"JP\tNZ,",
	"JP\tP,",
	"JP\tM,",
	"JP\tZ,",
	"LD\tHL," };

char* argstrings[10] = {
	"||",
	"&&",
	"|",
	"^",
	"&",
	"== !=",
	"<= >= < >",
	">> <<",
	"+ -",
	"* / %"
};

int argsdropval[2] = {
	1, 0
};

int argsendval[2] = {
	0, 1
};

int argsoff[10] = {
	0, 0, 0, 1, 2, 3, 5, 9, 11, 13
};

int argsf1[10];
int argsf2[2];

#ifdef MSC
extern int	/* from part 1 */
	outside(), dodeclare(), newfunc();

extern int	/* from part 2 */
	amatch(), blanks(), msearch(), bump(), newerror(),
	kill(), preprocess(), sout(), upper(), an();
	
extern int	/* from part 3 */
	number();

extern int
	hier3(), hier4(), hier5(), hier6(), hier7(), hier8(), hier9(),
	hier10(), hier11(), hier12(), hier13(), eq0(), ne0();

extern void	/* from part 4 */
	zadd(), zand(), zasl(), zasr(), uasr(), zeq(),
	zge(), zgt(), zle(), zlt(), zmod(),
	zdiv(), zmult(), zne(), zor(), point(), zsub(),
	trailer(), uge(), ugt(), ule(), ult(), zxor();

extern void
	strcopy(), extension(), outstr(), outbyte(), parse(),
	initheap(), openin(), doheader();

extern int
	ccalloc();
#endif

#ifdef MSC
	extern void initptrmaptable(void);
	extern int16_t mapptr2idx(char* syptr);
	extern char* mapidxtoptr(int16_t idx);

void ASSERT(bool, id) int bool, id;
{
	if (!bool) {
		printf("Assertion %d failure\n", id);
	}
}
#endif

#include "cc01.c"

