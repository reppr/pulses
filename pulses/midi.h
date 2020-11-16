/*
  midi.h
*/

#if ! defined MIDI_H

#include "HardwareSerial.h"

#define RXD2 16		// ESP32 *default* pins
#define TXD2 17		// ESP32 *default* pins


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

void note_on_send(uint8_t channel, uint8_t note, uint8_t velocity) {
  midi_channel_msg_send(0x90, channel, note, velocity);
}

void note_off_send(uint8_t channel, uint8_t note) {
  midi_channel_msg_send(0x90, channel, note, 0);
}


void MIDI_setup(uint8_t midi_RX, uint8_t midi_TX) {
  MENU.out(F("MIDI_setup()\tRX="));
  MENU.out(midi_RX);
  MENU.out(F(" TX="));
  MENU.outln(midi_TX);

  Serial2.begin(31250, SERIAL_8N1, midi_RX, midi_TX);
}

#define MIDI_H
#endif
