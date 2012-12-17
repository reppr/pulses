/* **************************************************************** */
// serial_menu alias softboard for the arduino
/* ****************************************************************

             Software breadboard for the Arduino.

              http://github.com/reppr/softboard


    Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/* **************************************************************** */
/* README:

   Softboard  http://github.com/reppr/softboard

   Hardware/software developing/testing tool.

   Simple hardware menu interface over serial line
   as a kind of arduino software breadboard.

   Send one letter commands (and sometimes also sequences of chiffres)
   over a serial line say from your computer to the arduino to trigger
   actions and get infos.

   The arduino will buffer serial input (without waiting for it) until
   a terminating linefeed is received as an end token.
   Any sequence of one or more '\n'  '\c'  '\0' accepted as end token.

   Commands can read and set I/O pin configuration and states,
   switch pins on/off, read and write digital and analog values,
   switch pullup/high-z, or continuously watch changing inputs over time.

   Use it to test hardware like sensors, motors, and things you want
   to run quick tests after setting it up on a (real) breadboard.

   Then you can use it to test software parts of your program while
   you write it and fit parts together. 


   Send 'm' and a linefeed over serial line to see the menu.
   It displays some basic infos and hints on one-letter commands.


   Easy to write your own menu, take the hardware menu as example.
   Use it together with your own programs.


   Integrate your own code:

   Grep the source for ´dance´ or ´yodel´ and you might find hints how to
   setup a program menu to tell the arduino sketch to dance on ´d´,
   to yodel on ´y´  and on ´s´ to do something special ;)

   Just do #define PROGRAM_menu by uncommenting the line further down
   and you will get an example program menu to build upon.


   Installation:

   Get it from http://github.com/reppr/softboard
   Move the 'softboard' folder with all files
   to your arduino sketchbook/ folder.
   Load the sketch from arduino menu Files>Sketchbook>softboard.

   Arduino versions older than 1.0 need a fix:
     see this thread:
     http://code.google.com/p/arduino/issues/detail?id=604&start=200

       short version (adapt to your arduino version)
         edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
         comment out line 79 (round macro)
         #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
         tested on arduino-0023

   For older arduino versions you also have to rename the sketch file
   from softboard.ino to softboard.pde


   How it works:

   You communicate with the arduino over a serial connection
   that could be a real serial line or an usb cable.

   The minimalistic menu shows you one letter commands and listens to
   your input. Serial inputs are buffered until you send a linefeed.
   Then your inputs (commands and numbers as sequences of chiffres)
   will be read and acted upon.

   Do configure your terminal emulation program to send some sort of
   line ending code, usual culprits should work.

   Set arduino baud rate by editing the line starting with
   #define USE_SERIAL_BAUD
   and set it to the same value in your terminal software.

   So if for example you use the Arduino 'Serial Monitor' window
   check that it *does* send 'Newline' (in the bottom window frame)
   and set baud rate to the same value as USE_SERIAL_BAUD on the arduino.

   Toggle serial echo with ´e´.


   Some examples:
       Always let your terminal send a linefeed after the example string
       to trigger actions.


   Example 1:  'D13 OH' switch LED on   (D select pin, O output, H high)
               'L'      off again       (L low)


   Example 2:  'D13 IH' pullup LED pin  (D select pin, I input, H high)
                                        LED glows at low level
               'O'      LED as OUTPUT   now the LED is on


   Example 3:  Watch an analog input like a VU meter, changing over time.
               See electric noise on unconnected floating A2 input
               scrolling over your serial terminal.
               (Touch the input if there is no visible signal.)

               Now connect a sensor to the input and explore its data...

               'A2 v'      A=select pin (both analog or digital)
                           v=display analog read values and bar graphs

175     ***********
190     ************
192     *************
181     ************
163     ***********
158     **********
164     ***********
181     ************
192     *************
190     ************
176     ************
161     ***********
159     **********
168     ***********
186     ************
194     *************
188     ************
170     ***********



   Example 4:  ','  Display snapshot values of all analog inputs.

                        [fixed font only]

pin     value   |                               |                               |
*A0     609     ***************************************
 A1     504     ********************************
 A2     451     *****************************
 A3     398     *************************
 A4     383     ************************
 A5     409     **************************



   Example 5:  '.'  Info on all digital pin's I/O configuration and state.

 pin 0  I  hi-z
 pin 1  I  hi-z
 pin 2  O  high
 pin 3  O  low
 pin 4  I  hi-z
*pin 5  I  pullup       // * indicates the selected pin
 pin 6  I  hi-z
 pin 7  I  hi-z
 pin 8  I  hi-z
 pin 9  I  hi-z
 pin 10 I  hi-z
 pin 11 I  hi-z
 pin 12 I  hi-z
 pin 13 I  pullup


   Copyright © Robert Epprecht  www.RobertEpprecht.ch   GPLv2

   http://github.com/reppr/softboard
*/


