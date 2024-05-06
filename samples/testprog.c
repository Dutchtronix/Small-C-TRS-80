#include <stdio/h>

#define FULLC 1
#define void  


char testch;
char inbuf[12];	/* input line */
char* stagenext;	/* next addr in stage */

void test(rr) int rr;
{
  int y;
  y = r;
}

/* max -- compute maximum of two integers */
max (x, y)
int x,y;
{
   if (x > y)
     return x;
   else
     return y;
}

/*
** test for early dropout from || or && evaluations
*/
dropout(k, testfunc, exit1, lval) int k, (*testfunc)(), exit1, lval[];
{
  if(k) rvalue(lval);
  else if(lval[3]) doconst(lval[3]);
	(*testfunc)(exit1); /* jumps on false */
}

plung1(heir, lval) int (*heir)(), lval[];
{
	char* before, * start;
	int k;
	if (!(before = stagenext)) {
		foo();
	}
	k = (*heir)(lval);
}

skim(opstr, testfunc, dropval, endval, heir, lval)
char* opstr;

#ifdef FULLC
int (*testfunc)(), dropval, endval, (*heir)(), lval[];
#else /* FULLC */
int testfunc, dropval, endval, heir, lval[];
#endif /* FULLC */
{
	int k, hits, droplab, endlab;
	dropout(k, testfunc, droplab, lval);
}

/* main(argc, argv) char *argv(..); crashes compiler */
main(argc, argv) char *argv[];
{
	foo();

	testch = '.';

	/* code gen bug */
	strcpy(inbuf, "Testing");
	printf("input line: %s %d",inbuf, *inbuf);
    if (*inbuf == testch)
		printf("incorrect\n");
}


