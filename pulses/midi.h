/*
  midi.h
*/

#if ! defined MIDI_H

#include "HardwareSerial.h"

/*
  #define RXD2 16	// ESP32 *default* pins, not used
  #define TXD2 17	// ESP32 *default* pins, not used
*/

/*
  see: musicbox.h
  const double semitone = pow(2.0, (1.0 / 12.0));
  double cent = pow(2.0, (1.0 / 1200.0));		// ln: const does not work!  ????
*/

typedef struct midi_pitch_t {
  double midi_note_f;
  double pitch_bend_f;
  int16_t pitch_bend_value=0;
  uint8_t midi_note_i=0;
} midi_pitch_t;


double midi_note_2_hertz(uint8_t midi_note) {
  double note_f = (double) midi_note;
  double hertz;
  if(midi_note<128)
    hertz = pow(2.0, (note_f - 69.0) / 12.0) * 440.0;
  else
    hertz = 0.0;	// ERROR

  return hertz;
}


bool /*midi_note_OK*/ hertz_2_midi_note_and_bend(double hz, midi_pitch_t* result_note) {
  double semitone = pow(2.0, (1.0 / 12.0));
  double note_f;
  note_f = log(hz / 440.0);
  note_f /= log(semitone);
  note_f += 69.0;

  int note_i = (int) ((double) note_f + 0.5);	// int midi_note
  if(note_i >= 0  &&  note_i <= 127) {		// in range?
    result_note->midi_note_i = note_i;		// range OK
    result_note->midi_note_f = note_f;

    // pitch bend:
    double bend_factor = hz / midi_note_2_hertz(note_i);
    result_note->pitch_bend_value = (8192.0 + (4096.0*12.0 * log2(bend_factor)) + 0.5);

#if false	// alternative methods to get pitch bend value:
    MENU.out("hz "); MENU.out(hz); MENU.tab();
    MENU.out("note "); MENU.out(note_i); MENU.tab(); MENU.out("=hz "); MENU.out(midi_note_2_hertz(note_i)); MENU.ln();
    MENU.out("bend1 "); MENU.outln(result_note->pitch_bend_value);

    double detune_cents = log(hz/midi_note_2_hertz(note_i))/log(2.0) * 1200.0;
    int  pitch_bend_value = 8192.0 + (8192.0 * detune_cents / 200.0) + 0.5;
    MENU.out("detune cents "); MENU.out(detune_cents); MENU.ln();
    MENU.out("bend2 "); MENU.outln(pitch_bend_value);

    double note_frac = note_f - note_i;				// <<<< TODO: use *this* version
    pitch_bend_value = 8192.0 + (4096.0 * note_frac) + 0.5;	// <<<< TODO: use *this* version
    MENU.out("bend3 "); MENU.outln(pitch_bend_value);
#endif

    return true;			// resultat is VALID
  } // else

  return false;				// result is *NOT* valid
} // hertz_2_midi_note_and_bend()


bool /*midi_note_OK*/ period_2_midi_note(pulse_time_t period, midi_pitch_t* result_midi_note) {
  // was: hertz_2_midi_note(1e6 / period);
  return hertz_2_midi_note_and_bend(5e5 / period, result_midi_note); // was: ...(1e6 / period);  hertz counts *double* periods, so 5e5 instead 1e6
}


char* /*metric_mnemonic*/ midi_note_name(int midi_note_i) {
  extern char* metric_mnemonics[];

  if (midi_note_i >= 0  &&  midi_note_i < 128 )
    return metric_mnemonics[((midi_note_i + 3) % 12) + 1];
  else
    return "??";
}


void inline midi_send(uint8_t byte) {
  Serial2.write(byte);
}

void midi_channel_msg_send(uint8_t status, uint8_t channel) {	// no data
  midi_send(status | channel);
}

void midi_channel_msg_send(uint8_t status, uint8_t channel, uint8_t data1) {	// one data byte
  midi_send(status | channel);
  midi_send(data1);
}

void midi_channel_msg_send(uint8_t status, uint8_t channel, uint8_t data1, uint8_t data2) {	// 2 data bytes
  midi_send(status | channel);
  midi_send(data1);
  midi_send(data2);
}

void midi_note_on_send(uint8_t channel, uint8_t note, uint8_t velocity) {
  midi_channel_msg_send(0x90, channel, note, velocity);
}

void midi_note_off_send(uint8_t channel, uint8_t note) {
  midi_channel_msg_send(0x90, channel, note, 0);
}


void midi_pitch_bend_send(uint8_t channel, uint16_t bend_value) {
  midi_channel_msg_send(0xE0, channel, /*LSB*/ (bend_value & 0x7f), /*MSB*/ ((bend_value >> 7) & 0x7f));
}

bool inline midi_available() {
  return Serial2.available();
}

uint8_t midi_receive() {
  return Serial2.read();
}


void midi_all_notes_off() {
  MENU.outln(F("MIDI all notes off"));

  for(uint8_t channel=0; channel<16; channel++) {
    midi_send(176 | channel);	// channel mode message
    midi_send(120);		// cc=120: all sounds off
    midi_send(0);		// dummy

    midi_send(176 | channel);	// channel mode message
    midi_send(123);		// cc=123: all notes off
    midi_send(0);		// dummy
  }
}


void pulses_midi_note_maybe_bend_send(int pulse) {
  midi_pitch_t note;
  if(period_2_midi_note(PULSES.pulses[pulse].period, &note)) {
    PULSES.pulses[pulse].midi_note = note.midi_note_i;
    midi_note_on_send(PULSES.pulses[pulse].midi_channel, PULSES.pulses[pulse].midi_note, (0x7f*PULSES.MIDI_volume + 0.5));
#if defined MIDI_DOES_PITCH_BEND
    midi_pitch_bend_send(PULSES.pulses[pulse].midi_channel, note.pitch_bend_value);
#endif
  }
} // pulses_midi_note_maybe_bend_send()


// void midi_note_and_pitch_bend_send(int pulse) {
//   double midi_note;
//   if(period_2_midi_note(PULSES.pulses[pulse].period), &midi_note) {
//
//     int note = (int) ((double) midi_note + 0.5);
//
//   PULSES.pulses[pulse].midi_note = (uint8_t) period_2_midi_note(PULSES.pulses[pulse].period);
//   midi_note_on_send(PULSE.pulses[pulse].midi_channel, PULSES.pulses[pulse].midi_note, (0x7f*PULSES.MIDI_volume + 0.5));
//
//
// }

void MIDI_reaction() {	// TODO: implement MIDI in reaction
  uint8_t midibyte = midi_receive();
  switch(midibyte) {
  case 254:	// active sensing
    {
      static uint16_t active_sensing_cnt=0;
      if((active_sensing_cnt++ % 256) == 0)
	MENU.outln(F("MIDI active sensing"));
    }
    break;

  default:
    MENU.out(F("MIDI in "));
    MENU.outln(midi_receive());
  }
} // MIDI_reaction


void MIDI_setup(uint8_t midi_RX, uint8_t midi_TX) {
  MENU.out(F("MIDI_setup()\tRX="));
  MENU.out(midi_RX);
  MENU.out(F(" TX="));
  MENU.outln(midi_TX);

  Serial2.begin(31250, SERIAL_8N1, midi_RX, midi_TX);
  yield();
  while (! Serial2) { yield(); }			// just in case?
  while (midi_available()) { midi_receive(); yield(); }	// not helpful?
} // MIDI_setup();

#define MIDI_H
#endif
