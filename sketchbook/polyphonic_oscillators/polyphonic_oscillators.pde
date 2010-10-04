// polyphonic_oscillators

// hmm, no...
// these are not oscillators,
//
// more like countylators ;)

#define USE_SERIAL	9600	// 9600

// #define DEBUG_HW_ON_INIT	// check hardware on init

/* **************************************************************** */
#define ILLEGALinputVALUE	-1	// impossible analog input value
#define ILLEGALpin		-1	// a pin that is not assigned
#define PIN13			13	// onboard LED
					// currently used for electrical tap activity


void wuschel()
{
  int i;

  digitalWrite(PIN13,LOW);
  for (i=0; i<4; i++) {
    delay(120) ; digitalWrite(PIN13,HIGH);
    delay(50) ; digitalWrite(PIN13,LOW);
  }

  Serial.println("wuschel");
}


/* **************************************************************** */
// LED stuff:	let me *see* what's happening in the machine...
int debugSwitch=0;		// hook for testing and debugging

/* **************************************************************** */
// Some coloured LEDs
// Quick hack to have some insight what's happening in the machine
// while building the software...
/* **************************************************************** */
//#define COLOUR_LEDs	// colour LED feedback
/* **************************************************************** */
#ifdef  COLOUR_LEDs

#define redPIN 8
#define greenPIN 9
#define bluePIN 10

/*
unsigned char redPIN=8;
unsigned char greenPIN=9;
unsigned char bluePIN=10;
*/

void init_colour_LEDs() {
  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);

#ifdef DEBUG_HW_ON_INIT		// blink the LEDs to check connections:
  digitalWrite(redPIN, HIGH); delay(100); digitalWrite(redPIN, LOW); delay(200);
  digitalWrite(greenPIN, HIGH); delay(100); digitalWrite(greenPIN, LOW); delay(200);
  digitalWrite(bluePIN, HIGH); delay(100); digitalWrite(bluePIN, LOW); delay(200);
#endif
}

#endif	// COLOUR_LEDs


/* **************************************************************** */
// more LED stuff:
// Quick hack to use some LED rows I have, 1 pin spacing between LEDs
// #define LED_ROW		4	// LED rows to stick in digital OUTPUTS
/* **************************************************************** */
#ifdef LED_ROW

int led=0;	// index

char led_row_anode_PIN[LED_ROW], led_row_cathode_PIN[LED_ROW];


void led_row_ON(int led) {
  digitalWrite(led_row_anode_PIN[led],HIGH); digitalWrite(led_row_cathode_PIN[led],LOW);	// ON
}

void init_LED_row(int start, int leds, int spacing, int anode_first) {
  int led;

  for (led=0; led < leds; led++) {
    if (anode_first) {
      led_row_anode_PIN[led]   = start +  led * (2 + spacing);
      led_row_cathode_PIN[led] = start +  led * (2 + spacing) + 1;
    } else {
      led_row_anode_PIN[led]   = start +  led * (2 + spacing) + 1;
      led_row_cathode_PIN[led] = start +  led * (2 + spacing);
    }
    pinMode(led_row_anode_PIN[led], OUTPUT);
    pinMode(led_row_cathode_PIN[led], OUTPUT);

    digitalWrite(led_row_anode_PIN[led],LOW);
    digitalWrite(led_row_cathode_PIN[led],LOW);

#ifdef DEBUG_HW_ON_INIT	// Serial info and blink to check LEDs
    Serial.print((int) led);
    Serial.print(" led_row_anode_PIN[led] ");
    Serial.print((int) led_row_anode_PIN[led]);
    Serial.print(" led_row_cathode_PIN[led] ");
    Serial.print((int) led_row_cathode_PIN[led]);
    Serial.println("");

    led_row_ON(led); delay(100);				// ON
    digitalWrite(led_row_anode_PIN[led],LOW); delay(200);	// OFF
#endif

  }
}
#endif // LED_ROW



/* **************************************************************** */
// OSCILLATORS
// once they were flippylators, now they are implemented as countillators
// we get many octaves for free ;)
//
#define OSCILLATORS	4	// # of oscillators
/* **************************************************************** */
#ifdef OSCILLATORS

int oscillator=0;	// index. int might produce faster code then unsigned char

// This version does not only flip a bit, it increases a short integer.
// We get 16 octaves for free :)
unsigned short osc_count[OSCILLATORS];

// unsigned long longest_period = ~0L ;
unsigned long period[OSCILLATORS], next[OSCILLATORS], nextFlip;

unsigned char osc_flags[OSCILLATORS];
#define OSC_FLAG_ACTIVE	1
#define OSC_FLAG_MUTE	2


signed char oscPIN[OSCILLATORS] = {47, 49, 51, 53};
// signed char oscPIN[OSCILLATORS];
// signed char oscPIN[OSCILLATORS] = {49, 51, 53};
// signed char oscPIN[OSCILLATORS] = {3, 4, 5};


// micro seconds for main loop other then oscillating
// check this value for your program by PROFILING and
// set default here between average and maximum lapse
// time (displayed by pressing 'd')
unsigned long timeFor1Round = 148;

// default switch settings:
char toneSwitch=0;	// tone switched OFF by default


