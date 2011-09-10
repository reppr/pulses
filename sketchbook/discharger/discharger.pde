/* **************************************************************** */
// discharger v0.2
/* **************************************************************** */


/* **************************************************************** */
/*
   Used to control a number of old NiMh accu cells during dischargment.

   This version uses one of the internal reference voltages.

   The cell voltages are measured through voltage dividers.
   The dividers can be different for each input, calibration in software.
     (using internal 1.1 volt reference and 4 NiMh cells I use 10k/56k resistors.
      Don't make impedance of the voltage dividers too high, 10k should be ok.)

   The analog inputs with their individual voltage dividers for the cells
   must be calibrated on first use. Connect a serial line and follow the
   instructions. On first start the program might start calibration
   automatically. Otherwise send the char 'C' through the line.

   Calibration data gets permanently stored in EEPROM.

   There is a very simple menu interface through serial line.

   This version can use a 20x4 LCD display.

   To save RAM constant strings are stored in PROGMEM.

*/
/* **************************************************************** */
#define ILLEGAL		-1


/* **************************************************************** */
// CONFIGURATION:

// cells in a battery:
#define BATTERY_CELLs	4

// analog input pins:
unsigned char battery_PIN[BATTERY_CELLs] = {0, 1, 2, 3};

// load_switch_pin connected to the cmos that switches the load:
#define LOAD_SWITCH_PIN	12

// levels to determine NiMh cell state (in millivolts):
#define LEVEL_EMERGENCY		1080		// mV
#define LEVEL_LOW		1190		// mV
#define LEVEL_OK		1240		// mV
#define LEVEL_GOOD		1300		// above is level_excellent

// oversampling:
#define OVERSAMPLING	4

// store calibration data of the analog inputs in eeprom?
#define USE_EEPROM	0		// #ifdef eeprom_start

// use serial line for communication?
//	#define USE_SERIAL	115200
#define USE_SERIAL	57600
//	#define USE_SERIAL	38400


#ifdef USE_SERIAL	// simple menus over serial line?
  // menu over serial line:
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

#define USE_LCD	7, 6, 5, 4, 3, 2	// LiquidCrystal(rs, enable, d4, d5, d6, d7) 
#ifdef USE_LCD
  #include <LiquidCrystal.h>
  LiquidCrystal LCD(USE_LCD);

  #define LCD_COLs	20		// this version only works with 20x4 displays and up to 4 cells
  #define LCD_ROWs	4		// this version only works with 20x4 displays and up to 4 cells
  #define TOP		0
  #define INFO_1	1		// line for LCD info_1 display
  #define INFO_2	2		// line for LCD info_2 display
  #define MESSAGE	3		// line for LCD message display
#endif

#define PIN_WITH_SWITCH_1	9	// pin with a switch against ground, if any
#define DEBOUNCE		5	// milliseconds
// #define LED_PIN_2		10	// pin of second LED, if any

// time interval between measurements
#define TIME_INTERVAL	30000	// milliseconds

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

// variable, as we might have more then one load and switch the pin at run time
char load_switch_pin=LOAD_SWITCH_PIN;

unsigned long last, time_interval=TIME_INTERVAL;	// milliseconds

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
// simle minded switch control:
#ifdef PIN_WITH_SWITCH_1
int switch_pressed(int pin) {
  unsigned long now, on=millis();

  if (digitalRead(pin) == HIGH )		// HIGH means OFF
    return 0;					// so return immediately

  while (digitalRead(pin) == LOW ) {		// LOW means ON
    if (((now=millis()) - on ) > DEBOUNCE ) {	// pressed long enough?
      while (digitalRead(pin) == LOW )		// yes, wait until key is released
	;
      now=millis();				// remember time
      delay(DEBOUNCE);				// debounce switching off
      return now - on;				// return duration in mS
    }
  }
  return 0;					// not pressed long enough
}
#endif

/* **************************************************************** */
// use PROGMEM to save RAM:
#include <avr/pgmspace.h>

