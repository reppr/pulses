/* **************************************************************** */
// discharger
/* **************************************************************** */


/* **************************************************************** */
/*
   Used to control a number of old NiM h accus during dischargment

   This version uses one of the internal reference voltages.

   The cell voltages are measured through voltage dividers
     (using internal 1.1 volt reference and 4 NiMh cells I use 10k/56k resistors.
      Don't make impedance of the voltage dividers too high, 10k should be ok.)

   The analog inputs with their individual voltage dividers for the cells
   must be calibrated before first use. You can connect a serial line and
   follow the instructions. On first start the program might start calibration
   automatically. Otherwise send the char 'C' through the line.

   Calibration data gets permanently stored in EEPROM.


   There is a very simple menu interface through serial line.

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
#define USE_SERIAL	115200

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

#endif

// time interval between measurements
#define TIME_INTERVAL	20000	// milliseconds

/* **************************************************************** */

// board specific things:
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
  #define ANALOG_INPUTs	16
  #define DIGITAL_PINs	54
#else								// 168/328 boards
  #define ANALOG_INPUTs	6
  #define DIGITAL_PINs	14
#endif

#define LED_pin	13

// variable, as we might have more then one load and switch the pin at run time
char load_switch_pin=LOAD_SWITCH_PIN;

unsigned long last, time_interval=TIME_INTERVAL;	// milliseconds

/* **************************************************************** */
// ADC inputs:

// data of the ADC inputs:
float cell_voltage[BATTERY_CELLs];		// calculated cell voltages
float cell_voltage_start[BATTERY_CELLs];	// voltages on startup

// calibration data of the ADC inputs with individual voltage dividers:
float voltage_scale[BATTERY_CELLs];		// scaling factors for each input

// obsolete, could be eliminated:
int input_scaling_mV[BATTERY_CELLs];		// so many mV on this cells input

// obsolete, could be eliminated:
int input_scaling_AD_value[BATTERY_CELLs];	//    give this AD value

void compute_input_scaling() {
  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    voltage_scale[cell] = input_scaling_mV[cell] / (1000.0 * input_scaling_AD_value[cell]);
  }
}

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

void get_check_display_react_voltages(void) {
  float drop;

  get_cell_voltages();
  check_worst_cell_state();

#ifdef USE_SERIAL
  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    Serial.print((int) cell + 1); 
    if (cell != worst_cell)
      Serial.print("  "); else
      Serial.print("- ");
    Serial.print((float) cell_voltage[cell] + 0.005,2);
    Serial.print("V  (");
    drop = cell_voltage[cell] - cell_voltage_start[cell];
    if (drop > 0)	// '+' sign
      Serial.print("+");
    if (drop == 0 )
      Serial.print(" ");

    Serial.print((float) drop + 0.005,2);
    Serial.print(")\t");
  }
  Serial.println("");
#endif

  react_on_battery_state();
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


void shutdown() {
  // switch known loads off immediately:
  switch_load_off(true);

  // give me some time to upload a new program... ;)
  for (int i=0; i<32; i++) {
    digitalWrite(LED_pin,HIGH);
    delay(62);
    digitalWrite(LED_pin,LOW);
    delay(938);
  }

  for (int pin=0; pin<DIGITAL_PINs; pin++)
    digitalWrite(pin,LOW);	// risky, we might have negative logic on some pins

#ifdef USE_SERIAL
  Serial.print("dead\n\n");
#endif

  for (int i=0; i<18; i++)
    detachInterrupt(i);

  while (1)
    ;
} 


char load_state=0;	// 0=off, 1=on, ?=pulsed....

void switch_load_on(int display) {
  #ifdef USE_SERIAL
    if (display) {
      if (load_state != 1) {
	Serial.print("Switched ON pin ");
	Serial.println((int) load_switch_pin);
      }
    }
  #endif

  load_state=1;
  digitalWrite(load_switch_pin,HIGH);
}


