/*
** ctype -- char manipulation in C
*/

isalpha(c) {
  return (isupper (c) || islower (c));
}

isupper(c) {
  return ('A' <= c && c <= 'Z');
}

islower(c) {
  return ('a' <= c && c <= 'z');
}

isdigit(c) {
  return ('0' <= c && c <= '9');
}

isspace(c) {
  return (c<=' ' && (c==' ' || (c<= '\n' && c >= '\t')));
}

ispunct(c) {
  return (!isalnum(c) && !iscntrl(c));
}

isalnum(c) {
  return (isalpha(c) || isdigit(c));
}

iscntrl(c) char *c; {  /* c is simulated unsigned */
  return (c < ' ' || c == 127);
}

isprint(c) {
  return (!iscntrl(c));
}

isascii (c) char *c; {  /* c is simulated unsigned */
  return (c < '\200');
}

toascii(c) {
  return c;
}

tolower(c) {
  if (isupper(c))
    return (c + ('a' - 'A'));
  else
    return (c);
}

toupper(c) {
  if (islower(c))
    return (c - ('a'-'A'));
  else
    return (c);
}
 