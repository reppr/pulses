/* **************************************************************** */
// polyphonic_oscillators
/* **************************************************************** */


/* **************************************************************** */
/*
  hmm, no...
  these are not really oscillators
  more like countators ;)
*/
/* **************************************************************** */


/* **************************************************************** */
// basic configuration:

#define OSCILLATORs	4	// # of oscillators
#define PROFILING		// gather info for debugging and profiling
#define INPUTs_ANALOG	16	// ANALOG INPUTs
#define TAP_PINs	16  	// # TAP pins for electrical touch INPUT
#define BIT_STRIPs	2	// show numbers, bitmasks, etc on LEDs
#define MAX_BiTS_IN_STRIP 4	// maximal # of bits in a strip
#define USE_SERIAL	115200	// 9600
#define TAP_EDIT		// user interface over taps
#define HAS_TUNING_WHEELs	// piezzo up/down, acc_x, y, z
#define MENU_over_serial	// do we use a serial menu?
#define HARDWARE_menu		// menu interface to hardware configuration
#define DEBUG_TOOLS		// some functions to help with debugging
// #define DEBUG_HW_ON_INIT	// check hardware on init

#define COLOUR_LEDs	// colour LED feedback
// #define OSCILLATOR_COLOUR_LED_PROFILING
// #define TAP_COLOUR_LED_DEBUG	// default does set  pinMode(PIN13, OUTPUT);
// #define LED_ROW	1	// # LED rows to stick in ouput pins

// #define MEMORY_INFO		// this code from the net is buggy


/* **************************************************************** */
/* some  basic defines:						    */

#define ACTIVE_undecided	1	// globally used in many status bitmaps
#define ILLEGAL			-1	// illegal value for many topics

#define ILLEGALinputVALUE	-1	// impossible analog input value
#define ILLEGALpin		-1	// a pin that is not assigned
#define PIN13			13	// onboard LED
					// currently used for electrical tap activity

#define WAITforSERIAL		10	// next byte was not always ready

int debugSwitch=0;			// hook for testing and debugging

/* **************************************************************** */


/* **************************************************************** */
// LED stuff:	let me *see* what's happening in the machine...

/* **************************************************************** */
// Some coloured LEDs
// Quick hack to have some insight what's happening in the machine
// while building the software...
/* **************************************************************** */
// #define COLOUR_LEDs	// colour LED feedback
/* **************************************************************** */
#ifdef COLOUR_LEDs

#define redPIN 8
#define greenPIN 9
#define bluePIN 10

char show_interference_color=0;

void init_colour_LEDs() {
  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);

#ifdef DEBUG_HW_ON_INIT		// blink the LEDs to check connections:
  digitalWrite(redPIN, HIGH); delay(100); digitalWrite(redPIN, LOW); delay(200);
  digitalWrite(greenPIN, HIGH); delay(100); digitalWrite(greenPIN, LOW); delay(200);
  digitalWrite(bluePIN, HIGH); delay(100); digitalWrite(bluePIN, LOW); delay(200);
#endif
}

void red(int ledstat) {
  if (ledstat)
    pinMode(redPIN, OUTPUT);
  digitalWrite(redPIN, ledstat);
}

void green(int ledstat) {
  if (ledstat)
    pinMode(greenPIN, OUTPUT);
  digitalWrite(greenPIN, ledstat);
}

void blue(int ledstat) {
  if (ledstat)
    pinMode(bluePIN, OUTPUT);
  digitalWrite(bluePIN, ledstat);
}

#endif	// COLOUR_LEDs
/* **************************************************************** */


/* **************************************************************** */
// OSCILLATORS
// once they were flippylators, now they are implemented as countillators
// we get many octaves for free ;)
//
// #define OSCILLATORs	4	// # of oscillators
/* **************************************************************** */
#ifdef OSCILLATORs

int osc=0;	// index. int might produce faster code then unsigned char

// This version does not only flip a bit, it increases a short integer.
// We get 16 octaves for free :)
unsigned long osc_count[OSCILLATORs];
unsigned long osc_mask[OSCILLATORs];	// for octaves and rhythms
#define DEFAULT_OCTAVE_SHIFT	6

unsigned long farest_future = ~0L ;
unsigned long period[OSCILLATORs], next[OSCILLATORs], nextFlip;

unsigned char osc_flags[OSCILLATORs];
#define OSC_FLAG_ACTIVE	1
#define OSC_FLAG_MUTE	2
// #define OSC_FLAG_SELECT	4	// unused

// signed char oscPIN[OSCILLATORs];
signed char oscPIN[OSCILLATORs] = {47, 49, 51, 53};
signed char oscMaskPIN[OSCILLATORs];


signed   char global_octave=0;			// global octave shift. ONLY negative shifts supported atm
unsigned long global_octave_mask=1;		// this mask gets shifted and then actually used
unsigned long current_global_octave_mask=1;	// actually used mask to switch oscillators


void global_shift(int global_octave)
{

  if (global_octave>0) {
    Serial.println("global_shift: ERROR only negative octave shifts implemented.");
    return;
  }
  current_global_octave_mask = global_octave_mask << -global_octave;
}


// micro seconds for main loop other then oscillating
// check this value for your program by PROFILING and
// set default here between average and maximum lapse
// time (displayed by pressing 'd')
unsigned long timeFor1Round = 220;

unsigned char toneSwitch=0;	// tone switched OFF by default
				// see toggle_toneSwitch(dummy)

void oscillatorInit() {
  int osc;

  for (osc=0; osc<OSCILLATORs; osc++) {
    osc_count[osc]=0;
    // use osc_mask's like 0x1 or 0x3
    osc_mask[osc]= 0x1 << DEFAULT_OCTAVE_SHIFT;

    period[osc] = next[osc] = farest_future;
    osc_flags[osc] = 0;		// off
    pinMode(oscPIN[osc], OUTPUT);
    oscMaskPIN[osc]=ILLEGALpin;
  }
  nextFlip=farest_future;
}


int startOscillator(int osc) {
  unsigned long now = micros();

  if (osc >= OSCILLATORs ) {	// ERROR recovery needed! #########

#ifdef USE_SERIAL
    Serial.println("ERROR: too many oscillators.");
#endif

    return 1;
  }

  if (oscPIN[osc] == ILLEGALpin)
    return 1;

  digitalWrite(oscPIN[osc], HIGH);

  osc_flags[osc] |= OSC_FLAG_ACTIVE;	// active ON
  next[osc] = now + period[osc];
  nextFlip = updateNextFlip();

  /*
  Serial.print("Started oscillator "); Serial.print(osc);
  Serial.print("\tpin "); Serial.print(oscPIN[osc]);
  Serial.print("\tperiod "); Serial.println(period[osc]);
  */

  return 0;
}


void stopOscillator(int osc) {
  osc_flags[osc] &= ~OSC_FLAG_ACTIVE;	// active OFF
  digitalWrite(oscPIN[osc], LOW);
  nextFlip = updateNextFlip();
}


// is oscillator ON or OFF?
int OSC_(int osc) {
  return (osc_count[osc] & current_global_octave_mask) !=0;
}


void set_osc_mask_pin(unsigned int osc, unsigned int pin) {
  oscMaskPIN[osc] = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}


void toggleOscillator(int osc) {
  if (osc_flags[osc] & OSC_FLAG_ACTIVE)	// was active
{
    stopOscillator(osc);
    Serial.print("Oscillator "); Serial.print(osc); Serial.println(" stopped");
  }
  else if (period[osc]) {
    startOscillator(osc);
  }
  else
    Serial.println("error: no period set");
}


// Bitmap of each oscillators mute status:
int oscillators_mute_bits() {
  int osc, bitpattern=0;

  for (osc=0; osc<OSCILLATORs; osc++) {
    if (osc_flags[osc] & OSC_FLAG_MUTE)
      bitpattern |= (1 << osc);
  }

  return bitpattern;
}


/* unused
// Bitmap of each oscillators select status:
int oscillators_SELECTed_bits() {
  int osc, bitpattern=0;

  for (osc=0; osc<OSCILLATORs; osc++) {
    if (osc_flags[osc] & OSC_FLAG_SELECT)
      bitpattern |= (1 << osc);
  }

  return bitpattern;
}
*/


#ifdef BIT_STRIPs
char show_interference_strip=0;	// ################ should be declared elsewhere
#endif

char show_mask=0;	// tempo indicator


void osc_flip_reaction(){	// whatever you want ;)
  int osc=0, led=0;
  int pin, bit;

  // check osc_mask reactions:
  if (show_mask) {
    for (osc=0; osc<OSCILLATORs; osc++) {
      if (osc_mask[osc] && ((osc_count[osc] & osc_mask[osc]) == osc_mask[osc])) {
	if (oscMaskPIN[osc] != ILLEGALpin)
	  digitalWrite(oscMaskPIN[osc], HIGH);
      } else {
	if (oscMaskPIN[osc] != ILLEGALpin)
	  digitalWrite(oscMaskPIN[osc], LOW);
      }
    }
  }

#ifdef BIT_STRIPs	// inside OSCILLATORs
  if (show_interference_strip) {
    /*
    for (bit=0; bit<4; bit++) {
      pin = bit_strip_PIN[output_strip][bit];
      digitalWrite(pin, ##############);
    }
    */

    /*
    bit=0;
    pin=17; // ################   pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, (OSC_(0) & OSC_(1)));

    bit=1;
    pin=16; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(0) | OSC_(1));

    bit=2;
    pin=15; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(0) ^ OSC_(1));

    bit=3;
    pin=14; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(0) == OSC_(1));
    */

    bit=0;
    pin=17; // ################   pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, (OSC_(osc) == OSC_(0)));

    bit=1;
    pin=16; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(osc) == OSC_(1));

    bit=2;
    pin=15; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(osc) == OSC_(2));

    bit=3;
    pin=14; //    pin = bit_strip_PIN[output_strip][bit];
    digitalWrite(pin, OSC_(osc) == OSC_(3));

  }
#endif // BIT_STRIPs inside OSCILLATORs

#ifdef COLOUR_LEDs	//  inside OSCILLATORs
  if (show_interference_color) {
    if (OSC_(osc) & OSC_(1))
      digitalWrite(redPIN, HIGH);
    else
      digitalWrite(redPIN, LOW);

    if (OSC_(osc) ^ OSC_(1))
      digitalWrite(greenPIN, HIGH);
    else
      digitalWrite(greenPIN, LOW);

//    // deactivated blue led, made me nervous ;)
//    if (!OSC_(osc) | OSC_(1))
//      digitalWrite(bluePIN, HIGH);
//    else
//      digitalWrite(bluePIN, LOW);
  }
#endif // COLOUR_LEDs	// inside OSCILLATORs

#ifdef LED_ROW	// inside OSCILLATORs
  // these are interesting regarding interferences:
  led=-1;

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] > next[1]);
  digitalWrite(led_row_cathode_PIN[led],LOW);

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] < next[1]);
  digitalWrite(led_row_cathode_PIN[led],LOW);

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] < next[1]);
  digitalWrite(led_row_cathode_PIN[led], OSC_(1));

  led++;
  digitalWrite(led_row_anode_PIN[led], next[0] > next[1]);
  digitalWrite(led_row_cathode_PIN[led], OSC_(1));

  /*
  led++;
  digitalWrite(led_row_anode_PIN[led],OSC_(0));
  digitalWrite(led_row_cathode_PIN[led],OSC_(1));
  
  led++;
  digitalWrite(led_row_anode_PIN[led],!OSC_(0));
  digitalWrite(led_row_cathode_PIN[led],OSC_(1));
  */
#endif	// LED_ROW	 inside OSCILLATORs
} // osc_flip_reaction()



/* **************************************************************** */
// #define BIT_STRIPs	2	// show numbers, bitmasks, etc on LEDs
//
// a number of (usually adjacent) arduino pins represent bit positions
//
/* **************************************************************** */
#ifdef BIT_STRIPs	// inside OSCILLATORs
// #define  MAX_BiTS_IN_STRIP 	4

signed char bit_strip_PIN[BIT_STRIPs][MAX_BiTS_IN_STRIP];
unsigned char bit_strip_bits[BIT_STRIPs];

// bit strips can have INPUT TAPs.
// pin to switch between TAPs and LEDs
// input switch pin (ILLEGALpin for no input capability)
unsigned char bit_strip_INPUT_PIN[BIT_STRIPs];

unsigned char output_strip=0;


