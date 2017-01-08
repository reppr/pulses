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


unsigned long Harmonical::LCM(unsigned int a, unsigned int b) {	// least common multiple
  return a * b / GCD(a, b);
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


unsigned int Harmonical::prime_factors(unsigned int p_factors[], int size, unsigned long number) {
  int highest_prime=0;
  int f=0;		// write index  for p_factors[f]

  if (number<2)
    return 0;

  //  Serial.println(number);
  while (number>1) {
    for(int p=0; p<size && small_primes[p]; p++) {	// loop over small_primes[]
      p_factors[f]=0;
      while ((number % small_primes[p]) == 0) {
	p_factors[f]++;
	number /= small_primes[p];
	//	Serial.print("number: "); Serial.print(number); Serial.print("	prime: "); Serial.print(small_primes[p]);
	//	Serial.print("	cnt: "); Serial.println(*p_factors[f]);
	if (number==1) {
	  //	  Serial.print(small_primes[p]); Serial.print('	'); Serial.println(*p_factors[f]);
	  return (unsigned int) small_primes[p];
	}
      }
      f++;
    }	// small_primes[]
    return 0;
  }
}