/* **************************************************************** */
// source code starts here:
/* **************************************************************** */
// keep arduino GUI happy ;(
#include "serial_menu.h"


/* **************************************************************** */
// arduino versions

// needed for pre 1.0 versions:
#if (ARDUINO < 100)
  #include <pins_arduino.h>

  // triggers another bug.
  // see this thread:
  // http://code.google.com/p/arduino/issues/detail?id=604&start=200

  // short version (adapt to your arduino version)
  // edit file arduino-0022/hardware/arduino/cores/arduino/wiring.h
  // comment out line 79 (round macro)
  // #define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

  // tested on arduino-0023
#endif



/* **************************************************************** */
#define ILLEGAL		-1


/* **************************************************************** */
// CONFIGURATION:

// to switch serial menu on you *must* ´#define USE_SERIAL_BAUD <baud>´

// USE_SERIAL_BAUD
// to switch on serial #define USE_SERIAL_BAUD <baud>  *otherwise serial will be off*
//
// You can switch all serial line and menu code *off* by not defining USE_SERIAL_BAUD
// You can also do this later to save program memory.
//   your sketch could include the menu, but later grow too much,
//   or you'd want to move to another processor with less program memory.
//
// #define USE_SERIAL_BAUD	115200		// works fine here
#define USE_SERIAL_BAUD	57600
// #define USE_SERIAL_BAUD	38400


#ifdef USE_SERIAL_BAUD			// activate menus over serial line?
  // menu over serial, basics:
  #define SERIAL_MENU			// we *do* use serial menu

  // this version buffers serial input:
  // serial_input_BUF_size		   size of serial input buffer
  // eats RAM
  #define serial_input_BUF_size	64	// serial input buffer size in bytes 
//#define serial_input_BUF_size	128	// if you want a bigger buffer
//#define serial_input_BUF_size	16	// if you are tight of RAM

  // simple menu to access arduino hardware:
  #define HARDWARE_menu	 // menu interface to hardware configuration
  	  		 // this will let you read digital and analog inputs
  			 // watch changes on inputs as value or as bar graphs
  	  		 // set digital and analog outputs, etc.
  #ifdef HARDWARE_menu
    char PIN_digital = ILLEGAL;	// would be dangerous to default to zero
    char PIN_analog = 0;	// 0 is save as default for analog pins
  #endif // HARDWARE_menu
#else
  #error #define   USE_SERIAL_BAUD <baud>   in file softboard.ino
#endif	// USE_SERIAL_BAUD



/* **************************************************************** */
// To integrate your own program menu  #define PROGRAM_menu 
// Write function menu_program_display() and serial_menu_program_reaction()
// Please see examples below.
//
#define PROGRAM_menu		// uncomment to activate



/* **************************************************************** */
// board specific things:
// FIXME DADA ################################################################
#if defined(NUM_ANALOG_INPUTS) && defined(NUM_DIGITAL_PINS)
  #define ANALOG_INPUTs	NUM_ANALOG_INPUTS
  #define DIGITAL_PINs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)
#else	// savety net
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega boards
    #define ANALOG_INPUTs	16
    #define DIGITAL_PINs	54
  #else								// 168/328 boards
    #define ANALOG_INPUTs	6
    #define DIGITAL_PINs	14
  #endif
#endif	// board specific initialisations

#if defined(LED_BUILTIN)
  #define LED_PIN	LED_BUILTIN
#else
  #define LED_PIN	13
#endif



/* **************************************************************** */
// use PROGMEM to save RAM:
#include <avr/pgmspace.h>

// to save RAM constant strings are stored in PROGMEM
const unsigned char programName[] PROGMEM = "serial MENU";
const unsigned char programLongName[] PROGMEM = \
  "*** serial menu v0.2 ***	buffer=";
const unsigned char version[] PROGMEM = "version 0.2";

const unsigned char tab_[] PROGMEM = "\t";


// sorry, there is *no* LCD menu support here yet
// maybe i´ll need it one day and write it...
//
// LCD related code comes from other progs of mine
//
//	#ifdef USE_LCD
//	// LCD.print() for progmem strings:
//	// void LCD_print_progmem(const prog_uchar *str)	// does not work :(
//	void LCD_print_progmem(const unsigned char *str) {
//	  unsigned char c;
//	  while((c = pgm_read_byte(str++)))
//	    LCD.print(c);
//	}
//	
//	void LCD_print_at_progmem(unsigned char col, unsigned char row, const unsigned char *str) {
//	  LCD.setCursor(col, row);
//	  LCD_print_progmem(str);
//	}
//	
//	// clear line and print on LCD at line 'row'
//	const unsigned char LCD_empty[] PROGMEM = "                    ";
//	void LCD_print_line_progmem(unsigned char row, const unsigned char *str) {
//	  LCD_print_at_progmem(0, row, LCD_empty);
//	  LCD_print_at_progmem(0, row, str);
//	}
//	
//	#endif


