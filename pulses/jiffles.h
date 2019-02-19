#ifndef JIFFLES_h
#ifndef RAM_IS_SCARE	// enough RAM?

// jiffletabs *MUST* have 2 trailing zeros

// pre defined jiffles:
unsigned int harmonics4[] = {1,1,1024, 1,2,1024, 1,3,1024, 1,4,1024, 0,0};	// magnets on strings experiments
unsigned int ting1024[] = {1,4096,64, 1,1024,128, 1,1024*3,128, 1,1024*2,128, 1,1024*8,64, 1,1024,64, 0,0}; // magnet strings experiments 2
// unsigned int ting1024[] = {1,4096,64, 1,1024,128, 0,0};			// magnet strings experiments 2
// unsigned int ting4096[] = {1,4096,1024, -1,0,0};			// magnet strings experiments 2
unsigned int ting4096[] = {1,4096,1024, 0,0,0};			// magnet strings experiments 2, KALIMBA7

unsigned int d4096_3072[] = {1,4096,3072, 0,0,0};	// == 3/4

unsigned int d4096_2048[] = {1,4096,2048, 0,0,0};	// == 1/2
unsigned int d4096_1024[] = {1,4096,1024, 0,0,0};	// == 1/4	== ting4096
unsigned int d4096_512[] = {1,4096,512, 0,0,0};
unsigned int d4096_256[] = {1,4096,256, 0,0,0};
unsigned int d4096_128[] = {1,4096,128, 0,0,0};
unsigned int d4096_64[] = {1,4096,64, 0,0,0};
unsigned int d4096_32[] = {1,4096,32, 0,0,0};
unsigned int d4096_16[] = {1,4096,16, 0,0,0};
unsigned int d4096_12[] = {1,4096,12, 0,0,0};
unsigned int d4096_8[] = {1,4096,8, 0,0,0};
unsigned int d4096_6[] = {1,4096,6, 0,0,0};
unsigned int d4096_4[] = {1,4096,4, 0,0,0};
unsigned int d4096_2[] = {1,4096,2, 0,0,0};

unsigned int din__dididi_dixi[] = {1,4096,1024, 1,4,1, 1,4096,32, 1,16,1, 1,4096,32, 1,16,1, 1,4096,64, 1,16,1, 1,8192,64, 1,32,1,
				   3,4*8192,63, 0,0,0};
unsigned int din__dididi_dix[] = {1,4096,1024, 1,4,1, 1,4096,32, 1,16,1, 1,4096,32, 1,16,1, 1,4096,64, 1,16,1, 1,8192,64, 0,0,0};
unsigned int din__dididi[] = {1,4096,1024, 1,4,1, 1,4096,32, 1,16,1, 1,4096,32, 1,16,1, 1,4096,64, 0,0,0};
unsigned int din_dididi[] = {1,4096,512, 1,8,2, 1,4096,64, 1,16,1, 1,4096,32, 1,16,1, 1,4096,15, 0,0,0};
unsigned int ding_ditditdit[] = {1,4096,1024, 1,8,1, 1,4096,16, 1,8,1, 1,4096,8, 1,8,1, 1,4096,7, 0,0,0};
unsigned int diing_ditditdit[] = {1,4096,2048, 1,8,1, 1,4096,16, 1,8,1, 1,4096,8, 1,8,1, 1,4096,6, 0,0,0};
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

// see: pentatonic_minor[] = {1,1, 5,6, 3,4, 2,3, 5*2,6*3, 0,0};
unsigned int pentatonic_rise[] =
  {1,4096,256, 5,6*4096,256, 3,4*4096,256, 2,3*4096,256, 5*2,6*3*4096,256, 1,2*4096,256, 0,0};

unsigned int pentatonic_descend[] =
  {1,2*4096,256, 5*2,6*3*4096,256, 2,3*4096,256, 3,4*4096,256, 5,6*4096,256, 1,4096,256, 0,0};

