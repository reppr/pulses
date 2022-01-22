/*
  SD_card_logging
*/

#include <SPI.h>
#include <SD.h>

SPIClass SDspi;

bool SD_card_ok = false;

const char* logfilename = "/log/LOG.txt";
//#define LOGFILENAME	/log/LOG.txt

File logFile;

void setup_SD_card() {
  MENU.out(F("\nsetup_SD_card(14, 2, 15, 13) SPI\t"));
  SDspi.begin(14, 2, 15, 13);

  if(SD_card_ok = SD.begin(13, SDspi))
    MENU.outln(F("ok"));
  else {
    MENU.outln(F("failed"));
    return;	// no card
  }

  // prepare logging:
  SD.mkdir("/log");			// create /log/ directory
  //
  if(logFile = SD.open(logfilename, FILE_APPEND)) {
    MENU.out(F("logfile "));
    MENU.outln(logfilename);
    logFile.close();

  } else {
    MENU.out(F("could not open "));
    MENU.outln(logfilename);
  }
} // setup_SD_card()


// try:	extern void do_on_other_core(void (*function_p)());
//	do_on_other_core(&append_battery_log);
void append_battery_log() {	// factored out
  if(! SD_card_ok)
    return;

  char* buf = (char*) malloc(32);
  if(buf) {
    extern unsigned int read_battery_level(unsigned int oversampling=15);
    int battery_level = read_battery_level();
    if(battery_level) {		// not on USB power
      snprintf(buf, 32, "\tbattery=%d", read_battery_level());
      logFile.print(buf);
    } else
      logFile.print(F("\tUSB power"));

    free(buf);
  } // else MENU.malloc_error();
} // append_battery_log()

void log_message(char* text, bool log_battery=false) {		// simple log message	open, log, close
  if(! SD_card_ok)
    return;

  if(logFile = SD.open(logfilename, FILE_APPEND)) {
    logFile.print(text);
    if(log_battery)
      do_on_other_core(&append_battery_log);
    logFile.println("");

    logFile.close();
  } // else ignore
} // log_message()

void log_message_timestamped(char* text, bool log_battery=false) {	// simple log messagewith time stamp
  if(! SD_card_ok)
    return;

  if(logFile = SD.open(logfilename, FILE_APPEND)) {
    char* timestamp = DS1307_time_stamp__();
    if(timestamp) {
      logFile.print(timestamp);
      free(timestamp);
      logFile.print(" ");
    }
    logFile.print(text);
    if(log_battery)
      do_on_other_core(&append_battery_log);
    logFile.println("");

    logFile.close();
  } // else ignore
} // log_message_timestamped()

void start_log_entry(char* text=NULL, bool log_battery=false) {		// opens logFile, does not close
  if(! SD_card_ok)
    return;

  if(logFile = SD.open(logfilename, FILE_APPEND)) {
    char* timestamp = DS1307_time_stamp__();
    if(timestamp) {
      logFile.print(timestamp);
      free(timestamp);
    }
    if(text) {
      logFile.print(" ");
      logFile.print(text);
    }
    if(log_battery)
      do_on_other_core(&append_battery_log);
    // *logFile stays open*	*NO* logFile.close();
  } //  else ignore
} // start_log_entry()

void end_log_entry(char* text=NULL, bool log_battery=false) {		// closes logFile
  if(! SD_card_ok)
    return;

  if(text)
    logFile.print(text);
  if(log_battery)
    do_on_other_core(&append_battery_log);

  logFile.println("");
  logFile.close();
} // end_log_entry()

// try:	extern void do_on_other_core(void (*function_p)());
//	do_on_other_core(&log_battery_level);
void log_battery_level() {	// TODO: FIXME: use append_battery_log() ################################################################
  if(! SD_card_ok)
    return;

  char* buf = (char*) malloc(32);
  if(buf) {
    extern unsigned int read_battery_level(unsigned int oversampling=15);	// TODO: FIXME: use append_battery_log()
    int battery_level = read_battery_level();
    if(battery_level) {		// not on USB power
      snprintf(buf, 32, "battery=%d", read_battery_level());
      start_log_entry(buf);
      end_log_entry();
    } // else log_message(F("on USB power"));
    free(buf);
  } else MENU.malloc_error();
} // log_battery_level()

// try:	extern void do_on_other_core(void (*function_p)());
//	do_on_other_core(&show_logfile);
void show_logfile() {
  if(logFile = SD.open(logfilename, FILE_READ)) {
    while (logFile.available()) {
      MENU.out((char) logFile.read());
    }
    logFile.close();

  } else {
    MENU.out(F("could not open "));
    MENU.outln(logfilename);
  }
} // show_logfile()
