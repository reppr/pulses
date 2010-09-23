// polyphonic_oscillators

/* **************************************************************** */
#define ILLEGALpin -1
int debugSwitch=0;	// debugging


/* **************************************************************** */
#define OSCILLATORS 3	// # of oscillators
#ifdef OSCILLATORS

unsigned long period[OSCILLATORS], next[OSCILLATORS], nextFlip;
int state[OSCILLATORS];
int oscPIN[OSCILLATORS] = {49, 51, 53};
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
#ifdef PROFILING
unsigned long inTime=0, late=0, easy=0, urgent=0, repeat=0, quit=0;
unsigned long enteredOscillatorCount=0, profiledRounds=0, leftOscillatorTime;
unsigned long lapseSum=0;
int lapse=0, maxLapse=0;
int dontProfileThisRound=0;	// no profiling when we spent time in menus or somesuch
float ratio;
unsigned char autoAdapt=false;
#endif // PROFILING (inside OSCILLATORS)


/* **************************************************************** */
// oscillate:
// check if it is time to switch a pin and do so
// if there is enough time (see 'timeFor1Round') return
// to the main loop do do other things (like reading sensors
// changing sound parameters)
// if the next flip is too near loop until there is enough idle time.
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
	if (toneSwitch) {
	  if ( (state[oscillator] *= -1) == -1)
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
#define INPUTs_ANALOG	4	// use analog inputs?
#ifdef INPUTs_ANALOG

char input_ana=0;			// index
char IN_PIN[INPUTs_ANALOG] = {8, 1, 2, 3};
char IN_state[INPUTs_ANALOG];
char input_analog_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators
short IN_last[INPUTs_ANALOG];

void input_analog_initialize() {
  int input_ana;

  for (input_ana=0; input_ana<INPUTs_ANALOG; input_ana++)
    IN_state[input_ana] = 0;

  IN_state[0]=1; // IN_state[3]=1;
}

void input_analog_cyclic_poll() {
  int input_ana = (input_analog_cyclic_index++ % INPUTs_ANALOG);	// cycle through inputs
  if (IN_state[input_ana]) {
    IN_last[input_ana] = analogRead(IN_PIN[input_ana]);
    // Serial.print(input_ana); Serial.print(" gives value "); Serial.println(IN_last[input_ana]);
    // period[0] = abs(10 * IN_last[input_ana]) + 1000 ;
    // Serial.println(abs(10 * IN_last[input_ana]));
  }
}
#endif	// INPUTs_ANALOG


/* **************************************************************** */
#define HARDWARE_menu		// menu interface to hardware configuration
#ifdef HARDWARE_menu
char hw_PIN = ILLEGALpin;
#endif // HARDWARE_menu

/* **************************************************************** */
#define MENU_over_serial	// do we use a serial menu?
#ifdef MENU_over_serial

// sometimes serial is not ready quick enough:
#define WAITforSERIAL 10

// get numeric input from serial
int numericInput(long oldValue) {
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

#ifdef OSCILLATORS

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
#ifdef PROFILING
  Serial.print("d=debug, profiling\t");
#endif
  Serial.println("");

  Serial.print("o=oscillator("); Serial.print(oscillator); Serial.print(") \t~="); ONoff(state[oscillator],2,false);
  Serial.print("\tp=period["); Serial.print(oscillator); Serial.print("] (");
  Serial.print(period[oscillator]); Serial.println(")");  

  Serial.println("");
  Serial.print("r=expected roundtrip time (outside oscillator "); Serial.print(timeFor1Round); Serial.println(")");

#ifdef HARDWARE_menu
  Serial.println("");
  Serial.print("P=select PIN (");
  if (hw_PIN == ILLEGALpin)
    Serial.print("none");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")\tH=set high\tL=set low\tA=analog write");
#endif

  Serial.println("");
}

// primitive menu working through the serial port
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
      Serial.println("");
      Serial.print("expected maximal timeFor1Round "); Serial.println(timeFor1Round);

      Serial.print("entered oscillator "); Serial.print(enteredOscillatorCount);
      Serial.print("\tprofiled rounds "); Serial.print(profiledRounds);
      Serial.print("\tskipped "); Serial.println(enteredOscillatorCount - profiledRounds);

      Serial.print("inTime "); Serial.print(inTime);
      Serial.print("\tlate   "); Serial.print(late);
      if (late) {
	ratio = (float) inTime / late;
	Serial.print("\tratio ");  Serial.println(ratio);
      }

      Serial.print("easy   "); Serial.print(easy);
      Serial.print("\turgent "); Serial.print(urgent);
      if (urgent) {
	ratio = (float) easy / urgent;
	Serial.print("\tratio ");  Serial.println(ratio);
      }

      Serial.print("quit   "); Serial.print(quit);
      Serial.print("\trepeat "); Serial.print(repeat);
      if (quit) {
	ratio = (float) quit / repeat ;
	Serial.print("\tratio ");  Serial.println(ratio);
      }

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

    case 'D':	// stuff for debugging, always changing...
      debugSwitch ^= -1 ;
      if (debugSwitch)
	Serial.println("debug: ON");
      else
	Serial.println("debug: OFF");

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
	// reset profiling data
	profiledRounds=0; enteredOscillatorCount=0;
	inTime=0; late=0; easy=0; urgent=0; repeat=0; quit=0;
	lapse=0, lapseSum=0, maxLapse=0;
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
	Serial.print("value ");
	newValue = numericInput(-1);
	if (newValue>=0 && newValue<=255) {
	  analogWrite(hw_PIN, newValue);
	  Serial.print("analogWrite("); Serial.print((int) hw_PIN); Serial.print(newValue); Serial.println(")");
	  Serial.println(newValue);
	} else
	  Serial.println("(quit)");
      }

      break;
#endif // HARDWARE_menu

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
  displayOscillatorsInfos();
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
#endif

#ifdef MENU_over_serial
  Serial.begin(9600);
  initMenu();
#endif

#ifdef INPUTs_ANALOG
  input_analog_initialize();
#endif

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

#ifdef INPUTs_ANALOG
  input_analog_cyclic_poll();
#endif

  if (debugSwitch)
    ;

}
/* **************************************************************** */
