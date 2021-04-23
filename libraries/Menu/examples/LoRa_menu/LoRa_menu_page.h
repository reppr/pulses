/*
 * ****************************************************************
 * LoRa_menu_page.ino
 * ****************************************************************
*/


/* **************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Menu.h>
#include <SPI.h>
#include <LoRa.h>

void LoRa_menu_display() {
  MENU.ln();
  MENU.outln(F("LoRa test menu"));
  show_pulses_LORA_conf();

  MENU.ln();
  MENU.out(F("'F<nnn>'=set frequency ["));
  MENU.out(pulses_LORA.frequency);
  MENU.outln(F("]"));

  MENU.outln(F("'S'=start  'E'=end  'R'=receive  'I'=idle  'L'=sleep"));
  MENU.outln(F("'G'=gain  'X'=TxPower  'P'=spreading  'C'=codingRate  'M'=preambleLen  'W'=syncWord 'B'=bandwith"));
  MENU.outln(F("'D'=register dump  '='=show config"));

} // LoRa_menu_display()


bool LoRa_menu_reaction(char token) {
  uint32_t input_value;
  switch (token) {
  case '0':	// default
    // TODO: reset to default ################
  case '=':
    show_pulses_LORA_conf();		// shows (known) configuration
    break;

  case 'B': case 'b':
    input_value = MENU.calculate_input(pulses_LORA.bandwidth);
    switch(input_value) {
    case 7800:
    case 10400:
    case 15600:
    case 20800:
    case 31250:
    case 41700:
    case 62500:
    case 125000:
    case 250000:
    case 500000:
      pulses_LORA.bandwidth = input_value;
      LoRa.setSignalBandwidth(pulses_LORA.bandwidth);
      MENU.out(F("LoRa.setSignalBandwidth("));
      MENU.out(pulses_LORA.bandwidth);
      MENU.outln(')');
      LoRa.setSignalBandwidth(pulses_LORA.bandwidth);
      break;

    default:
      MENU.outln_invalid();
    }
    MENU.outln(F("bandwidth supported 7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000"));
    MENU.out(F("bandwidth: "));
    MENU.outln(pulses_LORA.bandwidth);
    MENU.ln();
    break;

  case 'C': case 'c':
    input_value = MENU.calculate_input(pulses_LORA.coding_rate4);
    if(input_value > 4 && input_value < 9)
      pulses_LORA.coding_rate4 = input_value;
    MENU.out(F("coding rate: 4/"));
    MENU.outln(pulses_LORA.coding_rate4);
    LoRa.setCodingRate4(pulses_LORA.coding_rate4);
    break;

  case 'D': case 'd':
    MENU.outln(F("register dump"));
    LoRa.dumpRegisters(Serial);		// ATTENTION: works on Serial
    MENU.outln(F("register dump"));
    break;

  case 'F': case 'f':
    input_value = MENU.calculate_input(pulses_LORA.frequency);
    if(input_value)
      pulses_LORA.frequency = input_value;

    if(pulses_LORA.frequency < 1000000)	// frequency was given in Mhz?
      pulses_LORA.frequency *= 1000000;
    MENU.out(F("LoRa frequency: "));
    MENU.outln(pulses_LORA.frequency);
    break;

  case 'G': case 'g':
    input_value = MENU.calculate_input(pulses_LORA.gain);
    if(input_value < 7)	// 0-6
      pulses_LORA.gain = input_value;
    MENU.out(F("gain: "));
    MENU.outln(pulses_LORA.gain);
    break;

  case 'M': case 'm':
    input_value = MENU.calculate_input(pulses_LORA.preamble_len);
    if(input_value < 30)	// TODO: check range ???
      pulses_LORA.preamble_len = input_value;
    MENU.out(F("preamble_len: "));
    MENU.outln(pulses_LORA.preamble_len);
    LoRa.setPreambleLength(pulses_LORA.preamble_len);
    break;

  case 'R': case 'r':
    MENU.outln(F("LoRa.receive();"));
    LoRa.receive();
    break;

  case 'S': case 's':
    setup_LoRa();
    break;

   case 'E': case 'e':
    MENU.outln(F("LoRa.end();"));
    LoRa.end();
    break;

  case 'I': case 'i':
    MENU.outln(F("LoRa.idle();"));
    LoRa.idle();
    break;

  case 'L': case 'l':
    MENU.outln(F("LoRa.sleep();"));
    LoRa.sleep();
    break;

  case 'P': case 'p':
    input_value = MENU.calculate_input(pulses_LORA.spreading);
    if(input_value > 5 && input_value < 13)	// TODO: check range ???
      pulses_LORA.spreading = input_value;
    MENU.out(F("spreading factor: "));
    MENU.outln(pulses_LORA.spreading);
    LoRa.setSpreadingFactor(pulses_LORA.spreading);
    break;

  case 'W': case 'w':
    input_value = MENU.calculate_input(pulses_LORA.sync_word);
    if(input_value < 30)	// TODO: check range ???
      pulses_LORA.sync_word = input_value;
    MENU.out(F("sync_word: "));
    MENU.outln(pulses_LORA.sync_word);
    LoRa.setSyncWord(pulses_LORA.sync_word);
    break;

  case 'X': case 'x':
    input_value = MENU.calculate_input(pulses_LORA.TxPower);
    if(input_value < 30)	// TODO: check range ???
      pulses_LORA.TxPower = input_value;
    MENU.out(F("TxPower: "));
    MENU.outln(pulses_LORA.TxPower);
    LoRa.setTxPower(pulses_LORA.TxPower);
    break;

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
} // LoRa_menu_reaction()
