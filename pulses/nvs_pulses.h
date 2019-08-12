/*
  nvs_pulses.h
*/

#ifndef NVS_PULSES_H

#include <nvs.h>
#include <Preferences.h>
#include "nvs_flash.h"
#include "esp_system.h"

#define DEBUG_NVS	// TODO: unset

// esp_err_t ERROR reporting
#if ! defined ESP_ERR_INFO_DEFINED
  bool /* error */ esp_err_info(esp_err_t status) {
    if(status == ESP_OK) {	// ok
      if(MENU.maybe_display_more(VERBOSITY_LOWEST) || DEBUG_ESP_NOW)
	MENU.outln(F("ok"));
      return false;	// OK
    } else			// not ok
      if(MENU.maybe_display_more(VERBOSITY_LOWEST/* sic! */) || DEBUG_ESP_NOW)	// *do* display that
	MENU.error_ln(esp_err_to_name(status));
    return true;		// ERROR
  }
  #define ESP_ERR_INFO_DEFINED
#endif

String nvs_getString(char * key) {
  String s;

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  s = CONF_nvs.getString(key);

#if defined DEBUG_NVS
  MENU.out(F("nvs_getString() |"));
  MENU.out(s); MENU.outln('|');
#endif

  CONF_nvs.end();
  return s;
}


int nvs_free_entries() {
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int free_entries = CONF_nvs.freeEntries();
  CONF_nvs.end();
  return free_entries;
}


/* ****************************************************************
  as nvs blobs still make problems
  i keep some code from different attempts...
  TODO: TEST&FIX:  REMOVE: unused versions	DADA
*/
void nvs_save_blob_v1(char* key, void* blob, size_t length) {	// 1st version using CONF_nvs.putBytes
// see: https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/Prefs2Struct/Prefs2Struct.ino

  MENU.out(F("nvs_save_blob "));
  MENU.out(key);
  MENU.tab();

  Preferences CONF_nvs;
  if(CONF_nvs.begin("CONFIG", /* readonly is */ false))
    MENU.error_ln(F("CONF_nvs.begin()"));

  size_t did_put_bytes = CONF_nvs.putBytes("CONFIG", blob, length);
  MENU.out(did_put_bytes);
  MENU.outln(F(" bytes saved"));

  CONF_nvs.end();
}


void nvs_read_blob_v2(char* key, void* new_blob, uint8_t size) {	// 2nd version using nvs_getString
  MENU.out(F("nvs_read_blob (2nd)\t"));
  MENU.outln(key);

  Preferences CONF_nvs;
  if(CONF_nvs.begin("CONFIG", /* readonly is */ false))
    MENU.error_ln(F("CONF_nvs.begin()"));
  size_t read_size=0;
  std::string blobString((unsigned long) new_blob, size);
  // DADA TODO: not working yet....
  // DADA want to try SPIFFS before fixing the NVS BLOB stuff...
  // DADA the following line does not compile...
  //  blobString = nvs_getString(key);	// DOES NOT COMPILE ################################

  //  String((unsigned long) new_blob, size): blobString = nvs_getString(key);
  //  String((unsigned long) new_blob, size) blobString = nvs_getString(key);
  //  String blobString((char*) new_blob, &read_size) = nvs_getString(key);

//	#if defined DEBUG_NVS
//	  MENU.out(F("got |"));
//	  MENU.out(blobString);		// DOES NOT COMPILE ################################
//	  MENU.outln('|');
//	#endif

  size_t len = blobString.length();
  MENU.out(len);
  MENU.tab(); MENU.out(read_size);
  MENU.outln(F(" bytes read"));
  char* blob_as_char_p = (char*) new_blob;
  blob_as_char_p = (char*) blobString.c_str();

  CONF_nvs.end();
}


void nvs_read_blob_v1(char* key, void* new_blob, size_t buffer_size) {	// 1st version using CONF_nvs.getBytes
  MENU.out(F("nvs_read_blob_v1\t"));
  MENU.outln(key);

  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  size_t read_len = CONF_nvs.getBytes((const char*) key, new_blob, buffer_size);
  MENU.out(read_len);
  MENU.outln(F(" bytes read"));
  if(read_len > buffer_size) {
    MENU.error_ln(F("buffer too small\n"));
    CONF_nvs.end();
    return;
  }

  size_t read_bytes = CONF_nvs.getBytes(key, new_blob, buffer_size);
  MENU.out(read_bytes);
  MENU.outln(F(" bytes read"));

  CONF_nvs.end();
}

