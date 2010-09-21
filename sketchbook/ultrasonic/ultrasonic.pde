// ultrasonic_direct_connect
// version to plug the ultrasonic sensor directly into arduino
// power and ground through digital out pins

// we don't need these run time comfigurable: 
#define ultrasonicGroundPIN 35
#define ultrasonicEchoPIN 37
#define triggerPIN 39
#define ultrasonicPowerPIN 41

// #define delayTime 60

// these could go to the menus:
int triggerDuration = 10;
int ultrasonicToneOutPIN = 49;
int toneSustain = 60;

void setup () {
  pinMode(ultrasonicGroundPIN, OUTPUT);
  digitalWrite(ultrasonicGroundPIN, LOW);
  pinMode(ultrasonicPowerPIN, OUTPUT);
  digitalWrite(ultrasonicPowerPIN, HIGH);

  pinMode(triggerPIN, OUTPUT);
  pinMode(ultrasonicEchoPIN, INPUT);

  Serial.begin(9600);
}

void loop () {
  Serial.println("trigger");
  digitalWrite(triggerPIN, HIGH);  // trigger
  long time = micros();
  while (micros() - time < triggerDuration)    // >10 uS
    ;
  digitalWrite(triggerPIN, LOW);
  
  while (digitalRead(ultrasonicEchoPIN) == LOW)  // wait for echo
    ;
  time = micros();
  while (digitalRead(ultrasonicEchoPIN) == HIGH)
    ;
  int echo = micros() - time;
  int oldecho = 0;
  float hz;
  if (oldecho != echo ) {
    oldecho = echo;
    Serial.print(echo);
    float cm = echo / 58.0;
    Serial.print("\t"); Serial.print(cm, 2); Serial.print(" cm\t");
    hz = 2000000.0 / echo;
    Serial.print(hz, 1); Serial.println(" Hz");
  }
 
  // delay(delayTime);
  tone(ultrasonicToneOutPIN, hz, toneSustain);
}
