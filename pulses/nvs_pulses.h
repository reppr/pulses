/*
  nvs_pulses.h
*/

#ifndef NVS_PULSES_H

#include <nvs.h>
#include <Preferences.h>
#include "nvs_flash.h"
#include "esp_system.h"

//#include "nvs_encryption_key.h"	// not used yet

#define DEBUG_NVS

#include "pulses_esp_err.h"		// esp_err_t ERROR reporting

int nvs_free_entries() {
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int free_entries = CONF_nvs.freeEntries();
  CONF_nvs.end();
  MENU.out(F("nvs free entries:\t"));
  MENU.outln(free_entries);
  return free_entries;
}

typedef uint32_t nvs_handle_t;	// TODO: *where* is that defined?	################

size_t /*size*/ nvs_test_key(const char* key) {	// no trailing newline
  MENU.out(F("nvs key  "));
  MENU.out(key);
  MENU.tab();

  nvs_handle_t my_handle;
  esp_err_t err;
  size_t size = 0;  // default 0, if not set yet in NVS

  err = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return 0; // error, size==0
  } // nvs_open error?

  // read size of the blob:
  err = nvs_get_blob(my_handle, key, NULL, &size);
  if (err == ESP_OK) {
    MENU.out(size);
    MENU.out(F(" bytes"));
  } else {
    if (err == ESP_ERR_NVS_NOT_FOUND)
      MENU.out(F("(none)"));
    else
      esp_err_info(err);
  }

  nvs_close(my_handle);
  return size;
} // nvs_test_key()


void nvs_delete_key(const char* key) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (err == ESP_OK)
    err = nvs_erase_key(my_handle, key);
  if (err == ESP_OK)
    nvs_commit(my_handle);

  if (err != ESP_OK)
    esp_err_info(err);
  nvs_close(my_handle);
} // nvs_delete_key()


bool /*error*/ nvs_read_blob(const char* key, void* new_blob, size_t buffer_size) {
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
  MENU.outln(F(" bytes reading"));

  // double check buffer size
  if(required_size > buffer_size) {
    MENU.out(required_size);
    MENU.space();
    ERROR_ln(F("too much TODO: DEBUG:"));	// TODO: DEBUG: i have seen this...
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


bool /*error*/ nvs_save_blob(const char* key, void* new_blob, size_t buffer_size) {
  // see: https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c

  MENU.out(F("nvs_save_blob\t"));
  MENU.outln(key);

  nvs_handle_t my_handle;
  esp_err_t err;

  err = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return true; // error
  }

  MENU.out(buffer_size);
  MENU.out(F(" bytes\t"));

  // Write blob
  err = nvs_set_blob(my_handle, key, new_blob, buffer_size);
  if (err != ESP_OK) {
    esp_err_info(err);
    return true; // error
  } // else

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) {
    esp_err_info(err);
    return true; // error
  }

  // Close
  nvs_close(my_handle);
  MENU.outln(F("ok"));
  return false; // OK
} // nvs_save_blob


