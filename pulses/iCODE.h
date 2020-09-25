/*
  iCODE.h
*/

#ifndef ICODE_H

icode_t d4096_1024_i2cLED[] = { i2cW,1, 1,4096,1024, i2cW,0, KILL, };	// time machine bass icode with LED spots on i2c
icode_t d4096_1024_icode_jiff[] = { 1,4096,1024, KILL, };	// temporally duplicating jiffle, *no* i2c	TODO: soon obsolete
//icode_t d4096_1024_LED[] = { doA2,0,1, 1,4096,1024, doA2,0,0, KILL, };	// time machine bass icode with LED spots

// TODO: remove debug code
// icode_t TEST_ICODE[] = { INFO, 1, 1, 4 , INFO, 4, 3, 2, 0, 0, 0, INFO, KILL, };
// icode_t TEST_ICODE[] = { INFO, 1, 1, 4 , INFO, 4, 3, 2, 0, 0, 0, INFO, DONE, };
//icode_t TEST_ICODE[] = { INFO, 1,1,4, 3,1,0, 4,3,2, 0,0,0, INFO, KILL, };
////                          ^JIFF, ^WAIT, ^JIFF, ^ENDJ,

// icode_t TEST_ICODE[] = { 1,4096,4, KILL, };	// testing time machine icode embedded jiffle
// icode_t TEST_ICODE[] = { 1,4096,4, 0,0,0, };	// testing jiffle as icode

/*
icode_t TEST_ICODE[] = { 1,4096,1024, KILL, };	// time machine icode embedded jiffle

icode_t TEST_ICODE_WAIT[] = { WAIT,1,4, WAIT,1,4, WAIT,1,32, WAIT,1,32, KILL };	// testing time machine jiffle

icode_t TEST_ICODE_MCP23017[] = { doA2,0,1, 1,4096,1024, doA2,0,0, KILL, };	// time machine icode embedded jiffle
*/

//icode_t melody0[] = { MELODY_MODE, 0, 1, 2, 3, 4, STACCATO, 3, 2, 1, 0, PORTATO, 0, 0, KILL, };	// MELODY_MODE testing

icode_t melody0[] = { MELODY_MODE, 0, 1, 2, KILL, };				// MELODY_MODE testing
icode_t melody1[] = { LEGATO, NOTE_16, 0, -1, -2, -3, 0, 1, 2, 3, KILL, };	// MELODY_MODE testing
icode_t melody2[] = { LEGATO, NOTE_16, 0, 0, 0, 1,  NOTE_32, 0, 0, 0, 1,  NOTE_64, 0, 0, 0, 1,  NOTE_128, 0, 0, 0, 1,
		      KILL, };	// MELODY_MODE testing
icode_t melody3[] = { LEGATO, NOTE_16, 0, 0, 1, PORTATO, 0, 0, 1, SEPARATO, 0, 0, 1, STACCATO, 0, 0, 1,
		      STACCISS, 0, 0, 1, KILL, };	// MELODY_MODE testing
icode_t melody4[] = { NOTE_8, 0, 1, 2, MUTED, 0, LEGATO, 3, 1, 0, KILL, };	// MELODY_MODE MUTED testing

#define ICODE_H
#endif
