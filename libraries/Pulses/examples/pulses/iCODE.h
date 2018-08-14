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

#define ICODE_H
#endif