void oscillatorInit() {
  int i;

  for (i=0; i<OSCILLATORS; i++) {
    pinMode(oscPIN[i], OUTPUT);
    period[i] = next[i] = 0;
    osc_flags[i] = 0;		// off
  }
}

int startOscillator(int oscillator, unsigned long newPeriod) {
  unsigned long now = micros();

  if (oscillator >= OSCILLATORS )	// ERROR recovery needed! #########
    return 1;
  if (oscPIN[oscillator] == ILLEGALpin)
    return 1;

  period[oscillator] = newPeriod;
  next[oscillator] = now + newPeriod;
  osc_flags[oscillator] |= OSC_FLAG_ACTIVE;	// active ON

  digitalWrite(oscPIN[oscillator], HIGH);

  nextFlip = updateNextFlip();

  /*
  Serial.print("Started oscillator "); Serial.print(oscillator);
  Serial.print("\tpin "); Serial.print(oscPIN[oscillator]);
  Serial.print("\tperiod "); Serial.println(period[oscillator]);
  */

  return 0;
}

void stopOscillator(int oscillator) {
  osc_flags[oscillator] &= ~OSC_FLAG_ACTIVE;	// active OFF
  digitalWrite(oscPIN[oscillator], LOW);
  nextFlip = updateNextFlip();
}

// is oscillator ON or OFF?
int OSC_(int oscillator) {
  return (osc_count[oscillator] & 1);
}

void toggleOscillator(int oscillator) {
  if (osc_flags[oscillator] & OSC_FLAG_ACTIVE)	// was active
{
    stopOscillator(oscillator);
    Serial.print("Oscillator "); Serial.print(oscillator); Serial.println(" stopped");
  }
  else if (period[oscillator]) {
    startOscillator(oscillator, period[oscillator]);
  }
  else
    Serial.println("error: no period set");
}

// Bitmap of each oscillators mute status:
int oscillators_mute_bits() {
  int oscillator, bitpattern=0;

  for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
    if (osc_flags[oscillator] & OSC_FLAG_MUTE)
      bitpattern |= (1 << oscillator);
  }

  return bitpattern;
}

#define INTERFERENCE_COLOUR_LED
void osc_flip_reaction(){	// whatever you want ;)
  int oscillator=0, led=0;

#ifdef COLOUR_LEDs
  if (OSC_(oscillator) & OSC_(1))
    digitalWrite(redPIN, HIGH);
  else
    digitalWrite(redPIN, LOW);

  if (OSC_(oscillator) ^ OSC_(1))
    digitalWrite(greenPIN, HIGH);
  else
    digitalWrite(greenPIN, LOW);


  if (!OSC_(oscillator) | OSC_(1))
    digitalWrite(bluePIN, HIGH);
  else
    digitalWrite(bluePIN, LOW);
#endif

  // digitalWrite(PIN13, !(OSC_(oscillator) | OSC_(1)));

  // ################################################################


#ifdef LED_ROW
  // these are interesting regarding interferences:
  led=-1;

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] > next[1]);
  digitalWrite(led_row_cathode_PIN[led],LOW);

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] < next[1]);
  digitalWrite(led_row_cathode_PIN[led],LOW);

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] < next[1]);
  digitalWrite(led_row_cathode_PIN[led], OSC_(1));

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] > next[1]);
  digitalWrite(led_row_cathode_PIN[led], OSC_(1));

  /*
  led++;
  digitalWrite(led_row_anode_PIN[led],OSC_(0));
  digitalWrite(led_row_cathode_PIN[led],OSC_(1));
  
  led++;
  digitalWrite(led_row_anode_PIN[led],!OSC_(0));
  digitalWrite(led_row_cathode_PIN[led],OSC_(1));
  */
#endif
}

/* **************************************************************** */
#define BIT_STRIPs	1	// show numbers, bitmasks, etc on LEDs
//
// a number of (usually adjacent) arduino pins represent bit positions
//
/* **************************************************************** */
#ifdef BIT_STRIPs
#define  MAX_BiTS_IN_STRIP 	4

signed char bit_strip_PIN[BIT_STRIPs][MAX_BiTS_IN_STRIP];
char bit_strip_bits[BIT_STRIPs];

int init_bit_strip(int start, int bits, int downwards) { // 'downwards': PINs count down
  static unsigned char strips=0;
  int bit, strip=strips;
  int direction;

  if (bits>MAX_BiTS_IN_STRIP) {		// ERROR protection #######################
    Serial.println("init_bit_strip: ERROR bits value too high");
    return -1;
  }

  bit_strip_bits[strip]=bits;

  if (downwards)
    direction=-1;
  else
    direction=1;

  for (bit=0; bit<bits; bit++) {
    bit_strip_PIN[strip][bit] = start +  direction * (signed char) bit ;
    pinMode(bit_strip_PIN[strip][bit], OUTPUT);

#ifdef DEBUG_HW_ON_INIT				// blink each single LED
    digitalWrite(bit_strip_PIN[strip][bit], HIGH); delay(40);   // ON
    digitalWrite(bit_strip_PIN[strip][bit], LOW);  delay(60);	  // OFF
#endif

  }

#ifdef DEBUG_HW_ON_INIT				// count on all LEDs
  { int i;
    for (i=0; i<(2 << bits)+1; i++){	// from zero to 'zero'
      set_bit_strip(strip, i, 0);
      delay(20);
    }
  }
#endif

  return (int) strips++;
}