typedef uint32_t nvs_handle_t;	// where is that defined?

bool nvs_read_blob_v3(char* key, void* new_blob, size_t buffer_size) {
  // see: https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c

  //	MENU.out(F("nvs_read_blob_v3\t"));	// *DEBUGGING* DADA TODO: REACTIVATE MENU
  //	MENU.outln(key);

  nvs_handle_t my_handle;
  esp_err_t err;

  err = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return true; // error
  }

  // Read the size of memory space required for blob
  size_t required_size = 0;  // value will default to 0, if not set yet in NVS
  err = nvs_get_blob(my_handle, key, NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    esp_err_info(err);
    nvs_close(my_handle);
    return true; // error
  }
  //	MENU.out(required_size);
  //	MENU.outln(F(" bytes to read"));

  // double check buffer size
  if(required_size > buffer_size) {
    //	MENU.error_ln(F("too much"));
    nvs_close(my_handle);
    return true; // error
  }

//  //	MENU.outln("size ok"); return true;

  // Read previously saved blob if available
  // uint8_t* run_time = (uint8_t*) malloc(required_size);
  if (required_size > 0) {
    err = nvs_get_blob(my_handle, key, new_blob, &required_size);
    if (err != ESP_OK) {
      esp_err_info(err);
      nvs_close(my_handle);
      return true; // error
    }
  }

//	MENU.outln("read ok"); return true;

  // Close
  nvs_close(my_handle);
  return false; // OK
} // nvs_read_blob_v3


void nvs_save_blob_v3(char* key, void* new_blob, size_t buffer_size) {
  // see: https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c

  MENU.out(F("nvs_save_blob_v3\t"));
  MENU.outln(key);

  nvs_handle_t my_handle;
  esp_err_t err;

  err = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return;
  }

  MENU.out(buffer_size);
  MENU.out(F(" bytes\t"));

  // Write blob
  err = nvs_set_blob(my_handle, key, new_blob, buffer_size);
  if (err != ESP_OK) {
    esp_err_info(err);
    return;
  } // else

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return;
  }

  // Close
  nvs_close(my_handle);
  MENU.outln(F("ok"));
} // nvs_save_blob_v3