unsigned int penta_3rd_down_5[] =
  {1,2*4096,256, 2,3*4096,256, 5*2,6*3*4096,256, 3,4*4096,256, 2,3*4096,256, 5,6*4096,256, 3,4*4096,256, 1,4096,256,
   3,2*4096,256, 1,4096,512, 0,0};

unsigned int penta_3rd_rise[] =
  {1,4096,256, 3,4*4096,256, 5,6*4096,256, 2,3*4096,256, 3,4*4096,256, 5*2,6*3*4096,256, 2,3*4096,256,
   1,2*4096,512, 0,0};

unsigned int rising_pent_theme[] =
  {1,4096,256, 3,4*4096,256, 5,6*4096,256, 2,3*4096,256, 3,4*4096,1024, 1,4,1 ,1,1024,32, 1,32,1, 1,1024,32, 1,32,1, 1,1024,64, 0,0};

unsigned int pent_top_wave_0[] =
  {2,3*4096,512, 1,8,1, 5*2,6*3*4096,256, 1,8,1, 1,2*4096,256, 1,8,1, 5*2,6*3*4096,256, 1,8,1, 1,2*4096,256, 1,8,1, 2,3*4096,512, 0,0};

unsigned int pent_top_wave[] =
  {2,3*4096,512, 5*2,6*3*4096,256, 1,2*4096,256, 5*2,6*3*4096,256, 2,3*4096,256, 3,4*4096,256, 4,3*4096,256, 0,0};

unsigned int pent_patternA[] =
  {1,4096,256, 5,6*4096,256, 1,4096,256, 3,4*4096,256, 1,4096,256, 2,3*4096,256, 1,4096,256, 5*2,6*3*4096,256, 1,4096,256,
   1,2*4096,256, 1,4096,256, 1,2*4096,512, 0,0};

unsigned int pent_patternB[] =
  {1,2*4096,256, 5*2,6*3*4096,256, 1,2*4096,256, 2,3*4096,256, 1,2*4096,256, 3,4*4096,256, 1,2*4096,256, 5,6*4096,256, 1,2*4096,256,
   1,4096,256, 1,2*4096,256, 5,6*4096,256, 1,2*4096,256, 3,4*4096,256, 1,2*4096,256, 2,3*4096,256, 1,2*4096,256, 5*2,6*3*4096,256,
   1,2*4096,256, 1,4096,256, 0,0};

// PENTAtonic see:  european_PENTAtonic[] = {1,1, 8,9, 4,5, 2,3, 3,5, 0,0};
unsigned int PENTAtonic_rise[] =	// european pentatonic
  {1,4096,256, 8,9*4096,256, 4,5*4096,256, 2,3*4096,256, 3,5*4096,256, 1,2*4096,512, 0,0};
unsigned int PENTAtonic_descend[] =	// european pentatonic
  {1,2*4096,256, 3,5*4096,256, 2,3*4096,256, 4,5*4096,256, 8,9*4096,256, 1,4096,512, 0,0};
unsigned int PENTA_3rd_down_5[] =
  {1,2*4096,256, 2,3*4096,256, 3,5*4096,256, 4,5*4096,256, 2,3*4096,256, 8,9*4096,256, 4,5*4096,256,
   1,4096,256, 8,9*4096,256, 1,4096,256, 3,2*4096,256, 1,4096,512, 0,0};
unsigned int PENTA_3rd_rise[] =
  {1,4096,256, 4,5*4096,256, 8,9*4096,256, 2,3*4096,256, 4,5*4096,256, 3,5*4096,256, 2,3*4096,256, 1,2*4096,256,
   3,5*4096,256, 4,5*4096,256, 1,2*4096,512, 0,0};

unsigned int up_THRD[]={ 1,4096,256, 4,5*4096,256, 8,9*4096,256, 2,3*4096,256, 4,5*4096,256, 1,2*4096,512, 0,0};

unsigned int up_THRD_dn[]={ 1,4096,256, 8,9*4096,256, 1,4096,256, 4,5*4096,256, 8,9*4096,256,
    2,3*4096,256, 4,5*4096,256, 3,5*4096,256, 2,3*4096,256, 1,2*4096,512, 0,0};

