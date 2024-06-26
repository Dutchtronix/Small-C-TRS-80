/* { } \n [ ]  */
#include <stdio/h>

/* flags for destination */
int xxpff, xxpfd;

/*
 * Module printf/fprintf
 * Formatted print TRS-80 Small-C V2.1
 * specifications c, d, o, s, u, x are supported
 */
printf(ctrl, arg) char *ctrl; int arg; {
  xxpff = 1; xxpfd = stdout;
  xxprtfxx(ctrl, &arg);
}

fprintf(fd, ctrl, arg) FILE *fd; char *ctrl; int arg; {
  xxpff = 1; xxpfd = fd;
  xxprtfxx(ctrl, &arg);
}

sprintf(s, ctrl, arg) char *s; char *ctrl; int arg; {
  xxpff = 0; xxpfd = s;
  xxprtfxx(ctrl, &arg);
}
xxputc(ch) char ch; {
  if (xxpff)
    fputc(ch, xxpfd);
  else
    *xxpfd++ = ch;
}

xxprtfxx(ctrl, nxtarg) char *ctrl; int *nxtarg; {
  int arg, left, pad, cc, len, maxchr, width;
  char *sptr, str[17];
  cc = 0;
  while (*ctrl) {
    if (*ctrl != '%') {
      xxputc(*ctrl++); ++cc; continue;
    }
    else ++ctrl;
    if (*ctrl == '%') {
      xxputc(*ctrl++); ++cc; continue;
    }
    if (*ctrl == '-') {
      left = 1; ++ ctrl;
    }
    else left = 0;
    if (*ctrl == '0') pad = '0';
    else pad = ' ';
    ctrl += utoi(ctrl, &width);
    if (*ctrl == '.') {
      ++ctrl;
      ctrl += utoi(ctrl, &maxchr);
    }
    else
      maxchr = 0;
    arg = *nxtarg++;
    sptr = str;
    switch (*ctrl++) {
      case 'c':
      case 'C': str[0] = arg; str[1] = NULL; break;
      case 's':
      case 'S': sptr = arg; break;
      case 'd':
      case 'D': itoa(arg, str); break;
      case 'o':
      case 'O': itoab(arg, str, 8); break;
      case 'u':
      case 'U': itoab(arg, str, 10); break;
      case 'x':
      case 'X': itoab(arg, str, 16); break;
      default: return(cc);
    }
    len = strlen(sptr);
    if (maxchr && maxchr < len) len = maxchr;
    if (width > len)
      width -= len;
    else width = 0;
    if (!left)
      while (width--) { xxputc(pad); ++cc; }
    while (len--) { xxputc(*sptr++); ++cc; }
    if (left)
      while (width--) { xxputc(pad); ++cc; }
  }
  return(cc);
}
                    