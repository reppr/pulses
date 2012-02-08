/* **************************************************************** */
// serial_menu alias softboard for the arduino
/* ****************************************************************

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

   **************************************************************** */
/*

   Arduino software breadboard.
   Hardware/software developing/testing tool.

   Simple hardware menu interface over serial line
   as a kind of software breadboard
   Tool for hardware/software developing/testing.

   Read/set I/O pin configuration and states, switch pins on/off.
   Read and write digital and analog values, switch pullup/high-z,
   watch changing inputs over time.

   Send 'm' over serial line and a menu will be sent back
   displaying info on its one-letter commands.

   Easy to write your own menu, but not documented yet...
   Use it together with your own programs.


   Installation:

   Get it from http://github.com/reppr/softboard
   Move the 'softboard' folder with all files
   to your arduino sketchbook/ folder.
   Load the sketch from arduino menu Files>Sketchbook>softboard.

   Older arduino versions than 1.0 need a fix:
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

   Communicate over serial. The minimalistic menu shows you one letter
   commands to send. The menu reacts on these commands and reads numbers.



   Example 1:  'P13 OH' switch LED on   (P select pin, O output, H high)
               'L'      off again       (L low)


   Example 2:  'P13 IH' pullup LED pin  (P select pin, I input, H high)
                                        LED glows at low level
               'O'      LED as OUTPUT   now the LED is on


   Example 3:  Watch an analog input like a VU meter, changing over time.
               See electric noise on unconnected floating A2 input
               scrolling over your serial terminal.
               (Touch the input if there is no visible signal.)

               Now connect a sensor to the input and explore its data...

               'P2 v'      P=select pin (both analogue or digital)
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



   Example 4:  'a'      Display snapshot values on analog inputs.

                        [fixed font only]
pin     value   |                               |                               |
0       609     ***************************************
1       504     ********************************
2       451     *****************************
3       398     *************************
4       383     ************************
5       409     **************************



   Example 5:  '.'      Display info about all pin's I/O configuration and states.

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


*/
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

// #define USE_SERIAL_BAUD
// to switch on serial #define USE_SERIAL_BAUD <baud>  (otherwise serial will be *off*)
//
// You can switch all serial line and menu code *off* by not defining USE_SERIAL_BAUD
// You can also do this later to save program memory.
//   your sketch could include the menu, but later grow too much,
//   or you'd want to move to another processor with less program memory.
//
// #define USE_SERIAL_BAUD	115200		// works fine here
#define USE_SERIAL_BAUD	57600
// #define USE_SERIAL_BAUD	38400

#ifdef USE_SERIAL_BAUD	// activate minimalistic menus over serial line?
  // menu over serial, basics:
  #define MENU_over_serial	// we *do* use serial menu

  // simple menu to access arduino hardware:
  #define HARDWARE_menu		// menu interface to hardware configuration
  	  			// this will let you read digital and analog inputs
  				// watch changes on inputs as value or as bar graphs
  	  			// set digital and analog outputs, etc.
  #ifdef HARDWARE_menu
    char hw_PIN = ILLEGAL;
  #endif // HARDWARE_menu
#else
  #error #define   USE_SERIAL_BAUD <baud>   in file softboard.ino
#endif	// USE_SERIAL_BAUD



/* **************************************************************** */
// board specific things:
#if defined(NUM_ANALOG_INPUTS) && defined(NUM_DIGITAL_PINS)
  #define ANALOG_INPUTs	NUM_ANALOG_INPUTS
  #define DIGITAL_PINs	(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)
#else	// savety net
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
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
const unsigned char programLongName[] PROGMEM = "*** serial menu v0.1 ***";
const unsigned char version[] PROGMEM = "version 0.1";

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
// only MENU_over_serial implemented
//
// we could add other menus, like LCD or MIDI or whoknows,
// but nothing there in this version, sorry.
//
/* **************************************************************** */
// #define MENU_over_serial	// do we use serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial


// inside  #ifdef MENU_over_serial
// ****************************************************************
// basic menu I/O:

// sometimes serial is not ready quick enough:
#define WAITforSERIAL 10



// char input with one byte buffering:
// (I have not needed more then one char yet...)
char stored_char, chars_stored=0;



int get_char() {
  if(!char_available())
    return ILLEGAL;		// EOF no input available

  if (chars_stored) {
    --chars_stored;
    return stored_char;
  } else if (Serial.available())
    return Serial.read();
  else
    return ILLEGAL;		// EOF no input available
}