// const prog_uchar programName[]="BATTERY DISCHARGER";		// does not work :(
const unsigned char programName[] PROGMEM = "BATTERY DISCHARGER";
const unsigned char programLongName[] PROGMEM = "*** BATTERY DISCHARGER v0.2 ***";
const unsigned char version[] PROGMEM = "version 0.2";

const unsigned char tab_[] PROGMEM = "\t";
// const unsigned char empty[] PROGMEM = "";

#ifdef USE_LCD
// LCD.print() for progmem strings:
// void LCD_print_progmem(const prog_uchar *str)	// does not work :(
void LCD_print_progmem(const unsigned char *str) {
  unsigned char c;
  while((c = pgm_read_byte(str++)))
    LCD.print(c);
}

void LCD_print_at_progmem(unsigned char col, unsigned char row, const unsigned char *str) {
  LCD.setCursor(col, row);
  LCD_print_progmem(str);
}

// clear line and print on LCD at line 'row'
const unsigned char LCD_empty[] PROGMEM = "                    ";
void LCD_print_line_progmem(unsigned char row, const unsigned char *str) {
  LCD_print_at_progmem(0, row, LCD_empty);
  LCD_print_at_progmem(0, row, str);
}

#endif

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
// ADC inputs:

// data of the ADC inputs:
float cell_voltage[BATTERY_CELLs];		// calculated cell voltages
float cell_voltage_start[BATTERY_CELLs];	// voltages on startup

// calibration data of the ADC inputs with individual voltage dividers:
float voltage_scale[BATTERY_CELLs];		// scaling factors for each input

// obsolete, could be eliminated:
// int input_scaling_mV[BATTERY_CELLs];		// so many mV on this cells input
// obsolete, could be eliminated:
// int input_scaling_AD_value[BATTERY_CELLs];	//    give this AD value
#if (BATTERY_CELLs == 4)
  // uncalibrated value for 1.1V reference
  // with four identical 10k/56k voltage dividers
  int input_scaling_mV[BATTERY_CELLs] =
    {1250, 2500, 3750, 5000};
  int input_scaling_AD_value[BATTERY_CELLs] =
    {179, 357, 536, 714};
#else
  #if (BATTERY_CELLs == 6)
    // uncalibrated value for 1.1V reference
    // with four identical 10k/56k voltage dividers
    // plus two 10k/82k voltage dividers for the two topmost cells
    int input_scaling_mV[BATTERY_CELLs] =
       {1250, 2500, 3750, 5000, 6250 , 7500 };
    int input_scaling_AD_value[BATTERY_CELLs] =
      {179, 357, 536, 714, 640, 768};
  #else
    int input_scaling_mV[BATTERY_CELLs];
    int input_scaling_AD_value[BATTERY_CELLs];
  #endif
#endif	// number of BATTERY_CELLs


void compute_input_scaling() {
  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    voltage_scale[cell] = input_scaling_mV[cell] / (1000.0 * input_scaling_AD_value[cell]);
  }
}

const unsigned char V_[] PROGMEM = "V  (";

void get_cell_voltages(void) {
  int value;
  float sum=0.0;

  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    value=0;
    for (int sample=0; sample<OVERSAMPLING; sample++)  
      value += analogRead(battery_PIN[cell]);
    cell_voltage[cell] = (float) (value / OVERSAMPLING * voltage_scale[cell]) - sum;
    sum += cell_voltage[cell];
  }
}

char worst_cell=ILLEGAL;