void set_bit_strip(int strip, int value, int direction) {
  int bit, mask;
  int bits = bit_strip_bits[strip];

  if (direction == 0) {		// normal bit positions
    mask = 1;
    for (bit=0; bit<bits; bit++, mask *= 2 ) {
      digitalWrite(bit_strip_PIN[strip][bit], value & mask);    
    }
  } else {			// upside down
    mask = 1 << bits ;
    for (bit=0; bit<bits; bit++, mask /= 2 ) {
      digitalWrite(bit_strip_PIN[strip][bit], value & mask);    
    }
  }
}

#endif // BIT_STRIPs


/* **************************************************************** */
#define PROFILING	// gather info for debugging and profiling
			// mainly to find out how long the main loop
			// needs to come back to the oscillator in time.
/* **************************************************************** */
#ifdef PROFILING	// (inside OSCILLATORS)
unsigned long inTime=0, late=0, easy=0, urgent=0, repeat=0, quit=0;
unsigned long enteredOscillatorCount=0, profiledRounds=0, leftOscillatorTime;
unsigned long lapseSum=0;
int lapse=0, maxLapse=0;
int dontProfileThisRound=0;	// no profiling when we spent time in menus or somesuch
float ratio;
unsigned char autoAdapt=false;

void reset_profiling() {
  profiledRounds=0;
  enteredOscillatorCount=0;
  inTime=0; late=0;
  easy=0; urgent=0;
  repeat=0; quit=0;
  lapse=0, lapseSum=0, maxLapse=0;
}

#endif // PROFILING (inside OSCILLATORS)


// #define OSCILLATOR_COLOUR_LED_PROFILING
/* **************************************************************** */
/*
  main oscillator routine  void oscillate()

  Check if it is time to switch a pin and do so

  If there is enough time (see 'timeFor1Round') return
  to the main loop do do other things (like reading sensors,
  changing sound parameters)

  If the next flip approaches loop until there is idle time.
*/

void oscillate() {
  int oscillator;	// int might produce faster code then unsigned char
  unsigned long now = micros();

#ifdef OSCILLATOR_COLOUR_LED_PROFILING
  digitalWrite(bluePIN, HIGH);	// time inside oscillate()
  digitalWrite(greenPIN, HIGH); // no stress
  digitalWrite(redPIN, LOW);	// no stress
#endif

#ifdef PROFILING
  enteredOscillatorCount++;

  // how long did the program spend outside the oscillator?
  if (dontProfileThisRound==0) {
    if (enteredOscillatorCount > 2) {	// start on second run
      // did we arrive in time?
      if (now >= nextFlip)
	late++;
      else
	inTime++;

      lapse = now - leftOscillatorTime;	// how long have we been doing other things?
      lapseSum += lapse;

      if (lapse > maxLapse)		// track maximal lapse
	maxLapse = lapse;

      if (autoAdapt)			// maybe adapt timeFor1Round automatically?
	if (lapse > timeFor1Round)
	  timeFor1Round++;		// step by step ;)
    }
  }
#endif

  // First decide what to do now, based on current time.
  // If times are easy leave do do other things in between.
  // else check oscillators if it's time to flip.
  while (now >= nextFlip || ((nextFlip - now) <= timeFor1Round)) {
    for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
      if ((osc_flags[oscillator] & OSC_FLAG_ACTIVE) && (next[oscillator] <= now)) {
	// flip one
	osc_count[oscillator]++;
	// maybe sound
	if (toneSwitch && ((osc_flags[oscillator] & OSC_FLAG_MUTE) == 0)) {
	  if (osc_count[oscillator] & 1)
	    digitalWrite(oscPIN[oscillator], HIGH);
	  else
	    digitalWrite(oscPIN[oscillator], LOW);
	}

	// compute new next on this oscillator
	next[oscillator] += period[oscillator];
      }
    }

    // compute new nextFlip on one of all oscillators
    nextFlip = updateNextFlip();

    // decide what to do next based on time
    now = micros();

    if (now < nextFlip)		// all oscillators up to date?
      osc_flip_reaction();	// whatever you want ;)

#ifdef PROFILING
    // profile oscillator loop
    if (dontProfileThisRound==0 && enteredOscillatorCount>1) {

      if ((nextFlip - now) > timeFor1Round)
	easy++;
      else
	urgent++;

      if (now >= nextFlip || ((nextFlip - now) <= timeFor1Round))
	repeat++;
      else
	quit++;
    }
#endif

#ifdef OSCILLATOR_COLOUR_LED_PROFILING	// stress?
    // second round only when urgent
    digitalWrite(greenPIN, LOW);	// stress
    digitalWrite(redPIN, HIGH);		// stress
#endif
  }  // while no time to leave oscillator

#ifdef PROFILING
  if (dontProfileThisRound==0) {
    profiledRounds++;
  }
  dontProfileThisRound = 0;

  leftOscillatorTime = micros();
#endif

#ifdef OSCILLATOR_COLOUR_LED_PROFILING
  digitalWrite(bluePIN, LOW);	// time outside oscillate()
  digitalWrite(greenPIN, HIGH); // no stress
  digitalWrite(redPIN, LOW);	// no stress
#endif
}