const unsigned char storeFull[] PROGMEM = "char_store: sorry, buffer full.";
int char_store(char c) {
  if (chars_stored) {	// ERROR.  I have not needed more then one char yet...
    serial_println_progmem(storeFull);
  }

  chars_stored++;
  stored_char = c;
}



int char_available() {
  if (chars_stored || Serial.available()) 
    return 1;
  return 0;
}



// inside  #ifdef MENU_over_serial
// ****************************************************************
// menu I/O functions:

// get numeric input from serial
long numeric_input(long oldValue) {
  long input, num, sign=1;

  do {
    while (!char_available())	// wait for input
      ;

    delay(WAITforSERIAL);	// sometimes the second byte was not ready without that


    input = get_char();		// get first chiffre
  } while (input == ' ');	// skip leading space

  if (input == '-') {		//	check for sign
    sign = -1;
    input = get_char(); }
  else if (input == '+')
    input = get_char();

  if (input >= '0' && input <= '9')	// numeric?
    num = input - '0';
  else {				// NAN
    char_store(input);
    return oldValue;
  }

  while (char_available()) {
    input = get_char();
    if (input >= '0' && input <= '9')	// numeric?
      num = 10 * num + (input - '0');
    else {
      char_store(input);	// put NAN chars back into input buffer
      break;
    }
  }

  return sign * num;
}



// bar_graph()
// print one value & bar graph line:
const unsigned char outOfRange[] PROGMEM = " out of range.";
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



// follow_info(pin)
// info line for bar graph:
const unsigned char followPin[] PROGMEM = "Follow pin ";
const unsigned char tolerance_[] PROGMEM = "\ttolerance ";

void follow_info(int pin) {
  // display info about pin and tolerance
  serial_print_progmem(followPin);
  Serial.print((int) pin);
  serial_print_progmem(tolerance_);
  Serial.println(bar_graph_tolerance);
}



// bar_graph_VU(pin):
// display a scrolling bar graph over the readings of an analog input.
//
// whenever the reading changes for more then +/- tolerance a new line is displayed
// tolerance can be changed by pressing '+' or '-'
// any other key stops VU display

const unsigned char VU_title[] PROGMEM = \
  "values\t\t +/- set tolerance\t(any other byte to stop)\n";
const unsigned char quit_[] PROGMEM = "(quit)";

void bar_graph_VU(int pin) {
  int value, oldValue=-9997;		// just an unlikely value
  int menu_input;

  follow_info(pin);

  serial_println_progmem(VU_title);
  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > bar_graph_tolerance) {
      bar_graph(value);
      oldValue = value;
    }

    if (char_available()) {
      switch (menu_input = get_char()) {
      case '+':
	bar_graph_tolerance++;
	follow_info(pin);
	break;
      case '-':
	if (bar_graph_tolerance)
	  bar_graph_tolerance--;
	  follow_info(pin);
	break;
      case '\n': case '\r':	// linebreak after sending 'V'
        break;
      default:
	serial_println_progmem(quit_);
	return;		// exit
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



#ifdef HARDWARE_menu	// inside MENU_over_serial
// ****************************************************************
// hw info display functions:


// display_analog_reads()
// display analog snapshot read values and bar graphs, a line each input:
const unsigned char analog_reads_title[] PROGMEM = \
  "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void display_analog_reads() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); serial_print_progmem(tab_); bar_graph(value);
  }

  Serial.println();
}



// pin_info()
// display configuration and state of a pin:
const unsigned char pin_[] PROGMEM = "pin ";
const unsigned char high_[] PROGMEM = "high";
const unsigned char low_[] PROGMEM = "low";
const unsigned char pullup_[] PROGMEM = "pullup";
const unsigned char hiZ_[] PROGMEM = "hi-z";

void pin_info(uint8_t pin) {
  uint8_t mask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg;

  if (port == NOT_A_PIN) return;

  // selected sign * and pin:
  if (pin == hw_PIN )
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



// display configuration and state of all pins:
void pins_info() {
  for (uint8_t pin=0; pin<DIGITAL_PINs; pin++)
    pin_info(pin);
}




// watch_digital_input(pin)
// continuously display digital input readings, whenever the input changes:
const unsigned char watchingINpin[] PROGMEM = "watching digital input pin ";
const unsigned char anyStop[] PROGMEM = "\t\t(send any byte to stop)";
const unsigned char is_[] PROGMEM = " is ";

void watch_digital_input(int pin) {
  int value, old_value=-9997;

  serial_print_progmem(watchingINpin);
  Serial.print((int) pin);
  serial_println_progmem(anyStop);

  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      serial_print_progmem(pin_); Serial.print((int) pin);
      serial_print_progmem(is_);
      if (value)
	serial_println_progmem(high_);
      else
	serial_println_progmem(low_);
    }
  }
  serial_println_progmem(quit_);
  get_char();
}




