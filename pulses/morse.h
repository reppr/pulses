/*
  morse.h
  version 3 touch only
*/

#ifndef MORSE_H

//#define TOUCH_ISR_VERSION_2	// removed, see: morse_unused.txt
#define TOUCH_ISR_VERSION_3	// other code removed, see: morse_unused.txt

#define TOKEN_LENGTH_FEEDBACK_PULSE	// using a pulse for morse duration feedback
//#define TOKEN_LENGTH_FEEDBACK_TASK	// using a rtos task for morse duration feedback, experimental
// sanity check:
#if defined TOKEN_LENGTH_FEEDBACK_PULSE && defined TOKEN_LENGTH_FEEDBACK_TASK
  #error morse.h: TOKEN_LENGTH_FEEDBACK_PULSE and TOKEN_LENGTH_FEEDBACK_TASK are both defined
#endif

#define COMPILE_MORSE_CHEAT_SHEETS	// ;)

//#define DEBUG_TREAT_MORSE_EVENTS_V3

#if ! defined MORSE_MONOCHROME_ROW
  #define MORSE_MONOCHROME_ROW	0	// row for monochrome morse display
#endif

//#define MORSE_DECODE_DEBUG	// tokens to letters, commands

//#define MORSE_DEBUG_RECEIVE_TOKEN	// TODO: REMOVE: debugging code
//#define MORSE_TOKEN_DEBUG		// low level token recognising debugging
//#define DEBUG_MORSE_TOUCH_INTERRUPT
#define MORSE_COMPILE_HELPERS		// compile some functions for info, debugging, *can be left out*

#include "touch.h"
#include "driver/touch_pad.h"   // new ESP32-arduino version needs this for 'touch_pad_set_trigger_mode' declaration
#include "esp_attr.h"

#if defined ESP32
  extern const char* esp_err_to_name(esp_err_t);
#endif

// ################ TODO: MOVE: configuration
// use GPIO (with pulldown) as morse input?
// #define MORSE_GPIO_INPUT_PIN	34	// Morse input GPIO pin, 34 needs hardware pulldown

//#define MORSE_OUTPUT_PIN	2 // 2 is often blue onboard led, hang a led, a piezzo on that one :)
//#define MORSE_OUTPUT_PIN	12 // *one shot TEST ONLY*	// TODO: separe from PERIPHERAL_POWER_SWITCH_PIN

#ifndef MORSE_TOUCH_INPUT_PIN		// TODO: be more flexible
  #define MORSE_TOUCH_INPUT_PIN	13	// use ESP32 touch sensor as morse input	(13 just a test)
#endif

bool morse_uppercase=true;	// lowercase very rarely used, changed many menu interfaces
// menu interface is case sensitive, so I need lowercase

portMUX_TYPE morse_MUX = portMUX_INITIALIZER_UNLOCKED;

// morse tick
unsigned long morse_TimeUnit=100000;

unsigned long morse_start_ON_time=0;
unsigned long morse_start_OFF_time=0;

// morse token timing
uint8_t dotTim=1;
uint8_t dashTim=3;
uint8_t loongTim=9;
uint8_t overlongTim=15;

uint8_t separeTokenTim=1;	// used only for statistics, seems buggy btw
uint8_t separeLetterTim=3;
uint8_t separeWordTim=7;


float limit_debounce;
float limit_dot_dash;
float limit_dash_loong;
float limit_loong_overlong;

#if defined MORSE_COMPILE_HELPERS
// morse_show_limits():	code left for debugging
void morse_show_limits() {
  MENU.out("limit_debounce\t\t");
  MENU.out(limit_debounce);
  MENU.tab();
  MENU.outln((unsigned int) (limit_debounce * morse_TimeUnit));

  MENU.out("limit_dot_dash\t\t");
  MENU.out(limit_dot_dash);
  MENU.tab();
  MENU.outln((unsigned int) (limit_dot_dash * morse_TimeUnit));

  MENU.out("limit_dash_loong\t");
  MENU.out(limit_dash_loong);
  MENU.tab();
  MENU.outln((unsigned int) (limit_dash_loong * morse_TimeUnit));

  MENU.out("limit_loong_overlong\t");
  MENU.out(limit_loong_overlong);
  MENU.tab();
  MENU.outln((unsigned int) (limit_loong_overlong * morse_TimeUnit));

  MENU.ln();
}
#endif

#define MORSE_TOKEN_MAX	128			// a *lot* for debugging...	TODO:
volatile unsigned int morse_token_cnt=0;	// count up to MORSE_TOKEN_MAX . _ ! V tokens to form one letter or COMMAND
char morse_SEEN_TOKENS[MORSE_TOKEN_MAX];
float morse_token_duration[MORSE_TOKEN_MAX];	// debugging and statistics

/* I put these in pp #define macros
   as i am not sure about enum and IRAM_ATTR
*/
#define MORSE_TOKEN_nothing	'0'
#define MORSE_TOKEN_dot		'.'

#define MORSE_TOKEN_dash	'-'
//#define MORSE_TOKEN_dash	'_'	// on *your* terminal one *or* the other might look better...

#define MORSE_TOKEN_loong	'!'	// starts a COMMAND sequence
#define MORSE_TOKEN_overlong	'V'

#define MORSE_TOKEN_debounce		'°'
#define MORSE_TOKEN_separeToken		'\''
#define MORSE_TOKEN_separeLetter	' '
#define MORSE_TOKEN_separeWord		'|'

#define MORSE_TOKEN_separe_OTHER	'?'
// not used any more:
//  #define MORSE_TOKEN_OFF		'o'
//  #define MORSE_TOKEN_ON		'°'

#define MORSE_TOKEN_ILLEGAL		ILLEGAL8	// was: '§' multichar

bool is_real_token(char token) {	// check for real tokens like . - ! V and separation (word, letter)
  switch(token) {			//	reject all debugging and debounce stuff
  case MORSE_TOKEN_dot:
  case MORSE_TOKEN_dash:
  case MORSE_TOKEN_loong:
  case MORSE_TOKEN_overlong:
  case MORSE_TOKEN_separeLetter:
  case MORSE_TOKEN_separeWord:
    return true;
  }

  return false;
}

// forward declaration:
  void morse_received_token(char token, float token_duration);

#if defined MORSE_COMPILE_HELPERS	// *can* be left out...
void morse_show_tokens(bool show_all = false) {	// set show_all=true  for token debug output
  char token;
  bool last_was_separeLetter=false;	// normally we want to see *only the first space* of a sequence

  if(morse_token_cnt) {
    for (int i=0; i<morse_token_cnt; i++) {
      switch (token=morse_SEEN_TOKENS[i]) {
      case MORSE_TOKEN_dot:
      case MORSE_TOKEN_dash:
      case MORSE_TOKEN_loong:
      case MORSE_TOKEN_overlong:
      case MORSE_TOKEN_separeWord:
	MENU.out(token);	// known real tokens
	last_was_separeLetter=false;
	break;

      case MORSE_TOKEN_separeLetter:	// *only one* space separe letter
	if (!last_was_separeLetter)
	  MENU.out(token);	// space

	if(!show_all)
	  last_was_separeLetter=true;	// show_all *does show* sequences of spaces
	break;

      default:
	if(show_all) {
	  last_was_separeLetter=false;
	  MENU.out(token);	// show *all* tokens
	}
      }
    }
    // MENU.ln();
  }
}
#endif

uint8_t morse_expected_Tims(char token) {	// ATTENTION: returns 0 for unknown/unreal tokens
  uint8_t morse_Tims=0;

  switch(token) {				//	drop all debugging and debounce stuff
  case MORSE_TOKEN_dot:
    morse_Tims = dotTim;
    break;
  case MORSE_TOKEN_dash:
    morse_Tims = dashTim;
    break;
  case MORSE_TOKEN_loong:
    morse_Tims = loongTim;
    break;
  case MORSE_TOKEN_overlong:
    morse_Tims = overlongTim;
    break;
  case MORSE_TOKEN_separeWord:
    morse_Tims = separeWordTim;
    break;
  case MORSE_TOKEN_separeLetter:
    morse_Tims = separeLetterTim;
    break;
  }

  return morse_Tims;
}


#if defined DEBUG_MORSE_TOUCH_INTERRUPT // REMOVE: debug only ################
void morse_debug_token_info() {		// a *lot* of debug info...
  char token;

  if(morse_token_cnt) {
    for (int i=0; i<morse_token_cnt; i++) {
      token=morse_SEEN_TOKENS[i];
      MENU.out(token);	// show *all* tokens
      MENU.space();
      MENU.out(morse_token_duration[i], 4);
      if(is_real_token(token)) {
	  MENU.tab();
	  MENU.out(F("> "));
	  MENU.out(token);
	  MENU.tab();
	  MENU.out(morse_token_duration[i], 4);
	  if(morse_expected_Tims(token)) {
	    MENU.tab();
	    MENU.out(morse_token_duration[i] / morse_expected_Tims(token), 4);
	  }
      }
      MENU.ln();
    }
    MENU.outln(">>>done\n");
  }
} // morse_debug_token_info()
#endif	// DEBUG_MORSE_TOUCH_INTERRUPT


