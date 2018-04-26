#ifndef JIFFLES_h
#ifndef RAM_IS_SCARE	// enough RAM?

// jiffletabs *MUST* have 2 trailing zeros

// pre defined jiffles:
unsigned int harmonics4[] = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};	// magnets on strings experiments
unsigned int ting1024[] = {1,4096,64, 1,1024,128, 1,1024*3,128, 1,1024*2,128, 1,1024*8,64, 1,1024,64, 0,0}; // magnet strings experiments 2
// unsigned int ting1024[] = {1,4096,64, 1,1024,128, 0,0};			// magnet strings experiments 2
// unsigned int ting4096[] = {1,4096,1024, -1,0,0};			// magnet strings experiments 2
unsigned int ting4096[] = {1,4096,1024, 0,0,0};			// magnet strings experiments 2, KALIMBA7

unsigned int d1024_4096[] = {1,4096,512, 0,0,0};	// == ting4096
unsigned int d512_4096[] = {1,4096,512, 0,0,0};
unsigned int d256_4096[] = {1,4096,256, 0,0,0};
unsigned int d128_4096[] = {1,4096,128, 0,0,0};
unsigned int d64_4096[] = {1,4096,64, 0,0,0};
unsigned int d32_4096[] = {1,4096,32, 0,0,0};

unsigned int d16_4096[] = {1,4096,16, 0,0,0};
unsigned int d8_4096[] = {1,4096,8, 0,0,0};

unsigned int d4_4096[] = {1,4096,4, 0,0,0};
unsigned int d2_4096[] = {1,4096,2, 0,0,0};

// peepeep4096[] for tuning
unsigned int peepeep4096[] = {1,4096,2048, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 1,4096,256, 1,8*4096,1, 0,0};
// unsigned int arpeggio4096[] = {4,4096*4,128, 4,4096*3,128, 4,4096*4,128, 4,4096*5,128, 4,4096*6,64, 0,0}; // pezzo strings E16

unsigned int arpeggio4096[] = {
  64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256, 64,4096*5,1, 1,4096*5,256,
  64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 0
  };
unsigned int arpeggio4096down[] = {
  64,4096*8,1, 1,4096*8,256, 64,4096*6,1, 1,4096*6,256, 64,4096*5,1, 1,4096*5,256, 64,4096*4,1, 1,4096*4,256,
  64,4096*3,1, 1,4096*3,256, 64,4096*2,1, 1,4096*2,256, 64,4096*1,1, 1,4096*1,256, 1,512,16, 1,128,8, 1,64,4, 1,32,2,
  0,0};

unsigned int arpeggio_cont[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12*6,6, 1,4096*6,24, 1,48,2, 1,4096*8,256, 1,6,1, 1,256,2, 1,12,3, 0,0};

unsigned int arpeggio_and_down0[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,4, 1,2048,128, 1,4096*4,128, 0,0};

unsigned int arpeggio_and_down1[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 0,0};

unsigned int arpeggio_and_down[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 1,1024,64, 0,0};

unsigned int arpeggio_and_down3[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,4096*5,512, 1,12,2, 1,1024,64, 1,256,8, 1,2048,128, 1,4096*8,128, 1,4096*6,128, 1,4096*5,128, 1,4096*4,128, 1,4096*3,128, 1,4096*2,128, 1,4096,128, 1,1024,128, 3,1024*2,128, 1,512,16, 1,256,16, 1,128,8, 0,0};

unsigned int arpeggio_and_sayling[] = {64,4096,1, 1,4096,256,  64,4096*2,1, 1,4096*2,256, 64,4096*3,1, 1,4096*3,256,  64,4096*4,1, 1,4096*4,256, 64,4096*5,1, 1,4096*5,256, 64,4096*6,1, 1,4096*6,256, 64,4096*8,1, 1,4096*8,256, 1,5*4096,512, 1,12,1 , 1,6*4096,1024, 6,5*6*4096,1024, 1,5*4096,1024, 1,4*4096,1024, 1,3*4096,1024, 1,2*4096,1024, 1,1024,128, 1,128,4, 1,64,4, 1,32,2, 0,0};

