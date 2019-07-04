/*
  morse.h
*/

#ifndef MORSE_H

// avoid sound glitches when using OLED:
bool oled_feedback_while_playing = false;	// do *not* give morse feedback while playing

#if ! defined MORSE_MONOCHROME_ROW
  #define MORSE_MONOCHROME_ROW	0	// row for monochrome morse display
#endif

//#define MORSE_DECODE_DEBUG	// tokens to letters, commands

//#define MORSE_DEBUG_RECEIVE_TOKEN	// TODO: REMOVE: debugging code
//#define MORSE_TOKEN_DEBUG		// low level token recognising debugging
//#define DEBUG_MORSE_TOUCH_INTERRUPT
#define MORSE_COMPILE_HELPERS		// compile some functions for info, debugging, *can be left out*

#include "touch.h"
#include "esp_attr.h"


// ################ TODO: MOVE: configuration
// use GPIO (with pulldown) as morse input?
// #define MORSE_GPIO_INPUT_PIN	34	// Morse input GPIO pin, 34 needs hardware pulldown

//#define MORSE_OUTPUT_PIN	2 // 2 is often blue onboard led, hang a led, a piezzo on that one :)
//#define MORSE_OUTPUT_PIN	12 // *one shot TEST ONLY*	// TODO: separe from PERIPHERAL_POWER_SWITCH_PIN

#ifndef MORSE_TOUCH_INPUT_PIN		// TODO: be more flexible
  #define MORSE_TOUCH_INPUT_PIN	13	// use ESP32 touch sensor as morse input	(13 just a test)
#endif

// menu interface is case sensitive, so I need lowercase
// maybe also COMMAND mode?	TODO: rethink
bool morse_uppercase=true;

hw_timer_t * morse_separation_timer = NULL;
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

#define MORSE_TOKEN_MAX	256			// a *lot* for debugging...	TODO:
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
#define MORSE_TOKEN_OFF			'o'
#define MORSE_TOKEN_ON			'°'

#define MORSE_TOKEN_ILLEGAL		'§'

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

// forward declaration
void IRAM_ATTR morse_received_token(char token, float token_duration);

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
}


// factored out for testing...
void IRAM_ATTR morse_endOfLetter();
void IRAM_ATTR morse_separation_check_ON() {
  // see morse_setup.h	timerAttachInterrupt(morse_separation_timer, &morse_endOfLetter, true);
  // timerRestart(morse_separation_timer);
  timerStart(morse_separation_timer);
  timerWrite(morse_separation_timer, 0);

  timerAlarmWrite(morse_separation_timer, morse_TimeUnit * separeLetterTim, false);
  //timerAlarmWrite(morse_separation_timer, (unsigned long) ((float) morse_TimeUnit * separeLetter), false);
  timerAlarmEnable(morse_separation_timer);	// separation detection on
}

void IRAM_ATTR morse_separation_check_OFF() {
  timerAlarmDisable(morse_separation_timer);
  timerStop(morse_separation_timer);

  // morse_separation_timer.detach();
}


/* **************************************************************** */
// GPIO ISR

float scaled_low_duration;	// microseconds/morse_TimeUnit
float scaled_high_duration;

#if defined MORSE_GPIO_INPUT_PIN	// Morse input GPIO pin (34 needs hardware pulldown, check on others)
void IRAM_ATTR morse_GPIO_ISR_falling();	// declaration is needed...

