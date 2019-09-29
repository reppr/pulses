/*
  mpu6050_module.h

  see:  https://github.com/jrowberg/i2cdevlib
*/

#if ! defined MPU6050_MODULE_H

//#define DEBUG_AG_REACTION	// TODO: remove or fix the debug mess
//#define DEBUG_ACCELGYRO_BASICS	// TODO: remove or fix the debug mess

//#define DEBUG_ACCGYRO_SAMPLE	true	// TODO: remove or fix the debug mess
#define DEBUG_ACCGYRO_SAMPLE	false	// TODO: remove or fix the debug mess

bool debug_accgyro_sample=DEBUG_ACCGYRO_SAMPLE;	// TODO: remove or fix the debug mess

#define MPU6050_TEST_VERSION	2			// TODO: remove runtime test versions
uint8_t mpu6050_test_version=MPU6050_TEST_VERSION;	// DEFAULT, could be switched run time

bool mpu6050_available=true;	// will be reset automagically if there's no MPU6050 found
				// this will switch it off, including sampling...

// int16_t HARDWARE.accGyro_offsets[] = {-1493, -2125, 1253, 98, 85, -50};	// Ax, Ay, Az, Gx, Gy, Gz offsets
// *GIVE INDIVIDUAL OFFSET VALUES FOR YOUR CHIP HERE*
// get these values (with *horizontally resting* chips)
// see: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050/examples/IMU_Zero

#include <I2Cdev.h>
#include <MPU6050.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

MPU6050 mpu6050;

int16_t ax, ay, az;
int16_t gx, gy, gz;

bool accGyro_is_active = false ;	// accGyro switching by the program

bool accGyro_invert = true;	// OBSOLETE:	// defaults to invert mathemetical axis, which feels right in instrument handling
//bool accGyro_invert = false;		// default TEST ################################################################

// all 6 axis can be inverted individually	// TODO: configuration, nvs, ... ################
bool accX_invert = true;	// "rear" up (as seen from my current portable instruments)
bool accY_invert = false;	// right up
bool accZ_invert = true;	// TODO: ???

bool gyrX_invert = false;	// TODO: ???
bool gyrY_invert = false;	// TODO: ???
bool gyrZ_invert = true;	// TODO: TEST:


bool mpu6050_setup() {
  MENU.out(F("mpu6050_setup()\t"));

  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000L);	// must be *after* Wire.begin()
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  mpu6050.initialize();

  mpu6050_available=true;
  if(mpu6050.testConnection()) {
    MENU.outln(F("MPU6050 connected"));

    MENU.out(F("set accGyro offsets\t{"));
    int16_t o;

    mpu6050.setXAccelOffset(o = HARDWARE.accGyro_offsets[0]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYAccelOffset(o = HARDWARE.accGyro_offsets[1]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZAccelOffset(o = HARDWARE.accGyro_offsets[2]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setXGyroOffset(o = HARDWARE.accGyro_offsets[3]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYGyroOffset(o = HARDWARE.accGyro_offsets[4]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZGyroOffset(o = HARDWARE.accGyro_offsets[5]);
    MENU.out(o);
    MENU.outln('}');

    MENU.ln();
    return true;	// OK
  } // else

  MENU.out(F("failed: "));
  MENU.out(mpu6050.getDeviceID());
  MENU.ln(2);
  mpu6050_available=false;	// no connection to MPU6050

  return false;			// ERROR
} // mpu6050_setup()


bool volatile accGyro_new_data=false;		// new data has arrived, trigger accGyro_reaction()

// float  accGyro_current_??			// last accGyro data seen as accGyro_new_data got true		hä? ????
float accGyro_current_AX_f=0.0;
float accGyro_current_AY_f=0.0;
float accGyro_current_AZ_f=0.0;
float accGyro_current_GX_f=0.0;
float accGyro_current_GY_f=0.0;
float accGyro_current_GZ_f=0.0;


typedef struct accGyro_6d_data_t{
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;
} accGyro_6d_data_t;


#if ! defined MPU_OVERSAMPLING
  #define MPU_OVERSAMPLING	3	// TEST
//#define MPU_OVERSAMPLING	4	// TEST
//#define MPU_OVERSAMPLING	2	// TEST
  //#define MPU_OVERSAMPLING	1	// possible
#endif
int mpu_oversampling=MPU_OVERSAMPLING;		// default	TODO: UI
accGyro_6d_data_t mpu_samples[MPU_OVERSAMPLING] = {0};


#if ! defined GYRO_FLOAT_SCALING
  #define GYRO_FLOAT_SCALING	48.0	// very SENSITIVE, but *AVOID gyroZ FALSE POSITIVES*
#endif
float gyro_float_scaling=GYRO_FLOAT_SCALING;	// default, TEST&TRIMM:	(no UI planed)


/*  pulses does NOT use interrupt version any more	// TODO: REMOVE:
//	the interrupt version worked fine so far
//	but as i2c activity is quite critical while playing
//	i want the program to decide *when* to do it...

hw_timer_t * accGyro_timer = NULL;

//#define WITH_TIMER_MUX
#if defined WITH_TIMER_MUX
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif
*/

enum accGyro_modes {
  AG_mode_Ax=1,
  AG_mode_Ay=2,
  AG_mode_Az=4,
  AG_mode_A_reserved=8,

  AG_mode_Gx=16,
  AG_mode_Gy=32,
  AG_mode_Gz=64,
  AG_mode_G_reserved=128,
};

//int accGyro_mode=0;	// zero means inactive
int accGyro_mode = AG_mode_Gz | AG_mode_Ay | AG_mode_Ax;	// very temporary default ;)

int16_t Ax_sel_offset=0;
int16_t Ay_sel_offset=0;
int16_t Az_sel_offset=0;
int16_t Gx_sel_offset=0;
int16_t Gy_sel_offset=0;
int16_t Gz_sel_offset=0;

#define ACCELERO_SELECTION_SLOTS_DEFAULT		24	// TODO: test&trimm
int16_t Ax_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from
int16_t Ay_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from
int16_t Az_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from

void * Ax_reaction_source=NULL;	// DB pointers can be used here
void * Ay_reaction_source=NULL;	// DB pointers can be used here
void * Az_reaction_source=NULL;	// DB pointers can be used here

void * Gx_reaction_source=NULL;	// DB pointers can be used here
void * Gy_reaction_source=NULL;	// DB pointers can be used here
void * Gz_reaction_source=NULL;	// DB pointers can be used here


extern void extended_output(char* data, uint8_t row, uint8_t col, bool force);
#if ! defined MONOCHROME_MOTION_STATE_ROW
  #define MONOCHROME_MOTION_STATE_ROW	7
#endif
#if ! defined MONOCHROME_MOTION_MUTING_ROW
  #define MONOCHROME_MOTION_MUTING_ROW	0
#endif

void display_accGyro_mode() {
  MENU.out(F("accGyro mode: "));

  u8x8.setCursor(MONOCHROME_MOTION_STATE_ROW, 0);
  char buffer[] = {"acc ...  gyr ..."};
  //              {"0123456789abcdef"}

  if(accGyro_mode & AG_mode_Ax)
    buffer[4] = 'X';
  if(accGyro_mode & AG_mode_Ay)
    buffer[5] = 'Y';
  if(accGyro_mode & AG_mode_Az)
    buffer[6] = 'Z';

  if(accGyro_mode & AG_mode_Gx)
    buffer[13] = 'X';
  if(accGyro_mode & AG_mode_Gy)
    buffer[14] = 'Y';
  if(accGyro_mode & AG_mode_Gz)
    buffer[15] = 'Z';

  extended_output(buffer, MONOCHROME_MOTION_STATE_ROW, 0, false);
}


void reset_accGyro_selection() {	// reset accGyro selections, slots, reaction sources
  Ax_sel_offset=0;
  Ay_sel_offset=0;
  Az_sel_offset=0;

  Gx_sel_offset=0;
  Gy_sel_offset=0;
  Gz_sel_offset=0;

  Ax_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// or zero, meaning switch to raw linear float?
  Ay_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  Az_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;

  /*
  Gx_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  Gy_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  Gz_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  */

  Ax_reaction_source=NULL;
  Ay_reaction_source=NULL;
  Az_reaction_source=NULL;

  Gx_reaction_source=NULL;
  Gy_reaction_source=NULL;
  Gz_reaction_source=NULL;
}


extern void sync_shifting(fraction_t shift);
//int16_t GYROX_selected=0;
//int16_t GYROY_selected=0;
int16_t GYROZ_selected=0;
bool GYRO_only_check() {	// OBSOLETE:
#if defined DEBUG_AG_REACTION
  MENU.outln("GYRO_only_check()");
#endif
  float GZ = mpu6050.getRotationZ();
  GZ /= 128;		// TODO: TEST&TRIMM: new float scaling
  if(accGyro_invert)	// invert mathematical axis	// TODO: where to invert?
    GZ = -GZ;

  GYROZ_selected = GZ + 0.5;
  GYROZ_selected += Gz_sel_offset;
  if(GYROZ_selected) {	// gyro Z shows rotation?
    // TODO: feedback
    // sync_shifting({GYROZ_selected, 16*4096});	// was: DEFAULT in first version
    sync_shifting({GYROZ_selected, 16*4096});	// TODO: FIND&TRIMM new DEFAULT for version2
#if defined DEBUG_AG_REACTION
    MENU.out(GYROZ_selected);
    MENU.outln("\t<<<<<<<<<<<<<<<<");
#endif
    return true;
  }

  return false;
}

#if ! defined ACCGYRO_DEFAULT_PRESET
  #define ACCGYRO_DEFAULT_PRESET	1
  //#define ACCGYRO_DEFAULT_PRESET	2
#endif
uint8_t	accGyro_preset = ACCGYRO_DEFAULT_PRESET;


//	/*	void IRAM_ATTR accGyro_sample()	// pulses does NOT use interrupt version any more	*/
//	// mpu6050_test_version==1
//	void accGyro_sample() {
//	  if(!mpu6050_available) {						// catch bugs, if any ;)  TODO: REMOVE:
//	    MENU.error_ln(F("accGyro_sample() mpu6050_available=false"));	// catch bugs, if any ;)  TODO: REMOVE:
//	    return;
//	  }
//
//	  static int16_t mpu_sample_index=0;
//
//	//#define ACCELGYRO_SAMPLES	ALL6
//	// or individual parameters:
//	#define ACCELGYRO_SAMPLES_AX
//	#define ACCELGYRO_SAMPLES_AY
//	//#define ACCELGYRO_SAMPLES_AZ
//	//#define ACCELGYRO_SAMPLES_GX
//	//#define ACCELGYRO_SAMPLES_GY
//	#define ACCELGYRO_SAMPLES_GZ
//
//	#if ACCELGYRO_SAMPLES == ALL6
//	  // take one 6d sample
//	  mpu6050.getMotion6(&mpu_samples[mpu_sample_index].ax, &mpu_samples[mpu_sample_index].ay, &mpu_samples[mpu_sample_index].az, \
//			     &mpu_samples[mpu_sample_index].gx, &mpu_samples[mpu_sample_index].gy, &mpu_samples[mpu_sample_index].gz);
//	#else
//	  // individual switches:
//	  #if defined ACCELGYRO_SAMPLES_AX
//	  mpu_samples[mpu_sample_index].ax = mpu6050.getAccelerationX();
//	  #endif
//
//	  #if defined ACCELGYRO_SAMPLES_AY
//	  mpu_samples[mpu_sample_index].ay = mpu6050.getAccelerationY();
//	  #endif
//
//	  #if defined ACCELGYRO_SAMPLES_AZ
//	  mpu_samples[mpu_sample_index].az = mpu6050.getAccelerationZ();
//	  #endif
//
//	  #if defined ACCELGYRO_SAMPLES_GX
//	  mpu_samples[mpu_sample_index].gx = mpu6050.getRotationX();
//	  #endif
//
//	  #if defined ACCELGYRO_SAMPLES_GY
//	  mpu_samples[mpu_sample_index].gy = mpu6050.getRotationY();
//	  #endif
//
//	  #if defined ACCELGYRO_SAMPLES_GZ
//	  mpu_samples[mpu_sample_index].gz = mpu6050.getRotationZ();
//	  #endif
//	#endif
//
//	  if(((++mpu_sample_index) % MPU_OVERSAMPLING) == 0) {
//	    mpu_sample_index = 0;
//	    int32_t AX=0, AY=0, AZ=0, GX=0, GY=0, GZ=0;
//
//	    for(int i=0; i < MPU_OVERSAMPLING; i++) {
//	      AX += mpu_samples[i].ax;
//	      AY += mpu_samples[i].ay;
//	      AZ += mpu_samples[i].az;
//	      GX += mpu_samples[i].gx;
//	      GY += mpu_samples[i].gy;
//	      GZ += mpu_samples[i].gz;
//	    }
//
//	    // ACCELERO:	INVERTING DELAYED to accGyro_reaction()
//	    accGyro_current_AX_f = (AX / MPU_OVERSAMPLING) + 16384;
//	    accGyro_current_AX_f /= 32768;	// to float scaling
//	    accGyro_current_AX_f += 1.0;	// (inverted axis)  0..1
//
//	    accGyro_current_AY_f = (AY / MPU_OVERSAMPLING) + 16384;
//	    accGyro_current_AY_f /= 32768;	// to float scaling
//	    accGyro_current_AY_f += 1.0;	// (inverted axis)  0..1
//
//	    accGyro_current_AZ_f = (AZ / MPU_OVERSAMPLING) + 16384;
//	    accGyro_current_AZ_f /= 32768;	// to float scaling
//	    accGyro_current_AZ_f += 1.0;	// (inverted axis)  ????
//
//	    // GYRO:	INVERTING IS DONE HERE in accGyro_sample
//	    accGyro_current_GX_f = (GX / MPU_OVERSAMPLING);
//	    accGyro_current_GX_f /= 32;	// TODO: is *RANDOM* float scaling
//	    if(accGyro_invert)	// invert mathematical axis
//	      accGyro_current_GX_f = -accGyro_current_GX_f;
//
//	    accGyro_current_GY_f = (GY / MPU_OVERSAMPLING);
//	    accGyro_current_GY_f /= 32;	// TODO: is *RANDOM* float scaling
//	    if(accGyro_invert)	// invert mathematical axis
//	      accGyro_current_GY_f = -accGyro_current_GY_f;
//
//	    accGyro_current_GZ_f = (GZ / MPU_OVERSAMPLING);
//	    accGyro_current_GZ_f /= 32;	// TODO: is *RANDOM* float scaling
//	    if(accGyro_invert)	// invert mathematical axis
//	      accGyro_current_GZ_f = -accGyro_current_GZ_f;
//
//	    /*	portENTER_CRITICAL_ISR(&timerMux);	*/ // pulses does not use interrupt version any more
//	    accGyro_new_data = true;
//	    /*	portEXIT_CRITICAL_ISR(&timerMux);	*/ // pulses does not use interrupt version any more
//	  }
//
//	  #if defined DEBUG_ACCGYRO_SAMPLE
//	  if(accGyro_new_data && debug_accgyro_sample) {	// TODO: factor that out?
//	    MENU.out(F("AX "));
//	    MENU.out(accGyro_current_AX_f, 4);
//	    MENU.tab();
//
//	    MENU.out(F("AY "));
//	    MENU.out(accGyro_current_AY_f, 4);
//	    MENU.tab();
//
//	    MENU.out(F("AZ "));
//	    MENU.out(accGyro_current_AZ_f, 4);
//	    MENU.tab();
//
//	    MENU.out(F("GX "));
//	    MENU.out(accGyro_current_GX_f, 4);
//	    MENU.tab();
//
//	    MENU.out(F("GY "));
//	    MENU.out(accGyro_current_GY_f, 4);
//	    MENU.tab();
//
//	    MENU.out(F("GZ "));
//	    MENU.out(accGyro_current_GZ_f, 4);
//	    MENU.ln();
//	  }
//	#endif
//	} // accGyro_sample()
//
//	// void accGyro_reaction()
	extern bool monochrome_can_be_used();			// extern declarations
	extern void monochrome_show_line(uint8_t row, char * s);
	extern short lowest_primary, highest_primary;
	extern int primary_count;
	extern void noAction_flags_line();
//	void accGyro_reaction() {	// react on data coming from accGyro_sample()
//	#if defined DEBUG_AG_REACTION
//	  MENU.outln("accGyro_reaction()");
//	#endif
//	  if(accGyro_new_data && accGyro_mode) {
//	    accGyro_new_data = false;
//	    if(! accGyro_is_active)
//	      return;
//
//	    // selected slice:
//	    static int selected_Ax_seen_i;	// HUUUCH ????????????????
//	    static int selected_Ay_seen;	// HUUUCH ????????????????
//	    static int selected_Az_seen;
//
//	    static int selected_Gx_seen;
//	    static int selected_Gy_seen;
//	    static int selected_Gz_seen;
//
//	    int selected_Ax=0;
//	    int selected_Ay=0;
//	    int selected_Az=0;
//
//	    int selected_Gx=0;
//	    int selected_Gy=0;
//	    int selected_Gz=0;
//
//	    float AX=0.0, AY=0.0, AZ=0.0, GX=0.0, GY=0.0, GZ=0.0;
//
//	    bool there_was_output=false;
//	    reset_accGyro_selection();	// reset accGyro selections, slots, reaction sources
//
//	    if(accGyro_mode & AG_mode_Ax) {	// accelero X
//	      AX = accGyro_current_AX_f;
//	    }
//
//
//
//
//	    switch(accGyro_preset) {
//	    case 1:
//	//MENU.outln(">>>>>>>>>>>>>>>> DADA\tp1");
//	      if(accGyro_mode & AG_mode_Ax) {	// accelero X
//		AX = accGyro_current_AX_f;
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("AX+ ");
//		MENU.out(AX);
//		MENU.space(3);
//		MENU.out(AX/32768);
//	#endif
//		Ax_reaction_source = JIFFLES;
//		Ax_sel_offset = 0;			// JIFFLE RAM
//		Ax_select_slots = 41;		// tum8up
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		AY = accGyro_current_AY_f;
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("\t\tAY+ ");
//		MENU.out(AY);
//		MENU.space(3);
//		MENU.out(AY/32768);
//	#endif
//
//		Ay_reaction_source = NULL;
//		Ay_select_slots = 8;
//		Ay_sel_offset = 0;
//	      }
//
//	      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
//		GZ = accGyro_current_GZ_f;
//
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("\tGZ= ");
//		MENU.out(GZ);
//	#endif
//		if(accGyro_invert)	// invert mathematical axis
//		  GZ = -GZ;
//
//	#if defined DEBUG_ACCELGYRO_BASICS
//		MENU.space(3);
//		MENU.out(GZ);	// scaled and inverted
//	#endif
//	      }
//
//	      if(there_was_output)
//		MENU.ln();
//
//	      // ACCELERO:	INVERTING IS DONE HERE in  accGyro_reaction()
//	      if(accGyro_mode & AG_mode_Ax) {		// accelero X
//		selected_Ax = AX * Ax_select_slots +0.5;
//		selected_Ax += Ax_sel_offset;
//		if(accGyro_invert)	// invert mathematical axis
//		  selected_Ax = Ax_select_slots - 1 - selected_Ax;
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		selected_Ay = AY * Ay_select_slots +0.5;
//		selected_Ay += Ay_sel_offset;
//		if(accGyro_invert)	// invert mathematical axis
//		  selected_Ay = Ay_select_slots - 1 - selected_Ay;
//	      }
//
//	      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
//		selected_Gz = GZ + 0.5;
//		selected_Gz += Gz_sel_offset;
//	      }
//
//	#if defined DEBUG_AG_REACTION
//	      if (selected_Ax != selected_Ax_seen_i || selected_Ay != selected_Ay_seen || selected_Gz != selected_Gz_seen) {
//		MENU.out(F("SELECTED  Ax "));
//		MENU.out(selected_Ax);
//
//		MENU.out(F("\t\tAy "));
//		MENU.out(selected_Ay);
//
//		MENU.out(F("\t\tGz "));
//		MENU.outln(selected_Gz);
//
//		//selected_Ay_seen = selected_Ay;	// right now unused
//		selected_Gz_seen = selected_Gz;	// TODO: DEACTIVATED, see GYRO
//	      }
//	#endif
//
//	      if(accGyro_mode & AG_mode_Ax) {		// accelero X
//		unsigned int* jiffle = NULL;
//		if(jiffle = index2pointer(JIFFLES, selected_Ax)) {
//		  if(selected_Ax != selected_Ax_seen_i) {
//		    selected_Ax_seen_i = selected_Ax;
//		    select_in(JIFFLES, jiffle);
//		    setup_jiffle_thrower_selected(selected_actions);
//		    MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));
//
//	#if defined USE_MONOCHROME_DISPLAY
//		    if(monochrome_can_be_used()) {
//		      monochrome_show_line(3, array2name(JIFFLES, selected_in(JIFFLES)));
//		    }
//	#endif
//		  }
//		} // Ax JIFFLES
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		if(selected_Ay != selected_Ay_seen) {
//		  selected_Ay_seen = selected_Ay;
//
//		  switch(selected_Ay) {
//		    //      case 0:		// TODO: selected_Ay < 1
//		    //	// limit--
//		    //	break;
//		  case 1:	// all but high
//		    extended_output(F("LBM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    for(int pulse = highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET upper quarter
//		    break;
//		  case 2:	// all on
//		  case 3:	// all on
//		    extended_output(F("LBMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    break;
//		  case 4:	// middle only
//		    extended_output(F("_BM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    MENU.outln(highest_primary);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=lowest_primary + (primary_count/4) +1; pulse <= highest_primary - (primary_count/4); pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    break;
//		  case 5:	// extremes only
//		    extended_output(F("L__H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/4); pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR low quarter
//		    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
//		    break;
//		  case 6:	// high only
//		    extended_output(F("___H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
//		    break;
//
//		    //      case 7:	// bass_limit--		// TODO: near limit region
//		    //	break;
//		  default:	// toggle all
//		    extended_output(F("~~~~ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    PULSES.select_from_to(lowest_primary, highest_primary);
//		    PULSES.selected_toggle_no_actions();
//		    PULSES.select_n(voices);
//		  } // switch(selected_Ay)
//		  noAction_flags_line();
//		} //selected_Ay_seen
//	      } // accelero Y
//	      break;
//
//	    case 2:	// new implementation
//	//MENU.outln(">>>>>>>>>>>>>>>> DADA\tp2");
//	      if(accGyro_mode & AG_mode_Ax) {		// accelero X
//		AX = accGyro_current_AX_f;
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("AX+ ");
//		MENU.out(AX);
//		MENU.space(3);
//		MENU.out(AX/32768);
//	#endif
//		Ax_reaction_source = JIFFLES;
//		//	Ax_sel_offset = -40 -11 -10;		// tum8up ? -11 diing_ditditdit ?  -10 looong ?
//		Ax_sel_offset = -29;				// diing_ditditdit ?
//		Ax_select_slots = 41;		//
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		AY = accGyro_current_AY_f;
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("\t\tAY+ ");
//		MENU.out(AY);
//		MENU.space(3);
//		MENU.out(AY/32768);
//	#endif
//
//		Ay_reaction_source = NULL;
//		Ay_select_slots = 8;
//		Ay_sel_offset = 0;
//	      }
//
//	      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
//		GZ = accGyro_current_GZ_f;
//
//	#if defined DEBUG_ACCELGYRO_BASICS
//		there_was_output = true;
//		MENU.out("\tGZ= ");
//		MENU.out(GZ);
//	#endif
//		if(accGyro_invert)	// invert mathematical axis
//		  GZ = -GZ;
//
//	#if defined DEBUG_ACCELGYRO_BASICS
//		MENU.space(3);
//		MENU.out(GZ);	// scaled and inverted
//	#endif
//	      }
//
//	      if(there_was_output)
//		MENU.ln();
//
//	      // ACCELERO:	INVERTING IS DONE HERE in  accGyro_reaction()
//	      if(accGyro_mode & AG_mode_Ax) {		// accelero X
//		selected_Ax = AX * Ax_select_slots +0.5;
//		selected_Ax += Ax_sel_offset;
//		if(accGyro_invert)	// invert mathematical axis
//		  selected_Ax = Ax_select_slots - 1 - selected_Ax;
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		selected_Ay = AY * Ay_select_slots +0.5;
//		selected_Ay += Ay_sel_offset;
//		if(accGyro_invert)	// invert mathematical axis
//		  selected_Ay = Ay_select_slots - 1 - selected_Ay;
//	      }
//
//	      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
//		selected_Gz = GZ + 0.5;
//		selected_Gz += Gz_sel_offset;
//	      }
//
//	#if defined DEBUG_AG_REACTION
//	      if (selected_Ax != selected_Ax_seen_i || selected_Ay != selected_Ay_seen || selected_Gz != selected_Gz_seen) {
//		MENU.out(F("SELECTED  Ax "));
//		MENU.out(selected_Ax);
//
//		MENU.out(F("\t\tAy "));
//		MENU.out(selected_Ay);
//
//		MENU.out(F("\t\tGz "));
//		MENU.outln(selected_Gz);
//
//		//selected_Ay_seen = selected_Ay;	// right now unused
//		selected_Gz_seen = selected_Gz;	// TODO: DEACTIVATED, see GYRO
//	      }
//	#endif
//
//	      if(accGyro_mode & AG_mode_Ax) {		// accelero X
//		unsigned int* jiffle = NULL;
//		if(jiffle = index2pointer(JIFFLES, selected_Ax)) {
//		  if(selected_Ax != selected_Ax_seen_i) {
//		    selected_Ax_seen_i = selected_Ax;
//		    select_in(JIFFLES, jiffle);
//		    setup_jiffle_thrower_selected(selected_actions);
//		    MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));
//
//	#if defined USE_MONOCHROME_DISPLAY
//		    if(monochrome_can_be_used()) {
//		      monochrome_show_line(3, array2name(JIFFLES, selected_in(JIFFLES)));
//		    }
//	#endif
//		  }
//		} // Ax JIFFLES
//	      }
//
//	      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
//		if(selected_Ay != selected_Ay_seen) {
//		  selected_Ay_seen = selected_Ay;
//
//		  switch(selected_Ay) {
//		    //      case 0:		// TODO: selected_Ay < 1
//		    //	// limit--
//		    //	break;
//		  case 1:	// all but high
//		    extended_output(F("LBM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    for(int pulse = highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET upper quarter
//		    break;
//		  case 2:	// all on
//		  case 3:	// all on
//		    extended_output(F("LBMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    break;
//		  case 4:	// middle only
//		    extended_output(F("_BM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    MENU.outln(highest_primary);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=lowest_primary + (primary_count/4) +1; pulse <= highest_primary - (primary_count/4); pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
//		    break;
//		  case 5:	// extremes only
//		    extended_output(F("L__H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/4); pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR low quarter
//		    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
//		    break;
//		  case 6:	// high only
//		    extended_output(F("___H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
//		    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
//		      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
//		    break;
//
//		    //      case 7:	// bass_limit--		// TODO: near limit region
//		    //	break;
//		  default:	// toggle all
//		    extended_output(F("~~~~ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
//		    PULSES.select_from_to(lowest_primary, highest_primary);
//		    PULSES.selected_toggle_no_actions();
//		    PULSES.select_n(voices);
//		  } // switch(selected_Ay)
//		  noAction_flags_line();
//		} //selected_Ay_seen
//	      } // accelero Y
//	      break;
//
//	    default:
//	      MENU.error_ln(F("unknown accGyro_preset"));
//	    } // switch (accGyro_preset)
//	  } // if(accGyro_mode)
//	} // accGyro_reaction()


void accGyro_data_display() {
  if(! mpu6050.testConnection())	// if no connection, try to initialise
    mpu6050_setup();
  if(mpu6050.testConnection()) {	// (try again)
    MENU.out(mpu6050.getAccelerationX());
    MENU.tab();
    MENU.out(mpu6050.getAccelerationY());
    MENU.tab();
    MENU.out(mpu6050.getAccelerationZ());
    MENU.tab();

    MENU.out(mpu6050.getRotationX());
    MENU.tab();
    MENU.out(mpu6050.getRotationY());
    MENU.tab();
    MENU.outln(mpu6050.getRotationZ());
  } else
    MENU.outln(F("mpu6050 not connected"));
} // accGyro_data_display()


//#define COMPILE_ACCEL_GYRO_SPEED_TEST
#if defined COMPILE_ACCEL_GYRO_SPEED_TEST
void accGyro_speed_test(int n=1000) {
  unsigned int start_time, loop_time, elapsed;
  int16_t dummy;

  MENU.out(F("accGyro_speed_test("));
  MENU.out(n);
  MENU.outln(')');


  MENU.out(F("loop\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=i;
  }
  elapsed = loop_time = (micros() - start_time);
  MENU.outln((float) elapsed / n);


  MENU.out(F("Ax\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=mpu6050.getAccelerationX();
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.out(F("Gx\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=mpu6050.getRotationX();
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.out(F("XY\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=mpu6050.getAccelerationX();
    dummy=mpu6050.getAccelerationY();
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.out(F("XYZ\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=mpu6050.getAccelerationX();
    dummy=mpu6050.getAccelerationY();
    dummy=mpu6050.getRotationZ();
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.out(F("6D\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    mpu6050.getMotion6(&dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.out(F("XYZxyz\t"));
  start_time = micros();
  for(int i=0; i<n; i++) {
    dummy=mpu6050.getAccelerationX();
    dummy=mpu6050.getAccelerationY();
    dummy=mpu6050.getAccelerationZ();
    dummy=mpu6050.getRotationX();
    dummy=mpu6050.getRotationY();
    dummy=mpu6050.getRotationZ();
  }
  elapsed = micros() - start_time;
  elapsed -= loop_time;
  MENU.outln((float) elapsed / n);


  MENU.ln();
}
#endif // COMPILE_ACCEL_GYRO_SPEED_TEST

/* pulses does not use interrupt version any more
void activate_accGyro() {
  //  accGyro_timer = timerBegin(0, 80000, true);	// milliseconds
  accGyro_timer = timerBegin(0, 80, true);	// microseconds
  timerAttachInterrupt(accGyro_timer, &accGyro_sample, true);
  timerAlarmWrite(accGyro_timer, 1000000, true);
  timerAlarmEnable(accGyro_timer);
}
*/


/* **************************************************************** */
// new implementation _v2:

#define DEBUG_SHOW_EACH_MPU_SAMLE	// TODO: DEACTIVATE:
void accGyro_sample_v2() {
  if(!mpu6050_available) {						// catch bugs, if any ;)  TODO: REMOVE:
    MENU.error_ln(F("accGyro_sample_v2() mpu6050_available=false"));	// catch bugs, if any ;)  TODO: REMOVE:
    return;
  }

  static int16_t mpu_sample_index=0;

  // take one 6d sample
  mpu6050.getMotion6(&mpu_samples[mpu_sample_index].ax, &mpu_samples[mpu_sample_index].ay, &mpu_samples[mpu_sample_index].az, \
		     &mpu_samples[mpu_sample_index].gx, &mpu_samples[mpu_sample_index].gy, &mpu_samples[mpu_sample_index].gz);

// accGyro_6d_data_t* data_p = &mpu_samples[mpu_sample_index];
// mpu6050.getMotion6(data_p->ax, data_p->ay, data_p->az, data_p->gx, data_p->gy, data_p->gz);
  ++mpu_sample_index;
  if(mpu_sample_index % mpu_oversampling)
    return;

  // else (mpu_sample_index % mpu_oversampling) == 0	NEW OVERSAMPLED DATA READY
  mpu_sample_index = 0;
  accGyro_new_data = true;		// trigger accGyro_reaction()

  int32_t ax_i=0, ay_i=0, az_i=0, gx_i=0, gy_i=0, gz_i=0;	// int32 to sum up int16

  for(int i=0; i < mpu_oversampling; i++) {
    ax_i += mpu_samples[i].ax;
    ay_i += mpu_samples[i].ay;
    az_i += mpu_samples[i].az;
    gx_i += mpu_samples[i].gx;
    gy_i += mpu_samples[i].gy;
    gz_i += mpu_samples[i].gz;

#if defined DEBUG_SHOW_EACH_MPU_SAMLE
    MENU.out(mpu_samples[i].ax); MENU.tab();
    MENU.out(mpu_samples[i].ay); MENU.tab();
    MENU.out(mpu_samples[i].az); MENU.tab();
    MENU.out(mpu_samples[i].gx); MENU.tab();
    MENU.out(mpu_samples[i].gy); MENU.tab();
    MENU.outln(mpu_samples[i].gz);
#endif
  }

  // ACCELERO:
  accGyro_current_AX_f = (ax_i / mpu_oversampling) + 16384.0;
  accGyro_current_AX_f /= 32768.0;	// to float scaling
  if(accX_invert) {
    accGyro_current_AX_f *= -1.0;
    accGyro_current_AX_f += 1.0;	// 0..1
  } // else	not tested

  accGyro_current_AY_f = (ay_i / mpu_oversampling) + 16384.0;
  accGyro_current_AY_f /= 32768.0;	// to float scaling
  if(accY_invert) {
    accGyro_current_AY_f *= -1.0;
    accGyro_current_AY_f += 1.0;	// 0..1
  } // else	nothing to do

  accGyro_current_AZ_f = (az_i / mpu_oversampling) + 16384.0;
  accGyro_current_AZ_f /= 32768.0;	// to float scaling
  if(accZ_invert) {		// INVERT mathematical axis?
    accGyro_current_AZ_f *= -1.0;
    accGyro_current_AZ_f += 1.0;	// ????
  }

  // GYRO:
  accGyro_current_GX_f = (gx_i / mpu_oversampling);
  accGyro_current_GX_f /= gyro_float_scaling;
  if(gyrX_invert)		// INVERT mathematical axis?
    accGyro_current_GX_f *= -1;

  accGyro_current_GY_f = (gy_i / mpu_oversampling);
  accGyro_current_GY_f /= gyro_float_scaling;
  if(gyrY_invert)		// INVERT mathematical axis?
    accGyro_current_GY_f *= -1;

  accGyro_current_GZ_f = (gz_i / mpu_oversampling);
  accGyro_current_GZ_f /= gyro_float_scaling;
  if(gyrZ_invert)
    accGyro_current_GZ_f *= -1;

  //#if defined DEBUG_ACCGYRO_SAMPLE
  // TODO: DEACTIVATE DEBUGGING OUTPUT...	################
  if(true || accGyro_new_data && debug_accgyro_sample) {	// TODO: factor that out?
    MENU.out(F("AX "));
    MENU.out(accGyro_current_AX_f, 4);
    MENU.tab();

    MENU.out(F("AY "));
    MENU.out(accGyro_current_AY_f, 4);
    MENU.tab();

    MENU.out(F("AZ "));
    MENU.out(accGyro_current_AZ_f, 4);
    MENU.tab();

    MENU.out(F("GX "));
    MENU.out(accGyro_current_GX_f, 4);
    MENU.tab();

    MENU.out(F("GY "));
    MENU.out(accGyro_current_GY_f, 4);
    MENU.tab();

    MENU.out(F("GZ "));
    MENU.out(accGyro_current_GZ_f, 4);
    MENU.ln();
  }
  //#endif
} // accGyro_sample_v2()


void accGyro_reaction_v2() {	// react on data coming from accGyro_sample()
  if(accGyro_new_data && accGyro_mode) {
    accGyro_new_data = false;

    if(! accGyro_is_active) {	// maybe catch errors here, if any?	TODO: REMOVE:
      MENU.error_ln(F("accGyro_reaction_v2():\taccGyro_is_active is false"));
      return;
    }

    // last selected slice, integer
    static int _selected_Ax_i_seen;
    static int _selected_Ay_i_seen;
    static int _selected_Az_i_seen;

    static int _selected_Gx_i_seen;
    static int _selected_Gy_i_seen;
    static int _selected_Gz_i_seen;

    int Ax_i_new=0;
    int Ay_i_new=0;
    int Az_i_new=0;

    int selected_Gx_i=0;
    int selected_Gy_i=0;
    int selected_Gz_i=0;

    float AX_seen_f=0.0;
    float AY_seen_f=0.0;
    float AZ_seen_f=0.0;
    float GX_seen_f=0.0;
    float GY_seen_f=0.0;
    float GZ_seen_f=0.0;

    reset_accGyro_selection();	// offsets and slots

    // ACCELERO;
    if(accGyro_mode & AG_mode_Ax) {		// accelero X
      AX_seen_f = accGyro_current_AX_f;
    }

    if(accGyro_mode & AG_mode_Ay) {		// accelero Y
      AY_seen_f = accGyro_current_AY_f;
    }

    if(accGyro_mode & AG_mode_Az) {		// accelero Z
      AZ_seen_f = accGyro_current_AZ_f;
    }

    // GYRO:
    if(accGyro_mode & AG_mode_Gx) {		// gyro X
      GX_seen_f = accGyro_current_GX_f;
    }

    if(accGyro_mode & AG_mode_Gy) {		// gyro Y
      GY_seen_f = accGyro_current_GY_f;
    }

    if(accGyro_mode & AG_mode_Gz) {		// gyro Z
      GZ_seen_f = accGyro_current_GZ_f;
    }

    switch(accGyro_preset) {
    case 1:
      // ACCELERO;
      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	Ax_reaction_source = JIFFLES;
	Ax_sel_offset = 0;		// TODO: TRIMM:
	Ax_select_slots = 41;		// TODO: TRIMM:
      }

      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
	Ay_reaction_source = NULL;
	Ay_select_slots = 8;
	Ay_sel_offset = 0;
      }

      if(accGyro_mode & AG_mode_Az) {		// accelero Z
	Az_reaction_source = NULL;
	Az_select_slots = 8;
	Az_sel_offset = 0;
      }

      // ACCELERO:
      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	Ax_i_new = AX_seen_f * Ax_select_slots +0.5;
	Ax_i_new += Ax_sel_offset;
      }

      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
	Ay_i_new = AY_seen_f * Ay_select_slots +0.5;
	Ay_i_new += Ay_sel_offset;
      }

      if(accGyro_mode & AG_mode_Az) {		// accelero Z
	Az_i_new = AZ_seen_f * Az_select_slots +0.5;
	Az_i_new += Az_sel_offset;
      }


      // GYRO:
      if(accGyro_mode & AG_mode_Gx) {		// gyro X
	selected_Gx_i = GX_seen_f + 0.5;
	selected_Gx_i += Gx_sel_offset;
      }

      if(accGyro_mode & AG_mode_Gy) {		// gyro Y
	selected_Gy_i = GY_seen_f + 0.5;
	selected_Gy_i += Gy_sel_offset;
      }

      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
	selected_Gz_i = GZ_seen_f + 0.5;
	selected_Gz_i += Gz_sel_offset;
      }

      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	unsigned int* jiffle = NULL;
	if(jiffle = index2pointer(JIFFLES, Ax_i_new)) {
	  if(Ax_i_new != _selected_Ax_i_seen) {
	    _selected_Ax_i_seen = Ax_i_new;
	    select_in(JIFFLES, jiffle);
	    setup_jiffle_thrower_selected(selected_actions);
	    MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));

#if defined USE_MONOCHROME_DISPLAY
	    if(monochrome_can_be_used()) {
	      monochrome_show_line(3, array2name(JIFFLES, selected_in(JIFFLES)));
	    }
#endif
	}
	// else MENU.outln("\tseen already");

	} else {
	  MENU.error_ln(F("no jiffle?"));
	} // Ax JIFFLES
      }


      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
	if(Ay_i_new != _selected_Ay_i_seen) {
	  _selected_Ay_i_seen = Ay_i_new;

	  for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)	// default ALL UNMUTED
	    PULSES.pulses[pulse].action_flags &= ~noACTION;	// CLEAR all

	  switch(Ay_i_new) {
	    /*
	      1  L__H
	      2  LB__
	      3  LBM_
	      4  LBMH
	      5  _BMH
	      6  __MH
	      7  _BM_
	      8  L__H
	     */
	  case 1:	// extremes only
	  case 8:	// extremes only
	    for(int pulse=lowest_primary + (primary_count/4) +1; pulse <= highest_primary - (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute middle two quarters
	    break;

	  case 7:	// middles only
	    extended_output(F("_BM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 6:	// mute lower half
	    extended_output(F("__MH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/2); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute lower half
	    break;

	  case 5:	// mute low
	    extended_output(F("_BMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 4:	// all unmuted
	    extended_output(F("LBMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    break;

	  case 3:	// mute high
	    extended_output(F("LBM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    break;

	  case 2:	// mute upper half
	    extended_output(F("LB__ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = highest_primary - (primary_count/2) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute upper half
	    break;

	  default:
	    MENU.out(F("accGyro_reaction_v2()\tAy_i_new out of range "));
	    MENU.outln(Ay_i_new);
	  }

	  noAction_flags_line();
	} //_selected_Ay_i_seen
      } // accelero Y
      break;

    default:
      MENU.error_ln(F("unknown accGyro_preset"));
    } // switch (accGyro_preset)
  } // if(accGyro_mode)
} // accGyro_reaction_v2()


void show_accGyro_offsets() {
  MENU.out(F("accGyro offsets  Ax, Ay, Az,  Gx, Gy, Gz {"));
  for(int i=0; i<6; i++) {
    MENU.out(HARDWARE.accGyro_offsets[i]);
    MENU.out(F(", "));
  }
  MENU.outln('}');
}


void set_accGyro_offsets_UI() {
// int16_t HARDWARE.accGyro_offsets[] = {-1493, -2125, 1253, 98, 85, -50};	// Ax, Ay, Az, Gx, Gy, Gz offsets
  MENU.outln(F("set accGyro offsets"));
  show_accGyro_offsets();

  for(int i=0; i<6; i++) {
    //    if(MENU.is_numeric())
    HARDWARE.accGyro_offsets[i] = MENU.numeric_input(HARDWARE.accGyro_offsets[i]);

    if(MENU.peek() == ',')
      MENU.drop_input_token();
  }

  show_accGyro_offsets();
}


#define  MPU6050_MODULE_H
#endif
