/*
  Circle - fast circle plotting by Scott A. Loomer
  circle (funcod, x, y, r)
          funcod : 0 or 1
          x,y    : coordinates
          r      : radius
          return : -3 if parameter error
                   -1 if circle too large
                   else funcod
*/
circle(funcod, x1, y1, r1) char *r1; {
  int d, dx, dy;
  if (funcod > 1 || funcod < 0)
    return(-3);
  dx = 0; dy = r1; d = 3 - (dy << 1);
  while (dx < dy) {
    crlpoints(funcod, x1, y1, dx, dy);
    if (d < 0)
      d = d + (dx << 2) + 6;
    else {
      d = d + ((dx - dy) << 2) + 10;
      --dy;
    }
    ++dx;
  }
  if (dx == dy)
    crlpoints(funcod, x1, y1, dx, dy);
  d = r1 >> 1;  /* must be unsigned! */
  if ((x1 + r1) > 127 || (x1 - r1) < 0 ||
      (y1 + d) > 47 || (y1 - d) < 0)
    return -1;
  else
    return(funcod);
}

crlpoints(funcod, x, y, dx, dy) char *dx, dy; {
  int tdy, ty;
  tdy = dy >> 1;
  if (funcod == 1) {
    set(x+dx, y+tdy);
    set(x+dx, y-tdy);
    set(x-dx, y-tdy);
    set(x-dx, y+tdy);
  }
  else {
    reset(x+dx, y+tdy);
    reset(x+dx, y-tdy);
    reset(x-dx, y-tdy);
    reset(x-dx, y+tdy);
  }
  tdy = dx >> 1;
  if (funcod == 1) {
    set(x+dy, y+tdy);
    set(x+dy, y-tdy);
    set(x-dy, y-tdy);
    set(x-dy, y+tdy);
  }
  else {
    reset(x+dy, y+tdy);
    reset(x+dy, y-tdy);
    reset(x-dy, y-tdy);
    reset(x-dy, y+tdy);
  }
}

/*
  Fill -- non-recursive fill algorithm by Scott A. Loomer
  fill(funcod, x,y)
       funcod : 0 or 1, must be equal to the value
                        of the border character
       x,y    : a coordinate in the area, not on the
                border
*/
int gfltop, gflsx[128], gflsy[128];

fill(funcod, x, y) {
  int idx, max, min, x1, y1;
  gfltop = 1;
  while (gfltst(funcod, ++x, y)) ;
  gflpsh(--x, y);
  while (gflpop(&x, &y)) {
    max = x;
    do
      if (funcod = 1)
        set(x, y);
      else
        reset(x, y);
    while (gfltst(funcod, --x, y));
    min = x++;
    for (idx = 1; idx >= -1; idx -= 2) {
      y1 = y + idx;
      x1 = max;
      while (x1 > min) {
        if (gfltst(funcod, x1, y1)) {
          while (gfltst(funcod, ++x1, y1)) ;
          gflpsh(--x1, y1);
          while (gfltst(funcod, --x1, y1)) ;
        }
        --x1;
      }
    }
  }
}

gfltst(bordchar, x, y) {
  return (point(x, y) != bordchar &&  inscreen(x, y));
}

gflpsh(x, y) {
  gflsx[gfltop] = x;
  gflsy[gfltop++] = y;
}

gflpop(x, y) int *x, *y; {
  *x = gflsx[--gfltop];
  *y = gflsy[gfltop];
  return gfltop;
}

inscreen(x, y) {
  return (x >=0 && x <= 127 && y >=0 && y <= 47);
}
 