void IRAM_ATTR morse_GPIO_ISR_rising() {	// MORSE INPUT on GPIO
  static unsigned long morse_low_time=0;

  portENTER_CRITICAL_ISR(&morse_MUX);
  morse_start_ON_time = micros();
  morse_separation_check_OFF();

  morse_low_time = morse_start_ON_time - morse_start_OFF_time;
  scaled_low_duration = (float) morse_low_time / morse_TimeUnit;

#if defined MORSE_TOKEN_DEBUG
  morse_received_token('L', (float) scaled_low_duration);	// ################ DEBUGGING only
#endif

  if (scaled_low_duration >= separeWordTim) {
      morse_received_token(MORSE_TOKEN_separeLetter, scaled_low_duration);    // " |" is word end
      morse_received_token(MORSE_TOKEN_separeWord, scaled_low_duration);
  } else if (scaled_low_duration >= separeLetterTim) {
    morse_received_token(MORSE_TOKEN_separeLetter, scaled_low_duration);
  }

#if defined MORSE_TOKEN_DEBUG
 else if (scaled_low_duration < limit_debounce) {
   morse_received_token(MORSE_TOKEN_debounce, scaled_low_duration);
 } else
   morse_received_token(MORSE_TOKEN_separe_OTHER, scaled_low_duration);

  if(!digitalRead(MORSE_GPIO_INPUT_PIN)) {	// DOUBLE CHECK if MORSE_GPIO_INPUT_PIN is still high...
     morse_received_token('i', -1);
     MENU.outln("pin is LOW!");		// ################ sorry ;)
  }

#endif

  attachInterrupt(digitalPinToInterrupt(MORSE_GPIO_INPUT_PIN), morse_GPIO_ISR_falling, FALLING);
  portEXIT_CRITICAL_ISR(&morse_MUX);
} // morse_GPIO_ISR_rising()


void IRAM_ATTR morse_GPIO_ISR_falling() {			// MORSE INPUT on GPIO morse_GPIO_ISR_falling()
  static unsigned long morse_high_time=0L;

  portENTER_CRITICAL_ISR(&morse_MUX);
  morse_start_OFF_time = micros();
  morse_separation_check_OFF();

  morse_high_time = morse_start_OFF_time - morse_start_ON_time;

  scaled_high_duration = (float) morse_high_time / morse_TimeUnit; // microseconds/morse_TimeUnit
#if defined MORSE_TOKEN_DEBUG
  morse_received_token('H', scaled_high_duration);	// ################ DEBUGGING only
#endif

  if (scaled_high_duration > limit_debounce) {			// *real input* no debounce
    if(scaled_high_duration > limit_loong_overlong) {
      morse_received_token(MORSE_TOKEN_overlong, scaled_high_duration);
    } else if (morse_high_time > morse_TimeUnit * limit_dash_loong) {
      morse_received_token(MORSE_TOKEN_loong, scaled_high_duration);
    } else if (morse_high_time > morse_TimeUnit * limit_dot_dash) {
      morse_received_token(MORSE_TOKEN_dash, scaled_high_duration);
    } else {
      morse_received_token(MORSE_TOKEN_dot, scaled_high_duration);
    }
  }

#if defined MORSE_TOKEN_DEBUG
  else {		// debounce, ignore
    morse_received_token(MORSE_TOKEN_debounce, scaled_high_duration);
  }

  if(digitalRead(MORSE_GPIO_INPUT_PIN)) {	// DOUBLE CHECK if MORSE_GPIO_INPUT_PIN is still low...
    morse_received_token('I', scaled_high_duration);
    MENU.outln("pin is HIGH!");		// ################ sorry ;)
  }
#endif

  attachInterrupt(digitalPinToInterrupt(MORSE_GPIO_INPUT_PIN), morse_GPIO_ISR_rising, RISING);
  morse_separation_check_ON();

  portEXIT_CRITICAL_ISR(&morse_MUX);
} // morse_GPIO_ISR_falling()
#endif // MORSE_GPIO_INPUT_PIN


/* **************************************************************** */
// touch ISR

//#define DEBUG_MORSE_TOUCH_INTERRUPT
void IRAM_ATTR morse_endOfLetter() {
  morse_separation_check_OFF();

  float scaled_low_duration = (float) (micros() - morse_start_OFF_time) / morse_TimeUnit;
  //  morse_received_token(MORSE_TOKEN_separeLetter, -1);	// TODO: ################
#if defined DEBUG_MORSE_TOUCH_INTERRUPT
  morse_received_token('#', scaled_low_duration);	// TODO: REVERSE after debugging ################
#endif
  morse_received_token(MORSE_TOKEN_separeLetter, scaled_low_duration);	// TODO: REVERSE after debugging ################

#if defined DEBUG_MORSE_TOUCH_INTERRUPT
  //morse_debug_token_info();	// REMOVE: debug only ################
#endif
}


/* **************************************************************** */
// touch ISR:	touch_morse_ISR()
void IRAM_ATTR morse_stats_gather(char token, float duration);	// forwards declaration