// compute when the next flip (in any of the active oscillators is due
long updateNextFlip () {
  int oscillator;
  nextFlip |= -1;

  for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
    if (osc_flags[oscillator] & OSC_FLAG_ACTIVE)
      if (next[oscillator] < nextFlip)
	nextFlip = next[oscillator];
  }
  // Serial.print("NEXT "); Serial.println(nextFlip);
  return nextFlip;
}

#endif // OSCILLATORS



/* **************************************************************** */
#define INPUTs_ANALOG	4	// ANALOG INPUTS
/* **************************************************************** */
#ifdef INPUTs_ANALOG

int input_analog=0;			// index
char analog_IN_PIN[INPUTs_ANALOG] = {8, 9, 10, 11};	// on poti row
unsigned char analog_IN_state[INPUTs_ANALOG];
char analog_input_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators
short analog_IN_last[INPUTs_ANALOG];

// parameters for the in2out translation:
short analog_input_offset[INPUTs_ANALOG];
long  analog_input_output_offset[INPUTs_ANALOG];
double analog_in2out_scaling[INPUTs_ANALOG];

void analog_inputs_initialize() {
  int input_analog;

  for (input_analog=0; input_analog<INPUTs_ANALOG; input_analog++) {
    analog_IN_state[input_analog] = 0;
    analog_IN_last[input_analog] = ILLEGALinputVALUE;
    analog_input_offset[input_analog] = 0;
    analog_input_offset[input_analog] = 512;	// ###############
    analog_input_output_offset[input_analog] = 0;
    analog_input_output_offset[input_analog] = 6000;	// ###############
    analog_in2out_scaling[input_analog] = 1.0;
    analog_in2out_scaling[input_analog] = 5.0;	// ###############
  }
}

short analog_IN(int input_analog) {
  return analogRead(analog_IN_PIN[input_analog]);
}

long analog_inval2out(int input_analog, int input_raw_value) {
  long value=input_raw_value;
  value += analog_input_offset[input_analog];
  value *= analog_in2out_scaling[input_analog];;
  value += analog_input_output_offset[input_analog];
  return value;
}

void analog_input_cyclic_poll() {
  int actual_index=input_analog;
  int value;
  {
    int input_analog = (analog_input_cyclic_index++ % INPUTs_ANALOG);	// cycle through inputs
    if (analog_IN_state[input_analog]) {
      if ((value = analog_IN(input_analog)) != analog_IN_last[input_analog]) { // input has changed
	analog_IN_last[input_analog] = value;
	period[input_analog] = analog_inval2out(input_analog, value);
      }
    }
  }

  input_analog = actual_index;	// restore index
}
#endif	// INPUTs_ANALOG



/* **************************************************************** */
#define TAP_PINs	5  // # TAP pins for electrical touch INPUT
/* **************************************************************** */
#ifdef TAP_PINs

char tap = 0;	// index
char tapPIN[TAP_PINs];

// pointers on functions:

/*
  // pointers on  void something()  functions:
  void (*tap_do_on_tap[TAP_PINs])();
  void (*tap_do_on_toggle[TAP_PINs])();
*/

// pointers on  void something(int tap)  functions:
void (*tap_do_on_tap[TAP_PINs])(int);
void (*tap_do_on_toggle[TAP_PINs])(int);


// parameters for tap_do_on_xxx(tap) functions:
long tap_parameter_1[TAP_PINs];		// i.e. oscillator, PIN
long tap_parameter_2[TAP_PINs];		//
double tap_parameter_double[TAP_PINs];
char *tap_parameter_char_address[TAP_PINs];

// *logical* state:  0 inactive  1 OFF  2 ON (pin might be low, debouncing)
unsigned char tap_state[TAP_PINs];

// toggling is implemented with a counter. We get 2^n toggling for free ;)
unsigned char tap_count[TAP_PINs];

unsigned long tap_debouncing_since[TAP_PINs];	// debouncing flag and time stamp
unsigned long tap_debounce=68000L;		// debounce duration

void init_TAPs() {
  char tap;

  for (tap=0; tap<TAP_PINs; tap++) {
    tapPIN[tap] = ILLEGALpin;
    tap_state[tap] = 0;
    tap_debouncing_since[tap] = 0;

    /*
    // pointers on  void something()  functions:
    tap_do_on_tap[tap] = 0;
    tap_do_on_toggle[tap] = 0;
    */

    // pointers on  void something()  functions:
    tap_do_on_tap[tap] = 0;
    tap_do_on_toggle[tap] = 0;
  }
}

void setup_TAP(char tap, char pin, unsigned char toggle) {	// set toggle to 0 or 1...
  pinMode(tap, INPUT);
  tapPIN[tap] = pin;
  tap_count[tap] = toggle;
  tap_state[tap] = 1;			// default state 1 is active but OFF
  tap_debouncing_since[tap] = 0;	// not debouncing
}

// check if TAP 'tap' is logically ON
int TAP_(char tap) {
  return (tap_state[tap] == 2);
}

// check if TAP 'tap' is toggled ON
int TAP_toggled_(char tap) {
  return (tap_count[tap] &  1) ;
}

/* **************************************************************** */
// void check_TAPs()
// set logical state (including toggling) by checking PINs and debouncing:
// sorry, but i leave the colour LED debugging/profiling stuff in...

