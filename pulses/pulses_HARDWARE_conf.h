/*

  pulses_HARDWARE_conf.h

*/

/* ********************* HARDWARE *************************** */
#if ! defined PULSES_HARDWARE_CONF_H

enum monochrome_type {
  monochrome_type_off=0,
  monochrome_type_heltec,	// chip is probably SSD1309
  monochrome_type_LiPO,		// ditto?
  monochrome_type_LILYGO_T5,	// 2.13" eInk display
  monochrome_type_unknown,
};

enum RTC_types {
  RTC_type_off=0,
  RTC_type_DS1307,
  RTC_type_DS3231,
  RTC_type_unknown,
};

#define RGB_STRINGS_MAX		1	// 8 "possible"

// some HARDWARE must be known early	*can be managed by nvs*
typedef struct pulses_hardware_conf_t {
  // MPU6050
  int16_t accGyro_offsets[6] = {0};	// aX, aY, aZ, gX, gY, gZ offsets
  uint8_t mpu6050_addr=0;		// flag and i2c addr

  // gpio
  uint8_t gpio_pins_cnt=0;		// used GPIO click pins
  // gpio_pin_t gpio_pins[20]={ILLEGAL8};					// %4
  gpio_pin_t gpio_pins[20]={(gpio_pin_t) 255};					// %4

  // dac
  // uint8_t dac_channel_cnt=0;	// TODO:
  uint8_t DAC1_pin=ILLEGAL8;	// !flag ILLEGAL8 or 25	// ILLEGAL8==!flag or pin  // %4
  uint8_t DAC2_pin=ILLEGAL8;	// !flag ILLEGAL8 or 26	// ILLEGAL8==!flag or pin

  // trigger
  uint8_t musicbox_trigger_pin=ILLEGAL8;	// maybe include MUSICBOX_TRIGGER_BLOCK_SECONDS?

  // battery and peripheral power
  uint8_t battery_level_control_pin=ILLEGAL8;				// %4
  uint8_t periph_power_switch_pin=ILLEGAL8;

  // morse
  uint8_t morse_touch_input_pin=ILLEGAL8;	// TODO: store touch interrupt level
  uint8_t morse_gpio_input_pin=ILLEGAL8;				// %4
  uint8_t morse_output_pin=ILLEGAL8;

  // bluetooth
  uint8_t bluetooth_enable_pin=ILLEGAL8;	// 35

  // monochrome display
  uint8_t monochrome_type = monochrome_type_off;	// flag and monochrome_type
  uint8_t monochrome_reserved=0;					// %4
  /*
  uint8_t display_hw_rotation=ILLEGAL8;	// TODO: implement

  uint8_t display_hw_pin0=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 CS 5==SS	// TODO: implement
  uint8_t display_hw_pin1=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 DC 17
  uint8_t display_hw_pin2=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 RST 16
  uint8_t display_hw_pin3=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 BUSY 4	// %4
  uint8_t display_hw_pin4=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 MOSI 23
  uint8_t display_hw_pin5=ILLEGAL8;	// OLED		TTGO T5_V2.3_2.13 CLK 18
  uint8_t display_hw_colours=ILLEGAL8;	// OLED   2	TTGO T5_V2.3_2.13 2
  uint16_t display_hw_x_pix=0;								// %4
  uint16_t display_hw_y_pix=0;
  */

  // RTC
  uint8_t RTC_type = RTC_type_off;	// flag and RTC_type
  uint8_t RTC_addr=0;			// *NOT USED* see: RTC_I2C_ADDRESS 0x68    DS3231 and DS1307 use the same address
					// use RTC_I2C_ADDRESS instead of HARDWARE.RTC_addr

  // RGB LED strings
  uint8_t rgb_strings=0;		// flag and rgb led string cnt

  #if defined RGB_LED_STRIP_DATA_PIN
    uint8_t rgb_pin[RGB_STRINGS_MAX]={RGB_LED_STRIP_DATA_PIN};		// %4	TODO: FIX ALIGNEMENT
  #else
    uint8_t rgb_pin[RGB_STRINGS_MAX]={255};				// %4	TODO: FIX ALIGNEMENT
  #endif

  uint8_t rgb_pixel_cnt[RGB_STRINGS_MAX]={0};				// %4
  uint8_t rgb_led_voltage_type[RGB_STRINGS_MAX]={0};			// %4
  uint8_t rgb_pattern0[RGB_STRINGS_MAX]={0};				// %4

  // MIDI?
#if defined USE_MIDI && defined MIDI_IN_PIN
  uint8_t MIDI_in_pin=MIDI_IN_PIN;					// %4
#else
  uint8_t MIDI_in_pin=ILLEGAL8;						// %4
#endif

#if defined USE_MIDI && defined MIDI_OUT_PIN
  uint8_t MIDI_out_pin=MIDI_OUT_PIN;
#else
  uint8_t MIDI_out_pin=ILLEGAL8;
#endif

  // SD CARD?	// TODO: implement
  // type
  // MOSI  15
  // SCK   14
  // MISO   2
  // CS    13

  // MCP23017 et al
  // type
  // addr
  // ...

  // ADS1115 et al
  // type
  // addr
  // ...

  // CMOS PLL 4046
  // ...

  // other pins
  uint8_t magical_fart_output_pin=ILLEGAL8;	// who knows, maybe?
  uint8_t magical_sense_pin=ILLEGAL8;		// maybe?	i.e. see: magical_fart

  // 8 bytes RESERVED for future use, forward compatibility
#if defined ESP_NOW_CHANNEL
  uint8_t esp_now_channel=ESP_NOW_CHANNEL;	// *not used yet*	// %4
#else
  uint8_t esp_now_channel=ILLEGAL8;		// *not used yet*	// %4	TODO: ESP_NOW_CHANNEL?
#endif

  uint8_t midi_does_pitch_bend=false;		// *not used yet*
#if defined USE_MIDI
  uint16_t midi_baudrate_div10 = (MIDI_BAUDRATE / 10);
#else
  uint16_t midi_baudrate_div10=0;	// 0: no MIDI	3125: 31250	11520: 115200
#endif
  /*
	0	(MIDI not used)
	120	1200
	240	2400
	480	4800
	960	9600
	1920	19200
	3125	31250	<<< MIDI
	3840	38400
	5760	57600
	11520	115200	<<< UART default
	23040	230400
	25000	250000
	50000	500000
	// 111111	1000000
  */
  uint8_t reserved4=ILLEGAL8;						// %4
  uint8_t reserved5=ILLEGAL8;
  uint8_t reserved6=ILLEGAL8;
  uint8_t reserved7=ILLEGAL8;

  // other other
  uint8_t tone_pin=ILLEGAL8; // from very old code, could be recycled?	// %4

  // version
  uint8_t version = 0;		// 0 means in development

//// nvs read flags		// do we need|want them?
//  bool read_from_nvs=false;	// set if *anything* was read	do we need|want that?
//  bool gpios_from_nvs=false;	// do we need|want that?

} pulses_hardware_conf_t;

