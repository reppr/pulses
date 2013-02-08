/*
  out.cpp
  Testing overloaded out() function:
*/

#include <stdio.h>


void out(char c)		{ putchar(c); }
void out(int i)			{ printf("%i", i); }
void out(long l)		{ printf("%d", l); }
void out(const char *str)	{ printf("%s", str); }

// End of line versions:
void outln(char c)		{ printf("%c\n", c); }
void outln(int i)		{ printf("%i\n", i); }
void outln(long l)		{ printf("%d\n", l); }
void outln(const char *str)	{ printf("%s\n", str); }

/*
  out(xxx, char c)
  A char as a second parameter get's printed after first argument.
  like:  out(string, '\t');				*/
void out(char c, char x)	{ printf("%c%c", c, x); }
void out(int i, char x)		{ printf("%i%c", i, x); }
void out(long l, char x)	{ printf("%d%c", l, x); }
void out(const char *str, char x)	{ printf("%s%c", str, x); }

void ln()			{ putchar('\n'); }
void tab()			{ putchar('\t'); }
void space()			{ putchar(' '); }

void ticked(char c)	{ printf("\'%c\'", c); } // prints 'c'


int main() {
  char c = 'X';
  const char b = 'B';	// const char works without warning now.
  int i = 42;
  long l = 999999999L;

  out('%'); ln();

  /*  char *str =      "Stringedibim";	// Gives a warning.	*/
  char *str = (char *) "Stringedibim";	// (char *) avoids warning

  char gu[] = "guguseli...";
  char bye[]= "and bye...";

  out(c); space(); out((int) c); tab(); ticked(c); ln();
  out(b); space(); out((int) b); tab(); out((long) b); ln();
  out(i); tab();

  // out("\tThat was integer.\n");		// Gives a warning.
  out((char *) "\tThat was integer.\n");	// (char *) avoids warning
  out((char *) "ok?\n");			// (char *) avoids warning

  out("That gave a warning.\n");		// Gave a warning.

  outln(l);

  out(str); ln();

  out(gu); ln();

  out(bye, '\t'); outln(bye);

  return 0;
}
