/*
  mpu6050_module.h

  see:  https://github.com/jrowberg/i2cdevlib

*/

#if ! defined MPU6050_MODULE_H

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

bool volatile new_accelGyro_data=false;
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
    new_accelGyro_data = true;
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

void acceleroGyro_reaction() {
  if(! accelGyro_is_active) {
    return;
  }

  //MENU.out('~');
  float AX = accelGyro_current.ax + 16384;
  float AY = accelGyro_current.ay + 16384;
  AX /= 32786;
  AY /= 32786;
  int AXn = 50;
  int AYn = 12;
  AX *= AXn;
  AY *= AYn;

  int selected_aX = AX + 0.5;
  MENU.out((int) (AX + 0.5));
  MENU.tab();
  int selected_aY = AY + 0.5;
  MENU.out((int) (AY + 0.5));
  MENU.tab();

  new_accelGyro_data = false;

  unsigned int* jiffle = NULL;
  switch(selected_aX) {
  case 0:
    jiffle = PENTAtonic_rise;
    break;
  case 1:
    jiffle = PENTAtonic_desc;
    break;
  case 2:
    jiffle = pentatonic_rise;
    break;
  case 3:
    jiffle = pentatonic_desc;
    break;
  case 4:
    jiffle = tumtum;
    break;
  case 5:
    jiffle = jiff_dec128;
    break;
  case 6:
    jiffle = ding_ditditdit;
    break;
  case 7:
    jiffle = diing_ditditdit;
    break;
  case 8:
    jiffle = din__dididi_dixi;
    break;
  case 9:
    jiffle = din_dididi;
    break;
  case 10:
    jiffle = PENTA_3rd_rise;
    break;
  case 11:
    jiffle = up_THRD;
    break;
  case 12:
    jiffle = up_THRD_dn;
    break;
  case 13:
    jiffle = dwn_THRD;
    break;
  case 14:
    jiffle = dwn_THRD_up;
    break;
  case 15:
    jiffle = PENTA_3rd_down_5;
    break;
  case 16:
    jiffle = penta_3rd_down_5;
    break;
  case 17:
    jiffle = rising_pent_them;
    break;
  case 18:
    jiffle = penta_3rd_rise;
    break;
  case 19:
    jiffle = simple_theme;
    break;
  case 20:
    jiffle = jiff_dec_pizzica;
    break;
  case 21:
    jiffle = pent_patternA;
    break;
  case 22:
    jiffle = pent_patternB;
    break;
  case 23:
    jiffle = pent_top_wave;
    break;
  case 24:
    jiffle = pent_top_wave_0;
    break;
  case 25:
    jiffle = d4096_3072;
    break;
  case 26:
    jiffle = d4096_2048;
    break;
  case 27:
    jiffle = d4096_1024;
    break;
  case 28:
    jiffle = d4096_512;
    break;
  case 29:
    jiffle = d4096_256;
    break;
  case 30:
    jiffle = d4096_128;
    break;
  case 31:
    jiffle = d4096_64;
    break;
  case 32:
    jiffle = d4096_32;
    break;
  case 33:
    jiffle = d4096_16;
    break;
  case 34:
    jiffle = d4096_12;
    break;
  case 35:
    jiffle = tanboura;
    break;
  case 36:
    jiffle = doric_rise;
    break;
  case 37:
    jiffle = minor_rise;
    break;
  case 38:
    jiffle = doric_descend;
    break;
  case 39:
    jiffle = minor_descend;
    break;
  case 40:
    jiffle = major_descend;
    break;
  case 41:
    jiffle = major_rise;
    break;
  case 42:
    jiffle = pentaCHORD_rise;
    break;
  case 43:
    jiffle = tumtumtum;
    break;
  case 44:
    jiffle = tumtumtumtum;
    break;
  case 45:
    jiffle = pentachord_rise;
    break;
  case 46:
    jiffle = pentaCHORD_desc;
    break;
  case 47:
    jiffle = pentachord_descend;
    break;
  case 48:
    jiffle = tetraCHORD_rise;
    break;
  case 49:
    jiffle = tetraCHORD_desc;
    break;
  case 50:
    jiffle = mechanical;
    break;
  default:
    MENU.error_ln(F("acceleroGyro_reaction()"));
  }
  if(jiffle) {
    select_array_in(JIFFLES, jiffle);
    setup_jiffle_thrower_selected(selected_actions);
    MENU.outln(array2name(JIFFLES, selected_in(JIFFLES)));
  }
  MENU.ln();
}

void acceleroGyro_data_display() {
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