int init_bit_strip(int start, int bits, int downwards, int input_pin) {
  /* 'downwards': PINs count down
     'input_pin': ILLEGALpin  for no input capability */
  static unsigned char strips=0;
  int bit, pin, strip=strips;
  int direction, skip=0;

  if (bits>MAX_BiTS_IN_STRIP) {		// ERROR protection #######################
    Serial.println("init_bit_strip: ERROR bits value too high");
    return -1;
  }

  bit_strip_bits[strip]=bits;

  bit_strip_INPUT_PIN[strip]=input_pin;			// input/output
  if (input_pin != ILLEGALpin) {
    pinMode(bit_strip_INPUT_PIN[strip], OUTPUT);
    digitalWrite(bit_strip_INPUT_PIN[strip], LOW);	// defaults to output
  }

  if (downwards)
    direction=-1;
  else
    direction=1;

  for (bit=0; bit<bits; bit++) {
  init_strip_pins:

    // Maybe the input pin is inside the led row?
    if ((pin = start +  skip + direction * (signed char) bit ) == input_pin ) {
      skip++;			// skip input pin
      goto init_strip_pins;
    }

    bit_strip_PIN[strip][bit] = pin;
    pinMode(bit_strip_PIN[strip][bit], OUTPUT);

#ifdef DEBUG_HW_ON_INIT			// blink each single LED
    digitalWrite(bit_strip_PIN[strip][bit], HIGH); delay(40);   // ON
    digitalWrite(bit_strip_PIN[strip][bit], LOW);  delay(60);	  // OFF
#endif

  }

#ifdef DEBUG_HW_ON_INIT			// count on all LEDs
  delay(250);
  { int i;
    for (i=0; i<(2 << bits)+1; i++){	// from zero to 'zero'
      show_on_strip(strip, i, 0);
      delay(30);
    }
  }
#endif

  return (int) strips++;
}


void show_on_strip(int strip, int value, int direction) {
  int bit, mask;
  int bits = bit_strip_bits[strip];

  if (direction == 0) {		// normal bit positions
    mask = 1;
    for (bit=0; bit<bits; bit++, mask *= 2 ) {
      digitalWrite(bit_strip_PIN[strip][bit], value & mask);    
    }
  } else {			// upside down
    mask = 1 << bits ;
    for (bit=0; bit<bits; bit++, mask /= 2 ) {
      digitalWrite(bit_strip_PIN[strip][bit], value & mask);    
    }
  }
}


// strips can have both, tap input's and led output sharing pins
void switch_strip_as_TAPs(int strip) {
  int bit, bits= bit_strip_bits[strip];

  // we must switch the input *first*, otherwise
  // some of the nearby taps pick up the change as a high...
  if (bit_strip_INPUT_PIN[strip] != ILLEGALpin)
    digitalWrite(bit_strip_INPUT_PIN[strip], HIGH);	// switch to input

  // before switching	make sure there are no pullup resistors
  // and get rid of spurious charges.
  for (bit=0; bit<bits; bit++) {
    pinMode(bit_strip_PIN[strip][bit], OUTPUT);
    digitalWrite(bit_strip_PIN[strip][bit], LOW);
  }

  // I put this in a second loop to let the pins settle on electrical LOW
  for (bit=0; bit<bits; bit++)
    pinMode(bit_strip_PIN[strip][bit], INPUT);
}


// strips can have both, tap input's and led output sharing pins
void switch_strip_as_LEDs(int strip) {
  int bit, bits= bit_strip_bits[strip];

  for (bit=0; bit<bits; bit++) {
    pinMode(bit_strip_PIN[strip][bit], OUTPUT);
    digitalWrite(bit_strip_PIN[strip][bit], LOW);
  }

  if (bit_strip_INPUT_PIN[strip] != ILLEGALpin)
    digitalWrite(bit_strip_INPUT_PIN[strip], LOW);	// switch to output
}


void toggle_toneSwitch(int dummy)
{
  if (toneSwitch ^= ~0) {
#ifdef BIT_STRIPs
    show_on_strip(output_strip, ~oscillators_mute_bits(), 0);	// show ON/mute on LED strip
#endif
    ;
  } else {
#ifdef BIT_STRIPs
    show_on_strip(output_strip, 0, 0);	// switch leds off
#endif
    ;
  }
}
#endif // BIT_STRIPs	inside OSCILLATORs


/* **************************************************************** */
#define SERIAL_STRIPs	1

#ifdef SERIAL_STRIPs

char serial_strip_data_PIN[SERIAL_STRIPs];
char serial_strip_data_clock[SERIAL_STRIPs];

unsigned char s_strp=0;
 
void init_out_shift(char data_PIN, char clock_PIN) {	// version 4094, simplest
  pinMode(data_PIN, OUTPUT); digitalWrite(data_PIN, LOW);
  pinMode(clock_PIN, OUTPUT); digitalWrite(clock_PIN, LOW);
  out_shift(data_PIN, clock_PIN, 0, 64);
}

void out_shift(char data_PIN, char clock_PIN, int value, char bits) {	// version 4094, simplest
  for ( ; bits > 0; bits--, value >>= 1) {
    digitalWrite(clock_PIN, LOW);
    digitalWrite(data_PIN, (value & 1 ));
    digitalWrite(clock_PIN, HIGH);
    // value >>= 1;
  }
}

#endif // SERIAL_STRIPs
/* **************************************************************** */



/* **************************************************************** */
// #define PROFILING	// gather info for debugging and profiling
			// mainly to find out how long the main loop
			// needs to come back to the oscillator in time.
/* **************************************************************** */
#ifdef PROFILING	// inside OSCILLATORs
unsigned long inTime=0, late=0, easy=0, urgent=0, repeat=0, quit=0;
unsigned long enteredOscillatorCount=0, profiledRounds=0, outsideOscRounds=0, leftOscillatorTime;
unsigned long lapseSum=0;
int lapse=0, maxLapse=0;
int dontProfileThisRound=0;	// no profiling when we spent time in menus or somesuch
float ratio;
unsigned char autoAdapt=false;


void reset_profiling() {
  profiledRounds=0;
  enteredOscillatorCount=0;
  inTime=0; late=0;
  easy=0; urgent=0;
  repeat=0; quit=0;
  lapse=0, lapseSum=0, maxLapse=0;
}

#endif // PROFILING	inside OSCILLATORs



/* **************************************************************** */
// #define OSCILLATOR_COLOUR_LED_PROFILING
/* **************************************************************** */
/*
  main oscillator routine  void oscillate()

  Check if it is time to switch a pin and do so

  If there is enough time (see 'timeFor1Round') return
  to the main loop do do other things (like reading sensors,
  changing sound parameters)

  If the next flip approaches loop until there is idle time.
*/

/* **************************************************************** */
void oscillate() {
  int osc;	// int might produce faster code then unsigned char
  unsigned long now = micros();

#ifdef OSCILLATOR_COLOUR_LED_PROFILING
  digitalWrite(bluePIN, HIGH);	// time inside oscillate()
  digitalWrite(greenPIN, HIGH); // no stress
  digitalWrite(redPIN, LOW);	// no stress
#endif

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
#endif	// PROFILING

  // First decide what to do now, based on current time.
  // If times are easy leave do do other things in between.
  // else check oscillators if it's time to flip.
  while (now >= nextFlip || ((nextFlip - now) <= timeFor1Round)) {
    for (osc=0; osc<OSCILLATORs; osc++) {
      if ((osc_flags[osc] & OSC_FLAG_ACTIVE) && (next[osc] <= now)) {
	// flip one
	osc_count[osc]++;
	// maybe sound
	if (toneSwitch && ((osc_flags[osc] & OSC_FLAG_MUTE) == 0)) {
	  digitalWrite(oscPIN[osc], (osc_count[osc] & current_global_octave_mask) != 0);
	}

	// compute new next on this oscillator
	next[osc] += period[osc];
      }
    }

    // compute new nextFlip on one of all oscillators
    nextFlip = updateNextFlip();

    // decide what to do next based on time
    now = micros();

    if (now < nextFlip)		// all oscillators up to date?
      osc_flip_reaction();	// whatever you want ;)

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
#endif	// PROFILING

#ifdef OSCILLATOR_COLOUR_LED_PROFILING	// stress?
    // second round only when urgent
    digitalWrite(greenPIN, LOW);	// stress
    digitalWrite(redPIN, HIGH);		// stress
#endif
  }  // while no time to leave oscillator

#ifdef PROFILING
  if (dontProfileThisRound==0) {
    profiledRounds++;
  }
  dontProfileThisRound = 0;

  leftOscillatorTime = micros();
#endif

#ifdef OSCILLATOR_COLOUR_LED_PROFILING
  digitalWrite(bluePIN, LOW);	// time outside oscillate()
  digitalWrite(greenPIN, HIGH); // no stress
  digitalWrite(redPIN, LOW);	// no stress
#endif
}


// compute when the next flip (in any of the active oscillators is due
long updateNextFlip () {
  int osc;
  nextFlip |= -1;

  for (osc=0; osc<OSCILLATORs; osc++) {
    if (osc_flags[osc] & OSC_FLAG_ACTIVE)
      if (next[osc] < nextFlip)
	nextFlip = next[osc];
  }
  // Serial.print("NEXT "); Serial.println(nextFlip);
  return nextFlip;
}

/* ***************************************************************** */


/* ***************************************************************** */
// (re) synchronizing oscillators:

// bitmasks for sync_mode:
#define SYNC_phase		1	// do sync phase
#define SYNC_phase_stright	2	//   sync phase stright/middle
#define SYNC_alternate_phase	4	//   even/uneven oscs start up/down
#define SYNC_reset_counter	8	// yes, do reset counter
#define SYNC_reset_to_zero	16	//   start at zero, counting up ||
#define SYNC_start_all_on	32	//   start all hot or all cold

unsigned int sync_mode = 27;

// the sync does not happen *exactly* now,
// but is setup for a very near point in the future
#define SYNC_after		120	// micros delay to set it all up

// sync all oscillators
void sync_all_oscillators(unsigned int sync_mode) {
  int osc;
  unsigned long sync_time = micros() + SYNC_after;

  for (osc=0; osc<OSCILLATORs; osc++) {

    if(sync_mode & SYNC_reset_counter) {	// reset counter
      // *either* just reset to 0, so it will start with 00000001
      if (sync_mode & SYNC_reset_to_zero)	// start with 0000001
	osc_count[osc] = 0;
      // *or* arrange to start with all bits zero or all bits one:
      else if (sync_mode & SYNC_start_all_on)	// start all bits on?
	osc_count[osc] = (unsigned long) -2;	// next flip will be all on
      else
	osc_count[osc] = (unsigned long) -1;	// next flip will be all off
    }

    if(sync_mode & SYNC_phase) {	// phase sync
      // sync phase on flip or middle of a (half) period?
      if(sync_mode & SYNC_phase_stright)
	next[osc] = sync_time;				// stright
      else
	next[osc] = sync_time + (period[osc] / 2);	// middle

      if(sync_mode & SYNC_alternate_phase)	// alternate phase
	if (osc & 1)				// every second osc
	  osc_count[osc] ^= 1;			// start the other way round
    }

  }
}

#endif // OSCILLATORs
/* **************************************************************** */



/* **************************************************************** */
// #define INPUTs_ANALOG	16	// ANALOG INPUTS
/* **************************************************************** */
#ifdef INPUTs_ANALOG

unsigned int inp=8;				// index of the analog input
unsigned int analog_input_cyclic_index=0;	// cycle throug the inputs to return in time to the oscillators

unsigned char analog_IN_state[INPUTs_ANALOG];
char analog_IN_PIN[INPUTs_ANALOG];
short analog_IN_last[INPUTs_ANALOG];


unsigned char analog_in2out_method[INPUTs_ANALOG];
  // bitmask
/* ################ must be thought about more...... ################ */
/* ################ put them in the list display, too ################ */
  #define METHOD_add		1	// 0: set parameter 1: add to parameter
  #define METHOD_continuous	2	// act every time, not only on change
  #define METHOD_linear		4	// math method linear/multiply
  // valid bits, PLEASE ALWAYS DO UPDATE!
  unsigned char in2out_method_valid_mask=0x00000111;

// parameters for the in2out translation:
short analog_input_offset[INPUTs_ANALOG];
long  analog_output_offset[INPUTs_ANALOG];
double analog_in2out_scaling[INPUTs_ANALOG];
double analog_in2out_reminder[INPUTs_ANALOG];


// destination
unsigned char analog_in2out_destination_type[INPUTs_ANALOG];	// i.e. osc, analog out
#define TYPE_no			0	//
#define TYPE_osc_period		1	// oscillator period
// #define TYPE_osc_phase  	2	// oscillator phase
#define TYPE_analog_out		3	// analog output