// unsigned int halfway[] = {1,2,1, 1,4,1, 1,8,1, 1,16,1, 1,32,1, 1,64,1, 1,128,1, 1,256,1, 1,512,1, 1,1024,1, 1,2048,1, 1,4096,1, 1,8192,1, 1,16384,1, 0,0};

// unsigned int back_to_ground[] = {3,128*2,8, 1,128,4, 1,64,3, 0,0};
unsigned int stepping_down[] = {1,4096*8,512+256, 1,4096*7,512,  1,4096*6,512,  1,4096*5,512,  1,4096*4,512,  1,4096*3,512,  1,4096*2,512,  1,4096,512, 0,0};

unsigned int back_to_ground[] = {1,2048,32, 3,1024*2,32, 1,1024,32, 1,512,32, 2,512*3,32, 1,256,16, 1,128,8, 1,64,4, 0,0};

unsigned int pentatonic_rising[] =
  {1,4096,256, 5,6*4096,256, 3,4*4096,256, 2,3*4096,256, 5*2,6*3*4096,256, 1,2*4096,256, 0,0};

unsigned int simple_theme[] =
  {1,128,8, 1,2*128,8, 1,3*128,8, 1,4*128,8, 5,6*4*128,3*8, 1,12,3,
   3,4*2048,256*3/4, 5,6*2048,256*3/4, 2,3*2048,256*3/4, 1,2048,128,
   1,128,3, 1,64,3, 1,32,4,
   0,0};

unsigned int tingeling4096[] =
  {1,4096,512, 1,4,2, 1,4096,64, 1,16,4, 1,4096,16, 0,0,0};	// magnets on KALIMBA7
unsigned int ding1024[] =
  {1,1024,192, 0,0,0};			// KALIMBA7, four times faster
unsigned int kalimbaxyl[] =
  {1,1024,32, 1,16,2, 1,32,8, 1,1024,4, 1,32,2, 0, 0, 0};	// KALIMBA7, very silent jiffle

unsigned int ting_tick4096[] = {1,4096,1024, 1,2,1, 1,8192,3, 0,0,0};
unsigned int tigg_ding4096[] = {1,8192,3, 1,2,1, 1,4096,1024, 0,0,0};

unsigned int tumtum[] = {1,2048,2048/16, 7,16,1, 1,2048,2048/16, 0,0,0};

unsigned int piip2048[] ={1,2048,128, 0,0,0, };		// length 1/16
unsigned int tanboura[] ={1,2048,1536, 0,0,0, };	// length 3/4

unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0};

unsigned int jiff_december[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 0,0};

unsigned int jiff_dec128[] =
  {1,1024,4, 1,64,4, 1,128,16, 1,512,8, 1,1024,128, 0,0};

unsigned int jiff_dec_pizzicato[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 1,2048,8, 0,0};

// very old jiffles
unsigned int jiffletab01[] = {1,512,8, 1,1024,16, 1,2048,32, 1,1024,16, 0,0};
unsigned int jiffletab02[] = {1,128,2, 1,256,6, 1,512,10, 1,1024,32, 1,3*128,20, 1,64,8, 0,0};
unsigned int jiffletab03[] = {1,32,4, 1,64,8, 1,128,16, 1,256,32, 1,512,64, 1,1024,128, 0,0};	// testing octaves
unsigned int jiffletab04[] =
  {1,2096,4, 1,512,2, 1,128,2, 1,256,2, 1,512,8, 1,1024,32, 1,512,4, 1,256,3, 1,128,2, 1,64,1, 0,0};
unsigned int jiffletab05[] = {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,2, 1,64,1, 1,32,1, 1,16,2, 0,0};
unsigned int jiffletab06[] = {1,32,2, 0,0};	// doubleclick
unsigned int jiffletab1[]  = {1,1024,64, 1,512,4, 1,128,2, 1,64,1, 1,32,1, 1,16,1, 0,0};

unsigned int gling128_0[] = {1,128,16, 0,0};
unsigned int gling128_1[] = {1,256,2, 1,128,16, 0,0};
unsigned int gling128_2[] = {1,512,4, 1,256,4, 1,128,16, 0,0};
unsigned int gling128[]   = {1,512,8, 1,256,4, 1,128,16, 0,0};

#endif	// ! RAM_IS_SCARE
#define JIFFLES_h
#endif