/* **************************************************************** */
// GPIO ISR code removed

/* **************************************************************** */
// touch ISR:	touch_morse_ISR()
void morse_stats_gather(char token, float duration);	// forwards declaration

#if defined MORSE_TOUCH_INPUT_PIN
#if defined TOUCH_ISR_VERSION_3	// currently used version, removed older implementations

// data structure to save morse touch events from touch_morse_ISR_v3():
typedef struct morse_seen_events_t {
  int type=0;		// 0 is UNTOUCHED	1 is ON
  unsigned long time=0;
} morse_seen_events_t;


#if ! defined MORSE_EVENTS_MAX
  #define MORSE_EVENTS_MAX	256			// TODO: TEST&TRIMM:
#endif
volatile morse_seen_events_t morse_events_cbuf[MORSE_EVENTS_MAX];
volatile unsigned int morse_events_write_i=0;		// morse_events_cbuf[i]  write index
unsigned int morse_events_read_i=0;			// morse_events_cbuf[i]  read index

volatile bool too_many_events=false;			// error flag set by touch_morse_ISR_v3()

unsigned long morse_letter_separation_expected=0L;	// flag or time of automatic letter separation
							//		after input stays idle, untouched

#if defined DEBUG_TREAT_MORSE_EVENTS_V3
void show_morse_event_buffer() {	// debugging only
  MENU.outln(F("\nmorse events circular buffer"));
  for(int i=0; i<MORSE_EVENTS_MAX; i++) {
    MENU.out(i);
    MENU.space();
    if(i<10)
      MENU.space();

    if(i==morse_events_write_i)
      MENU.out(F("<WP>"));
    else
      MENU.space(4);

    if(i==morse_events_read_i)
      MENU.out(F("<RP>"));
    else
      MENU.space(4);

    MENU.tab();
    switch(morse_events_cbuf[i].type) {
    case 0:
      MENU.out(F("released"));
      break;
    case 1:
      MENU.out(F("TOUCHED "));
      break;

    default:
      MENU.out(morse_events_cbuf[i].type);
      MENU.out(F("\t?"));
    }

    MENU.tab();
    MENU.outln(morse_events_cbuf[i].time);
  }
  MENU.ln();
} // show_morse_event_buffer()
#endif // DEBUG_TREAT_MORSE_EVENTS_V3

#if defined MORSE_OUTPUT_PIN
  #if defined TOKEN_LENGTH_FEEDBACK_PULSE
int morse_length_feedback_pulse_i = ILLEGAL32;

void deactivate_morse_feedback_d_pulse() {
  if(morse_length_feedback_pulse_i != ILLEGAL32) {
    PULSES.pulses[morse_length_feedback_pulse_i].flags &= ~ACTIVE;	 // sleep,
    PULSES.pulses[morse_length_feedback_pulse_i].flags |= DO_NOT_DELETE; // but stay there
    PULSES.pulses[morse_length_feedback_pulse_i].counter = 0;		 // do we need that?
    // PULSES.fix_global_next();	// hmm?
  } // else nothing to do
}

void morse_feedback_d(int pulse) {	// payload for morse duration feedback pulse
  portENTER_CRITICAL(&morse_MUX);	// do we need that?
  switch (PULSES.pulses[pulse].counter) {
  case 1: // wait for loong
    PULSES.pulses[pulse].period = (pulse_time_t) (limit_dash_loong * morse_TimeUnit);
    digitalWrite(HARDWARE.morse_output_pin, HIGH);
    break;
  case 2: // loong reached: blink
    digitalWrite(HARDWARE.morse_output_pin, LOW);
    PULSES.pulses[pulse].period = PULSES.simple_time(100000);
    break;
  case 3: //  wait for overlong
    digitalWrite(HARDWARE.morse_output_pin, HIGH);
    PULSES.pulses[pulse].period = PULSES.simple_time(((limit_loong_overlong - limit_dash_loong) * morse_TimeUnit) - 100000);
    break;
  case 4: // overlong: switch LED off
    digitalWrite(HARDWARE.morse_output_pin, LOW);
  default:
    deactivate_morse_feedback_d_pulse(); // sleep but stay ready for next press
  }
  portEXIT_CRITICAL(&morse_MUX);
} // morse_feedback_d(pulse) payload

void prepare_morse_feedback_d_pulse() {	// prepares, but does not start it
  if((morse_length_feedback_pulse_i==ILLEGAL32) || (PULSES.pulses[morse_length_feedback_pulse_i].flags==0)) {
    morse_length_feedback_pulse_i = PULSES.highest_available_pulse();
    if(morse_length_feedback_pulse_i == ILLEGAL32)	// no pulse available
      return;						//   just ignore
    PULSES.init_pulse(morse_length_feedback_pulse_i);
    PULSES.set_payload_with_pin(morse_length_feedback_pulse_i, morse_feedback_d, HARDWARE.morse_output_pin);  // pin not really used, hardcoded
    PULSES.pulses[morse_length_feedback_pulse_i].groups = g_AUXILIARY;
    PULSES.pulses[morse_length_feedback_pulse_i].flags |= DO_NOT_DELETE; // redundant
    deactivate_morse_feedback_d_pulse();
  }
} // prepare_morse_feedback_d_pulse()

void start_morse_feedback_d_pulse() {
  portENTER_CRITICAL(&morse_MUX);
  // deactivate_morse_feedback_d_pulse();	// safety net...
  if((morse_length_feedback_pulse_i==ILLEGAL32) || (PULSES.pulses[morse_length_feedback_pulse_i].flags==0)) {
    prepare_morse_feedback_d_pulse();
  }
  if(morse_length_feedback_pulse_i != ILLEGAL32) {
    PULSES.pulses[morse_length_feedback_pulse_i].next = PULSES.get_now();
    PULSES.pulses[morse_length_feedback_pulse_i].flags |= ACTIVE;
    PULSES.fix_global_next();
  } // else (no free pulse): just ignore
  portEXIT_CRITICAL(&morse_MUX);
} // start_morse_feedback_d_pulse()

  #else // no TOKEN_LENGTH_FEEDBACK_PULSE
    #if defined ESP32 && defined TOKEN_LENGTH_FEEDBACK_TASK	// experimental

#include <freertos/task.h>

TaskHandle_t morse_input_feedback_handle;

void morse_input_duration_feedback(void* dummy) {
  if(touchRead(HARDWARE.morse_touch_input_pin) < touch_threshold) {	// looks STILL TOUCHED
    vTaskDelay((TickType_t) (limit_dash_loong * morse_TimeUnit / 1000 / portTICK_PERIOD_MS));
    //vTaskDelay((TickType_t) (dashTim * morse_TimeUnit / 1000 / portTICK_PERIOD_MS));
    if(touchRead(HARDWARE.morse_touch_input_pin) < touch_threshold) {	// looks STILL TOUCHED
      digitalWrite(HARDWARE.morse_output_pin, LOW);
      vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
      if(touchRead(HARDWARE.morse_touch_input_pin) < touch_threshold) {	// looks STILL TOUCHED
	digitalWrite(HARDWARE.morse_output_pin, HIGH);
      }
    }
  }
  morse_input_feedback_handle = NULL;
  vTaskDelete(NULL);
}

void trigger_token_duration_feedback() {
  BaseType_t err = xTaskCreatePinnedToCore(morse_input_duration_feedback,	// function
					   "morse_duration",			// name
					   2000,				// stack size
					   NULL,				// task input parameter
					   0,					// task priority
					   &morse_input_feedback_handle,	// task handle
					   0);					// core 0
  if(err != pdPASS) {
    MENU.out(err);
    MENU.space();
    MENU.error_ln(F("morse input feedback"));
  }
}

// works better when inlined
// void IRAM_ATTR stop_token_duration_feedback() {	// (works better as inline)
//   if(morse_input_feedback_handle != NULL) {
//     vTaskDelete(morse_input_feedback_handle);
//     morse_input_feedback_handle = NULL;
//   }
// }
  #endif // TOKEN_LENGTH_FEEDBACK_TASK	// experimental
  #endif // kind of token length feedback
#endif // MORSE_OUTPUT_PIN