unsigned char analog_in2out_destination_index[INPUTs_ANALOG];	// osc


// Do this once:
void analog_inputs_initialize() {
  int inp;

  for (inp=0; inp<INPUTs_ANALOG; inp++) {
    analog_IN_state[inp] = 0;
    analog_IN_last[inp] = ILLEGALinputVALUE;
    analog_in2out_method[inp] = 0;
    analog_input_offset[inp] = 0;
    analog_output_offset[inp] = 0;
    analog_in2out_scaling[inp] = 1.0;
    analog_in2out_reminder[inp] = 0.0;
    analog_in2out_destination_type[inp] = TYPE_no;
    analog_in2out_destination_index[inp] = 0;
  }
}


// Do this once for each analog input:
int analog_input_setup(char pin, unsigned char state, unsigned char method,  \ 
		       short in_offset, long out_offset, double i2o_scaling, \
		       unsigned char destination_type, unsigned char index) {
  static unsigned char analog_inputs=0;
  int inp= analog_inputs;

  if (inp >= INPUTs_ANALOG) {	// ERROR

#ifdef USE_SERIAL
    Serial.println("ERROR: too many analog inputs.");
#endif

    return ILLEGAL;
  }

  analog_IN_PIN[inp]			= pin;
  analog_IN_state[inp]			= state;

  analog_in2out_method[inp]		= method;
  analog_input_offset[inp]		= in_offset;
  analog_output_offset[inp]		= out_offset;
  analog_in2out_scaling[inp]		= i2o_scaling;
  analog_in2out_destination_type[inp]	= destination_type;
  analog_in2out_destination_index[inp]	= index;

  return analog_inputs++;
}


short analog_IN(int inp) {
  return analogRead(analog_IN_PIN[inp]);
}


long analog_inval2out(int inp, int input_raw_value) {
  // Despite the long integer return type we do compute a long float value.
  // When adding to parameters we can accumulate cut-offs in analog_in2out_reminder[inp]
  double value = input_raw_value;
  long out_value;

  if (analog_in2out_method[inp] & METHOD_linear) {
    value += analog_input_offset[inp];
    value *= analog_in2out_scaling[inp];;
    value += analog_output_offset[inp];
  } // else: ERROR

  if(analog_in2out_method[inp] & METHOD_add) {
    value += analog_in2out_reminder[inp];		// reminder from last time
    out_value = (long) value;				// integer part
    analog_in2out_reminder[inp] = value - out_value;	// new reminder
  }
    
  return (long) value;
}


/* **************************************************************** */
// analog_input_cyclic_poll(), main routine dealing with analog inputs.
// The top level loop calls it whenever there is time.
// The routine checks *one* active input and triggers action, if appropriate.
// Then it returns immediately not to block more realtime critical tasks.

void analog_input_cyclic_poll() {
  int actual_index=inp;
  int value, osc;
  long now;

  {
    unsigned int i, inp;
    // find the next active input starting from analog_input_cyclic_index: 
    for (i=0; i<=INPUTs_ANALOG; i++) {
      inp = (analog_input_cyclic_index++ % INPUTs_ANALOG); // try all inputs, starting
      if (analog_IN_state[inp] & ACTIVE_undecided)	// found next active input
	break;
      if (i == INPUTs_ANALOG)				// there's no active input
	goto cyclic_done;
    }

    value = analog_IN(inp);
    // actions are triggered if the input has changed or if it's in continuous mode:
    if ((analog_in2out_method[inp] & METHOD_continuous) || (value != analog_IN_last[inp])) {
      analog_IN_last[inp] = value;

      switch (analog_in2out_destination_type[inp]) {

      case TYPE_no:	// nothing
	break;

      case TYPE_osc_period: // set oscillator period
	osc = analog_in2out_destination_index[inp];

	// add or set?
	if (analog_in2out_method[inp] & METHOD_add)	// add to parameter
  	  period[osc] += analog_inval2out(inp, value);
	else						// set parameter
	  period[osc] = analog_inval2out(inp, value);

	// check if next[osc] is not too far in the future:
	now = micros();
	if ((now + period[osc]) < next[osc]) { // we will not wait that long...
	  next[osc] = now + period[osc];
	  nextFlip = updateNextFlip();
	}
	break;

      default:	// ERROR

#ifdef USE_SERIAL
	Serial.print("analog_input_cyclic_poll: unknown analog_in2out_destination_type[inp].");
	Serial.print("\tinp "); Serial.println((int) inp);
#endif

	break;

      }
    }
  }

  cyclic_done:
  inp = actual_index;	// restore index
}


void display_analog_inputs_info() {
  int global_inp=inp;
  int inp, value;

  Serial.println("  state\tpin\tvalue\tlast\tout val\tmethod\tin offs\toutoff\tscaling\tout\ttype");
  Serial.println("");
  for (inp=0; inp<INPUTs_ANALOG; inp++) {
    if (inp==global_inp)
      Serial.print(" * ");
    else
      Serial.print("   ");

    if (analog_IN_state[inp] & ACTIVE_undecided)
      Serial.print("ON\t");
    else
      Serial.print("off\t");

    Serial.print((int) analog_IN_PIN[inp]); Serial.print("\t");

    Serial.print(value = analogRead(analog_IN_PIN[inp])); Serial.print("\t");
    Serial.print(analog_IN_last[inp]); Serial.print("\t");
    Serial.print(analog_inval2out(inp,value)); Serial.print("\t");

    Serial.print((int) analog_in2out_method[inp]); Serial.print("\t");
    Serial.print(analog_input_offset[inp]); Serial.print("\t");
    Serial.print(analog_output_offset[inp]); Serial.print("\t");
    Serial.print(analog_in2out_scaling[inp]); Serial.print("\t");

    Serial.print((int) analog_in2out_destination_index[inp]); Serial.print("\t");
    switch(analog_in2out_destination_type[inp]) {
    case TYPE_osc_period:
      Serial.print("oscperi");
      break;

    case TYPE_analog_out:
      Serial.print("analog");
      break;

    case TYPE_no:
      Serial.print("(none)");
      break;

    default:
      Serial.print("unknown");
    }

    Serial.println("");
  }
}

#endif	// INPUTs_ANALOG
/* **************************************************************** */



/* **************************************************************** */
// #define TAP_PINs	16  // # TAP pins for electrical touch INPUT
/* **************************************************************** */
#ifdef TAP_PINs

int tap = 0;	// index
char tapPIN[TAP_PINs];


// pointers on functions:

/*
  // pointers on  void something()  functions:
  void (*tap_do_on_tap[TAP_PINs])();
  void (*tap_do_on_toggle[TAP_PINs])();
*/

// pointers on  void something(int tap)  functions:
void (*tap_do_on_tap[TAP_PINs])(int);
void (*tap_do_on_toggle[TAP_PINs])(int);
void (*tap_do_after[TAP_PINs])(int);


// parameters for tap_do_on_xxx(tap) functions:
long tap_parameter_1[TAP_PINs];		// i.e. osc, PIN
// long tap_parameter_2[TAP_PINs];		//
double tap_parameter_double[TAP_PINs];
unsigned char *tap_parameter_char_address[TAP_PINs];

unsigned char tap_state[TAP_PINs];
// *logical* state:  0 inactive  1 OFF  2 ON (pin might be low, debouncing)
// bitmasks for tap_state[tap]
// #define ACTIVE_undecided	1	// globally used in many status bitmaps
#define TAP_STATE_ON		2	// *logical ON/OFF* (pin might be low, debouncing)
#define ENABLE_MULTITAP		4	// this tap waits for multitap detection
#define COUNTING_MULTITAPS	8	// waiting counting multitaps
#define TAP_IS_ANATAP		16	// tap on an analog input like a piezzo


// toggling is implemented with a counter. We get 2^n toggling for free ;)
unsigned char tap_count[TAP_PINs];


// debouncing:
unsigned long tap_debouncing_since[TAP_PINs];	// debouncing flag and time stamp
// unsigned long tap_debounce=68000L;		// debounce duration
unsigned long tap_debounce=120000L;		// debounce duration


void init_TAPs() {
  int tap;

  for (tap=0; tap<TAP_PINs; tap++) {
    tapPIN[tap] = ILLEGALpin;
    tap_state[tap] = 0;
    tap_debouncing_since[tap] = 0;

    // pointers on  void something(int tap)  functions:
    tap_do_on_tap[tap] = NULL;
    tap_do_on_toggle[tap] = NULL;
    tap_do_after[tap] = NULL;
  }
}


int setup_TAP(char pin, unsigned char toggle, void (*do_on_tap)(int), long parameter_1) {	// set toggle to 0 or 1...
  static int taps=0;
  int tap=taps;

  if (taps >= TAP_PINs) {	// ERROR

#ifdef USE_SERIAL
    Serial.println("ERROR: too many TAPs.");
#endif

    return -1;
  }

  tapPIN[tap] = pin;
  pinMode(pin, INPUT);
  tap_count[tap] = toggle;
  tap_state[tap] = ACTIVE_undecided;	// default active but OFF
  tap_debouncing_since[tap] = 0;	// not debouncing
  tap_do_on_tap[tap] = do_on_tap;
  tap_parameter_1[tap] = parameter_1;
  return taps++;	// returns tap index
}


// check if TAP 'tap' is logically ON
int TAP_(int tap) {
  if (tap_state[tap] & TAP_STATE_ON)
    return 1;	// yes, i want it to be 1, exactly
  else
    return 0;
}


// check if TAP 'tap' is *toggled* ON
int TAP_toggled_(int tap) {
  return (tap_count[tap] &  1) ;
}


/* **************************************************************** */
// void check_TAPs()  main routine dealing with TAPs:
// set logical state (including toggling) by checking PINs and debouncing.
// act if appropriate, then return to leave time to more realtime critical tasks.
//
// sorry, but i leave the colour LED debugging/profiling stuff in...
// I will do more like multitaps, long taps, ... and might need the leds to debug...
/* **************************************************************** */
// #define TAP_COLOUR_LED_DEBUG	// default does set  pinMode(PIN13, OUTPUT);
/*
  electrical state: PIN13
  logical ON	    green
  logical OFF	    red
  debouncing	    blue
 */

void check_TAPs() {
  // int tap=0;				// id as index
  int tap_electrical_activity=0;	// any tap switch electrical high?
  int did_do;
  unsigned long now=micros();

  for (tap=0; tap<TAP_PINs; tap++) {		// check all active TAPs
    if (tap_state[tap] &  ACTIVE_undecided) {		// active?, then check pin state
      // check anatab here ########################
      if (digitalRead(tapPIN[tap])) {		// TAP PIN is HIGH
	tap_electrical_activity=1;

#ifdef TAP_COLOUR_LED_DEBUG
	digitalWrite(bluePIN,LOW);
	digitalWrite(redPIN,LOW); digitalWrite(greenPIN,HIGH);
#endif

	// check if logical state is ON already:
	if ((tap_state[tap] & TAP_STATE_ON) == 0) {	// just switching on?
// if (!tap_state[tap] & ENABLE_MULTITAP) ################
	  did_do=0;

	  tap_count[tap]++;			//     yes, so count to toggle
	  tap_state[tap] |= TAP_STATE_ON;	//     meaning *logically* ON
	  tap_debouncing_since[tap] = 0;	//     not debouncing

	  /*
	  // pointers on  void something()  functions:
	  if (tap_do_on_tap[tap] !=0)		//		maybe do something on tap?
	    tap_do_on_tap[tap]();		//		yes
	  if (tap_count[tap] & 1 && tap_do_on_toggle[tap] !=0)	//  maybe do something on toggle?
	    tap_do_on_toggle[tap]();				//     yes
	  */

	  // pointers on  void something(int tap)  functions:
	  if (tap_do_on_tap[tap] !=NULL) {	//		maybe do something on tap?
	    (*tap_do_on_tap[tap])(tap);		//		  yes
	    did_do++;
	  }

	  if (tap_count[tap] & 1 && tap_do_on_toggle[tap] !=NULL) { //  maybe do something on toggle?
	    (*tap_do_on_toggle[tap])(tap);			    //    yes
	    did_do++;
	  }

	  if (did_do && (tap_do_after[tap] !=NULL))		// something else to do?
	    (*tap_do_after[tap])(tap);				//   yes
	} // switching to ON
      } else {					// TAP PIN is LOW
	if (tap_state[tap] & TAP_STATE_ON) {	//   LOW but, logical state ON. Debounce 

#ifdef TAP_COLOUR_LED_DEBUG
	  digitalWrite(bluePIN,HIGH);		// debounce
#endif

	  if(tap_debouncing_since[tap] != 0) {	//   debouncing is in progress
						//     long enough to switch off?
	    if ((now - tap_debouncing_since[tap]) > tap_debounce) {	// done?
	      tap_state[tap] &= ~TAP_STATE_ON;				//   switch to logical state 1, OFF
	      tap_debouncing_since[tap] = 0;				//   debouncing finished

#ifdef TAP_COLOUR_LED_DEBUG
	      digitalWrite(bluePIN,LOW); // debounce
	      digitalWrite(redPIN,HIGH); digitalWrite(greenPIN,LOW);
#endif

	    }
	  } else {
	    tap_debouncing_since[tap] = now;	//   start debouncing
	  }
	} // debounce
      } // pin low
    } // tap active
  } // for tap

  digitalWrite(PIN13,tap_electrical_activity);	// PIN13 is activity LED
} // check_TAPs()