#ifdef USE_SERIAL_BAUD
  // Serial.print() for progmem strings:
  // void serial_print_progmem(const prog_uchar *str)	// does not work :(
  void serial_print_progmem(const unsigned char *str) {
    unsigned char c;
    while((c = pgm_read_byte(str++)))
      Serial.write(c);
  }


  void serial_println_progmem(const unsigned char *str) {
    serial_print_progmem(str);
    Serial.println();
  }
#endif	// USE_SERIAL_BAUD



/* **************************************************************** */
// determine RAM usage:
extern int __bss_end;
extern void *__brkval;

int get_free_RAM() {
  int free;

  if ((int) __brkval == 0)
    return ((int) &free) - ((int) &__bss_end);
  else
    return ((int) &free) - ((int) __brkval);
}



/* **************************************************************** */
// menu over different interfaces
// only SERIAL_MENU implemented
//
// we could add other menus, like LCD or MIDI or whoknows,
// but nothing there in this version, sorry.
//
/* **************************************************************** */
// #define SERIAL_MENU	// do we use serial menu?
/* **************************************************************** */
#ifdef SERIAL_MENU

// inside  #ifdef SERIAL_MENU
// ****************************************************************
// basic menu I/O:

// To keep arduino GUI happy I put this into serial_menu.h:
//	// circular buffer circ_buf:
//	typedef struct {
//	  int size;		/* maximum number of elements           */
//	  int start;		/* index of oldest element              */
//	  int count;		/* index at which to write new element  */
//	  char *buf;		/* buffer                               */
//	} circ_buf;
//	
//	circ_buf serial_input_BUFFER;

bool echo_switch=true;

void cb_init(circ_buf *cb, int size) {
  cb->size  = size;
  cb->start = 0;
  cb->count = 0;
  cb->buf = (char *) malloc(cb->size);
}

/* unused
void cb_free(circ_buf *cb) {
  free(cb->buf);
}
*/


int cb_is_full(circ_buf *cb) {
  return cb->count == cb->size;
}

// cb_stored() number of buffered bytes:
int cb_stored(circ_buf *cb) {
  return cb->count;
}

// cb_write() save a byte to the buffer:
// *does not check if buffer is full*
void cb_write(circ_buf *cb, char value) {
  int end = (cb->start + cb->count) % cb->size;
  cb->buf[end] = value;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    ++ cb->count;
}

// cb_read() get oldest byte from the buffer:
// *does not check if buffer is empty*
char cb_read(circ_buf *cb) {
  char value = cb->buf[cb->start];
  cb->start = (cb->start + 1) % cb->size;
  --cb->count;
  return value;
}

// cb_recover_last(): recover one byte immediately read before
// *no checks inside*
void cb_recover_last(circ_buf *cb) {
  cb->start = (cb->start - 1 + cb->size) % cb->size;	// safety net ;)
  ++cb->count;
}


// inside  #ifdef SERIAL_MENU
// ****************************************************************
// menu I/O functions:

const unsigned char number_missing[] PROGMEM = "number missing";

// get numeric integer input from chiffre sequence in the serial buffer
void numeric_input(circ_buf *cb, long *value) {
  long input, num, sign=1;

  if (cb_stored(cb) == 0)
    goto number_missing;		// no input at all, return

  // skip spaces, get first chiffre:
  while ((input=cb_read(cb)) == ' ')	// first chiffre
    if (cb_stored(cb) == 0)
      goto number_missing;		// only spaces, return

  // check for sign:
  if (input == '-') {
    sign = -1;
    if (cb_stored(cb) == 0)
      goto number_missing;	// no input after sign, return
    input = cb_read(cb);	// first chiffre after sign
  }
  else if (input == '+') {
    if (cb_stored(cb) == 0)
      goto number_missing;	// no input after sign, return
    input = cb_read(cb);	// first chiffre after sign
  }

  if (input >= '0' && input <= '9')	// numeric first chiffre?
    num = input - '0';
  else {				// NAN
    cb_recover_last(cb); // put NAN char back into input buffer
    goto number_missing;		// give up...
  }
  // first chiffre has been read now

  // at least one more numeric chiffre:
  while (cb_stored(cb) > 0) {
    input = cb_read(cb);
    if (input >= '0' && input <= '9')	// numeric?
      num = 10 * num + (input - '0');
    else {				// NAN
      cb_recover_last(cb); // put NAN char back into input buffer
      break;
    }
  }

  // *if* we reach here change *value
  *value = sign * num;
  return;
  
 number_missing:
  serial_println_progmem(number_missing);
  return;
}



