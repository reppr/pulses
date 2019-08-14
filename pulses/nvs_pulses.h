/*
  nvs_pulses.h
*/

#ifndef NVS_PULSES_H

#include <nvs.h>
#include <Preferences.h>
#include "nvs_flash.h"
#include "esp_system.h"

//#define DEBUG_NVS	// TODO: unset

// why can't i do this?
// Preferences CONF_nvs;
// CONF_nvs.begin("CONFIG", /* readonly is */ false);

#define RGB_STRINGS_MAX		8
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

typedef uint32_t nvs_handle_t;	// TODO: *where* is that defined?	################

bool nvs_read_blob(char* key, void* new_blob, size_t buffer_size) {
  // see: https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c

  MENU.out(F("nvs_read_blob\t"));
  MENU.out(key);
  MENU.tab();

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
  MENU.out(required_size);
  MENU.outln(F(" bytes to read"));

  // double check buffer size
  if(required_size > buffer_size) {
    //	MENU.error_ln(F("too much"));
    nvs_close(my_handle);
    return true; // error
  }

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

  // Close
  nvs_close(my_handle);
  return false; // OK
} // nvs_read_blob


void nvs_save_blob(char* key, void* new_blob, size_t buffer_size) {
  // see: https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c

  MENU.out(F("nvs_save_blob\t"));
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
} // nvs_save_blob