#if defined MORSE_TOUCH_INPUT_PIN
void IRAM_ATTR touch_morse_ISR(void) {	// ISR for ESP32 touch sensor as morse input
  portENTER_CRITICAL_ISR(&morse_MUX);	// MAYBE: a separated mux for touch?

  static bool seems_touched=false;
  static unsigned long morse_touch_time=0L;
  static unsigned long morse_release_time=0L;

  if(touchRead(MORSE_TOUCH_INPUT_PIN) < touch_threshold) { // check if IT *IS* TOUCHED RIGHT NOW, YES <<<<
#if defined MORSE_OUTPUT_PIN
    digitalWrite(MORSE_OUTPUT_PIN, HIGH);	// feedback: pin is touched, LED on
#endif
    if(seems_touched) {		// *IS* touched, BUT was seen touched before...
#if defined DEBUG_MORSE_TOUCH_INTERRUPT
      morse_received_token(MORSE_TOKEN_ON, -1);	// so far, I have never seen this :)
#else
      ;					// just ignore
#endif
    } else {			// looks probably touched
      morse_separation_check_OFF();
      morse_touch_time = micros();
      seems_touched = true;
      float scaled_off_duration = (float) (morse_touch_time - morse_release_time) / morse_TimeUnit;
      if (scaled_off_duration >= separeWordTim) {
	morse_received_token(MORSE_TOKEN_separeLetter, scaled_off_duration);	// " |" is word end
	morse_received_token(MORSE_TOKEN_separeWord, scaled_off_duration);
#if defined  DEBUG_MORSE_TOUCH_INTERRUPT
	morse_received_token('X', scaled_off_duration);	// REMOVE: debug only ################
#endif
      } else if (scaled_off_duration >= separeLetterTim) {
	morse_received_token(MORSE_TOKEN_separeLetter, scaled_off_duration);
#if defined DEBUG_MORSE_TOUCH_INTERRUPT
	morse_received_token('Y', scaled_off_duration);	// REMOVE: debug only ################
#endif
      }
      else {
	morse_stats_gather(MORSE_TOKEN_separeToken, scaled_off_duration);
//#if defined DEBUG_MORSE_TOUCH_INTERRUPT
//	morse_received_token(MORSE_TOKEN_separe_OTHER, scaled_off_duration); // arrives often, no harm (?)
//#endif
      }
    }

    touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE);	// wait for touch release

  } else {			// >>>>>>>>>>>>>>>> IS *NOT* TOUCHED, right now <<<<<<<<<<<<<<<<
#if defined MORSE_OUTPUT_PIN
    digitalWrite(MORSE_OUTPUT_PIN, LOW); // feedback: pin released, LED off
#endif
    if(!seems_touched) {		// not touched *BUT* was already so
#if defined DEBUG_MORSE_TOUCH_INTERRUPT
      morse_received_token(MORSE_TOKEN_OFF, -1);	// happens quite often..., ignore
#else
      ;				// just ignore ;)
#endif

    } else {			// looks like *not* touched
      morse_separation_check_ON();
      morse_release_time = micros();
      seems_touched = false;

      float scaled_touched_duration = (float) (morse_release_time - morse_touch_time) / morse_TimeUnit;
      if (scaled_touched_duration > limit_debounce) {			// *real input* no debounce
	if(scaled_touched_duration > limit_loong_overlong) {
	  morse_received_token(MORSE_TOKEN_overlong, scaled_touched_duration);
	} else if (scaled_touched_duration > limit_dash_loong) {
	  morse_received_token(MORSE_TOKEN_loong, scaled_touched_duration);
	} else if (scaled_touched_duration > limit_dot_dash) {
	  morse_received_token(MORSE_TOKEN_dash, scaled_touched_duration);
	} else {	// this may give false dots...
	  morse_received_token(MORSE_TOKEN_dot, scaled_touched_duration);
	}
      }
#if defined DEBUG_MORSE_TOUCH_INTERRUPT
      else {		// debounce, ignore
	morse_received_token(MORSE_TOKEN_debounce, scaled_touched_duration);
      }
#endif
    }

    touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);	// wait for next touch
  }

  portEXIT_CRITICAL_ISR(&morse_MUX);	// MAYBE: a separated mux for touch?
}
#endif // MORSE_TOUCH_INPUT_PIN
/* **************************************************************** */


