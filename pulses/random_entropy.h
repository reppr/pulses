/*
  random_entropy.h
*/

#ifndef RANDOM_ENTROPY_H

unsigned int random_entropy() {	// start this from setup before initializing GPIOs
  unsigned int dummy;	// not initalized!	start with some entropy ;)
#if defined ESP32
  // TODO: touch
  dummy += analogRead(0);	// on my system this gives a *lot* of entropy
  for (int i=0; i<dummy; i++)
    random(66);

  dummy += analogRead(12);	// gives much entropy on my system
  for (int i=0; i<dummy; i++)
    random(77);

  for(uint8_t pin=34; pin<40; pin++)	// more entropy
    dummy += analogRead(pin);

  dummy += touchRead(12);	// little entropy, (most of the time)
  dummy &= 0xff;

  for (int i=0; i<dummy; i++)
    random(88);

#else	// TODO: arduino
  ;
#endif
  return dummy=random(1000001);
}

#define RANDOM_ENTROPY_H
#endif
