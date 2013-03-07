// value to tone

// inactive, unused pin
#define ILLEGALpin 255

// dummy value, not possible from analogRead
#define ILLEGALanalogValue -1

int analogInPIN = 0;
int toneOutPIN = 53;

int toneSwitch = -1;
int displayValueSwitch = 0;
int displayFrequencySwitch = 0;

int lowPIN = ILLEGALpin;
int highPIN = ILLEGALpin;

int value, valueOut;
int lastDisplayedValue=-9999;

int valueInLow = 0;
int valueInHigh = 1023;
int valueOutLow = 120;
int valueOutHigh = 820;

int delayTime = 10;
int toneDuration = 10;


void setup () {
  if (lowPIN != ILLEGALpin) {
    pinMode(lowPIN,OUTPUT);
    digitalWrite(lowPIN,LOW);
  }

  if (highPIN != ILLEGALpin) {
    pinMode(highPIN,OUTPUT);
    digitalWrite(highPIN,HIGH);
  }

  Serial.begin(9600);

  initMenu();
}


void initMenu() {
  Serial.println("Press m or ? for menu.");
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

void ValueToToneMenu () {
  int input, newValue;

  Serial.println("");
  Serial.println("*********** ValueToToneMenu *********** ");
  Serial.println("t=toggle tone \tv=value display\tf=frequency display \td=delay time");
  Serial.println("i=input pin \to=output pin \tl=tone length");
  Serial.println("H set pin high \tL set pin low");
  Serial.println("");

//  while(!Serial.available())
//    ;

  input = Serial.read();

  switch (input) {
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

  case 'v':
    displayValueSwitch ^= -1 ;
    if (displayValueSwitch)
      Serial.println("display value: ON");
    else
      Serial.println("display value: OFF");
    break;

  case 'f':
    displayFrequencySwitch ^= -1 ;
    if (displayFrequencySwitch)
      Serial.println("display frequency: ON");
    else
      Serial.println("display frequency: OFF");
    break;

  case 'd':
    Serial.print("delayTime = "); Serial.print(delayTime); Serial.println("\t:? ");

    newValue = numericInput(delayTime);
    if (delayTime != newValue) {
      delayTime = newValue;
      Serial.print("delayTime set to "); Serial.println(delayTime);
    } 
    else
      Serial.println("(quit)");

    break;

  case 'l':
    Serial.print("toneDuration = "); Serial.print(toneDuration); Serial.println("\t:? ");

    newValue = numericInput(toneDuration);
    if (toneDuration != newValue) {
      toneDuration = newValue;
      Serial.print("toneDuration set to "); Serial.println(toneDuration);
    } 
    else
      Serial.println("(quit)");

    break;

  case 'i':
    Serial.print("analogInPIN = "); Serial.print(analogInPIN); Serial.println("\t:? ");

    newValue = numericInput(analogInPIN);
    if (analogInPIN != newValue) {
      analogInPIN = newValue;
      Serial.print("analogInPIN set to "); Serial.println(analogInPIN);
    }
    else
      Serial.println("(quit)");

    break;

  case 'o':
    Serial.print("toneOutPIN = "); Serial.print(toneOutPIN); Serial.println("\t:? ");
    newValue = numericInput(toneOutPIN);
    if (toneOutPIN != newValue) {
      noTone(toneOutPIN);	// *must* be called for the new pin to work
      toneOutPIN = newValue;
      Serial.print("toneOutPIN set to "); Serial.println(toneOutPIN);
    }
    else
      Serial.println("(quit)");

    break;

  case 'H':
    Serial.println("set pin HIGH, \tpin = ? ");
    while (!Serial.available())
      ;
    input = Serial.read() - '0';
    if (input >= 0 && input < 10 ) {
      highPIN = input;
      Serial.print("highPIN = "); Serial.println(highPIN);
      if ( highPIN != ILLEGALpin) { 
        pinMode(highPIN,OUTPUT);
        digitalWrite(highPIN,HIGH);
        Serial.print(highPIN); Serial.println(" set to HIGH ");
      }
    }
    else
      Serial.println("(quit)");

    break;

  case 'L':
    Serial.println("set pin LOW,\tpin = ? ");
    while (!Serial.available())
      ;
    input = Serial.read() - '0';
    if (input >= 0 && input < 10 ) {
      lowPIN = input;
      Serial.print("lowPIN = "); Serial.println(lowPIN);
      if ( lowPIN != ILLEGALpin ) {
        pinMode(lowPIN,OUTPUT);
        digitalWrite(lowPIN,LOW);
        Serial.print(lowPIN); Serial.println(" set to LOW ");
      }
    }
    else
      Serial.println("(quit)");

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
}


void loop () {
  if (Serial.available() > 0 )
    ValueToToneMenu();

  value=analogRead(analogInPIN);
  if (value != lastDisplayedValue) {
    lastDisplayedValue = value;
    valueOut = map(value, valueInLow, valueInHigh, valueOutLow, valueOutHigh);

    if (displayValueSwitch) {
      Serial.print("value = ");
      Serial.println(value);
    }

    if (displayFrequencySwitch) { 
      Serial.print("freq = ");
      Serial.println(valueOut);
    }

    if (toneSwitch)
      tone(toneOutPIN, valueOut, toneDuration);

    if (delayTime)
      delay(delayTime);

  }
}
