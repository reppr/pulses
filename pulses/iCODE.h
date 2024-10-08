/*
  iCODE.h
*/

#ifndef ICODE_H

const icode_t d4096_1024_i2cLED[] = { i2cW,1, 1,4096,1024, i2cW,0, KILL, };	// time machine bass icode with LED spots on i2c
const icode_t d4096_1024_icode_jiff[] = { 1,4096,1024, KILL, };	// temporally duplicating jiffle, *no* i2c	TODO: soon obsolete
//const icode_t d4096_1024_LED[] = { doA2,0,1, 1,4096,1024, doA2,0,0, KILL, };	// time machine bass icode with LED spots

const icode_t melody1[] = { NOTE_16, SOUNDSEP,1,6, 0,0,1,2,3,4, NOTE_8,LEGATO,5, PAUSE,5, NOTE_32,SEPARATO,6,7,8,7,6,5,4,3,2,1, NOTE_16,0, KILL, };  // *nice*	testing SOUNDSEP articulation

// use on major or minor scales
const icode_t melody2[] = { NOTE_32, SOUNDSEP,1,6, /*I*/0,2,4,8,2,4,0,2, /*IV*/0,3,5,8,3,5,0,3, /*V*/-1,1,4,9,7,4,-1,1, /*I*/0,2,4,8,2,4,0,2,
		      KILL};

const icode_t melody3[] = { NOTE_16, SOUNDSEP,1,6, /*I*/0,2,4,0,2,4, /*IV*/0,3,5,0,3,5, /*V*/1,4,6,-1,1,4, /*I*/0,2,4,0,2,4, KILL};


const icode_t melody7[] = { MELODY_MODE,STACCISS,0, PAUSE,0, SEPARATO,NOTE_64,2,0,-1,-2,-3,-4, NOTE_16,STACCISS,-5,
		      NOTE_32,STACCATO,3,1,0,-1,-2,-3, STACCISS,4,2,1,0,-1,-2, STACCATO,-3,-2,-1,0,1, NOTE_16,SEPARATO,0, STACCATO,0,0,
		      NOTE_8,PAUSE,0, NOTE_16,STACCISS,1,2,3,4,5,6,7,8, KILL, };

const icode_t melody_diatonic_rise[] = { SEPARATO,NOTE_32, 0,-1,0,1,2,3,4,5,6,7,8, NOTE_8, 7, KILL };
const icode_t melody_diatonic_fall[] = { STACCATO,NOTE_32, 7,8,7,6,5,4,3,2,1, NOTE_8,0, KILL };

const icode_t melody_upthirds_rise[] = { SEPARATO,NOTE_32, 0,2,1,3, 2,4,3,5,4,6,5,7,6,8, NOTE_4, 7, KILL };
const icode_t melody_downthirds_rise[] = { SEPARATO,NOTE_32, 0,-1,1,-1,2,0,3,1,4,2,5,3,6,4,7,5,8,6, NOTE_4, 7, KILL };

const icode_t melody_dwnthirds_fall[] = { SEPARATO,NOTE_32, 7,6,7,5,6,4,5,3,4,2,3,1,2,0,1,-1, 0,0,0,NOTE_8,0, KILL };

const icode_t midi_melody_16[] = {NOTE_16, 0, KILL};
const icode_t midi_melody_8[] = {NOTE_8, 0, KILL};
const icode_t midi_melody_4[] = {NOTE_4, 0, KILL};

const icode_t midi_melody2[] = {NOTE_8, PAUSE, 0, NOTE_4, 0, NOTE_8, 4, PAUSE, 8, KILL};

#define ICODE_H
#endif
