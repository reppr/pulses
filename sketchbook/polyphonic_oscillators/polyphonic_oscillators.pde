// polyphonic_oscillators

// hmm, no...
// these are not oscillators,
//
// more like countylators ;)


// #define DEBUG_HW_ON_INIT	// check hardware on init

/* **************************************************************** */
#define ILLEGALpin	-1	// a pin that is not assigned
#define PIN13		13	// onboard LED
				// currently used for electrical tap activity



/* **************************************************************** */
// LED stuff:	let me *see* what's happening in the machine...
int debugSwitch=0;		// hook for testing and debugging

/* **************************************************************** */
// Some coloured LEDs
// Quick hack to have some insight what's happening in the machine
// while building the software...
/* **************************************************************** */
#define COLOUR_LEDs	// colour LED feedback
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
int toneSwitch=-1;	// tone switched on by default


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

  if (oscillator >= OSCILLATORS )
    return 1;
  if (oscPIN[oscillator] == ILLEGALpin)
    return 1;

  period[oscillator] = newPeriod;
  next[oscillator] = now + newPeriod;
  osc_flags[oscillator] |= OSC_FLAG_ACTIVE;	// active ON

  digitalWrite(oscPIN[oscillator], HIGH);

  nextFlip = updateNextFlip();

  Serial.print("Started oscillator "); Serial.print(oscillator);
  Serial.print("\tpin "); Serial.print(oscPIN[oscillator]);
  Serial.print("\tperiod "); Serial.println(period[oscillator]);

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

#define INTERFERENCE_COLOUR_LED
void osc_flip_reaction(){	// whatever you want ;)
  int oscillator=0, led=0;

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

char input_analog=0;			// index
char analog_IN_PIN[INPUTs_ANALOG] = {8, 9, 10, 11};	// on poti row
char analog_IN_state[INPUTs_ANALOG];
char analog_input_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators
short analog_IN_last[INPUTs_ANALOG];

void analog_input_initialize() {
  int input_analog;

  for (input_analog=0; input_analog<INPUTs_ANALOG; input_analog++)
    analog_IN_state[input_analog] = 0;
}


short int output_offset=4000;	// output start value when analog input is zero
int output_scaling=5;		// factor analog input to output change

void analog_input_cyclic_poll() {
  int input_analog = (analog_input_cyclic_index++ % INPUTs_ANALOG);	// cycle through inputs
  if (analog_IN_state[input_analog]) {
    analog_IN_last[input_analog] = analogRead(analog_IN_PIN[input_analog]);
    period[input_analog] = output_offset + (output_scaling * analog_IN_last[input_analog]);
  }
}
#endif	// INPUTs_ANALOG



/* **************************************************************** */
#define TAP_PINs	5  // # TAP pins for electrical touch INPUT
/* **************************************************************** */
#ifdef TAP_PINs

char tap = 0;	// index
char taps=0;	// number of Taps already setup
char tapPIN[TAP_PINs];

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
  }
}