void IRAM_ATTR touch_morse_ISR_v3() {	// ISR for ESP32 touch sensor as morse input	*NEW VERSION 3*
  unsigned long now = micros();

  portENTER_CRITICAL_ISR(&morse_MUX);
  unsigned long next_index = morse_events_write_i + 1;
  next_index %= MORSE_EVENTS_MAX;

  if(next_index == morse_events_read_i) {		// buffer full?
    too_many_events = true;				//   ERROR too_many_events
    goto morse_isr_exit;				//	   return
  }

  morse_events_cbuf[morse_events_write_i].time = now;	// save time

  if(touchRead(HARDWARE.morse_touch_input_pin) < touch_threshold) {	// >>>>>>>>>>>>>>>> looks TOUCHED <<<<<<<<<<<<<<<<
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE);		// wait for touch release
    morse_events_cbuf[morse_events_write_i].type = 1 /*touched*/;
#if defined MORSE_OUTPUT_PIN
    digitalWrite(HARDWARE.morse_output_pin, HIGH);		// feedback: pin is TOUCHED, LED on
 #if defined TOKEN_LENGTH_FEEDBACK_PULSE
    start_morse_feedback_d_pulse();
 #elif defined TOKEN_LENGTH_FEEDBACK_TASK	// experimental
    // stop_token_duration_feedback();			// assert it is off	// works better as inline:
    if(morse_input_feedback_handle != NULL) {
      vTaskDelete(morse_input_feedback_handle);
      morse_input_feedback_handle = NULL;
    }
    trigger_token_duration_feedback();			// switch on
 #endif // TOKEN_LENGTH_FEEDBACK_TASK	// experimental
#endif // MORSE_OUTPUT_PIN

  } else {							// >>>>>>>>>>>>>>>> looks RELEASED <<<<<<<<<<<<<<<<
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);		// wait for next touch
    morse_events_cbuf[morse_events_write_i].type = 0 /*released*/;
#if defined MORSE_OUTPUT_PIN
    digitalWrite(HARDWARE.morse_output_pin, LOW);		// feedback: pin is RELEASED, LED off
  #if defined TOKEN_LENGTH_FEEDBACK_PULSE
    deactivate_morse_feedback_d_pulse();
  #elif defined TOKEN_LENGTH_FEEDBACK_TASK		// experimental rtos task
    // stop_token_duration_feedback();			// works better as inline:
    if(morse_input_feedback_handle != NULL)
      vTaskDelete(morse_input_feedback_handle);
    morse_input_feedback_handle = NULL;
  #endif // TOKEN_LENGTH_FEEDBACK_TASK			// experimental rtos task
#endif // MORSE_OUTPUT_PIN
  }
  morse_events_write_i++;
  morse_events_write_i %= MORSE_EVENTS_MAX;		// it's a ring buffer

 morse_isr_exit:
  portEXIT_CRITICAL_ISR(&morse_MUX);
} // touch_morse_ISR_v3()


bool check_and_treat_morse_events_v3() {	// polled from pulses.ino main loop()	*NEW VERSION 3*
  static int last_seen_type=ILLEGAL32;

  static unsigned long last_seen_touch_time=ILLEGAL32;
  static unsigned long last_seen_release_time=ILLEGAL32;

  float scaled_touch_duration = 0.0;
  float scaled_released_duration = 0.0;

  portENTER_CRITICAL(&morse_MUX);

  if(morse_events_read_i == morse_events_write_i) {
    bool retval=false;
    if(morse_letter_separation_expected) {
      bool morse_poll_letter_separation();  // pre declaration
      retval = morse_poll_letter_separation();
    }
    portEXIT_CRITICAL(&morse_MUX);
    return false;			// NO DATA, or no letter separation check needed
  }

  if(too_many_events) {			// ERROR buffer too small
    too_many_events = false;
    portEXIT_CRITICAL(&morse_MUX);

    MENU.error_ln(F("morse event buffer too small"));
    return true;			// give up (for this round)

  } else {	// (! too_many_events)		OK,  *NO ISR ERROR*
     // check if already seen same type?
    if(morse_events_cbuf[morse_events_read_i].type == last_seen_type) { // already seen SAME TYPE?
      MENU.out_Error_();					// strange ERROR, SAVETY NET, ignore
      MENU.out(F("skip repeated "));				// strange ERROR, SAVETY NET, ignore
      if(morse_events_cbuf[morse_events_read_i].type)
	MENU.outln(F("TOUCH"));
      else
	MENU.outln(F("RELEASE"));
      morse_events_read_i++;				// just ignore?
      morse_events_read_i %= MORSE_EVENTS_MAX;

      portEXIT_CRITICAL(&morse_MUX);
      return true;

    } else {		// NORMAL CASE:  >>>>>>>>>>>>>>>>  *NEW TYPE* was detected  <<<<<<<<<<<<<<<<

      last_seen_type = morse_events_cbuf[morse_events_read_i].type;

      morse_letter_separation_expected=0L;	// default, will be set if appropriate

      if(last_seen_type /* == new TOUCH */) {	//    >>>>>>>>>>>>>>>> seen TOUCHED now <<<<<<<<<<<<<<<<
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.out(F("TOUCH\t"));
#endif
	last_seen_touch_time = morse_events_cbuf[morse_events_read_i].time;
	scaled_released_duration = (float) (last_seen_touch_time - last_seen_release_time) / morse_TimeUnit;
	if(scaled_released_duration <= 0.0) {	// SAVETY NET, should not happen
	  MENU.out_Error_();
	  MENU.out(F("TODO:  negative scaled_released_duration "));
	  MENU.outln(scaled_released_duration);
	} // TODO: *does* this ever happen?

	if (scaled_released_duration >= separeWordTim) {		// word end?
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.outln(F("word end"));
#endif
	  morse_received_token(MORSE_TOKEN_separeLetter, scaled_released_duration);	// " |" is word end
	  morse_received_token(MORSE_TOKEN_separeWord, scaled_released_duration);

	} else if (scaled_released_duration >= separeLetterTim) {	// letter end?
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.outln(F("letter end by touch"));
#endif
	  morse_received_token(MORSE_TOKEN_separeLetter, scaled_released_duration);

	} else if (scaled_released_duration <= limit_debounce) {	// ignore debounce
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	  MENU.outln(F("DEBOUNCE ignored"));
#endif
	  ;
	} else  {							// token end
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	  MENU.outln(F("token end"));
#endif
	  morse_stats_gather(MORSE_TOKEN_separeToken, scaled_released_duration);
	}

      } else { /* new RELEASE */	//    >>>>>>>>>>>>>>>> seen RELEASED now <<<<<<<<<<<<<<<<
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.out(F("RELEASE\t"));
#endif
	last_seen_release_time = morse_events_cbuf[morse_events_read_i].time;
	scaled_touch_duration = (float) (last_seen_release_time - last_seen_touch_time) / morse_TimeUnit;
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.out(F("scaled_touch_duration\t"));
	MENU.outln(scaled_touch_duration);
#endif
	if(scaled_touch_duration <= 0.0) {		// SAVETY NET, should not happen
	  MENU.out_Error_();
	  MENU.out(F("TODO:  negative touch duration "));
	  MENU.outln(scaled_touch_duration);
	} // TODO: *does* this ever happen?

	if (scaled_touch_duration > limit_debounce) {		// *REAL INPUT* no debounce
	  if(scaled_touch_duration > limit_loong_overlong) {
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	    MENU.outln(F("overlong"));
#endif
	    morse_received_token(MORSE_TOKEN_overlong, scaled_touch_duration);

	  } else if (scaled_touch_duration > limit_dash_loong) {
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	    MENU.outln(F("loong"));
#endif
	    morse_received_token(MORSE_TOKEN_loong, scaled_touch_duration);

	  } else if (scaled_touch_duration > limit_dot_dash) {
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	    MENU.outln('-');
#endif
	    morse_received_token(MORSE_TOKEN_dash, scaled_touch_duration);

	  } else {
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	    MENU.outln('.');
#endif
	    morse_received_token(MORSE_TOKEN_dot, scaled_touch_duration);
	  }
	} // real input?
	else {						// DEBOUNCE, ignore
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	  MENU.outln(F("DEBOUNCE ignored"));
#endif
	  ;
	} // debounce
      } // touched | RELEASED ?

      morse_events_read_i++;
      morse_events_read_i %= MORSE_EVENTS_MAX;

      if(morse_events_read_i == morse_events_write_i) {	// waiting untouched, if no input follows it would miss separeLetter
	morse_letter_separation_expected = last_seen_release_time + (separeLetterTim * morse_TimeUnit)*3; // wait 3* as long...
											// TODO: TEST&TRIMM: wait 3* as long...
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
	MENU.outln(F("waiting for letter separation"));
#endif
      } // set morse_letter_separation_expected ?
    } // NEW or repeated type?
  } // ! (too_many_events) ISR error?

  portEXIT_CRITICAL(&morse_MUX);
  return true;
} // check_and_treat_morse_events_v3()


bool morse_poll_letter_separation() {
  if(morse_letter_separation_expected == 0L)		// we *might* miss one at a 0, i don't mind ;)
    return false;	// nothing to do, return
  // else

  unsigned long now = micros();
  signed long diff = now - morse_letter_separation_expected;
  if(diff < 0)		// not time yet
    return false;	//   do *not* block while polling
  else {		// it *is* time
    morse_received_token(MORSE_TOKEN_separeLetter, separeLetterTim /*that's a fake!*/);
    morse_letter_separation_expected = 0L;
#if defined DEBUG_TREAT_MORSE_EVENTS_V3
    MENU.outln(F("\nautomatic letter separation"));
#endif
    return true;
  }
} // morse_poll_letter_separation()  TOUCH_ISR_VERSION_3
//  #elif defined TOUCH_ISR_VERSION_2	// REMOVED, see: morse_unused.txt
#endif // TOUCH_ISR_VERSION

#endif // MORSE_TOUCH_INPUT_PIN
/* **************************************************************** */