// bar_graph(value)
// print one value & bar graph line:
const unsigned char outOfRange[] PROGMEM = "out of range";
const unsigned char value_[] PROGMEM = "value ";

void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); serial_print_progmem(tab_);
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");
					// middle or top
      else if \
	((i == length/2 && value == 512) || (i == length && value == scale))
	Serial.print("|");
      else
	Serial.print("*");
    }
  } else {
    serial_print_progmem(value_);
    Serial.print(value);
    serial_println_progmem(outOfRange);
  }

  Serial.println();
}


// bar_graph_VU(pin):
// display a scrolling bar graph over the readings of an analog input.
//
// whenever the reading changes for more then +/- tolerance a new line is displayed
// tolerance can be changed by pressing '+' or '-'
// any other key stops VU display

// tolerance default 0. Let the user *see* the noise...
int bar_graph_tolerance=0;


// bar_graph_feedback(pin), info line for bar graph:
const unsigned char tolerance_[] PROGMEM = "tolerance ";

void feedback_tolerance() {
  serial_print_progmem(tolerance_);
  Serial.println(bar_graph_tolerance);
}


const unsigned char analog_reads_title[] PROGMEM = \
  "pin\tvalue\t|\t\t\t\t|\t\t\t\t|";


// bar_graph_VU(pin):
// display a scrolling bar graph over the readings of an analog input.
//
// whenever the reading changes for more then +/- tolerance a new line is displayed
// tolerance can be changed by pressing '+' or '-'
// any other key stops VU display

const unsigned char VU_title[] PROGMEM = \
  "values\t\t +/- set tolerance\t(q=stop)";
const unsigned char quit_[] PROGMEM = "(quit)";

void bar_graph_VU(int pin) {
  int value, oldValue=-9997;		// just an unlikely value
  int menu_input;

  feedback_tolerance();

  serial_println_progmem(VU_title);
  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > bar_graph_tolerance) {
    Serial.print("*A");
    Serial.print((int) pin);
    serial_print_progmem(tab_);
    bar_graph(value);
    oldValue = value;
    }

    if (Serial.available()) {
      switch (menu_input = Serial.read()) {
      case '+':
	bar_graph_tolerance++;
	feedback_tolerance();
	break;
      case '-':
	if (bar_graph_tolerance)
	  bar_graph_tolerance--;
	  feedback_tolerance();
	break;
      case '\n': case '\r':	// linebreak
        break;
      default:
	return;			// exit
      }
    }
  }    
}



// print binary numbers with leading zeroes and a space
void serial_print_BIN(unsigned long value, int bits) {
  int i;
  unsigned long mask=0;

  for (i = bits - 1; i >= 0; i--) {
    mask = (1 << i);
      if (value & mask)
	Serial.print(1);
      else
	Serial.print(0);
  }
  Serial.print(" ");
}



#ifdef HARDWARE_menu	// inside SERIAL_MENU
// ****************************************************************
// hw info display functions:


// pins_info_analog()
// display analog snapshot read values and bar graphs, a line each input:

/* const unsigned char analog_reads_title[] PROGMEM =	\
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";
*/
void pins_info_analog() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++)
    pin_info_analog(i);

  Serial.println();
}



// pin_info_digital()
// display configuration and state of a pin:
const unsigned char pin_[] PROGMEM = "pin ";
const unsigned char high_[] PROGMEM = "HIGH";
const unsigned char low_[] PROGMEM = "LOW";
const unsigned char pullup_[] PROGMEM = "pullup";
const unsigned char hiZ_[] PROGMEM = "hi-z";

void pin_info_digital(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;

  // selected sign * and pin:
  if (pin == PIN_digital )
    Serial.print("*");
  else
    Serial.print(" ");
  serial_print_progmem(pin_);
  Serial.print((int) pin);
  serial_print_progmem(tab_);

  // input or output?
  reg = portModeRegister(port);
  // uint8_t oldSREG = SREG;	// let interrupt ACTIVE ;)
  // cli();
  if (*reg & mask) {		// digital OUTPUTS
    // SREG = oldSREG;
    Serial.print("O  ");

    // high or low?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // HIGH
      // SREG = oldSREG;
      serial_print_progmem(high_);
    } else {			    // LOW
      // SREG = oldSREG;
      serial_print_progmem(low_);
    }
  } else {			// digital INPUTS
    // SREG = oldSREG;
    Serial.print("I  ");

    // pullup, tristate?
    reg = portOutputRegister(port);
    // oldSREG = SREG;		// let interrupt ACTIVE ;)
    // cli();
    if (*reg & mask) {		    // pull up resistor
      // SREG = oldSREG;
      serial_print_progmem(pullup_);
    } else {			    // tri state high-Z
      // SREG = oldSREG;
      serial_print_progmem(hiZ_);
    }
  }
  Serial.println();
}



