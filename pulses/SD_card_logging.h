/*
  SD_card_logging
*/

#include <SPI.h>
#include <SD.h>

SPIClass SDspi;

bool SD_card_ok = false;

bool do_log = true;

#if defined LOG_PLAY_DEFAULT
  bool log_play = true;
#else
  bool log_play = false;
#endif

#if defined LOG_BATTERY_DEFAULT
  bool log_battery = true;
#else
  bool log_battery = false;
#endif


const char* logfilename = "/log/LOG.txt";
//#define LOGFILENAME	/log/LOG.txt

File logFile;

void setup_SD_card() {
  MENU.out(F("\nsetup_SD_card(14, 2, 15, 13) SPI\t"));
  SDspi.begin(14, 2, 15, 13);

  if((SD_card_ok = SD.begin(13, SDspi)))
    MENU.outln(F("ok"));
  else {
    MENU.outln(F("failed"));
    do_log = false;
    return;	// no card
  }

  // prepare logging:
  SD.mkdir("/log");			// create /log/ directory
  //
  if(logFile = SD.open(logfilename, FILE_APPEND)) {
    MENU.out(F("logfile "));
    MENU.outln(logfilename);
    logFile.close();
    do_log = true;

  } else {
    MENU.out(F("could not open "));
    MENU.outln(logfilename);
    do_log = false;
  }
} // setup_SD_card()


// try:	extern void do_on_other_core(void (*function_p)());
//	do_on_other_core(&append_battery_log);
void append_battery_log() {	// factored out
  if(! do_log)
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
  if(! do_log)
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
  if(! do_log)
    return;

  if(logFile = SD.open(logfilename, FILE_APPEND)) {
#if defined USE_RTC_MODULE
    if(rtc_module_is_usable) {
      char* timestamp = DS1307_time_stamp__();
      if(timestamp) {
	logFile.print(timestamp);
	free(timestamp);
	logFile.print(" ");
      }
    }
#endif // USE_RTC_MODULE

    logFile.print(text);
    if(log_battery)
      do_on_other_core(&append_battery_log);
    logFile.println("");

    logFile.close();
  } // else ignore
} // log_message_timestamped()

void start_log_entry(const char* text=NULL, bool log_battery=false) {		// opens logFile, does not close
  if(! do_log)
    return;

  if(logFile = SD.open(logfilename, FILE_APPEND)) {
#if defined USE_RTC_MODULE
    if(rtc_module_is_usable) {
      char* timestamp = DS1307_time_stamp__();
      if(timestamp) {
	logFile.print(timestamp);
	free(timestamp);
      }
    }
#endif // USE_RTC_MODULE

    if(text) {
      logFile.print(" ");
      logFile.print(text);
    }
    if(log_battery)
      do_on_other_core(&append_battery_log);
    // *logFile stays open*	*NO* logFile.close();
  } //  else ignore
} // start_log_entry()

void end_log_entry(const char* text=NULL, bool log_battery=false) {		// closes logFile
  if(! do_log)
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
  if(! do_log)
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

void logging_UI_display() {
  if(! SD_card_ok)
    MENU.out(F(">>>NO SD CARD<<<  "));

  MENU.out(F("'O' "));
  if(do_log && SD_card_ok)
    MENU.out(F("LOGGING"));
  else
    MENU.out(F("logging"));

  MENU.out(F("  'O?'=show  'OE'=init  'OT'=off  'OP'="));
  if(log_play)
    MENU.out(F("PLAY"));
  else
    MENU.out(F("play"));

#if defined USE_BATTERY_LEVEL_CONTROL
  MENU.out(F("  'OB'="));
  if(log_battery)
    MENU.out(F("BATTERY"));
  else
    MENU.out(F("battery"));
#endif

  MENU.outln();
} // logging_UI_display()

bool logging_UI_reaction() {	// 'Ox'
  uint8_t first_token = MENU.peek();
  switch(first_token) {
  case '?':	// 'O?'		show_logfile()
    MENU.drop_input_token();
  case EOF8:	// bare 'O'	show_logfile()
    do_on_other_core(&show_logfile);
    break;

  case 'E':
    MENU.drop_input_token();
    setup_SD_card();
    break;

  case 'T':
    MENU.drop_input_token();
    logFile.close();
    do_log = false;
    MENU.outln(F("logging off"));
    break;

  case 'P':
    MENU.drop_input_token();
    if(! (log_play = ! log_play))
      MENU.out(F("not "));
    MENU.outln(F("logging play"));
    break;

#if defined USE_BATTERY_LEVEL_CONTROL
  case 'B':
    MENU.drop_input_token();
    if((log_battery = ! log_battery))
      MENU.out(F("not "));
    else
      MENU.out(F("TODO: IMPLEMENT LOGGING BATTERY"));
    MENU.outln(F("logging battery"));
    break;
#endif

  default:
    return false;	// not found
  }

  return true;		// ok, found
} // logging_UI_reaction()