void get_check_and_display_cells(void) {
  float drop;

#ifdef LED_PIN_2
  digitalWrite(LED_PIN_2, HIGH);
#else
  #ifdef LED_PIN
    digitalWrite(LED_PIN, HIGH);
  #endif
#endif

  get_cell_voltages();
  check_worst_cell_state();	// to know which cell is worst
  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    drop = cell_voltage[cell] - cell_voltage_start[cell];

#ifdef USE_LCD
  int drop_mV=(int) ((float) drop * 1000.0 + 0.5);

  LCD.setCursor(cell*5, INFO_1);
  if (cell != worst_cell)
    LCD.print(" ");
  else
    LCD.print("!");
  LCD.print((int) ((cell_voltage[cell] + 0.0005) * 1000.0));

  LCD.setCursor(cell*5, INFO_2);
  if (drop_mV > 0)	// '+' sign
    LCD.print("+");
  else if (drop_mV == 0)
    LCD.print("-");
  LCD.print(drop_mV);
#endif

#ifdef USE_SERIAL
    Serial.print((int) cell + 1); 
    if (cell != worst_cell)
      Serial.print("  "); else
      Serial.print("- ");
    Serial.print((float) cell_voltage[cell] + 0.005,2);
    serial_print_progmem(V_);
    if (drop > 0)	// '+' sign
      Serial.print("+");
    if (drop == 0 )
      Serial.print(" ");

    Serial.print((float) drop + 0.005,2);
    Serial.print(")\t");
#else
    ;
#endif
  }

#ifdef USE_SERIAL
  Serial.println("");
#endif

#ifdef LED_PIN_2
  digitalWrite(LED_PIN_2, LOW);
#else
  #ifdef LED_PIN
    digitalWrite(LED_PIN, LOW);
  #endif
#endif

}


/* **************************************************************** */
// react on battery cells voltage:
// levels in mV:
int level_emergency, level_low, level_ok, level_good;	// above is level_excellent

void initialize_levels() {
  level_emergency=LEVEL_EMERGENCY;	// mV
  level_low=LEVEL_LOW;			// mV
  level_ok=LEVEL_OK;			// mV
  level_good=LEVEL_GOOD;		// above is level_excellent
}

enum battery_state {emergency, low, ok, good, excellent};
battery_state worst_cell_state;
int worst_cell_voltage;
// char worst_cell=ILLEGAL;	// already declared

// return state of the *worst* cell
int check_worst_cell_state() {
  int cell_voltage_mV;
  worst_cell_voltage = 9999;

  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    cell_voltage_mV= ((int) (cell_voltage[cell] * 1000) + 0.5);
    if (cell_voltage_mV < worst_cell_voltage) {
      worst_cell_voltage = cell_voltage_mV;
      worst_cell = cell;
    }
  }

  if (worst_cell_voltage < level_emergency) {
    worst_cell_state = emergency;
  } else if (worst_cell_voltage < level_low) {
    worst_cell_state = low;
  } else if (worst_cell_voltage < level_ok) {
    worst_cell_state = ok;
  } else if (worst_cell_voltage < level_good) {
    worst_cell_state = good;
  } else
    worst_cell_state = excellent;

  return worst_cell_state;		// return state of the *worst* cell
}



char load_state=0;	// 0=off, 1=on, ?=pulsed....
const unsigned char switchedONpin[] PROGMEM = "Switched ON pin";
const unsigned char switchedOFFpin[] PROGMEM = "Switched OFF pin";

void switch_load_on(int display) {
  #ifdef USE_LCD
    if (display) {
      if (load_state != 1) {
	LCD_print_line_progmem(MESSAGE, switchedONpin);
	LCD.print((int) load_switch_pin);
      }
    }
  #endif

  #ifdef USE_SERIAL
    if (display) {
      if (load_state != 1) {
	serial_print_progmem(switchedONpin);
	Serial.println((int) load_switch_pin);
      }
    }
  #endif

  load_state=1;
  digitalWrite(load_switch_pin,HIGH);
}


void switch_load_off(int display) {
  #ifdef USE_LCD
    if (display) {
      if (load_state != 0) {
	LCD_print_line_progmem(MESSAGE, switchedOFFpin);
	LCD.print((int) load_switch_pin);
      }
    }
  #endif

  #ifdef USE_SERIAL
    if (display) {
      if (load_state != 0) {
	serial_print_progmem(switchedOFFpin);
	Serial.println((int) load_switch_pin);
      }
    }
  #endif

  load_state=0;
  digitalWrite(load_switch_pin,LOW);
}

char disable_shutdown=0;

const unsigned char shutDown[] PROGMEM = "shutdown  ";
const unsigned char enabled[]  PROGMEM = "ENABLED";
const unsigned char disabled[] PROGMEM = "DISABLED";

