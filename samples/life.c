/*
 * John Horton Conway's game of life
 *  Written by John Waples.
 *  Speed up: Mikrorie Software.
 * Compile: cc life
 *          m80 life=life
 *          l80 life,life-n-e
 */

#include <stdio/h>

#define BKSP 8
#define TAB 9
#define UPARROW 0x5b
#define UNDERSCORE 0x5f
#define CLEAR 0x1f
#define HOME 0x1c
#define BREAKVALUE 1

#define MINRWORLDSIZE 3
#define MAXRWORLDSIZE 12
#define MINCWORLDSIZE 3
#define MAXCWORLDSIZE 64
#define MAXSAMEPOPCNT 30

#define AVIDSTART 3CC0H
#define AVIDEND 3FBFH
#define CVIDSTART 0x3CC0
#define CVIDEND 0x3FBF

int popcnt, samepopcnt, prepopcnt, generation;
char cursor, keystruck;
char *location;
int rworldsize, cworldsize;

main() {
  do {
/*
** TODO update key value so easier on the emulator
** speed option
** option to randomly generate a colony
**/
    printf("%c%c\nThe Game of Life\n",HOME,CLEAR);
	printf("press R to randomise the world or S to set up Colony.\n");
    printf("<@> to exit.\n");
	while ((keystruck = getkey()) == 0) ;
	if (keystruck == '@') {
		exit(1);
	}
	location = CVIDSTART; cursor = ' ';
	if ((keystruck == 'r') || (keystruck == 'R')) {
		do {
			printf("World size rows: ");
			scanf("%d", &rworldsize);
			if ((rworldsize < MINRWORLDSIZE) || (rworldsize > MAXRWORLDSIZE)) {
				printf("\nWorld size invalid\n");
				continue;
			}
			printf("World size columns: ");
			scanf("%d", &cworldsize);
			if ((cworldsize < MINCWORLDSIZE) || (cworldsize > MAXCWORLDSIZE)) {
				printf("\nWorld size invalid\n");
				continue;
			}
			break;
		}
		while (TRUE);
		printf("%c%c",HOME,CLEAR);
		RndWorld();
		printf("%cprint any key to start \n", HOME);
		while (getkey() == 0) ;
	}
	else {
		printf("%c%cUpArrow, Linefeed, Backspace, Tab, O or <space>\n", HOME, CLEAR);
		printf("Press <ENTER> when ready ");
		printf("<@> to exit.\n");
		do {
		  *location = UNDERSCORE;
		  pause();
		  *location = cursor;
		  pause();
		  switch (keystruck = getkey()) {
		  case UPARROW:
			if (location > (CVIDSTART+64))
			  location -= 64;
			break;
		  case LF:
			if (location < (CVIDEND-64) /*0x408E */)
			  location += 64;
			break;
		  case BKSP:
			if (location > CVIDSTART)
			  --location;
			break;
		  case TAB:
			if (location < CVIDEND)
			  ++location;
			break;
		  case 'o':
		  case 'O':
			*location++ = 'O'; break;
			if (location >= CVIDEND) {
				location -= 1024;
			}
		  case ' ':
			*location++ = ' '; break;
			if (location >= CVIDEND) {
				location -= 1024;
			}
		  case '@':
			exit();
		  }
		  cursor = *location;
		}
		while (keystruck != '\n');
	}
    generation = 0;
	samepopcnt = 0;
	popcnt = 0;
    printf("%cThe Game of Life                         \n", HOME);
    do {
	  prepopcnt = popcnt;
      popcnt = 0;
      countpop();
	  if (popcnt == prepopcnt) {
		  ++samepopcnt;
	  }
	  if (samepopcnt >= MAXSAMEPOPCNT) {
		  break;
	  }
      printf("%c\ngeneration %d",HOME, generation);
      printf(" popcnt %d.\n\n",popcnt);
      nextgen();
      generation++;
	  prepopcnt = popcnt;
    }
    while ((popcnt != 0 && (keystruck=getkey()) != BREAKVALUE)) ;
    if (keystruck != BREAKVALUE) {
		if (popcnt > 0) {
			printf("The population has stabilized\n");
		}
		else {
			printf("\nThe Colony has perished.\n");
		}
	  printf("print any key to continue\n");
	  while (getkey() == 0) ;
	}
  }
  while (keystruck != BREAKVALUE);
}