/* **************************************************************** */
// #define TAP_COLOUR_LED_DEBUG	// default does set  pinMode(PIN13, OUTPUT);
/*
  electrical state: PIN13
  logical ON	    green
  logical OFF	    red
  debouncing	    blue
 */

void check_TAPs() {
  int tap=0;				// id as index
  int tap_electrical_activity=0;	// any tap switch electrical high?
  unsigned long now=micros();

  for (tap=0; tap<TAP_PINs; tap++) {	// check all active TAPs
    if (tap_state[tap]) {			// active?, then check pin state
      if (digitalRead(tapPIN[tap])) {		// TAP PIN is HIGH
	tap_electrical_activity=1;

#ifdef TAP_COLOUR_LED_DEBUG
	digitalWrite(bluePIN,LOW);
	digitalWrite(redPIN,LOW); digitalWrite(greenPIN,HIGH);
#endif

	if (tap_state[tap] == 1) {		//   just switched on?
	  tap_count[tap]++;			//     yes, so count to toggle
	  tap_state[tap] = 2;			//     set state 2 meaning *logical* ON
	  tap_debouncing_since[tap] = 0;	//     not debouncing

	  /*
	  // pointers on  void something()  functions:
	  if (tap_do_on_tap[tap] !=0)		//		maybe do something on tap?
	    tap_do_on_tap[tap]();		//		yes
	  if (tap_count[tap] & 1 && tap_do_on_toggle[tap] !=0)	//  maybe do something on toggle?
	    tap_do_on_toggle[tap]();				//     yes
	  */

	  // pointers on  void something(int tap)  functions:
	  if (tap_do_on_tap[tap] !=0)		//		maybe do something on tap?
	    tap_do_on_tap[tap](tap);		//		yes
	  if (tap_count[tap] & 1 && tap_do_on_toggle[tap] !=0)	//  maybe do something on toggle?
	    tap_do_on_toggle[tap](tap);				//     yes

	}
      } else {					// TAP PIN is LOW
	if (tap_state[tap] != 1) {		//   LOW but, logical state ON. Debounce 

#ifdef TAP_COLOUR_LED_DEBUG
	  digitalWrite(bluePIN,HIGH);		// debounce
#endif

	  if(tap_debouncing_since[tap] != 0) {	//   debouncing is in progress
						//     long enough to switch off?
	    if ((now - tap_debouncing_since[tap]) > tap_debounce) {	// done?
	      tap_state[tap] = 1;					//   switch to logical state 1, OFF
	      tap_debouncing_since[tap] = 0;				//   debouncing finished

#ifdef TAP_COLOUR_LED_DEBUG
	      digitalWrite(bluePIN,LOW); // debounce
	      digitalWrite(redPIN,HIGH); digitalWrite(greenPIN,LOW);
#endif

	    }
	  } else {
	    tap_debouncing_since[tap] = now;	//   start debouncing
	  }
	}
      }
    }
  }

  digitalWrite(PIN13,tap_electrical_activity);	// PIN13 is activity LED

}

#ifdef OSCILLATORS
void tap_do_toggle_osc_mute(int tap) {
  osc_flags[tap_parameter_1[tap]] ^= OSC_FLAG_MUTE;	// toggle muting of oscillator

#ifdef BIT_STRIPs
  set_bit_strip(0, ~oscillators_mute_bits(), 0);	// show ON/mute on LED strip
#endif
}

void tap_do_XOR_byte(int tap) {
  *tap_parameter_char_address[tap] ^= (char) tap_parameter_1[tap];
}

#endif // OSCILLATORS

#endif	// TAP_PINs



/* **************************************************************** */
#define HARDWARE_menu		// menu interface to hardware configuration
/* **************************************************************** */
#ifdef HARDWARE_menu
char hw_PIN = ILLEGALpin;
#endif // HARDWARE_menu



/* **************************************************************** */
#define MENU_over_serial	// do we use a serial menu?
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
      // return sign * num; ###########
      break;
    }
  }

  return sign * num;
}

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
  int tolerance=0, menu_input;

  Serial.print("Follow pin "); Serial.print((int) pin); Serial.print("values\t\t +/- tolerance (send any other byte to stop)");

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

  Serial.print("pin\t");
  Serial.println("\t|\t\t\t\t|\t\t\t\t|");

  for (i=0; i<16; i++) {
    value = analogRead(i);
    Serial.print(i); Serial.print("\t"); bar_graph(value);
  }

  Serial.println("");
}