void toggle_disable_shutdown() {
  disable_shutdown = ~disable_shutdown;

#ifdef USE_LCD
  LCD_print_line_progmem(TOP, shutDown);
  if (disable_shutdown)
    LCD_print_progmem(disabled);
  else
    LCD_print_progmem(enabled);
#endif

#ifdef USE_SERIAL
  serial_print_progmem(shutDown);
  if (disable_shutdown)
    serial_println_progmem(disabled);
  else
    serial_println_progmem(enabled);
#endif
}

const unsigned char emergencyShutdown[] PROGMEM = "EMERGENCY SHUTDOWN";
#ifdef USE_SERIAL
const unsigned char emergencyShutdownCell[] PROGMEM = "\n==> EMERGENCY SHUTDOWN <==\tworst cell=";
#endif
const unsigned char cell_[] PROGMEM = "cell ";
const unsigned char hasOnly[] PROGMEM = " has only ";
const unsigned char has_[] PROGMEM = " has ";

void react_on_battery_state() {
  switch (check_worst_cell_state()) {
  case emergency:
    if (disable_shutdown == 0) {

#ifdef USE_LCD
      LCD_print_line_progmem(MESSAGE, emergencyShutdown);
      LCD_print_line_progmem(TOP, cell_);
      LCD.print(worst_cell + 1);
      LCD_print_progmem(has_);
      LCD.print((int) ((cell_voltage[worst_cell] + 0.0005) * 1000.0));
      LCD.print("mV");
#endif

#ifdef USE_SERIAL
      serial_print_progmem(emergencyShutdownCell);
      Serial.print((int) worst_cell + 1);
      serial_print_progmem(hasOnly);
      Serial.print((float) cell_voltage[worst_cell] + 0.005,2);
      Serial.println("V");
#endif

      shutdown();

#ifdef USE_LCD
      // if we get here, there was no shutdown, so clear message:
      LCD_print_line_progmem(MESSAGE, LCD_empty);
#endif

#ifdef USE_SERIAL
      // if we get here, there was no shutdown, cleanup:
      Serial.println("");
#endif

    }
    break;
  case low:
    switch_load_off(true);
    digitalWrite(load_switch_pin,LOW);
    break;
  case ok:
    switch_load_on(true);
    break;
  case good:
    switch_load_on(true);
    break;
  case excellent:
    switch_load_on(true);
    break;
  }
}


/* **************************************************************** */
// #define MENU_over_serial	// do we use a serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial

// sometimes serial is not ready quick enough:
#define WAITforSERIAL 10


// char input with one byte buffering:
char stored_char, chars_stored=0;


