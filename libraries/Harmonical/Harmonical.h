/* **************************************************************** */
/*
   Harmonical.h
*/
/* **************************************************************** */
#ifndef HARMONICAL_H
#define HARMONICAL_H

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


#if ! defined ILLEGAL32
  #define ILLEGAL32	0xffffffff
#endif


/* **************************************************************** */

class Harmonical {

 public:
  Harmonical(unsigned long harmonical_base);
  ~Harmonical();

  typedef struct fraction_t {
    unsigned int multiplier;
    unsigned int divisor;
  } fraction_t;

  unsigned int GCD(unsigned int a, unsigned int b);		// greatest common divisor, Euklid
  unsigned long LCM(unsigned int a, unsigned int b);		// least common multiple
  bool fraction_LCM(fraction_t* F, fraction_t*LCM);	// LCM for fractions, returns error
  void reduce_fraction(fraction_t*f);
  void expand_fractions(fraction_t* a, fraction_t* b);
  fraction_t* add_fraction(fraction_t* delta, fraction_t* sum);
  int prime_factors(unsigned int p_factors[], unsigned long number);	// returns index-1 of highest used prime, -1 is error
  bool is_small_prime(unsigned long number);

  unsigned long harmonical_base;
  static const uint8_t small_primes[];
  static unsigned short small_primes_cnt;	// number of known small primes
  static unsigned short USED_primes;		// *USED* small primes
};


/* **************************************************************** */
#endif // HARMONICAL_H