// display configuration and state of all digital pins:
void pins_info_digital() {
  for (uint8_t pin=0; pin<DIGITAL_PINs; pin++)
    pin_info_digital(pin);
}


// display analog pin name and value as number and bar graph: 
void pin_info_analog(uint8_t pin) {
  if (pin == PIN_analog)
    Serial.print("*A");
  else
    Serial.print(" A");
  Serial.print((int) pin);
  serial_print_progmem(tab_);
  bar_graph(analogRead(pin));
}

// watch_digital_input(pin)
// continuously display digital input readings, whenever the input changes:
const unsigned char watchingPin[] PROGMEM = "watching pin D";
const unsigned char qStop[] PROGMEM = "\t\t(q=stop)";

void watch_digital_input(int pin) {
  int value, old_value=-9997;

  serial_print_progmem(watchingPin);
  Serial.print((int) pin);
  serial_println_progmem(qStop);

  while (!Serial.available()) {
    value = digitalRead(PIN_digital);
    if (value != old_value) {
      old_value = value;
      Serial.print("*D");  Serial.print((int) pin);
      serial_print_progmem(tab_);
      if (value)
	serial_println_progmem(high_);
      else
	serial_println_progmem(low_);
    }
  }
  Serial.read();
  serial_println_progmem(quit_);
}




// ****************************************************************
// menu hardware display and reaction:

// menu_hardware_display()  display hardware menu:
const unsigned char hwMenuTitle[] PROGMEM = \
  "\n*** HARDWARE MENU ***\t\tfree RAM=";
const unsigned char select_[] PROGMEM = "select ";
const unsigned char _digital[] PROGMEM = "digital";
const unsigned char _analog[] PROGMEM = "analog";
const unsigned char pinFor[] PROGMEM = " pin for ";
const unsigned char digitalKeys[] PROGMEM = \
  "'d, r, I, O, H, L, W'";
const unsigned char analogKeys[] PROGMEM = "'a, v'";
const unsigned char toWork_[] PROGMEM = " to work on:\t";
const unsigned char pin__[] PROGMEM = "pin (";
const unsigned char _close[] PROGMEM = ")";
const unsigned char OIHLWd[] PROGMEM = \
  "O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tpwm: W=WRITE\td=pin info";
const unsigned char v_r[] PROGMEM = "watch over time:\tv=VU bar\tr=read";

const unsigned char all_[] PROGMEM = ".=all digital\t,=all analog\t;=both";

// factored out display functions
void _select_digital(bool key) {
  if (key)
    Serial.print("D=");
  serial_print_progmem(select_);
  serial_print_progmem(_digital);
  serial_print_progmem(pinFor);
  serial_print_progmem(digitalKeys);
}

void _select_analog(bool key) {
  if (key)
    Serial.print("A=");
  serial_print_progmem(select_);
  serial_print_progmem(_analog);
  serial_print_progmem(pinFor);
  serial_print_progmem(analogKeys);
}

void menu_hardware_display() {
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  Serial.println();

  _select_digital(true);
  serial_print_progmem(toWork_);
  serial_print_progmem(pin__);
  if (PIN_digital == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) PIN_digital);
  serial_println_progmem(_close);
  serial_println_progmem(OIHLWd);
  Serial.println();

  _select_analog(true);
  serial_print_progmem(toWork_);
  serial_print_progmem(pin__);
  Serial.print((int) PIN_analog);
  serial_println_progmem(_close);
  serial_println_progmem(v_r);
  Serial.println();

  serial_println_progmem(all_);
}

// menu reactions:
// factored out helper function
// digital_pin_ok()  checks if PIN_digital has been set
bool digital_pin_ok() {
  // testing on ILLEGAL is enough in this context
  if (PIN_digital == ILLEGAL) {
    _select_digital(true);
    Serial.println();
    return false;
  } else
    return true;
}

// bool menu_hardware_reaction(menu_input)
// try to react on menu_input, return success flag
const unsigned char pwmWrite[] PROGMEM = "pwm write ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";

