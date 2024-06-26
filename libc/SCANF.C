/* { } 1>\n [ ]  */
#include <stdio/h>

/* source status words */
int xxsff;
char *xxsfd;

/*
 * Scanf(ctrl, arg)
 * Formatted read from stdin
 *   specifications c, d, o, s, u, x
 */
Scanf(ctrl, arg) char *ctrl; int arg; {
  xxsff = 1; xxsfd = stdin;
  xxscnfxx(ctrl, &arg);
}

/*
 * Fscanf(fd, ctrl, arg)
 * Formatted read from file
 *   specifications c, d, o, s, u, x
 */
Fscanf(fd, ctrl, arg) FILE *fd; char *ctrl; int arg; {
  xxsff = 1; xxsfd = fd;
  xxscnfxx(ctrl, &arg);
}

/*
 * Sscanf(str, ctrl, arg)
 * Formatted read from string
 *   specifications c, d, o, s, u, x
 */
Sscanf(str, ctrl, arg) char *str; char *ctrl; int arg; {
  xxsff = 0; xxsfd = str;
  xxscnfxx(ctrl, &arg);
}

xxgetc() {
  if (xxsff)
    return fgetc(xxsfd);
  else
    return *xxsfd++;
}


xxugetc(ch) char ch; {
  if (xxsff)
    ungetc(ch, xxsfd);
  else
    --xxsfd;
}

/*
 * xxscnfxx(ctrl, nxtarg)
 * internal function for formatted input
 */
xxscnfxx(ctrl, nxtarg) char *ctrl; int *nxtarg; {
  char *carg, *usigned;
  int *narg, wast, ac, width, ch, cnv, base, ovfl, sign;
  ac = 0;
  while (*ctrl) {
    if (isspace(*ctrl)) { ++ctrl; continue; }
    if (*ctrl++ != '%') continue;
    if (*ctrl == '*') {
      narg = carg = &wast; ++ctrl;
    }
    else
      narg = carg = *nxtarg++;
    ctrl += utoi(ctrl, &width);
    if (!width) width = 32767;
    if (!(cnv = *ctrl++)) break;
    while (isspace(ch = xxgetc()) && ch != '\n') ;
    if (ch == EOF)
      if (ac) break; else return(EOF);
    xxugetc(ch);
    switch(cnv) {
      case 'c':
      case 'C': *carg = xxgetc(); break;
      case 's':
      case 'S':
        while (width--) {
          if ((*carg = xxgetc()) == EOF) break;
          if (isspace(*carg) || *carg == '\0') break;
          if (carg != &wast) ++carg;
        }
        *carg = '\0';
        break;
      default: {
        switch (cnv) {
          case 'd':
          case 'D': base = 10; sign = 0; ovfl = 3276; break;
          case 'o':
          case 'O': base =  8; sign = 1; ovfl = 8191; break;
          case 'u':
          case 'U': base = 10; sign = 1; ovfl = 6553; break;
          case 'x':
          case 'X': base = 16; sign = 1; ovfl = 4095; break;
          default: return(ac);
        }
        *narg = usigned = 0;
        while (width-- && !isspace(ch=xxgetc())
                   && ch != '\0' && ch != EOF) {
          if (!sign)
            if (ch == '-') {
              sign = -1; continue;
            }
            else sign = 1;
          if (ch < '0') return(ac);
          if (ch >= 'a')
            ch -= ('a'-':');
          else if (ch >= 'A')
            ch -= ('A' - ':');
          ch -= '0';
          if (ch >= base || usigned > ovfl) return(ac);
          usigned = usigned * base + ch;
        }                                  /* end while */
        *narg = sign * usigned;
      }                                    /* end default */
    }                                      /* end switch */
    ++ac;
  }                                        /* end while */
  return (ac);
}
 