void watch_digital_input(int pin) {
  int state, old_state=-9997;

  Serial.print("watching digital input pin "); Serial.print((int) pin); Serial.println("\t\t(send any byte to stop)");
  while (!char_available()) {
    state = digitalRead(hw_PIN);
    if (state != old_state) {
      old_state = state;
      Serial.print("pin "); Serial.print((int) pin); Serial.print(" is ");
      if (state)
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

/* **************************************************************** */
#ifdef OSCILLATORS	// MENU STUFF for OSCILLATORS

void displayOscillatorsInfos() {
  int i;

  Serial.println("  osc\tactive\tmute\tstate\tperiod\tPIN\toctaves");
  Serial.println("");

  for (i=0; i<OSCILLATORS; i++) {
    if (i == oscillator)
      Serial.print("* ");
    else
      Serial.print("  ");

    Serial.print(i); Serial.print("\t");


    if (osc_flags[i] & OSC_FLAG_ACTIVE)
      Serial.print("ON\t");
    else
      Serial.print("off\t");

    if (osc_flags[i] & OSC_FLAG_MUTE)
      Serial.print("mute\t");
    else
      Serial.print(" on\t");

    if (osc_count[i] & 1)
      Serial.print("HIGH\t");
    else
      Serial.print("LOW\t");

    Serial.print(period[i]); Serial.print("\t");
    Serial.print(oscPIN[i]); Serial.print("\t");
    serial_print_BIN(osc_count[i], 16); Serial.print("\t");
    Serial.println("");
  }

  Serial.println("");
}

void displayMenuOscillators() {
  Serial.println("");
  Serial.print("** MENU OSCILLATORS **  t=toggle tone ("); ONoff(toneSwitch, 2, false); Serial.println(")");

  Serial.print("display: l=oscs list\t");
  Serial.print("y=analog in\t");
#ifdef PROFILING
  Serial.print("d=debug, profiling\t");
#endif
  Serial.println("");

  Serial.print("o=oscillator("); Serial.print(oscillator);
  Serial.print(")\t~="); ONoff((osc_flags[oscillator] & OSC_FLAG_ACTIVE),2,false);
  Serial.print("\tp=period["); Serial.print(oscillator); Serial.print("] (");
  Serial.print(period[oscillator]); Serial.println(")");

  Serial.print("i=analog inp("); Serial.print(input_analog);
  Serial.print(")\tj=input offset ("); Serial.print(analog_input_offset[input_analog]);
  Serial.print(") \ts=scaling ("); Serial.print(analog_in2out_scaling[input_analog]);
  Serial.println(")");
  Serial.print("\t\tf=output offset("); Serial.print(analog_input_output_offset[input_analog]);
  Serial.print(")\t(last output="); Serial.print(analog_inval2out(input_analog, analog_IN_last[input_analog]));
  Serial.println(")");

#ifdef HARDWARE_menu
  Serial.println("");
  Serial.print("P=PIN (");
  if (hw_PIN == ILLEGALpin)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")\tH=set HIGH\tL=set LOW\tA=analog write\tR=read");
  Serial.println("V=VU bar\tI=digiwatch");
#endif

  Serial.println("");
  Serial.print("r=expected roundtrip time (outside oscillator "); Serial.print(timeFor1Round); Serial.print(")");
  Serial.print("\ta=adapt "); ONoff(autoAdapt,0,0); Serial.println("");

}

// simple menu interface through serial port
void menuOscillators() {
  int menu_input;
  long newValue;

  while(!char_available())
    ;

  while(char_available()) {
    switch (menu_input = get_char()) {
    case ' ': case '\t':		// skip white chars
      break;

    case 'm': case '?':
      displayMenuOscillators();
      break;

    case 't':
      toneSwitch ^= ~0 ;
      Serial.print("tone: ");
      ONoff(toneSwitch, 1, false);
      Serial.println("");

      break;

#ifdef PROFILING
    case 'd':	// display profiling and debugging infos
      Serial.print("expected maximal timeFor1Round "); Serial.println(timeFor1Round);

      Serial.print("entered oscillator "); Serial.print(enteredOscillatorCount);
      Serial.print("\tprofiled rounds "); Serial.print(profiledRounds);
      Serial.print("\tskipped "); Serial.println(enteredOscillatorCount - profiledRounds);

      Serial.print("inTime "); Serial.print(inTime);
      Serial.print("\tlate   "); Serial.print(late);
      ratio = (float) inTime / (inTime + late);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("easy   "); Serial.print(easy);
      Serial.print("\turgent "); Serial.print(urgent);
      ratio = (float) easy / (easy + urgent);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("quit   "); Serial.print(quit);
      Serial.print("\trepeat "); Serial.print(repeat);
      ratio = (float) quit / (quit + repeat);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("maxLapse "); Serial.print(maxLapse);
      Serial.print("\taverage "); Serial.println(lapseSum / profiledRounds);

      break;

    case 'a':
      autoAdapt ^= -1 ;
      if (autoAdapt)
	Serial.println("adapt: ON");
      else
	Serial.println("adapt: OFF");
      break;
#endif

    case 'D':	// stuff to test, try, debug, always changing...
      debugSwitch ^= -1 ;
      if (debugSwitch)
	Serial.println("debug: ON");
      else
	Serial.println("debug: OFF");

      newValue=tap_debounce / 2;
      tap_debounce += newValue;
      Serial.println(tap_debounce);

      // tap_debounce=numericInput(tap_debounce);

      break;

    case 's': // set output scaling	########### need float input... ######################
      Serial.print("A"); Serial.print(input_analog); Serial.print("\tin2out scaling: ");
      Serial.println(analog_in2out_scaling[input_analog]);
      Serial.print("output scaling new = ");
      analog_in2out_scaling[input_analog] = numericInput(analog_in2out_scaling[input_analog]);
      Serial.println(analog_in2out_scaling[input_analog]);

      break;

    case 'f': // set output offset
      Serial.print("A"); Serial.print(input_analog); Serial.print("\toutput offset: ");
      Serial.println(analog_input_output_offset[input_analog]);
      Serial.print("output offset new = ");
      analog_input_output_offset[input_analog] = numericInput(input_analog);
      Serial.println(analog_input_output_offset[input_analog]);

      break;

    case 'o': // set menu local oscillator index to act on the given oscillator
      Serial.print("oscillator       "); Serial.println(oscillator);
      Serial.print("oscillator new = ");
      newValue = numericInput(oscillator);
      if (newValue < OSCILLATORS && newValue >= 0) {
	oscillator = newValue;
	Serial.println(oscillator);
      } else {
	Serial.print(" must be between 0 and "); Serial.println(OSCILLATORS -1);
      }

      break;

    case 'i': // set menu local analog input index
      Serial.print("input_analog       "); Serial.println(input_analog);
      Serial.print("input_analog new = ");
      newValue = numericInput(input_analog);
      if (newValue < INPUTs_ANALOG && newValue >= 0) {
	input_analog = newValue;
	Serial.println(input_analog);
      } else {
	Serial.print(" must be between 0 and "); Serial.println(INPUTs_ANALOG -1);
      }

      break;

    case 'r': // set timeFor1Round
      Serial.print("timeFor1Round       "); Serial.println(timeFor1Round);
      Serial.print("timeFor1Round new = ");
      newValue = numericInput(timeFor1Round);
      if (timeFor1Round > 0) {
	timeFor1Round = newValue;
      	Serial.println(timeFor1Round);

#ifdef PROFILING
	reset_profiling();	// reset profiling data
#endif

     } else
      	Serial.println("must be positive");

      break;

    case '~': // toggle oscillator on/off
      toggleOscillator(oscillator);

      break;

    case 'p':
      Serial.print("Oscillator "); Serial.print(oscillator);
      Serial.print(" \tperiod     = "); Serial.println(period[oscillator]);
      Serial.print("Oscillator "); Serial.print(oscillator);
      Serial.print(" \tperiod new = "); Serial.println(period[oscillator]);
      
      newValue = numericInput(period[oscillator]);
      if (period[oscillator] != newValue) {
	if (newValue > 0) {
	  period[oscillator] = newValue;
	  Serial.println(newValue);
	} else
	  Serial.println("must be positive.");
      }
      else
	Serial.println("(quit)");

    break;

    case 'l':
      Serial.println("");
      displayOscillatorsInfos();

      break;

#ifdef HARDWARE_menu
    case 'P':
      Serial.print("Number of pin to work on: ");
      newValue = numericInput(period[oscillator]);
      if (newValue>=0 && newValue<255) {
	hw_PIN = newValue;
	Serial.println((int) hw_PIN);
      } else
	Serial.println("(none)");

      break;

    case 'H':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, HIGH);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to HIGH.");
      }

      break;

    case 'L':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, LOW);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to LOW.");
      }

      break;

    case 'A':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
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
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	  Serial.print("analog value on pin "); Serial.print((int) hw_PIN); Serial.print(" is ");
	  Serial.println(analogRead(hw_PIN));
      }

      break;

    case 'V':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else
	bar_graph_VU(hw_PIN);

      break;

    case 'I':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, INPUT);
	watch_digital_input(hw_PIN);
      }

      break;