// functions called by taps:
void tap_do_XOR_byte(int tap) {
  *tap_parameter_char_address[tap] ^= (unsigned char) tap_parameter_1[tap];
}


#ifdef OSCILLATORs
// individual oscillator mute function called by taps:
void tap_do_toggle_osc_mute(int tap) {	// old routine, left for reuse
  osc_flags[tap_parameter_1[tap]] ^= OSC_FLAG_MUTE;	// toggle muting of oscillator

#ifdef BIT_STRIPs
  show_on_strip(output_strip, ~oscillators_mute_bits(), 0);	// show ON/mute on LED strip
#endif

}
#endif // OSCILLATORs

#endif	// TAP_PINs
/* **************************************************************** */



/* **************************************************************** */
// more LED stuff:
// Quick hack to use some LED rows I have, 1 pin spacing between LEDs
// #define LED_ROW		4	// LED rows to stick in digital OUTPUTS
/* **************************************************************** */
#ifdef LED_ROW

int led=0;	// index

char led_row_anode_PIN[LED_ROW], led_row_cathode_PIN[LED_ROW];


void led_row_ON(int led) {
  digitalWrite(led_row_anode_PIN[led],HIGH); digitalWrite(led_row_cathode_PIN[led],LOW);	// ON
}


void init_LED_row(int start, int leds, int spacing, int anode_first) {
  int led;

  for (led=0; led < leds; led++) {
    if (anode_first) {
      led_row_anode_PIN[led]   = start +  led * (2 + spacing);
      led_row_cathode_PIN[led] = start +  led * (2 + spacing) + 1;
    } else {
      led_row_anode_PIN[led]   = start +  led * (2 + spacing) + 1;
      led_row_cathode_PIN[led] = start +  led * (2 + spacing);
    }
    pinMode(led_row_anode_PIN[led], OUTPUT);
    pinMode(led_row_cathode_PIN[led], OUTPUT);

    digitalWrite(led_row_anode_PIN[led],LOW);
    digitalWrite(led_row_cathode_PIN[led],LOW);

#ifdef DEBUG_HW_ON_INIT	// Serial info and blink to check LEDs
    Serial.print((int) led);
    Serial.print(" led_row_anode_PIN[led] ");
    Serial.print((int) led_row_anode_PIN[led]);
    Serial.print(" led_row_cathode_PIN[led] ");
    Serial.print((int) led_row_cathode_PIN[led]);
    Serial.println("");

    led_row_ON(led); delay(100);				// ON
    digitalWrite(led_row_anode_PIN[led],LOW); delay(200);	// OFF
#endif

  }
}

#endif // LED_ROW
/* **************************************************************** */



/* **************************************************************** */
// #define HARDWARE_menu   // hardware configuration menu interface
/* **************************************************************** */
#ifdef HARDWARE_menu

char hw_PIN = ILLEGALpin;

#endif // HARDWARE_menu
/* **************************************************************** */



/* **************************************************************** */
// #define MENU_over_serial	// do we use a serial menu?
/* **************************************************************** */
#ifdef MENU_over_serial

// sometimes serial is not ready quick enough:
// #define WAITforSERIAL 10


// char input with one byte buffering:
char stored_char, chars_stored=0;


int get_char() {
  if(!char_available())
    return -1;		// EOF no input available

  if (chars_stored) {
    --chars_stored;
    return stored_char;
  } else if (Serial.available())
    return Serial.read();
  else
    return -1;		// EOF no input available
}


int char_store(char c) {
  if (chars_stored) {	// ERROR ##################
    Serial.println("char_store: sorry, buffer full.");
  }

  chars_stored++;
  stored_char = c;
}


int char_available() {
  if (chars_stored || Serial.available()) 
    return 1;
  return 0;
}


// get numeric input from serial
long numericInput(long oldValue) {
  long input, num, sign=1;

  do {
    while (!char_available())	// wait for input
      ;
    delay(WAITforSERIAL);	// sometimes the second byte was not ready without that


    input = get_char();		// get first chiffre
  } while (input == ' ');	// skip leading space

  if (input == '-') {		//	check for sign
    sign = -1;
    input = get_char(); }
  else if (input == '+')
    input = get_char();

  if (input >= '0' && input <= '9')	// numeric?
    num = input - '0';
  else {				// NAN
    char_store(input);
    return oldValue;
  }

  while (char_available()) {
    input = get_char();
    if (input >= '0' && input <= '9')	// numeric?
      num = 10 * num + (input - '0');
    else {
      char_store(input);	// put NAN chars back into input buffer
      break;
    }
  }

  return sign * num;
}


// serial display helper function
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


// display helper function
void bar_graph(int value) {
  int i, length=64, scale=1023;
  int stars = ((long) value * (long) length) / scale + 1 ;


  if (value >=0 && value <= 1024) {
    Serial.print(value); Serial.print("\t");
    for (i=0; i<stars; i++) {
      if (i == 0 && value == 0)		// zero
	Serial.print("0");
      else if ((i == length/2 && value == 512) || (i == length && value == scale))	// middle or top
	Serial.print("|");
      else
	Serial.print("*");
    }
  } else {
    Serial.print("value "); Serial.print(value); Serial.println(" out of range."); }

  Serial.println("");
}


void bar_graph_VU(int pin) {
  int value, oldValue=-9997;	// just an unlikely value...
  int tolerance=0, menu_input;

  Serial.print("Follow pin "); Serial.print((int) pin); Serial.print("values\t\t +/- tolerance (send any other byte to stop)");

  while (true) {
    value =  analogRead(pin);
    if (abs(value - oldValue) > tolerance) {
      bar_graph(value);
      oldValue = value;
    }

    if (char_available()) {
      switch (menu_input = get_char()) {
      case '+':
	tolerance++;
	break;
      case '-':
	if (tolerance)
	  tolerance--;
	break;
      default:
	Serial.println("(quit)");
	return;		// exit
      }
    }
  }    
}


void display_analog_reads() {
  int i, value;

  Serial.print("pin\t");
  Serial.println("\t|\t\t\t\t|\t\t\t\t|");

  for (i=0; i<16; i++) {
    value = analogRead(i);
    Serial.print(i); Serial.print("\t"); bar_graph(value);
  }

  Serial.println("");
}


void watch_digital_input(int pin) {
  int value, old_value=-9997;

  Serial.print("watching digital input pin "); Serial.print((int) pin); Serial.println("\t\t(send any byte to stop)");
  while (!char_available()) {
    value = digitalRead(hw_PIN);
    if (value != old_value) {
      old_value = value;
      Serial.print("pin "); Serial.print((int) pin); Serial.print(" is ");
      if (value)
	Serial.println("HIGH");
      else
	Serial.println("LOW");
    }
  }
  Serial.println("(quit)");
  get_char();
}


// print binary numbers with leading zeroes and a space
void serial_print_BIN(unsigned long value, int bits) {
  int i;
  unsigned long mask=0;

  for (i = bits - 1; i >= 0; i--) {
    mask = (1 << i);
      if (value & mask)
	Serial.print(1);
      else
	Serial.print(0);
  }
  Serial.print(" ");
}


/* **************************************************************** */
#ifdef OSCILLATORs	// inside MENU_over_serial	MENU STUFF for OSCILLATORs

void displayOscillatorsInfos() {
  int i;

  Serial.println("  osc\tactive\tmute\tstate\tperiod\tPIN");
  Serial.println("");

  for (i=0; i<OSCILLATORs; i++) {
    if (i == osc)
      Serial.print("* ");
    else
      Serial.print("  ");

    Serial.print(i); Serial.print("\t");


    if (osc_flags[i] & OSC_FLAG_ACTIVE)
      Serial.print("ON\t");
    else
      Serial.print("off\t");

    if (osc_flags[i] & OSC_FLAG_MUTE)
      Serial.print("mute\t");
    else
      Serial.print(" on\t");

    if (osc_count[i] & 1)
      Serial.print("HIGH\t");
    else
      Serial.print("LOW\t");

    Serial.print(period[i]); Serial.print("\t");
    Serial.print(oscPIN[i]); Serial.println("");
  }

  Serial.println("");


  Serial.println("");
  Serial.println("  osc\toctaves\t\t\t\t  mask");
  Serial.println("");

  for (i=0; i<OSCILLATORs; i++) {
    if (i == osc)
      Serial.print("* ");
    else
      Serial.print("  ");
    Serial.print(i); Serial.print("\t"); serial_print_BIN(osc_count[i], 32);
    Serial.print(" "); serial_print_BIN(osc_mask[i], 32); Serial.println("");
  }

  Serial.println("");
}


void displayMenuOscillators() {
  Serial.println("");
  Serial.print("** MENU OSCILLATORs **  t=toggle tone ("); ONoff(toneSwitch, 2, false); Serial.println(")");

  Serial.print("display: O=oscs list\t");
  Serial.print("a=analog in\t");
  Serial.println("A=inp config");

  Serial.println("");

  Serial.print("o=oscillator("); Serial.print(osc);
  Serial.print(")\t~="); ONoff((osc_flags[osc] & OSC_FLAG_ACTIVE),2,false);
  Serial.print("\tp=period["); Serial.print(osc); Serial.print("] (");
  Serial.print(period[osc]); Serial.println(")");

  Serial.print("i=analog inp("); Serial.print(inp);
  Serial.print(")\tj=input offset ("); Serial.print(analog_input_offset[inp]);
  Serial.print(") \ts=scaling ("); Serial.print(analog_in2out_scaling[inp]);
  Serial.println(")");
  Serial.print("\t\tf=output offset("); Serial.print(analog_output_offset[inp]);
  Serial.print(")\t(last output="); Serial.print(analog_inval2out(inp, analog_IN_last[inp]));
  Serial.println(")");

#ifdef HARDWARE_menu
  Serial.println("");
  Serial.print("P=PIN (");
  if (hw_PIN == ILLEGALpin)
    Serial.print("no");
  else
    Serial.print((int) hw_PIN);
  Serial.println(")\tH=set HIGH\tL=set LOW\tW=analog write\tR=read");
  Serial.println("V=VU bar\tI=digiwatch");
#endif

  Serial.println("");
  Serial.print("r=expected roundtrip time (outside oscillator "); Serial.print(timeFor1Round); Serial.print(")");
  Serial.print("\ty=adapt "); ONoff(autoAdapt,0,0); Serial.println("");

#ifdef PROFILING
  Serial.print("d=debug, profiling\t");
#endif

}


