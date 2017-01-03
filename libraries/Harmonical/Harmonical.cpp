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
  #include <Pulses.h>
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