unsigned int top_RETurn[]={ 1,2*4096,512, 8,2*9*4096,256, 3,5*4096,256, 1,2*4096,512, 2,3*4096,256,
    3,5*4096,256, 4,5*4096,256, 2,3*4096,256, 8,9*4096,256, 4,5*4096,256, 1,4096,256, 8,9*4096,256, 1,4096,256, 0,0};

unsigned int dwn_THRD[]={1,2*4096,512, 2,3*4096,256, 3,5*4096,256, 8,9*4096,256, 2,3*4096,256, 8,9*4096,256, 4,5*4096,256,
    1,4096,256, 8,9*4096,256, 1,4096,256, 1,4096,256, 0,0};

unsigned int dwn_THRD_up[]={1,2*4096,512, 4,2*5*4096,256, 3,5*4096,256, 8,2*9*4096,256, 2,3*4096,256, 1,2*4096,512,
			    4,5*4096,256, 3,5*4096,256, 8,9*4096,256, 2,3*4096,256, 1,4096,256, 1,4096,256, 0,0};

unsigned int simple_theme[] =
  {1,128,8, 1,2*128,8, 1,3*128,8, 1,4*128,8, 5,6*4*128,3*8, 1,12,3,
   3,4*2048,256*3/4, 5,6*2048,256*3/4, 2,3*2048,256*3/4, 1,2048,128,
   1,128,3, 1,64,3, 1,32,4, 0,0};

unsigned int tingeling4096[] =
  {1,4096,512, 1,4,2, 1,4096,64, 1,16,4, 1,4096,16, 0,0,0};	// magnets on KALIMBA7
unsigned int ding1024[] =
  {1,1024,192, 0,0,0};			// KALIMBA7, four times faster
unsigned int kalimbaxyl[] =
  {1,1024,32, 1,16,2, 1,32,8, 1,1024,4, 1,32,2, 0, 0, 0};	// KALIMBA7, very silent jiffle

unsigned int ting_tick4096[] = {1,4096,1024, 1,2,1, 1,8192,3, 0,0,0};
unsigned int tigg_ding4096[] = {1,8192,3, 1,2,1, 1,4096,1024, 0,0,0};

unsigned int tumtum[] = {1,2048,2048/16, 7,16,1, 1,2048,2048/16, 0,0,0};
unsigned int tumtumtum[] = {1,2048,2048/16, 3,16,1, 1,2048,2048/16, 3,16,1, 1,2048,2048/16, 0,0,0};
unsigned int tumtumtumtum[] = {1,2048,2048/16, 1,16,1, 1,2048,2048/16, 1,16,1, 1,2048,2048/16, 1,16,1, 1,2048,2048/16, 0,0,0};

unsigned int piip2048[] ={1,2048,128, 0,0,0, };		// length 1/16
unsigned int tanboura[] ={1,2048,1536, 0,0,0, };	// length 3/4

unsigned int loong[]{1,4096,4080, 0,0,0, };		// length 255/256
unsigned int looong[]{1,4096,4088, 0,0,0, };		// length 511/512
unsigned int loooong[]{1,4096,4092, 0,0,0, };		// length 1023/1024

unsigned int jiffletab[] =
  {1,16,2, 1,256,32, 1,128,8, 1,64,2, 1,32,1, 1,16,1, 1,8,2, 0,0};

unsigned int jiff_december[] =
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 0,0};

// try jiff_dec128 with sync=0 to hear the big drum in the cathedral ;)
unsigned int jiff_dec128[] =
  {1,1024,4, 1,64,4, 1,128,16, 1,512,8, 1,1024,128, 0,0};

unsigned int jiff_dec_pizzicato[] =	// dirty!
  {1,1024,4, 1,64,4, 1,28,16, 1,512,8, 1,1024,128, 1,2048,8, 0,0};