void switch_load_off(int display) {
  #ifdef USE_SERIAL
    if (display) {
      if (load_state != 0) {
	Serial.print("Switched OFF pin ");
	Serial.println((int) load_switch_pin);
      }
    }
  #endif

  load_state=0;
  digitalWrite(load_switch_pin,LOW);
}


void react_on_battery_state() {
  switch (check_worst_cell_state()) {
  case emergency:
#ifdef USE_SERIAL
    Serial.print("\n==> EMERGENCY SHUTDOWN <==\tworst cell=");
    Serial.println((int) worst_cell + 1);
#endif
    shutdown();
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


int char_store(char c) {
  if (chars_stored) {	// ERROR ##################
    Serial.println("char_store: sorry, buffer full.");
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


// serial display helper function
int ONoff(int value, int mode, int tab) {
  if (value) {
    switch (mode) {
    case 0:		// ON		off
    case 1:		// ON		OFF
      Serial.print("ON");
      break;
    case 2:		// ON/off	on/OFF
      Serial.print("ON/off");
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
      Serial.print("on/OFF");
      break;
    }
  }

  if (tab)
    Serial.print("\t");

  return value;
}


// display helper function
void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); Serial.print("\t");
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");
      else if ((i == length/2 && value == 512) || (i == length && value == scale))	// middle or top
	Serial.print("|");
      else
	Serial.print("*");
    }
  } else {
    Serial.print("value "); Serial.print(value); Serial.println(" out of range."); }

  Serial.println("");
}


void bar_graph_VU(int pin) {
  int value, oldValue=-9997;	// just an unlikely value...
  int tolerance=1, menu_input;

  Serial.print("Follow pin "); Serial.print((int) pin);
  Serial.print("values\t\t +/- tolerance (send any other byte to stop)\n");

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
	Serial.println("(quit)");
	return;		// exit
      }
    }
  }    
}


void display_analog_reads() {
  int i, value;

  Serial.println("\npin\t\t|\t\t\t\t|\t\t\t\t|");

  for (i=0; i<ANALOG_INPUTs; i++) {
    value = analogRead(i);
    Serial.print(i); Serial.print("\t"); bar_graph(value);
  }

  Serial.println("");
}


void watch_digital_input(int pin) {
  int value, old_value=-9997;

  Serial.print("watching digital input pin "); Serial.print((int) pin); Serial.println("\t\t(send any byte to stop)");
  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      Serial.print("pin "); Serial.print((int) pin); Serial.print(" is ");
      if (value)
	Serial.println("HIGH");
      else
	Serial.println("LOW");
    }
  }
  Serial.println("(quit)");
  get_char();
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