#endif // HARDWARE_menu
    case 'y':
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

#ifdef PROFILING
  dontProfileThisRound=1;
#endif

}
#endif // OSCILLATORS (inside MENU_over_serial)

void initMenu() {
  Serial.println("Press 'm' or '?' for menu.");

#ifdef OSCILLATORS
  displayMenuOscillators();
  // displayOscillatorsInfos();
#endif

  Serial.println("");
}

#endif // MENU_over_serial

/* **************************************************************** */
/* **************************************************************** */
// found this on the net to determine free memory:

// I changed these to extern int
// extern void __bss_end;
// extern void *__brkval;

extern int __bss_end;
extern int *__brkval;

int get_free_memory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
/* **************************************************************** */

// this function will return the number of bytes currently free in RAM
// written by David A. Mellis
// based on code by Rob Faludi http://www.faludi.com
int availableMemory() {
  int size = 8192;	// 1024 // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}

/* **************************************************************** */
/* // dosn't seem to work #######################
Here is an alternative function:
 * This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
*/

uint8_t * heapptr, * stackptr;
void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}

/* **************************************************************** */

// 
// I would like to add a similar function posted in the forum (here):
// MemoryFree.h:
// // memoryFree header
// 
// #ifndef	MEMORY_FREE_H
// #define MEMORY_FREE_H
// 
// #ifdef __cplusplus
// extern "C" {
// #endif
// 
// int freeMemory();
// 
// #ifdef  __cplusplus
// }
// #endif
// 
// #endif
// 
// MemoryFree.cpp:
// extern unsigned int __bss_end;
// extern unsigned int __heap_start;
// extern void *__brkval;
// 
// 
// #include "MemoryFree.h";
// 
// 
// int freeMemory() {
//   int free_memory;
// 
//   if((int)__brkval == 0)
//      free_memory = ((int)&free_memory) - ((int)&__bss_end);
//   else
//     free_memory = ((int)&free_memory) - ((int)__brkval);
// 
//   return free_memory;
// }
// 
// Example sketch:
// #include <MemoryFree.h>
// 

