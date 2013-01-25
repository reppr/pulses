#include <stdlib.h>
#include <iostream>

#include <stdio.h>
// #include <termios.h>

void unbuffered() {
  printf("Set stream to unbuffered mode  ");
  if (setvbuf(stdin, NULL, _IONBF, 0) == 0)
    printf("UNBUFFERED\n");
  else
    printf("failed!!!\n");
}

void line_buffered() {
  printf("Set stream to line buffered mode  ");
  if (setvbuf(stdin, NULL, _IOLBF, 0) == 0)
    printf("LINE BUFFERED\n");
  else
    printf("failed!!!\n");
}

bool is_char() {
  int INP = getchar_unlocked();
  if ( INP == EOF )
    return false;

  char c = INP;
  std::cout << c;
  return true;
}

char scratch[80];

int main() {
  std::cout << "testing unbuffered input: (press keys)\n";

  int INP;
  char c;

  unbuffered();
/*
  char * p;

  p = scratch;
  *p=0;
  std::cout << "testing cin.get\t\t: ";
  std::cin.get(scratch, 79);

  std::cout << "cin.get received\t: ";
  while (c = *p++)
    std::cout << c;
  std::cout << "\n";
*/

/*
  std::cout << "testing getchar()\t\t: ";

  std::cout << "getchar() received\t: ";
  while (INP = getchar() != EOF ) {
    c = INP;
    std::cout << c;
  }
  std::cout << "\n";


  std::cout << "testing getchar_unlocked()\t\t: ";

  std::cout << "getchar_unlocked() received\t: ";
  while (INP = getchar_unlocked() != EOF ) {
    c=INP;
    std::cout << c;
  }
  std::cout << "\n";
*/

  while (true) {
    if ( is_char() )
      std::cout << "*";
  }


  line_buffered();
  std::cout << "(done)\n";
  return 0;
}