// interactively calibrate ADC:
void calibrate()
{
  int millivolts, got_input=0, menu_input;

  Serial.println("");
  Serial.println("** CALIBRATION **");
  Serial.println("By connecting a stable and measured voltages to each input in turn.");
  Serial.println("");

  for (int cell=0; cell<BATTERY_CELLs; cell++) {
    Serial.print("cell ");
    Serial.print((int) cell + 1);
    Serial.print("  connected to pin ");
    Serial.println((int) battery_PIN[cell]);
    Serial.println("\tconnect voltage, then press <RETURN>");

    while (char_available())	// clear prior input
      get_char();
    while (!char_available)	// wait for key pressed
      ;
    while (char_available())	// disregard input
      get_char();

    Serial.print("Please input voltage on pin ");
    Serial.print((int) battery_PIN[cell]);
    Serial.println(" in millivolts.");

    millivolts = numericInput(input_scaling_mV[cell]);
    if (millivolts != 0) {
      input_scaling_AD_value[cell] = analogRead(battery_PIN[cell]);
      input_scaling_mV[cell] = millivolts;
      Serial.print(millivolts); Serial.print("mV\tAD value=");
      Serial.println(input_scaling_AD_value[cell]);
      got_input++;
    } else
      Serial.println("(pin NOT CALIBRATED)");

    Serial.println("");
  }

  if (got_input > 0 ) {
    Serial.println("\nUse and store the following values?");
    for (int cell=0; cell<BATTERY_CELLs; cell++) {
      Serial.print("cell=");
      Serial.print(cell);
      Serial.print("\tpin=");
      Serial.print((int) battery_PIN[cell]);
      Serial.print("\tvoltage=");
      Serial.print(input_scaling_mV[cell]);
      Serial.print("mV\tAD_value=");
      Serial.println(input_scaling_AD_value[cell]);
    }
    Serial.println("\nEnter 'y' to use and permanently store");
    Serial.println("'u' to USE them for this session, 'q' to quit.\t(y/u/Q)?");

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
    Serial.println("*NO CALIBRATION DONE*");

  Serial.println("");
}


/* **************************************************************** */
// simple menu interface through serial port:

void display_menu_discharger() {
  Serial.println("");
  Serial.println("Menu  *** DISCHARGER ***");
  Serial.print("C=calibrate\tt=time_interval (");
  Serial.print(time_interval);
  Serial.print(")\ts=switch_pin (");
  Serial.print((int) load_switch_pin);
  Serial.println(")\tr=reset");
  Serial.print("levels in mV:\te=emergency (");
  Serial.print(level_emergency);
  Serial.print(")\tl=low (");
  Serial.print(level_low);
  Serial.print(")\to=ok (");
  Serial.print(level_ok);
  Serial.print(")\tg=good (");
  Serial.print(level_good);
  Serial.println(")\t<excellent");

#ifdef HARDWARE_menu	// inside MENU_over_serial
  Serial.println("");
  Serial.println("Menu  ***  HARDWARE  ***");
  Serial.print("P=PIN (");
  if (hw_PIN == ILLEGAL)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")\tH=set HIGH\tL=set LOW\tW=analog write\tR=read");
  Serial.print("V=VU bar\tI=digiwatch\t");
#endif			// HARDWARE_menu  inside MENU_over_serial
  Serial.println("a=analog read");

  Serial.print("\nPress 'm' or '?' for menu.\n\n");

}

void please_select_pin() {
  Serial.println("Select a pin with P.");
}

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
	Serial.print("time interval ");
	Serial.print(time_interval);
	Serial.print(" milliseconds\t: ");
	newValue = numericInput(time_interval);
	if (newValue>0) {
	  time_interval = newValue;
	  Serial.println(time_interval);
	} else
	  Serial.println("(unchanged)");
	break;

      case 's':
	Serial.print("switch pin ");
	Serial.print((int) load_switch_pin);
	Serial.print("\t: ");
	newValue = numericInput(load_switch_pin);
	if ((newValue>=0) & (newValue<DIGITAL_PINs)) {
	  load_switch_pin = newValue;
	  Serial.println((int) load_switch_pin);
	} else
	  Serial.println("(unchanged)");
	break;

      case 'r':
	  Serial.print("\n==> RESET <==\n");
	  reset();
	  save_startup_voltages();
	  display_menu_discharger();
	  break;

      case 'e':
	Serial.print("emergency level ");
	Serial.print(level_emergency);
	Serial.print(" mV\t: ");
	level_emergency = numericInput(level_emergency);
	Serial.println(level_emergency);
	break;

      case 'l':
	Serial.print("low level ");
	Serial.print(level_low);
	Serial.print(" mV\t: ");
	level_low = numericInput(level_low);
	Serial.println(level_low);
	break;

      case 'o':
	Serial.print("ok level ");
	Serial.print(level_ok);
	Serial.print(" mV\t: ");
	level_ok = numericInput(level_ok);
	Serial.println(level_ok);
	break;

      case 'g':
	Serial.print("good level ");
	Serial.print(level_good);
	Serial.print(" mV\t: ");
	level_good = numericInput(level_good);
	Serial.println(level_good);
	break;