// very old jiffles
unsigned int jiffletab0[] =
  {2,1024*3,4, 1,1024,64, 1,2048,64, 1,512,4, 1,64,3, 1,32,1, 1,16,2, 0,0};	// nice short jiffy, FROGS
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


// major_scale[] = {1,1, 8,9, 4,5, 3,4, 2,3, 3,5, 8,15, 0,0};
unsigned int major_rise[] =
  {1,4096,256, 1,9*512,256, 1,5*1024,256, 3,4*4096,256, 1,3*2048,256, 3,5*4096,256, 1,15*512,256, 1,2*4096,256*2, 0,0};
//{1,4096,256, 8,9*4096,256, 4,5*4096,256, 3,4*4096,256, 2,3*4096,256, 3,5*4096,256, 8,15*4096,256, 1,2*4096,256*2, 0,0};

unsigned int major_descend[] =
  {1,4096,256, 1,15*256,256, 3,5*2048,256, 1,3*1024,256, 3,2*4096,256, 1,5*512,256, 1,9*156,256, 1,1048,256, 0,0};

// minor_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 5,8, 5,9, 0,0};
unsigned int minor_rise[] =
  {1,4096,256, 1,9*256,256, 5,6*4096,256, 3,4*4096,256, 1,3*2048,256, 5,8*4096,256, 5,9*4096,256, 1,2*4096,256*2, 0,0};
//{1,4096,256, 8,9*4096,256, 5,6*4096,256, 3,4*4096,256, 2,3*4096,256, 5,8*4096,256, 5,9*4096,256, 1,2*4096,256*2, 0,0};

unsigned int minor_descend[] =
  {1,4096,256, 5,9*2048,256, 5,4*4096,256, 1,3*1024,256, 3,2*4096,256, 5,3*4096,256, 1,9*256,256, 1,2048,256, 0,0};

// doric_scale[] = {1,1, 8,9, 5,6, 3,4, 2,3, 3,5, 5,9, 0,0};
unsigned int doric_rise[] =
  {1,4096,256, 8,9*4096,256, 5,6*4096,256, 3,4*4096,256, 2,3*4096,256, 3,5*4096,256, 5,9*4096,256, 1,2*4096,256*2, 0,0};

unsigned int doric_descend[] =
  {1,4096,256, 5,9*2048,256, 3,5*2048,256, 2,3*2048,256, 3,4*2048,256, 5,6*2048,256, 8,9*2048,256, 1,2*2048,256, 0,0};

unsigned int pentaCHORD_rise[] =
  {1,4096,256, 1,9*512,256, 1,5*1024,256, 3,4*4096,256, 1,3*2048,256*2, 0,0};

unsigned int pentaCHORD_descend[] =
  {1,3*1024,256, 3,2*4096,256, 1,5*512,256, 1,9*156,256, 1,1048,256*2, 0,0};

unsigned int pentachord_rise[] =
  {1,4096,256, 1,9*512,256, 5,6*4096,256, 3,4*4096,256, 1,3*2048,256*2, 0,0};

unsigned int pentachord_descend[] =
  {1,3*1024,256, 3,4*2048,256, 5,6*2048,256, 1,9*256,256, 1,2048,256*2, 0,0};

unsigned int tetraCHORD_rise[] =
  {1,4096,256, 1,9*512,256, 1,5*1024,256, 3,4*4096,256*2, 0,0};

unsigned int tetraCHORD_descend[] =
  {3,2*4096,256, 1,5*512,256, 1,9*156,256, 1,1048,256*2, 0,0};

// try on doric scale
unsigned int doDe_i[] = {1,512,16, 1,1024,16, 1,8,1, 1,2084,11, 0,0};	// TODO: test only, remove?

unsigned int asdfg[] = {1,64,3, 1,128,3, 1,256,3, 1,512,3, 1,4,1, 1,2084,11, 1,3,1, 1,8192,16, 0,0};	// TODO: test only, remove?

#endif	// ! RAM_IS_SCARE
#define JIFFLES_h
#endif