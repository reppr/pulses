// testing loop() rountrip time

unsigned long roundtrip=0, last=0;
long counter=-1;

const unsigned long times=100000;

void setup () {
  Serial.begin(57600);
}

void loop() {

  while (true) {
    unsigned long now=micros();

    switch (++counter) {
    case 0:
      roundtrip=0;
      break;
    case times+1:
      Serial.println(roundtrip);
      counter=-1;
      break;
    default:
      if (now - last > roundtrip)
	roundtrip = now - last;
    }
    last=now;
  }
}