#ifdef HARDWARE_menu inside MENU_over_serial
      case 'P':
	Serial.print("Number of pin to work on: ");
	newValue = numericInput(hw_PIN);
	if (newValue>=0 && newValue<DIGITAL_PINs) {
	  hw_PIN = newValue;
	  Serial.println((int) hw_PIN);
	} else
	  Serial.println("(none)");
	break;

      case 'H':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  pinMode(hw_PIN, OUTPUT);
	  digitalWrite(hw_PIN, HIGH);
	  Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to HIGH.");
	}
	break;

      case 'L':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  pinMode(hw_PIN, OUTPUT);
	  digitalWrite(hw_PIN, LOW);
	  Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to LOW.");
	}
	break;

      case 'W':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  Serial.print("analog write value ");
	  newValue = numericInput(-1);
	  if (newValue>=0 && newValue<=255) {
	    Serial.println(newValue);

	    analogWrite(hw_PIN, newValue);
	    Serial.print("analogWrite("); Serial.print((int) hw_PIN);
	    Serial.print(", "); Serial.print(newValue); Serial.println(")");
	  } else
	    Serial.println("(quit)");
	}
	break;

      case 'R':
	if (hw_PIN == ILLEGAL)
	  please_select_pin();
	else {
	  Serial.print("analog value on pin "); Serial.print((int) hw_PIN); Serial.print(" is ");
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
	Serial.print("unknown menu_input: "); Serial.print(byte(menu_input));
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


#endif	\\ MENU_over_serial
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


int save_scaling_to_eeprom() {
  int eeprom_addr=USE_EEPROM;

  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    write_int_to_eeprom(eeprom_addr, input_scaling_AD_value[cell]);
    eeprom_addr += 2;

    write_int_to_eeprom(eeprom_addr, input_scaling_mV[cell]);
    eeprom_addr += 2;
  }

#ifdef USE_SERIAL	// inside USE_EEPROM
  Serial.print("Saved ");
  Serial.print(eeprom_addr - eeprom_start);
  Serial.println(" bytes scaling data to EEPROM.");
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
// startup, reset


// remember startup voltages:
void save_startup_voltages() {
  get_cell_voltages();
  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    cell_voltage_start[cell] = cell_voltage[cell];
  }
}


void reset() {
  int error;

  switch_load_off(true);	// switch *current* load off

  // pin to the cmos switching the load:
  load_switch_pin=LOAD_SWITCH_PIN;
  pinMode(load_switch_pin,OUTPUT);
  switch_load_off(true);	// switch *default* load off

#ifdef	USE_EEPROM	// load calibration data for analog inputs from EEPROM
  if ( (error = read_scaling_from_eeprom()) != 0 )
    calibrate();
#endif

  initialize_levels();

  // remember startup voltages:
  save_startup_voltages();

  time_interval=TIME_INTERVAL;
}

/* **************************************************************** */
// setup and main loop:

void setup() {
  reset();

  // battery_PIN[] = {0, 1, 2, 3};

#ifdef USE_SERIAL
  Serial.begin(USE_SERIAL);
  delay(1000);		// without delay startup message are not always shown
  Serial.print("*** BATTERY DISCHARGER ***\n");
#endif

  // set an internal analog reference:
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)	// mega boards
  // uncomment *ONE* of these:
  analogReference(INTERNAL1V1);		// internal reference 1.1 V
  // analogReference(INTERNAL2V56);	// internal reference 2.56 V
#else								// 168/328 boards
  analogReference(INTERNAL);		// internal reference 1.1 V
#endif

  // after switching reference voltage we do a couple of analogReads
  // to stabilize and initialize readings
  for (int i=0; i<16; i++)
    get_cell_voltages();

#ifdef MENU_over_serial	// show message about menu
  delay(2000);		// without delay the menu message often does not show up
  display_menu_discharger();
#endif

  // remember startup voltages:
  get_cell_voltages();
  for (int cell=0; cell<BATTERY_CELLs ; cell++) {
    cell_voltage_start[cell] = cell_voltage[cell];
  }

  switch_load_off(true);
  get_check_display_react_voltages();	// start with some activity ;)
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
    get_check_display_react_voltages();
  }
}