// ****************************************************************
// menu hardware display and reaction:

// menu_hardware_display()  display hardware menu:
const unsigned char hwMenuTitle[] PROGMEM = \
  "\n*** HARDWARE MENU ***\t\tfree RAM=";
const unsigned char selectPin[] PROGMEM = \
  "P=select pin for 'I, O, H, L, r, W, d, v' to work on ";
const unsigned char PPin[] PROGMEM = "\tP=pin (";
const unsigned char OIHLWr[] PROGMEM = \
  "O=OUTPUT\tI=INPUT\t\tH=HIGH\tL=LOW\tanalog:\tW=WRITE\t    r=read";
const unsigned char dv_[] PROGMEM = "d=digiwatch\tv=VU bar\t";
const unsigned char aAnalog_[] PROGMEM = "a=all analog\t.=pins_info";

void menu_hardware_display() {
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  Serial.println();

  serial_print_progmem(selectPin);
  serial_print_progmem(PPin);
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")");

  serial_println_progmem(OIHLWr);
  serial_print_progmem(dv_);
  serial_println_progmem(aAnalog_);
}




// please_select_pin()
// give a warning that no valid pin was selected
void please_select_pin() {
  serial_println_progmem(selectPin);
}



// bool menu_hardware_reaction(menu_input)
// try to react on menu_input, return success flag
const unsigned char none_[] PROGMEM = "(none)";
const unsigned char analogWriteValue[] PROGMEM = "analog write value ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";
const unsigned char analogValueOnPin[] PROGMEM = "analog value on pin ";
// const unsigned char bytes_[] PROGMEM = " bytes";
// const unsigned char freeRAM[] PROGMEM = "free RAM: ";

bool menu_hardware_reaction(char menu_input) {
  long newValue;

  switch (menu_input) {
//  // DEACTIVATED as it is in the menu already
//  case  'M':
//    serial_print_progmem(freeRAM);
//    Serial.print(get_free_RAM());
//    serial_println_progmem(bytes_);
//    break;

  case 'P':	// uppercase/lowercase
  case 'p':	// we accept lowercase as it does not change the chip
    serial_print_progmem(selectPin);
    serial_print_progmem(tab_);

    newValue = numeric_input(hw_PIN);
    if (newValue>=0 && newValue<DIGITAL_PINs) {
      hw_PIN = newValue;
      pin_info(hw_PIN);
    } else
      serial_println_progmem(none_);
    break;

  case 'O':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, OUTPUT);
      pin_info(hw_PIN);
    }
    break;

  case 'I':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, INPUT);
      pin_info(hw_PIN);
    }
    break;

 case 'H':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      digitalWrite(hw_PIN, HIGH);
      pin_info(hw_PIN);
    }
    break;

  case 'L':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      digitalWrite(hw_PIN, LOW);
      pin_info(hw_PIN);
    }
    break;

  case 'W':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      serial_print_progmem(analogWriteValue);
      newValue = numeric_input(-1);
      if (newValue>=0 && newValue<=255) {
	Serial.println(newValue);

	analogWrite(hw_PIN, newValue);
	serial_print_progmem(analogWrite_); Serial.print((int) hw_PIN);
	Serial.print(", "); Serial.print(newValue); Serial.println(")");
      } else
	serial_println_progmem(quit_);
    }
    break;

  case 'r':
    if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
      please_select_pin();
    else {
      serial_print_progmem(analogValueOnPin); Serial.print((int) hw_PIN);
      serial_print_progmem(is_); Serial.println(analogRead(hw_PIN));
    }
    break;

  case 'v':
  case 'V':	// ok i'm still used to uppercase V
    if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
      please_select_pin();
    else
      bar_graph_VU(hw_PIN);
    break;

  case 'd':
    if (hw_PIN == ILLEGAL)
      please_select_pin();
    else {
      pinMode(hw_PIN, INPUT);
      pin_info(hw_PIN);
      watch_digital_input(hw_PIN);
    }
    break;

  case 'a':
    display_analog_reads();
    break;

  case '.':
    Serial.println();
    pins_info();
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
#if (defined(MENU_over_serial) || defined(MENU_LCD) ) 
  // global menu variable switches active menu:

  // menu codes: (codes for non-existing menus are not a problem)
  #define MENU_UNDECIDED	0
  #define MENU_PROGRAM		1
  #define MENU_HARDWARE		2

  // unsigned char menu=MENU_UNDECIDED;	// normally better default
  //
  // but for this version with HARDWARE_menu only i take:
  unsigned char menu=MENU_HARDWARE;

