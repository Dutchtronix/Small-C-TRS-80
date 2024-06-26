/* from 80-Microcomputing july 1981.
 * author: John B. Harrell
 *     30 Keklico Court West
 *     Charleston, SC 29408
 * Adapted for TRS-80 Small-C V2.1:  Mikrorie Software
 * This version is four times as fast as the original!
 * compile: cc brkout
 *      m80 brkout=brkout
 *      l80 brkout,brkout-n-e
 */

#include <stdio/h>

#define VIDEO  0x3C00
#define KEYBD 0x3840
#define MAXCHAR 132

min(a,b) int a,b; {
  if (a>b) return(b);
  else return(a);
}

max(a,b) int a,b; {
  if (a>b) return(a);
  else return(b);
}

int
  rnda = 55, rndb = 89,
  stop = 0, best = 0,
  speed,xpos,ypos,ppos,xdir,ydir,
  score,spvar, nb,flag,
  temp,nbp;

char offpaddle[] = "        ",
   onpaddle[] = "\260\260\260\260\260\260\260\260";

main() {
  int i, k;
  char key;
  do {
    ppos=28; spvar=0;
    init(); pset();
    for (nbp=1; nbp++ <= nb; ) {
      for (k=1; k++ <= (5 * speed); ) {
        delay(); paddle();
      }
      ptc(0,61);
      printf("%2d", nbp);
      spvar=5;
      if (rand(2) == 1)
        xdir=1;
      else
        xdir=-1;
      ydir=1; ypos=29; xpos=rand(58)+2;
      do {
        for (k=1; k++ <= 6; paddle()) ;
        ball(); chkgame(); delay();
      }
      while (flag==FALSE);
    }
    best=max(best,score);
    ptc(8,18);
    printf("Run game again? ");
    do
      key = toupper(getkey());
    while  (key !='Y' && key != 'N');
    stop = (key == 'N');
  }
  while (stop == 0);
  printf("\034\037");
}

mem(ptr) char *ptr; {
  return(*ptr);
}

rand(n) int n; {
  int temp;
  temp = (rnda+rndb) & 0x007F;
  rnda = rndb; rndb = temp;
  return(((n*temp) >> 7)+1);
}

ptc(line,pos) int line, pos; {
  setc((line << 6) + pos);
}

line(number) int number; {
  ptc(number,0); putchar('\036');
}

fill(start,count,ch) char *start, ch; int count; {
  char *p;
  for (p=start+count-1; start<=p; *start++=ch) ;
}

pclr() {
  move(offpaddle, 0x3fc0+ppos, 8);
}

pset() {
  move(onpaddle, 0x3fc0+ppos, 8);
}

paddle() {
  if (mem(KEYBD)==32) {
    pclr();  ppos=max(2,ppos-1);
    pset();
  }
  if (mem(KEYBD)==64) {
    pclr(); ppos=min(54,ppos+1);
    pset();
  }
}

dset(x,y) int x,y; {
  set(x+x,y); set(x+x+1,y);
}

dclr(x,y) int x,y; {
  reset(x+x,y); reset(x+x+1,y);
}

dtest(x,y) int x,y; {
  if (point(x+x,y) && point(x+x+1,y))
    return(1);
  else
    return(0);
}

xchk() {
  if (xpos<2) {
    xdir = -xdir; xpos = 2;
 }
  if (xpos>61) {
    xdir = -xdir; xpos = 61;
  }
}

ychk() {
  if (ypos<5) {
    ydir=1; ypos=5; spvar=1;
  }
  if (ypos<23)
    spvar=min(spvar,4);
  if (ypos<19)
    spvar=min(spvar,3);
  if (ypos<15)
    spvar=min(spvar,2);
}

pchk() {
  int temp;
  flag=FALSE;
  if (ypos>=47) {
    ypos=46; temp=xpos-ppos;
    if (temp>=0 && temp<8) {
      ydir = -1;
      switch (temp) {
      case 0: xdir = -2; break;
      case 1:
      case 2:
      case 3: xdir = -1; break;
      case 4:
      case 5:
      case 6: xdir = +1; break;
      case 7: xdir = +2; break;
      }
    }
    else
      flag = TRUE;
  }
}

init() {
  int i; char s[MAXCHAR];
  printf("\034\037\017");
  ptc(3,22);
  printf(" < B R E A K O U T > ");
  line(10);  printf("speed (1-10, 1 is fastest) ");
  while (TRUE)
    if (scanf("%d", &speed) == 1) break;
/* fgets(s, MAXCHAR, stdin); speed = atoi(s); */
  speed = min(max(speed,1),10) * 2;
  line(12);  printf("number of balls (1-50) ");
  while (TRUE)
    if (scanf("%d", &nb) == 1) break;
/* fgets(s, MAXCHAR, stdin); nb = atoi(s); */
  nb = min(50,max(1,nb));
  printf("\034\037\017");
  for (i=1; i<=63; i++) {
    dset(i,3); dset(i,4);
  }
  for (i=3; i<=47; i++) {
    dset(0,i); dset(1,i); dset(62,i); dset(63,i);
  }
  fill(15616,320,191);
  score=0; line(0);
  printf("Breakout                   score: 0000      ");
  printf("best: %d", best);
  ptc(0,54); printf("ball:");
}

clr() {
  int i, temp;
  temp=((xpos-2) & 124)+2;
  for (i=temp; i <= temp+3; dclr(i++, ypos)) ;
  score += abs(ypos-27);
  ptc(0,34);
  printf("%4d", score);
  ydir = -ydir;
}

chkball() {
  ypos += ydir;
  xpos += xdir;
  xchk(); ychk(); pchk();
  if (dtest(xpos,ypos))
    clr();
}

ball() {
  dclr(xpos,ypos);
  chkball();
  if (!flag)
    dset(xpos,ypos);
}

chkgame() {
  if ((score % 1800) == 0)
    fill (15616,320,191);
}

delay() {
  int i;
  for (i=0; i++ <= spvar*speed; ) ;
}
 