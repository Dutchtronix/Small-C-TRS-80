#include "stdio/h"
#define MAXS 132
#define CLEAR 0x1f
#define HOME 0x1c

double x1, x2, x3;
double fnull = 0;
char s[MAXS];
char keystruck;

nl()
{
	printf("\n");
}

main(argc, argv) char *argv[]; {
	double x4, x5, x6;
	flinit();
	nl();
	printf("floating point package: ");
	flscan("123.456\n",x1); flprint(x1,10,5); nl();

	printf("number 1: ");
	fgets(s, MAXS, stdin);
	flscan(s, x1); 
	printf("number 2:  ");
	fgets(s, MAXS, stdin);
	flscan(s, x2);
	printf("Press any key to continue");
	while ((keystruck = getkey()) == 0) ;
	printf("%c%c", HOME, CLEAR);

	printf("entered numbers ");
	flprint(x1, 10, 5);
	printf(" and ");
	flprint(x2, 10, 5);
	nl();
	printf("sum:      "); fadd(x3,x1,x2);
	flprint(x3,10,5);      
	printf(" delta:    "); fsub(x3,x1,x2);
	flprint(x3,10,5); nl();
	printf("product:  "); fmul(x3,x1,x2);
	flprint(x3,10,5);      
	printf(" quotient: "); fdiv(x3,x1,x2);
	flprint(x3,10,5); nl();
/*
** wrong answer
*/
	printf("exp of number 1: "); fexp(x4,x1);
	flprint(x4,10,5); nl();

	if (fcomp(x2,fnull)>0) {
		printf("nat.log number 2: ");
		fln(x4,x2); flprint(x4,10,5); nl();
	 }
	printf("random:  "); frnd(x5);
	flprint(x5,3,15); nl();
	if (fcomp(x1,fnull)>0) {
		printf("number 1 to the power number 2 :");
		fpower(x5,x1,x2); flprint(x5,10,5); nl();
	}
	printf("largest of number 1 and number 2 is ");
	if (fcomp(x1,x2)>=0)
		printf("number 1\n");
	else
		printf("number 2\n");
/*
** trig functions give the wrong answer
*/
#ifdef TRIG
	printf("sin number 1: ");
	fsin(x5, x1); flprint(x5,10,5);      
	printf(" cos number 2: ");
	fcos(x5, x2); flprint(x5,10,5); nl();
	printf("tan number 1: ");
	ftan(x5, x1); flprint(x5,10,5);      
	printf("arctan number 2: ");
	fatn(x5, x2); flprint(x5,10,5); nl();
#endif
	if (fcomp(x1,fnull)>0) {
		printf("square root number 1: ");
		fsqr(x5,x1); flprint(x5,10,5); nl();
	}
	printf("round number 1:  %10d ",round(x1));
	printf("trunc number 2:  %10d\n",trunc(x2));
	printf("int number 1: ");
	fint(x5,x1); flprint(x5,10,5);      
	printf(" fix number 2: ");
	fix(x5,x2);  flprint(x5,10,5); nl();
	printf("abs number 2: ");
	fabs(x5,x2); flprint(x5,10,5);      
	cvfloat(x5,9);
	fsqr(x6,x5);
	printf(" root 9:  %10d\n",round(x6));
}
 