#define MORSE_OUTPUT_MAX	64
char morse_OUTPUT_buffer[MORSE_OUTPUT_MAX];
short morse_OUTPUT_cnt=0;

bool morse_2output_buffer(char letter) {
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

#ifndef ILLEGAL
  #define ILLEGAL	~0
#endif

bool morse_store_as_letter(char token) {	// translate special cases and store as letters
  switch (token) {
  case ILLEGAL:
    morse_2output_buffer(MORSE_TOKEN_ILLEGAL);
  case '\0':
    return false;
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
}

// decode stored tokens and store recognised letters
bool morse2letter_decoder_state(char token, bool init=false) {
  static char state='\0';

  // if init is true initialize, then return	// ################ TODO: REVIEW:
  if (init) {
    state='\0';					// reset
    return true;	// dummy
  }

  // letter separation first, then return
  if (token == MORSE_TOKEN_separeLetter) {
    if(state) {
      bool ret = morse_store_as_letter(state);	// save
      state='\0';				// reset
//      morse_stats_init();			// prepare next statistic run
      return ret;				// reset
   } else
      return false;				// no state, just return
  }

  // word separation: store a space, then return
  if (token == MORSE_TOKEN_separeWord) {
    state='\0';				// reset
    return morse_store_as_letter(' ');		// store a space and return
  }

/*
  // maybe treat COMMANDs here?
  if (token == MORSE_TOKEN_loong) {
    // COMMAND parser
  }
*/

  // decode letters
  if((token == MORSE_TOKEN_dot) || (token == MORSE_TOKEN_dash)) {
    switch (state) {
    case ILLEGAL:	// TODO: needs testing ################
      state='\0';
    case '\0':	//	initial state, start reading first token
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'E';
	break;
      case MORSE_TOKEN_dash:
	state = 'T';
	break;
      }
      break;

    case 'E':	// .
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'I';
	break;
      case MORSE_TOKEN_dash:
	state = 'A';
	break;
      }
      break;

    case 'T':	// -
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'N';
	break;
      case MORSE_TOKEN_dash:
	state = 'M';
	break;
      }
      break;

    case 'I':	// ..
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'S';
	break;
      case MORSE_TOKEN_dash:
	state = 'U';
	break;
      }
      break;

    case 'A':	// .-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'R';
	break;
      case MORSE_TOKEN_dash:
	state = 'W';
	break;
      }
      break;

    case 'N':	// -.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'D';
	break;
      case MORSE_TOKEN_dash:
	state = 'K';
	break;
      }
      break;

    case 'M':	// --
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'G';
	break;
      case MORSE_TOKEN_dash:
	state = 'O';
	break;
      }
      break;

    case 'S':	// ...
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'H';
	break;
      case MORSE_TOKEN_dash:
	state = 'V';
	break;
      }
      break;

    case 'U':	// ..-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'F';
	break;
      case MORSE_TOKEN_dash:
	state = 220;
	// state = 'Ü';
	break;
      }
      break;

    case 'R':	// .-.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'L';
	break;
      case MORSE_TOKEN_dash:
	state = 'Ä';
	break;
      }
      break;

    case 'W':	// .--
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'P';
	break;
      case MORSE_TOKEN_dash:
	state = 'J';
	break;
      }
      break;

    case 'D':	// -..
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'B';
	break;
      case MORSE_TOKEN_dash:
	state = 'X';
	break;
      }
      break;

    case 'K':	// -.-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'C';
	break;
      case MORSE_TOKEN_dash:
	state = 'Y';
	break;
      }
      break;

    case 'G':	// --.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'Z';
	break;
      case MORSE_TOKEN_dash:
	state = 'Q';
	break;
      }
      break;

    case 'O':	// ---
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'Ö';
	break;
      case MORSE_TOKEN_dash:
	state = 'c';	// 'c' means CH here ;)
	break;
      }
      break;


    case 'H':	// ....
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '5';
	break;
      case MORSE_TOKEN_dash:
	state = '4';
	break;
      }
      break;

    case 'V':	// ...-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'v';	// 'v' means VE, verstanden
	break;
      case MORSE_TOKEN_dash:
	state = '3';
	break;
      }
      break;

    case 'F':	// ..-.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'É';
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case 220:	// ..--
      // case 'Ü':	// ..--
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = '2';
	break;
      }
      break;

    case 'L':	// .-..
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = 'È';
	break;
      }
      break;

    case 'Ä':	// .-.-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = 'a';	// 'a' means AR Spruchende here
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case 'P':	// .--.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '?';
	break;
      case MORSE_TOKEN_dash:
	state = 'Å';
	break;
      }
      break;

    case 'J':	// .---
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = '1';
	break;
      }
      break;

    case 'B':	// -...
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '6';
	break;
      case MORSE_TOKEN_dash:
	state = 'b';	// 'b' means BT	Pause here
	break;
      }
      break;

    case 'X':	// -..-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '/';
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case 'C':	// -.-.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = 'k';	// 'k' means KA	Spruchanfang
	break;
      }
      break;

    case 'Y':	// -.--
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '(';
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case 'Z':	// --..
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = '7';
	break;
      }
      break;

    case 'Q':	// --.-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = 'Ñ';
	break;
      }
      break;

    case 'Ö':	// ---.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '8';
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case 'c':	// ----	'c' means CH here ;)
      switch(token) {
      case MORSE_TOKEN_dot:
	state = '9';
	break;
      case MORSE_TOKEN_dash:
	state = '0';
	break;
      }
      break;

    case '5':	// .....
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '4':	// ....-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '3':	// ...--
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '2':	// ..---
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '1':	// .----
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '0':	// -----
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '9':	// ----.
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '8':	// ---..
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '7':	// --...
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    case '6':	// -....
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;


    case '.':	// .-.-.-
      switch(token) {
      case MORSE_TOKEN_dot:
	state = ILLEGAL;
	break;
      case MORSE_TOKEN_dash:
	state = ILLEGAL;
	break;
      }
      break;

    default:
      MENU.out(state);
      MENU.out(F(" morse2letter_decoder_state  unknown morse code"));
      MENU.tab();
      MENU.outln((int) state);
      break;
    }
  }

  return false;
}


