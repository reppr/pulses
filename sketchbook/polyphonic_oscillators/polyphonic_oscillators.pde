// polyphonic_oscillators

// hmm, no...
// these are not oscillators, these are flippylators ;)

/* **************************************************************** */
#define ILLEGALpin	-1	// a pin that is not assigned


/* **************************************************************** */
int debugSwitch=0;		// hook for testing and debugging


/* **************************************************************** */
// Some coloured LEDs
// Quick hack to have some insight what's happening in the machine
// while building the software...
/* **************************************************************** */
#define COULOUR_LEDs	// colour LED feedback
/* **************************************************************** */
#ifdef  COULOUR_LEDs

#define redPIN 8
#define greenPIN 9
#define bluePIN 10

/*
unsigned char redPIN=8;
unsigned char greenPIN=9;
unsigned char bluePIN=10;
*/

void init_coulour_LEDs() {
  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);
}
#endif	// COULOUR_LEDs


/* **************************************************************** */
#define OSCILLATORS	4	// # of oscillators
/* **************************************************************** */
#ifdef OSCILLATORS

unsigned long period[OSCILLATORS], next[OSCILLATORS], nextFlip;
int state[OSCILLATORS];
int oscPIN[OSCILLATORS] = {47, 49, 51, 53};
int oscillator=0;

void oscillatorInit() {
  int i;

  for (i=0; i<OSCILLATORS; i++) {
    pinMode(oscPIN[i], OUTPUT);
    period[i] = next[i] = 0;
    state[i] = 0;
  }
}

// micro seconds for main loop other then oscillating
// check this value for your program by PROFILING and
// set default here between average and maximum lapse
// time (displayed by pressing 'd')
unsigned long timeFor1Round = 148;

// default switch settings:
int toneSwitch=-1;	// tone switched on by default


int startOscillator(int oscillator, unsigned long newPeriod) {
  unsigned long now = micros();

  if (oscillator >= OSCILLATORS )
    return 1;
  if (oscPIN[oscillator] == ILLEGALpin)
    return 1;

  period[oscillator] = newPeriod;
  next[oscillator] = now + newPeriod;
  state[oscillator] = 1;

  digitalWrite(oscPIN[oscillator], HIGH);

  nextFlip = updateNextFlip();

  Serial.print("Started oscillator "); Serial.print(oscillator);
  Serial.print("\tpin "); Serial.print(oscPIN[oscillator]);
  Serial.print("\tperiod "); Serial.println(period[oscillator]);

  return 0;
}

void stopOscillator(int oscillator) {
  state[oscillator] = 0;
  digitalWrite(oscPIN[oscillator], LOW);
  nextFlip = updateNextFlip();
}

void toggleOscillator(int oscillator) {
  if (state[oscillator]) {
    stopOscillator(oscillator);
    Serial.print("Oscillator "); Serial.print(oscillator); Serial.println(" stopped");
  }
  else if (period[oscillator]) {
    startOscillator(oscillator, period[oscillator]);
  }
  else
    Serial.println("error: no period set");
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
  int oscillator;
  unsigned long now = micros();

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

  // is there enough time to do something else in between?
  while (now >= nextFlip || ((nextFlip - now) <= timeFor1Round)) {
    // check all the oscillators if it's time to flip
    for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
      if (state[oscillator] && (next[oscillator] <= now)) {
	state[oscillator] *= -1;
	if (toneSwitch) {
	  if (state[oscillator] < 0)
	    digitalWrite(oscPIN[oscillator], LOW);
	  else
	    digitalWrite(oscPIN[oscillator], HIGH);
	}

	next[oscillator] += period[oscillator];
	nextFlip = updateNextFlip();
      }
    }

    // is there enough time to do something else in between?
    now = micros();

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

  }

#ifdef PROFILING
  if (dontProfileThisRound==0) {
    profiledRounds++;
  }
  dontProfileThisRound = 0;

  leftOscillatorTime = micros();
#endif
}