#if ! defined MORSE_OUTPUT_MAX
  #define MORSE_OUTPUT_MAX	64
#endif
char morse_OUTPUT_buffer[MORSE_OUTPUT_MAX];
short morse_OUTPUT_cnt=0;

void morse_2output_buffer(char letter) {
  if (morse_OUTPUT_cnt < MORSE_OUTPUT_MAX) {
    morse_OUTPUT_buffer[morse_OUTPUT_cnt++] = letter;
  } else {
    MENU.error_ln(F("MORSE_OUTPUT_MAX	output skipped"));
    morse_OUTPUT_cnt=0;	// TODO: maybe output data first? maybe not...
  }

  morse_OUTPUT_buffer[morse_OUTPUT_cnt] = '\0';	// c style string end
}

void show_morse_output_buffer() {
  if (morse_OUTPUT_cnt) {
    MENU.ln();
    for (int i=0; i<morse_OUTPUT_cnt; i++)
      MENU.out(morse_OUTPUT_buffer[i]);
    MENU.ln();
  }
}

void morse_store_as_letter(char token) {	// translate special cases and store as letters		TODO: obsolete?
  switch (token) {
  case ILLEGAL8:
    morse_2output_buffer(MORSE_TOKEN_ILLEGAL);
  case '\0':
    return;
    break;

  case MORSE_TOKEN_separeWord:	// separe words by a space
    morse_2output_buffer(' ');
    break;

  // translate special meanings like 'c' 'k' ...	and save
  case 'c':	// CH
    morse_2output_buffer('|');
    morse_2output_buffer('C');
    morse_2output_buffer('H');
    morse_2output_buffer('|');
    break;

  case 'v':	// VE	verstanden
    morse_2output_buffer('|');
    morse_2output_buffer('V');
    morse_2output_buffer('E');
    morse_2output_buffer('|');
    break;

  case 'k':	// KA	Spruchanfang
    morse_2output_buffer('|');
    morse_2output_buffer('K');
    morse_2output_buffer('A');
    morse_2output_buffer('|');
    break;

  case 'a':	// AR	Spruchende
    morse_2output_buffer('|');
    morse_2output_buffer('A');
    morse_2output_buffer('R');
    morse_2output_buffer('|');
    break;

  case 'b':	// BT	Pause
    morse_2output_buffer('|');
    morse_2output_buffer('B');
    morse_2output_buffer('T');
    morse_2output_buffer('|');
    break;

  default:
    morse_2output_buffer(token);
    break;
  }
} // morse_store_as_letter(char token)



/* **************************************************************** */
// morse token duration statistics
uint16_t morse_stats_dot_cnt=0;			// sample count dot
float morse_stats_dot_duration_=0.0;		// summing up dot durations
uint16_t morse_stats_dash_cnt=0;		// sample count dash
float morse_stats_dash_duration_=0.0;		// summing up dash durations
uint16_t morse_stats_loong_cnt=0;		// sample count loong
float morse_stats_loong_duration_=0.0;		// summing up loong durations
uint16_t morse_stats_separeToken_cnt=0;		// sample count separeToken
float morse_stats_separeToken_duration_=0.0;	// summing up separeToken durations
uint16_t morse_stats_separeLetter_cnt=0;	// sample count separeLetter
float morse_stats_separeLetter_duration_=0.0;	// summing up separeLetter durations
// uint16_t morse_stats_separeWord_cnt=0;	// sample count separeWord
// float morse_stats_separeWord_duration_=0.0;	// summing up separeWord durations

// auto adapt
float morse_stats_mean_dash_factor=1.0;		// last letters mean dash duration factor
						//	(also used as flag for display)

void morse_stats_init() {
  morse_stats_dot_cnt = 0;
  morse_stats_dot_duration_ = 0.0;
  morse_stats_dash_cnt = 0;
  morse_stats_dash_duration_ = 0.0;
  morse_stats_loong_cnt = 0;
  morse_stats_loong_duration_= 0.0;
  morse_stats_separeToken_cnt = 0;
  morse_stats_separeToken_duration_ = 0.0;
  morse_stats_separeLetter_cnt = 0;
  morse_stats_separeLetter_duration_ = 0.0;
  // morse_stats_separeWord_cnt = 0;
  // morse_stats_separeWord_duration_ = 0.0;
}


// TODO: comment ################
void morse_stats_gather(char token, float duration) {	// only real tokens please
  switch (token) {
  case MORSE_TOKEN_dot:
    morse_stats_dot_duration_ += duration;
    morse_stats_dot_cnt++;
    break;
  case MORSE_TOKEN_dash:
    morse_stats_dash_duration_ += duration;
    morse_stats_dash_cnt++;
    break;
  case MORSE_TOKEN_loong:
    morse_stats_loong_duration_ += duration;
    morse_stats_loong_cnt++;
    break;
  case MORSE_TOKEN_separeToken:
    morse_stats_separeToken_duration_ += duration;
    morse_stats_separeToken_cnt++;
    break;
  case MORSE_TOKEN_separeLetter:
    morse_stats_separeLetter_duration_ += duration;
    morse_stats_separeLetter_cnt++;
    break;
  }
} // morse_stats_gather()


float morse_saved_stats_dot=0.0;
float morse_saved_stats_dash=0.0;
float morse_saved_stats_loong=0.0;
float morse_saved_stats_separeToken=0.0;
float morse_saved_stats_separeLetter=0.0;
// float morse_saved_stats_separeWord=0.0;


unsigned short morse_stat_ID=0;		// flag (counter)	// flag (counter) for display control
unsigned short morse_stat_seen_ID=0;	// flag (counter)	// flag (counter) for display control

void static IRAM_ATTR morse_stats_save() {	// call immediately after morse_stats_do() to save statistics
  morse_stat_ID++;	// display flag (counter)

  if(morse_stats_dot_cnt)
    morse_saved_stats_dot = morse_stats_dot_duration_;

  if(morse_stats_dash_cnt)
    morse_saved_stats_dash = morse_stats_dash_duration_;

  if(morse_stats_loong_cnt)
    morse_saved_stats_loong = morse_stats_loong_duration_;

  if(morse_stats_separeToken_cnt)
    morse_saved_stats_separeToken = morse_stats_separeToken_duration_;

  if(morse_stats_separeLetter_cnt)
    morse_saved_stats_separeLetter = morse_stats_separeLetter_duration_;

//  if(morse_stats_separeWord_cnt)
//    morse_saved_stats_separeWord = morse_stats_separeWord_duration_;
} // morse_stats_save()

void morse_show_saved_stats() {
  morse_stat_seen_ID = morse_stat_ID;	// flag (counter) for display control

  MENU.outln(F("morse duration stats"));

  if(morse_saved_stats_dot) {
    MENU.out(F("dot\t\t"));
    MENU.outln(morse_saved_stats_dot);
  }
  if(morse_saved_stats_dash) {
    MENU.out(F("dash\t\t"));
    MENU.outln(morse_saved_stats_dash / dashTim);
  }
  if(morse_saved_stats_loong) {
    MENU.out(F("loong\t\t"));
    MENU.outln(morse_saved_stats_loong / loongTim);
  }
  if(morse_saved_stats_separeToken) {
    MENU.out(F("separeToken\t"));
    MENU.outln(morse_saved_stats_separeToken / separeTokenTim);
  }
  if(morse_saved_stats_separeLetter) {
    MENU.out(F("separeLetter\t\t"));
    MENU.outln(morse_saved_stats_separeLetter / separeLetterTim);
  }
//  if(morse_saved_stats_separeWord) {
//    MENU.out(F("separeWord\t\t"));
//    MENU.outln(morse_saved_stats_separeWord / separeWordTim);
//  }

  MENU.out(F("mean dash factor "));
  MENU.out(morse_stats_mean_dash_factor);
  MENU.tab();
  MENU.out(F("morse_TimeUnit "));
  MENU.outln(morse_TimeUnit);
} // morse_show_saved_stats()

//#define DEBUG_MORSE_STATS_DO	// ATTENTION can be called from interrupt...
void static IRAM_ATTR morse_stats_do() {
#if defined DEBUG_MORSE_STATS_DO
  MENU.ln();
#endif

  if(morse_stats_dot_cnt) {
    morse_stats_dot_duration_ /= morse_stats_dot_cnt;
#if defined DEBUG_MORSE_STATS_DO
  MENU.out(F("dot\t\t% "));
  MENU.outln(morse_stats_dot_duration_ / dotTim);
#endif
  }

  if(morse_stats_dash_cnt) {
    morse_stats_dash_duration_ /= morse_stats_dash_cnt;
#if defined DEBUG_MORSE_STATS_DO
    MENU.out(F("dash\t\t% "));
    MENU.outln(morse_stats_dash_duration_ / dashTim);
#endif

    // auto adapt, 1st simple try: only look at dashs
    morse_stats_mean_dash_factor = morse_stats_dash_duration_ / dashTim; // save for auto speed adaption
  }

  if(morse_stats_loong_cnt) {
    morse_stats_loong_duration_ /= morse_stats_loong_cnt;
#if defined DEBUG_MORSE_STATS_DO
    MENU.out(F("loong\t\t% "));
    MENU.outln(morse_stats_loong_duration_ / loongTim);
#endif
  }

  if(morse_stats_separeToken_cnt) {
    morse_stats_separeToken_duration_ /= morse_stats_separeToken_cnt;
#if defined DEBUG_MORSE_STATS_DO
    MENU.out(F("separeToken\t% "));
    MENU.outln(morse_stats_dash_duration_ / separeTokenTim);
#endif
  }

  if(morse_stats_separeLetter_cnt) {
    morse_stats_separeLetter_duration_ /= morse_stats_separeLetter_cnt;
#if defined DEBUG_MORSE_STATS_DO
    MENU.out(F("separeLetter\t% "));
    MENU.outln(morse_stats_dash_duration_ / separeLetterTim);
#endif
  }

#if defined DEBUG_MORSE_STATS_DO
//  if(morse_stats_separeWord_cnt) {
//    morse_stats_separeWord_duration_ /= morse_stats_separeWord_cnt;
//    MENU.out(F("separeWord\t% "));
//    MENU.outln(morse_stats_dash_duration_ / separeWordTim);
//  }
#endif

  morse_stats_save();		// called from morse_stats_do()
} // morse_stats_do()