// TODO: obsolete? ################
void morse_tokens2meaning_state() {	// translate saved tokens into letters or COMMANDs, save letters
  morse2letter_decoder_state(0, true);	// init
  char token;

  for(int i=0; i<morse_token_cnt; i++) {
    if(is_real_token(token = morse_SEEN_TOKENS[i]))
      morse2letter_decoder_state(token);
  }
  morse_token_cnt=0;
}

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
// TODO: inline
void IRAM_ATTR morse_stats_gather(char token, float duration) {	// only real tokens please
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
}

float morse_saved_stats_dot=0.0;
float morse_saved_stats_dash=0.0;
float morse_saved_stats_loong=0.0;
float morse_saved_stats_separeToken=0.0;
float morse_saved_stats_separeLetter=0.0;
// float morse_saved_stats_separeWord=0.0;


unsigned short morse_stat_ID=0;		// flag (counter)	// flag (counter) for display control
unsigned short morse_stat_seen_ID=0;	// flag (counter)	// flag (counter) for display control

void IRAM_ATTR morse_stats_save() {	// call immediately after morse_stats_do() to save statistics
  morse_stat_ID++;	// display flag (counter)

  if(morse_stats_dot_cnt) {
    morse_saved_stats_dot = morse_stats_dot_duration_;
  }
  if(morse_stats_dash_cnt) {
    morse_saved_stats_dash = morse_stats_dash_duration_;
  }
  if(morse_stats_loong_cnt) {
    morse_saved_stats_loong = morse_stats_loong_duration_;
  }
  if(morse_stats_separeToken_cnt) {
    morse_saved_stats_separeToken = morse_stats_separeToken_duration_;
  }
  if(morse_stats_separeLetter_cnt) {
    morse_saved_stats_separeLetter = morse_stats_separeLetter_duration_;
  }
//  if(morse_stats_separeWord_cnt) {
//    morse_saved_stats_separeWord = morse_stats_separeWord_duration_;
//  }
}

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

}