int get_char() {
  if(!char_available())
    return -1;		// EOF no input available

  if (chars_stored) {
    --chars_stored;
    return stored_char;
  } else if (Serial.available())
    return Serial.read();
  else
    return -1;		// EOF no input available
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


#ifdef HARDWARE_menu	// inside MENU_over_serial
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

// interactively calibrate ADC:
const unsigned char calibrateExplanation[] PROGMEM = "\n** CALIBRATION **\nBy connecting a stable known voltage to each input in turn.\n(y/n)?";
const unsigned char notCalibrated[] PROGMEM = "*NO CALIBRATION DONE*";
const unsigned char connectedToPin[] PROGMEM = " connected to pin ";
const unsigned char connectAndMeasure[] PROGMEM = "        Connect and measure voltage against ground.";
const unsigned char inputVoltageOnPin[] PROGMEM = "        Input voltage on pin ";
const unsigned char inMillivolts[] PROGMEM = " in millivolts:";
const unsigned char mV_ADvalue[] PROGMEM = "mV\tAD value=";
const unsigned char pinNotCalibrated[] PROGMEM = "(pin NOT CALIBRATED)";
const unsigned char askUseStore[] PROGMEM = "\n\nUse and store the following values?";
const unsigned char answerUseStore[] PROGMEM = "\nEnter 'y' to use and store permanently\n'u' to USE for this session, 'q' to quit.\t(y/u/Q)?";
const unsigned char cell___[] PROGMEM = "\nCell "; 

void calibrate()
{
  int millivolts, value, got_input=0, menu_input;

  serial_println_progmem(calibrateExplanation);

  while (char_available())	// clear prior input
    get_char();
  while (!char_available())	// wait for key pressed
    ;

  switch (get_char()) {
    case 'y' :
      break;
    default:
      serial_println_progmem(notCalibrated);
      return;
    }

  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    serial_print_progmem(cell___);
    Serial.print((int) cell + 1);
    serial_println_progmem(connectedToPin);
    Serial.println((int) battery_PIN[cell]);
    serial_println_progmem(connectAndMeasure);

    serial_println_progmem(inputVoltageOnPin);
    Serial.print((int) battery_PIN[cell]);
    serial_println_progmem(inMillivolts);

    millivolts = numericInput(input_scaling_mV[cell]);
    if (millivolts != 0) {
      input_scaling_mV[cell] = millivolts;

      value=0;
      for (int sample=0; sample<OVERSAMPLING; sample++)  
	value += analogRead(battery_PIN[cell]);
      input_scaling_AD_value[cell] = value / OVERSAMPLING;

      Serial.print(millivolts); serial_println_progmem(mV_ADvalue);
      Serial.println(input_scaling_AD_value[cell]);
      got_input++;
    } else
      serial_println_progmem(pinNotCalibrated);
  }

  if (got_input > 0 ) {
    serial_println_progmem(askUseStore);
    show_scaling();
    serial_println_progmem(answerUseStore);

    while (char_available())	// disregard any prior input
      get_char();
    while (!char_available())	// wait for input
      ;
    switch (menu_input = get_char()) {
    case 'y' :
      compute_input_scaling();
      save_scaling_to_eeprom();
      break;
    case 'u' :
      compute_input_scaling();
      break;
    default :
      got_input=0;
    }
  }
 
  if (got_input == 0 )
    serial_println_progmem(notCalibrated);

  Serial.println("");
}

const unsigned char scalingTitle[] PROGMEM = "\nScaling was calibrated with the following data:";
const unsigned char cell__[] PROGMEM = "cell=";
const unsigned char pin__[] PROGMEM = "\tpin=";
const unsigned char voltage_[] PROGMEM = "\tvoltage=";
const unsigned char factor_[] PROGMEM = "\tfactor=";

void show_scaling() {
  serial_println_progmem(scalingTitle);
  for (int cell=0; cell<BATTERY_CELLs; cell++) {
      serial_println_progmem(cell__);
      Serial.print(cell);
      serial_println_progmem(pin__);
      Serial.print((int) battery_PIN[cell]);
      serial_println_progmem(voltage_);
      Serial.print(input_scaling_mV[cell]);
      serial_println_progmem(mV_ADvalue);
      Serial.print(input_scaling_AD_value[cell]);
      serial_println_progmem(factor_);
      Serial.println(voltage_scale[cell],6);
    }
  Serial.println("");
}


/* **************************************************************** */
// simple menu interface through serial port:

const unsigned char menuLine1[] PROGMEM = ".=display\ts=show\t\tr=reset\t\tx=toggle shutdown ";
const unsigned char freeRAM[] PROGMEM = "free RAM: ";
const unsigned char levelsMvEmergency[] PROGMEM = "LEVELS mV:\te=emergency (";
const unsigned char on_[] PROGMEM = "(on)";
const unsigned char off_[] PROGMEM = "(OFF)";
const unsigned char pPin[] PROGMEM = "p=pin (";
const unsigned char cCalibrate[] PROGMEM = ")\tC=calibrate";
const unsigned char tTime[] PROGMEM = "\tt=time mS (";
const unsigned char lLow[] PROGMEM = ")\tl=low (";
const unsigned char oOk[] PROGMEM = ")\to=ok (";
const unsigned char gGood[] PROGMEM = ")\tg=good (";
const unsigned char excellent_[] PROGMEM = ")\t<excellent";
const unsigned char hwMenuTitle[] PROGMEM = "\n***  HARDWARE  ***\t\tfree RAM=";
const unsigned char PPin[] PROGMEM = "P=PIN (";
const unsigned char HLWR[] PROGMEM = ")\tH=set HIGH\tL=set LOW\tW=analog write\tR=read";
const unsigned char VI[] PROGMEM = "V=VU bar\tI=digiwatch\t";
const unsigned char aAnalogRead[] PROGMEM = "a=analog read";
const unsigned char pressm[] PROGMEM = "\nPress 'm' or '?' for menu.\n\n";