/* **************************************************************** */
void static morse_token_decode();	// pre declaration

void morse_received_token(char token, float duration) {
  portENTER_CRITICAL(&morse_MUX);

#if defined MORSE_DEBUG_RECEIVE_TOKEN
  //MENU.out("morse_received_token() "); MENU.outln(token);
#endif

  if(morse_token_cnt < MORSE_TOKEN_MAX) {	// buffer not full?
    morse_token_duration[morse_token_cnt] = duration;	// SAVE TOKEN and duration
    morse_SEEN_TOKENS[morse_token_cnt++] = token;

    if(is_real_token(token)) {	// react on REAL morse tokens
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      //MENU.out("morse real token "); MENU.outln(token);
#endif
      switch (token) {		// save letters?, do statistics?
      case MORSE_TOKEN_separeLetter:
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      MENU.outln("MORSE_TOKEN_separeLetter");
#endif
	if(morse_token_cnt == 1) {	// ignore separation tokens on startup
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      MENU.outln("skip 1st");
#endif
	  --morse_token_cnt;		//   remove 1st from buffer
	} else {			// MORSE_TOKEN_separeLetter  ok
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      MENU.outln("statistics");
#endif
	  morse_stats_do();	// do statistics on received letter
	  morse_stats_init();	// prepare stats for next run

	  morse_token_decode();
	}
	break;

      case MORSE_TOKEN_separeWord:
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      MENU.outln("MORSE_TOKEN_separeWord");
#endif
	if(morse_token_cnt == 1) {	// ignore separation tokens on startup
#if defined MORSE_DEBUG_RECEIVE_TOKEN
      MENU.outln("skip 1st");
#endif
	  --morse_token_cnt;		//   remove from buffer
	} else {
	  //	morse_save_symbol(token);
	}
	break;

      default:
	morse_stats_gather(token, duration);
      }
    }
#if defined MORSE_DEBUG_RECEIVE_TOKEN
    else {
      MENU.out("morse unreal token ");
      MENU.outln(token);
    }
#endif
  } else {
    MENU.error_ln(F("MORSE_TOKEN_MAX	buffer cleared"));
    morse_token_cnt=0;	// TODO: maybe still use data or use a ring buffer?
  }

  portEXIT_CRITICAL(&morse_MUX);
} // morse_received_token()


//%/* **************************************************************** */
//%// morse output, i.e. on a piezzo, led
//%
//%// declarations to be defined later
//%void morse_out_dot();
//%void morse_out_dash();
//%void morse_out_loong();
//%void morse_out_overlong();
//%void morse_out_separeLetter();
//%void morse_out_separeWord();
//%
//%#if defined MORSE_OUTPUT_PIN
//%void morse_play_out_tokens(bool show=true) {	// play (and show) saved tokens in current morse speed
//%  char token;
//%  for(int i=0 ; i < morse_token_cnt; i++) {
//%    token = morse_SEEN_TOKENS[i];
//%    switch (token) {
//%    case MORSE_TOKEN_dot:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_dot();
//%      break;
//%    case MORSE_TOKEN_dash:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_dash();
//%      break;
//%    case MORSE_TOKEN_loong:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_loong();
//%      break;
//%    case MORSE_TOKEN_overlong:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_overlong();
//%      break;
//%    case MORSE_TOKEN_separeLetter:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_separeLetter();
//%      break;
//%    case MORSE_TOKEN_separeWord:
//%      if(show)
//%	MENU.out(token);	// show *before* sound
//%      morse_out_separeWord();
//%      break;
//%    }
//%  }
//%  if(show)
//%    MENU.ln();
//%}
//%#endif // MORSE_OUTPUT_PIN

/* **************************************************************** */
// 2nd implementation: human readable look up table

// morse code see i.e.
// https://www.electronics-notes.com/articles/ham_radio/morse_code/characters-table-chart.php


// the table *is sorted* for quick lookup

// common fixed place entries:
// *		number of tokens 1-9 A-F
//  *		delimiter ' '
//   *		type: '*' is LETTER
//                    'C' is COMMAND
//                    '0' undefined
//    *		delimiter ' '
//     *	first token
//      *	next token until space as delimiter)

// LETTERS:	if type=='*' it continues like this:
//       *	first UPPERCASE letter or compound string, *can* be ' '
//        ****	' ' delimiter or more chars from UPPERCASE COMPOUND string
//            *	first LOWERCASE letter or compound string, *can* be ' '
//             ****	' ' delimiter or more chars from LOWERCASE COMPOUND string

//                 *	delimiter or string end
//                  *<NAME>" comment or extensions

// COMMAND:
// *		number of tokens 1-9 A-F
//  *		delimiter
//  'C'		type: 'C' is COMMAND
//     *...	tokens
//	   *	delimiter
//          *<COMMAND>"
//		      "	STRING END,
//                    " delimiter ' ' is *not* enough here

const char * morse_definitions_tab[] = {
  "1 * . E e",
  "1 * - T t",

  "1 C ! NEXT",		// SEND if morse input,  else  START/STOP playing
  "1 C V CANCEL",

  "2 * .. I i",
  "2 * .- A a",
  "2 * -. N n",
  "2 * -- M m",

  "2 C !. LOWER",
  "2 C !- UPPER",
  "2 C !! DELLAST",

  "3 * ... S s",
  "3 * ..- U u",
  "3 * .-. R r",
  "3 * .-- W w",
  "3 * -.. D d",
  "3 * -.- K k",
  "3 * --. G g",
  "3 * --- O o",

  "4 * .... H h",
  "4 * ...- V v",
  "4 * ..-. F f",
  "4 * ..-- Ü ü",
  "4 * .-.. L l",
  "4 * .-.- Ä ä",
  "4 * .--. P p",
  "4 * .--- J j",
  "4 * -... B b",
  "4 * -..- X x",
  "4 * -.-. C c",
  "4 * -.-- Y y",
  "4 * --.. Z z",
  "4 * --.- Q q",
  "4 * ---. Ö ö",
  "4 C ---- ANY1",	// was: |CH| |ch|

  "5 * ..... 5 5",
  "5 * ....- 4 4",
  "5 C ...-. MACRO_NOW",	// SN
  "5 * ...-- 3 3",
//"5 * ..-.. É É",	// was: É	FIX: lowercase
  "5 C ..-.. UIswM",	// switch Motion UI activity
//"5 0 ..-.- 5 5",	// ..K  IK  UA  FT EÄ
  "5 * ..--. - -",	// ???
  "5 * ..--- 2 2",
  "5 * .-... & &",
  "5 * .-..- È È",	// È	FIX: lowercase
  "5 * .-.-. + +",
//"5 0 .-.-- 5 5",
//"5 0 .--.. 5 5",
  "5 * .--.- Å Å",	// Å	FIX: lowercase
  "5 * .---. - -",	// ???
  "5 * .---- 1 1",
  "5 * -.... 6 6",
  "5 * -...- = =",
  "5 * -..-. / /",
  "5 C -..-- UPPER",	// NW	_..__	UPPERCASE
  "5 C -.-.. LOWER",	// ND	-.-..	LOWERCASE
//"5 C -.-.- SEND",	// NK	-.-.-	SEND	obsolete, replaced by 'NEXT'
  "5 * -.--. ( (",
  "5 C -.--- DELLAST",	// NO -.---	DELETE LAST LETTER
  "5 * --... 7 7",
//"5 0 --..- 5 5",	// FREE M	MU	__.._ 3 M- COMMAND triggers	free :)  rhythm sample? tap  *HOW TO GET OUT?*
//"5 0 --.-. 5 5",	// FREE M	MR	--.-. 3 M- COMMAND triggers	free :)  rhythm replay
//"5 * --.-- Ñ Ñ",	// Ñ
  "5 * --.-- * *",	// my private code for '*'	// was: Ñ
  "5 0 ---.- OLED",	// OA	---.-	OLED	toggle oled display
  "5 * ---.. 8 8",
  "5 * ----. 9 9",
  "5 C ---.- DELWORD",	// MK	---.-	DELETE WORD
  "5 * ----- 0 0",


//"6 0 ...... TODO",	// FREE
//"6 0 ....._ TODO",	// FREE
//"6 0 ...._. TODO",	// FREE
//"6 0 ....__ TODO",	// FREE
//"6 0 ...-.. TODO",	// FREE		(maybe morse speed calibration?)
//"6 0 ...-.- TODO",	// FREE		(maybe morse speed calibration?)
//"6 0 ...__. TODO",	// FREE
//"6 C ...--- (was: OLED)",	// FREE	  was: OLED toggle oled display while playing
//"6 0 ...... TODO",	// FREE
//"6 0 ...... TODO",	// FREE

  "6 * ..--.. ? ?",
  "6 * .-..-. \" \"",
  "6 * .----. ' '",
  "6 * .-.-.- . .",
  "6 * .--.-. @ @",
  "6 * -....- _ _",	// check - _
  "6 * -.-.-. ; ;",
  "6 * -.-.-- ! !",
  "6 * --..-- , ,",
  "6 * ---... : :",

  "7 * ...-..- $ $",
//"7 0 ...--.. X X TODO:",		// beta ss	TODO: FIX:

  "8 C ........ MISTAKE",		// MISTAKE
};
#define MORSE_DEFINITIONS	(sizeof (morse_definitions_tab) / sizeof(const char *))