extern bool rgb_strings_available;
extern bool rgb_strings_active;
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
    MENU.outln(F("HARDWARE_nvs does not exist"));
    return;						// no
  }

  // check version compatibility
  if(HARDWARE_from_nvs.version != HARDWARE.version) {
    ERROR_ln(F("version mismatch"));
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
      ERROR_ln(F("gpio_pins_cnt"));
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
    HARDWARE.monochrome_type = x;
    // TODO: factor that out
    MENU.out(F("monochrome type\t"));
    switch(x) {
    case monochrome_type_off:
      MENU.outln('-');
      break;
    case monochrome_type_heltec:
      MENU.outln(F("heltec"));
      break;
    case monochrome_type_LiPO:
      MENU.outln(F("LiPO"));
      break;
    default:
      MENU.error_ln(F("monochrome type unknown"));	// TODO: ERROR_ln() does not work here???
      //ERROR_ln(F("monochrome type unknown"));
    }
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

  a = HARDWARE_from_nvs.RTC_addr;	// see: #define RTC_I2C_ADDRESS 0x68
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
    rgb_strings_active = true;
    MENU.out(F("RGB LED strings available "));
    MENU.outln(n);

    for(int i=0; i < n; i++) {
      MENU.space(2);
      MENU.out(i);

      MENU.out(F("  pin "));
      x = HARDWARE.rgb_pin[i] = HARDWARE_from_nvs.rgb_pin[i];
      MENU.out(x);

      MENU.out(F("\tcnt "));
      x = HARDWARE.rgb_pixel_cnt[i] = HARDWARE_from_nvs.rgb_pixel_cnt[i];
      MENU.out(x);

      MENU.out(F("\ttype "));
      x = HARDWARE.rgb_led_voltage_type[i] = HARDWARE_from_nvs.rgb_led_voltage_type[i];
      MENU.out(x);
      MENU.out('V');

      MENU.out(F("\tpattern start "));
      x = HARDWARE.rgb_pattern0[i] = HARDWARE_from_nvs.rgb_pattern0[i];
      MENU.outln(x);
    }

    // currently *all* strings share software conf (as setup for string[0])
    if((x = HARDWARE_from_nvs.rgb_led_voltage_type[0]) != ILLEGAL8) {
      extern int selected_rgb_LED_string;
      extern void set_rgb_string_voltage_type(int, int);
      MENU.out(F("=>"));
      set_rgb_string_voltage_type(x, selected_rgb_LED_string);
    }
  } else {	// no rgb led strings
    if(rgb_strings_available || rgb_strings_active) {
      rgb_strings_available = false;
      rgb_strings_active = false;
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


void configure_IDENTITY_from_nvs() {
  peer_ID_t* IDENTITY_from_nvs_p = (peer_ID_t*) malloc(sizeof(peer_ID_t));
  if(IDENTITY_from_nvs_p==NULL) {
    MENU.malloc_error();
    return;
  }

  IDENTITY_from_nvs_p->version = ILLEGAL8;
  MENU.outln(F("\nconfigure_IDENTITY_from_nvs()\t"));
  if(nvs_read_blob("IDENTITY_nvs", IDENTITY_from_nvs_p, sizeof(peer_ID_t))) {
    goto free_identity_nvs;
  }

    // blob is loaded now, *if* exists, check that
  if(IDENTITY_from_nvs_p->version == ILLEGAL8) {		// did key exist?
    MENU.outln(F("IDENTITY_nvs does not exist"));
    goto free_identity_nvs;
  }

  // check version compatibility
  if(IDENTITY_from_nvs_p->version != my_IDENTITY.version) {
    ERROR_ln(F("version mismatch"));
    goto free_identity_nvs;
  }

  if(IDENTITY_from_nvs_p->preName != "") {
    for(int b=0; b<16; b++)
      my_IDENTITY.preName[b] = IDENTITY_from_nvs_p->preName[b];

    MENU.out(F("preName\t\t"));
    MENU.out_IstrI(my_IDENTITY.preName);
    MENU.ln();
  }

  // mac

  // time slice
  if(IDENTITY_from_nvs_p->esp_now_time_slice != ILLEGAL8) {
    my_IDENTITY.esp_now_time_slice = IDENTITY_from_nvs_p->esp_now_time_slice;
    MENU.out(F("time slice\t"));
    MENU.outln(my_IDENTITY.esp_now_time_slice);
  }

 free_identity_nvs:
  free(IDENTITY_from_nvs_p);
} // configure_IDENTITY_from_nvs()


bool /*configured?*/ configure_BATTERY_from_nvs() {	// can even be used *without* USE_BATTERY_LEVEL_CONTROL
  MENU.outln(F("\nconfigure_BATTERY_from_nvs()\t"));
  size_t size = nvs_test_key("BATTERY_nvs");
  if(size==0) {		// no BATTERY configuration in nvs
#if defined USE_BATTERY_LEVEL_CONTROL
    MENU.outln(F("\nno BATTERY nvs data, using:"));
    battery_conf_UI_display(false);	// just show configuration, no data
#endif
    return false;	// no data
  } // else

  // there *is* BATTERY data in nvs
#if defined USE_BATTERY_LEVEL_CONTROL
  MENU.out(F("read BATTERY_nvs\t"));
  if(nvs_read_blob("BATTERY_nvs", &BATTERY, sizeof(battery_levels_conf_t))) {
    MENU.ln();
    return false;	// ERROR
  }
  battery_conf_UI_display(false);	// just show configuration, no data
  return true;	// configuration OK

#else		// no code, but data...
  MENU.error_ln(F("BATTERY data ignored"));
  return false;	// no code, but data...
#endif
} // configure_BATTERY_from_nvs()


char* nvs_AUTOSTART_kb_macro=NULL;	// will be interpreted instead of AUTOSTART if it exists
					// 'A' will still do "normal" autostart
void set_nvs_autostart_kb_macro(char* macro) {
  if(nvs_AUTOSTART_kb_macro)
    free(nvs_AUTOSTART_kb_macro);	// free previous allocation
  nvs_AUTOSTART_kb_macro=NULL;

  if(macro && strlen(macro)) {
    nvs_AUTOSTART_kb_macro = (char*) malloc(strlen(macro) + 1);
    if(nvs_AUTOSTART_kb_macro) {
      strcpy(nvs_AUTOSTART_kb_macro, macro);
      MENU.out(F("set nvs_AUTOSTART \""));
      MENU.out(nvs_AUTOSTART_kb_macro);
      MENU.out('"');
    } else
      MENU.malloc_error();
  }
} // set_nvs_autostart_kb_macro()

void read_nvs_autostart() {
  size_t size = nvs_test_key("nvs_AUTOSTART");
  MENU.ln();
  if(size == 0) {
    set_nvs_autostart_kb_macro(NULL);
    MENU.ln();
    return;
  } // else

//  MENU.out(F(" nvs AUTOSTART kb macro\tsize "));
//  MENU.out(size);

  char buffer[size];
  if(! nvs_read_blob("nvs_AUTOSTART", buffer, size))
    set_nvs_autostart_kb_macro(buffer);
  MENU.ln();
} // read_nvs_autostart()

#if true // test OLD & new version  nvs_clear_all_keys()
void nvs_clear_all_keys() {	// TODO: runs, but then crashes...
  MENU.out(F(">>> CLEARED ALL KEYS in CONF_nvs <<<  "));
  Preferences CONF_nvs;
  CONF_nvs.begin("CONFIG", /* readonly is */ false);
  int was_free = CONF_nvs.freeEntries();
  CONF_nvs.clear();

  MENU.out(CONF_nvs.freeEntries() - was_free);
  MENU.out(F("\tfree entries now "));
  MENU.outln(CONF_nvs.freeEntries());

  CONF_nvs.end();
} // nvs_clear_all_keys()

#else // test old & new version  nvs_clear_all_keys()
void nvs_clear_all_keys() {	// TODO: tried to write new, still crashes pulses...
  nvs_handle_t my_handle;
  esp_err_t status = nvs_open("CONFIG", NVS_READWRITE, &my_handle);
  if (status != ESP_OK) {
    esp_err_info(status);
    return;
  } // nvs_open error?

  nvs_erase_all(my_handle);
  nvs_commit(my_handle);
  nvs_close(my_handle);
} // nvs_erase_all()
#endif // test old & new version  nvs_clear_all_keys()


void nvs_pulses_setup() {
  configure_HARDWARE_from_nvs();
  configure_IDENTITY_from_nvs();
  configure_BATTERY_from_nvs();
  MENU.ln();
  read_nvs_autostart();
} // nvs_pulses_setup()

#define NVS_PULSES_H
#endif
