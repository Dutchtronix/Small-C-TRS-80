/*
 * Game by K.M.Chung,4/26/79
 *  TRS-80 Small-C 2.1
 *  by Mikrorie Software
 * compile: cc blockade
 *          m80 blockade=blockade
 *          l80 blockade,blockade-n-e
 */

#include <stdio/h>

#define SCREEN 0x3C00
#define LINESIZE 64
#define MAXCHAR 132

int speed, abort,
    score[2], mark[2], pmove[2],
    cursor[2];

pscore() {
    printf("%3d",score[0]);
    setc(1020);
    printf("%3d",score[1]);
}

blink(i) int i; {
    int *t, k, delay;
    t = cursor[i]-pmove[i];
    for (k=1; k<=30; k++) {
        for (delay=1; delay++ <=200;) ;
        if (*t == '  ')
            *t = mark[i];
        else
            *t = '  ';
    }
}

main() {
    char key, s[MAXCHAR];
    int i,j;
    printf("\034\037***  Blockade  ***\n\n");
    printf("Speed (1=fast, 10=slow): ");
    scanf("%d", &speed);
    speed *= 20;
    *mark = '**'; mark[1] = '()';
    *score = 0; score[1] = 0;
    do {
        printf("\017\034\037");
        for (i=9; i <= 117; i++) {
            set(i,1); set(i,45);
        }
        for (i=1; i<=45; i++) {
            set(9,i); set(10,i);
            set(116,i); set(117,i);
        }
        *cursor = SCREEN+(LINESIZE << 2) +12;
        cursor[1] = 0x4000-(LINESIZE << 2) -16;
        for (j=0; j<=1; j++)
            *(cursor[j]) = mark[j];
        *pmove = LINESIZE; pmove[1] = -LINESIZE;
        i = 1; abort = FALSE;
        pscore();
        setc(16);
        printf("<ENTER> to continue, <BREAK> to exit");
        do {
            if ((key = getkey()) == 27) /* escape */
                key = 1;
        }
        while (key != '\n' && key != 1);
        if (key == 1) goto einde;
        for (j=9; j<= 117; set(j++,1)) ;
        do {
            i = 1 - i;
            for (j=1; j<= speed; j++) {
                key=getkey();
                switch (key) {
                case 'W': *pmove = -LINESIZE; break;
                case 'Z': *pmove = LINESIZE; break;
                case 'S': *pmove = 2; break;
                case 'A': *pmove = -2; break;
                case 'P': pmove[1] = -LINESIZE; break;
                case '.': pmove[1] = LINESIZE; break;
                case ';': pmove[1] = 2; break;
                case 'L': pmove[1] = -2; break;
                }
            }
            cursor[i] += pmove[i];
            if (*(cursor[i]) == '  ')
                *(cursor[i]) = mark[i];
            else {
                ++score[1-i];
                abort = TRUE; blink(i);
            }
        }
        while (abort != TRUE);
    }
    while (score[1-i]<10);
einde:
    printf("\034\037\n");
}

 