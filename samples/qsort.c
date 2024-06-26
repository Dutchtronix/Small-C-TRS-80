/*
 * recursive Quicksort on TRS-80 screen
 *   Mikrorie Software
 * compile: cc qsort
 *          m80 qsort=qsort
 *          l80 qsort,gamelib,qsort-n-e
 */

#include <stdio/h>

#define VIDEO 0x3C00
#define HOME 28
#define CLEAR 31

qsort (left,right) char *left, *right; {
  char *ptr1, *ptr2;
  int compare, exchange;
  /* split interval */
  /* assure unsigned shift right; undo sign extent */
  compare = *((ptr1 = left+right) >> 1) & 0xFF;
  ptr1 = left; ptr2 = right;
  do {
    while ((*ptr1 & 0xFF) < compare)  ++ptr1;
    while ((*ptr2 & 0xFF) > compare)  --ptr2;
    if ( ptr2 > ptr1) {
      exchange = *ptr1; *ptr1 = *ptr2; *ptr2 = exchange;
    }
    if (ptr2 >= ptr1) {
      ++ptr1; --ptr2;
    }
  }
  while (ptr2 >= ptr1);

  if (left < ptr2)  qsort (left,ptr2);
  if (ptr1 < right) qsort (ptr1,right);
}

main() {
  char *address, *p1, *p2, key;
  int i, n;
  do {
    printf("%c%c\nQuick Sort op TRS-80 scherm\n\n",
           HOME, CLEAR);
    printf("\n\nvanaf welk geheugen adres ? ( hex ) ");
    scanf("%x", &address);
    printf("hoeveel bytes wilt u sorteren ? ( <=1024 ) ");
    scanf("%d", &n);
    if (n == 0 || n > 1024)
      n = 1024;
    move(address, video, n);
    qsort(video, video+n-1);
    do
      key = getkey();
    while (key != '\n' && key != '@');
  }
  while (key != '@');
  printf("%c%cQuick Sort Finished\n",HOME,CLEAR);
}
 