void IRAM_ATTR morse_stats_do() {
  MENU.ln();
  if(morse_stats_dot_cnt) {
    morse_stats_dot_duration_ /= morse_stats_dot_cnt;
//    MENU.out(F("dot\t\t% "));
//    MENU.outln(morse_stats_dot_duration_ / dotTim);
  }
  if(morse_stats_dash_cnt) {
    morse_stats_dash_duration_ /= morse_stats_dash_cnt;
//    MENU.out(F("dash\t\t% "));
//    MENU.outln(morse_stats_dash_duration_ / dashTim);

    // auto adapt, 1st simple try: only look at dashs
    morse_stats_mean_dash_factor = morse_stats_dash_duration_ / dashTim; // save for auto speed adaption
  }
  if(morse_stats_loong_cnt) {
    morse_stats_loong_duration_ /= morse_stats_loong_cnt;
//    MENU.out(F("loong\t\t% "));
//    MENU.outln(morse_stats_loong_duration_ / loongTim);
  }
  if(morse_stats_separeToken_cnt) {
    morse_stats_separeToken_duration_ /= morse_stats_separeToken_cnt;
//    MENU.out(F("separeToken\t% "));
//    MENU.outln(morse_stats_dash_duration_ / separeTokenTim);
  }
  if(morse_stats_separeLetter_cnt) {
    morse_stats_separeLetter_duration_ /= morse_stats_separeLetter_cnt;
//    MENU.out(F("separeLetter\t% "));
//    MENU.outln(morse_stats_dash_duration_ / separeLetterTim);
  }
//  if(morse_stats_separeWord_cnt) {
//    morse_stats_separeWord_duration_ /= morse_stats_separeWord_cnt;
//    MENU.out(F("separeWord\t% "));
//    MENU.outln(morse_stats_dash_duration_ / separeWordTim);
//  }

  morse_stats_save();		// call immediately after morse_stats_do()
}

/* **************************************************************** */
void morse_decode();	// pre declaration

void IRAM_ATTR morse_received_token(char token, float duration) {
  portENTER_CRITICAL_ISR(&morse_MUX);

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

	  morse_decode();
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

  portEXIT_CRITICAL_ISR(&morse_MUX);
}


/* **************************************************************** */
// morse output, i.e. on a piezzo, led

// declarations to be defined later
void morse_out_dot();
void morse_out_dash();
void morse_out_loong();
void morse_out_overlong();
void morse_out_separeLetter();
void morse_out_separeWord();

#if defined MORSE_OUTPUT_PIN
void morse_play_out_tokens(bool show=true) {	// play (and show) saved tokens in current morse speed
  char token;
  for(int i=0 ; i < morse_token_cnt; i++) {
    token = morse_SEEN_TOKENS[i];
    switch (token) {
    case MORSE_TOKEN_dot:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_dot();
      break;
    case MORSE_TOKEN_dash:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_dash();
      break;
    case MORSE_TOKEN_loong:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_loong();
      break;
    case MORSE_TOKEN_overlong:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_overlong();
      break;
    case MORSE_TOKEN_separeLetter:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_separeLetter();
      break;
    case MORSE_TOKEN_separeWord:
      if(show)
	MENU.out(token);	// show *before* sound
      morse_out_separeWord();
      break;
    }
  }
  if(show)
    MENU.ln();
}
#endif // MORSE_OUTPUT_PIN

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
//"5 0 ...-. 5 5",
  "5 * ...-- 3 3",
  "5 * ..-.. É É",	// É	FIX: lowercase
//"5 0 ..-.- 5 5",
//"5 0 ..--. - -",
  "5 * ..--- 2 2",
//"5 0 .-... 5 5",
  "5 * .-..- È È",	// È	FIX: lowercase
  "5 * .-.-. + +",
//"5 0 .-.-- 5 5",
//"5 0 .--.. 5 5",
  "5 * .--.- Å Å",	// Å	FIX: lowercase
  "5 * .---. - -",
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
  "5 * --.-- Ñ Ñ",	// Ñ
  "5 C ---.- OLED",	// OA	---.-	OLED	toggle oled display while playing ################
  "5 * ---.. 8 8",
  "5 * ----. 9 9",
  "5 C ---.- DELWORD",	// MK	---.-	DELETE WORD
  "5 * ----- 0 0",

  "6 * -....- _ _",	// check - _

  "6 * .-.-.- . .",
  "6 * --..-- , ,",
  "6 * ..--.. ? ?",
  "6 * ---... : :",
  "6 * .----. ' '",
  "6 * .-..-. \" \"",
  "6 * .--.-. @ @",

  "7 * ...--.. X X TODO:",		// beta ss	TODO: FIX:

  "8 C ........ MISTAKE",		// MISTAKE
};
#define MORSE_DEFINITIONS	(sizeof (morse_definitions_tab) / sizeof(const char *))