// simple menu interface through serial port
void menuOscillators() {
  int menu_input;
  long newValue;

  while(!char_available())
    ;

  while(char_available()) {
    switch (menu_input = get_char()) {
    case ' ': case '\t':		// skip white chars
      break;

    case 'm': case '?':
      displayMenuOscillators();
      break;

    case 't':
      toneSwitch ^= ~0 ;
      Serial.print("tone: ");
      ONoff(toneSwitch, 1, false);
      Serial.println("");

      break;

#ifdef PROFILING
    case 'd':	// display profiling and debugging infos
      Serial.print("expected maximal timeFor1Round "); Serial.println(timeFor1Round);

      Serial.print("entered oscillator "); Serial.print(enteredOscillatorCount);
      Serial.print("\tprofiled rounds "); Serial.print(profiledRounds);
      Serial.print("\tskipped "); Serial.println(enteredOscillatorCount - profiledRounds);

      Serial.print("inTime "); Serial.print(inTime);
      Serial.print("\tlate   "); Serial.print(late);
      ratio = (float) inTime / (inTime + late);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("easy   "); Serial.print(easy);
      Serial.print("\turgent "); Serial.print(urgent);
      ratio = (float) easy / (easy + urgent);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("quit   "); Serial.print(quit);
      Serial.print("\trepeat "); Serial.print(repeat);
      ratio = (float) quit / (quit + repeat);
      Serial.print("\tratio "); Serial.println(ratio);

      Serial.print("maxLapse "); Serial.print(maxLapse);
      Serial.print("\taverage "); Serial.println(lapseSum / profiledRounds);

      Serial.print("rounds outside oscillator "); Serial.println(outsideOscRounds);

      break;

    case 'y':
      autoAdapt ^= -1 ;
      if (autoAdapt)
	Serial.println("adapt: ON");
      else
	Serial.println("adapt: OFF");
      break;
#endif

    case 'D':	// stuff to test, try, debug, always changing...
      debugSwitch ^= -1 ;

      analog_in2out_scaling[6] *= 2;
      analog_in2out_scaling[7] *= 2;

      /*
      if (debugSwitch) {
	Serial.println("debug: ON");
      pinMode(8,OUTPUT);
      pinMode(9,OUTPUT);
      pinMode(10,OUTPUT);
      }
      else {
	Serial.println("debug: OFF");
      pinMode(8,INPUT);
      pinMode(9,INPUT);
      pinMode(10,INPUT);
      }
      */

      /*
      newValue=tap_debounce / 2;
      tap_debounce += newValue;
      Serial.println(tap_debounce);
      */

      // tap_debounce=numericInput(tap_debounce);

      break;

    case 's': // set output scaling	########### need float input... ######################
      Serial.print("A"); Serial.print(inp); Serial.print("\tin2out scaling: ");
      Serial.println(analog_in2out_scaling[inp]);
      Serial.print("output scaling new = ");
      analog_in2out_scaling[inp] = numericInput(analog_in2out_scaling[inp]);
      Serial.println(analog_in2out_scaling[inp]);

      break;

    case 'f': // set output offset
      Serial.print("A"); Serial.print(inp); Serial.print("\toutput offset: ");
      Serial.println(analog_output_offset[inp]);
      Serial.print("output offset new = ");
      analog_output_offset[inp] = numericInput(inp);
      Serial.println(analog_output_offset[inp]);

      break;

    case 'o': // set menu local osc index to act on the given oscillator
      Serial.print("oscillator       "); Serial.println(osc);
      Serial.print("oscillator new = ");
      newValue = numericInput(osc);
      if (newValue < OSCILLATORs && newValue >= 0) {
	osc = newValue;
	Serial.println(osc);
      } else {
	Serial.print(" must be between 0 and "); Serial.println(OSCILLATORs -1);
      }

      break;

    case 'i': // set menu local analog input index
      Serial.print("analog inpput       "); Serial.println(inp);
      Serial.print("analog inpput new = ");
      newValue = numericInput(inp);
      if (newValue < INPUTs_ANALOG && newValue >= 0) {
	inp = newValue;
	Serial.println(inp);
      } else {
	Serial.print(" must be between 0 and "); Serial.println(INPUTs_ANALOG -1);
      }

      break;

    case 'r': // set timeFor1Round
      Serial.print("timeFor1Round       "); Serial.println(timeFor1Round);
      Serial.print("timeFor1Round new = ");
      newValue = numericInput(timeFor1Round);
      if (timeFor1Round > 0) {
	timeFor1Round = newValue;
      	Serial.println(timeFor1Round);

#ifdef PROFILING
	reset_profiling();	// reset profiling data
#endif

     } else
      	Serial.println("must be positive");

      break;

    case '~': // toggle oscillator on/off
      toggleOscillator(osc);

      break;

    case 'p':
      Serial.print("Oscillator "); Serial.print(osc);
      Serial.print(" \tperiod     = "); Serial.println(period[osc]);
      Serial.print("Oscillator "); Serial.print(osc);
      Serial.print(" \tperiod new = "); Serial.println(period[osc]);
      
      newValue = numericInput(period[osc]);
      if (period[osc] != newValue) {
	if (newValue > 0) {
	  period[osc] = newValue;
	  Serial.println(newValue);
	} else
	  Serial.println("must be positive.");
      }
      else
	Serial.println("(quit)");

    break;

    case 'O':
      Serial.println("");
      displayOscillatorsInfos();

      break;

    case 'A':
      Serial.println("");
      display_analog_inputs_info();

      break;

#ifdef HARDWARE_menu
    case 'P':
      Serial.print("Number of pin to work on: ");
      newValue = numericInput(period[osc]);
      if (newValue>=0 && newValue<255) {
	hw_PIN = newValue;
	Serial.println((int) hw_PIN);
      } else
	Serial.println("(none)");

      break;

    case 'H':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, HIGH);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to HIGH.");
      }

      break;

    case 'L':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, OUTPUT);
	digitalWrite(hw_PIN, LOW);
	Serial.print("PIN "); Serial.print((int) hw_PIN); Serial.println(" was set to LOW.");
      }

      break;

    case 'W':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	Serial.print("analog write value ");
	newValue = numericInput(-1);
	if (newValue>=0 && newValue<=255) {
	  Serial.println(newValue);

	  analogWrite(hw_PIN, newValue);
	  Serial.print("analogWrite("); Serial.print((int) hw_PIN);
	  Serial.print(", "); Serial.print(newValue); Serial.println(")");
	} else
	  Serial.println("(quit)");
      }

      break;

    case 'R':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	  Serial.print("analog value on pin "); Serial.print((int) hw_PIN); Serial.print(" is ");
	  Serial.println(analogRead(hw_PIN));
      }

      break;

    case 'V':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else
	bar_graph_VU(hw_PIN);

      break;

    case 'I':
      if (hw_PIN == ILLEGALpin)
	Serial.println("Select a pin with P.");
      else {
	pinMode(hw_PIN, INPUT);
	watch_digital_input(hw_PIN);
      }

      break;

#endif // HARDWARE_menu
    case 'a':
      display_analog_reads();

      break;

    default:
      Serial.print("unknown menu_input: "); Serial.print(byte(menu_input));
      Serial.print(" = "); Serial.println(menu_input);
      while (char_available() > 0) {
	menu_input = get_char();
	Serial.print(byte(menu_input));
      }
      Serial.println("");

      break;
    }

    if (!char_available())
      delay(WAITforSERIAL);
  }

#ifdef PROFILING
  dontProfileThisRound=1;
#endif

} // menuOscillators()

#endif // OSCILLATORs	inside MENU_over_serial


void initMenu() {
  Serial.println("Press 'm' or '?' for menu.");

#ifdef OSCILLATORs
  displayMenuOscillators();
  // displayOscillatorsInfos();
#endif

  Serial.println("");
}


#endif // MENU_over_serial
/* **************************************************************** */



/* **************************************************************** */
// tuning wheels
/* **************************************************************** */

// controlling parameters like period, phase, analog outputs through
// analog inputs like piezzos or accelerometers.

/* **************************************************************** */
#ifdef HAS_TUNING_WHEELs
char up=ILLEGAL, down=ILLEGAL;	// up/down analog inputs, like piezzos	################
char acc_x=ILLEGAL, acc_y=ILLEGAL, acc_z=ILLEGAL;	// accelerometer


#define PIEZZO_UpDown	0
#define ACCELERO_x	1
#define ACCELERO_y	2
#define ACCELERO_z	3
#define TUNING_WHEELS	4	// *** always keep up to date ***


char tuning_wheel=ACCELERO_x;	// default
// char tuning_wheel=PIEZZO_UpDown;

#define TUNE_period		0
#define TUNE_phase		1
#define TUNING_parameters	2	// *** always keep up to date ***

char tuned_parameter = TUNE_period;

#endif HAS_TUNING_WHEELs
/* **************************************************************** */



/* **************************************************************** */
// #define TAP_EDIT	// user interface over taps
/* **************************************************************** */
#ifdef TAP_EDIT
#define TAP_ED_SERIAL_DEBUG	// debugging only

#define EDIT_BITS_MASK	0x00ff	// i/o bits like taps and leds
				// currently only 4 in use

unsigned char edit_strip;	// I/O strip for editing
int input_value;		// any int value (for input)
unsigned int edit_type=0;	// what type of editing is going on?


// edit_type codes
#define EDIT_no				0x00   //  0000
#define EDIT_ANALOG_select		0x01   //  0001
#define EDIT_ANALOG_destination_index	0x02   //  0010
#define EDIT_ANALOG_destination_type	0x03   //  0011
#define EDIT_ANALOG_scaling		0x04   //  0100
#define EDIT_ANALOG_inp_offset		0x05   //  0101
#define EDIT_ANALOG_out_offset		0x06   //  0110
// #define EDIT_ANALOG_out_method		0x07   //  0111
#define EDIT_HAND_TUNE			0x07   //  0111

#define EDIT_OUT_OSC_mul_div		0x08   //  1000
  // hold integer rational factors:
  int mul=1, divi=1;	// name 'div' is taken

  // maybe hold a source period:
  unsigned long mul_div_source;

  // interface logic states:
  unsigned char mul_div_state=0;
// flags for the mul_div input logic
#define MUL_DIV_INPUT_divi		1	// next numeric input will be divi
#define MUL_DIV_INPUT_dest		2
#define MUL_DIV_has_source		4

#define EDIT_OUT_OSC_select		0x09   //  1001
#define EDIT_OSC_sync			0x0a   //  1010
#define EDIT_OUTPUT_method		0x0b   //  1011
#define EDIT_OUTPUT_mask		0x0c   //  1100

// ################ temporary hack:
#define EDIT_OUTPUT_switch_out_0	0x0d   //  1101
#define EDIT_OUTPUT_switch_out_1	0x0e   //  1110

#define EDIT_undecided_active		0x0f   //  1111


double scaling_zoom_factor=2.0;
int offset_zoom_factor=2;

#define IMPOSSIBLE_MASK		0x0f00


void tap_edit_setup(int setPIN, int hotPIN, int coldPIN) {
  int tap;

  tap= setup_TAP(setPIN,  1, &SET_TAP_do,  -999);
  tap= setup_TAP(hotPIN,  0, &HOT_TAP_do,  -999);
  tap= setup_TAP(coldPIN, 0, &COLD_TAP_do, -999);
}


// action of the SET tap:
void SET_TAP_do(int dummy) {
  switch (edit_type) {

  case EDIT_no:

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("activated editing on TAPs");
#endif

    switch_edit_type(EDIT_undecided_active);

#ifdef COLOUR_LEDs
    show_interference_color=0;
    blue(true);
#endif

#ifdef BIT_STRIPs
    show_interference_strip=0;
#endif

    show_on_strip(output_strip, 0, 0);
    switch_strip_as_TAPs(edit_strip);

    break;

  case EDIT_undecided_active:
    if (input_value > 0 && input_value != EDIT_undecided_active) {	// user selected edit_type
      switch_edit_type(input_value);
      green(true);
    } else 
      end_edit_mode();

    break;

  case EDIT_ANALOG_select:
    end_edit_mode();

    break;

  case EDIT_ANALOG_destination_index:
    // only oscillators implemented here ################
    if(input_value >= 0 && input_value < OSCILLATORs) {
      osc = input_value;	// global
      // set destination, activate, switch other inputs of the same dest off:
      set_analog_destination(inp, TYPE_osc_period, osc);
      show_on_strip(output_strip, input_value, 0);
      analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set destination of inp "); Serial.print((int) inp); Serial.print(" to "); Serial.println(input_value);
#endif

    }

    end_edit_mode();

    break;

  case EDIT_ANALOG_destination_type:
    end_edit_mode();	// ###############

    break;

  case EDIT_ANALOG_scaling:
    if(input_value) {
      analog_in2out_scaling[inp] = (double) input_value;
      analog_IN_last[inp] = ILLEGALinputVALUE;
      show_on_strip(output_strip, input_value, 0);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set scaling of inp "); Serial.print((int) inp); Serial.print(" to "); Serial.println((double) input_value);
#endif

      analog_IN_last[inp] = ILLEGALinputVALUE;
    }

    end_edit_mode();

    break;

  case EDIT_ANALOG_inp_offset:
    end_edit_mode();	// ###############

    break;

  case EDIT_ANALOG_out_offset:
    end_edit_mode();	// ###############

    break;

  case EDIT_HAND_TUNE:
    // analog_in2out_method[inp] is set on the fly, so just quit
    end_edit_mode();

    break;

  case EDIT_OUT_OSC_mul_div:
    mul_div_state |= MUL_DIV_INPUT_dest;	// no source input any more

    // is input mul or divi?
    if (mul_div_state & MUL_DIV_INPUT_divi) {		// divi input
      if (input_value == 00) {	// input_value zero exits edit mode
	end_edit_mode();
      } else {	// it *is* divi input:
	divi = input_value;
	show_on_strip(output_strip, input_value, 0);
	switch_strip_as_TAPs(edit_strip);			// the led might trigger tap

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("Set divi to " ); Serial.print(divi); Serial.println("");
#endif

	input_value = 0;	// so next time we will exit
      }
    } else {					// MUL INPUT
      if (input_value) {	// input_value zero does not touch mul, but proceed to divi input
	mul = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("Set mul to " ); Serial.print(mul); Serial.println("");
#endif
      }

      mul_div_state |= MUL_DIV_INPUT_divi;	// next numeric input will be divi
      input_value = divi;			// expecting divi (or multiply only)
      show_on_strip(output_strip, input_value, 0);	// already show divi
      switch_strip_as_TAPs(edit_strip);			// the led might trigger tap

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.println("(next numeric input will be div)");
#endif

    }

    break;

  case EDIT_OUT_OSC_select:
    if(input_value >= 0 && input_value < OSCILLATORs) {
      osc = input_value;
      show_on_strip(output_strip, input_value, 0);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set osc to "); Serial.print(input_value); Serial.println("");
#endif

    }

    end_edit_mode();

    break;

  case EDIT_OSC_sync:
    end_edit_mode();
    break;

  case EDIT_OUTPUT_method:
    break;

  case EDIT_OUTPUT_mask:
    if(input_value != IMPOSSIBLE_MASK) {
      input_value &= EDIT_BITS_MASK;
      show_on_strip(output_strip, input_value, 0);
      input_value <<= DEFAULT_OCTAVE_SHIFT;
      osc_mask[osc] = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set octave mask of inp "); Serial.print((int) inp);
      Serial.print(" to "); serial_print_BIN(input_value, 32);
#endif

    }

    end_edit_mode();

    break;

  case EDIT_OUTPUT_switch_out_0:
    if (oscPIN[0] == 47) {	// ################ temporary hack
      pinMode(20, OUTPUT);
      oscPIN[0] = 20;
    } else
      oscPIN[0] = 47;

    end_edit_mode();

    break;

  case EDIT_OUTPUT_switch_out_1:
    if (oscPIN[1] == 49) {	// ################ temporary hack
      pinMode(19, OUTPUT);
      oscPIN[1] = 19;
    } else
      oscPIN[1] = 49;

    end_edit_mode();

    break;

  default:
    end_edit_mode();	// ###############

    break;
  }
} // SET_TAP_do()


