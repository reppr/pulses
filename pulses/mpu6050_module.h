/*
  mpu6050_module.h

  see:  https://github.com/jrowberg/i2cdevlib

*/

#if ! defined MPU6050_MODULE_H

int16_t accelGyro_offsets[] = {-1493, -2125, 1253, 98, 85, -50};	// aX, aY, aZ, gX, gY, gZ offsets
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


bool accelGyro_is_active = false ;	// accelGyro switching by the program
bool accelGyro_invert = true;		// defaults to invert mathemetical axis, which feels right in instrument handling

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

    MENU.out(F("set accelGyro offsets\t{"));
    int16_t o;

    mpu6050.setXAccelOffset(o = accelGyro_offsets[0]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYAccelOffset(o = accelGyro_offsets[1]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZAccelOffset(o = accelGyro_offsets[2]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setXGyroOffset(o = accelGyro_offsets[3]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYGyroOffset(o = accelGyro_offsets[4]);
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZGyroOffset(o = accelGyro_offsets[5]);
    MENU.out(o);
    MENU.outln('}');

    return true;
  }

  MENU.out(F("failed: "));
  MENU.outln(mpu6050.getDeviceID());
  return false;
}

typedef struct {
  int16_t ax;
  int16_t ay;
  int16_t az;
  int16_t gx;
  int16_t gy;
  int16_t gz;
} accelGyro_6d_data ;

accelGyro_6d_data accelGyro_current = { 0 };	// current values (after oversampling)

#if ! defined MPU_OVERSAMPLING
  //#define MPU_OVERSAMPLING	15
  //#define MPU_OVERSAMPLING	7	// ok, but slow
  #define MPU_OVERSAMPLING	4	// TEST
  //#define MPU_OVERSAMPLING	2	// quite usable, but sounds trigger accelero...
  //#define MPU_OVERSAMPLING	1	// possible
#endif
accelGyro_6d_data mpu_samples[MPU_OVERSAMPLING] = { 0 };

bool volatile accelGyro_new_data=false;
hw_timer_t * accelGyro_timer = NULL;

//#define WITH_TIMER_MUX	// DEACTIVATED for a test	TODO: REMOVE:
#if defined WITH_TIMER_MUX
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif

//void IRAM_ATTR accelGyro_sample_ISR() {
void accelGyro_sample_ISR() {	// test	from *outside* interrupt context	TODO: RENAME:
  static int16_t mpu_sample_index=0;

//#define ACCELGYRO_SAMPLES	ALL
// or individual parameters:
#define ACCELGYRO_SAMPLES_AX
#define ACCELGYRO_SAMPLES_AY
//#define ACCELGYRO_SAMPLES_AZ
//#define ACCELGYRO_SAMPLES_GX
//#define ACCELGYRO_SAMPLES_GY
#define ACCELGYRO_SAMPLES_GZ

#if ACCELGYRO_SAMPLES == ALL
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

    accelGyro_current.ax = AX / MPU_OVERSAMPLING;
    accelGyro_current.ay = AY / MPU_OVERSAMPLING;
    accelGyro_current.az = AZ / MPU_OVERSAMPLING;

    accelGyro_current.gx = GX / MPU_OVERSAMPLING;
    accelGyro_current.gy = GY / MPU_OVERSAMPLING;
    accelGyro_current.gz = GZ / MPU_OVERSAMPLING;

#if defined WITH_TIMER_MUX
    portENTER_CRITICAL_ISR(&timerMux);
#endif
    accelGyro_new_data = true;
#if defined WITH_TIMER_MUX
    portEXIT_CRITICAL_ISR(&timerMux);
#endif

    //MENU.out('%');
  }
}

void activate_accelGyro() {
  //  accelGyro_timer = timerBegin(0, 80000, true);	// milliseconds
  accelGyro_timer = timerBegin(0, 80, true);	// microseconds
  timerAttachInterrupt(accelGyro_timer, &accelGyro_sample_ISR, true);
  timerAlarmWrite(accelGyro_timer, 1000000, true);
  timerAlarmEnable(accelGyro_timer);
}


//	void calibrate_accelGyro_offsets() {			// TODO: rewrite or REMOVE:
//	  #define ACCELGYRO_CALIBRATE_OVERSAMPLING	16
//	  MENU.outln(F("\tcalibrate_accelGyro_offsets()"));
//
//	  int32_t AX=0, AY=0, AZ=0, GX=0, GY=0, GZ=0;
//	  for(int i=0; i<ACCELGYRO_CALIBRATE_OVERSAMPLING; i++) {
//	    AX += mpu6050.getAccelerationX();
//	    AY += mpu6050.getAccelerationY();
//	    AZ += mpu6050.getAccelerationZ();
//	    GX += mpu6050.getRotationX();
//	    GY += mpu6050.getRotationY();
//	    GZ += mpu6050.getRotationZ();
//	  }
//
//	  ax_off0 = AX / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//	  ax_off0 -= 16384;
//
//	  ay_off0 = AY / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//	  ay_off0 -= 16384;
//
//	  az_off0 = AZ / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//	  gx_off0 = GX / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//	  gy_off0 = GY / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//	  gz_off0 = GZ / ACCELGYRO_CALIBRATE_OVERSAMPLING;
//
//	  MENU.out(ax_off0); MENU.tab();
//	  MENU.out(ay_off0); MENU.tab();
//	  MENU.out(az_off0); MENU.tab();
//	  MENU.out(gx_off0); MENU.tab();
//	  MENU.out(gy_off0); MENU.tab();
//	  MENU.out(gz_off0);
//	}

enum AG_modes {
  axM=1,
  ayM=2,
  azM=4,

  gxM=8,
  gyM=16,
  gzM=32,
} ;

int accelGyro_mode=0;	// zero means inactive

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


// accelGyro_reaction()
extern void monochrome_show_line(uint8_t row, char * s);	// pre declaration
#define DEBUG_AG_REACTION		// *DO* show selected slices
//#define DEBUG_ACCELGYRO_BASICS	// deactivated
void accelGyro_reaction() {
  bool there_was_output=false;
  if(accelGyro_mode) {
    static int selected_aX_seen;
    static int selected_aY_seen;
    static int selected_gZ_seen;

    float AX=0.0, AY=0.0, AZ=0.0, GX=0.0, GY=0.0, GZ=0.0;
    accelGyro_new_data = false;
    if(! accelGyro_is_active)
      return;

    reset_accGyro_selection();	// offsets and slots

    if(accelGyro_mode & axM) {		// accelero X
      AX = accelGyro_current.ax + 16384;
#if defined DEBUG_ACCELGYRO_BASICS
      there_was_output = true;
      MENU.out("AX+ ");
      MENU.out(AX);
      MENU.out("  ax ");
      MENU.out(accelGyro_current.ax);
      MENU.space(3);
      MENU.out(AX/32768);
#endif
      AX /= 32768;	// to float scaling

      aX_reaction_source = JIFFLES;
      aX_select_slots = 66;
      aX_sel_i0 = 0;
    }

    if(accelGyro_mode & ayM) {		// accelero Y
      AY = accelGyro_current.ay + 16384;
#if defined DEBUG_ACCELGYRO_BASICS
      there_was_output = true;
      MENU.out("\t\tAY+ ");
      MENU.out(AY);
      MENU.out("  ay ");
      MENU.out(accelGyro_current.ay);
      MENU.space(3);
      MENU.out(AY/32768);
#endif
      AY /= 32768;	// to float scaling

      aY_reaction_source = NULL;
      aY_select_slots = ACCELERO_SELECTION_SLOTS_DEFAULT;
      aY_sel_i0 = 0;
    }

    if(accelGyro_mode & gzM) {		// gyro Z
      GZ = accelGyro_current.gz;

#if defined DEBUG_ACCELGYRO_BASICS
      there_was_output = true;
      MENU.out("\t\tGZ= ");
      MENU.out(GZ);
      MENU.out("  gz ");
      MENU.out(accelGyro_current.gz);
#endif
      GZ /= 32;			// TODO: is *RANDOM* float scaling
      if(accelGyro_invert)	// invert mathematical axis
	GZ = -GZ;

#if defined DEBUG_ACCELGYRO_BASICS
      MENU.space(3);
      MENU.out(GZ);
#endif
    }

    if(there_was_output)
      MENU.ln();

    // select slice:
    int selected_aX = AX * aX_select_slots +0.5;
    selected_aX += aX_sel_i0;
    if(accelGyro_invert)	// invert mathematical axis
      selected_aX = aX_select_slots - 1 - selected_aX;

    int selected_aY = AY * aY_select_slots +0.5;
    selected_aY += aY_sel_i0;
    if(accelGyro_invert)	// invert mathematical axis
      selected_aY = aY_select_slots - 1 - selected_aY;

    int selected_gZ = GZ + 0.5;
    selected_gZ += gZ_sel_i0;

#if defined DEBUG_AG_REACTION
    if (selected_aX != selected_aX_seen || selected_aY != selected_aY_seen || selected_gZ != selected_gZ_seen)
      //if (selected_aX != selected_aX_seen || selected_aY != selected_aY_seen)
#else
      if (selected_aX != selected_aX_seen)
#endif
	{
	  MENU.out(F("SELECTED  aX "));
	  MENU.out(selected_aX);

	  MENU.out(F("\t\taY "));
	  MENU.out(selected_aY);

	  MENU.out(F("\t\tgZ "));
	  MENU.outln(selected_gZ);

	  selected_aY_seen = selected_aY;	// TODO: DEACTIVATED, unused
	  selected_gZ_seen = selected_gZ;	// TODO: DEACTIVATED, unused
	}

    unsigned int* jiffle = NULL;
    if(jiffle = index2pointer(JIFFLES, selected_aX)) {
      if(selected_aX != selected_aX_seen) {
	select_array_in(JIFFLES, jiffle);
	setup_jiffle_thrower_selected(selected_actions);
	MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));
	selected_aX_seen = selected_aX;

#if defined USE_MONOCHROME_DISPLAY
	if(morse_feedback_while_playing || musicbox_is_idle()) {
	  monochrome_show_line(3, array2name(JIFFLES, selected_in(JIFFLES)));
	}
#endif
      }
    }
  } // if(accelGyro_mode)
}

