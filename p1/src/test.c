#include <stdio.h>

int main (int argc, char *argv[]) {
  char a[12] = "Hola Mundo!";
  char *b, *c;

  c = a;       putchar(*c);
  b = ++c;     putchar(*b);
  b = (a+2);   putchar(*b);
  b = (*c)+2;  putchar(*b);

  return 0;
}