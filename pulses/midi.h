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

double /*midi_note*/ hertz_2_midi_note(double hz) {
  double semitone = pow(2.0, (1.0 / 12.0));
  double midi_note = log(hz / 440.0);
  midi_note /= log(semitone);
  midi_note += 69.0;

  return midi_note;
}


double /*midi_note*/ period_2_midi_note(pulse_time_t period) {
  // was: hertz_2_midi_note(1e6 / period);
  return hertz_2_midi_note(5e5 / period);	// hertz counts *double* periods, so 5e5 instead 1e6
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

bool inline midi_available() {
  return Serial2.available();
}

uint8_t midi_receive() {
  return Serial2.read();
}

void midi_all_notes_off() {
  for(uint8_t channel=0; channel<16; channel++) {
    midi_send(176 | channel);	// channel mode message
    midi_send(120);		// cc=120: all sounds off
    midi_send(0);		// dummy

    midi_send(176 | channel);	// channel mode message
    midi_send(123);		// cc=123: all notes off
    midi_send(0);		// dummy
  }
}


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
}

#define MIDI_H
#endif
