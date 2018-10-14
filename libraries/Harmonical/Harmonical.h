/* **************************************************************** */
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


#if ! defined ILLEGAL
  #define ILLEGAL	~0
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

  unsigned int GCD(unsigned int a, unsigned int b);		// greatest common divisor, Euklid
  unsigned long LCM(unsigned int a, unsigned int b);		// least common multiple
  bool fraction_LCM(struct fraction * F, struct fraction *LCM);	// LCM for fractions, returns error
  void reduce_fraction(struct fraction *f);
  void expand_fractions(struct fraction * a, struct fraction * b);
  fraction * add_fraction(struct fraction * delta, struct fraction * sum);
  int prime_factors(unsigned int p_factors[], unsigned long number);	// returns index-1 of highest used prime, -1 is error

  unsigned long harmonical_base;
  static const uint8_t small_primes[];
  static unsigned short small_primes_cnt;	// number of known small primes
  static unsigned short USED_primes;		// *USED* small primes
};


/* **************************************************************** */
#endif HARMONICAL_h
