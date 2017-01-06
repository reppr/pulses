/* **************************************************************** */
/*
  Harmonical.cpp
*/
/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#include <Harmonical.h>

#ifdef ARDUINO

  /* Keep ARDUINO GUI happy ;(		*/
  #if ARDUINO >= 100
    #include "Arduino.h"
  #else
    #include "WProgram.h"
  #endif

//	  #include <Menu.h>
//	  #include <Pulses.h>
#else	// PC version *NOT SUPPORTED*
  #include <iostream>

//	  #include <Menu/Menu.h>
//	  #include <Pulses/Pulses.h>
#endif


/* **************************************************************** */
// Constructor/Destructor:

Harmonical::Harmonical(unsigned long harmonical_base):
  harmonical_base(harmonical_base)
{
  ;
}

Harmonical::~Harmonical() {
  ;
}

const uint8_t Harmonical::small_primes[] =
    {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61,
     67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137,
     139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211,
     223, 227, 229, 233, 239, 241, 251, 0};

unsigned int Harmonical::GCD (unsigned int a, unsigned int b) {	// greatest common divisor, Euklid
  unsigned int m;

  while (b) {
    m = a % b;
    a = b;
    b = m;
  }
  return a;
}


unsigned int Harmonical::LCM(unsigned int a, unsigned int b) {	// least common multiple
  return a * b / GCD(a, b);
}


void Harmonical::reduce_fraction(struct fraction *f) {
  unsigned int d = GCD(f->multiplier, f->divisor);

  (*f).multiplier /= d;
  (*f).divisor /= d;
}


void Harmonical::expand_fractions(struct fraction * a, struct fraction * b) {
  unsigned int l = LCM(a->divisor, b->divisor);
  unsigned int f;
  f =  l / a->divisor ;
  (*a).multiplier *= f;
  (*a).divisor    *= f;
  f =  l / b->divisor ;
  (*b).multiplier *= f;
  (*b).divisor *= f;
}


struct fraction* Harmonical::add_fraction(struct fraction * delta, struct fraction * sum) {
  expand_fractions(delta, sum);
  (*sum).multiplier += (*delta).multiplier;
  reduce_fraction(sum);
  return sum;
}