bool menu_hardware_reaction(char menu_input) {
  long newValue;

  switch (menu_input) {
  case 'A':
    _select_analog(false);
    serial_print_progmem(tab_);

    newValue = PIN_analog;
    numeric_input(&serial_input_BUFFER, &newValue);
    if (newValue>=0 && newValue<ANALOG_INPUTs)
      PIN_analog = newValue;
    else
      serial_println_progmem(outOfRange);

    serial_print_progmem(pin_);
    Serial.println((int) PIN_analog);
    break;

  case 'D':
    _select_digital(false);
    serial_print_progmem(tab_);

    newValue = PIN_digital;
    numeric_input(&serial_input_BUFFER, &newValue);
    if (newValue>=0 && newValue<DIGITAL_PINs) {
      PIN_digital = newValue;
      pin_info_digital((int) PIN_digital);
    } else
      if (newValue != ILLEGAL)
	serial_println_progmem(outOfRange);
    break;

  case 'O':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, OUTPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'I':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, INPUT);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'H':
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, HIGH);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'L':
    if (digital_pin_ok()) {
      digitalWrite(PIN_digital, LOW);
      pin_info_digital(PIN_digital);
    }
    break;

  case 'W':
    if (digital_pin_ok()) {
      // ################ which pins can do pwm? ############################
      serial_print_progmem(pwmWrite);
      newValue = -1;
      numeric_input(&serial_input_BUFFER, &newValue);
      if (newValue>=0 && newValue<=255) {
	Serial.println(newValue);

	analogWrite(PIN_digital, newValue);
	serial_print_progmem(analogWrite_); Serial.print((int) PIN_digital);
	Serial.print(", "); Serial.print(newValue);
	serial_println_progmem(_close);
      } else
	serial_println_progmem(outOfRange);
    }
    break;

  case 'a':
    serial_println_progmem(analog_reads_title);
    pin_info_analog(PIN_analog);
    break;

  case 'd':
    if (digital_pin_ok())
      pin_info_digital(PIN_digital);
    break;

  case 'v':
    bar_graph_VU(PIN_analog);
    break;

  case 'r':
    if (digital_pin_ok()) {
      pinMode(PIN_digital, INPUT);	// ################
      watch_digital_input(PIN_digital);
    }
    break;

  case '.':	// all digital
    Serial.println();
    pins_info_digital();
    Serial.println();
    break;

  case ',':	// all analog
    pins_info_analog();
    break;

  case ';':	// both ;)
    Serial.println();
    pins_info_analog();
    pins_info_digital();
    Serial.println();
    break;

  default:
    return 0;		// menu_input not found in this menu
  }
  return 1;		// menu_input found in this menu
}
#endif // HARDWARE_menu




/* **************************************************************** */
//    MENU core
/* **************************************************************** */
#if (defined(SERIAL_MENU) || defined(MENU_LCD) ) 
  // global menu variable switches active menu:

  // menu codes: (codes for non-existing menus are not a problem)
  #define MENU_CODE_UNDECIDED	0
  #define MENU_CODE_PROGRAM	1
  #define MENU_CODE_HARDWARE	2

  // unsigned char menu;  holds the code of the active menu.
  // normally it would default to
  // unsigned char menu=MENU_CODE_UNDECIDED;	// normal default
  // as this version comes with hw menu only and could have been extended
  // by the user with his own program menu I do here
  #ifdef PROGRAM_menu
    unsigned char menu=MENU_CODE_PROGRAM;	// program menu exists
  #else
    unsigned char menu=MENU_CODE_HARDWARE;	// hw menu only
  #endif

#endif	// (SERIAL_MENU || MENU_LCD )




// ****************************************************************
// inside #defined SERIAL_MENU
// top level serial menu display and reactions:


// serial_menu_common_display()
// display menu items common to all menus:
const unsigned char common_[] PROGMEM = \
  "\nPress 'm' or '?' for menu, 'e' toggle echo, 'q' quit this menu.";

#ifdef PROGRAM_menu
const unsigned char program_[] PROGMEM = \
  " 'P' program menu ";
#endif

#ifdef HARDWARE_menu
const unsigned char hardware_[] PROGMEM = \
  " 'H' hardware menu ";
#endif

// serial_menu_common_display()
// display menu items common to all menus:
void serial_menu_common_display() {
  serial_print_progmem(common_);
#ifdef PROGRAM_menu
  if (menu != MENU_CODE_PROGRAM)
    serial_print_progmem(program_);
#endif
#ifdef HARDWARE_menu
  if (menu != MENU_CODE_HARDWARE)
    serial_print_progmem(hardware_);
#endif
}



// serial_menu_display()
// top level serial menu display function
void serial_menu_display() {
  serial_print_progmem(programLongName);
  Serial.println(serial_input_BUF_size);

  switch (menu) {
  case MENU_CODE_UNDECIDED:
#ifdef PROGRAM_menu
  case MENU_CODE_PROGRAM:
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case MENU_CODE_HARDWARE:
    menu_hardware_display();
    break;
#endif

  default:		// ERROR: unknown menu code
    ;
  }
  serial_menu_common_display();
  Serial.println();

  Serial.println();
}



