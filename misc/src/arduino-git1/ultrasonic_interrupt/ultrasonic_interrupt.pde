// ultrasonic_interrupt

// use ultrasonic distance sensor through interrupt routines


#define USE_SERIAL	115200

// ARDUINO BOARD TYPE:
//
#define ARDUINO_mega
// #define ARDUINO_2009


#ifdef ARDUINO_mega
  // module plugged directly into inp 21, 20, 19, 18
  #define ULTRASONIC_ECHO_PIN		19
  #define ULTRASONIC_ECHO_INTERRUPT	4	// interrupt number of the pin

  #define ULTRASONIC_TRIGGER_PIN	20

  #define ULTRASONIC_PowerPIN		21
  #define ULTRASONIC_GroundPIN		18

  #define ULTRASONIC_ToneOutPIN		53
#endif


#ifdef ARDUINO_2009
  #define ULTRASONIC_ECHO_INTERRUPT	1
  #define ULTRASONIC_ECHO_PIN		3

  #define ULTRASONIC_TRIGGER_PIN	2

  // #define ULTRASONIC_PowerPIN
  // #define ULTRASONIC_GroundPIN

  #define ULTRASONIC_ToneOutPIN		13
  // #define ULTRASONIC_ToneOutGND	7
#endif



int toneSustain = 60;
int triggerDuration = 10;


volatile unsigned long echo=0;
// unsigned long last_echo=0;


void setup_ultrasonic() {
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

#ifdef ULTRASONIC_GroundPIN
  pinMode(ULTRASONIC_GroundPIN, OUTPUT);
  digitalWrite(ULTRASONIC_GroundPIN, LOW);
#endif

#ifdef ULTRASONIC_PowerPIN
  pinMode(ULTRASONIC_PowerPIN, OUTPUT);
  digitalWrite(ULTRASONIC_PowerPIN, HIGH);
#endif

}


void read_ultrasonic() {
  static long now;
  
  digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);	// trigger on
  now = micros();

  // setup interrupt (while trigger stays on...)
  attachInterrupt(ULTRASONIC_ECHO_INTERRUPT, &wait_for_echo, RISING);

  while (micros() - now < triggerDuration)	// wait > 10 uS
    ;
  digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);	// then end trigger
}


volatile unsigned long echo_start;

// first interrupt service routine
void wait_for_echo() {
  echo_start = micros();
  attachInterrupt(ULTRASONIC_ECHO_INTERRUPT, &measure_echo, FALLING);
}


// second interrupt service routine
void measure_echo() {
  echo = micros() - echo_start;
  detachInterrupt(ULTRASONIC_ECHO_INTERRUPT);
}



void setup () {
  setup_ultrasonic();

  #ifdef ULTRASONIC_ToneOutGND
    // use a digital pin as gnd connection
    pinMode(ULTRASONIC_ToneOutGND, OUTPUT);
    digitalWrite(ULTRASONIC_ToneOutGND, LOW);
  #endif

  #ifdef USE_SERIAL
    Serial.begin(USE_SERIAL);
  #endif

}


void loop () {

  read_ultrasonic();

  int oldecho = 0;
  float hz, cm;

  if (oldecho != echo ) {
    oldecho = echo;

    hz = 2000000.0 / echo;
    float cm = echo / 58.0;

    #ifdef USE_SERIAL
      Serial.print(echo); Serial.print("\t");
      Serial.print(cm, 2); Serial.print(" cm\t");
      Serial.print(hz, 1); Serial.println(" Hz");
    #endif
  }

  tone(ULTRASONIC_ToneOutPIN, hz, toneSustain);
}
