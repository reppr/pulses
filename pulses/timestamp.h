/*
  timestamp.h

  *unused* file with code from the Humidity sketch
  meant to become useful for timestamps use in pulses
*/

#if ! defined TIMESTAMP_H

#define TIMESTAMPLEN	26		// including '\0'
char timestamp[TIMESTAMPLEN] = {0};	// new: declared globally

bool rtc_module_is_usable=false;
bool time_synchronised_from_NTP=false;	// synchonised  local time sources esp32rtc and maybe RTC module
bool volatile is_waiting_for_NTP_reply=false;

bool timestamp_from_RTC_module=false;
bool timestamp_from_esp32rtc=false;
bool timestamp_alternative=true;	// like sample count or somesuch, fake fallback when time is unknown

void autoselect_time_source() { // set timestamp_from_RTC_module, timestamp_alternative, timestamp_from_esp32rtc
  timestamp_from_RTC_module=false;
  timestamp_from_esp32rtc=false;

#if defined USE_TIMESERVER_FROM_WIFI

  is_waiting_for_NTP_reply; // drop;	it is volatile? but I do not get it...
  if(is_waiting_for_NTP_reply) {
    MENU.out(F("\nwaiting for NTP reply "));
    for (int i=0; i<12 && is_waiting_for_NTP_reply; i++) {
      delay(500);
      MENU.out('.');
      is_waiting_for_NTP_reply; // drop;	it is volatile? but I do not get it...
    }
    MENU.ln();
  }

  if (time_synchronised_from_NTP) {
    #if defined USE_RTC_MODULE
    if(rtc_module_is_usable)	// double check	// DADA: TODO: check again
      timestamp_from_RTC_module = true;
    else {
      timestamp_from_esp32rtc = true;
    }
    #endif
  }
#else
  #if defined USE_RTC_MODULE
  if(!timestamp_from_esp32rtc && !timestamp_from_RTC_module)	// no time source found yet
    timestamp_from_RTC_module = rtc_module_is_usable;
  #endif
#endif

  // timestamp_alternative=true;	like sample count or somesuch, fake FALLBACK when TIME IS UNKNOWN
  timestamp_alternative= !(timestamp_from_RTC_module || timestamp_from_esp32rtc); // still nothing found?
} // autoselect_time_source()


#define TIMESTAMPLEN	26		// including '\0'
char time_indicator[TIMESTAMPLEN]= {0};	// new: global declaration

char* update_timestamp() {		// build timestamp from used time source
  if(timestamp_from_RTC_module) {
    update_DS1307_time_stamp();		// get time from RTC module, timestamp_from_esp32rtc,
  } else {
    if (timestamp_from_esp32rtc) {
      update_esp32rtc_time_stamp();
    } else {
      // timestamp_alternative
      snprintf(timestamp, TIMESTAMPLEN, F("%i]\t\t\0"), got_data_cnt);	// FALLBACK if we do not know the time
    }
  }
  return timestamp;
} // update_timestamp()

char* get_time_indicator() {	// string to start data file entry
  time_indicator[0] = 0;
  // 'time_indicator' starts the line with a timestamp or last sample number

  // autoselect_time_source();	// must have been called before
  update_timestamp();
  strncpy(time_indicator , timestamp, TIMESTAMPLEN-4);	// skip day of the week
  time_indicator[20] = 0;

#if defined DEBUGGING
  MENU.ln();
  MENU.out(F("time_indicator "));
  MENU.out_IstrI(time_indicator);
  MENU.tab();
  MENU.outln(strlen(time_indicator));
  MENU.ln();
#endif

  return time_indicator;
} // get_time_indicator()


// extern char* update_DS1307_time_stamp();
// extern char* update_esp32rtc_time_stamp();
// extern char time_indicator[];



#define TIMESTAMP_H
#endif
