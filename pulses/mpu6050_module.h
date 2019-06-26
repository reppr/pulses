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

#define ONE_SHOT_ACCELGYRO
#if ! defined ONE_SHOT_ACCELGYRO
  // take one 6d sample
  mpu6050.getMotion6(&mpu_samples[mpu_sample_index].ax, &mpu_samples[mpu_sample_index].ay, &mpu_samples[mpu_sample_index].az, \
		     &mpu_samples[mpu_sample_index].gx, &mpu_samples[mpu_sample_index].gy, &mpu_samples[mpu_sample_index].gz);
#else
  // TEST: take just *ONE* data
  mpu_samples[mpu_sample_index].ax = mpu6050.getAccelerationX();
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
