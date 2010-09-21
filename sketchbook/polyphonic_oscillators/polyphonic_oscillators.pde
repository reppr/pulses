//  poly_osc

#define OSCILLATORS 3
#define ILLEGALpin -1

int oscillator;
unsigned long periodSet;
unsigned long period[OSCILLATORS], next[OSCILLATORS];
int state[OSCILLATORS], oscPIN[OSCILLATORS];

int toneSwitch=-1;
int debugSwitch=0;

void setup() {
  oscPIN[0] = 49;
  oscPIN[1] = 51;
  oscPIN[2] = 53;
  oscillatorInit();

  oscillator=0; periodSet=5000; startOscillator();
  oscillator=1; periodSet=5001; startOscillator();
  oscillator=2; periodSet=15002; startOscillator();

  Serial.begin(9600);
  initMenu();
}

void oscillatorInit() {
  int i;

  for (i=0; i<OSCILLATORS; i++) {
    pinMode(oscPIN[i], OUTPUT);
    period[i] = next[i] = 0;
    state[i] = 0;
  }
}

int startOscillator() {
  unsigned long now = micros();

  if (oscillator >= OSCILLATORS )
    return 1;
  if (oscPIN[oscillator] == ILLEGALpin)
    return 1;

  period[oscillator] = periodSet;
  next[oscillator] = now + periodSet;
  state[oscillator] = 1;

  digitalWrite(oscPIN[oscillator], HIGH);

  Serial.print("Started oscillator "); Serial.print(oscillator);
  Serial.print("\tpin "); Serial.print(oscPIN[oscillator]);
  Serial.print("\tperiod "); Serial.println(period[oscillator]);

  return 0;
}

void stopOscillator(int oscillator) {
  state[oscillator] = 0;
  digitalWrite(oscPIN[oscillator], LOW);
}

void toggleOscillator() {
  if (state[oscillator]) {
    stopOscillator(oscillator);
    Serial.print("Oscillator "); Serial.print(oscillator); Serial.println(" stopped");
  }
  else if (period[oscillator]) {
    periodSet = period[oscillator];
    startOscillator();
  }
  else
    Serial.println("error: no period set");
}

int numericInput(int oldValue) {
  int input, num; 

  while (!Serial.available())	// wait for input
    ;
  delay(10);	// sometimes the second byte was not ready without that

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

int isOscOK() {
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

void menuOscillators(){
  int input, newValue, oscillator=-1;

  Serial.println("");
  Serial.println("*********** Menu oscillators *********** ");
  Serial.println("t=toggle tone \to=oscillator \t~=on/off \tp=period");
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

    case 'D':
      debugSwitch ^= -1 ;
      if (debugSwitch)
	Serial.println("debug: ON");
      else
	Serial.println("debug: OFF");

      break;

    case 'o': // set menu local oscillator index to act on the given oscillator
      oscillator = numericInput(oscillator);
      Serial.print("oscillator "); Serial.println(oscillator);

      break;

    case '~': // toggle oscillator on/off
      if (isOscOK())
	toggleOscillator();

      break;

    case 'p':
      if (isOscOK()) {
	newValue = numericInput(period[oscillator]);
	if (period[oscillator] != newValue) {
	  period[oscillator] = newValue;
	  Serial.print("period set to "); Serial.println(newValue);
	} 
	else
	  Serial.println("(quit)");
      }

    break;

    }
  }
}

void oscillate() {
  unsigned long now = micros();

  int i;
  if (debugSwitch)
    for (i=0; i<4; i++)
      analogRead(0);


  for (oscillator=0; oscillator<OSCILLATORS; oscillator++) {
    if (state[oscillator] && (next[oscillator] <= now)) {
      if (toneSwitch) {
	if ( (state[oscillator] *= -1) == -1)
	  digitalWrite(oscPIN[oscillator], LOW);
	else
	  digitalWrite(oscPIN[oscillator], HIGH);
      }

      next[oscillator] += period[oscillator];
    }
  }
}

void loop() {
  oscillate();

  if (Serial.available())
    menuOscillators();
}