uint8_t morse_def_token_cnt=0;
char morse_def_TYPE=ILLEGAL8;
string * morse_def_TOKENS;	// TODO: fix or remove ################
string * morse_def_UPPER_Letter;
string * morse_def_LOWER_Letter;
string * morse_def_Letter_in_CASE;
string * morse_def_COMMAND;

#define morse_def_TOKENS_LEN	16	// up to 15 tokens for morse pattern strings
#define morse_def_LETTER_LEN	8	// up to 7  chars for compound morse "letter" strings
#define morse_def_COMMAND_LEN	12	// up to 11 chars for morse command names


// show the words of a space separated string
int space_separated_string_WORDS(string * source) {	/* for debugging only */
  int cnt=0, size=0, pos=0;
  string word;
  while (size >= 0) {
    size = source->find(' ', pos);
    word = source->substr(pos, size - pos);
    MENU.outln(word.c_str());
    pos = size;
    pos++;
    cnt++;
  }

  MENU.outln(cnt);
  return cnt;
}

// TODO: comments ################################################################
string morse_DEFINITION;
string morse_DEFINITION_TOKENS;
char morse_PRESENT_UPPER_Letter;
char morse_PRESENT_LOWER_Letter;
char morse_PRESENT_in_case_Letter;
char morse_PRESENT_TYPE='\0';
string morse_PRESENT_COMMAND;

uint8_t morse_seen_TOKENS=0;
uint8_t morse_SYMBOLS_cnt=0;

void morse_show_definition() {
  if(morse_DEFINITION.length()) {
    MENU.out(morse_PRESENT_TYPE);
    MENU.space();
    MENU.out(morse_DEFINITION_TOKENS.c_str());
    MENU.space();
    switch (morse_PRESENT_TYPE) {
    case '*':
      MENU.out(morse_PRESENT_UPPER_Letter);
      MENU.space();
      MENU.out(morse_PRESENT_LOWER_Letter);
      MENU.space();
      if(morse_PRESENT_in_case_Letter)
	MENU.out(morse_PRESENT_in_case_Letter);
      break;

    case 'C':
      MENU.outln(morse_PRESENT_COMMAND.c_str());	// TODO: do I still need .c_str() ??? ################################
      break;
    }
  }
}


void morse_reset_definition(string source) {
  morse_DEFINITION = source;

  morse_DEFINITION_TOKENS="";
  morse_PRESENT_UPPER_Letter='\0';
  morse_PRESENT_LOWER_Letter='\0';
  morse_PRESENT_in_case_Letter='\0';
  morse_PRESENT_TYPE='\0';
  morse_PRESENT_COMMAND="";

  morse_seen_TOKENS=0;
  morse_SYMBOLS_cnt=0;
}

void morse_read_definition(int index) {	// keep index in range, please
  int cnt=0, size=0, pos=0;
  string dataset, word;
  dataset = morse_definitions_tab[index];
  morse_reset_definition(dataset);

  while (size >= 0) {
    size = dataset.find(' ', pos);
    word = dataset.substr(pos, size - pos);
    pos = size;
    pos++;

    switch (cnt) {
    case 0:
      morse_SYMBOLS_cnt = word[0] - '0';
      break;
    case 1:
      morse_PRESENT_TYPE = word[0];
      break;
    case 2:
      morse_DEFINITION_TOKENS = word;
      break;
    case 3:
      if(morse_PRESENT_TYPE=='*') {		// item 3 LETTER mode	= UPPER
	morse_PRESENT_UPPER_Letter = word[0];
      } else if(morse_PRESENT_TYPE=='C') {	// item 3 COMMAND mode	= COMMAND  (last command item)
	morse_PRESENT_COMMAND = word;
	size = -1;	// command read, done
      }
      break;
    case 4:
      if(morse_PRESENT_TYPE=='*') {		// item 4 LETTER mode	= LOWER  (last letter item)
	morse_PRESENT_LOWER_Letter = word[0];
	size = -1;	// letter read, done
      }
      // else  (no check here)
      break;
    }

    if(morse_PRESENT_TYPE=='*') {		// upper | lowercase letters
      if(morse_uppercase)
	morse_PRESENT_in_case_Letter = morse_PRESENT_UPPER_Letter;
      else
	morse_PRESENT_in_case_Letter = morse_PRESENT_LOWER_Letter;
    }

    cnt++;
  }
}

// probably only used for debugging
void morse_show_space_delimited_string(const char *p) {
  if(p==NULL)
    return;

  char c;
  for(int i=0; (c = p[i]) ; i++) {
    MENU.out(c);
    if(c == ' ')	// *does* show delimiting space
      break;
  }
}

// probably only used for debugging
void morse_definition_set_show(bool uppercase) {
  MENU.ln();
  MENU.out(morse_def_token_cnt);
  MENU.space();
  MENU.out(morse_def_TYPE);
  MENU.space();

  // morse_show_space_delimited_string(morse_def_TOKENS);
  MENU.out(morse_def_TOKENS->c_str());
  MENU.space();

  switch(morse_def_TYPE) {
  case '*':	// letters
    //    morse_show_space_delimited_string(morse_def_Letter_in_CASE);
    MENU.out(morse_def_Letter_in_CASE->c_str());
    break;	// TODO: inserted break, is it ok?
  case 'C':	// COMMANDs
    morse_show_space_delimited_string(morse_def_COMMAND->c_str());
    break;
  }

  MENU.ln();
}


int morse_find_definition(const char* pattern) {  // returns index in morse_definitions_tab[i] or ILLEGAL32
  for(int i=0; i<MORSE_DEFINITIONS ; i++) {
    morse_read_definition(i);
    if((string) pattern == morse_DEFINITION_TOKENS) {
#if defined MORSE_DECODE_DEBUG
      MENU.out("== ");
      MENU.outln(pattern);
      morse_show_definition();
#endif
      return i;
    }
  }

  morse_reset_definition("");
  return ILLEGAL32;	// not found
} // morse_find_definition()

char morse_2ACTION() {
/*
  SAVE LETTERS,
  return CHAR for letters or ILLEGAL8 for unknown patterns

  EXECUTE COMMANDS,
  return of commands is currently unused, always 0
*/
  for(int i=0; i < MORSE_DEFINITIONS; i++) {
    morse_read_definition(i);

    switch (morse_def_TYPE) {
    case '*': // LETTER
      morse_show_space_delimited_string(morse_def_Letter_in_CASE->c_str());
      MENU.tab();
      MENU.outln(F("LETTER action"));
      break;

    case 'C': // COMMAND
      morse_show_space_delimited_string(morse_def_COMMAND->c_str());
      MENU.tab();
      MENU.outln("COMMAND action");

      if(strcmp(morse_def_COMMAND->c_str(), "UPPER") == 0) {
	morse_uppercase = true;
      } else if(strcmp(morse_def_COMMAND->c_str(), "LOWER") == 0) {
	morse_uppercase = false;
      } else if(strcmp(morse_def_COMMAND->c_str(), "NEXT") == 0) {
	MENU.out(">>>> NEXT\t");
	//MENU.outln(morse_OUTPUT_buffer);
	//MENU.play_KB_macro(morse_OUTPUT_buffer);
      } else if(strcmp(morse_def_COMMAND->c_str(), "DELLAST") == 0) {
	MENU.outln(">>>> DELLAST");
      } else if(strcmp(morse_def_COMMAND->c_str(), "DELWORD") == 0) {
	MENU.outln(">>>> DELLWORD");

      } else if(strcmp(morse_def_COMMAND->c_str(), "MISTAKE") == 0) {
	MENU.outln(">>>> MISTAKE");
      }

      break;

    default:
      MENU.error_ln(F("morse_2ACTION"));
    }
  }


  return ILLEGAL8;
} // morse_2ACTION()