void global_shift_up_maybe(void) {
  if(global_octave < 0) {
    ++global_octave;
    global_shift(global_octave);
    show_on_strip(output_strip, -global_octave, 0);

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Raised global_octave to    "); Serial.print(global_octave);
    Serial.print("\tmask= "); serial_print_BIN(current_global_octave_mask, 32); Serial.println("");
#endif
  }
}


// action of the HOT/UP/YES pad:
void HOT_TAP_do(int dummy) {
  switch (edit_type) {
  case EDIT_no:
    global_shift_up_maybe();
    break;

  case EDIT_undecided_active:
    break;

  case EDIT_ANALOG_select:
    if(input_value >= 0 && input_value < INPUTs_ANALOG) {
      inp = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Activated inp "); Serial.print((int) inp); Serial.println("");
#endif

      analog_IN_state[inp] |= ACTIVE_undecided;	// activate analog input

      // switch all other inputs of the same destination off...
      set_analog_destination(inp, analog_in2out_destination_type[inp], analog_in2out_destination_index[inp]);
    }
    break;

  case EDIT_ANALOG_destination_index:
    break;

  case EDIT_ANALOG_destination_type:
    break;

  case EDIT_ANALOG_scaling:
    analog_in2out_scaling[inp] *= scaling_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed scaling of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_in2out_scaling[inp]);
#endif

    break;

  case EDIT_ANALOG_inp_offset:
    analog_input_offset[inp] *= offset_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed input offset of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_input_offset[inp]);
#endif

    break;

  case EDIT_ANALOG_out_offset:
    analog_output_offset[inp] *= offset_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed output offset of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_output_offset[inp]);
#endif

    break;

  case EDIT_HAND_TUNE:

#ifdef HAS_TUNING_WHEELs
    // cycle_driven_parameter like period, phase
#endif

    break;

  case EDIT_OUT_OSC_mul_div:
    global_shift_up_maybe();
    break;

  case EDIT_OUT_OSC_select:
    break;

  case EDIT_OSC_sync:
    sync_all_oscillators(sync_mode | SYNC_phase_stright);

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("synced all oscillators, mode "); serial_print_BIN(sync_mode | SYNC_phase_stright, 8); Serial.println("");
#endif

    break;

  case EDIT_OUTPUT_method:
    break;

  case EDIT_OUTPUT_mask:
    if(input_value != IMPOSSIBLE_MASK) {
      input_value &= EDIT_BITS_MASK;
      input_value <<= DEFAULT_OCTAVE_SHIFT;
      osc_mask[osc] = input_value;
      input_value = IMPOSSIBLE_MASK;
    }

    osc_mask[osc] >>= 1;

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Shifted octave mask of inp "); Serial.print((int) inp); Serial.print(" up to ");
      serial_print_BIN(osc_mask[osc], 32); Serial.println("");
#endif

    break;

  default:
    break;
  }
} // HOT_TAP_do()


void global_shift_down() {
  --global_octave;
  global_shift(global_octave);
  show_on_strip(output_strip, -global_octave, 0);

#ifdef TAP_ED_SERIAL_DEBUG
  Serial.print("Lowered global_octave to "); Serial.print(global_octave); Serial.println("");
  Serial.print("current_global_octave_mask "); serial_print_BIN(current_global_octave_mask, 32); Serial.println("");
#endif
}


// action of the cold/DOWN/NO pad:
void COLD_TAP_do(int dummy) {
  switch (edit_type) {
  case EDIT_no:
    global_shift_down();
    break;

  case EDIT_undecided_active:
    break;

  case EDIT_ANALOG_select:
    if (input_value >= 0 && input_value < INPUTs_ANALOG) {
      inp = input_value;
      analog_IN_state[inp] &= ~ACTIVE_undecided;	// deactivate analog input

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Deactivated inp "); Serial.print((int) inp); Serial.println("");
#endif

    }
    break;

  case EDIT_ANALOG_destination_index:
    break;

  case EDIT_ANALOG_destination_type:
    break;

  case EDIT_ANALOG_scaling:
    analog_in2out_scaling[inp] /= scaling_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed scaling of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_in2out_scaling[inp]);
#endif

    break;

  case EDIT_ANALOG_inp_offset:
    analog_input_offset[inp] /= offset_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed input offset of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_input_offset[inp]);
#endif

    break;

  case EDIT_ANALOG_out_offset:
    analog_output_offset[inp] /= offset_zoom_factor;
    analog_IN_last[inp] = ILLEGALinputVALUE;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Zoomed output offset of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println(analog_output_offset[inp]);
#endif

    break;

  case EDIT_HAND_TUNE:

#ifdef HAS_TUNING_WHEELs
    // cycle input sources like up/down piezzos, accelerometer x, y, z, generic analog input.
    tuning_wheel = ++tuning_wheel % TUNING_WHEELS;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Cycle through tuning_wheel's to "); Serial.println((int) tuning_wheel);
#endif

#endif

    break;

  case EDIT_OUT_OSC_mul_div:
    global_shift_down();
    break;

  case EDIT_OUT_OSC_select:
    break;

  case EDIT_OSC_sync:
    sync_all_oscillators(sync_mode & ~SYNC_phase_stright);

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("synced all oscillators, mode "); serial_print_BIN(sync_mode & ~SYNC_phase_stright, 8); Serial.println("");
#endif

    break;

  case EDIT_OUTPUT_method:
    break;

  case EDIT_OUTPUT_mask:
    if(input_value != IMPOSSIBLE_MASK) {
      input_value &= EDIT_BITS_MASK;
      input_value <<= DEFAULT_OCTAVE_SHIFT;
      osc_mask[osc] = input_value;
      input_value = IMPOSSIBLE_MASK;
    }

    osc_mask[osc] <<= 1;
    
#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Shifted octave mask of inp "); Serial.print((int) inp); Serial.print(" down to ");
      serial_print_BIN(osc_mask[osc], 32);
#endif

    break;

  default:
    break;
  }
}  // COLD_TAP_do()


void switch_edit_type(int new_edit_type) {
  input_value = 0;

  switch (edit_type = new_edit_type) {

  case EDIT_no:

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type OFF");
#endif

    end_edit_mode();
    break;

  case EDIT_undecided_active:
    break;

  case EDIT_ANALOG_select:
    input_value = inp;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type EDIT_ANALOG_select");
    Serial.print("enter with  inp="); Serial.println(input_value);
#endif

    break;

  case EDIT_ANALOG_destination_index:
    // only oscillators implemented here ################
    input_value =  analog_in2out_destination_index[inp];

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_ANALOG_destination_index");
    Serial.print("enter with  destination_index="); Serial.println(input_value);
#endif

    break;

  case EDIT_ANALOG_destination_type:
    break;

  case EDIT_ANALOG_scaling:
    input_value = 0;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_ANALOG_scaling");
#endif

    break;

  case EDIT_ANALOG_inp_offset:
    break;

  case EDIT_ANALOG_out_offset:
    break;

  case EDIT_HAND_TUNE:
    input_value = analog_in2out_method[inp];

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_HAND_TUNE");
    Serial.print("enter with  method="); serial_print_BIN(input_value, 8); Serial.println("");
#endif

    break;

  case EDIT_OUT_OSC_mul_div:
    mul_div_state = 0;			// expects source or mul now
    // different strategies: keep prior mul/divi values or reset them to 1?
    // let's try out to keep them if they look save
    mul = (mul < 1) ? 1: mul;
    divi= (divi< 1) ? 1: divi;
    input_value=mul;			// now mul is expected (or source)
    show_on_strip(output_strip, input_value, 0);
    switch_strip_as_TAPs(edit_strip);	// the led might trigger tap


#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_OUT_OSC_mul_div");
    Serial.print("enter with values of  mul="); Serial.print(mul); Serial.print("  and  divi="); Serial.println(divi); 
#endif

    break;

  case EDIT_OUT_OSC_select:
    input_value= osc;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to TAP_ED_SERIAL_DEBUG");
    Serial.print("enter with  osc="); Serial.println(input_value);
#endif

    break;

  case EDIT_OSC_sync:
    input_value = sync_mode;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_OSC_sync");
    Serial.print("enter with  sync_mode="); serial_print_BIN(input_value, 8); Serial.println("");
#endif

    break;

  case EDIT_OUTPUT_method:
    break;

  case EDIT_OUTPUT_mask:
    input_value = IMPOSSIBLE_MASK;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type to EDIT_OUTPUT_mask");
#endif

    break;

  default:

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("switch_edit_type ERROR: unknown type");
#endif

    break;
  }

  show_on_strip(output_strip, input_value, 0);
} // switch_edit_type()


void end_edit_mode() {
  edit_type = EDIT_no;
  show_on_strip(output_strip, 0, 0);
  input_value = 0;	// robustness

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.println("end_edit_mode");
#endif

#ifdef COLOUR_LEDs
  blue(false);
  red(false);
  green(false);
#endif
}


void tap_input_bit_toggle(int tap) {
    input_value ^= tap_parameter_1[tap];
    show_on_strip(output_strip, input_value, 0);
    switch_strip_as_TAPs(edit_strip);
}


