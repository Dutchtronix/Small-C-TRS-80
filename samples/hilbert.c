/*
 * Hilbert curves on TRS-80 SCREEN
 *    Mikrorie Software
 * compile: cc hilbert
 *          m80 hilbert=hilbert
 *          l80 hilbert,gamelib,hilbert-n-e
 */

#include <stdio/h>

extern printf(), scanf(), getkey(), set();

#define EAST 10
#define SOUTH 11
#define WEST 12
#define NORTH 13

int len,posx,posy;   /* startx,starty  */ 
int endx, endy, n, scale;

min(a,b) int a, b; {
  if (a>b) return(b); else return(a);
}

max(a,b) int a, b; {
  if (a<b) return(b); else return(a);
}

main() {
  printf("\034\037\027\n\n\n HILBERT kurven (07/04/84)");
  printf("\n\n  Mikrorie Software");
  printf("\n\nGeef de schaal: (default 0) ");
  scanf("%d", &scale);
  if (scale == 0) scale = 48;
  do {
    printf("\016\034\037\360Hilbert kurven");
    printf("\n\361order: ");
    scanf("%d", &n);
    posx = posy = 0;
    len = scale >> n;
    endx = posx; endy = posy;
    if (n != 0) {
      hilbert(EAST,SOUTH,WEST,NORTH,n);
      do ;
      while (getkey() == 0);
    }
  }
  while (n != 0);
}

gmove(dir) int dir; {
  int i,j;
  switch (dir) {
    case EAST: posx += (len+len); break;
    case SOUTH: posy += len; break;
    case WEST: posx -= (len+len); break;
    case NORTH: posy -= len; break;
  }
  for (i=min(posx,endx); i <= max(posx,endx); ++i) 
    for (j=min(posy,endy); j <= max(posy,endy);)
      set(i,j++);
  endx = posx; endy = posy;
}

hilbert(r,d,l,u,i) int r, d, l, u, i; {
  /* voor het gebruikte algorithme zie:
         Software-Practice and Experience,
          Vol.11, 99-100 (1981)
  */
  if (i>0) {
    hilbert(d,r,u,l,i-1); gmove(r);
    hilbert(r,d,l,u,i-1); gmove(d);
    hilbert(r,d,l,u,i-1); gmove(l);
    hilbert(u,l,d,r,i-1);
  }
}
 