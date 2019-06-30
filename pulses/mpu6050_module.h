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
#define MPU_OVERSAMPLING	7
//#define MPU_OVERSAMPLING	4
//#define MPU_OVERSAMPLING	2
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

extern void monochrome_show_line(uint8_t row, char * s);	// pre declaration

#define DEBUG_AG_REACTION
#define DEBUG_ACCELGYRO_BASICS
void accelGyro_reaction() {
  static int selected_aX_seen;
  static int selected_aY_seen;
  //static int selected_gZ_seen;

  accelGyro_new_data = false;
  if(! accelGyro_is_active)
    return;

  float AX = accelGyro_current.ax + 16384;

#if defined DEBUG_ACCELGYRO_BASICS
  //  MENU.out("AX+16384 ");
  MENU.out("AX ");
  MENU.out(AX);
  MENU.out("\tax ");
  MENU.out(accelGyro_current.ax);
#endif

  AX /= 32768;

#if defined DEBUG_ACCELGYRO_BASICS
  MENU.tab();
  MENU.out(AX);
  MENU.tab(2);
#endif

  int AXn = 66;
  AX *= AXn;

  // TODO: AY is *not* working yet, AX does...
//float AY = accelGyro_current.ay - ay_off0;
//float AY = accelGyro_current.ay;
float AY = accelGyro_current.ay + 16384;
//float AY = accelGyro_current.ay + 16384/2;

#if defined DEBUG_ACCELGYRO_BASICS
//MENU.out("AY +00000 ");
//MENU.out("AY+16384  ");
//MENU.out("AY+16384/2 ");
  MENU.out("AY ");
  MENU.out(AY);
  MENU.out("\tay ");
  MENU.out(accelGyro_current.ay);
#endif

  AY /= 32768;
#if defined DEBUG_ACCELGYRO_BASICS
  MENU.tab();
  //  MENU.out(F("\t/32768 "));
  MENU.out(AY);
  MENU.ln();
#endif

  int AYn = 30;		// TODO: FIXME: as is  works only with n=30
  AY *= AYn;
#if defined DEBUG_ACCELGYRO_BASICS
  // MENU.out(F("AY *n "));
  // MENU.out(AY);
#endif

//float GZ = accelGyro_current.gz - gz_off0;

  int selected_aX = AX + 1.5;	// TODO: rethink
//int selected_aY = AY + 1.5;	// TODO: rethink
  int selected_aY = AY -0.5;	// TODO: rethink

#if defined DEBUG_AG_REACTION
  if (selected_aX != selected_aX_seen || selected_aY != selected_aY_seen)
#else
  if (selected_aX != selected_aX_seen)
#endif
    {
      MENU.out(F("selected\t"));
      MENU.out(selected_aX);
      MENU.tab(2);
      MENU.out(selected_aY);
      MENU.ln();

      selected_aY_seen = selected_aY;	// TODO: DEACTIVATED, unused
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