// action of the edit 4-TAP row:
void editTAPs_do(int tap) {
  switch (edit_type) {
  case EDIT_no:
    editTAPs_noedit_do(tap);
    return;
    break;

  case EDIT_undecided_active:
    tap_input_bit_toggle(tap);
    break;

  case EDIT_ANALOG_select:
    tap_input_bit_toggle(tap);

    if (input_value >= 0 && input_value < INPUTs_ANALOG) {
      inp = input_value;	// global
      green(true); red(false);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set inp to "); Serial.println(input_value);
#endif

    } else {
      red(true); green(false);
    }

    switch_strip_as_TAPs(edit_strip);	// the led might trigger tap

    break;

  case EDIT_ANALOG_destination_index:
    tap_input_bit_toggle(tap);

    if (input_value >= 0 && input_value < OSCILLATORs) {
      osc = input_value;
      green(true); red(false);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set osc to "); Serial.println(input_value);
#endif

    } else {
      red(true); green(false);
    }
 
    switch_strip_as_TAPs(edit_strip);	// the led might trigger tap

    break;

  case EDIT_ANALOG_destination_type:
    break;

  case EDIT_ANALOG_scaling:
    tap_input_bit_toggle(tap);
    break;

  case EDIT_ANALOG_inp_offset:
    break;

  case EDIT_ANALOG_out_offset:
    break;

  case EDIT_HAND_TUNE:
    tap_input_bit_toggle(tap);			// set analog_in2out_method[inp]
    input_value &= in2out_method_valid_mask;	// mask out illegal bits
    analog_in2out_method[inp] = input_value;	// and set on the fly
    show_on_strip(output_strip, input_value, 0);// show
    switch_strip_as_TAPs(edit_strip);		// the led might trigger tap

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Set analog_in2out_method[inp] to "); serial_print_BIN(input_value, 8); Serial.println("");
#endif

    break;

  case EDIT_OUT_OSC_mul_div:
    tap_input_bit_toggle(tap);
    mul_div_state |= MUL_DIV_INPUT_dest;	// no source input any more

    // mul or divi input?
    if (mul_div_state & MUL_DIV_INPUT_divi) {	// divi bit toggle input
      if (input_value) {
	divi = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("temporally set divi to " ); Serial.print(divi); Serial.println("");
#endif

      }
    } else {					// mul bit toggle input
      if (input_value) {
	mul = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("temporally set mul to " ); Serial.print(mul); Serial.println("");
#endif
      }
    }

    break;

  case EDIT_OUT_OSC_select:
    tap_input_bit_toggle(tap);

    if (input_value >= 0 && input_value < OSCILLATORs) {
      osc = input_value;
      green(true); red(false);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set osc to "); Serial.println(input_value);
#endif

    } else {
      red(true); green(false);
    }
 
    switch_strip_as_TAPs(edit_strip);	// the led might trigger tap
    break;

  case EDIT_OSC_sync:
    tap_input_bit_toggle(tap);
    sync_mode = input_value;

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Set sync_mode to "); serial_print_BIN(input_value,8); Serial.println("");
#endif

    break;

  case EDIT_OUTPUT_method:
    break;

  case EDIT_OUTPUT_mask:
    tap_input_bit_toggle(tap);

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set output_mask to "); Serial.println(input_value);
#endif

    break;

  default:
    break;
  }
} // editTAPs_do()

void editTAPs_noedit_do(int tap) {
  // we just use the mask bits from editing mode
  // to recognize the taps:
  switch (tap_parameter_1[tap]) {

  case 8:
    show_interference_color ^= ~0;
    if (!show_interference_color) { // shut down active leds 
      red(false);
      green(false);
      // blue(false);
    }
    break;

  case 4:
    show_interference_strip ^= ~0;
    if (!show_interference_strip) { // shut down active leds 
      show_on_strip(output_strip, 0, 0);
    }
    break;

  case 2:
    show_on_strip(output_strip, 0, 0);
    break;

  case 1:
    show_mask ^= ~0;
    break;

  default:
    Serial.println("editTAPs_noedit_do: ERROR unknown code.");
  }
}


// check if any active input influences a given output
// return the first inp found or ILLEGAL if none
int which_inp_sets_this_output_(int type, int index) {
  int inp;

  for (inp=0; inp<INPUTs_ANALOG; inp++) {
    // check all acive inputs for same destination:
    if (analog_IN_state[inp] & ACTIVE_undecided)	// active?
      if ((analog_in2out_destination_index[inp] == index) && (analog_in2out_destination_type[inp] == type))
	return inp;
  }

  return ILLEGAL;
}


// Set destination of an analog input, activate it,
// and switch all other inputs of the same destination off...
void set_analog_destination(int inp, int type, int index) {
  int i;

  if (inp >= 0 && inp < INPUTs_ANALOG) {
    switch (type) {

    case TYPE_no:
      break;

    case TYPE_osc_period: case TYPE_analog_out:
      // brute force: deactivate all (other) inputs with the same destination:
      while ((i = which_inp_sets_this_output_(type, index)) != ILLEGAL) {
	analog_IN_state[i] &= ~ACTIVE_undecided;	// deactivate

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("set_analog_destination:  deactivate input "); Serial.print(i); Serial.println("] ");
#endif

      }

      // now activate selected input
      analog_IN_state[inp] |= ACTIVE_undecided;	// activate selected input
      analog_IN_last[inp] = ILLEGALinputVALUE;	// trigger first action

      analog_in2out_destination_type[inp] = type;
      analog_in2out_destination_index[inp] = index;

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("set_analog_destination  inp="); Serial.print(inp);
      Serial.print("  type="); serial_print_BIN(type, 8);
      Serial.print("  index="); Serial.println(index);
#endif

      break;

    default:
      Serial.println("set_analog_destination: ERROR unknown output type.");
    }
  }
  else
    Serial.println("set_analog_destination: ERROR inp out of range.");
}
#endif	// TAP_EDIT
/* **************************************************************** */



/* **************************************************************** */
void set_unset_regulate_this_output(int destination_type, int destination_index) {
  int inp, was_active_already=0;

  if (up==ILLEGAL || down==ILLEGAL) { // ERROR
#ifdef USE_SERIAL
    Serial.println("ERROR 'up' and 'down' inputs not available");
#endif
    return;
  }

  // deactivate *all* inputs that influence this output
  while ((inp = which_inp_sets_this_output_(destination_type, destination_index)) != ILLEGAL) {
    analog_IN_state[inp] &= ~ACTIVE_undecided;	// switch it off
    if ((inp==up) || (inp==down) || (inp=acc_x) || (inp=acc_y) || (inp=acc_z))
      was_active_already = 1;
    else {

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("set_unset_regulate_this_output: deactivate input "); Serial.print(inp); Serial.println("] ");
#endif
      ;
    }
  }

  if (was_active_already) {	// if it was active already we just turn it off, done.

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.println("hand tuning switched off ");
#endif

    return;
  }

 // any other case setup and activate a hand tuning wheel:
  switch (tuning_wheel) {

  case PIEZZO_UpDown:
    // in this case two separate up/down inputs are used:

    analog_in2out_reminder[up]			= 0.0;	// reset reminder 
    analog_in2out_destination_type[up]		= destination_type;
    analog_in2out_destination_index[up]		= destination_index;
    analog_IN_state[up] |= ACTIVE_undecided;	// switch it on

    analog_in2out_reminder[down]		= 0.0;	// reset reminder 
    analog_in2out_destination_type[down]	= destination_type;
    analog_in2out_destination_index[down]	= destination_index;
    analog_IN_state[down] |= ACTIVE_undecided;	// switch it on

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("Activated tuning_wheel PIEZZO_UpDown");
#endif

    break;

  case ACCELERO_x:
    // calibrate to cancel current acceleration:
    analog_input_offset[acc_x] = -analogRead(analog_IN_PIN[acc_x]);

    analog_in2out_reminder[acc_x] 		= 0.0;  // reset reminder 
    analog_in2out_destination_type[acc_x]	= destination_type;
    analog_in2out_destination_index[acc_x]	= destination_index;
    analog_IN_state[acc_x] |= ACTIVE_undecided;	// switch it on

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("Activated tuning_wheel ACCELERO_x");
#endif

    break;

  case ACCELERO_y:
    // calibrate to cancel current acceleration:
    analog_input_offset[acc_y] = -analogRead(analog_IN_PIN[acc_y]);

    analog_in2out_reminder[acc_y] 		= 0.0;  // reset reminder 
    analog_in2out_destination_type[acc_y]	= destination_type;
    analog_in2out_destination_index[acc_y]	= destination_index;
    analog_IN_state[acc_y] |= ACTIVE_undecided;	// switch it on

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("Activated tuning_wheel ACCELERO_y");
#endif

    break;

  case ACCELERO_z:
    // calibrate to cancel current acceleration:
    analog_input_offset[acc_z] = -analogRead(analog_IN_PIN[acc_z]);

    analog_in2out_reminder[acc_z] 		= 0.0;  // reset reminder 
    analog_in2out_destination_type[acc_z]	= destination_type;
    analog_in2out_destination_index[acc_z]	= destination_index;
    analog_IN_state[acc_z] |= ACTIVE_undecided;	// switch it on

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.println("Activated tuning_wheel ACCELERO_z");
#endif

    break;

  default:
    // ERROR
    ;
  }
}


/* **************************************************************** */
#ifdef OSCILLATORs
void tap_do_osc_up_taps(int tab) {

#ifdef TAP_EDIT
  int i;

  switch (edit_type) {	// only few edit_types change behavior of these UP taps:

  case EDIT_OUT_OSC_mul_div:
    osc=tap_parameter_1[tap];	// set globally

    // source input or action on destination osc?
    if (mul_div_state & MUL_DIV_INPUT_dest) {	// it is destination period input
      // destination input: act on selected oscillator

      // first deactivate analog inputs setting the same oscillators period:
      while ((i = which_inp_sets_this_output_(TYPE_osc_period, osc)) != ILLEGAL) {
	analog_IN_state[i] &= ~ACTIVE_undecided;	// deactivate

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("mul_div: deactivate input "); Serial.print(i); Serial.println("] ");
#endif

      }

      // now set the new period:
      // from other source?
      if (mul_div_state & MUL_DIV_has_source) {	// from other source

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print("mul_div: period["); Serial.print((int) osc); Serial.print("] ");
	Serial.print(" from source value ");  Serial.print(mul_div_source);
	Serial.print(" * "); Serial.print(mul); Serial.print(" / "); Serial.print(divi);
#endif

	period[osc] = mul_div_source * mul;	// I insist on * coming first
	period[osc] /= divi;			// and then /

#ifdef TAP_ED_SERIAL_DEBUG
	Serial.print(" = "); Serial.print(period[osc]);
	Serial.println("");
#endif

      return; }					// EXIT here

      // no source: act directly on selected oscillator.

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("mul_div period[" ); Serial.print((int) osc); Serial.print("] ");
      Serial.print(" from current value "); Serial.print(period[osc]);
      Serial.print(" * "); Serial.print(mul); Serial.print(" / "); Serial.print(divi);
#endif

      period[osc] *= mul;			// I insist on * coming first
      period[osc] /= divi;			// and then /

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print(" = "); Serial.print(period[osc]); Serial.println("");
#endif

      return;					// EXIT here

    } else {					// it is source period input
      mul_div_source = period[osc];
      mul_div_state = MUL_DIV_has_source;

      mul_div_state |= MUL_DIV_INPUT_dest;	// next will be destination input

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("Set mul_div source to period[" ); Serial.print((int) osc); Serial.print("] = ");
      Serial.println(mul_div_source);
#endif

      return;					// EXIT here
    }

    break;	// everybody has gone out anyway...

  case EDIT_ANALOG_select:
  case EDIT_ANALOG_destination_index:	// maybe obsolete
    // only oscillators implemented here ################
    osc = tap_parameter_1[tap];		// set globally

    set_analog_destination(inp, TYPE_osc_period, osc);
    show_on_strip(output_strip, osc, 0);

#ifdef TAP_ED_SERIAL_DEBUG
    Serial.print("Set destination of inp "); Serial.print((int) inp); Serial.print(" to ");
    Serial.println((int) analog_in2out_destination_index[inp]);
#endif

    analog_IN_last[inp] = ILLEGALinputVALUE;
    end_edit_mode();	// at least i think so...
      
    return;
    break;

    //  case EDIT_HAND_TUNE:	does default action here
    //    break;
  }
#endif

  // everything else:
  osc = tap_parameter_1[tap];		// set globally
  set_unset_regulate_this_output(TYPE_osc_period, osc);

  /* unused
  osc_flags[tap_parameter_1[tap]] ^= OSC_FLAG_SELECT;	// toggle select state of oscillator

#ifdef BIT_STRIPs
  show_on_strip(output_strip, ~oscillators_SELECTed_bits(), 0);	// show selected on LED strip
#endif
  */
} // tap_do_osc_up_taps()



