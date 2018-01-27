/*
 sampling points in time
 working test code
 not yet used

 code sleeping here
*/


#define TIME_SAMPLES	(32+1)
volatile int time_samp_index=0;

/*
 loop() {
  if(time_samp_index>=TIME_SAMPLES)
    show_time_samples();
  ...
 }
*/

  case 'D':	// DADA reserved for temporary code   testing debugging ...
#define EXTERNAL_INTERRUPT_PIN	25
    MENU.out("time_sampling_init(");
    MENU.out(TIME_SAMPLES);
    MENU.out(F(", "));
    MENU.out(EXTERNAL_INTERRUPT_PIN);
    MENU.outln(F(");"));

    time_sampling_init(TIME_SAMPLES, EXTERNAL_INTERRUPT_PIN);
    break;



// volatile unsigned int time_samp_index=0;
//#define TIME_SAMPLES	(8+1)
unsigned long time_samples[TIME_SAMPLES]= { 0 };

//#define EXTERNAL_INTERRUPT_PIN	25

void take_time_sample_ISR(void) {
  if (time_samp_index >= 0) {
    time_samples[time_samp_index++] = micros();

    if (time_samp_index >= TIME_SAMPLES)
      detachInterrupt(digitalPinToInterrupt(EXTERNAL_INTERRUPT_PIN));

    return;
  } // else

  time_samp_index++;	// skip first few time readings
}

bool time_sampling_init(unsigned int periods, int pin) {
  pinMode(EXTERNAL_INTERRUPT_PIN, INPUT);
  //  time_samp_index = -4;		// skip first 4 time readings
  time_samp_index = 0;		// no skips for PLL testing
  attachInterrupt(digitalPinToInterrupt(EXTERNAL_INTERRUPT_PIN), take_time_sample_ISR, RISING);
}

void show_time_samples() {
  unsigned long sum=0;
  unsigned long diff;
  unsigned long average;
  long delta;

  // calculate (sum and) average prior to displaying:
  for (int i=1; i<TIME_SAMPLES; i++)
    sum += time_samples[i] - time_samples[i-1];
  average = sum / (TIME_SAMPLES - 1);

  for (int i=0; i<TIME_SAMPLES; i++){
    MENU.pad(i,4);
    MENU.out(time_samples[i]);

    if (i) {
      MENU.tab();
      diff = time_samples[i] - time_samples[i-1];
      MENU.out(diff);
      MENU.tab();
      delta = diff - average;
      if (delta >= 0)
	MENU.out('+');
      MENU.out(delta);
    } // i > 0
    MENU.ln();
  }
  MENU.out(F("average : "));
  MENU.outln(average);

  time_samp_index=0;
  //  detachInterrupt(digitalPinToInterrupt(EXTERNAL_INTERRUPT_PIN));
  //  detachInterrupt(EXTERNAL_INTERRUPT_PIN);
}