extern bool rgb_strings_available;
void configure_HARDWARE_from_nvs() {
  int v;
  pulses_hardware_conf_t HARDWARE_from_nvs;
  HARDWARE_from_nvs.version = ILLEGAL8;	// see below
  MENU.outln(F("configure_HARDWARE_from_nvs()\t"));

  if(nvs_read_blob("HARDWARE_nvs", &HARDWARE_from_nvs, sizeof(pulses_hardware_conf_t))) {
    return;
  }
  // blob is loaded now, *if* exists, check that
  if(HARDWARE_from_nvs.version == ILLEGAL8) {		// did key exist?
    MENU.outln(F("HARDWARE_nvs does not exist\n"));
    return;						// no
  }

  // check version compatibility
  if(HARDWARE_from_nvs.version != HARDWARE.version) {
    MENU.error_ln(F("version mismatch"));
    return;
  }

  // MPU6050
  if(HARDWARE_from_nvs.mpu6050_addr) {
    HARDWARE.mpu6050_addr = HARDWARE_from_nvs.mpu6050_addr;

    bool AccGyr_offset_set=false;
    for(int i=0; i<6; i++) {
      if(HARDWARE_from_nvs.accGyro_offsets[i])
	AccGyr_offset_set = true;
    }

    if(AccGyr_offset_set) {
      MENU.out(F("AccGyr offsets\t{"));
      for(int o, i=0; i<6; i++) {
	o = HARDWARE.accGyro_offsets[i] = HARDWARE_from_nvs.accGyro_offsets[i];
	MENU.out(o);
	MENU.out(',');
	MENU.space();
      }
      MENU.outln('}');
    }
  }

  // GPIO
  if(HARDWARE_from_nvs.gpio_pins_cnt) {
    MENU.out(F("GPIO clicks\t"));
    MENU.outln(HARDWARE_from_nvs.gpio_pins_cnt);

    if(HARDWARE_from_nvs.gpio_pins_cnt > 20)	// invalid?
      MENU.error_ln(F("gpio_pins_cnt"));
    else {
      MENU.out(F("gpio_pins\t{"));
      uint8_t p;
      for(int i=0; i<HARDWARE_from_nvs.gpio_pins_cnt; i++) {
	p = HARDWARE_from_nvs.gpio_pins[i];
	if(p != ILLEGAL8) {	// illegal?
	  HARDWARE.gpio_pins[i] = p;
	  MENU.out(p);
	  MENU.out(',');
	  MENU.space();
	} else {
	  MENU.out(F("ILL, "));
	}
      } // for( pins )
      MENU.outln('}');
    }
  }

  uint8_t pin, x, a, n;

  // DAC
  pin = HARDWARE_from_nvs.DAC1_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("DAC1_pin\t"));
    MENU.outln(pin);
    HARDWARE.DAC1_pin = pin;
  }

  pin = HARDWARE_from_nvs.DAC2_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("DAC2_pin\t"));
    MENU.outln(pin);
    HARDWARE.DAC2_pin = pin;
  }

  // trigger
  pin = HARDWARE_from_nvs.musicbox_trigger_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("trigger pin\t"));
    MENU.outln(pin);
    HARDWARE.musicbox_trigger_pin = pin;
  }

  // battery and peripheral power
  pin = HARDWARE_from_nvs.battery_level_control_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("Battery level pin "));
    MENU.outln(pin);
    HARDWARE.battery_level_control_pin = pin;
  }

  pin = HARDWARE_from_nvs.periph_power_switch_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("periph pwr PIN\t"));
    MENU.outln(pin);
    HARDWARE.periph_power_switch_pin = pin;
  }

  // morse
  pin = HARDWARE_from_nvs.morse_touch_input_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("morse touch pin\t"));
    MENU.outln(pin);
    HARDWARE.morse_touch_input_pin = pin;
  }

  pin = HARDWARE_from_nvs.morse_gpio_input_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("morse gpio pin\t"));
    MENU.outln(pin);
    HARDWARE.morse_gpio_input_pin = pin;
  }

  pin = HARDWARE_from_nvs.morse_output_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("morse out pin\t"));
    MENU.outln(pin);
    HARDWARE.morse_output_pin = pin;
  }

  // bluetooth
  pin = HARDWARE_from_nvs.bluetooth_enable_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("BT enable pin\t"));
    MENU.outln(pin);
    HARDWARE.bluetooth_enable_pin = pin;
  }

  // monochrome
  x = HARDWARE_from_nvs.monochrome_type;
  if(x != monochrome_type_off) {
    MENU.out(F("monochrome type\t"));
    MENU.outln(x);
    HARDWARE.monochrome_type = x;
  }

  x = HARDWARE_from_nvs.monochrome_reserved;
  if(x) {
    MENU.out(F("monochrome reserved\t"));
    MENU.outln(x);
    HARDWARE.monochrome_reserved = x;
  }

  // RTC module
  x = HARDWARE_from_nvs.RTC_type;
  if(x != RTC_type_off) {
    MENU.out(F("RTC_type\t"));
    MENU.outln(x);
    HARDWARE.RTC_type = x;
  }

  a = HARDWARE_from_nvs.RTC_addr;
  if(a) {
    MENU.out(F("RTC_addr\t\t"));
    MENU.outln(a);
    HARDWARE.RTC_addr = a;
  }

  // RGB LED strings
  n = HARDWARE_from_nvs.rgb_strings;
  if(n) {
    HARDWARE.rgb_strings = n;
    rgb_strings_available = true;

    MENU.out(F("RGB LED strings available "));
    MENU.outln(n);

    for(int i=0; i < n; i++) {
      MENU.space(2);
      MENU.out(i);

      MENU.out(F("  pin "));
      x = HARDWARE.rgb_pin[i] = HARDWARE_from_nvs.rgb_pin[i];
      MENU.out(x);

      MENU.out(F("\tcnt "));
      x = HARDWARE.rgb_led_cnt[i] = HARDWARE_from_nvs.rgb_led_cnt[i];
      MENU.out(x);

      MENU.out(F("\ttype "));
      x = HARDWARE.rgb_led_voltage_type[i] = HARDWARE_from_nvs.rgb_led_voltage_type[i];
      MENU.out(x);
      MENU.outln('V');
    }

    // currently *all* strings share software conf (as setup for string[0])
    if((x = HARDWARE_from_nvs.rgb_led_voltage_type[0]) != ILLEGAL8) {
      extern int selected_rgb_LED_string;
      extern void set_rgb_string_voltage_type(int, int);
      MENU.out(F("=>"));
      set_rgb_string_voltage_type(x, selected_rgb_LED_string);
    }
  } else {	// no rgb led strings
    if(rgb_strings_available) {
      rgb_strings_available = false;
      MENU.outln(F("*switched RGB LED STRINGS OFF*"));
    }
  }

  // MIDI
  pin = HARDWARE_from_nvs.MIDI_in_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("MIDI_in_pin\t"));
    MENU.outln(pin);
    HARDWARE.MIDI_in_pin = pin;
  }

  pin = HARDWARE_from_nvs.MIDI_out_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("MIDI_out_pin\t"));
    MENU.outln(pin);
    HARDWARE.MIDI_out_pin = pin;
  }

  // other
  pin = HARDWARE_from_nvs.magical_fart_output_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("magical fart pin "));
    MENU.outln(pin);
    HARDWARE.magical_fart_output_pin = pin;
  }

  pin = HARDWARE_from_nvs.magical_sense_pin;
  if(pin != ILLEGAL8) {
    MENU.out(F("magical sense pin "));
    MENU.outln(pin);
    HARDWARE.magical_sense_pin = pin;
  }

  // reserved
} // configure_HARDWARE_from_nvs()


void nvs_show_HW_both() {
  pulses_hardware_conf_t hardware_from_nvs;
  hardware_from_nvs.version = ILLEGAL8;	// see below
  nvs_read_blob("HARDWARE_nvs", &hardware_from_nvs, sizeof(pulses_hardware_conf_t));

  if(hardware_from_nvs.version == ILLEGAL8)
    MENU.outln(F("no data in nvs\n"));
  else {
    MENU.outln(F("HARDWARE configuration from nvs:"));
    extern void show_hardware_conf(pulses_hardware_conf_t*);
    show_hardware_conf(&hardware_from_nvs);
  }

  extern void show_current_hardware_conf();
  show_current_hardware_conf();
}


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
}


#define NVS_PULSES_H
#endif
