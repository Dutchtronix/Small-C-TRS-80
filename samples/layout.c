/*
 text format program
*/
#include <stdio/h>
#include <cclib/h>

#define ENDSTR 0
#define BACKSPACE 8
#define TAB 9
#define NEWLINE 13
#define BLANK ' '
      /* string size 0..132 */
#define STRSIZE 133
#define MAXSTR 132
#define DQUOTE   '"'
#define PAGENUM  '#'
#define SQUOTE   39   /* ' */
#define PLUS    '+'
#define MINUS   '-'
#define PAGEWIDTH   60
#define PAGELEN   66
#define HUGE   2500
#define FORMFEED   12
#define PERIOD   '.'
#define UNDERLINE  '_'
#define TILDE   '~'
#define BP 0
#define BR 1
#define CE 2
#define FI 3
#define FO 4
#define HE 5
#define IND 6
#define LS 7
#define NF 8
#define PL 9
#define RM 11
#define SP 12
#define TI 13
#define UL 14
#define CP 15
#define PA 16
#define JU 17
#define NJ 18
#define TM 19
#define BM 20
#define PR 21
#define NP 22
#define PS 23
#define FS 24
#define FC 25
#define FH 26
#define UNKNOWN 27

int fd;       /* input file */
/* page parameters */
int curpage;     /* current output page #; init 0 */
int newpage;     /* next output page #; init 1 */
int lineno;      /* next line to be printed; init 0 */
int plval;       /* page length in lines; init PAGELEN */
int m1val;       /* margin before and including header */
int m2val;       /* margin after header */
int m3val;       /* margin after last text line */
int m4val;       /* bottom margin, including footer */
int bottom;      /* last line on page: plval-m3val-m4val */
char header[STRSIZE]; /* top of page title; init NEWLINE */
char footer[STRSIZE]; /* bottom of page title; init NEWLINE */
char *cptr1, *cptr2;  /* general pointers */
/* global parameters */
char fill;         /* fill if TRUE; init TRUE */
char justify;      /* justify if TRUE; init TRUE */
int lsval;         /* current line spacing; init 1 */
int spval;         /* # of lines to space */
int inval;         /* current indent, >  0; init 0 */
int rmval;         /* right margin; init PAGEWIDTH */
int tival;         /* current temp.indent; init 0 */
int ceval;         /* # of lines to center; init 0 */
int ulval;         /* # of lines to underline; init 0 */
char print;        /* suppress printing flag; init FALSE */
char pause;        /* pause after page; init FALSE */
char cmdchar;      /* command line character */
char spacechar;    /* force spaces character; init ~ */
char hyphchar;   /* possible hyphenation indication */
char hyphflag;   /* TRUE if hyphen character detected */
/* output area */
int outp;          /* last char pos in outbuf; init 0 */
int outw;          /* width of text in outbuf; init 0 */
int outwds;        /* number of words in outbuf; init 0 */
int debug;         /* debug program */
char outbuf[STRSIZE]; /* lines to be filled collect here */
char inbuf[STRSIZE];  /* input line */
char dir;          /* direction for blank padding */

