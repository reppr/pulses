/*
  scales.h
*/

#if ! defined SCALES_H


#ifndef SCALES_RAM_SIZE	// scales on small harware ressources, FIXME: test	################
  #define SCALES_RAM_SIZE 9*2+2	// small buffer might fit on simple hardware
#endif

#ifdef SCALES_RAM_SIZE
  // scales:
  unsigned int scale_RAM[SCALES_RAM_SIZE] = {0};
  unsigned int scale_RAM_length = SCALES_RAM_SIZE;
  unsigned int scale_write_index=0;
//  unsigned int *scale=scale_RAM;	// OBSOLETE? see: selected_in(SCALES)
#else
  #error SCALES_RAM_SIZE is not defined
#endif // SCALES_RAM_SIZE


// scaletabs *MUST* have 2 trailing zeros

const unsigned int octaves[] = {1,1, 0,0};				// zero terminated
const unsigned int octaves_fifths[] = {1,1, 2,3, 0,0};			// zero terminated
const unsigned int octaves_fourths[] = {1,1, 3,4, 0,0};		// zero terminated
const unsigned int octave_4th_5th[] = {1,1, 3,4, 2,3, 0,0};	// zero terminated

const unsigned int scale_int[]  = {1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 0,0};  // zero terminated
const unsigned int overnotes[]  = {1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7, 1,8, 1,9, 1,10, 1,11, 1,12, 1,13, 1,14, 1,15, 1,16, 0,0};
const unsigned int scale_rationals[]  = {1,1, 1,2, 2,3, 3,4, 5,6, 6,7, 7,8, 8,9, 9,10, 0,0};  // zero terminated

const unsigned int europ_PENTAtonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};	// scale each octave	zero terminated

const unsigned int pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 0,0};	// scale each octave	zero terminated
// nice first try with "wrong" note:
//  const unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 8*2,9*3, 1,2, 8,9*2, 5,12, 2,6, 8,9*3, 1,4, 0,0 };  // zero terminated
// second try:
const unsigned int mimic_japan_pentatonic[] = {1,1, 8,9, 5,6, 2,3, 2*15,3*16, 0,0 };	// scale each octave	zero terminated


const unsigned int minor_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 5,8, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1

// for doric_scale i try minor scale with major scale's sixth:
const unsigned int doric_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 3,5, 5,9, 0,0};	// scale each octave	zero terminated
// 1/1	9/8	6/5	4/3	3/2	8/5	9/5	2/1


const unsigned int major_scale[] = {1,1, 8,9, 4,5, 3,4, 2,3, 3,5, 8,15, 0,0};	// scale each octave	zero terminated
// 24	27	30	32	36	40	45	48
// 1:1	9:8	5:4	4:3	3:2	5:3	15:8	2:1

const unsigned int tetraCHORD[] = {1,1, 8,9, 4,5, 3,4, 0,0};		// scale major tetraCHORD
const unsigned int tetrachord[] = {1,1, 8,9, 5,6, 3,4, 0,0};		// scale minor tetrachord
const unsigned int pentaCHORD[] = {1,1, 8,9, 4,5, 3,4, 2,3, 0,0};	// major
const unsigned int pentachord[] = {1,1, 8,9, 5,6, 3,4, 2,3, 0,0};	// minor

const unsigned int trichord[] = {1,1, 8,9, 4,5, 0,0};			// major

const unsigned int TRIAD[] ={1,1, 4,5, 2,3, 0,0};
const unsigned int triad[] ={1,1, 5,6, 2,3, 0,0};


#define SCALES_H
#endif