// serial_menu_common_reaction(menu_input)
// test menu_input for being a common menu entry key
// if yes, do it
// return success flag:
bool serial_menu_common_reaction(char menu_input) {
  switch (menu_input) {
  case 'm': case '?':	// menu
    serial_menu_display();
    break;

  case 'e':	// toggle echo
    echo_switch = !echo_switch;
    break;

  case 'q':	// quit
    menu=MENU_CODE_UNDECIDED;
    serial_menu_display();
    break;
#ifdef PROGRAM_menu
  case 'P':	// PROGRAM menu
    menu = MENU_CODE_PROGRAM;
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case 'H':	// HARDWARE menu
    menu = MENU_CODE_HARDWARE;
    serial_menu_display();
    break;
#endif
  default:
    return false;	// menu entry not found
  }
  return true;		// menu entry found
}

/* int do_menu_actions()
   done on receiving an end-of-data-package token.
*/
int do_menu_actions() {
  char inp;
  while (cb_stored(&serial_input_BUFFER)) {
    inp = cb_read(&serial_input_BUFFER);
    serial_menu_reaction(inp);
  }
}


const unsigned char bufferFull[] PROGMEM = "BUFFER FULL!";
/* serial_menu()
   Top level function for user sketch main loop.
   Collect serial input bytes until a data package is complete, then react.
   Do *not* wait for input, can be used for run-through technique.

   If the serial input buffer gets full before receiving an end token
   an error message "BUFFER FULL!" is displayed, and an attempt is made
   to take use the buffer content as menu input. No attempt is made to be
   clever about multy byte tokens (like numbers) where it *will* fail...
*/
int serial_menu() {
  if (!Serial.available())
    return 0;

  char token;
  switch (token = Serial.read()) {
  case '\n':  case '\r':	/* set \0 end token, case '\0': implizit */
    token=0;
  }
  // accumulate
  cb_write(&serial_input_BUFFER, token);

  if (token) {
    if (echo_switch)
      Serial.print(token);

    if (!cb_is_full(&serial_input_BUFFER)) {
      return 0;			// not end token, buffer not full, done

    } else {			// not end, but buffer full: *undefined*
      serial_println_progmem(bufferFull);
      // simple last resort: just try to do it ;)
      // this *will* fail with multibyte items like numbers
    }
  }
  if (echo_switch)
    Serial.println();
  // token == 0 || cb_is_full(&serial_input_BUFFER)

  // cr-lf and it´s cracy sisters produce empty packages
  // drop them here
  if (cb_stored(&serial_input_BUFFER) == 1) {	// empty package
    cb_read(&serial_input_BUFFER);		// drop end token
    return 0;
  }

  return do_menu_actions();			// react
}

// serial_menu_reaction(menu_input), react on serial menu input:
// called after receiving an end token (or on buffer overflow)
// and will get called repeatedly on all pending input tokens.
const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

void serial_menu_reaction(char menu_input) {
  bool found=false;

  switch (menu_input) {	// submenu forking
  case ' ': case '\t':			// skip white chars
  case '\0':				// '\0' is end token
// case '\n': case '\r':			// should not happen
    break;

  default:				// no whitespace, check menus:
    switch (menu) {			// check active menu:
    case MENU_CODE_UNDECIDED:
#ifdef PROGRAM_menu
    case MENU_CODE_PROGRAM:
      menu=MENU_CODE_PROGRAM;		// case MENU_CODE_UNDECIDED
      found = serial_menu_program_reaction(menu_input);
      break;
#endif
#ifdef HARDWARE_menu
    case MENU_CODE_HARDWARE:
      menu=MENU_CODE_HARDWARE;		// case MENU_CODE_UNDECIDED
      found = menu_hardware_reaction(menu_input);
      break;
#endif
    default:				// ERROR: unknown menu code
      ;
    } // menu branching

    if (!found)				// common menu entry?
      found = serial_menu_common_reaction(menu_input);

    if (!found) {			// unknown menu entry
      serial_print_progmem(unknownMenuInput);
      serial_print_progmem(tab_);
      char token = menu_input;
      Serial.print(token);
      while (cb_stored(&serial_input_BUFFER)) {
	if (token = cb_read(&serial_input_BUFFER)) // omit end token
	  Serial.print(token);
      }
      Serial.println();
    } // unknown menu entry

  } // submenu forking
} // serial_menu_reaction()


#endif	// SERIAL_MENU
/* **************************************************************** */



/* **************************************************************** */
#if defined(PROGRAM_menu) && defined(SERIAL_MENU)
/*
    Define your own program specific menu here.
    To integrate your own program menu  #define PROGRAM_menu 
    See comments near the top of this file.
    
    Write functions:
    	menu_program_display()  serial_menu_program_reaction()
    Edit:
    	ProgramMenuTitle_  ProgramMenuKeys_
    
    Please see below.
*/

/*
    This code is just a template stub to show how it works.
    Change everything here to match your own needs.
*/

