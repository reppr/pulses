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

// return error codes <0 from  prime_factors()
#define PF_0		-3	// zero
#define PF_1		-2	// one
#define PF_ILLEGAL	-1	// invalid
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
     223, 227, 229, 233, 239, 241, 251};

unsigned short Harmonical::small_primes_cnt=(sizeof(small_primes) / sizeof(small_primes[0]));

unsigned short Harmonical::USED_primes=6;	// most of the time even 3 is enough for me ;)

unsigned int Harmonical::GCD (unsigned int a, unsigned int b) {	// greatest common divisor, Euklid
  unsigned int m;

  while (b) {
    m = a % b;
    a = b;
    b = m;
  }
  return a;
}


unsigned long Harmonical::LCM(unsigned int a, unsigned int b) {	// least common multiple
  return a * b / GCD(a, b);
}


bool Harmonical::fraction_LCM(fraction_t *F, fraction_t *LCM_) {	// returns error
  reduce_fraction(F);
  reduce_fraction(LCM_);
  (*LCM_).multiplier = LCM((*F).multiplier, (*LCM_).multiplier);
  (*LCM_).divisor = GCD((*F).divisor, (*LCM_).divisor);
  reduce_fraction(LCM_);
}


void Harmonical::reduce_fraction(fraction_t *f) {
  unsigned int d = GCD(f->multiplier, f->divisor);

  (*f).multiplier /= d;
  (*f).divisor /= d;
}


void Harmonical::expand_fractions(fraction_t * a, fraction_t * b) {
  unsigned long l = LCM(a->divisor, b->divisor);
  unsigned int f;
  f =  l / a->divisor ;
  (*a).multiplier *= f;
  (*a).divisor    *= f;
  f =  l / b->divisor ;
  (*b).multiplier *= f;
  (*b).divisor *= f;
}


fraction_t* Harmonical::add_fraction(fraction_t * delta, fraction_t * sum) {
  expand_fractions(delta, sum);
  (*sum).multiplier += (*delta).multiplier;
  reduce_fraction(sum);
  return sum;
}


int Harmonical::prime_factors(unsigned int p_factors[], unsigned long number) {
/*
  return values:
  -3	PF_0	number == 0
  -2	PF_1	number == 1
  -1	ILLEGAL not enough small primes (or negative number)
  0	index of highest prime	0 ~ 2
  1	index of highest prime	0 ~ 3
  2	index of highest prime	0 ~ 5
  3	index of highest prime	0 ~ 5
  ...	...
*/

  // special cases: negative, zero, one
  if(number < 0)
    return -1;		// number is negative, ILLEGAL, abort
  if(number==0)
    return PF_0;	// 0
  if(number==1)		// 1
    return PF_1;

  int f=0;		// write index  for p_factors[f]
  while (number>1) {
    for(int p=0; p<small_primes_cnt; p++) {	// loop over small_primes[]
      if(p == (USED_primes + 1))
	//	(*MENU).out(F("[!]"));			// "[!]" need more primes then USED_primes
	Serial.print(F("[!]"));			// "[!]" need more primes then USED_primes
      p_factors[f]=0;
      while ((number % small_primes[p]) == 0) {
	p_factors[f]++;
	number /= small_primes[p];
	//	Serial.print("number: "); Serial.print(number); Serial.print("	prime: "); Serial.print(small_primes[p]);
	//	Serial.print("	cnt: "); Serial.println(*p_factors[f]);
	if (number==1) {
	  //	  Serial.print(small_primes[p]); Serial.print('	'); Serial.println(*p_factors[f]);
	  p_factors[f+1]=0;
	  return p;				// OK, DONE:  return *INDEX of HIGHEST PRIME* used
	}
      }
      f++;
    } // loop over small_primes[]
    return ILLEGAL;	// not enough small primes, ILLEGAL, abort
  }
}


bool Harmonical::is_small_prime(unsigned long number) {
  for(int i=0; i<small_primes_cnt; i++)
    if(number == small_primes[i])
      return true;

  return false;
}