//	void configure_HARDWARE_from_nvs_v4() {
//	  MENU.outln(F("configure_HARDWARE_from_nvs-v4() using new()"));
//
//	  pulses_hardware_conf_t* HARDWARE_from_nvs;
//	  HARDWARE_from_nvs = new pulses_hardware_conf_t;
//
//	  if(nvs_read_blob_v3("HARDWARE_nvs", &HARDWARE_from_nvs, sizeof(pulses_hardware_conf_t))) {
//	    free(HARDWARE_from_nvs);
//	    return;
//	  }
//	  // blob is loaded now
//
//	  // check version compatibility
//	  if(*HARDWARE_from_nvs.version != HARDWARE_Conf.version) { // DADA *DEBUGGING* CRASH!
//	    //	MENU.error_ln(F("version mismatch"));
//	    free(HARDWARE_from_nvs);
//	    return;
//	  }
//
//	  // MPU6050
//	  if(HARDWARE_from_nvs->mpu6050_addr) {
//	    HARDWARE_Conf.mpu6050_addr = HARDWARE_from_nvs->mpu6050_addr;
//	    for(int i=0; i<6; i++) {
//	      if(HARDWARE_from_nvs->accGyro_offsets[i])
//		HARDWARE_Conf.accGyro_offsets[i] = HARDWARE_from_nvs->accGyro_offsets[i];
//	    }
//	  }
//
//	  // GPIO
//	  if(HARDWARE_from_nvs->gpio_pins_cnt) {
//	    if(HARDWARE_from_nvs->gpio_pins_cnt > 20)	// invalid?
//	      ; //	MENU.error_ln(F("gpio_pins_cnt"));
//	    else {
//	      for(int i=0; i<HARDWARE_from_nvs->gpio_pins_cnt; i++) {
//		if(HARDWARE_from_nvs->gpio_pins[i] != ILLEGAL) // illegal?
//		  HARDWARE_Conf.gpio_pins[i] = HARDWARE_from_nvs->gpio_pins[i];
//		else {
//		  //	MENU.error_ln(F("illegal gpio"));
//		  break;
//		}
//	      }
//	    }
//	  }
//
//	  // DAC
//	  if(HARDWARE_from_nvs->DAC1_pin != ILLEGAL)
//	    HARDWARE_Conf.DAC1_pin = HARDWARE_from_nvs->DAC1_pin;
//	  if(HARDWARE_from_nvs->DAC2_pin != ILLEGAL)
//	    HARDWARE_Conf.DAC2_pin = HARDWARE_from_nvs->DAC2_pin;
//
//	  // trigger
//	  if(HARDWARE_from_nvs->musicbox_trigger_pin != ILLEGAL)
//	    HARDWARE_Conf.musicbox_trigger_pin = HARDWARE_from_nvs->musicbox_trigger_pin;
//
//	  // battery and peripheral power
//	  if(HARDWARE_from_nvs->battery_level_control_pin != ILLEGAL)
//	    HARDWARE_Conf.battery_level_control_pin = HARDWARE_from_nvs->battery_level_control_pin;
//	  if(HARDWARE_from_nvs->peripheral_power_switch_pin != ILLEGAL)
//	    HARDWARE_Conf.peripheral_power_switch_pin = HARDWARE_from_nvs->peripheral_power_switch_pin;
//
//	  // morse
//	  if(HARDWARE_from_nvs->morse_touch_input_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_touch_input_pin = HARDWARE_from_nvs->morse_touch_input_pin;
//	  if(HARDWARE_from_nvs->morse_gpio_input_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_gpio_input_pin = HARDWARE_from_nvs->morse_gpio_input_pin;
//	  if(HARDWARE_from_nvs->morse_output_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_output_pin = HARDWARE_from_nvs->morse_output_pin;
//
//	  // bluetooth
//	  if(HARDWARE_from_nvs->bluetooth_enable_pin != ILLEGAL)
//	    HARDWARE_Conf.bluetooth_enable_pin = HARDWARE_from_nvs->bluetooth_enable_pin;
//
//	  // oled
//	  if(HARDWARE_from_nvs->OLED_type != ILLEGAL)
//	    HARDWARE_Conf.OLED_type = HARDWARE_from_nvs->OLED_type;
//	  if(HARDWARE_from_nvs->oled_reserved != ILLEGAL)
//	    HARDWARE_Conf.oled_reserved = HARDWARE_from_nvs->oled_reserved;
//
//	  // RTC module
//	  if(HARDWARE_from_nvs->rtc_type != rtc_type_off)
//	    HARDWARE_Conf.rtc_type = HARDWARE_from_nvs->rtc_type;
//	  if(HARDWARE_from_nvs->rtc_addr != ILLEGAL)
//	    HARDWARE_Conf.rtc_addr = HARDWARE_from_nvs->rtc_addr;
//
//	  // RGB LED strings
//	  if(HARDWARE_from_nvs->rgb_strings) {
//	    HARDWARE_Conf.rgb_strings = HARDWARE_from_nvs->rgb_strings;
//	    for(int i=0; i < HARDWARE_from_nvs->rgb_strings; i++) {
//	      HARDWARE_Conf.rgb_pin[i] = HARDWARE_from_nvs->rgb_pin[i];
//	      HARDWARE_Conf.rgb_led_cnt[i] = HARDWARE_from_nvs->rgb_led_cnt[i];
//	      HARDWARE_Conf.rgb_led_voltage_type[i] = HARDWARE_from_nvs->rgb_led_voltage_type[i];
//	    }
//	  }
//
//	  // MIDI
//	  if(HARDWARE_from_nvs->MIDI_in_pin != ILLEGAL)
//	    HARDWARE_Conf.MIDI_in_pin = HARDWARE_from_nvs->MIDI_in_pin;
//	  if(HARDWARE_from_nvs->MIDI_out_pin != ILLEGAL)
//	    HARDWARE_Conf.MIDI_out_pin = HARDWARE_from_nvs->MIDI_out_pin;
//
//	  // other
//	  if(HARDWARE_from_nvs->magical_fart_output_pin != ILLEGAL)
//	    HARDWARE_Conf.magical_fart_output_pin = HARDWARE_from_nvs->magical_fart_output_pin;
//	  if(HARDWARE_from_nvs->magical_sense_pin != ILLEGAL)
//	    HARDWARE_Conf.magical_sense_pin = HARDWARE_from_nvs->magical_sense_pin;
//
//	  // reserved
//
//	  free(HARDWARE_from_nvs);
//	} // configure_HARDWARE_from_nvs-v4()


