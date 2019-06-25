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
} accelero_gyro_6d_data ;

accelero_gyro_6d_data accelero_gyro_current = { 0 };	// current values (after oversampling)

#if ! defined MPU_OVERSAMPLING
  #define MPU_OVERSAMPLING	16
#endif
accelero_gyro_6d_data mpu_samples[MPU_OVERSAMPLING] = { 0 };

void sample_accelero_giro_6d() {
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

  accelero_gyro_current.ax = AX / MPU_OVERSAMPLING;
  accelero_gyro_current.ay = AY / MPU_OVERSAMPLING;
  accelero_gyro_current.az = AZ / MPU_OVERSAMPLING;

  accelero_gyro_current.gx = GX / MPU_OVERSAMPLING;
  accelero_gyro_current.gy = GY / MPU_OVERSAMPLING;
  accelero_gyro_current.gz = GZ / MPU_OVERSAMPLING;
}

/*
	void getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);
	void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
	int16_t getAccelerationX();
	int16_t getAccelerationY();
	int16_t getAccelerationZ();

	 // GYRO_*OUT_* registers
	void getRotation(int16_t* x, int16_t* y, int16_t* z);
	int16_t getRotationX();
	int16_t getRotationY();
	int16_t getRotationZ();
*/


#define  MPU6050_MODULE_H
#endif