void display_menu_discharger() {
  serial_println_progmem(programLongName);

  serial_print_progmem(menuLine1);
  if (!disable_shutdown)
    serial_println_progmem(on_);
  else
    serial_println_progmem(off_);

  serial_print_progmem(pPin);
  Serial.print((int) load_switch_pin);
  serial_print_progmem(cCalibrate);
  serial_print_progmem(tTime);
  Serial.print(time_interval);
  Serial.println(")");

  serial_print_progmem(levelsMvEmergency);
  Serial.print(level_emergency);
  serial_print_progmem(lLow);
  Serial.print(level_low);
  serial_print_progmem(oOk);
  Serial.print(level_ok);
  serial_print_progmem(gGood);
  Serial.print(level_good);
  serial_println_progmem(excellent_);

#ifdef HARDWARE_menu	// inside MENU_over_serial
  serial_print_progmem(hwMenuTitle);
  Serial.println(get_free_RAM());
  serial_print_progmem(PPin);
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  serial_println_progmem(HLWR);
  serial_print_progmem(VI);
#endif			// HARDWARE_menu  inside MENU_over_serial
  serial_println_progmem(aAnalogRead);

  serial_print_progmem(pressm);
}

const unsigned char selectPin[] PROGMEM = "Select a pin with P.";

void please_select_pin() {
  serial_println_progmem(selectPin);
}

const unsigned char timeInterval[] PROGMEM = "time interval ";
const unsigned char milliseconds_[] PROGMEM = "milliseconds\t";
const unsigned char unchanged_[] PROGMEM = "(unchanged)";
const unsigned char reset_[] PROGMEM = "\n==> RESET <==\n";
const unsigned char emergencyLevel[] PROGMEM = "emergency level ";
const unsigned char mV_[] PROGMEM = " mV\t: ";
const unsigned char lowLevel[] PROGMEM = "low level ";
const unsigned char okLevel[] PROGMEM = "ok level ";
const unsigned char goodLevel[] PROGMEM = "good level ";
const unsigned char bytes_[] PROGMEM = " bytes";
const unsigned char numberOfPin[] PROGMEM = "Number of pin to work on: ";
const unsigned char none_[] PROGMEM = "(none)";
const unsigned char setToHigh[] PROGMEM = " was set to HIGH.";
const unsigned char setToLow[] PROGMEM = " was set to LOW.";
const unsigned char analogWriteValue[] PROGMEM = "analog write value ";
const unsigned char analogWrite_[] PROGMEM = "analogWrite(";
const unsigned char analogValueOnPin[] PROGMEM = "analog value on pin ";
const unsigned char switchPin_[] PROGMEM = "switch pin ";
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
	display_menu_discharger();
	break;

      case 'C':	// calibrate
	calibrate();
	break;

      case 't':
	serial_print_progmem(timeInterval);
	Serial.print(time_interval);
	serial_print_progmem(milliseconds_);
	newValue = numericInput(time_interval);
	if (newValue>0) {
	  time_interval = newValue;
	  Serial.println(time_interval);
	} else
	  serial_println_progmem(unchanged_);
	break;

      case 'p':
	serial_print_progmem(switchPin_);
	Serial.print((int) load_switch_pin);
	Serial.print("\t: ");
	newValue = numericInput(load_switch_pin);
	if ((newValue>=0) & (newValue<DIGITAL_PINs)) {
	  load_switch_pin = newValue;
	  Serial.println((int) load_switch_pin);
	} else
	  serial_println_progmem(unchanged_);
	break;

      case 'r':
	  serial_print_progmem(reset_);
	  reset();
	  save_startup_voltages();
	  display_menu_discharger();
	  break;

      case 's':
	show_scaling();			// show_infos();
	break;

      case 'e':
	serial_print_progmem(emergencyLevel);
	Serial.print(level_emergency);
	serial_print_progmem(mV_);
	level_emergency = numericInput(level_emergency);
	Serial.println(level_emergency);
	break;

      case 'l':
	serial_print_progmem(lowLevel);
	Serial.print(level_low);
	serial_print_progmem(mV_);
	level_low = numericInput(level_low);
	Serial.println(level_low);
	break;

      case 'o':
	serial_print_progmem(okLevel);
	Serial.print(level_ok);
	serial_print_progmem(mV_);
	level_ok = numericInput(level_ok);
	Serial.println(level_ok);
	break;

      case 'g':
	serial_print_progmem(goodLevel);
	Serial.print(level_good);
	serial_print_progmem(mV_);
	level_good = numericInput(level_good);
	Serial.println(level_good);
	break;

      case 'x':
	toggle_disable_shutdown();
	break;

      case '.':
	get_check_and_display_cells();
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

