/*
  RTC_module_DS1307_or_DS3231.h

  tested on 'Tiny RTC Module' DS1307  and  'precision RTC module' DS3231

  see: https://randomnerdtutorials.com/guide-for-real-time-clock-rtc-module-with-arduino-ds1307-and-ds3231/
  sketch was written by John Boxall from http://tronixstuff.com

  adapted for use in pulses
*/

#include "Wire.h"

bool rtc_module_is_usable=true;	// to *get* a test read on first read, we fake it to be there

//#if defined PULSES_SYSTEM	// see: HARDWARE.RTC_addr (unsused)
#define RTC_I2C_ADDRESS 0x68	// DS3231 and DS1307 use the same address 0x68	// attention MPU6050 can use it too

// check_for_rtc_module() set 'rtc_module_is_usable' and inform user about RTC
bool check_for_rtc_module() {	// TODO: FIXME: RTC and mpu6050 might be on same address!
  Wire.beginTransmission(RTC_I2C_ADDRESS);
  rtc_module_is_usable = ! Wire.endTransmission();

  MENU.out(F("\nRTC module "));
  if(rtc_module_is_usable)
    MENU.outln(F("found"));
  else
    MENU.outln(F("not available"));

  return rtc_module_is_usable;
}

byte decToBcd(byte val) {	// convert normal decimal numbers to binary coded decimal
  return( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val) {	// convert binary coded decimal to normal decimal numbers
  return( (val/16*10) + (val%16) );
}

int day_of_week(int y, int m, int d) {		// SUN == 1, MON == 2, , , , , SAT == 7
/*
  int day_of_week(int y, int m, int d);		// SUN == 1, MON == 2, , , , , SAT == 7

  see  https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
  Michael Keith and Tom Craver, 1990
  I leave this juwel of unreadable code as is ;)
  ...  even added '+1' to start with sunday==1
*/
  return (d+=m<3?y--:y-2,23*m/9+d+4+y/4-y/100+y/400)%7+1;
}

void set_DS1307_time(byte second, byte minute, byte hour, byte day, byte month, byte year) {
  byte weekday= day_of_week(year, month, day);	// SUN == 1, MON == 2, , , , , SAT == 7

  Wire.beginTransmission(RTC_I2C_ADDRESS);
  Wire.write(0);			// set next input to start at the seconds register
  Wire.write(decToBcd(second));		// set seconds
  Wire.write(decToBcd(minute));		// set minutes
  Wire.write(decToBcd(hour));		// set hours
  Wire.write(decToBcd(weekday));	// SUN == 1, MON == 2, , , , , SAT == 7
  Wire.write(decToBcd(day));		// set date (1 to 31)
  Wire.write(decToBcd(month));		// set month
  Wire.write(decToBcd(year));		// set year (0 to 99)
  Wire.endTransmission();
}

bool read_DS1307_time(byte *second, byte *minute, byte *hour,
		      byte *weekday, byte *day, byte *month, byte *year) {	// returns error
  Wire.beginTransmission(RTC_I2C_ADDRESS);
  Wire.write(0);	// set DS1307 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(RTC_I2C_ADDRESS, 7);
  // request seven bytes of data from DS1307 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *weekday = bcdToDec(Wire.read());
  *day = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());

  if (*month==0 && *day==0 && *weekday==0) {	// invalid, no module reached or not initialized
    rtc_module_is_usable = false;
    return true;	// ERROR
  }

  rtc_module_is_usable = true;
  return false;	// ok
}

#define TIMESTAMPLEN	26	// including '\0'
char* DS1307_time_stamp__() {	// format like  "2018-11-11_11h11m11s_DAY"
  if(! rtc_module_is_usable)
    return NULL;	// be quiet if the system considers the module to be missing or not initialized

  byte second, minute, hour, weekday, day, month, year;
  if(read_DS1307_time(&second, &minute, &hour, &weekday, &day, &month, &year)) {
    MENU.out_Error_();
    MENU.out(F("read_DS1307_time()\t*deactivated*"));
    return NULL;	// ERROR
  }

  char* timestamp = (char*) malloc(TIMESTAMPLEN);
  if (timestamp == NULL)
    return NULL;	// ERROR

  snprintf(timestamp, TIMESTAMPLEN, "20%02d-%02d-%02d_%02dh%02dm%02ds_", year, month, day, hour, minute, second);
  switch(weekday){
  case 1:
    strcat(timestamp, "SUN");
    break;
  case 2:
    strcat(timestamp, "MON");
    break;
  case 3:
    strcat(timestamp, "TUE");
    break;
  case 4:
    strcat(timestamp, "WED");
    break;
  case 5:
    strcat(timestamp, "THU");
    break;
  case 6:
    strcat(timestamp, "FRI");
    break;
  case 7:
    strcat(timestamp, "SAT");
    break;
  default:
    strcat(timestamp, "???");		// ERROR:  like 0 == not initialised
  }

  return timestamp;
} // DS1307_time_stamp__()


// if(rtc_module_is_usable) show time stamp, else be quiet :)
void show_DS1307_time_stamp() {		// format like  "2018-11-11_11h11m11s"
  if(! rtc_module_is_usable)
    return;	// be quiet if the system considers the module to be missing or not initialized

  char* timestamp= DS1307_time_stamp__();
  if(timestamp) {
    MENU.out(timestamp);
    free(timestamp);
  }
} // show_DS1307_time_stamp()
