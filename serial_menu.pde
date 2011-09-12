/* **************************************************************** */
// serial_menu
/* **************************************************************** */


/* **************************************************************** */
/*
   Simple menu interface over serial line.
   To save RAM constant strings are stored in PROGMEM.

*/
/* **************************************************************** */



/* **************************************************************** */
#define ILLEGAL		-1



/* **************************************************************** */
// CONFIGURATION:

// use serial line for communication? Define serial speed:
//	#define USE_SERIAL	115200
#define USE_SERIAL	57600
//	#define USE_SERIAL	38400

#ifdef USE_SERIAL	// simple menus over serial line?
  // menu over serial line, basics:
  #define MENU_over_serial	// do we use a serial menu?

  // simple menu to access arduino hardware:
  #define HARDWARE_menu		// menu interface to hardware configuration
  	  			// this will let you read digital and analog inputs
  				// watch changes on inputs as value or as bar graphs
  	  			// set digital and analog outputs, etc.
  #ifdef HARDWARE_menu
    char hw_PIN = ILLEGAL;
  #endif // HARDWARE_menu
#endif	// USE_SERIAL



/* **************************************************************** */
// board specific things:
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
  #define ANALOG_INPUTs	16
  #define DIGITAL_PINs	54
#else								// 168/328 boards
  #define ANALOG_INPUTs	6
  #define DIGITAL_PINs	14
#endif	// board specific initialisations

#define LED_PIN	13



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
// use PROGMEM to save RAM:
#include <avr/pgmspace.h>

const unsigned char programName[] PROGMEM = "SERIAL MENU";
const unsigned char programLongName[] PROGMEM = "*** SERIAL MENU v0.0 ***";
const unsigned char version[] PROGMEM = "version 0.0";

const unsigned char tab_[] PROGMEM = "\t";


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


#ifdef USE_SERIAL
// Serial.print() for progmem strings:
// void serial_print_progmem(const prog_uchar *str)	// does not work :(
void serial_print_progmem(const unsigned char *str) {
  unsigned char c;
  while((c = pgm_read_byte(str++)))
    Serial.print(c, BYTE);
}


void serial_println_progmem(const unsigned char *str) {
  serial_print_progmem(str);
  Serial.println("");
}
#endif



/* **************************************************************** */
// #define MENU_over_serial	// do we use a serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial

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



// get numeric input from serial
long numericInput(long oldValue) {
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



const unsigned char ON_off[] PROGMEM = "ON/off";
const unsigned char on_OFF[] PROGMEM = "on/OFF";

// serial display helper function
int ONoff(int value, int mode, int tab) {
  if (value) {
    switch (mode) {
    case 0:		// ON		off
    case 1:		// ON		OFF
      Serial.print("ON");
      break;
    case 2:		// ON/off	on/OFF
      serial_print_progmem(ON_off);
      break;
    }
  }
  else {
    switch (mode) {
    case 0:		// ON		off
      Serial.print("off");
      break;
    case 1:		// ON		OFF
      Serial.print("OFF");
      break;
    case 2:		// ON/off	on/OFF
      serial_print_progmem(on_OFF);
      break;
    }
  }

  if (tab)
    serial_print_progmem(tab_);

  return value;
}



const unsigned char outOfRange[] PROGMEM = " out of range.";
const unsigned char value_[] PROGMEM = "value ";

// display helper function
void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); serial_print_progmem(tab_);
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");
      else if ((i == length/2 && value == 512) || (i == length && value == scale))	// middle or top
	Serial.print("|");
      else
	Serial.print("*");
    }
  } else {
    serial_print_progmem(value_);
    Serial.print(value);
    serial_println_progmem(outOfRange);
  }

  Serial.println("");
}



const unsigned char followPin[] PROGMEM = "Follow pin ";
const unsigned char VU_title[] PROGMEM = "values\t\t +/- tolerance (send any other byte to stop)\n";
const unsigned char quit_[] PROGMEM = "(quit)";

