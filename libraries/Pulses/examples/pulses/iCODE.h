/*
  iCODE.h
*/

#ifndef ICODE_H

icode_t d1024_4096_LED[] = { doA2,0,1, 1,4096,1024, doA2,0,0, KILL, };	// time machine bass icode with LED spots

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
