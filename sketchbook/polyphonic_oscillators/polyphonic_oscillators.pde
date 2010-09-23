//  poly_oscillators

#define OSCILLATORS 3	// # of oscillators
unsigned long period[OSCILLATORS], next[OSCILLATORS], nextFlip;
int state[OSCILLATORS];
int oscPIN[OSCILLATORS] = {49, 51, 53};
int oscillator;

#define ILLEGALpin -1

// sometimes serial is not ready quick enough:
#define WAITforSERIAL 10

// micro seconds for main loop other then oscillating
// check this value for your program by PROFILING and
// set default here between average and maximum lapse
// time (displayed by pressing 'd')
unsigned long timeFor1Round = 148;

// default switch settings:
int toneSwitch=-1;	// tone switched on by default
int debugSwitch=0;	// debugging

#define PROFILING	// gather info for debugging and profiling
			// mainly to find out how long the main loop
			// needs to come back to the oscillator in time.
#ifdef PROFILING
  unsigned long inTime=0, late=0, easy=0, urgent=0, repeat=0, quit=0, leftOscillatorAtTime=0, leftOscillatorCount=0;
  unsigned long lapseSum=0;
  int lapse=0, maxLapse=0;
int dontProfileThisRound=0;	// no profiling when we spent time in menus or somesuch
#endif


void oscillatorInit() {
  int i;

  for (i=0; i<OSCILLATORS; i++) {
    pinMode(oscPIN[i], OUTPUT);
    period[i] = next[i] = 0;
    state[i] = 0;
  }
}

void setup() {
  oscillatorInit();

  startOscillator(0, 1000);
  startOscillator(1, 1201);
  startOscillator(2, 799);

  Serial.begin(9600);
  initMenu();
}

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

// get numeric input from serial
int numericInput(int oldValue) {
  int input, num; 

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

// check if oscillator is set to a legal value, tell the user if it isn't.
int isOscOK(int oscillator) {
  if (oscillator < 0) {
    Serial.println("You must set the oscillator with o<number> first.");
    return 0;
  }
  else if (oscillator >= OSCILLATORS) {
    Serial.print("oscillator must be between 0 and "); Serial.println(OSCILLATORS - 1);
    return 0;
  }
  return 1;
} 
      
void initMenu() {
  Serial.println("Press m or ? for menu.");
}

// primitive menu working through the serial port
void menuOscillators(){
  int input, newValue, oscillator=-1;

  Serial.println("");
  Serial.println("*********** Menu oscillators *********** ");
  Serial.println("t=toggle tone \to=oscillator \t~=on/off \tp=period");
  Serial.println("r=set expected time for one roundtrip (outside oscillator)");
#ifdef PROFILING
  Serial.println("d=display profiling and debug information");
#endif
  Serial.println("");

  while(!Serial.available())
    ;

  while(Serial.available()) {
    switch (input = Serial.read()) {
    case ' ':
      break;

    case 'm': case '?':
      break;

    case 't':
      toneSwitch ^= -1 ;
      if (toneSwitch)
	Serial.println("tone: ON");
      else
	Serial.println("tone: OFF");

      break;

#ifdef PROFILING
    case 'd':	// display profiling and debugging infos
      Serial.print("expected timeFor1Round (set with 'r') "); Serial.println(timeFor1Round);
      Serial.print("inTime "); Serial.print(inTime);
      Serial.print("\tlate "); Serial.print(late);
      Serial.print("\teasy "); Serial.print(easy);
      Serial.print("\turgent "); Serial.println(urgent);
      Serial.print("repeat "); Serial.print(repeat);
      Serial.print("\tquit "); Serial.println(quit);
      Serial.print("maxLapse "); Serial.print(maxLapse);
      Serial.print("\taverage "); Serial.println(lapseSum / leftOscillatorCount);

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
      if (isOscOK(newValue)) {
	oscillator = newValue;
	Serial.print("oscillator "); Serial.println(oscillator);
      }

      break;

    case 'r': // set timeFor1Round
      Serial.print("timeFor1Round = "); Serial.println(timeFor1Round);
      newValue = numericInput(timeFor1Round);
      if (timeFor1Round > 0) {
	timeFor1Round = newValue;
      	Serial.print("timeFor1Round set to "); Serial.println(timeFor1Round);

#ifdef PROFILING
	inTime=0; late=0; easy=0; urgent=0; repeat=0; quit=0, leftOscillatorAtTime=0, leftOscillatorCount=0;
	lapse=0, lapseSum=0, maxLapse=0;
#endif

     }

      break;

    case '~': // toggle oscillator on/off
      if (isOscOK(oscillator))
	toggleOscillator(oscillator);

      break;

    case 'p':
      if (isOscOK(oscillator)) {
	Serial.print("Oscillator "); Serial.print(oscillator);
	Serial.print(" \tperiod = "); Serial.println(period[oscillator]);

	newValue = numericInput(period[oscillator]);
	if (period[oscillator] != newValue) {
	  period[oscillator] = newValue;
	  Serial.print("period set to "); Serial.println(newValue);
	} 
	else
	  Serial.println("(quit)");
      }

    break;

    default:
      Serial.print("unknown input: "); Serial.print(byte(input));
      Serial.print(" = "); Serial.println(input);
      while (Serial.available() > 0) {
	input = Serial.read();
	Serial.print(byte(input));
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
  if (dontProfileThisRound==0) {
    if (leftOscillatorCount > 1) {	// no use at round zero and takes too long at 1
      lapse = now - leftOscillatorAtTime;
      if (lapse > maxLapse)
	maxLapse = lapse;

      lapseSum += lapse;
    }
  }
#endif

  do {
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
    if (dontProfileThisRound==0) {
      if (now < nextFlip)
	inTime++;
      else
	late++;

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

  } while (now >= nextFlip || ((nextFlip - now) <= timeFor1Round));

#ifdef PROFILING
  if (dontProfileThisRound==0) {
    leftOscillatorCount++;
  }
  dontProfileThisRound = 0;

  leftOscillatorAtTime = micros();
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

void loop() {
  oscillate();

  if (Serial.available())
    menuOscillators();

  analogRead(0);
  if (debugSwitch) {
      analogRead(0);
  }
}