void accelGyro_data_display() {
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


#define DEBUG_SAMPLE_OFFSETS
void accelGyro_sample_offsets() {

}


/*
void sample_accelGyro_6d() {	// too heavy,
  static int16_t mpu_sample_index=0;

  mpu6050.getMotion6(&mpu_samples[mpu_sample_index].ax, &mpu_samples[mpu_sample_index].ay, &mpu_samples[mpu_sample_index].az, \
		     &mpu_samples[mpu_sample_index].gx, &mpu_samples[mpu_sample_index].gy, &mpu_samples[mpu_sample_index].gz);

  int32_t AX=0, AY=0, AZ=0, GX=0, GY=0, GZ=0;

  for(int i=0; i < MPU_OVERSAMPLING; i++) {
    AX += mpu_samples[i].ax;
    AY += mpu_samples[i].ay;
    AZ += mpu_samples[i].az;
    GX += mpu_samples[i].gx;
    GY += mpu_samples[i].gy;
    GZ += mpu_samples[i].gz;
  }

  accelGyro_current.ax = AX / MPU_OVERSAMPLING;
  accelGyro_current.ay = AY / MPU_OVERSAMPLING;
  accelGyro_current.az = AZ / MPU_OVERSAMPLING;

  accelGyro_current.gx = GX / MPU_OVERSAMPLING;
  accelGyro_current.gy = GY / MPU_OVERSAMPLING;
  accelGyro_current.gz = GZ / MPU_OVERSAMPLING;
}
*/

#define  MPU6050_MODULE_H
#endif