#endif // HARDWARE_menu
      case 'a':
	display_analog_reads();
	break;

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
// do we use eeprom to store calibration data of ADC inputs?
// #define USE_EEPROM eeprom_start
/* **************************************************************** */
#ifdef USE_EEPROM

#include <EEPROM.h>

int eeprom_start=USE_EEPROM;


// arbitrary byte order...
int write_int_to_eeprom(int eeprom_addr, int value) {
  EEPROM.write(eeprom_addr,lowByte(value));
  EEPROM.write(++eeprom_addr,highByte(value));

  return 0;
}


int read_int_from_eeprom(int eeprom_addr) {
  int value;
  value = (EEPROM.read(eeprom_addr+1)<<8) | EEPROM.read(eeprom_addr);
  return value;
}

const unsigned char saved_[] PROGMEM = "Saved ";
const unsigned char bytesScalingData[] PROGMEM = " bytes scaling data to EEPROM.";
const unsigned char bye_[] PROGMEM = "bye\n\n";

int save_scaling_to_eeprom() {
  int eeprom_addr=USE_EEPROM;

  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    write_int_to_eeprom(eeprom_addr, input_scaling_AD_value[cell]);
    eeprom_addr += 2;

    write_int_to_eeprom(eeprom_addr, input_scaling_mV[cell]);
    eeprom_addr += 2;
  }

#ifdef USE_SERIAL	// inside USE_EEPROM
  serial_print_progmem(saved_);
  Serial.print(eeprom_addr - eeprom_start);
  serial_println_progmem(bytesScalingData);
#endif

  return 0 ;
}


int read_scaling_from_eeprom() {
  int millivolts, AD_value, eeprom_addr=USE_EEPROM;

  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    if ( (AD_value = read_int_from_eeprom(eeprom_addr) ) < 1)	// ERROR
      return 1;
    eeprom_addr += 2;
    input_scaling_AD_value[cell] = AD_value;

    input_scaling_mV[cell] = read_int_from_eeprom(eeprom_addr);
    eeprom_addr += 2;
  }
  compute_input_scaling();
  return 0;
}

#endif	// USE_EEPROM


/* **************************************************************** */
// startup, shutdown, reset:


// remember startup voltages:
void save_startup_voltages() {
  get_cell_voltages();
  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    cell_voltage_start[cell] = cell_voltage[cell];
  }
}


void shutdown() {
  // switch known loads off immediately:
  switch_load_off(true);

#ifdef USE_SERIAL
  while (char_available())	// clear prior input
    get_char();
#endif

  for (int i=0; i<32; i++) {	// delay
    digitalWrite(LED_PIN,HIGH);
    delay(10);
    digitalWrite(LED_PIN,LOW);
    for (int j=0; j<9; j++) {
      delay(120);
      #ifdef PIN_WITH_SWITCH_1
        if (digitalRead(PIN_WITH_SWITCH_1) == LOW ) {	// maybe the user wants to continue?
	  toggle_disable_shutdown();
	  return;
	}
      #endif
    }

#ifdef USE_SERIAL
    if (char_available()) {				// maybe the user wants to continue?
      switch (get_char()) {
      case 'x':
	toggle_disable_shutdown();
	return ;
      }
    }
#endif
  }

  // ok, we *will* shutdown now:
  for (int pin=0; pin<DIGITAL_PINs; pin++)
    digitalWrite(pin,LOW);	// risky, we might have negative logic on some pins

#ifdef USE_SERIAL
  serial_print_progmem(bye_);
#endif

  for (int i=0; i<18; i++)	// ################# correct that
    detachInterrupt(i);
				// or maybe we should use this?
				// #include <avr/interrupt.h>
				// #define cli()	

  while (1)
    ;
} 