/* dbprint -- print if debug option */
dbprint(control, arg) char *control; int arg; {
  if (debug)
    printf(control, arg);
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

/* min --compute minimum of two integers */
min (x, y)
int x,y;
{
   if (x < y)
     return x;
   else
     return y;
}

/* outchar -- send character to STDOUT */
outchar (c)
char c;
{
   if (print)
      putchar(c);
}

/* dopause -- pause until <NEWLINE> entered */
dopause()
{
   while (getchar() != NEWLINE) ;
}

/* skipbl -- skip blanks and tabs at s(.i.)...*/
skipbl (s, i)
char *s; int *i;
{
   while (s[*i] == BLANK || s[*i] == TAB)
      (*i)++;
}

/* skip -- output n blank lines */
skip (n)
int n;
{
   int i;
   for (i = 0; ++i <= n;)
      outchar(NEWLINE);
}

/* gettl -- copy title from buf to ttl */
gettl (buf, ttl, idx)
char *buf, *ttl;
int *idx;
{
   skipbl(buf, idx);      /* find argument */
   if (buf[*idx] == SQUOTE || buf[*idx] == DQUOTE)
      (*idx)++;           /* strip leading quote */
   cptr1 = buf + *idx;
   while (*ttl++ = *cptr1++) ;
}

/* puttl -- put out title line with optional page number */
puttl (buf, pageno)
char *buf;
int pageno;
{
   char c, tbuf[11];
   cptr1 = buf; cptr2 = tbuf;
   while (c = *cptr1++) {
     if (c == PAGENUM) {
       itoa(pageno, cptr2);
       while (c = *cptr2++)
         outchar(c);
     }
     else
       outchar(c);
   }
}

/* puthead -- put out page header */
puthead()
{
   curpage = newpage;
   ++newpage;
   if (pause)
      dopause();
   if (m1val > 0) {
      skip(m1val - 1);
      puttl(header, curpage);
   }
   skip(m2val);
   lineno = m1val + m2val + 1;
}

/* putfoot -- put out page footer */
putfoot()
{
   if (m4val > 0) {
      skip(m3val);
      puttl(footer, curpage);
      outchar(FORMFEED);  /* or skip(m4val-1) */
   }
   else
      outchar(FORMFEED);  /* anyway */
}

/* width -- compute width of character string */
width(buf)
char *buf;
{
   int i, w;
   w = 0;
   hyphflag = FALSE;
   cptr1 = buf;
   while (*cptr1) {
      if (*cptr1 == BACKSPACE)
         --w;
      else if (*cptr1 == hyphchar)
         hyphflag = TRUE;
      else if (*cptr1 != NEWLINE)
         ++w;
      ++cptr1;
   }
   return w;
}

/* put -- put out line with proper spacing and indenting */
put (buf)
char *buf;
{
   int i, j;
   int tabstop;
   dbprint("put: %s\n", buf);
   tabstop = 0;		/* tabstops hard coded at 4 */
   cptr1 = buf;
   if (lineno <= 0 || lineno > bottom)
      puthead();
   for (i=1; i++ <= (inval + tival);)  /* indenting */
      outchar(BLANK);
   tival = 0;
   /* replace spacechar by blanks, remove hyphenchars */
   while (*cptr1) {
      if (*cptr1 == spacechar) {
         *cptr1 = BLANK;
      }
      else if (*cptr1 == hyphchar) {
         cptr2 = cptr1;
         while (*cptr2 = *++cptr2) ;
      }
	  else if (*cptr1 == '\t') {
		  while ((++tabstop % 4) != 0) *cptr1++ = BLANK;
	  }
      ++cptr1;
   }
   if (print) {
      fputs (buf, STDOUT);
	  fputc('\n', STDOUT);
   }
   skip(min(lsval-1, bottom - lineno));
   lineno += lsval;
   if (lineno > bottom)
      putfoot();
}

/* dobreak -- end current filled line */
dobreak()
{
   if (outp > 0) {
      outbuf[outp] = NEWLINE;
      outbuf[outp+1] = ENDSTR;
      put(outbuf);
   }
   outp = outw = outwds = 0;
}

/* space -- space n lines or to bottom of page */
space (n)
int n;
{
   dobreak();
   if (lineno <= bottom) {
      if (lineno <= 0)
         puthead();
     skip(min(n, bottom + 1 - lineno));
     lineno += n;
      if (lineno > bottom)
         putfoot();
   }
}

/* page -- get to top of new page */
page()
{
   dobreak();
   if (lineno > 0 && lineno <= bottom) {
      skip(bottom + 1 - lineno);
      putfoot();
   }
   lineno = 0;
}

/* leadbl -- delete leading blanks, set tival */
leadbl (buf)
char *buf;
{
   dobreak();
   cptr1 = buf;
   while (*cptr1++ == BLANK) ;  /* find first non-blank */
   if (*--cptr1 != NEWLINE)
      tival += (cptr1 - buf);
   if (buf != cptr1) {
     while (*cptr1)             /* move line to left */
       *buf++ = *cptr1++;
     *buf = ENDSTR;
   }
}

/* spread -- spread words to justify right margin */
spread (buf, outp, nextra, outwds)
char *buf;
int  outp, nextra, outwds;
{
   int i, j, nb, nholes;
   if (nextra > 0 && outwds > 1) { /* this test is not needed */
      dir = 1 - dir;      /* reverse previous direction */
      nholes = outwds - 1;
      i = outp - 2;       /* outp is next free position */
      j = min(MAXSTR-2, i + nextra); /* room for NEWLINE */
      while (i < j) {          /* and ENDSTR */
         buf[j] = buf[i];
         if (buf[i] == BLANK) {
            if (dir == 0)
               nb = (nextra - 1) / nholes + 1;
            else
               nb = nextra / nholes;
            nextra -= nb;
            --nholes;
            while (nb-- > 0) {
               buf[--j] = BLANK;
            }
         }
         --i; --j;
      }
   }
}

/* hyphlength -- compute length of string */
hyphlength (s)
char *s;
{
   int m ;
   m = 0;
   while (*s) {
      if (*s++ != hyphchar)
         ++m;
   }
   return m;
}

/* hyphcopy -- copy string at src[0] to dest [j],
  without dobreakcharacters
*/
hyphcopy (src, dest, j)
char *src, *dest;
int j;
{
   char c;
   cptr1 = src; cptr2 = dest + j;
   while (c = *cptr1++) {
      if (c != hyphchar) {
         *cptr2++ = c;
      }
   }
   *cptr2 = ENDSTR;
}

/* f -- put word in outbuf; does margin justification */
putword (wordbuf)
char *wordbuf;
{
   int last, l1val, nextra, w, i, j, k;
   char tbuf[STRSIZE];
   char dohyphen;
   dbprint("putword: %s\n", wordbuf);
   w = width(wordbuf);
   last = hyphlength(wordbuf) + outp; /* new begin of outbuf */
   l1val = rmval - tival - inval;
   if (outp > 0 &&
         (outw+w > l1val || last >= MAXSTR)) {
      last -= outp;
      nextra = l1val - outw + 1;
      if (hyphflag) {
         j = hyphlength(wordbuf) - 1;
         dohyphen = FALSE;
         while (j>0 && !dohyphen) {
            if (wordbuf[j] == hyphchar) {
               cptr1 = wordbuf; cptr2 = tbuf;
               while (*cptr2++ = *cptr1++) ;
               tbuf[j] = MINUS;
               tbuf[j + 1] = ENDSTR;
               k = outw + width(tbuf);
               if (k <= l1val) {
                  hyphcopy(tbuf, outbuf, outp);
                  nextra = l1val - k;
                  outp += (1 + hyphlength(tbuf));
                  cptr1 = wordbuf + j + 1; cptr2 = tbuf;
                  while (*cptr2++ = *cptr1++) ;
                  cptr1 = tbuf; cptr2 = wordbuf;
                  while (*cptr2++ = *cptr1++);
                  last = hyphlength(wordbuf) + 1;
                  dohyphen = TRUE;
                  ++outwds;
                  w = width(wordbuf);
               }
            }
            --j;
         }
      }
      if (nextra>0  &&  outwds>1  &&  justify) {
         spread(outbuf, outp, nextra, outwds);
         outp += nextra - 1;  /* set to first free */
      }
      dobreak();
   }
   hyphcopy(wordbuf, outbuf, outp);
   outp = last;
   outbuf[outp++] = BLANK;    /* blank between words */
   outw += w + 1;
   ++outwds;
}

/* getaword -- get word from s[i] into out */
getaword (s, i, out)
char *s, *out;
int i;
{
   cptr1 = s + i; cptr2 = out;
   while (*cptr1 == BLANK || *cptr1 == TAB || *cptr1== NEWLINE)
      ++cptr1;
   while (*cptr1 != ENDSTR  && *cptr1 != BLANK &&
          *cptr1 != TAB  && *cptr1 != NEWLINE) {
      *cptr2++ = *cptr1++;
   }
   *cptr2 = ENDSTR;
   if (*cptr1 == ENDSTR)
      return 0;
   else
      return (cptr1 - s);
}

/* center -- center a line by setting tival */
center (buf)
char *buf;
{
   tival = max((rmval-inval + tival - width(buf)) / 2, 0);
}

/* underln -- underline a line */
underln (buf, size)
char *buf;
int size;
{
   int i, j;
   char tbuf[STRSIZE];
   char first;
   cptr1 = tbuf;               /* get address */
   cptr2 = cptr1 + MAXSTR - 1; /* end address */
   j = 0;       /* expand into tbuf */
   i = 0;
   first = TRUE;
   while (buf[i] != NEWLINE  &&  cptr1 < cptr2 - 1) {
      if (first  &&  buf[i] != BLANK)
         first = FALSE;
      if (!first) {
         *cptr1++ = UNDERLINE;
         *cptr1++ = BACKSPACE;
      }
      *cptr1++ = buf[i++];
   }
   if (cptr1 > cptr2 - 1)
      cptr1  = cptr2 - 1;
   *cptr1++ = NEWLINE;
   *cptr1 = ENDSTR;
   cptr1 = tbuf; cptr2 = buf;
   while (*cptr2++ = *cptr1++) ;  /* copy it back to buf */
}

/* text -- process text lines */
text (inbuf)
char *inbuf;
{
   char wordbuf[STRSIZE];
   int i;
   if (*inbuf == BLANK || *inbuf == NEWLINE)
      leadbl(inbuf);           /* move left, set tival */
   if (ulval > 0) {   /* underlining */
      underln(inbuf, MAXSTR);
      ulval--;
   }
   if (ceval > 0) {   /* centering */
      center(inbuf);
      put(inbuf);
      ceval--;
   }
   else if (*inbuf == NEWLINE) /* empty line */
      put(inbuf);
   else if (!fill)               /* unfilled text */
      put(inbuf);
   else {                         /* filled text */
      i = 0;
      do {
         i = getaword(inbuf, i, wordbuf);
         if (i > 0)
            putword(wordbuf);
      }
      while (i != 0);
   }
}

/* getcommand -- decode command type */
getcommand (buf, idx)
char *buf;
int *idx;
{
   char cmd[3];
   int val;
   cmd[0] = tolower(buf[(*idx)+1]);
   cmd[1] = tolower(buf[(*idx)+2]);
   cmd[2] = ENDSTR;
   if (!strcmp (cmd, "fi"))
      val = FI;
   else if (!strcmp(cmd, "nf"))
      val = NF;
   else if (!strcmp(cmd,  "br"))
      val = BR;
   else if (!strcmp(cmd,  "ls"))
      val = LS;
   else if (!strcmp(cmd,  "bp"))
      val = BP;
   else if (!strcmp(cmd,  "sp"))
      val = SP;
   else if (!strcmp(cmd,  "in"))
      val = IND;
   else if (!strcmp(cmd,  "rm"))
      val = RM;
   else if (!strcmp(cmd,  "ti"))
      val = TI;
   else if (!strcmp(cmd,  "ce"))
      val = CE;
   else if (!strcmp(cmd,  "ul"))
      val = UL;
   else if (!strcmp(cmd,  "he"))
      val = HE;
   else if (!strcmp(cmd,  "fo"))
      val = FO;
   else if (!strcmp(cmd,  "pl"))
      val = PL;
   else if (!strcmp(cmd,  "cp"))
      val = CP;
   else if (!strcmp(cmd,  "pa"))
      val = PA;
   else if (!strcmp(cmd,  "ju"))
      val = JU;
   else if (!strcmp(cmd,  "nj"))
      val = NJ;
   else if (!strcmp(cmd,  "bm"))
      val = BM;
   else if (!strcmp(cmd,  "tm"))
      val = TM;
   else if (!strcmp(cmd,  "pr"))
      val = PR;
   else if (!strcmp(cmd,  "np"))
      val = NP;
   else if (!strcmp(cmd,  "fs"))
      val = FS;
   else if (!strcmp(cmd,  "fc"))
      val = FC;
   else if (!strcmp(cmd,  "fh"))
      val = FH;
   else
      val = UNKNOWN;
   /* skip command, set idx */
   while (isalpha(buf[ ++(*idx) ])) ;
   return val;
}

/* setparam -- set parameters and check range */
setparam (param, val, argtype, defval, minval, maxval)
int *param;
int val, argtype, defval, minval, maxval;
{
   if (argtype == NEWLINE)        /* defaulted */
      *param = defval;
   else if (argtype == PLUS)      /* relative + */
      *param += val;
   else if (argtype == MINUS)     /* relative - */
      *param -= val;
   else                           /* absolute */
      *param = val;
   param = min(*param, maxval);
   param = max(*param, minval);
}

/* getval -- evaluate optional numeric argument */
getval (buf, idx, argtype)
char *buf;
int *idx,*argtype;
{
   int nbr, i;
   skipbl(buf, idx);      /* find argument */
   *argtype = buf[*idx];
   if (*argtype == PLUS || *argtype == MINUS)
      ++(*idx);
   nbr = atoi(&buf[idx]);
   while (isdigit(buf[*idx])) ++(*idx);
   return nbr;
}

/* command -- perform formatting command */
command (buf)
char *buf;
{
   int cmd;
   int argtype, val, index, len;
   index = 0;
   cptr1 = buf; while (*cptr1++) ;
   len = cptr1 - buf - 1;
   while (buf[index] == cmdchar && index < len) {
      cmd = getcommand(buf, &index);
      dbprint("command: %d\n", cmd);
      if (cmd != UNKNOWN)
         val = getval(buf, &index, &argtype);
      switch (cmd) {
      case FI:{
        dobreak(); fill = TRUE; break;
      }
      case NF:{
        dobreak(); fill = FALSE; break;
      }
      case BR:{
        dobreak();
        break;
      }
      case LS:{
        setparam(&lsval, val, argtype, 1, 1, HUGE);
        break;
      }
      case CE:{
        dobreak();
        setparam(&ceval, val, argtype, 1, 0, HUGE);
        break;
      }
      case UL:{
        setparam(&ulval, val, argtype, 1 , 0, HUGE);
        break;
      }
      case HE:{
        gettl(buf, header, &index);
        index = len + 1;  /* set behind command */
        break;
      }
      case FO:{
        gettl(buf, footer, &index);
        index = len + 1;  /* set behind command */
        break;
      }
      case BP:{
        page();
        setparam(&curpage, val, argtype, curpage+1, -HUGE, HUGE);
        newpage = curpage;
        break;
      }
      case SP:{
        setparam(&spval, val, argtype, 1, 0, HUGE);
        space(spval);
        break;
      }
      case IND:{
        setparam(&inval, val ,argtype, 0, 0, rmval-1);
        break;
      }
      case RM:{
        setparam(&rmval, val, argtype, PAGEWIDTH,
                 inval+tival+1, HUGE);
        break;
      }
      case TI:{
        dobreak();
        setparam(&tival, val, argtype, 0, -HUGE, rmval);
        break;
      }
      case PL:{
        setparam(&plval, val, argtype, PAGELEN,
                 m1val + m2val + m3val + m4val + 1, HUGE);
        bottom = plval - m3val - m4val;
        break;
      }
      case CP:{
        dobreak();
        setparam(&spval, val, argtype, 1, 0, HUGE);
        if (spval > bottom - lineno)
          page();
        break;
      }
      case PA:{
        pause = TRUE;
        break;
      }
      case JU:{
        justify = TRUE;
        break;
      }
      case NJ:{
        justify = FALSE;
        break;
      }
      case BM:{
        setparam(&m4val, val, argtype, 3, 0, plval-1);
        bottom = plval - m3val - m4val;
        break;
      }
      case TM:{
        setparam(&m1val, val, argtype, 3, 0, plval-1);
        break;
      }
      case NP:{
        print = FALSE;
        break;
      }
      case PR:{
        print = TRUE;
        break;
      }
      case FS:{
        spacechar = argtype;  /* ascii char */
        ++index;
        break;
      }
      case FC:{
        cmdchar = argtype;  /* ascii char */
        ++index;
        break;
      }
      case FH:{
        hyphchar = argtype;  /* ascii char */
        ++index;
        break;
      }
/*    case UNKNOWN: ;   ignore or print message */
      }       /* end of case */
      skipbl(buf, &index);
   }
}

/* inifmt -- initiate format variables */
inifmt()
{
   fill =justify = TRUE;
   dir =
   inval =
   tival =
   spval =
   ceval =
   ulval =
   lineno =
   curpage = 0;
   rmval = PAGEWIDTH;
   lsval =
   newpage = 1;
   plval = PAGELEN;
   m1val = 3; m2val = 2; m3val = 2; m4val = 3;
   bottom = plval - m3val - m4val;
   header[0] = NEWLINE; header[1] = ENDSTR;
   footer[0] = NEWLINE; footer[1] = ENDSTR;
   outp = outw = outwds = 0;
   pause = FALSE;
   print = TRUE;
   cmdchar = PERIOD;
   spacechar = TILDE;
   hyphchar = NULL;
}

/* format start */
main(argc, argv) int argc, *argv; {
   debug=0;
   if (argc > 1) ++debug;
   inifmt();
   while (fgets (inbuf, MAXSTR, STDIN)) {
     dbprint("input line: %s\n",inbuf);
     if (*inbuf == cmdchar)
         command(inbuf);
      else
         text(inbuf);
   }
   page();   /* flush last output, if any */
   if (pause)
      dopause();
}

 