/* **************************************************************** */

#ifndef MORSE_OUTPUT_BUFFER_SIZE
  #define MORSE_OUTPUT_BUFFER_SIZE	64	// size of morse output buffer
#endif
char  morse_output_buffer[MORSE_OUTPUT_BUFFER_SIZE];	// buffer
short morse_out_buffer_cnt=0;


bool morse_output_to_do=false;		// triggers morse_do_output()
extern bool musicbox_is_idle();

#if defined HAS_OLED
extern bool monochrome_can_be_used();	// TODO: do i want that *here*?
extern void MC_display_message(const char * message);
// avoid sound glitches when using OLED:
extern bool oled_feedback_while_playing;
extern void MC_printlnBIG(uint8_t row, const char* str);		// 2x2 lines
extern void MC_setInverseFont();
extern void MC_clearInverseFont();
extern void monochrome_setPowerSave(uint8_t value);
extern void MC_setCursor(uint8_t col, uint8_t row);
extern uint8_t monochrome_getCols();
extern void MC_print(const char* str);
extern void monochrome_print_f(float f, int chiffres);
extern void MC_printBIG_at(uint8_t col, uint8_t row, const char* str);	// for short 2x2 strings
extern void monochrome_clear();
#endif // HAS_OLED

void morse_do_output() {
  morse_output_buffer[morse_out_buffer_cnt]='\0';	// append '\0'
  if(morse_out_buffer_cnt) {
#if defined HAS_ePaper
    xSemaphoreTake(MC_mux, portMAX_DELAY);	// TODO: could delay application...
    set_used_font(&FreeMonoBold12pt7b);
    xSemaphoreGive(MC_mux);

    MC_print_1line_at(MORSE_MONOCHROME_ROW, "");
#elif defined HAS_OLED
    MC_printlnBIG(MORSE_MONOCHROME_ROW, "        ");
#endif

    MENU.out(F("morse "));
    MENU.play_KB_macro((char*) morse_output_buffer);	// *does* the menu feedack
  }

  morse_out_buffer_cnt=0;
  morse_output_to_do=false;

  morse_uppercase = true;	// reset to uppercase
} // morse_do_output()

char morse_output_char = '\0';	// '\0' means *no* output

#if defined HAS_DISPLAY
void monochrome_out_morse_char() {
 #if defined HAS_OLED
  if(! monochrome_can_be_used())
    return;
 #endif

  if(morse_output_char && morse_out_buffer_cnt) {
    // 2x2 version
    char s[]="  ";
    s[0] = morse_output_char;
 #if defined HAS_ePaper
    MC_printBIG_at((morse_out_buffer_cnt - 1), MORSE_MONOCHROME_ROW, s);
 #else	// HAS_OLED
    MC_printBIG_at(2*(morse_out_buffer_cnt - 1), MORSE_MONOCHROME_ROW, s);
 #endif
  }

  morse_output_char = '\0';	// trigger off
}
#endif // HAS_DISPLAY


bool echo_morse_input=true;
bool morse_store_received_letter(char letter) {		// returns error
  if(echo_morse_input)
    MENU.out(letter);
#if defined HAS_DISPLAY
  morse_output_char = letter;	// char and switch
#endif
  if(morse_out_buffer_cnt < MORSE_OUTPUT_BUFFER_SIZE) {
    morse_output_buffer[morse_out_buffer_cnt++] = letter;
    morse_output_buffer[morse_out_buffer_cnt]='\0';	// append '\0' just in case ;)
    return 0;	// ok
  }

  return true;	// ERROR
}


extern bool accGyro_is_active;
extern uint8_t monochrome_power_save;
void static morse_token_decode() {	// decode received token sequence
/*
  decode received token sequence
  SAVE LETTERS,

  EXECUTE COMMANDS,
  return of commands is currently unused, always 0

  see: MORSE_DECODE_DEBUG
*/
  char pattern[10] = { '\0' };	// 10 (up to 9 real tokens + '\0'
  char token;

#if defined MORSE_DECODE_DEBUG
  MENU.outln("morse_token_decode()");
#endif

  pattern[0] = '\0';
  short pattern_length=0;
  if(morse_token_cnt) {
    for (int i=0; i<morse_token_cnt; i++) {
      token = morse_SEEN_TOKENS[i];
      if(is_real_token(token)) {
	if(pattern_length > 8) {	  // ERROR
	  MENU.error_ln(F("morse pattern_length"));
	  morse_token_cnt=0;
	  return;
	}
	switch(token) {
	case MORSE_TOKEN_dot:
	case MORSE_TOKEN_dash:
	  pattern[pattern_length++] = token;
#if defined MORSE_DECODE_DEBUG
	  MENU.space(2);
	  MENU.out(token);
#endif
	  break;

	case MORSE_TOKEN_separeLetter:		// time to store letter
#if defined MORSE_DECODE_DEBUG
	  MENU.out(F("\tsepareLetter "));
#endif
	  if(morse_find_definition(pattern) != ILLEGAL32) {
	    switch(morse_PRESENT_TYPE) {
	    case '*':	// letter
	      morse_store_received_letter(morse_PRESENT_in_case_Letter);
	      break;

	    case 'C':	// Command
	      if(morse_PRESENT_COMMAND == "NEXT") {
		if(morse_out_buffer_cnt)	// if something is buffered, send it,
		  morse_output_to_do = true;	//	triggers morse_do_output() on morse input
		else {				// else: 'P'=START/STOP		TODO: for preset mode, TEST: for others
		  morse_store_received_letter(morse_PRESENT_in_case_Letter = 'P');
		  morse_output_to_do = true;	//	triggers morse_do_output() on 'P'
		}
	      } else if(morse_PRESENT_COMMAND == "LOWER")
		morse_uppercase = false;

	      else if(morse_PRESENT_COMMAND == "UPPER")
		morse_uppercase = true;

	      else if(morse_PRESENT_COMMAND == "DELLAST") {
		if(morse_out_buffer_cnt) {
		  morse_out_buffer_cnt--;
#if defined HAS_ePaper
		  MC_printBIG_at(morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, " ");	// DELLAST
#elif defined HAS_OLED
		  MC_printBIG_at(2*morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, " ");	// DELLAST
#endif
		}

	      } else if(morse_PRESENT_COMMAND == "OLED") {	// ---.-  "OA"
#if defined HAS_OLED			// (NOOP if no HAS_OLED)
		if(oled_feedback_while_playing ^= 1) {		// got switched on
		  monochrome_power_save = 0;
		  monochrome_setPowerSave(monochrome_power_save);

		  MENU.out(F("OLED"));
		  MENU.out_ON_off(oled_feedback_while_playing);
		  MENU.ln();
		  MC_display_message(F(" OLED "));
		} else {					// got switched off
		  MC_display_message(F(" off "));
		} // oled switched on/off
#else
		;
#endif

	      } else if(morse_PRESENT_COMMAND == "CANCEL") {	// CANCEL
		morse_token_cnt = 0;
		morse_out_buffer_cnt = 0;
#if defined HAS_DISPLAY
		MC_printBIG_at(0, MORSE_MONOCHROME_ROW, "__");	// CANCEL shows "__"
#endif
	      } else if(morse_PRESENT_COMMAND == "ANY1") {	// '----'
		MENU.outln(F("\"ANY1\" currently unused"));

	      } else if(morse_PRESENT_COMMAND == "MACRO_NOW") {	// ...-.  SN
		if(morse_out_buffer_cnt) {
#if defined USE_ESP_NOW
		  extern void esp_now_send_maybe_do_macro(uint8_t* mac_addr, char * macro);
		  morse_output_buffer[morse_out_buffer_cnt]='\0';	// append '\0'
		  extern uint8_t* esp_now_send2_mac_p;
		  esp_now_send_maybe_do_macro(esp_now_send2_mac_p, morse_output_buffer); // send to *esp_now_send2_mac_p
#else
		  MENU.outln(F("*no* USE_ESP_NOW"));
		  MENU.outln(F("TODO: reset morse output buffer?"));	// TODO: TEST:
#endif
		} else {
		  MENU.outln(F("no data to send now"));
		}

#if defined USE_MPU6050
	      } else if(morse_PRESENT_COMMAND == "UIswM") {	// '..-..'  UI	switch Motion UI on/off
		  MENU.out(F("motion UI "));
		  MENU.out_ON_off(accGyro_is_active ^= 1);	// toggle and show
		  MENU.ln();
  #if defined HAS_OLED
		  if(monochrome_can_be_used()) {
		    MC_setCursor(monochrome_getCols() - 7 ,0);	// position of OLED UI display
		    if(accGyro_is_active) {
		      MC_setInverseFont();
		      MC_print(F("      "));	// TODO: real string
		      MC_clearInverseFont();
		    } else
		      MC_print(F("      "));
		  }
  #endif // HAS_OLED
#endif // USE_MPU6050

	      } else	// unknown morse command
		MENU.out("\nCOMMAND:\t"); MENU.outln(morse_PRESENT_COMMAND.c_str());
	      break;

	    default:
	      MENU.error_ln(F("morse_decode type"));
	      morse_reset_definition("");
	      morse_token_cnt=0;
	      return;
	    }
	  } else {	// no definition found, error
#if defined HAS_DISPLAY
	    if(morse_out_buffer_cnt) {
  #if defined HAS_ePaper
	      MC_printBIG_at(morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, "'");	// TODO: TEST:
  #elif defined HAS_OLED
	      if(monochrome_can_be_used()) {
		MC_setInverseFont();
		MC_printBIG_at(2*morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, "'");	// TODO: TEST:
		MC_clearInverseFont();
	      }
  #endif	// ePaper | OLED
	    }
#endif // HAS_DISPLAY
	    MENU.space(2);
	    ERROR_ln(F("morse  no definition"));
	    morse_reset_definition("");
	    morse_token_cnt=0;
	    return;
	  }
	  break;

	case MORSE_TOKEN_separeWord:
#if defined MORSE_DECODE_DEBUG
	  MENU.out(F("\tsepareWord "));
#endif
	  break;

	case MORSE_TOKEN_loong:
	  pattern[pattern_length++] = token;
#if defined MORSE_DECODE_DEBUG
	  MENU.space(2);
	  MENU.out(token);
#endif
	  break;

	case MORSE_TOKEN_overlong:
	  pattern[pattern_length++] = token;
	  break;

	default:
	  MENU.error_ln(F("morse real token unknown"));
	  morse_reset_definition("");
	  morse_token_cnt=0;
	  return;
	}
      } // real token
    }

    morse_token_cnt=0;
  } // if(morse_token_cnt)
} // morse_token_decode()