void bar_graph_VU(int pin) {
  int value, oldValue=-9997;	// just an unlikely value...
  int tolerance=1, menu_input;

  serial_println_progmem(followPin);
  Serial.print((int) pin);
  serial_println_progmem(VU_title);

  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > tolerance) {
      bar_graph(value);
      oldValue = value;
    }

    if (char_available()) {
      switch (menu_input = get_char()) {
      case '+':
	tolerance++;
	break;
      case '-':
	if (tolerance)
	  tolerance--;
	break;
      default:
	serial_println_progmem(quit_);
	return;		// exit
      }
    }
  }    
}



#ifdef HARDWARE_menu	// inside MENU_over_serial
const unsigned char analog_reads_title[] PROGMEM = "\npin\tvalue\t|\t\t\t\t|\t\t\t\t|";

void display_analog_reads() {
  int i, value;

  serial_println_progmem(analog_reads_title);

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); serial_print_progmem(tab_); bar_graph(value);
  }

  Serial.println("");
}



const unsigned char watchingINpin[] PROGMEM = "watching digital input pin ";
const unsigned char anyStop[] PROGMEM = "\t\t(send any byte to stop)";
const unsigned char pin_[] PROGMEM = "pin ";
const unsigned char is_[] PROGMEM = " is ";
const unsigned char high_[] PROGMEM = "HIGH";
const unsigned char low_[] PROGMEM = "LOW";

void watch_digital_input(int pin) {
  int value, old_value=-9997;

  serial_println_progmem(watchingINpin);
  Serial.print((int) pin);
  serial_println_progmem(anyStop);

  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      serial_println_progmem(pin_); Serial.print((int) pin); serial_print_progmem(is_);
      if (value)
	serial_println_progmem(high_);
      else
	serial_println_progmem(low_);
    }
  }
  serial_println_progmem(quit_);
  get_char();
}
#endif	// HARDWARE_menu inside MENU_over_serial



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



/* **************************************************************** */
// simple menu interface through serial port:

const unsigned char menuLine1[] PROGMEM = "\nSorry, this menu is empty.";
const unsigned char freeRAM[] PROGMEM = "free RAM: ";
//	const unsigned char on_[] PROGMEM = "(on)";
//	const unsigned char off_[] PROGMEM = "(OFF)";
const unsigned char pPin[] PROGMEM = "p=pin (";


#ifdef HARDWARE_menu
const unsigned char hwMenuTitle[] PROGMEM = "\n***  HARDWARE  ***\t\tfree RAM=";
const unsigned char PPin[] PROGMEM = "P=PIN (";
const unsigned char HLWR[] PROGMEM = ")\tH=set HIGH\tL=set LOW\tW=analog write\tR=read";
const unsigned char VI[] PROGMEM = "V=VU bar\tI=digiwatch\t";
const unsigned char aAnalogRead[] PROGMEM = "a=analog read";
#endif


const unsigned char pressm[] PROGMEM = "\nPress 'm' or '?' for menu.\n\n";



void display_serial_menu() {
  serial_println_progmem(programLongName);

  serial_print_progmem(menuLine1);

  Serial.println("");

#ifdef HARDWARE_menu	// inside MENU_over_serial
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  Serial.println("");

  serial_print_progmem(PPin);
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  serial_println_progmem(HLWR);
  serial_print_progmem(VI);
  serial_println_progmem(aAnalogRead);
#endif			// HARDWARE_menu  inside MENU_over_serial

  serial_print_progmem(pressm);
}



const unsigned char selectPin[] PROGMEM = "Select a pin with P.";

void please_select_pin() {
  serial_println_progmem(selectPin);
}

//	const unsigned char timeInterval[] PROGMEM = "time interval ";
//	const unsigned char milliseconds_[] PROGMEM = "milliseconds\t";
//	const unsigned char unchanged_[] PROGMEM = "(unchanged)";

const unsigned char bytes_[] PROGMEM = " bytes";
#ifdef HARDWARE_menu
const unsigned char numberOfPin[] PROGMEM = "Number of pin to work on: ";
const unsigned char none_[] PROGMEM = "(none)";
const unsigned char setToHigh[] PROGMEM = " was set to HIGH.";
const unsigned char setToLow[] PROGMEM = " was set to LOW.";
const unsigned char analogWriteValue[] PROGMEM = "analog write value ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";
const unsigned char analogValueOnPin[] PROGMEM = "analog value on pin ";
#endif