void reset() {
  int error;

  switch_load_off(true);	// switch *current* load off

  // pin to the cmos switching the load:
  load_switch_pin=LOAD_SWITCH_PIN;
  pinMode(load_switch_pin,OUTPUT);
  switch_load_off(true);	// switch *default* load off

#ifdef USE_EEPROM	// load calibration data for analog inputs from EEPROM
  if ( (error = read_scaling_from_eeprom()) != 0 )
  #ifdef USE_SERIAL
    calibrate();
  #else
    ;
  #endif
#endif	// USE_EEPROM

  compute_input_scaling();	// we need this in all cases

  initialize_levels();

  // remember startup voltages:
  save_startup_voltages();

  time_interval=TIME_INTERVAL;
}

/* **************************************************************** */
// setup and main loop:

void setup() {

  // battery_PIN[] = {0, 1, 2, 3};

#ifdef USE_SERIAL
  Serial.begin(USE_SERIAL);
#endif

#ifdef USE_LCD
  LCD.begin(LCD_COLs, LCD_ROWs);
  LCD_print_line_progmem(0, programName);
  LCD_print_line_progmem(1, version);
  LCD_print_line_progmem(3, freeRAM);
  LCD.print(get_free_RAM());
#endif

  // without delay startup message are not always shown
  for (int i=0; i<4; i++) {
    digitalWrite(LED_PIN,HIGH);
    delay(4);
    digitalWrite(LED_PIN,LOW);
    delay(240);
  }

#ifdef USE_SERIAL
  #ifndef MENU_over_serial
   Serial.println("");
   serial_println_progmem(programLongName);
  #endif
#endif

  // set an internal analog reference:
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
  // uncomment *ONE* of these:
  analogReference(INTERNAL1V1);		// internal reference 1.1 V
  // analogReference(INTERNAL2V56);	// internal reference 2.56 V
#else								// 168/328 boards
  analogReference(INTERNAL);		// internal reference 1.1 V
#endif	// board specific

  // after switching reference voltage we do a couple of analogReads
  // to stabilize readings
  for (int i=0; i<16; i++)
    get_cell_voltages();  // dummy voltages, as scaling might not be initialized

  reset();		// must be *after* setting analog reference

#ifdef MENU_over_serial	// show message about menu
  display_menu_discharger();
#endif

  // remember startup voltages:
  get_cell_voltages();
  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    cell_voltage_start[cell] = cell_voltage[cell];
  }

#ifdef PIN_WITH_SWITCH_1
  pinMode(PIN_WITH_SWITCH_1, INPUT);
  digitalWrite(PIN_WITH_SWITCH_1, HIGH);	// activate pullup resistor
#endif

#ifdef USE_LCD
  delay(3000);
  LCD.clear();
  LCD_print_line_progmem(TOP, programName);
#endif

  switch_load_off(true);

  get_check_and_display_cells();
  react_on_battery_state();
}


void loop() {
  unsigned long now;

#ifdef MENU_over_serial
  if (char_available())
    menu_discharger();
#endif //  MENU_over_serial

  now=millis();
  if ( (now - last) > time_interval ) {
    last = now;
    get_check_and_display_cells();
    react_on_battery_state();
  }

#ifdef PIN_WITH_SWITCH_1
  int switch_pressed_ms;

  if (switch_pressed_ms = switch_pressed(PIN_WITH_SWITCH_1)) {
    get_check_and_display_cells();
    react_on_battery_state();
  }
#endif

}