void show_memory() {
  Serial.println("memory info looks wrong!");


  Serial.print("get free memory  = "); Serial.println(get_free_memory());
  Serial.print("available memory = "); Serial.println(availableMemory());
  Serial.print("stackptr = "); Serial.print((int) *stackptr);

  // does not seem to work ################
  Serial.print("\theapptr = "); Serial.print((int) *heapptr);
  Serial.print("\tstackptr - heapptr = "); Serial.println((int) (*stackptr - *heapptr));
}

 
/* **************************************************************** */
void setup() {
  int i;

#ifdef USE_SERIAL
  Serial.begin(USE_SERIAL);
#endif

  // show_memory();

#ifdef OSCILLATORS
  // signed char oscPIN[OSCILLATORS] = {49, 51, 53};

  oscillatorInit();

  startOscillator(0, 4000);
  startOscillator(1, 6000);
  startOscillator(2, 8000);
  startOscillator(3, 12000);
#endif


#ifdef COLOUR_LEDs
  init_colour_LEDs();
#endif


#ifdef LED_ROW
  init_LED_row(14, LED_ROW, 0, 1);
#endif


#ifdef TAP_PINs
  init_TAPs();

#ifdef OSCILLATORS
  {
    int oscillator, pin, tap=0;

    // TAPs start with a down/mute tap for each oscillator:
    for (oscillator=0; oscillator<4; oscillator++) {
      pin = 22;	// ################
      pin += (2 * oscillator);

      setup_TAP(tap, pin, 0);	// "down" TAPs
      tap_parameter_1[tap] = oscillator;
      tap_do_on_tap[tap] = &tap_do_toggle_osc_mute;
      tap++;
    }

    /*
    // Followed by a up/select tap for each oscillator:
    for (oscillator=0; oscillator<4; oscillator++) {
      pin = 23;	// ################
      pin += 2 * oscillator;

      setup_TAP(tap, pin, 0);	// "down" TAPs
      tap_parameter_1[tap] = oscillator;
      // tap_do_on_tap[tap] = &tap_toggle_select_osc; #######################
      tap++;
    }
    */

    // next: toneSwitch to mute all oscillators:
    setup_TAP(tap, 30, 1);	// PIN 30, all tones toneSwitch, start tone OFF
    tap_parameter_1[tap] = ~0;
    tap_parameter_char_address[tap] = &toneSwitch;
    tap_do_on_tap[tap]= &tap_do_XOR_byte;
    tap++;

    /*
    // hot tap
    setup_TAP(tap, 31, 0);	// PIN 31, 'hot tap'
    tap++;
    */
  }
#endif // OSCILLATORS

  #ifdef TAP_COLOUR_LED_DEBUG
  // TAP colour LED debugging, assume electrical state low to start
  pinMode(PIN13, OUTPUT); digitalWrite(PIN13, LOW);
  #endif 

#endif


#ifdef INPUTs_ANALOG
  analog_inputs_initialize();

//  #####################
//  analog_IN_state[0]=1;		// analog_IN_PIN[] = {0, 1, 2};		accelerometer
//  analog_IN_PIN[0] = 0;

//  analog_IN_state[1]=1; 
//  analog_IN_PIN[1] = 1;

//  analog_IN_state[2]=1; 
//  analog_IN_PIN[2] = 2;

//  analog_IN_state[3]=1;	// other analog input like piezzo 
//  analog_IN_PIN[3] = 3;


  analog_IN_state[0]=1;		// analog_IN_PIN[] = {15, 14, 13, 12};	poti row
  analog_IN_PIN[0] = 15;

  analog_IN_state[1]=1; 
  analog_IN_PIN[1] = 14;

  analog_IN_state[2]=1; 
  analog_IN_PIN[2] = 13;

  analog_IN_state[3]=1;
  analog_IN_PIN[3] = 12;
#endif	// INPUTs_ANALOG

#ifdef BIT_STRIPs
  pinMode(18,OUTPUT); digitalWrite(18, LOW);	// ground connection
  init_bit_strip(14, 4, 0);			// 4bit chain from pin 14
#endif

#ifdef MENU_over_serial
  initMenu();
#endif


  show_memory();

//  for (i=0; i<32 ;i++){	// count down negative ########################
//    set_bit_strip(0, -i, 0); delay(200);
//    Serial.print(-i); Serial.print("\t"); serial_print_BIN(-i, 16); Serial.println("");
//  }
//    set_bit_strip(0, 0, 0); delay(200);

  // PIN settings:
pinMode(PIN13, OUTPUT);		// to use the onboard LED

  // temporary PIN settings:
  pinMode(PIN13, OUTPUT); digitalWrite(PIN13, LOW);	// TAP state
  pinMode(6, OUTPUT); digitalWrite(6, LOW);		// ground for piezzos

}

/* **************************************************************** */
void loop() {

#ifdef OSCILLATORS
  oscillate();


#ifdef MENU_over_serial

  if (char_available())
    menuOscillators();

#endif //  MENU_over_serial
#endif // OSCILLATORS


#ifdef TAP_PINs
  check_TAPs ();	// set logigal state, debounce, act
#endif


#ifdef INPUTs_ANALOG
  analog_input_cyclic_poll();
#endif


  if (debugSwitch)
    ;

}
/* **************************************************************** */
