/*
  melody_jiffles.h
  just a first test with pp
*/

/*
unsigned int PENTAtonic_rising[] =	// european pentatonic
  {1,4096,256, 8,9*4096,256, 4,5*4096,256, 2,3*4096,256, 3,5*4096,256, 1,2*4096,512, 0,0};
*/



// #define RAT
#define DIV	4096
#define CNT	256

#define T0	'1,DIV,CNT,'
#define T1	'8,9*DIV,CNT,'
#define T2	'4,5*DIV,CNT,'
#define T3	'2,3*DIV,CNT,'
#define T4	'3,5*DIV,CNT,'

#define T5	'1,2*DIV,512,'
#define T6	'8,2*9*DIV,CNT,'
#define T7	'4,2*5*DIV,CNT,'
#define T8	'2,2*3*DIV,CNT,'
#define T9	'3,2*5*DIV,CNT,'

MENU.outln("DADA");
MENU.outln(STRINGIFY(T4));