// not used
// void morse_show_tokens_of_letter(char c, uint8_t row=0) {	// uppercase only
//   int maxlen=17;
//   char txt[maxlen]= {0};
//   char* format = F("%c %s");
//   for(int i=0; i < MORSE_DEFINITIONS; i++) {
//     morse_read_definition(i);
//     if(morse_PRESENT_UPPER_Letter == c) {	// uppercase only
//       snprintf(txt, maxlen, format, morse_PRESENT_UPPER_Letter, morse_DEFINITION_TOKENS.c_str());
//       extern uint8_t /*next_row*/ extended_output(char* data, uint8_t col=0, uint8_t row=0, bool force=false);
//       extended_output(txt, 0, row, false);
//       MENU.ln();
//
//       break;
//     }
//   }
// } // morse_show_tokens_of_letter()


#if defined COMPILE_MORSE_CHEAT_SHEETS && defined HAS_DISPLAY
bool /*ok*/ morse_tokens_of_letter(char* result, uint8_t len, char c) {	// uppercase only
  result[0] = '\0';
  char* format = F("%c %s");
  for(int i=0; i < MORSE_DEFINITIONS; i++) {
    morse_read_definition(i);
    if(morse_PRESENT_UPPER_Letter == c) {	// uppercase only
      snprintf(result, len, format, morse_PRESENT_UPPER_Letter, morse_DEFINITION_TOKENS.c_str());
      return true;	// OK
    }
  }
  return false;	// ERROR, not found
} // morse_tokens_of_letter()

#if defined HAS_OLED
  #define CHEAT_BUFLEN	4
#elif defined HAS_ePaper
  #define CHEAT_BUFLEN	8
#endif
char cheat_buffer[CHEAT_BUFLEN] = {0};

void show_cheat_sheet() {
  char c;
  uint8_t maxlen=17;

#if defined HAS_DISPLAY
  monochrome_clear();	// subito, MC_clear_display() does it too late!
#endif
  char result[maxlen];
#if defined HAS_OLED
  uint8_t rows=4;
#elif defined HAS_ePaper
  uint8_t rows=5;
#endif
  for(int i=0; i<rows; i++) {
    if((c = cheat_buffer[i])) {
      if(morse_tokens_of_letter(result, maxlen, c)) {	// uppercase only
#if defined HAS_OLED
	extern uint8_t /*next_row*/ monochrome_big_or_multiline(int row, const char* str);
	monochrome_big_or_multiline(2*i, result);
#elif defined HAS_ePaper
	ePaper_BIG_or_multiline(i, result);
#endif
	MENU.outln(result);
      } else {	// morse code not found
	MENU.out(F("unknown symbol "));
	MENU.out(c);
	MENU.tab();
	MENU.outln((int) c);
      }
    } else break;
  }
}

void make_morse_cheat_sheet(char* symbols) {
  for(int i=0; i<CHEAT_BUFLEN; i++)
    cheat_buffer[i] = symbols[i];
  do_on_other_core(show_cheat_sheet);
}

void morse_cheat_sheets_UI() {
  static uint8_t cheat_sheet_i = 0;

  bool next_sheet=true;
  int input_value = MENU.numeric_input(cheat_sheet_i);	// use false input as UI to *keep* the same sheet ;)

  #define CHEAT_SHEETS_MAX	8	//	>>>>>>>> *DO* *UPDATE* number of CHEAT_SHEETS_MAX <<<<<<<<<<<<<<
  if(input_value <= CHEAT_SHEETS_MAX  &&  input_value >= 0)
    cheat_sheet_i = (uint8_t) input_value;
  else				// use false input as UI to *keep* the same sheet ;)
    next_sheet = false;

  MENU.out(F("morse cheat sheet "));
  MENU.outln(cheat_sheet_i);

  switch(cheat_sheet_i) {
  case 0:
    make_morse_cheat_sheet(F("BCDG"));
    break;
  case 1:
    make_morse_cheat_sheet(F("FLPQ"));
    break;
  case 2:
    make_morse_cheat_sheet(F("JUVW"));
    break;
  case 3:
    make_morse_cheat_sheet(F("XYZ'"));
    break;
  case 4:
    make_morse_cheat_sheet(F("*/+-"));
    break;
  case 5:
    make_morse_cheat_sheet(F("=!:?"));
    break;
  case 6:
    make_morse_cheat_sheet(F(".,;:"));
    break;
  case 7:
    make_morse_cheat_sheet(F("&$'\""));
    break;
  case 8:
    make_morse_cheat_sheet(F("@"));
    break;
    /*  unkown: |~%<>{} <space>
	case :
	make_morse_cheat_sheet(F("|~% "));
	break;
	case :
	make_morse_cheat_sheet(F("<>{}"));
	break;
    */
  }
  // *DO* update #define CHEAT_SHEETS_MAX

  if(next_sheet) {
    cheat_sheet_i++;
    if(cheat_sheet_i > CHEAT_SHEETS_MAX)
      cheat_sheet_i=0;
  }
} // morse_cheat_sheets_UI()
#endif // COMPILE_MORSE_CHEAT_SHEETS

  /* **************************************************************** */

void morse_init() {
  // MENU.outln(F("MORSE morse_init()"));

  //limit_debounce		= 0.5;
  //limit_debounce		= 0.25;	// *LOWERED* as a test		// TODO: test&trim
  limit_debounce		= 0.35;	// hat some stray 'E' so test 0.35	// TODO: test&trim
  limit_dot_dash		= (float) (dotTim + dashTim + 1)/2;
  limit_dot_dash = 2.0;	// maybe better?	TODO: TEST: ################
  limit_dash_loong	= (float) (dashTim + loongTim + 1 )/2;
  limit_loong_overlong	= (float) (loongTim + overlongTim + 1)/2;

  /*	float mean(int a, int b) is (a+b)/2	*/

#ifdef MORSE_GPIO_INPUT_PIN	// use GPIO with pulldown as morse input	// TODO: update, very old version, untested
  MENU.out(F("MORSE GPIO input pin "));
  MENU.outln(MORSE_GPIO_INPUT_PIN);

  pinMode(MORSE_GPIO_INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MORSE_GPIO_INPUT_PIN), morse_GPIO_ISR_rising, RISING);
#endif

#ifdef MORSE_TOUCH_INPUT_PIN	// use ESP32 touch sensor as morse input	// in use
  MENU.out(F("MORSE touch pin "));
  MENU.out(HARDWARE.morse_touch_input_pin);
  MENU.out(F("\tinterrupt level "));
  MENU.outln(touch_threshold);
  // touchAttachInterrupt(HARDWARE.morse_touch_input_pin, touch_morse_ISR, touch_threshold);	// do that last
#endif

#ifdef MORSE_OUTPUT_PIN
  MENU.out(F("MORSE output pin "));
  MENU.outln(HARDWARE.morse_output_pin);

  pinMode(HARDWARE.morse_output_pin, OUTPUT);
  digitalWrite(HARDWARE.morse_output_pin, LOW);
#endif

#if defined(MORSE_GPIO_INPUT_PIN) || defined(MORSE_TOUCH_INPUT_PIN)
  // morse_show_limits();	// code left for debugging
#endif

#ifdef MORSE_TOUCH_INPUT_PIN	// use ESP32 touch sensor as morse input
  touchAttachInterrupt(HARDWARE.morse_touch_input_pin, touch_morse_ISR_v3, touch_threshold);
#endif // MORSE_TOUCH_INPUT_PIN
} // morse_init()


#define MORSE_H
#endif