// compute when the next flip (in any of the active oscillators is due
long updateNextFlip () {
  int oscillator;
  nextFlip |=-1;

  for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
    if (state[oscillator])
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
char IN_PIN[INPUTs_ANALOG] = {8, 9, 10, 11};
char IN_state[INPUTs_ANALOG];
char input_analog_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators
short IN_last[INPUTs_ANALOG];

void input_analog_initialize() {
  int input_analog;

  for (input_analog=0; input_analog<INPUTs_ANALOG; input_analog++)
    IN_state[input_analog] = 0;

  IN_state[0]=1; 
  IN_state[1]=1;
  IN_state[2]=1;
  IN_state[3]=1;
}

void input_analog_cyclic_poll() {
  int input_analog = (input_analog_cyclic_index++ % INPUTs_ANALOG);	// cycle through inputs
  if (IN_state[input_analog]) {
    IN_last[input_analog] = analogRead(IN_PIN[input_analog]);
    period[input_analog] = 1000 + (2 * IN_last[input_analog]);
  }
}
#endif	// INPUTs_ANALOG



/* **************************************************************** */
#define TAP_PINs	1  // TAP pins for electrical touch INPUT
/* **************************************************************** */
#ifdef TAP_PINs

char tap = 0;
char tapPIN[TAP_PINs];

// *logical* state:  0 inactive  1 OFF  2 ON (pin might be low, debouncing)
unsigned char tap_state[TAP_PINs];

// toggling is implemented with a counter. We get 2^n toggling for free ;)
unsigned char tap_count[TAP_PINs];

unsigned long tap_debouncing_since[TAP_PINs];	// debouncing flag and time stamp
unsigned long tap_debounce=20000;		// duration

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
}

// check if TAP 'tap' is logically ON
int TAP_(char tap) {
  return (tap_state[tap] == 2);
}

// check if TAP 'tap' is toggled ON
int TAP_toggled_(char tap) {
  return (tap_count[tap] &  1) ;
}

// set logical state (including toggling) by checking PINs and debouncing:
// sorry, but i leave the colour LED debugging/profiling stuff in...
#define TAP_COLOUR_LED_DEBUG	// do set  pinMode(13, OUTPUT);
/*
  electrical state: PIN13
  logical ON	    green
  logical OFF	    red
  debouncing	    blue
 */