uint8_t morse_def_token_cnt=0;
char morse_def_TYPE=ILLEGAL;
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
  if(morse_DEFINITION.c_str() != "") {
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
  for(int i=0; c = p[i] ; i++) {
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
  case 'C':	// COMMANDs
    morse_show_space_delimited_string(morse_def_COMMAND->c_str());
    break;
  }

  MENU.ln();
}


int morse_find_definition(const char* pattern) {  // returns index in morse_definitions_tab[i] or ILLEGAL
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
  return ILLEGAL;	// not found
}

char morse_2ACTION() {
/*
  SAVE LETTERS,
  return CHAR for letters or ILLEGAL for unknown patterns

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


  return ILLEGAL;
}

/* **************************************************************** */

#ifndef MORSE_OUTPUT_BUFFER_SIZE
  #define MORSE_OUTPUT_BUFFER_SIZE	64	// size of morse output buffer
#endif
char  morse_output_buffer[MORSE_OUTPUT_BUFFER_SIZE];	// buffer
short morse_out_buffer_cnt=0;


bool musicbox_is_idle();	// pre declaration

bool morse_output_to_do=false;		// triggers morse_do_output()
void morse_do_output() {
  morse_output_buffer[morse_out_buffer_cnt]='\0';	// append '\0'
  if(morse_out_buffer_cnt) {
#if defined USE_MONOCHROME_DISPLAY
    if(oled_feedback_while_playing || musicbox_is_idle())
      u8x8.draw2x2String(0, MORSE_MONOCHROME_ROW, "        ");
#endif

    MENU.play_KB_macro((char*) morse_output_buffer);
  }

  morse_out_buffer_cnt=0;
  morse_output_to_do=false;

  morse_uppercase = true;	// reset to uppercase
}

#if defined USE_MONOCHROME_DISPLAY
char morse_do_monochrome_display = '\0';

//#define CHATTY_MONOCHROME
void morse_monochrome_display() {
#if defined CHATTY_MONOCHROME
  MENU.out("\nmorse_monochrome_display()\t");
  MENU.outln(morse_do_monochrome_display);
  /*	prior (small) version
  MENU.out_ON_off(morse_do_monochrome_display);
  MENU.tab();
  MENU.out_ON_off(monochrome_display_hardware);
  */
#endif
  if(morse_do_monochrome_display && morse_out_buffer_cnt) {
    if(oled_feedback_while_playing || musicbox_is_idle()) {
      u8x8.setInverseFont(0);

      // 2x2 version
      char s[]="? ";
      s[0] = morse_do_monochrome_display;
      u8x8.draw2x2String(2*(morse_out_buffer_cnt - 1), MORSE_MONOCHROME_ROW, s);

      /*	prior (small) version
		u8x8.setCursor(morse_out_buffer_cnt - 1, MORSE_MONOCHROME_ROW);
		u8x8.print(morse_do_monochrome_display);
		u8x8.print(' ');		// clear next char
      */
    }
  }

  morse_do_monochrome_display = '\0';	// trigger off
}
#endif

bool morse_store_received_letter(char letter) {		// returns error
#if defined USE_MONOCHROME_DISPLAY
  morse_do_monochrome_display = letter;
#endif
  if(morse_out_buffer_cnt < MORSE_OUTPUT_BUFFER_SIZE) {
    morse_output_buffer[morse_out_buffer_cnt++] = letter;
    morse_output_buffer[morse_out_buffer_cnt]='\0';	// append '\0' just in case ;)
    return 0;	// ok
  }

  return true;	// ERROR
}

extern uint8_t monochrome_power_save;
void morse_decode() {	// decode received token sequence
/*
  decode received token sequence
  SAVE LETTERS,

  EXECUTE COMMANDS,
  return of commands is currently unused, always 0

  see: MORSE_DECODE_DEBUG
*/
  char pattern[10] = { '\0' };	// 10 (up to 9 real tokens + '\0'
  char token;
  bool last_was_separeLetter=false;	// normally we want to store *only the first space* of a sequence

#if defined MORSE_DECODE_DEBUG
  MENU.outln("morse_decode()");
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
	  if(morse_find_definition(pattern) != ILLEGAL) {
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

#if defined USE_MONOCHROME_DISPLAY
		  if(oled_feedback_while_playing || musicbox_is_idle())
		    u8x8.draw2x2String(2*morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, " ");
#endif
		}
	      } else if(morse_PRESENT_COMMAND == "OLED") {
		if(oled_feedback_while_playing ^= 1)
		  u8x8.setCursor(0,1);

		MENU.out(F("OLED"));
		MENU.out_ON_off(oled_feedback_while_playing);
		MENU.ln();
	      } else if(morse_PRESENT_COMMAND == "CANCEL") {
		morse_out_buffer_cnt = 0;
/*   no, i do not want CANCEL to change accGyro_is_active any more
#if defined USE_MPU6050		// MPU-6050 6d accelero/gyro
		extern bool accGyro_is_active;
		accGyro_is_active = false;	// CANCEL accGyro_is_active
#endif
*/
#if defined USE_MONOCHROME_DISPLAY
		if(oled_feedback_while_playing || musicbox_is_idle())
		  u8x8.draw2x2String(0, MORSE_MONOCHROME_ROW, "C ");	// *short&quick*
#endif
	      } else if(morse_PRESENT_COMMAND == "ANY1") {	// '----'
#if defined USE_MONOCHROME_DISPLAY
		u8x8.setPowerSave(monochrome_power_save++ & 1);	// temporary ANY1 action
#else
		;
#endif
	      } else	// unknown
		MENU.out("\nCOMMAND:\t"); MENU.outln(morse_PRESENT_COMMAND.c_str());
	      break;

	    default:
	      MENU.error_ln(F("morse_decode type"));
	      morse_reset_definition("");
	      morse_token_cnt=0;
	      return;
	    }
	  } else {
#if defined USE_MONOCHROME_DISPLAY
	    if(oled_feedback_while_playing || musicbox_is_idle()) {
	      if(morse_out_buffer_cnt) {
		u8x8.setInverseFont(1);
		u8x8.draw2x2String(2*morse_out_buffer_cnt, MORSE_MONOCHROME_ROW, "?");
		u8x8.setInverseFont(0);
	      }
	    }
#endif
	    MENU.error_ln(F("morse  no definition"));
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
}

