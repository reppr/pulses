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


bool Harmonical::fraction_LCM(fraction *F, fraction *LCM) {		// returns error
  unsigned int LCM_mul_prime_factors[small_primes_cnt];		// LCM storage for prime factors multipliers
  unsigned int LCM_div_prime_factors[small_primes_cnt];		// LCM storage for prime factors divisors
  unsigned int F_mul_prime_factors[small_primes_cnt];		// new fraction F storage
  unsigned int F_div_prime_factors[small_primes_cnt];

  reduce_fraction(LCM);

  // init prime factors of existing LCM
  int PF_LCM_mul_return;
  if((PF_LCM_mul_return = prime_factors(LCM_mul_prime_factors, (*LCM).multiplier)) == -1)
    return true;	// ERROR
  if(PF_LCM_mul_return == PF_0)
    return true;	// error

  int PF_LCM_div_return;
  if((PF_LCM_div_return = prime_factors(LCM_div_prime_factors, (*LCM).divisor)) -- -1)
    return true;	// error
  if(PF_LCM_div_return == PF_0)
    return true;	// error

  // init prime factors of new fraction F
  int PF_F_mul_return;
  if((PF_F_mul_return = prime_factors(F_mul_prime_factors, (*F).multiplier)) == -1)
    return true;	// error
  if(PF_F_mul_return == PF_0)
    return true;	// error

  int PF_F_div_return;
  if((PF_F_div_return = prime_factors(F_div_prime_factors, (*F).divisor)) == -1)
    return true;	// error
  if(PF_LCM_div_return == PF_0)
    return true;	// error

  // OK, no insane cases should land here...
  bool LCM_mul_has_pfactors = (PF_LCM_mul_return != PF_1);
  bool LCM_div_has_pfactors = (PF_LCM_mul_return != PF_1);

  bool F_mul_has_pfactors = (PF_F_mul_return != PF_1);
  bool F_div_has_pfactors = (PF_F_mul_return != PF_1);

  // multiplier prime factors
  int p_cnt=0;
  unsigned int LCM_mul_value=1;	// calculated value, default
  unsigned int F_mul_value=1;
  unsigned int LCM_div_value=1;
  unsigned int F_div_value=1;

  if(LCM_mul_has_pfactors && F_mul_has_pfactors) {	// LCM && F have mul prime factors :)
    for(int i=0; i<=PF_LCM_mul_return || i<=PF_F_mul_return; i++) {
      p_cnt = LCM_mul_prime_factors[i];
      if(p_cnt) {
	if(F_mul_prime_factors[i])
	  if(p_cnt > F_mul_prime_factors[i])
	    p_cnt =  F_mul_prime_factors[i];
      } else
	p_cnt = F_mul_prime_factors[i];
      while(p_cnt--) { LCM_mul_value *= small_primes[i]; }

      p_cnt = LCM_div_prime_factors[i];
      if(p_cnt) {
	if(F_div_prime_factors[i])
	  if(p_cnt > F_div_prime_factors[i])
	    p_cnt =  F_div_prime_factors[i];
      } else
	p_cnt = F_div_prime_factors[i];
      while(p_cnt--) { LCM_div_value *= small_primes[i]; }
    } // loop over prime factors
  } else if(F_mul_has_pfactors) {			// F *only* mul prime factors
    for(int i=0; i<=PF_LCM_mul_return; i++) {
      if(p_cnt = LCM_mul_prime_factors[i])
	while(p_cnt--) { LCM_mul_value *= small_primes[i]; }
    }
  } else if(LCM_mul_has_pfactors) {			// LCM *only* mul prime factors
    for(int i=0; i<=PF_F_mul_return; i++) {
      if(p_cnt = F_mul_prime_factors[i])
	while(p_cnt--) { F_mul_value *= small_primes[i]; }
    }
  } else						// *no* mul prime factors
    ;							// nothing to do

  (*LCM).multiplier = LCM_mul_value;


  // divisor prime factors
  if(LCM_div_has_pfactors && F_div_has_pfactors) {	// LCM && F have div prime factors :)
    for(int i=0; i<=PF_LCM_div_return || i<=PF_F_div_return; i++) {
      p_cnt = LCM_div_prime_factors[i];
      if(p_cnt) {
	if(F_div_prime_factors[i])
	  if(p_cnt > F_div_prime_factors[i])
	    p_cnt =  F_div_prime_factors[i];
      } else
	p_cnt = F_div_prime_factors[i];
      while(p_cnt--) { LCM_div_value *= small_primes[i]; }

      p_cnt = LCM_div_prime_factors[i];
      if(p_cnt) {
	if(F_div_prime_factors[i])
	  if(p_cnt > F_div_prime_factors[i])
	    p_cnt =  F_div_prime_factors[i];
      } else
	p_cnt = F_div_prime_factors[i];
      while(p_cnt--) { LCM_div_value *= small_primes[i]; }
    } // loop over prime factors
  } else if(F_div_has_pfactors) {			// F *only* div prime factors
    for(int i=0; i<=PF_LCM_div_return; i++) {
      if(p_cnt = LCM_div_prime_factors[i])
	while(p_cnt--) { LCM_div_value *= small_primes[i]; }
    }
  } else if(LCM_div_has_pfactors) {			// LCM *only* div prime factors
    for(int i=0; i<=PF_F_div_return; i++) {
      if(p_cnt = F_div_prime_factors[i])
	while(p_cnt--) { F_div_value *= small_primes[i]; }
    }
  } else						// *no* div prime factors
    ;							// nothing to do

  (*LCM).divisor = LCM_div_value;
}


void Harmonical::reduce_fraction(struct fraction *f) {
  unsigned int d = GCD(f->multiplier, f->divisor);

  (*f).multiplier /= d;
  (*f).divisor /= d;
}


void Harmonical::expand_fractions(struct fraction * a, struct fraction * b) {
  unsigned long l = LCM(a->divisor, b->divisor);
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