void check_TAPs () {
  char tap=0;
  unsigned long now=micros();

  for (tap=0; tap<TAP_PINs; tap++) {	// check all active TAPs
    if (tap_state[tap]) {			// active?, then check pin state
      if (digitalRead(tapPIN[tap])) {		// TAP PIN is HIGH

#ifdef TAP_COLOUR_LED_DEBUG
	digitalWrite(13,HIGH);
	digitalWrite(bluePIN,LOW);
	digitalWrite(redPIN,LOW); digitalWrite(greenPIN,HIGH);
#endif

	if (tap_state[tap] == 1) {		//   just switched on?
	  tap_count[tap]++;			//     yes, so count to toggle
	  tap_state[tap] = 2;			//     set state 2 meaning *logical* ON
	  tap_debouncing_since[tap] = 0;	//     not debouncing
	}
      } else {					// TAP PIN is LOW

#ifdef TAP_COLOUR_LED_DEBUG
	digitalWrite(13,LOW);
#endif

	if (tap_state[tap] != 1) {		//   LOW but, logical state ON 

#ifdef TAP_COLOUR_LED_DEBUG
	  digitalWrite(bluePIN,HIGH); // debounce
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


/* **************************************************************** */
#ifdef OSCILLATORS	// MENU STUFF for OSCILLATORS

void displayOscillatorsInfos() {
  int i;

  Serial.println("osc\tactive\tstate\tperiod\tPIN");
  for (i=0; i<OSCILLATORS; i++) {
    if (i == oscillator)
      Serial.print("*");
    else
      Serial.print(" ");

    Serial.print(i); Serial.print("\t");

    switch (state[i]) {
    case 0:
      Serial.print("off\tlow\t");
      break;
    case 1:
      Serial.print("ON\tHIGH\t");
      break;
    case -1:
      Serial.print("ON\tLOW\t");
      break;
    default:
      Serial.print("UNKNOWN\t\t");
    }
    Serial.print(period[i]); Serial.print("\t");
    Serial.print(oscPIN[i]); Serial.print("\t");
    Serial.println("");
  }

  Serial.println("");
  // Serial.print("tone: "); ONoff(toneSwitch, 1, false); Serial.println("");
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

  Serial.print("o=oscillator("); Serial.print(oscillator); Serial.print(") \t~="); ONoff(state[oscillator],2,false);
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

//      tap_debounce=numericInput(tap_debounce);

      break;

    case 'o': // set menu local oscillator index to act on the given oscillator
      newValue = numericInput(oscillator);
      if (newValue < OSCILLATORS && newValue >= 0) {
	oscillator = newValue;
	Serial.print("oscillator "); Serial.println(oscillator);
      } else {
	Serial.print("oscillator must be between 0 and "); Serial.println(OSCILLATORS -1);
      }

      break;

    case 'r': // set timeFor1Round
      Serial.print("timeFor1Round = "); Serial.println(timeFor1Round);
      newValue = numericInput(timeFor1Round);
      if (timeFor1Round > 0) {
	timeFor1Round = newValue;
      	Serial.print("timeFor1Round set to "); Serial.println(timeFor1Round);

#ifdef PROFILING
	reset_profiling();	// reset profiling data
#endif
     }

      break;

    case '~': // toggle oscillator on/off
      toggleOscillator(oscillator);

      break;

    case 'p':
      Serial.print("Oscillator "); Serial.print(oscillator);
      Serial.print(" \tperiod = "); Serial.println(period[oscillator]);
      
      newValue = numericInput(period[oscillator]);
      if (period[oscillator] != newValue) {
	period[oscillator] = newValue;
	Serial.print("period set to "); Serial.println(newValue);
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
      Serial.print("Select pin ");
      newValue = numericInput(period[oscillator]);
      if (newValue>=0 && newValue<255) {
	hw_PIN = newValue;
	Serial.println((int) hw_PIN);
      } else
	Serial.println("(quit)");

      break;

    case 'H':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Please select pin with P first.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, HIGH);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to HIGH.");
      }

      break;

    case 'L':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Please select pin with P first.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, LOW);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to LOW.");
      }

      break;

    case 'A':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Please select pin with P first.");
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
	Serial.println("Please select pin with P first.");
      else {
	  Serial.print("analog value on pin "); Serial.print((int) hw_PIN); Serial.print(" is ");
	  Serial.println(analogRead(hw_PIN));
      }

      break;

    case 'V':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Please select pin with P first.");
      else
	bar_graph_VU(hw_PIN);

      break;

    case 'I':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Please select pin with P first.");
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

#ifdef OSCILLATORS
  oscillatorInit();

  startOscillator(0, 1000);
  startOscillator(1, 1201);
  startOscillator(2, 799);
  startOscillator(3, 2000);
#endif

#ifdef MENU_over_serial
  Serial.begin(9600);
  initMenu();
#endif

#ifdef COULOUR_LEDs
  init_coulour_LEDs();
#endif

#ifdef TAP_PINs
  init_TAPs();
  setup_TAP(0, 2, 1);	// PIN 2, start toggled on 
#endif

#ifdef INPUTs_ANALOG
  input_analog_initialize();
#endif

  // i happen to use these pins fon HIGH and LOW levels right now... 
#ifdef TAP_COLOUR_LED_DEBUG
  pinMode(13, OUTPUT); digitalWrite(13, LOW);	// TAP colour LED debugging
#endif

  pinMode(21, OUTPUT); digitalWrite(21, LOW);	// ground for colour LEDs
  pinMode(13, OUTPUT); digitalWrite(13, LOW);	// TAP state

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

  toneSwitch = TAP_toggled_(0);		// toggling tone

#ifndef TAP_COLOUR_LED_DEBUG	// I use LED 13 for electrical TAP state there
  if (TAP_(0))				// TAP status on LED 13
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
#endif

#endif

#ifdef INPUTs_ANALOG
  input_analog_cyclic_poll();
#endif

  if (debugSwitch)
    ;

}
/* **************************************************************** */