void tap_do_osc_down_taps(int tab) {

#ifdef TAP_EDIT
  switch (edit_type) {	// only few edit_types change behavior of these DOWN/mute osc taps:

  case EDIT_OUT_OSC_mul_div:
    if (mul_div_state) {
    osc=tap_parameter_1[tap];	// set globally

    // destination input: reciprocal action on selected oscillator
    // from other source?
    if (mul_div_state & MUL_DIV_has_source) {	// from other source
      period[osc] = mul_div_source * divi;	// I insist on * coming first
      period[osc] /= mul;			// and then / both reciprocal

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("mul_div  period[" ); Serial.print((int) osc); Serial.print("] reciprocal ");
      Serial.print(" from source value ");  Serial.print(mul_div_source);
      Serial.print(" * "); Serial.print(divi); Serial.print(" / "); Serial.print(mul);
      Serial.print(" = "); Serial.print(period[osc]);
      Serial.println("");
#endif

      return;					// EXIT here
    }

    // no source: act directly on selected oscillator (reciprocal)
#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print("mul_div period[" ); Serial.print((int) osc); Serial.print("] reciprocally ");
      Serial.print(" from current value "); Serial.print(period[osc]);
      Serial.print(" * "); Serial.print(divi); Serial.print(" / "); Serial.print(mul);
#endif

    period[osc] *= divi;		// I insist on * coming first
    period[osc] /= mul;			// and then /

#ifdef TAP_ED_SERIAL_DEBUG
      Serial.print(" = "); Serial.print(period[osc]); Serial.println("");
#endif

    return;				// EXIT here
    }

    break;	// nobody passes here...

  }  // switch(edit_type)
#endif

  tap_do_toggle_osc_mute(tap);
}  // tap_do_osc_down_taps()


#endif // OSCILLATORs
/* **************************************************************** */



/* **************************************************************** */
// BUGGY CODE FROM THE NET	################

// #define MEMORY_INFO	// this code from the net is buggy
#ifdef MEMORY_INFO	// this code from the net is buggy
/* **************************************************************** */
/* **************************************************************** */
// found this on the net to determine free memory:

// I changed these to extern int
// extern void __bss_end;
// extern void *__brkval;

extern int __bss_end;
extern int *__brkval;

int get_free_memory()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}
/* **************************************************************** */

// this function will return the number of bytes currently free in RAM
// written by David A. Mellis
// based on code by Rob Faludi http://www.faludi.com
int availableMemory() {
  int size = 8192;	// 1024 // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}

/* **************************************************************** */
/* // dosn't seem to work #######################
Here is an alternative function:
 * This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
*/

uint8_t * heapptr, * stackptr;
void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}

/* **************************************************************** */

// 
// I would like to add a similar function posted in the forum (here):
// MemoryFree.h:
// // memoryFree header
// 
// #ifndef	MEMORY_FREE_H
// #define MEMORY_FREE_H
// 
// #ifdef __cplusplus
// extern "C" {
// #endif
// 
// int freeMemory();
// 
// #ifdef  __cplusplus
// }
// #endif
// 
// #endif
// 
// MemoryFree.cpp:
// extern unsigned int __bss_end;
// extern unsigned int __heap_start;
// extern void *__brkval;
// 
// 
// #include "MemoryFree.h";
// 
// 
// int freeMemory() {
//   int free_memory;
// 
//   if((int)__brkval == 0)
//      free_memory = ((int)&free_memory) - ((int)&__bss_end);
//   else
//     free_memory = ((int)&free_memory) - ((int)__brkval);
// 
//   return free_memory;
// }
// 
// Example sketch:
// #include <MemoryFree.h>
// 

void show_memory() {
  Serial.println("memory info looks wrong!");


  Serial.print("get free memory  = "); Serial.println(get_free_memory());
  Serial.print("available memory = "); Serial.println(availableMemory());
  Serial.print("stackptr = "); Serial.print((int) *stackptr);

  // does not seem to work ################
  Serial.print("\theapptr = "); Serial.print((int) *heapptr);
  Serial.print("\tstackptr - heapptr = "); Serial.println((int) (*stackptr - *heapptr));
}
#endif	// #ifdef MEMORY_INFO



/* **************************************************************** */
// #define DEBUG_TOOLS	// some functions to help with debugging
#ifdef DEBUG_TOOLS	// some functions to help with debugging
void wuschel()
{
  int i;

  digitalWrite(PIN13,LOW);
  for (i=0; i<4; i++) {
    delay(120) ; digitalWrite(PIN13,HIGH);
    delay(50) ; digitalWrite(PIN13,LOW);
  }

  Serial.println("wuschel");
}
#endif	// DEBUG_TOOLS



/* **************************************************************** */
/* **************************************************************** */
/* **************************************************************** */
void setup() {
  int i, osc;
  unsigned long initial_period;

#ifdef USE_SERIAL
  Serial.begin(USE_SERIAL);
#endif

#ifdef MEMORY_INFO	// this code from the net is buggy
  show_memory();
#endif

#ifdef COLOUR_LEDs
  init_colour_LEDs();
#endif


#ifdef OSCILLATORs
  // oscPIN = {47, 49, 51, 53};
  oscillatorInit();


  // ################################################################
  // 2*2*2*2 * 3*3*3 * 5*5 = 2160
  // 2*3* 2160 = 12960
  // or alternative: 7 * 2160 = 15120
  // ################################################################
  initial_period =  2UL;
  initial_period *= 2UL;
  initial_period *= 2UL;
  initial_period *= 2UL;

  initial_period *= 3UL;
  initial_period *= 3UL;
  initial_period *= 3UL;

  initial_period *= 5UL;
  initial_period *= 5UL;

  //    initial_period *= 7UL;

  for (osc=0; osc<OSCILLATORs; osc++)
    period[osc] = initial_period;


  startOscillator(0);			// on

  startOscillator(1);			// on

  startOscillator(2);
  osc_flags[2] |= OSC_FLAG_MUTE;	// muted

  startOscillator(3);
  osc_flags[3] |= OSC_FLAG_MUTE;	// muted


  set_osc_mask_pin(0, 42);		// rhythm led out
#endif


#ifdef LED_ROW
  // init_LED_row(14, LED_ROW, 0, 1);
#endif


#ifdef TAP_PINs
  init_TAPs();

#ifdef OSCILLATORs	// inside #ifdef TAP_PINs
  {
    int osc, pin, tap=0;

    // double TAP rows on double pin row for 4 oscillators

    // TAPs start with a down/mute tap for each oscillator
    // on even PIN numbers starting with pin
    pin= 22;	// ################
    //
    for (osc=0; osc<4; osc++, pin+=2, tap++) {
      //      setup_TAP(pin, 0, &tap_do_toggle_osc_mute, osc);	// "down" TAPs
      setup_TAP(pin, 0, &tap_do_osc_down_taps, osc);	// "down" TAPs
    }


    // Followed by a up/select tap for each oscillator:
    // on the uneven pins starting with pin
    pin= 23;	// ################
    for (osc=0; osc<4; osc++, pin+=2, tap++) {
      setup_TAP(pin, 0, &tap_do_osc_up_taps, osc);	// "up" TAPs
    }

#ifdef TAP_EDIT
    tap_edit_setup(33, 31, 30);
#endif

    // mute switch on 44
    setup_TAP(44, 0, &toggle_toneSwitch, tap);	// (tap is a dummy her, not used)

  }
#endif // OSCILLATORs	// inside #ifdef TAP_PINs

  #ifdef TAP_COLOUR_LED_DEBUG
  // TAP colour LED debugging, assume electrical state low to start
  pinMode(PIN13, OUTPUT); digitalWrite(PIN13, LOW);
  #endif 

#endif // TAP_PINs


#ifdef INPUTs_ANALOG
  analog_inputs_initialize();

  /* int analog_input_setup(char pin, unsigned char state, unsigned char method,  \ 
		       short in_offset, long out_offset, double i2o_scaling, \
		       unsigned char destination_type, unsigned char index) {
  */

  // accelerometer:	// analog_IN_PIN[] = {0, 1, 2}
  // for parameter tuning

  // y-acceleration	the input offset will be calibrated on each activation to cancel current acceleration
  acc_y = analog_input_setup(0, 0, (METHOD_linear | METHOD_add | METHOD_continuous), -425, 0, 0.001L, TYPE_osc_period, 0);

  // x-acceleration	the input offset will be calibrated on each activation to cancel current acceleration
  acc_x = analog_input_setup(1, 0, (METHOD_linear | METHOD_add | METHOD_continuous), -425, 0, 0.001L, TYPE_osc_period, 1);

  // z-acceleration	the input offset will be calibrated on each activation to cancel current acceleration
  acc_z = analog_input_setup(2, 0, (METHOD_linear | METHOD_add | METHOD_continuous), -612, 0, 0.001L, TYPE_osc_period, 2);


  // unused:
  //
  // unused
  analog_input_setup(3, 0, METHOD_linear, 0, 8000, 10.0, TYPE_osc_period, 0);
  //
  // unused
  analog_input_setup(4, 0, METHOD_linear, 0, 8000, 10.0, TYPE_osc_period, 0);
  //
  // unused
  analog_input_setup(5, 0, METHOD_linear, 0, 8000, 10.0, TYPE_osc_period, 0);


  // 2 piezzo UP/DOWN analog inputs
  // for hand tuning parameters
  // piezzo DOWN
  down = analog_input_setup(6, 0, (METHOD_linear | METHOD_add | METHOD_continuous), -4, 0,  0.001L, TYPE_osc_period, 0);
  //
  // piezzo UP
  up   = analog_input_setup(7, 0, (METHOD_linear | METHOD_add | METHOD_continuous), -4, 0, -0.001L, TYPE_osc_period, 0);

  // poti row, starting pin 8
  {
    int osc, pin=8;

    // 2*2*2*2 * 3*3*3 * 5*5 = 2160
    // 2*3* 2160 = 12960
    // or alternative: 7 * 2160 = 15120

    for (osc=0; osc<OSCILLATORs; osc++)
      // analog_input_setup(pin++, 1, METHOD_linear, -512, 12960, 20.0, TYPE_osc_period, osc);  // default active
      analog_input_setup(pin++, 0, METHOD_linear, -512, 12960, 20.0, TYPE_osc_period, osc);  // default off
  }
#endif	// INPUTs_ANALOG

#ifdef BIT_STRIPs
  // 4bit strip, DISPLAY only:
  pinMode(18,OUTPUT); digitalWrite(18, LOW);	// ground connection
  output_strip = init_bit_strip(17, 4, 1, ILLEGALpin);	// 4bit chain on pins 17-14, upside down

  // 4bit/5pin strip, with INPUT taps, input switch pin in the middle:
  // 4bit/5pin I/o strip from pin 3, INPUT switch on pin 5
  edit_strip = init_bit_strip(3, 4, 0, 5);
  // strip has TAPs
  // setup_TAP(char pin, unsigned char toggle, void (*do_on_tap)(int), long parameter_1) {	// set toggle to 0 or 1...
  setup_TAP(3, 0, &editTAPs_do, 1);
  setup_TAP(4, 0, &editTAPs_do, 2);
  setup_TAP(6, 0, &editTAPs_do, 4);
  setup_TAP(7, 0, &editTAPs_do, 8); pinMode(7, OUTPUT);
  switch_strip_as_TAPs(edit_strip);

#endif

#ifdef SERIAL_STRIPs
  init_out_shift(20, 19);	// data_PIN 20,  clock_PIN 19
#endif

#ifdef MENU_over_serial
  initMenu();
#endif

#ifdef MEMORY_INFO	// this code from the net is buggy
  show_memory();
#endif

  // PIN settings:
  pinMode(PIN13, OUTPUT);		// to use the onboard LED

  // golden out piezzo
  char golden_A = 43; 
  char golden_B = 45; 
  pinMode(golden_A, OUTPUT); digitalWrite(golden_A, LOW);
  pinMode(golden_B, OUTPUT); digitalWrite(golden_B, LOW);

  // temporary PIN settings:
  pinMode(PIN13, OUTPUT); digitalWrite(PIN13, LOW);	// TAP state

}

/* **************************************************************** */
void loop() {
#ifdef OSCILLATORs
  oscillate();

  outsideOscRounds++;

#ifdef MENU_over_serial

  if (char_available())
    menuOscillators();

#endif //  MENU_over_serial
#endif // OSCILLATORs


#ifdef TAP_PINs
  check_TAPs ();	// set logigal state, debounce, act
#endif


#ifdef INPUTs_ANALOG
  analog_input_cyclic_poll();
#endif


  if (debugSwitch)
    ;

}
/* **************************************************************** */

/* **************************************************************** */
/*

#define ANATAPs		2
*/
/* **************************************************************** */
#ifdef ANATAPs

unsigned char  anatap_state[ANATAPs];
unsigned short anatap_wake_up_level=[ANATAPs];	// get attention when sleeping
unsigned short anatap_ignore_level[ANATAPs];	// filter noise

unsigned short multitap_gap=360;		// ms wait for evental next tap
unsigned int   last_multitap_stopped[ANATAPs];	// now we wait for multitap_gap
unsigned char  anatap_multitap[ANATAPs];	// multitap count

unsigned long  anatap_got_on[ANATAPs], anatap_got_off[ANATAPs]	// top logic level
#endif ANATAPs
/* **************************************************************** */
