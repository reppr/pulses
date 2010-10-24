// ultrasonic_direct_connect

// test ultrasonic distance module (with tone pitch feedback)

// version to plug the ultrasonic sensor directly into arduino
// power and ground through digital out pins


// arduino board type

// #define ARDUINO_mega
#define ARDUINO_2009

// we don't need pin configuration run time configurable: 
#ifdef ARDUINO_mega
  #define ultrasonicGroundPIN	35
  #define ultrasonicEchoPIN	37
  #define triggerPIN		39
  #define ultrasonicPowerPIN	41

  #define ultrasonicToneOutPIN	49
#endif

#ifdef ARDUINO_2009
  #define ultrasonicGroundPIN	7
  #define ultrasonicEchoPIN	6
  #define triggerPIN		5
  #define ultrasonicPowerPIN	4

  #define ultrasonicToneOutPIN	13
// #define ultrasonicToneOutGND	7
#endif

#define USE_SERIAL	9600


// these could go to the menus:
int triggerDuration = 10;
int toneSustain = 60;


void setup_ultrasonic() {
  pinMode(ultrasonicGroundPIN, OUTPUT);
  digitalWrite(ultrasonicGroundPIN, LOW);

  pinMode(ultrasonicPowerPIN, OUTPUT);
  digitalWrite(ultrasonicPowerPIN, HIGH);

  pinMode(triggerPIN, OUTPUT);

  pinMode(ultrasonicEchoPIN, INPUT);
}


void setup () {
  setup_ultrasonic();

  #ifdef ultrasonicToneOutGND
    // use a digital pin as gnd connection
    pinMode(ultrasonicToneOutGND, OUTPUT);
    digitalWrite(ultrasonicToneOutGND, LOW);
  #endif

  #ifdef USE_SERIAL
    Serial.begin(USE_SERIAL);
  #endif

}


void loop () {
  #ifdef USE_SERIAL
    Serial.println("trigger");
  #endif

  digitalWrite(triggerPIN, HIGH);	// trigger
  long time = micros();

  while (micros() - time < triggerDuration)    // >10 uS
    ;

  digitalWrite(triggerPIN, LOW);	// end trigger


  while (digitalRead(ultrasonicEchoPIN) == LOW)  // wait for echo
    ;

  time = micros();
  while (digitalRead(ultrasonicEchoPIN) == HIGH) // echo still running
    ;
  int echo = micros() - time;			// ok, measure duration

  int oldecho = 0;
  float hz;
  if (oldecho != echo ) {
    oldecho = echo;

    #ifdef USE_SERIAL
      Serial.print(echo);
      float cm = echo / 58.0;
      Serial.print("\t"); Serial.print(cm, 2); Serial.print(" cm\t");
      hz = 2000000.0 / echo;
      Serial.print(hz, 1); Serial.println(" Hz");
    #endif
  }
 
  tone(ultrasonicToneOutPIN, hz, toneSustain);
}