/* **************************************************************** */

  // ################################################################
// ################ TODO: REMOVE: replace
// *very* simple minded morse output hack ;)
// do *not* use with pulses, delays
#if defined MORSE_OUTPUT_PIN
void morse_out_dot() {
  unsigned long start_time=micros();

  while(micros() - start_time <= morse_TimeUnit * dotTim) {
    digitalWrite(MORSE_OUTPUT_PIN, HIGH);
    delay(4);
    digitalWrite(MORSE_OUTPUT_PIN, LOW);
    delay(4);
  }
  delay(morse_TimeUnit/1000);
}

void morse_out_dash() {
  unsigned long start_time=micros();

  while(micros() - start_time <= morse_TimeUnit * dashTim) {
    digitalWrite(MORSE_OUTPUT_PIN, HIGH);
    delay(4);
    digitalWrite(MORSE_OUTPUT_PIN, LOW);
    delay(4);
  }
  delay(morse_TimeUnit/1000);
}

void morse_out_loong() {
  unsigned long start_time=micros();

  while(micros() - start_time <= morse_TimeUnit * loongTim) {
    digitalWrite(MORSE_OUTPUT_PIN, HIGH);
    delay(4);
    digitalWrite(MORSE_OUTPUT_PIN, LOW);
    delay(4);
  }
  delay(morse_TimeUnit/1000);
}

void morse_out_overlong() {
  unsigned long start_time=micros();

  while(micros() - start_time <= morse_TimeUnit * overlongTim) {
    digitalWrite(MORSE_OUTPUT_PIN, HIGH);
    delay(4);
    digitalWrite(MORSE_OUTPUT_PIN, LOW);
    delay(4);
  }
  delay(morse_TimeUnit/1000);
}

void morse_out_separeLetter() {
  delay((separeLetterTim - 1) * morse_TimeUnit / 1000);
}

void morse_out_separeWord() {
  delay((separeWordTim - separeLetterTim - 1) * morse_TimeUnit / 1000);
}
#endif // MORSE_OUTPUT_PIN

#define MORSE_H
#endif