#endif	// (MENU_over_serial || MENU_LCD )




// ****************************************************************
// inside #defined MENU_over_serial
// top level serial menu display and reactions:


// menu_serial_common_display()
// display menu items common to all menus:
const unsigned char common_[] PROGMEM = \
  "\nPress 'm' or '?' for menu, 'q' quit this menu.";

#ifdef PROGRAM_menu
const unsigned char program_[] PROGMEM = \
  " 'P' program menu ";
#endif

#ifdef HARDWARE_menu
const unsigned char hardware_[] PROGMEM = \
  " 'H' hardware menu ";
#endif

// menu_serial_common_display()
// display menu items common to all menus:
void menu_serial_common_display() {
  serial_print_progmem(common_);
#ifdef PROGRAM_menu
  if (menu != MENU_PROGRAM)
    if (menu != MENU_HARDWARE)		// hardware menu hides 'P'
      serial_print_progmem(program_);
#endif
#ifdef HARDWARE_menu
  if (menu != MENU_HARDWARE)
    serial_print_progmem(hardware_);
#endif
}



// menu_serial_display()
// top level serial menu display function
void menu_serial_display() {
  serial_println_progmem(programLongName);

  switch (menu) {
  case MENU_UNDECIDED:
#ifdef PROGRAM_menu
  case MENU_PROGRAM:
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case MENU_HARDWARE:
    menu_hardware_display();
    break;
#endif

  default:		// ERROR: unknown menu code
    ;
  }
  menu_serial_common_display();
  Serial.println();

  Serial.println();
}



// menu_serial_common_reaction(menu_input)
// test menu_input for being a common menu entry key
// if yes, do it
// return success flag:
bool menu_serial_common_reaction(char menu_input) {
  switch (menu_input) {
  case 'm': case '?':	// menu
    menu_serial_display();
    break;

  case 'q':	// quit
    menu=MENU_UNDECIDED;
    menu_serial_display();
    break;
#ifdef PROGRAM_menu
  case 'P':	// PROGRAM menu
    menu = MENU_PROGRAM;
    menu_program_display();
    break;
#endif
#ifdef HARDWARE_menu
  case 'H':	// HARDWARE menu
    menu = MENU_HARDWARE;
    menu_serial_display();
    break;
#endif
  default:
    return false;	// menu entry not found
  }
  return true;		// menu entry found
}



// menu_serial_reaction(), react on serial menu input.
// check for serial input, wait if there´s none
// react on all available serial input
const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

void menu_serial_reaction() {
  char menu_input;
  bool found;

  while(!char_available())
    ;

  if (char_available()) {
    while (char_available()) {
      found=false;

      switch (menu_input = get_char()) {	// submenu forking

      case ' ': case '\t':		// skip white chars
      case '\n': case '\r':		// skip newlines
	break;

      default:				// no whitespace, check menus:
	switch (menu) {			// check active menu:
	case MENU_UNDECIDED:
#ifdef PROGRAM_menu
	case MENU_PROGRAM:
	  found = menu_serial_program_reaction(menu_input);
	  break;
#endif
#ifdef HARDWARE_menu
	case MENU_HARDWARE:
	  found = menu_hardware_reaction(menu_input);
	  break;
#endif
	default:		// ERROR: unknown menu code
	  ;
	} // menu branching

	if (!found)		// common menu entry?
	  found = menu_serial_common_reaction(menu_input);

	if (!found) {		// unknown menu entry
	  serial_print_progmem(unknownMenuInput); Serial.println(menu_input);
	  while (char_available() > 0) {
	    menu_input = get_char();
	    Serial.print(byte(menu_input));
	  }
	  Serial.println();
	  break;
	}
      } // submenu forking

      if (!char_available())
	delay(WAITforSERIAL);
    } // input loop
  } // any input?
} // menu_serial_reaction()


#endif	// MENU_over_serial
/* **************************************************************** */



/* **************************************************************** */
// setup and main loop:

void setup() {

// You can switch all serial line and menu code *off*
// by not defining USE_SERIAL_BAUD
#ifdef USE_SERIAL_BAUD
  Serial.begin(USE_SERIAL_BAUD);
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
  #ifdef MENU_over_serial	// show message about menu
    menu_serial_display();
    pins_info();
    Serial.println();
  #else				// no menu, show program name
    Serial.println();
    serial_println_progmem(programLongName);
  #endif
#endif
//
//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif

}



/* **************************************************************** */

void loop() {

#ifdef MENU_over_serial
  if (char_available())
    menu_serial_reaction();
#endif

}

/* **************************************************************** */