//	void configure_HARDWARE_from_nvs() {
//	  MENU.outln(F("DADA DEBUGGING configure_HARDWARE_from_nvs() *DEACTIVATED*\n")); return;	// TODO: REMOVE:
//	  int v;
//	  pulses_hardware_conf_t* HARDWARE_from_nvs = (pulses_hardware_conf_t*) malloc(sizeof(pulses_hardware_conf_t));
//	  if(HARDWARE_from_nvs == NULL) {
//	    //	MENU.error_ln(F("malloc"));
//	    return;
//	  }
//
//	  if(nvs_read_blob_v3("HARDWARE_nvs", &HARDWARE_from_nvs, sizeof(pulses_hardware_conf_t))) {
//	    free(HARDWARE_from_nvs);
//	    return;
//	  }
//	  // blob is loaded now
//
//	  // check version compatibility
//	  if(HARDWARE_from_nvs->version != HARDWARE_Conf.version) { // DADA *DEBUGGING* CRASH!
//	    //	MENU.error_ln(F("version mismatch"));
//	    free(HARDWARE_from_nvs);
//	    return;
//	  }
//
//	  // MPU6050
//	  if(HARDWARE_from_nvs->mpu6050_addr) {
//	    HARDWARE_Conf.mpu6050_addr = HARDWARE_from_nvs->mpu6050_addr;
//	    for(int i=0; i<6; i++) {
//	      if(HARDWARE_from_nvs->accGyro_offsets[i])
//		HARDWARE_Conf.accGyro_offsets[i] = HARDWARE_from_nvs->accGyro_offsets[i];
//	    }
//	  }
//
//	  // GPIO
//	  if(HARDWARE_from_nvs->gpio_pins_cnt) {
//	    if(HARDWARE_from_nvs->gpio_pins_cnt > 20)	// invalid?
//	      ; //	MENU.error_ln(F("gpio_pins_cnt"));
//	    else {
//	      for(int i=0; i<HARDWARE_from_nvs->gpio_pins_cnt; i++) {
//		if(HARDWARE_from_nvs->gpio_pins[i] != ILLEGAL) // illegal?
//		  HARDWARE_Conf.gpio_pins[i] = HARDWARE_from_nvs->gpio_pins[i];
//		else {
//		  //	MENU.error_ln(F("illegal gpio"));
//		  break;
//		}
//	      }
//	    }
//	  }
//
//	  // DAC
//	  if(HARDWARE_from_nvs->DAC1_pin != ILLEGAL)
//	    HARDWARE_Conf.DAC1_pin = HARDWARE_from_nvs->DAC1_pin;
//	  if(HARDWARE_from_nvs->DAC2_pin != ILLEGAL)
//	    HARDWARE_Conf.DAC2_pin = HARDWARE_from_nvs->DAC2_pin;
//
//	  // trigger
//	  if(HARDWARE_from_nvs->musicbox_trigger_pin != ILLEGAL)
//	    HARDWARE_Conf.musicbox_trigger_pin = HARDWARE_from_nvs->musicbox_trigger_pin;
//
//	  // battery and peripheral power
//	  if(HARDWARE_from_nvs->battery_level_control_pin != ILLEGAL)
//	    HARDWARE_Conf.battery_level_control_pin = HARDWARE_from_nvs->battery_level_control_pin;
//	  if(HARDWARE_from_nvs->peripheral_power_switch_pin != ILLEGAL)
//	    HARDWARE_Conf.peripheral_power_switch_pin = HARDWARE_from_nvs->peripheral_power_switch_pin;
//
//	  // morse
//	  if(HARDWARE_from_nvs->morse_touch_input_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_touch_input_pin = HARDWARE_from_nvs->morse_touch_input_pin;
//	  if(HARDWARE_from_nvs->morse_gpio_input_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_gpio_input_pin = HARDWARE_from_nvs->morse_gpio_input_pin;
//	  if(HARDWARE_from_nvs->morse_output_pin != ILLEGAL)
//	    HARDWARE_Conf.morse_output_pin = HARDWARE_from_nvs->morse_output_pin;
//
//	  // bluetooth
//	  if(HARDWARE_from_nvs->bluetooth_enable_pin != ILLEGAL)
//	    HARDWARE_Conf.bluetooth_enable_pin = HARDWARE_from_nvs->bluetooth_enable_pin;
//
//	  // oled
//	  if(HARDWARE_from_nvs->OLED_type != ILLEGAL)
//	    HARDWARE_Conf.OLED_type = HARDWARE_from_nvs->OLED_type;
//	  if(HARDWARE_from_nvs->oled_reserved != ILLEGAL)
//	    HARDWARE_Conf.oled_reserved = HARDWARE_from_nvs->oled_reserved;
//
//	  // RTC module
//	  if(HARDWARE_from_nvs->rtc_type != rtc_type_off)
//	    HARDWARE_Conf.rtc_type = HARDWARE_from_nvs->rtc_type;
//	  if(HARDWARE_from_nvs->rtc_addr != ILLEGAL)
//	    HARDWARE_Conf.rtc_addr = HARDWARE_from_nvs->rtc_addr;
//
//	  // RGB LED strings
//	  if(HARDWARE_from_nvs->rgb_strings) {
//	    HARDWARE_Conf.rgb_strings = HARDWARE_from_nvs->rgb_strings;
//	    for(int i=0; i < HARDWARE_from_nvs->rgb_strings; i++) {
//	      HARDWARE_Conf.rgb_pin[i] = HARDWARE_from_nvs->rgb_pin[i];
//	      HARDWARE_Conf.rgb_led_cnt[i] = HARDWARE_from_nvs->rgb_led_cnt[i];
//	      HARDWARE_Conf.rgb_led_voltage_type[i] = HARDWARE_from_nvs->rgb_led_voltage_type[i];
//	    }
//	  }
//
//	  // MIDI
//	  if(HARDWARE_from_nvs->MIDI_in_pin != ILLEGAL)
//	    HARDWARE_Conf.MIDI_in_pin = HARDWARE_from_nvs->MIDI_in_pin;
//	  if(HARDWARE_from_nvs->MIDI_out_pin != ILLEGAL)
//	    HARDWARE_Conf.MIDI_out_pin = HARDWARE_from_nvs->MIDI_out_pin;
//
//	  // other
//	  if(HARDWARE_from_nvs->magical_fart_output_pin != ILLEGAL)
//	    HARDWARE_Conf.magical_fart_output_pin = HARDWARE_from_nvs->magical_fart_output_pin;
//	  if(HARDWARE_from_nvs->magical_sense_pin != ILLEGAL)
//	    HARDWARE_Conf.magical_sense_pin = HARDWARE_from_nvs->magical_sense_pin;
//
//	  // reserved
//
//	  free(HARDWARE_from_nvs);
//	} // configure_HARDWARE_from_nvs()


