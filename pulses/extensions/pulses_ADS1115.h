/*
  extensions/pulses_ADS1115.h
*/

#include <Adafruit_ADS1015.h>
Adafruit_ADS1115* ADS1115;

adsGain_t ADS1115_gain;
short ADS1115_range_mV=-1;		// for display (only?)	ILLEGAL
float ADS1115_resolution_mV=-1;	// for display (only?)	ILLEGAL


void ads1115_first_test_A0() {
  static int16_t just_read = (*ADS1115).readADC_SingleEnded(0);	// initialise to get sum right
  int16_t read_before=just_read;
  just_read = (*ADS1115).readADC_SingleEnded(0);

  static float volts= (just_read * ADS1115_resolution_mV)/1000;	// initialise to get sum right
  float volts_before=volts;
  volts = (just_read * ADS1115_resolution_mV) / 1000.0;

  MENU.out(F("volts "));
  MENU.out(volts, 6);

  float diff = volts - volts_before;
  MENU.out(F("\t± "));
  if(diff == 0)
    MENU.out('=');
  else if(diff > 0)
    MENU.out('+');
  MENU.out(diff, 6);

  static float sum=0.0;
  sum += diff;
  MENU.out(F("\t\t(summed ± "));
  if(sum >= 0)
    MENU.out('+');
  MENU.out(sum, 6);

  MENU.out(F(")\tresolution mV "));
  MENU.out(ADS1115_resolution_mV, 6);

  MENU.out(F("\t\t15bit read "));
  MENU.out(just_read);

  MENU.out(F("\t\trange mV "));
  MENU.outln(ADS1115_range_mV);
}


void ads1115_first_test_diff_A2_A3() {
  static int16_t just_read = (*ADS1115).readADC_Differential_2_3();	// initialise to get sum right
  int16_t read_before=just_read;
  just_read = (*ADS1115).readADC_Differential_2_3();

  static float volts= (just_read * ADS1115_resolution_mV)/1000;	// initialise to get sum right
  float volts_before=volts;
  volts = (just_read * ADS1115_resolution_mV) / 1000.0;

  MENU.out(F("volts "));
  MENU.out(volts, 6);

  float diff = volts - volts_before;
  MENU.out(F("\t± "));
  if(diff == 0)
    MENU.out('=');
  else if(diff > 0)
    MENU.out('+');
  MENU.out(diff, 6);

  static float sum=0.0;
  sum += diff;
  MENU.out(F("\t\t(summed ± "));
  if(sum >= 0)
    MENU.out('+');
  MENU.out(sum, 6);

  MENU.out(F(")\tresolution mV "));
  MENU.out(ADS1115_resolution_mV, 6);

  MENU.out(F("\t\t15bit read "));
  MENU.out(just_read);

  MENU.out(F("\t\trange mV "));
  MENU.outln(ADS1115_range_mV);
}


void pulses_ADS1115_setup() {
  ADS1115 = new Adafruit_ADS1115(USE_ADS1115_AT_ADDR);

  (*ADS1115).begin();
  //(*ADS1115).setGain((adsGain_t) ADS1015_REG_CONFIG_PGA_4_096V);
  //(*ADS1115).setGain((adsGain_t) ADS1015_REG_CONFIG_PGA_2_048V);
  (*ADS1115).setGain((adsGain_t) ADS1015_REG_CONFIG_PGA_1_024V);
  //(*ADS1115).setGain((adsGain_t) ADS1015_REG_CONFIG_PGA_0_512V);	// GAIN_EIGHT
  //(*ADS1115).setGain((adsGain_t) ADS1015_REG_CONFIG_PGA_0_256V);

  ADS1115_gain = (*ADS1115).getGain();
  switch(ADS1115_gain) {
  case ADS1015_REG_CONFIG_PGA_6_144V:	// GAIN_TWOTHIRDS
    ADS1115_range_mV = 6144;
    break;
  case ADS1015_REG_CONFIG_PGA_4_096V:	// GAIN_ONE
    ADS1115_range_mV = 4096;
    break;
  case ADS1015_REG_CONFIG_PGA_2_048V:	// GAIN_TWO
    ADS1115_range_mV = 2048;
    break;
  case ADS1015_REG_CONFIG_PGA_1_024V:	// GAIN_FOUR
    ADS1115_range_mV = 1024;
    break;
  case ADS1015_REG_CONFIG_PGA_0_512V:	// GAIN_EIGHT
    ADS1115_range_mV = 512;
    break;
  case ADS1015_REG_CONFIG_PGA_0_256V:	// GAIN_SIXTEEN
    ADS1115_range_mV = 256;
    break;
  default:
    MENU.error_ln(F("ADS1115_gain"));
  } // switch(ADS1115_gain)

  MENU.out(F("\nADS1115_range_mV "));
  MENU.out(ADS1115_range_mV);

  ADS1115_resolution_mV = ADS1115_range_mV / 32767.0;
  MENU.out(F("\tresolution mV "));
  MENU.out(ADS1115_resolution_mV, 6);
  MENU.ln();
} // pulses_ADS1115_setup()