void setup_TAP(char tap, char pin, unsigned char toggle) {	// set toggle to 0 or 1...
  pinMode(tap, INPUT);
  tapPIN[tap] = pin;
  tap_count[tap] = toggle;
  tap_state[tap] = 1;			// default state 1 is active but OFF
  tap_debouncing_since[tap] = 0;	// not debouncing
  taps++;
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
  char tap=0;				// id as index
  char tap_electrical_activity=0;	// any tap switch electrical high?
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

// get numeric input from serial
long numericInput(long oldValue) {
  long input, num; 

  while (!Serial.available())	// wait for input
    ;
  delay(WAITforSERIAL);	// sometimes the second byte was not ready without that

  input = Serial.read();
  if (input >= '0' && input <= '9')
    num = input - '0';
  else {
    if (input != ' ' && input != '\t') { 
      Serial.print(byte(input)); Serial.println("  : not parsed...");
  }

    return oldValue;
  }

  while (Serial.available()) {
    input = Serial.read();
    if (input >= '0' && input <= '9')
      num = 10 * num + (input - '0');
    else {
      if (input != ' ' && input != '\t') {
        Serial.print(byte(input)); Serial.println("  : not parsed..."); }

      return num;
    }
  }

  return num;
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

    if (Serial.available()) {
      switch (menu_input = Serial.read()) {
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
  while (!Serial.available()) {
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
  Serial.read();
}

// print binary numbers with leading zeroes
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
}

/* **************************************************************** */
#ifdef OSCILLATORS	// MENU STUFF for OSCILLATORS

void displayOscillatorsInfos() {
  int i;

  Serial.println("  osc\tactive\tstate\tperiod\tPIN\toctaves");
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

  Serial.print("display: i=infos\t");
  Serial.print("y=analog in\t");
#ifdef PROFILING
  Serial.print("d=debug, profiling\t");
#endif
  Serial.println("");

  Serial.print("o=oscillator("); Serial.print(oscillator);
  Serial.print(") \t~="); ONoff((osc_flags[oscillator] & OSC_FLAG_ACTIVE),2,false);
  Serial.print("\tp=period["); Serial.print(oscillator); Serial.print("] (");
  Serial.print(period[oscillator]); Serial.println(")");

  Serial.println("");
  Serial.print("r=expected roundtrip time (outside oscillator "); Serial.print(timeFor1Round); Serial.print(")");
  Serial.print("\ta=adapt "); ONoff(autoAdapt,0,0); Serial.println("");

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
}

// simple menu interface through serial port
void menuOscillators() {
  int menu_input;
  long newValue;

  while(!Serial.available())
    ;

  while(Serial.available()) {
    switch (menu_input = Serial.read()) {
    case ' ':		// continue reading menu_input (i.e. after numeric input) 
      break;

    case 'm': case '?':
      displayMenuOscillators();
      break;

    case 't':
      toneSwitch ^= -1 ;
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

    case 's': // set output scaling
      Serial.print("output_scaling       "); Serial.println(output_scaling);
      Serial.print("output_scaling new = ");
      output_scaling = numericInput(output_scaling);
      Serial.println(output_scaling);

      break;

    case 'S': // set output offset
      Serial.print("output_offset       "); Serial.println(output_offset);
      Serial.print("output_offset new = ");
      output_offset = numericInput(output_offset);
      Serial.println(output_offset);

      break;

    case 'o': // set menu local oscillator index to act on the given oscillator
      Serial.print("oscillator       "); Serial.println(oscillator);
      Serial.print("oscillator new = ");
      newValue = numericInput(oscillator);
      if (newValue < OSCILLATORS && newValue >= 0) {
	oscillator = newValue;
	Serial.println(oscillator);
      } else {
	Serial.println(" must be between 0 and "); Serial.println(OSCILLATORS -1);
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

    case 'i':
      Serial.println("");
      displayOscillatorsInfos();

      break;

#ifdef HARDWARE_menu
    case 'P':
      Serial.print("Give number of pin to work on ");
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
      while (Serial.available() > 0) {
	menu_input = Serial.read();
	Serial.print(byte(menu_input));
      }
      Serial.println("");

      break;
    }

    if (!Serial.available())
      delay(WAITforSERIAL);
  }

#ifdef PROFILING
  dontProfileThisRound=1;
#endif

}
#endif // OSCILLATORS (inside MENU_over_serial)

void initMenu() {
  Serial.println("Press m or ? for menu.");

#ifdef OSCILLATORS
  displayMenuOscillators();
  // displayOscillatorsInfos();
#endif

  Serial.println("");
}

#endif // MENU_over_serial


/* **************************************************************** */
void setup() {
  int i;

#ifdef OSCILLATORS
  // signed char oscPIN[OSCILLATORS] = {49, 51, 53};

  oscillatorInit();

  startOscillator(0, 4000);
  startOscillator(1, 6000);
  startOscillator(2, 8000);
#endif


#ifdef MENU_over_serial
  Serial.begin(9600);
  initMenu();
#endif


#ifdef COLOUR_LEDs
  init_colour_LEDs();
#endif


#ifdef LED_ROW
  init_LED_row(14, LED_ROW, 0, 1);
#endif


#ifdef TAP_PINs
  init_TAPs();

  setup_TAP(4, 30, 1);	// PIN 30, all tones toneSwitch, start ON

#ifdef OSCILLATORS
  int oscillator;

  setup_TAP(0, 22, 0);	// "down" TAPs
  setup_TAP(1, 24, 0);	// "down" TAPs
  setup_TAP(2, 26, 0);	// "down" TAPs

  /*
  for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
    setup_TAP(taps, 22 + 2*oscillator, 0);	// "down" TAPs
    setup_TAP(taps, 23 + 2*oscillator, 0);	// "up  " TAPs
  }
  */

#endif

  #ifdef TAP_COLOUR_LED_DEBUG
  // TAP colour LED debugging, assume electrical state low to start
  pinMode(PIN13, OUTPUT); digitalWrite(PIN13, LOW);
  #endif 

#endif


#ifdef INPUTs_ANALOG
  analog_input_initialize();

  analog_IN_state[0]=1;		// analog_IN_PIN[] = {0, 1, 2};		accelerometer
  analog_IN_PIN[0] = 0;

  analog_IN_state[1]=1; 
  analog_IN_PIN[1] = 1;

  analog_IN_state[2]=1; 
  analog_IN_PIN[1] = 2;

//  analog_IN_state[3]=1;	// other analog input like piezzo 
//  analog_IN_PIN[1] = 3;


//  analog_IN_state[0]=1;		// analog_IN_PIN[] = {8, 9, 10, 11};	poti row
//  analog_IN_PIN[0] = 8;
//
//  analog_IN_state[1]=1; 
//  analog_IN_PIN[1] = 9;
//
//  analog_IN_state[2]=1; 
//  analog_IN_PIN[1] = 10;
//
//  analog_IN_state[3]=0;	// OFF
//  analog_IN_PIN[1] = 11;

#endif

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

  if (Serial.available())
    menuOscillators();

#endif //  MENU_over_serial
#endif // OSCILLATORS


#ifdef TAP_PINs
  check_TAPs ();	// set logigal state, debounce

  toneSwitch = TAP_toggled_(4);		// toggling toneSwitch

  { 
    int oscillator;
    for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
      if (TAP_toggled_(oscillator))
	osc_flags[oscillator] |= OSC_FLAG_MUTE;		// mute this oscillatur
      else
	osc_flags[oscillator] &= ~OSC_FLAG_MUTE;	// unmute this oscillatur
    }
  }

#endif


#ifdef INPUTs_ANALOG
  analog_input_cyclic_poll();
#endif


  if (debugSwitch)
    ;

}
/* **************************************************************** */
