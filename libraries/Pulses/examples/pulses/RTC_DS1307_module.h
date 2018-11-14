/*
  RTC_DS1307_module.h

  tested on 'Tiny RTC Module'	DS1307
  DS3231 should work also

  see: https://randomnerdtutorials.com/guide-for-real-time-clock-rtc-module-with-arduino-ds1307-and-ds3231/
  sketch was written by John Boxall from http://tronixstuff.com

  adapted for use in pulses
*/

#include "Wire.h"

bool rtc_module_is_usable=true;	// to *get* a test read on first read, we fake it to be there

#define DS1307_I2C_ADDRESS 0x68

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

  Wire.beginTransmission(DS1307_I2C_ADDRESS);
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
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);	// set DS1307 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
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

// if(rtc_module_is_usable) show time stamp, else be quiet :)
void show_DS1307_time_stamp() {		// format like  "2018-11-11_11h11m11s"
  if(! rtc_module_is_usable)
    return;	// be quiet if the system considers the module to be missing or not initialized

  byte second, minute, hour, weekday, day, month, year;
  if(read_DS1307_time(&second, &minute, &hour, &weekday, &day, &month, &year)) {
    MENU.out_Error_();
    MENU.out(F("read_DS1307_time()\t*deactivated*"));
    return;	// ERROR
  }

  MENU.out((int) 20);	// "20xx" century
  if(year < 10)
    MENU.out('0');
  MENU.out((int) year);
  MENU.out('-');
  if(month < 10)
    MENU.out('0');
  MENU.out((int) month);
  MENU.out('-');
  if(day < 10)
    MENU.out('0');
  MENU.out((int) day);
  MENU.out('_');
  if(hour < 10)
    MENU.out('0');
  MENU.out((int) hour);
  MENU.out('h');
  if (minute < 10)
    MENU.out('0');
  MENU.out((int) minute);
  MENU.out('m');
  if (second < 10)
    MENU.out('0');
  MENU.out((int) second);
  MENU.out(F("s\t"));

  switch(weekday){
  case 1:
    MENU.out(F("SUN"));
    break;
  case 2:
    MENU.out(F("MON"));
    break;
  case 3:
    MENU.out(F("TUE"));
    break;
  case 4:
    MENU.out(F("WED"));
    break;
  case 5:
    MENU.out(F("THU"));
    break;
  case 6:
    MENU.out(F("FRI"));
    break;
  case 7:
    MENU.out(F("SAT"));
    break;
  default:
    MENU.out(F("???"));		// ERROR:  like 0 == not initialised
  }
}
