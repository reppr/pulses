#/* **************************************************************** */
/*
   Harmonical.h
*/
/* **************************************************************** */
#ifndef HARMONICAL_h
#define HARMONICAL_h

#include <stdio.h>
#include <stdlib.h>

#ifdef ARDUINO
  #define STREAMTYPE	Stream

  /* Keep ARDUINO GUI happy ;(				*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif
#else
  #include <iostream>

  #ifndef STREAMTYPE		// could have been #define'd in Pulses.h
    using namespace std;
    ostream & Serial=cout;	// nice trick from johncc
  #endif
  #define STREAMTYPE	ostream
#endif


/* **************************************************************** */
struct fraction {
  unsigned int multiplier;
  unsigned int divisor;
};

/* **************************************************************** */
class Harmonical {

 public:
  Harmonical(unsigned long harmonical_base);
  ~Harmonical();

  unsigned int GCD(unsigned int a, unsigned int b);	// greatest common divisor, Euklid
  unsigned int LCM(unsigned int a, unsigned int b);	// least common multiple
  void reduce_fraction(struct fraction *f);
  void expand_fractions(struct fraction * a, struct fraction * b);
  fraction * add_fraction(struct fraction * delta, struct fraction * sum);

  unsigned long harmonical_base;
  static const uint8_t small_primes[];
};


/* **************************************************************** */
#endif HARMONICAL_h
