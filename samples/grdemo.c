/* program to test circle and fill functions */
extern
 oputc(), circle(), fill();


main() begin
  int x, y;
  oputc(28); oputc(31);
  circle(1, 15, 5, 10);
  circle(1, 105, 8, 15);
  circle(1, 20, 35, 14);
  circle(1, 110, 35, 9);
  circle(1, 64, 24, 44);
  fill(1, 15, 5);
  fill(1, 105, 8);
  fill(1, 20, 35);
  fill(1, 110, 35);
  fill(1, 64, 24);
  circle(0, 64, 24, 34);
  circle(0, 64, 24, 17);
end
            