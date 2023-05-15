/*
  mpu6050_module.h

  see:  https://github.com/jrowberg/i2cdevlib
*/

#if ! defined MPU6050_MODULE_H

//#define DEBUG_AG_REACTION	// TODO: remove or fix the debug mess
//#define DEBUG_ACCELGYRO_BASICS	// TODO: remove or fix the debug mess

#define MPU6050_SAMPLE_TASK_PRIORITY	0

bool display_accGyro_raw=false;	  // for debugging only

bool mpu6050_available=true;	// will be reset automagically if there's no MPU6050 found
				// this will switch it off, including sampling...

#include <I2Cdev.h>
#include <MPU6050.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

//MPU6050 mpu6050(HARDWARE.mpu6050_addr);	// not possible here...
MPU6050 mpu6050(USE_MPU6050_at_ADDR);

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


bool mpu6050_setup() {		// Wire must be started before
  MENU.out(F("mpu6050_setup()  "));
  MENU.out_hex(HARDWARE.mpu6050_addr);

  mpu6050.initialize();

  mpu6050_available=true;
  if(mpu6050.testConnection()) {
    MENU.outln(F("  MPU6050 connected"));

    MENU.out(F("set accGyro offsets\t{"));
    int16_t o;
    bool mpu_offsets_zero=true;

    mpu6050.setXAccelOffset(o = HARDWARE.accGyro_offsets[0]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYAccelOffset(o = HARDWARE.accGyro_offsets[1]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZAccelOffset(o = HARDWARE.accGyro_offsets[2]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setXGyroOffset(o = HARDWARE.accGyro_offsets[3]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setYGyroOffset(o = HARDWARE.accGyro_offsets[4]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.out(',');
    MENU.space();

    mpu6050.setZGyroOffset(o = HARDWARE.accGyro_offsets[5]);
    if(o)
      mpu_offsets_zero = false;
    MENU.out(o);
    MENU.outln('}');
    if(mpu_offsets_zero) {
      ERROR_ln(F("accGyro not calibrated"));
#if defined HAS_DISPLAY	// let the user read the message (happens only while booting)
      delay(5500);	//     it's only visible for a second or so, *not* 5.5
#endif
    }
    MENU.ln();

    return true;	// OK (but calibration *might* be missing)
  } // else

  MENU.out(F("\tfailed: "));
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

// int accGyro_mode=0;	// zero means inactive
int accGyro_mode = AG_mode_Gz | AG_mode_Ay | AG_mode_Ax;	// restored old default

float Ax_select_offset_f=0.0;
float Ay_select_offset_f=0.0;
float Az_select_offset_f=0.0;
float Gx_select_offset_f=0.0;
float Gy_select_offset_f=0.0;
float Gz_select_offset_f=0.0;

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


extern uint8_t extended_output(char* data, uint8_t col, uint8_t row, bool force);
#if ! defined MONOCHROME_MOTION_STATE_ROW
  #define MONOCHROME_MOTION_STATE_ROW	7
#endif
#if ! defined MONOCHROME_MOTION_MUTING_ROW
  #define MONOCHROME_MOTION_MUTING_ROW	0
#endif

void display_accGyro_mode() {
  if(!mpu6050_available) {	// should not happen, just in case
    extended_output(F("no MPU6050 "), 0,0,false);
    MENU.ln();
    return;
  } // else

  MENU.out(F("accGyro mode: "));

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

#if defined HAS_OLED
  extended_output(buffer, 0, MONOCHROME_MOTION_STATE_ROW, false);
#elif defined HAS_DISPLAY
  MC_display_message(buffer);
#endif
} // display_accGyro_mode()


void reset_accGyro_selection() {	// reset accGyro selections, slots, reaction sources
  Ax_select_offset_f=0.0;
  Ay_select_offset_f=0.0;
  Az_select_offset_f=0.0;

  Gx_select_offset_f=0.0;
  Gy_select_offset_f=0.0;
  Gz_select_offset_f=0.0;

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
} // reset_accGyro_selection()


enum accgyro_preset_modes {
  ACCGYR_PRES_MODE_RAW=0,
  ACCGYR_PRES_MODE_AXAYGZ=1,
  ACCGYR_PRES_MODE_MUTE_VOLUME_JIFF,
  ACCGYR_PRES_MODE_TUNING_Y,
  ACCGYR_PRES_MODE_MAX,
};


#if ! defined ACCGYRO_DEFAULT_PRESET
//#define ACCGYRO_DEFAULT_PRESET	ACCGYR_PRES_MODE_AXAYGZ
#define ACCGYRO_DEFAULT_PRESET	ACCGYR_PRES_MODE_MUTE_VOLUME_JIFF
//#define ACCGYRO_DEFAULT_PRESET	ACCGYR_PRES_MODE_TUNING_Y
//#define ACCGYRO_DEFAULT_PRESET	ACCGYR_PRES_MODE_RAW
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


void accGyro_toggle_TUNING_mode() {
  extern uint32_t accgyro_modulus;
  if(accGyro_preset==ACCGYR_PRES_MODE_TUNING_Y && accGyro_is_active) { // WAS tuning, so switch it off
    accGyro_is_active = false;
    accGyro_preset = ACCGYR_PRES_MODE_AXAYGZ;	// not so sure where to switch to ;)
    accgyro_modulus = 21221;	// prime	// TODO: UI

  } else {					// switch to TUNING
    MENU.outln(F("TUNING AY mode"));
    accGyro_preset = ACCGYR_PRES_MODE_TUNING_Y;
    accGyro_is_active = true;
    //sweep_up = 0;				// keep mental sanity ;)
    extern void entune_basic_musicbox_pulses();
    entune_basic_musicbox_pulses();
    accgyro_modulus = 2027;	// prime	// TODO: UI
  }
} // accGyro_toggle_TUNING_mode()


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
    ERROR_ln(F("accGyro_sample_v2() mpu6050_available=false"));	// catch bugs, if any ;)  TODO: REMOVE:
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
  if(accX_invert) {			// INVERT mathematical X axis?
    accGyro_current_AX_f *= -1.0;
    accGyro_current_AX_f += 1.0;	// 0..1
  } // else	not tested

  accGyro_current_AY_f = (ay_i / mpu_oversampling) + 16384.0;
  accGyro_current_AY_f /= 32768.0;	// to float scaling
  if(accY_invert) {			// INVERT mathematical Y axis?
    accGyro_current_AY_f *= -1.0;
    accGyro_current_AY_f += 1.0;	// 0..1
  } // else	nothing to do

  accGyro_current_AZ_f = (az_i / mpu_oversampling) + 16384.0;
  accGyro_current_AZ_f /= 32768.0;	// to float scaling
  if(accZ_invert) {			// INVERT mathematical Z axis?
    accGyro_current_AZ_f *= -1.0;
    accGyro_current_AZ_f += 1.0;	// ????
  }

  // GYRO:
  accGyro_current_GX_f = (gx_i / mpu_oversampling);
  accGyro_current_GX_f /= gyro_float_scaling;
  if(gyrX_invert)			// INVERT gyrX direction?
    accGyro_current_GX_f *= -1;

  accGyro_current_GY_f = (gy_i / mpu_oversampling);
  accGyro_current_GY_f /= gyro_float_scaling;
  if(gyrY_invert)			// INVERT gyrY direction?
    accGyro_current_GY_f *= -1;

  accGyro_current_GZ_f = (gz_i / mpu_oversampling);
  accGyro_current_GZ_f /= gyro_float_scaling;
  if(gyrZ_invert)			// INVERT gyrZ direction?
    accGyro_current_GZ_f *= -1;

  if(display_accGyro_raw && accGyro_new_data) {		// debugging
    // do_on_other_core(&show_current_accGyro_values);	// could be dangerous
    show_current_accGyro_values();
  }
} // accGyro_sample_v2()


#if defined  MULTICORE_MPU_SAMPLING
TaskHandle_t multicore_sample_mpu_handle;

void multicore_sample_mpu_task(void* dummy) {
  accGyro_sample_v2();
  vTaskDelete(NULL);
}


#if ! defined MC_SAMPLE_MCU_STACK_SIZE
  #define MC_SAMPLE_MCU_STACK_SIZE	4*1024		// TODO: test
#endif
void multicore_sample_mpu() {	// create and do one shot task
  BaseType_t err = xTaskCreatePinnedToCore(multicore_sample_mpu_task,		// function
					   "sample mpu",			// name
					   MC_SAMPLE_MCU_STACK_SIZE,		// stack size
					   NULL,				// task input parameter
					   MPU6050_SAMPLE_TASK_PRIORITY,	// task priority
					   &multicore_sample_mpu_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    ERROR_ln(F("sample mpu"));
  }
} // multicore_sample_mpu()
#endif


extern void noAction_flags_line();
void narrow_activity_range(uint8_t lowest_unmuted, uint8_t highest_unmuted) {
  int pulse = highest_unmuted;
  bool do_mute = false;
  unsigned int unmuted_mask = ~noACTION;
  for(int cnt=0; cnt < voices; cnt++) {
    if(do_mute)
      PULSES.pulses[pulse].action_flags |= noACTION;
    else
      PULSES.pulses[pulse].action_flags &= unmuted_mask;

    if(pulse == lowest_unmuted)
      do_mute = true;
    if(pulse == musicBoxConf.lowest_primary)
      pulse += voices;
    pulse--;
  }

  if(MENU.maybe_display_more(VERBOSITY_SOME))
    noAction_flags_line();
} // narrow_activity_range()


extern bool monochrome_can_be_used();
extern void monochrome_show_line(uint8_t row, char * s);
extern bool do_recalibrate_Y_ui;

void accGyro_reaction_v2() {	// react on data coming from accGyro_sample()
  if(accGyro_new_data && accGyro_mode) {
    accGyro_new_data = false;

    if(! accGyro_is_active) {	// maybe catch errors here, if any?	TODO: REMOVE:
      ERROR_ln(F("accGyro_reaction_v2():\taccGyro_is_active is false"));
      return;
    }

    // last selected slice, integer
    static int _selected_Ax_i_seen;
    static int _selected_Ay_i_seen;
    // static int _selected_Az_i_seen;	// TODO: unused?

    // static int _selected_Gx_i_seen;	// TODO: unused?
    // static int _selected_Gy_i_seen;	// TODO: unused?
    static int _selected_Gz_i_seen;

    float Ax_f_new=0.0;
    float Ay_f_new=0.0;
    float Az_f_new=0.0;

    int Ax_i_new=0;
    int Ay_i_new=0;
    int Az_i_new=0;

    float Gx_f_new=0.0;
    float Gy_f_new=0.0;
    float Gz_f_new=0.0;

    int Gx_i_new=0;
    int Gy_i_new=0;
    int Gz_i_new=0;

    float AX_seen_f=0.0;
    float AY_seen_f=0.0;
    float AZ_seen_f=0.0;

    float GX_seen_f=0.0;
    float GY_seen_f=0.0;
    float GZ_seen_f=0.0;

    // reset_accGyro_selection();	// offsets and slots	TODO: check if not partly needed

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

    switch(accGyro_preset) {			// switch accGyro_preset mode
    case ACCGYR_PRES_MODE_AXAYGZ:		// accGyro_preset 1
      // ACCELERO;
      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	Ax_reaction_source = JIFFLES;
	//Ax_select_offset_f = 0.0;		// TODO: check removal
	Ax_select_slots = 41;			// TODO: TRIMM:
      }

      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
	Ay_reaction_source = NULL;
	Ay_select_slots = 8;
	//Ay_select_offset_f = 0.0;		// TODO: check removal
      }

      if(accGyro_mode & AG_mode_Az) {		// accelero Z
	Az_reaction_source = NULL;
	Az_select_slots = 8;
	//Az_select_offset_f = 0.0;		// TODO: check removal
      }

      // ACCELERO:
      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	Ax_f_new = AX_seen_f * Ax_select_slots;
	if(do_recalibrate_Y_ui) {
	  Ax_select_offset_f = pointer2index(JIFFLES, selected_in(JIFFLES)) - Ax_f_new - 0.25;	// - 0.25 seems to do good...
	  do_recalibrate_Y_ui = false;
	}
	Ax_f_new += Ax_select_offset_f;
	Ax_i_new = (int) (Ax_f_new + 0.5);
      }

      if(accGyro_mode & AG_mode_Ay) {		// accelero Y
	Ay_f_new = AY_seen_f * Ay_select_slots;
	Ay_f_new += Ay_select_offset_f;
	Ay_i_new = (int) (Ay_f_new + 0.5);
      }

      if(accGyro_mode & AG_mode_Az) {		// accelero Z
	Az_f_new = AZ_seen_f * Az_select_slots;
	Az_f_new += Az_select_offset_f;
	Az_i_new = (int) (Az_f_new + 0.5);
      }


      // GYRO:
      if(accGyro_mode & AG_mode_Gx) {		// gyro X
	Gx_f_new = GX_seen_f;
	Gx_f_new += Gx_select_offset_f;
	Gx_i_new = (int) Gx_f_new;	// TODO: + 0.5 ???
      }

      if(accGyro_mode & AG_mode_Gy) {		// gyro Y
	Gy_f_new = GY_seen_f;
	Gy_f_new += Gy_select_offset_f;
	Gy_i_new = (int) Gy_f_new;	// TODO: + 0.5 ???
      }

      if(accGyro_mode & AG_mode_Gz) {		// gyro Z
	Gz_f_new = GZ_seen_f;
	Gz_f_new += Gz_select_offset_f;
	Gz_i_new = (int) Gz_f_new;	// TODO: + 0.5 ???
      }

      // now react;
      if(accGyro_mode & AG_mode_Ax) {		// accelero X
	unsigned int* jiffle = NULL;
	if((jiffle = index2pointer(JIFFLES, Ax_i_new))) {
	  if(Ax_i_new != _selected_Ax_i_seen) {
	    _selected_Ax_i_seen = Ax_i_new;
	    select_in(JIFFLES, jiffle);
//#if defined ICODE_INSTEAD_OF_JIFFLES
//	    en_icode_seeder_selected((icode_t*) selected_in(iCODEs), selected_actions);
//#else
	    setup_jiffle_thrower_selected(selected_actions);	// TODO: jiffle or iCODE ???	###########
//#endif
	    MENU.outln(selected_name(JIFFLES));

#if defined HAS_OLED
	    extern void MC_big_or_multiline(uint8_t row, const char* str);
	    MC_big_or_multiline(2, selected_name(JIFFLES));	// TODO: jiffle or iCODE ???	###########
#elif defined HAS_DISPLAY
	    MC_display_message(selected_name(JIFFLES));	    	// TODO: jiffle or iCODE ???	###########
#endif
	  }
	} else {
	  ERROR_ln(F("no jiffle?"));
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
	    extended_output(F("L__H"), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse=musicBoxConf.lowest_primary + (musicBoxConf.primary_count/4) +1; pulse <= musicBoxConf.highest_primary - (musicBoxConf.primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute middle two quarters
	    break;

	  case 7:	// middles only
	    extended_output(F("_BM_ "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse = musicBoxConf.highest_primary - (musicBoxConf.primary_count/4) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (musicBoxConf.primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 6:	// mute lower half
	    extended_output(F("__MH "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (musicBoxConf.primary_count/2); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute lower half
	    break;

	  case 5:	// mute low
	    extended_output(F("_BMH "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse=musicBoxConf.lowest_primary; pulse <= musicBoxConf.lowest_primary + (musicBoxConf.primary_count/4); pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute low quarter
	    break;

	  case 4:	// all unmuted
	    extended_output(F("LBMH "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    break;

	  case 3:	// mute high
	    extended_output(F("LBM_ "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse = musicBoxConf.highest_primary - (musicBoxConf.primary_count/4) +1; pulse <= musicBoxConf.highest_primary; pulse++)
	      PULSES.pulses[pulse].action_flags |= noACTION;	// mute high quarter
	    break;

	  case 2:	// mute upper half
	    extended_output(F("LB__ "), 0, MONOCHROME_MOTION_MUTING_ROW, false);
	    for(int pulse = musicBoxConf.highest_primary - (musicBoxConf.primary_count/2) +1; pulse <= musicBoxConf.highest_primary; pulse++)
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
      if(accGyro_mode & AG_mode_Gz) {		// gyro Y
	if(Gz_i_new != _selected_Gz_i_seen) {
	  _selected_Gz_i_seen = Gz_i_new;
	  extern int sync_shifting_divisor;
	  extern int sync_shifting_multiplier;
#if defined PULSES_USE_DOUBLE_TIMES
	  extern void sync_shifting(int multiplier, int divisor);
	  sync_shifting(_selected_Gz_i_seen * sync_shifting_multiplier, sync_shifting_divisor);
#else
	  extern void sync_shifting(Harmonical::fraction_t shift);
	  sync_shifting({_selected_Gz_i_seen * sync_shifting_multiplier, sync_shifting_divisor});
#endif // PULSES_USE_DOUBLE_TIMES

//#if defined DEBUG_AG_REACTION		// fixme: TODO: how and when to report? ################
	  MENU.out(F("sync_shifting "));
	  MENU.outln(_selected_Gz_i_seen);
//#endif
	}
      }
      break;		// ACCGYR_PRES_MODE_AXAYGZ

    case ACCGYR_PRES_MODE_MUTE_VOLUME_JIFF:
      if(accGyro_mode & AG_mode_Ax) {		// accelero X	volume
	float volume = /*scaling*/2.4 * (accGyro_current_AX_f - /*zero point*/0.15);
	/*
	MENU.out(accGyro_current_AX_f, 4);
	MENU.tab();
	MENU.out(volume, 4);
	MENU.ln();
	*/
	if(volume > 1.0)	// check range
	  volume = 1.0;
	if(volume < 0.0)
	  volume = 0.0;
	PULSES.volume = volume;
	if(MENU.maybe_display_more(VERBOSITY_MORE)) {
	  MENU.out(F("volume\t"));
	  MENU.out(PULSES.volume, 4);
	  MENU.ln();
	}
      } // volume on AX

      if(accGyro_mode & AG_mode_Ay) {	// accelero Y	action muting range
	static uint8_t lowest_unmuted = musicBoxConf.lowest_primary;
	static uint8_t highest_unmuted = musicBoxConf.highest_primary;

	uint8_t lowest_unmuted_seen = lowest_unmuted;
	uint8_t highest_unmuted_seen = highest_unmuted;

// #define DEBUG_ACTION_NARROWING
#if defined DEBUG_ACTION_NARROWING
	MENU.out(accGyro_current_AY_f, 4);
#endif
	float diff = accGyro_current_AY_f - 0.5;
	int muted;
	if(diff < -0.05) {
	  muted = voices * (diff + 0.05) / 0.45;
	  highest_unmuted = musicBoxConf.highest_primary + muted + 1 ;	// muted is negative
	  if(highest_unmuted > musicBoxConf.highest_primary)		// check range
	    highest_unmuted = musicBoxConf.highest_primary;
	  else if(highest_unmuted < musicBoxConf.lowest_primary)
	    highest_unmuted = musicBoxConf.lowest_primary;
#if defined DEBUG_ACTION_NARROWING
	  MENU.out("\tSKY ");
	  MENU.out(muted);
	  MENU.out(F("\tTOP unmuted\t"));
	  MENU.out(highest_unmuted);
#endif

	} else if(diff > 0.05) {
	  muted = voices * (diff - 0.05) / 0.45;
	  lowest_unmuted = musicBoxConf.lowest_primary + muted;		// muted is positive
	  if(lowest_unmuted > musicBoxConf.highest_primary)		// check range
	    lowest_unmuted = musicBoxConf.highest_primary;
#if defined DEBUG_ACTION_NARROWING
	  MENU.out("\tLOW ");
	  MENU.out(muted);
	  MENU.out(F("\tBOTTOM unmuted\t"));
	  MENU.out(lowest_unmuted);
	} else {
	  MENU.out("\tdead zone 0\t\t");
#endif
	}

#if defined DEBUG_ACTION_NARROWING
	MENU.tab();
	MENU.out(lowest_unmuted);
	MENU.out(F("..."));
	MENU.outln(highest_unmuted);
	MENU.verbosity=VERBOSITY_SOME;	// brute
#endif
	if(highest_unmuted_seen != highest_unmuted || lowest_unmuted_seen != lowest_unmuted)
	  narrow_activity_range(lowest_unmuted, highest_unmuted);
      } // accelero Y	action muting range

      if(accGyro_mode & AG_mode_Gz) {	// gyro Z jiffle +-
	int diff = (int) (accGyro_current_GZ_f / 20.0);
	/*
	MENU.out(accGyro_current_GZ_f, 4);
	MENU.tab(2);
	MENU.out(accGyro_current_GZ_f / 20, 4);
	MENU.tab(2);
	MENU.out((int) (accGyro_current_GZ_f / 20.0));
	MENU.ln();
	*/
	/*
	if(diff)				// TODO: very rude quick hack!
	  if(diff>0)
	    for(; diff ; diff--)
	      MENU.play_KB_macro(F("J+"));
	  else
	    for(; diff ; diff++)
	      MENU.play_KB_macro(F("J-"));
	*/
	if(diff) {
	  int i = pointer2index(JIFFLES, selected_in(JIFFLES)) + diff;
	  if(i<DB_items(JIFFLES) && i >= 0) {
	    select_in(JIFFLES,index2pointer(JIFFLES, i));
	    setup_jiffle_thrower_selected(selected_actions);

	    extern uint8_t extended_output(char* data, uint8_t col=0, uint8_t row=0, bool force=false);
	    extended_output(selected_name(JIFFLES));
	    MENU.ln();
	  }
	}
      }	// gyro Z jiffle +-

      break; // ACCGYR_PRES_MODE_MUTE_VOLUME_JIFF

    // TUNING MODE
#define DEBUG_U_TUNING_MODE
    case ACCGYR_PRES_MODE_TUNING_Y:	//  ACC Y TUNING mode
      {
	double UI_value = AY_seen_f;
	UI_value -= 0.18;	// offset	just testing...
	double detune = 1.0;

	// debug output showed cents as suitable unit, so i go this way:
	extern double cent;	// double cent = pow(2.0, (1.0 / 1200.0));	// for human readers ;)
	detune = pow(cent, (AY_seen_f * -2.0) + 1.0);
	detune *= detune;
	detune *= detune;
	detune *= detune;
	PULSES.tuning *= detune;

// #if defined DEBUG_U_TUNING_MODE && false
// // #if false
// //	if(monochrome_can_be_used()) {
// //	  MC_setCursor(0,0);
// //	  //	extern void monochrome_print_f(float f, int chiffres);
// //	  monochrome_print_f(AY_seen_f, 6);
// //	  MC_print(F("  "));
// //
// //	  MC_setCursor(0,1);
// //	  monochrome_print_f(accGyro_current_AX_f, 6);
// //	  MC_print(F("  "));
// //	}
// // #endif
//	MENU.out(F("seen_Y "));
//	MENU.out(AY_seen_f, 9);
//	MENU.tab();
//
//	MENU.out(F("UI_v "));
//	MENU.out(UI_value, 9);
//	MENU.tab();
//
//	MENU.out((AY_seen_f * -2) + 1.0, 9);
//	MENU.tab(); MENU.out(detune, 9);
//	MENU.tab(); MENU.out(PULSES.tuning, 9);
//	MENU.ln();
// #endif
      }
      break;				// accGyro_preset==ACCGYR_PRES_MODE_TUNING_Y

    case ACCGYR_PRES_MODE_RAW:
      display_accGyro_raw=true;		// (redundant)	cannot be switched off (debugging only)
      break;

    default:
      ERROR_ln(F("unknown accGyro_preset"));
    } // switch (accGyro_preset)
  } // if(accGyro_new_data && accGyro_mode)
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

    MENU.check_next(',');	// just drop any ','
  }

  show_accGyro_offsets();
}


#define  MPU6050_MODULE_H
#endif
