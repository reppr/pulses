// read HCSR04 ultrasonic distance sensor


#define HCSR04_TRIGGER_PIN	41
#define HCSR04_ECHO_PIN		39


void setup_HCSR04_ultrasonic() {
  pinMode(HCSR04_TRIGGER_PIN, OUTPUT);
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);

  pinMode(HCSR04_ECHO_PIN, INPUT);
}

#define HCSR04_TRIGGER_DURATION	10
int read_HCSR04_ultrasonic() {
  long time;

  digitalWrite(HCSR04_TRIGGER_PIN, HIGH);			// trigger
  time = micros();

  while (micros() - time < HCSR04_TRIGGER_DURATION)	// >= 10 uS
    ;
  digitalWrite(HCSR04_TRIGGER_PIN, LOW);			// end trigger


  while (digitalRead(HCSR04_ECHO_PIN) == LOW)  	// wait for echo
    ;

  time = micros();				// echo started
  while (digitalRead(HCSR04_ECHO_PIN) == HIGH)	// echo still running
    ;
  return (int) (micros() - time);		// return echo duration
}


void setup() {
  setup_HCSR04_ultrasonic();
  Serial.begin(9600);
}


void loop() {
  int echo_duration;

  echo_duration=read_HCSR04_ultrasonic();
  Serial.print("echo_duration\t");
  Serial.println(echo_duration);

  delay(1000);
}