void configure_HARDWARE_from_nvs_v3() {
  int v;
  pulses_hardware_conf_t HARDWARE_from_nvs;
  MENU.outln(F("configure_HARDWARE_from_nvs_v3()\t"));

  if(nvs_read_blob_v3("HARDWARE_nvs", &HARDWARE_from_nvs, sizeof(pulses_hardware_conf_t))) {
    return;
  }
  // blob is loaded now

  // check version compatibility
  if(HARDWARE_from_nvs.version != HARDWARE_Conf.version) {
    //	MENU.error_ln(F("version mismatch"));
    return;
  }

  // MPU6050
  if(HARDWARE_from_nvs.mpu6050_addr) {
    HARDWARE_Conf.mpu6050_addr = HARDWARE_from_nvs.mpu6050_addr;
    for(int i=0; i<6; i++) {
      if(HARDWARE_from_nvs.accGyro_offsets[i])
	HARDWARE_Conf.accGyro_offsets[i] = HARDWARE_from_nvs.accGyro_offsets[i];
    }
  }

  // GPIO
  if(HARDWARE_from_nvs.gpio_pins_cnt) {
    if(HARDWARE_from_nvs.gpio_pins_cnt > 20)	// invalid?
      ; //	MENU.error_ln(F("gpio_pins_cnt"));
    else {
      for(int i=0; i<HARDWARE_from_nvs.gpio_pins_cnt; i++) {
	if(HARDWARE_from_nvs.gpio_pins[i] != ILLEGAL) // illegal?
	  HARDWARE_Conf.gpio_pins[i] = HARDWARE_from_nvs.gpio_pins[i];
	else {
	  //	MENU.error_ln(F("illegal gpio"));
	  break;
	}
      }
    }
  }

  // DAC
  if(HARDWARE_from_nvs.DAC1_pin != ILLEGAL)
    HARDWARE_Conf.DAC1_pin = HARDWARE_from_nvs.DAC1_pin;
  if(HARDWARE_from_nvs.DAC2_pin != ILLEGAL)
    HARDWARE_Conf.DAC2_pin = HARDWARE_from_nvs.DAC2_pin;

  // trigger
  if(HARDWARE_from_nvs.musicbox_trigger_pin != ILLEGAL)
    HARDWARE_Conf.musicbox_trigger_pin = HARDWARE_from_nvs.musicbox_trigger_pin;

  // battery and peripheral power
  if(HARDWARE_from_nvs.battery_level_control_pin != ILLEGAL)
    HARDWARE_Conf.battery_level_control_pin = HARDWARE_from_nvs.battery_level_control_pin;
  if(HARDWARE_from_nvs.peripheral_power_switch_pin != ILLEGAL)
    HARDWARE_Conf.peripheral_power_switch_pin = HARDWARE_from_nvs.peripheral_power_switch_pin;

  // morse
  if(HARDWARE_from_nvs.morse_touch_input_pin != ILLEGAL)
    HARDWARE_Conf.morse_touch_input_pin = HARDWARE_from_nvs.morse_touch_input_pin;
  if(HARDWARE_from_nvs.morse_gpio_input_pin != ILLEGAL)
    HARDWARE_Conf.morse_gpio_input_pin = HARDWARE_from_nvs.morse_gpio_input_pin;
  if(HARDWARE_from_nvs.morse_output_pin != ILLEGAL)
    HARDWARE_Conf.morse_output_pin = HARDWARE_from_nvs.morse_output_pin;

  // bluetooth
  if(HARDWARE_from_nvs.bluetooth_enable_pin != ILLEGAL)
    HARDWARE_Conf.bluetooth_enable_pin = HARDWARE_from_nvs.bluetooth_enable_pin;

  // oled
  if(HARDWARE_from_nvs.OLED_type != ILLEGAL)
    HARDWARE_Conf.OLED_type = HARDWARE_from_nvs.OLED_type;
  if(HARDWARE_from_nvs.oled_reserved != ILLEGAL)
    HARDWARE_Conf.oled_reserved = HARDWARE_from_nvs.oled_reserved;

  // RTC module
  if(HARDWARE_from_nvs.rtc_type != rtc_type_off)
    HARDWARE_Conf.rtc_type = HARDWARE_from_nvs.rtc_type;
  if(HARDWARE_from_nvs.rtc_addr != ILLEGAL)
    HARDWARE_Conf.rtc_addr = HARDWARE_from_nvs.rtc_addr;

  // RGB LED strings
  if(HARDWARE_from_nvs.rgb_strings) {
    HARDWARE_Conf.rgb_strings = HARDWARE_from_nvs.rgb_strings;
    for(int i=0; i < HARDWARE_from_nvs.rgb_strings; i++) {
      HARDWARE_Conf.rgb_pin[i] = HARDWARE_from_nvs.rgb_pin[i];
      HARDWARE_Conf.rgb_led_cnt[i] = HARDWARE_from_nvs.rgb_led_cnt[i];
      HARDWARE_Conf.rgb_led_voltage_type[i] = HARDWARE_from_nvs.rgb_led_voltage_type[i];
    }
  }

  // MIDI
  if(HARDWARE_from_nvs.MIDI_in_pin != ILLEGAL)
    HARDWARE_Conf.MIDI_in_pin = HARDWARE_from_nvs.MIDI_in_pin;
  if(HARDWARE_from_nvs.MIDI_out_pin != ILLEGAL)
    HARDWARE_Conf.MIDI_out_pin = HARDWARE_from_nvs.MIDI_out_pin;

  // other
  if(HARDWARE_from_nvs.magical_fart_output_pin != ILLEGAL)
    HARDWARE_Conf.magical_fart_output_pin = HARDWARE_from_nvs.magical_fart_output_pin;
  if(HARDWARE_from_nvs.magical_sense_pin != ILLEGAL)
    HARDWARE_Conf.magical_sense_pin = HARDWARE_from_nvs.magical_sense_pin;

  // reserved
} // configure_HARDWARE_from_nvs_v3()


void nvs_clear_all_keys() {
  MENU.out(F(">>> CLEARED ALL KEYS in CONF_nvs <<<  "));
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int was_free = CONF_nvs.freeEntries();
  CONF_nvs.clear();

  MENU.out(CONF_nvs.freeEntries() - was_free);
  MENU.out(F("\tfree entries now "));
  MENU.outln(CONF_nvs.freeEntries());
  CONF_nvs.end();
  MENU.outln("DADA");
}


#define NVS_PULSES_H
#endif