/*
    To save RAM you should store all strings you need in program memory:
	const unsigned char MY_STRING_NAME[] PROGMEM = "my string";
    Print it with:
	serial_print_progmem(MY_STRING_NAME); 
	serial_println_progmem(MY_STRING_NAME); 
*/


// Edit the following two strings:

// ProgramMenuTitle_  title of your program menu:
// (the '\n' lets it start with an empty line)
const unsigned char ProgramMenuTitle_[] PROGMEM = \
  "\n*** MY PROGRAM MENU ***";

// ProgramMenuKeys_  user infos about active menu keys:
// ('\t' means a tab between entries)
const unsigned char ProgramMenuKeys_[] PROGMEM = \
  "  d=dance\ty=yodel \ts=something special\tb=get bonus points";


// Add whatever you want, like...
long bonus_points=0;

void dance() {
  bonus_points++;
}

void yodel() {
  dance();
  dance();
}

const unsigned char something[] PROGMEM = \
  "* ** *** go and write that code now *** ** *";

void do_something_special() {
  dance();
  yodel();
  serial_println_progmem(something);
}

const unsigned char bonusPoints[] PROGMEM = "bonus points : ";
void bonus_info() {
  serial_print_progmem(bonusPoints);
  Serial.println(bonus_points);
}

const unsigned char addedTo[] PROGMEM = "added to ";

void add_bonus(long points) {
  if (points) {
    bonus_points += points;

    serial_print_progmem(addedTo);
    serial_print_progmem(bonusPoints);
    Serial.println(points);
  }
}


// Fill the following two functions with your own functionality:

// void menu_program_display()
// Display your program menu. Give infos about active keys.
void menu_program_display() {
  // menu title:
  serial_println_progmem(ProgramMenuTitle_);
  Serial.println();

  // inform user about active keys:
  serial_println_progmem(ProgramMenuKeys_);	// show menu keys

  // example using a numeric value
  if (bonus_points)
    bonus_info();
}


// serial_menu_program_reaction(char menu_input)
// React on menu input.
//
// Will be called on each menu input byte:
// 	check menu_input for matching keys,
// 	react on valid input, call your sketch functions.
// 	return success flag,
// 		true  if a matching entry was found.
// 		false if no matching entry was found.
bool serial_menu_program_reaction(char menu_input) {
  long newValue;	// for numeric input

  switch (menu_input) {
    // Insert a case statement for each menu key and
    // call the code you want to be run when the key is received:    

  case 'd':
    dance();
    bonus_info();
    break;

  case 'y':
    yodel();
    bonus_info();
    break;

  case 's':
    do_something_special();
    break;

  case 'b':
    // numeric input:
    // a sequence of chiffres following the 'b'
    newValue = 0;
    numeric_input(&serial_input_BUFFER, &newValue);

    // add bonus points:
    add_bonus(newValue);

    // inform user:
    bonus_info();

    // end the code with a break;
    break;


  // * ** *** DO NOT CHANGE THE FOLLOWING LINES *** ** *
  default:
    return false;	// menu_input not found in this menu
  }
  return true;		// menu_input found in this menu
}

#endif	// #if defined(PROGRAM_menu) && defined(SERIAL_MENU)
/* **************************************************************** */



/* **************************************************************** */
// setup and main loop:

void setup() {

// You can switch all serial line and menu code *off*
// by not defining USE_SERIAL_BAUD
#ifdef USE_SERIAL_BAUD
  Serial.begin(USE_SERIAL_BAUD);

  #ifdef SERIAL_MENU
    // circ_buf *serial_input_BUFFER;
    cb_init(&serial_input_BUFFER, serial_input_BUF_size);
  #endif
#endif

//	#ifdef USE_LCD
//	  LCD.begin(LCD_COLs, LCD_ROWs);
//	  LCD_print_line_progmem(0, programName);
//	  LCD_print_line_progmem(1, version);
//	  LCD_print_line_progmem(3, freeRAM);
//	  LCD.print(get_free_RAM());
//	#endif


// startup messages:
#ifdef USE_SERIAL_BAUD
  #ifdef SERIAL_MENU		// show message about menu
    if (menu == MENU_CODE_HARDWARE )
      {
	pins_info_digital();
	pins_info_analog();
	Serial.println();
      }

    serial_menu_display();

  #else				// no menu, show program name
    Serial.println();
    serial_print_progmem(programLongName);
    Serial.println(serial_input_BUF_size);
  #endif
#endif

//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif

}



/* **************************************************************** */

void loop() {

#ifdef SERIAL_MENU
  /* accumulate serial input bytes until end token '\n' | '\c' | '\0'
     then react and do the menu actions
     run-through technique:  does *not* wait or delay
  */
  serial_menu();
#endif

}

/* **************************************************************** */