const unsigned char unknownMenuInput[] PROGMEM = "unknown menu input: ";

void menu_discharger() {
  char menu_input;
  long newValue;

  while(!char_available())
    ;

  if (char_available()) {
    while (char_available()) {
      switch (menu_input = get_char()) {
      case ' ': case '\t':		// skip white chars
	break;

      case 'm': case '?':
	display_serial_menu();
	break;

#ifdef HARDWARE_menu inside MENU_over_serial
      case  'M':
	serial_print_progmem(freeRAM);
	Serial.print(get_free_RAM());
	serial_println_progmem(bytes_);
	break;

      case 'P':
	serial_print_progmem(numberOfPin);
	newValue = numericInput(hw_PIN);
	if (newValue>=0 && newValue<DIGITAL_PINs) {
	  hw_PIN = newValue;
	  Serial.println((int) hw_PIN);
	} else
	  serial_println_progmem(none_);
	break;

      case 'H':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  pinMode(hw_PIN, OUTPUT);
	  digitalWrite(hw_PIN, HIGH);
	  serial_print_progmem(pin_); Serial.print((int) hw_PIN); serial_println_progmem(setToHigh);
	}
	break;

      case 'L':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  pinMode(hw_PIN, OUTPUT);
	  digitalWrite(hw_PIN, LOW);
	  serial_print_progmem(pin_); Serial.print((int) hw_PIN); serial_println_progmem(setToLow);
	}
	break;

      case 'W':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  serial_print_progmem(analogWriteValue);
	  newValue = numericInput(-1);
	  if (newValue>=0 && newValue<=255) {
	    Serial.println(newValue);

	    analogWrite(hw_PIN, newValue);
	    serial_print_progmem(analogWrite_); Serial.print((int) hw_PIN);
	    Serial.print(", "); Serial.print(newValue); Serial.println(")");
	  } else
	    serial_println_progmem(quit_);
	}
	break;

      case 'R':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  serial_print_progmem(analogValueOnPin); Serial.print((int) hw_PIN); serial_print_progmem(is_);
	  Serial.println(analogRead(hw_PIN));
	}
	break;

      case 'V':
	if ((hw_PIN == ILLEGAL) | (hw_PIN >= ANALOG_INPUTs))
	  please_select_pin();
	else
	  bar_graph_VU(hw_PIN);
	break;

      case 'I':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  pinMode(hw_PIN, INPUT);
	  watch_digital_input(hw_PIN);
	}
	break;

      case 'a':
	display_analog_reads();
	break;
#endif // HARDWARE_menu

      default:
	serial_print_progmem(unknownMenuInput); Serial.print(byte(menu_input));
	Serial.print(" = "); Serial.println(menu_input);
	while (char_available() > 0) {
	  menu_input = get_char();
	  Serial.print(byte(menu_input));
	}
	Serial.println("");
	break;
      }

      if (!char_available())
	delay(WAITforSERIAL);
    }
  }
} // menu_discharger()


#endif	// MENU_over_serial
/* **************************************************************** */



/* **************************************************************** */
// setup and main loop:

void setup() {

#ifdef USE_SERIAL
  Serial.begin(USE_SERIAL);
#endif

//	#ifdef USE_LCD
//	  LCD.begin(LCD_COLs, LCD_ROWs);
//	  LCD_print_line_progmem(0, programName);
//	  LCD_print_line_progmem(1, version);
//	  LCD_print_line_progmem(3, freeRAM);
//	  LCD.print(get_free_RAM());
//	#endif


#ifdef USE_SERIAL
  #ifndef MENU_over_serial
   Serial.println("");
   serial_println_progmem(programLongName);
  #endif
#endif

#ifdef MENU_over_serial	// show message about menu
  display_serial_menu();
#endif

//	#ifdef USE_LCD
//	  delay(3000);
//	  LCD.clear();
//	  LCD_print_line_progmem(TOP, programName);
//	#endif

}



/* **************************************************************** */

void loop() {
  unsigned long now;

#ifdef MENU_over_serial
  if (char_available())
    menu_discharger();
#endif
}


/* **************************************************************** */
