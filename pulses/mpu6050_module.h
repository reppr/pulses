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

bool mpu6050_available=true;	// will be reset automagically if there's no MPU6050 found
				// this will switch it off, including sampling...

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

//bool accGyro_invert = true;	// OBSOLETE:	// defaults to invert mathemetical axis, which feels right in instrument handling

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

// float  accGyro_current_xx_f			// last accGyro data seen as accGyro_new_data got true		hä? ????
float accGyro_current_AX_f=0.0;
float accGyro_current_AY_f=0.0;
float accGyro_current_AZ_f=0.0;
float accGyro_current_GX_f=0.0;
float accGyro_current_GY_f=0.0;
float accGyro_current_GZ_f=0.0;

void show_current_accGyro_values() {
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
} // show_current_accGyro_values()


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

/*
  pulses does NOT use interrupt version any more

  the interrupt version worked fine so far
  but as i2c activity is quite critical while playing
  i want the program to decide *when* to do it...
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


#if ! defined ACCGYRO_DEFAULT_PRESET
  #define ACCGYRO_DEFAULT_PRESET	1
  //#define ACCGYRO_DEFAULT_PRESET	2
#endif
uint8_t	accGyro_preset = ACCGYRO_DEFAULT_PRESET;

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


/* **************************************************************** */
// new implementations _v2:

// #define DEBUG_SHOW_EACH_MPU_SAMLE
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

#if defined DEBUG_ACCGYRO_SAMPLE
  if(accGyro_new_data && debug_accgyro_sample)	// TODO: REMOVE: runtime debug switching...
    show_current_accGyro_values();
#endif
} // accGyro_sample_v2()


extern bool monochrome_can_be_used();
extern void monochrome_show_line(uint8_t row, char * s);
extern short primary_count;		// TODO: use musicBoxConf.primary_count in next version
extern void noAction_flags_line();

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

    int Gx_i_new=0;
    int Gy_i_new=0;
    int Gz_i_new=0;

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
	Gx_i_new = GX_seen_f + 0.5;
	Gx_i_new += Gx_sel_offset;
      }

      if(accGyro_mode & AG_mode_Gy) {		// gyro Y
	Gy_i_new = GY_seen_f + 0.5;
	Gy_i_new += Gy_sel_offset;
      }

      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
	Gz_i_new = GZ_seen_f + 0.5;
	Gz_i_new += Gz_sel_offset;
      }

      // now react;
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

	  for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.highest_primary; pulse++)	// default ALL UNMUTED
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
	    extended_output(F("L__H"), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=musicBoxConf.lowest_primary + (primary_count/4) +1; pulse <= musicBoxConf.highest_primary - (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute middle two quarters
	    break;

	  case 7:	// middles only
	    extended_output(F("_BM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = musicBoxConf.highest_primary - (primary_count/4) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 6:	// mute lower half
	    extended_output(F("__MH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (primary_count/2); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute lower half
	    break;

	  case 5:	// mute low
	    extended_output(F("_BMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 4:	// all unmuted
	    extended_output(F("LBMH "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    break;

	  case 3:	// mute high
	    extended_output(F("LBM_ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = musicBoxConf.highest_primary - (primary_count/4) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    break;

	  case 2:	// mute upper half
	    extended_output(F("LB__ "), MONOCHROME_MOTION_MUTING_ROW, 0, false);
	    for(int pulse = musicBoxConf.highest_primary - (primary_count/2) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute upper half
	    break;

	  default:
	    MENU.out(F("accGyro_reaction_v2()\tAy_i_new out of range "));
	    MENU.outln(Ay_i_new);
	  }

	  noAction_flags_line();
	} //_selected_Ay_i_seen
      } // accelero Y

      // gyro X not used
      // gyro Y not used

      // gyro Z
      if(accGyro_mode & AG_mode_Gz) {		// accelero Y
	if(Gz_i_new != _selected_Gz_i_seen) {
	  _selected_Gz_i_seen = Gz_i_new;
	  extern void sync_shifting(fraction_t shift);
	  // sync_shifting({_selected_Gz_i_seen, 16*4096});	// TODO: FIND&TRIMM new DEFAULT for version2
	  sync_shifting({_selected_Gz_i_seen, 4*4096});		// TODO: FIND&TRIMM new DEFAULT for version2
//#if defined DEBUG_AG_REACTION		// TODO: how and when to report? ################
	  MENU.out(F("sync_shifting "));
	  MENU.outln(_selected_Gz_i_seen);
//#endif
	}
      }
      break; // preset 1

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
