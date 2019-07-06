/*
  mpu6050_module.h

  see:  https://github.com/jrowberg/i2cdevlib

*/

#if ! defined MPU6050_MODULE_H

int16_t accGyro_offsets[] = {-1493, -2125, 1253, 98, 85, -50};	// aX, aY, aZ, gX, gY, gZ offsets
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
bool accGyro_invert = true;		// defaults to invert mathemetical axis, which feels right in instrument handling

bool mpu6050_setup() {
  MENU.out(F("mpu6050_setup()\t"));

  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  mpu6050.initialize();

  if(mpu6050.testConnection()) {
    MENU.outln(F("ok"));

    MENU.out(F("set accGyro offsets\t{"));
    int16_t o;

    mpu6050.setXAccelOffset(o = accGyro_offsets[0]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYAccelOffset(o = accGyro_offsets[1]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZAccelOffset(o = accGyro_offsets[2]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setXGyroOffset(o = accGyro_offsets[3]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYGyroOffset(o = accGyro_offsets[4]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZGyroOffset(o = accGyro_offsets[5]);
    MENU.out(o);
    MENU.outln('}');

    return true;
  }

  MENU.out(F("failed: "));
  MENU.outln(mpu6050.getDeviceID());
  return false;
}

bool volatile accGyro_new_data=false;		// new data has arrived, waits for accGyro_reaction()
// float  accGyro_current_??			// last accGyro data seen as accGyro_new_data got true
float accGyro_current_AX=0.0;
float accGyro_current_AY=0.0;
float accGyro_current_AZ=0.0;
float accGyro_current_GX=0.0;
float accGyro_current_GY=0.0;
float accGyro_current_GZ=0.0;


typedef struct {
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;
} accGyro_6d_data;

#if ! defined MPU_OVERSAMPLING
  //#define MPU_OVERSAMPLING	15
  //#define MPU_OVERSAMPLING	7	// ok, but slow
  #define MPU_OVERSAMPLING	4	// TEST
  //#define MPU_OVERSAMPLING	2	// quite usable, but sounds trigger accelero...
  //#define MPU_OVERSAMPLING	1	// possible
#endif
accGyro_6d_data mpu_samples[MPU_OVERSAMPLING] = { 0 };	// TODO: yes?

/*  pulses does NOT use interrupt version any more
//	the interrupt version worked fine so far
//	but as i2c activity is quite critical while playing
//	i want the program to decide *when* to do it...

hw_timer_t * accGyro_timer = NULL;

//#define WITH_TIMER_MUX
#if defined WITH_TIMER_MUX
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif
*/

enum AG_mode {
  axM=1,
  ayM=2,
  azM=4,

  gxM=8,
  gyM=16,
  gzM=32,
  D6M=64,	// unused yet
};

//int accGyro_mode=0;	// zero means inactive
int accGyro_mode = gzM | ayM | axM;	// very temporary default ;)
int16_t aX_sel_i0=0;
int16_t aY_sel_i0=0;
int16_t aZ_sel_i0=0;
int16_t gX_sel_i0=0;
int16_t gY_sel_i0=0;
int16_t gZ_sel_i0=0;

#define ACCELERO_SELECTION_SLOTS_DEFAULT		24	// TODO: test&trimm
int16_t aX_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from
int16_t aY_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from
int16_t aZ_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;	// n items to select from

void * aX_reaction_source=NULL;	// DB pointers can be used here
void * aY_reaction_source=NULL;	// DB pointers can be used here
void * aZ_reaction_source=NULL;	// DB pointers can be used here

void * gX_reaction_source=NULL;	// DB pointers can be used here
void * gY_reaction_source=NULL;	// DB pointers can be used here
void * gZ_reaction_source=NULL;	// DB pointers can be used here


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

  if(accGyro_mode & axM)
    buffer[4] = 'X';
  if(accGyro_mode & ayM)
    buffer[5] = 'Y';
  if(accGyro_mode & azM)
    buffer[6] = 'Z';

  if(accGyro_mode & gxM)
    buffer[13] = 'X';
  if(accGyro_mode & gyM)
    buffer[14] = 'Y';
  if(accGyro_mode & gzM)
    buffer[15] = 'Z';

  extended_output(buffer, MONOCHROME_MOTION_STATE_ROW, 0, false);
}

void reset_accGyro_selection() {
  aX_sel_i0=0;
  aY_sel_i0=0;
  aZ_sel_i0=0;
  gX_sel_i0=0;
  gY_sel_i0=0;
  gZ_sel_i0=0;

  aX_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  aY_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;
  aZ_select_slots=ACCELERO_SELECTION_SLOTS_DEFAULT;

  aX_reaction_source=NULL;
  aY_reaction_source=NULL;
  aZ_reaction_source=NULL;

  gX_reaction_source=NULL;
  gY_reaction_source=NULL;
  gZ_reaction_source=NULL;
}


extern void sync_shifting(fraction shift);			// pre declaration
//int16_t GYROX_selected=0;
//int16_t GYROY_selected=0;
int16_t GYROZ_selected=0;
bool GYRO_only_check() {
  float GZ = mpu6050.getRotationZ();
  GZ /= 32;	// TODO: is *RANDOM* float scaling
  if(accGyro_invert)	// invert mathematical axis
    GZ = -GZ;

  GYROZ_selected = GZ + 0.5;
  GYROZ_selected += gZ_sel_i0;
  if(GYROZ_selected) {	// gyro Z shows rotation?
    // TODO: feedback
//  sync_shifting({GYROZ_selected, 8*4096});
    sync_shifting({GYROZ_selected, 16*4096});	// TODO: UI for shift amount
    return true;
  }

  return false;
}

#if ! defined ACCGYRO_DEFAULT_PRESET
  #define ACCGYRO_DEFAULT_PRESET	1
#endif
uint8_t	accGyro_preset = ACCGYRO_DEFAULT_PRESET;


/*	void IRAM_ATTR accGyro_sample()	// pulses does NOT use interrupt version any more	*/
void accGyro_sample() {
  static int16_t mpu_sample_index=0;

//#define ACCELGYRO_SAMPLES	ALL6
// or individual parameters:
#define ACCELGYRO_SAMPLES_AX
#define ACCELGYRO_SAMPLES_AY
//#define ACCELGYRO_SAMPLES_AZ
//#define ACCELGYRO_SAMPLES_GX
//#define ACCELGYRO_SAMPLES_GY
#define ACCELGYRO_SAMPLES_GZ

#if ACCELGYRO_SAMPLES == ALL6
  // take one 6d sample
  mpu6050.getMotion6(&mpu_samples[mpu_sample_index].ax, &mpu_samples[mpu_sample_index].ay, &mpu_samples[mpu_sample_index].az, \
		     &mpu_samples[mpu_sample_index].gx, &mpu_samples[mpu_sample_index].gy, &mpu_samples[mpu_sample_index].gz);
#else
  // individual switches:
  #if defined ACCELGYRO_SAMPLES_AX
  mpu_samples[mpu_sample_index].ax = mpu6050.getAccelerationX();
  #endif

  #if defined ACCELGYRO_SAMPLES_AY
  mpu_samples[mpu_sample_index].ay = mpu6050.getAccelerationY();
  #endif

  #if defined ACCELGYRO_SAMPLES_AZ
  mpu_samples[mpu_sample_index].az = mpu6050.getAccelerationZ();
  #endif

  #if defined ACCELGYRO_SAMPLES_GX
  mpu_samples[mpu_sample_index].gx = mpu6050.getRotationX();
  #endif

  #if defined ACCELGYRO_SAMPLES_GY
  mpu_samples[mpu_sample_index].gy = mpu6050.getRotationY();
  #endif

  #if defined ACCELGYRO_SAMPLES_GZ
  mpu_samples[mpu_sample_index].gz = mpu6050.getRotationZ();
  #endif
#endif

  if(((++mpu_sample_index) % MPU_OVERSAMPLING) == 0) {
    mpu_sample_index = 0;
    int32_t AX=0, AY=0, AZ=0, GX=0, GY=0, GZ=0;

    for(int i=0; i < MPU_OVERSAMPLING; i++) {
      AX += mpu_samples[i].ax;
      AY += mpu_samples[i].ay;
      AZ += mpu_samples[i].az;
      GX += mpu_samples[i].gx;
      GY += mpu_samples[i].gy;
      GZ += mpu_samples[i].gz;
    }

    // ACCELERO:	INVERTING DELAYED to accGyro_reaction()
    accGyro_current_AX = (AX / MPU_OVERSAMPLING) + 16384;
    accGyro_current_AX /= 32768;	// to float scaling

    accGyro_current_AY = (AY / MPU_OVERSAMPLING) + 16384;
    accGyro_current_AY /= 32768;	// to float scaling

    accGyro_current_AZ = (AZ / MPU_OVERSAMPLING) + 16384;
    accGyro_current_AZ /= 32768;	// to float scaling

    // GYRO:	INVERTING IS DONE HERE in accGyro_sample
    accGyro_current_GX = (GX / MPU_OVERSAMPLING);
    accGyro_current_GX /= 32;	// TODO: is *RANDOM* float scaling
    if(accGyro_invert)	// invert mathematical axis
      accGyro_current_GX = -accGyro_current_GX;

    accGyro_current_GY = (GY / MPU_OVERSAMPLING);
    accGyro_current_GY /= 32;	// TODO: is *RANDOM* float scaling
    if(accGyro_invert)	// invert mathematical axis
      accGyro_current_GY = -accGyro_current_GY;

    accGyro_current_GZ = (GZ / MPU_OVERSAMPLING);
    accGyro_current_GZ /= 32;	// TODO: is *RANDOM* float scaling
    if(accGyro_invert)	// invert mathematical axis
      accGyro_current_GZ = -accGyro_current_GZ;

    /*	portENTER_CRITICAL_ISR(&timerMux);	*/ // pulses does not use interrupt version any more
    accGyro_new_data = true;
    /*	portEXIT_CRITICAL_ISR(&timerMux);	*/ // pulses does not use interrupt version any more
  }
}

// void accGyro_reaction()
//#define DEBUG_AG_REACTION		// DO show selected slices
//#define DEBUG_ACCELGYRO_BASICS	// deactivated
extern bool monochrome_is_on();					// extern declaration
extern void monochrome_show_line(uint8_t row, char * s);	// extern declaration
extern int lowest_primary, highest_primary, primary_count;	// extern declaration
extern void noAction_flags_line();				// extern declaration
void accGyro_reaction() {	// react on data coming from accGyro_sample()
  if(accGyro_new_data && accGyro_mode) {
    accGyro_new_data = false;
    if(! accGyro_is_active)
      return;

    // selected slice:
    static int selected_aX_seen;
    static int selected_aY_seen;
    static int selected_gZ_seen;
    int selected_aX=0;
    int selected_aY=0;
    int selected_gZ=0;

    float AX=0.0, AY=0.0, AZ=0.0, GX=0.0, GY=0.0, GZ=0.0;

    bool there_was_output=false;
    reset_accGyro_selection();	// offsets and slots

    switch(accGyro_preset) {
    case 1:
      if(accGyro_mode & axM) {		// accelero X
	AX = accGyro_current_AX;
#if defined DEBUG_ACCELGYRO_BASICS
	there_was_output = true;
	MENU.out("AX+ ");
	MENU.out(AX);
	MENU.space(3);
	MENU.out(AX/32768);
#endif
	aX_reaction_source = JIFFLES;
	aX_sel_i0 = 0;			// JIFFLE RAM
	aX_select_slots = 41;		// tum8up
      }

      if(accGyro_mode & ayM) {		// accelero Y
	AY = accGyro_current_AY;
#if defined DEBUG_ACCELGYRO_BASICS
	there_was_output = true;
	MENU.out("\t\tAY+ ");
	MENU.out(AY);
	MENU.space(3);
	MENU.out(AY/32768);
#endif

	aY_reaction_source = NULL;
	aY_select_slots = 8;
	aY_sel_i0 = 0;
      }

      if(accGyro_mode & gzM) {		// gyro Z
	GZ = accGyro_current_GZ;

#if defined DEBUG_ACCELGYRO_BASICS
	there_was_output = true;
	MENU.out("\tGZ= ");
	MENU.out(GZ);
#endif
	if(accGyro_invert)	// invert mathematical axis
	  GZ = -GZ;

#if defined DEBUG_ACCELGYRO_BASICS
	MENU.space(3);
	MENU.out(GZ);	// scaled and inverted
#endif
      }

      if(there_was_output)
	MENU.ln();

      // ACCELERO:	INVERTING IS DONE HERE in  accGyro_reaction()
      if(accGyro_mode & axM) {		// accelero X
	selected_aX = AX * aX_select_slots +0.5;
	selected_aX += aX_sel_i0;
	if(accGyro_invert)	// invert mathematical axis
	  selected_aX = aX_select_slots - 1 - selected_aX;
      }

      if(accGyro_mode & ayM) {		// accelero Y
	selected_aY = AY * aY_select_slots +0.5;
	selected_aY += aY_sel_i0;
	if(accGyro_invert)	// invert mathematical axis
	  selected_aY = aY_select_slots - 1 - selected_aY;
      }

      if(accGyro_mode & gzM) {		// gyro Z
	selected_gZ = GZ + 0.5;
	selected_gZ += gZ_sel_i0;
      }

#if defined DEBUG_AG_REACTION
      if (selected_aX != selected_aX_seen || selected_aY != selected_aY_seen || selected_gZ != selected_gZ_seen) {
	MENU.out(F("SELECTED  aX "));
	MENU.out(selected_aX);

	MENU.out(F("\t\taY "));
	MENU.out(selected_aY);

	MENU.out(F("\t\tgZ "));
	MENU.outln(selected_gZ);

	//selected_aY_seen = selected_aY;	// right now unused
	selected_gZ_seen = selected_gZ;	// TODO: DEACTIVATED, see GYRO
      }
#endif

      if(accGyro_mode & axM) {		// accelero X
	unsigned int* jiffle = NULL;
	if(jiffle = index2pointer(JIFFLES, selected_aX)) {
	  if(selected_aX != selected_aX_seen) {
	    selected_aX_seen = selected_aX;
	    select_in(JIFFLES, jiffle);
	    setup_jiffle_thrower_selected(selected_actions);
	    MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));

#if defined USE_MONOCHROME_DISPLAY
	    if(monochrome_is_on()) {
	      monochrome_show_line(3, array2name(JIFFLES, selected_in(JIFFLES)));
	    }
#endif
	  }
	} // aX JIFFLES
      }

      if(accGyro_mode & ayM) {		// accelero Y
	if(selected_aY != selected_aY_seen) {
	  selected_aY_seen = selected_aY;

	  switch(selected_aY) {
	    //      case 0:		// TODO: selected_aY < 1
	    //	// limit--
	    //	break;
	  case 1:	// all but high
	    extended_output(F("LBM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
	    for(int pulse = highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION; // SET upper quarter
	    break;
	  case 2:	// all on
	  case 3:	// all on
	    extended_output(F("LBMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
	    break;
	  case 4:	// middle only
	    extended_output(F("_BM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    MENU.outln(highest_primary);
	    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
	    for(int pulse=lowest_primary + (primary_count/4) +1; pulse <= highest_primary - (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR all
	    break;
	  case 5:	// extremes only
	    extended_output(F("L__H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
	    for(int pulse=lowest_primary; pulse <= lowest_primary + (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR low quarter
	    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
	    break;
	  case 6:	// high only
	    extended_output(F("___H "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=lowest_primary; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION; // SET all
	    for(int pulse=highest_primary - (primary_count/4) +1; pulse <= highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags &= ~noACTION; // CLEAR high quarter
	    break;

	    //      case 7:	// bass_limit--		// TODO: near limit region
	    //	break;
	  default:	// toggle all
	    extended_output(F("~~~~ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    PULSES.select_from_to(lowest_primary, highest_primary);
	    PULSES.selected_toggle_no_actions();
	    PULSES.select_n(voices);
	  } // switch(selected_aY)
	  noAction_flags_line();
	} //selected_aY_seen
      } // accelero Y
      break;

    case 2:
      break;

    default:
      MENU.error_ln(F("unknown accGyro_preset"));
    } // switch (accGyro_preset)
  } // if(accGyro_mode)
}


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
}


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

#define  MPU6050_MODULE_H
#endif