pause() {
  int i;
  for (i = 1; i <= 100; ++i) ;
}

nextgen()
{
#asm
 LD HL,3CC0H  ;FIRST LOOP
LOOP1:
 LD DE,3FBFH ;END
 EX DE,HL
 OR A
 SBC HL,DE
 JR C,NEXTG2 ;GO-ON
 EX DE,HL
 LD A,'N'
 LD B,0  ;NUMBER
 PUSH HL
 LD DE,-65
 ADD  HL,DE  ;I-65
 CP (HL)
 JR NC,L002
 INC  B
;
L002: INC  HL ;I-64
 CP (HL)
 JR NC,L004
 INC  B
L004: INC  HL ;I-63
 CP (HL)
 JR NC,L006
 INC  B
L006: POP  HL ;I BACK
 DEC  HL ;I-1
 CP (HL)
 JR NC,L008
 INC  B  ;NUMBER
L008: INC  HL
 PUSH HL ;I
 INC  HL ;I+1
 CP (HL)
 JR NC,L010
 INC  B
L010: LD DE,62
 ADD  HL,DE  ;I+63
 CP (HL)
 JR NC,L012
 INC  B
L012: INC  HL ;I+64
 CP (HL)
 JR NC,L014
 INC  B
L014: INC  HL ;I+65
 CP (HL)
 JR NC,L016
 INC  B
L016: POP  HL ;I
 CP (HL) ;MEM(I)
 JR NC,ELSE1
 LD A,B ;NUMBER
 CP 4
 JR NC,IF1 ;B>3
 CP 2
 JR NC,ELSE1;B<2
IF1: LD (HL),'X'
 JR OUT1
ELSE1: LD A,(HL) ;MEM(I)
 CP ' '
 JR NZ,OUT1
 LD A,B ;NUMBER
 CP 3
 JR NZ,OUT1
 LD (HL),'.'
OUT1: INC  HL ;LOOP
 JR LOOP1
#endasm
}

nextg2()
{
#asm
LOOP2: LD HL,3CC0H
LP2: LD DE,3FBFH
 OR A
 EX DE,HL
 SBC HL,DE
 JR C,OUTL
 EX DE,HL
 LD A,'X'
 CP (HL)
 JR NZ,IF2
 LD (HL),' '
 JR OUT2
IF2: LD A,'.'
 CP (HL)
 JR NZ,OUT2
 LD (HL),'O'
OUT2: INC  HL
 JR LP2
OUTL: RET
#endasm
}

countpop()
{
#asm
 LD HL,3CC0H
 LD A,'O'
 LD IX,POPCNT 
LP3: LD DE,3FBFH
 OR A
 EX DE,HL
 SBC  HL,DE
 JR C,OUTC
 EX DE,HL
 CP (HL)
 JR NZ,CNT002
 INC (IX+0)
CNT002: INC HL
 JR LP3
OUTC: RET
#endasm
}

RndWorld()
{
	/* randomizes the world with a density of 1/2 */
	int r,c;
	int rowstartlocation;
	rndini();
#asm
	ld a,r
	ld (40ABH),a
#endasm
	for (c = 0; c < rworldsize; ++c) {
		rowstartlocation = location;
		for (r = 0; r < cworldsize; ++r) {
			if (random(2) == 1) {
				cursor = ' ';
			}
			else {
				cursor = 'O';
			}
			*location++ = cursor;
		}
		location = rowstartlocation + 64;
		if (location >= CVIDEND) {
			location -= 1024;
		}
	}
}
 