extern pulses_hardware_conf_t HARDWARE;	// hardware of this instrument

void show_pin_or_dash(uint8_t pin) {
  if(pin==ILLEGAL8)
    MENU.out('-');
  else
    MENU.out(pin);
}

void show_monochrome_type(int type) {
  MENU.out(F("monochrome\t\t"));
  switch(type) {
  case monochrome_type_off:
    MENU.outln('-');
    break;
  case monochrome_type_heltec:
    MENU.outln(F("heltec"));
    break;
  case monochrome_type_LiPO:
    MENU.outln(F("OLED LiPO"));
    break;
  case monochrome_type_LILYGO_T5:
    MENU.outln(F("LILYGO_T5"));
    break;
  default:
    extern void ERROR_ln(const char* text);
    ERROR_ln(F("monochrome_type unknown"));
  }
} // show_monochrome_type(int type)


bool show_pulses_pin_usage(gpio_pin_t pin) {
  bool retval=false;
  // gpio
  if(HARDWARE.gpio_pins_cnt) {
    for(int i=0; 1 < HARDWARE.gpio_pins_cnt; i++) {
      if(pin == HARDWARE.gpio_pins[i]) {
	MENU.out(F("GPIO "));
	MENU.out(pin);
	MENU.tab(2);
	retval = true;
      }
    }
  }

  // dac
  if(pin == HARDWARE.DAC1_pin) {
    MENU.out(F("DAC1 "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }
  if(pin == HARDWARE.DAC2_pin) {
    MENU.out(F("DAC2 "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

  // morse
  if(pin == HARDWARE.morse_touch_input_pin) {
    MENU.out(F("morse touch "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }
  if(pin == HARDWARE.morse_gpio_input_pin) {
    MENU.out(F("morse gpio "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }
  if(pin == HARDWARE.morse_output_pin) {
    MENU.out(F("morse out "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // rgb led strings
  if(HARDWARE.rgb_strings) {
    for(int i=0; i < RGB_STRINGS_MAX; i++) {
      if(pin == HARDWARE.rgb_pin[i]) {
	MENU.out(F("RGB["));
	MENU.out(i);
	MENU.out(F("] data "));
	MENU.out(pin);
	MENU.tab();
	retval = true;
      }
    }
  }

  // trigger
  if(pin == HARDWARE.musicbox_trigger_pin) {
    MENU.out(F("trigger "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

  // peripheral power switch
  if(pin == HARDWARE.periph_power_switch_pin) {
    MENU.out(F("peripheral power "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // battery control
  if(pin == HARDWARE.battery_level_control_pin) {
    MENU.out(F("battery level "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // bluetooth
  if(pin == HARDWARE.bluetooth_enable_pin) {
    MENU.out(F("bluetooth enable "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // MIDI
  if(pin == HARDWARE.MIDI_in_pin) {
    MENU.out(F("MIDI IN "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }
  if(pin == HARDWARE.MIDI_out_pin) {
    MENU.out(F("MIDI OUT "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  // other pins
  if(pin == HARDWARE.magical_fart_output_pin) {
    MENU.out(F("magical fart "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  if(pin == HARDWARE.magical_sense_pin) {
    MENU.out(F("magical sense "));
    MENU.out(pin);
    MENU.tab();
    retval = true;
  }

  if(pin == HARDWARE.tone_pin) {	// from very old code, could be recycled?
    MENU.out(F("tone "));
    MENU.out(pin);
    MENU.tab(2);
    retval = true;
  }

// HCSR04_TRIGGER_PIN
// HCSR04_ECHO_PIN

// LED_PIN
//#if defined ONBOARD_LED
//  #error IMPLEMENT ONBOARD_LED ...
//#endif

  return retval;
} // show_pulses_pin_usage()

void show_hardware_conf(pulses_hardware_conf_t* hardware) {
#if defined PULSES_SYSTEMS
  MENU.out(F("GPIO click pins\t\t"));
  if(hardware->gpio_pins_cnt) {
    MENU.outln(hardware->gpio_pins_cnt);
    extern void show_GPIOs();
    show_GPIOs();
  } else
    MENU.outln('-');
#endif

  MENU.out(F("DAC1/DAC2 pins\t\t"));
  show_pin_or_dash(hardware->DAC1_pin);
  MENU.tab();
  show_pin_or_dash(hardware->DAC2_pin);
  MENU.ln();

  MENU.out(F("MPU6050\t\t\t"));
  if(hardware->mpu6050_addr) {
    MENU.out_hex(hardware->mpu6050_addr);
    MENU.ln();
#if defined USE_MPU6050_at_ADDR
    extern void show_accGyro_offsets();
    show_accGyro_offsets();
#endif
  } else
    MENU.outln(F("no"));

  MENU.out(F("musicbox_trigger_pin\t"));
  show_pin_or_dash(hardware->musicbox_trigger_pin);
  MENU.ln();

  MENU.out(F("battery level pin\t"));
  show_pin_or_dash(hardware->battery_level_control_pin);
  MENU.ln();

  MENU.out(F("peripheral power switch\t"));
  show_pin_or_dash(hardware->periph_power_switch_pin);
  MENU.ln();

  MENU.out(F("morse_touch_input\t"));
  show_pin_or_dash(hardware->morse_touch_input_pin);
  MENU.ln();

  MENU.out(F("morse_gpio_input\t"));
  show_pin_or_dash(hardware->morse_gpio_input_pin);
  MENU.ln();

  MENU.out(F("morse_output_pin\t"));
  show_pin_or_dash(hardware->morse_output_pin);
  MENU.ln();

  MENU.out(F("bluetooth_enable_pin\t"));
  show_pin_or_dash(hardware->bluetooth_enable_pin);
  MENU.ln();

  show_monochrome_type(hardware->monochrome_type);

#if defined USE_ESP_NOW
  MENU.out(F("ESP-NOW CHANNEL\t\t"));
  show_pin_or_dash(hardware->esp_now_channel);
  MENU.ln();
#endif

  MENU.out(F("RGB LED strings\t\t"));
  if(hardware->rgb_strings) {
    MENU.outln(hardware->rgb_strings);
    for(int i=0; i<hardware->rgb_strings; i++) {
      MENU.out(F(" #"));
      MENU.out(i);

      MENU.out(F("\tpin "));
      MENU.out(hardware->rgb_pin[i]);

      MENU.out(F("\tcnt "));
      MENU.out(hardware->rgb_pixel_cnt[i]);

      MENU.out(F("\tvoltage "));
      MENU.out(hardware->rgb_led_voltage_type[i]);

      MENU.out(F("\tstart "));
      MENU.outln(hardware->rgb_pattern0[i]);
    }

    MENU.out(F("all use voltage type\t"));
    MENU.outln(hardware->rgb_led_voltage_type[0]);
  } else
    MENU.outln('-');

  MENU.out(F("MIDI\t\t\tin "));
  show_pin_or_dash(hardware->MIDI_in_pin);
  MENU.out(F("\tout "));
  show_pin_or_dash(hardware->MIDI_out_pin);
  MENU.ln();

/*
  MENU.outln(F("RTC\t\t\tTODO:"));	// TODO: RTC
*/

  // other pins
  // switch(version)
  // nvs flags
  // LORA?
  MENU.ln();
} // show_hardware_conf()


void show_current_hardware_conf() {	// same, with title, for menu output
  MENU.outln(F("current HARDWARE configuration:"));
  show_hardware_conf(&HARDWARE);
}

#define PULSES_HARDWARE_CONF_H
#endif
