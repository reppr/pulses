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
  show_pulses_LORA_conf();

  MENU.ln();
  MENU.out(F("'F<nnn>'=set frequency ["));
  MENU.out(pulses_LORA.frequency);
  MENU.outln(F("]"));

  MENU.outln(F("'S'=start  'E'=end  'R'=receive  'I'=idle  'L'=sleep"));
  MENU.outln(F("'G'=gain  'X'=TxPower  'P'=spreading  'T'=codingRate"));
  MENU.outln(F("'M'=preambleLen  'W'=syncWord  'B'=bandwith"));
  MENU.outln(F("'D'=register dump  '='=show config"));
  MENU.ln();

#if defined USE_LoRa_EXPLORING
  MENU.outln(F("'O'=ping other\t'C<xxx>'=send macro  'CC<xxx>=send and do"));
  MENU.outln(F("'N'=send repeated\t'N<number>T<seconds>\"<text>"));	// ################ TODO: implement "
  MENU.ln();
#endif

} // LoRa_menu_display()


bool LoRa_menu_reaction(char token) {
  uint32_t input_value;
  switch (token) {
  case '0':	// default
    // TODO: reset to default ################
  case '=':
    show_pulses_LORA_conf();		// shows (known) configuration
    break;
  case '"':
    {
     size_t buflen = MENU.cb_stored();
     if(buflen) {
       uint8_t buf[buflen];
       for(int i=0; i<buflen; i++){
	 buf[i] = MENU.drop_input_token();
       }
       MENU.out(F("LoRa sent "));
       MENU.out(LoRa_send_blob(buf, buflen));
       MENU.out('/');
       MENU.out(buflen);
       MENU.out(F(" bytes\t"));
     }
    }
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

  case 'T': case 't':
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

  case 'N': case 'n':
    {
      if(MENU.is_numeric())	// 'N<nnn>' TX_repetitions
	TX_repetitions = MENU.calculate_input(TX_repetitions);

      if(MENU.check_next('T'))	// 'NT<nnn>' time intervall
	LoRa_tx_interval_seconds = MENU.calculate_input(LoRa_tx_interval_seconds);

      MENU.out(F("send repeated "));
      MENU.out(TX_repetitions);

      MENU.out(F(" all "));
      MENU.out(LoRa_tx_interval_seconds);
      MENU.out(F("\"\t|"));
      char* rest_of_line = NULL;
      int data_len=0;
      if(data_len = MENU.cb_stored()) {
	rest_of_line = (char*) malloc(data_len + 1);
	if(rest_of_line) {
	  int i;
	  char c;
	  for(i=0; i<data_len; i++) {
	    c = *(rest_of_line + i) = MENU.drop_input_token();
	    MENU.out(c);
	  }
	  *(rest_of_line + i) = '\0';
	} else
	  MENU.malloc_error();
      }
      MENU.outln('|');
      LoRa_start_repeated_TX(TX_repetitions, LoRa_tx_interval_seconds, (uint8_t*) rest_of_line, data_len);
    }
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

#if defined USE_LoRa_EXPLORING
  case 'C': case 'c':	// 'C','c' = LoRa send macro	'CC'=send and do macro
    {
      bool do_locally=false;
      MENU.out(F("LoRa send macro "));
      if(MENU.check_next('C') || MENU.check_next('c')) {
	do_locally = true;
	MENU.out(F("and do it "));
      }

      int len=0;
      char c;
      for(;len < 128; len++) {
	if(MENU.peek(len) == EOF8)
	  break;
      }
      // _macro = "! <macro code>"	
      //      len++;	// '\0'
      len += 2;	// LORA_CODE_KB_MACRO + ' ' precedes the macro code
      char* _macro = (char*) malloc(len + /*'\0'*/ 1);
      if(_macro) {
	_macro[0] = (char) LORA_CODE_KB_MACRO;
	_macro[1] = ' ';
	int i;
	for (i=2; i<len; i++)
	  *(_macro + i) = (char) MENU.drop_input_token();
	*(_macro + i) = '\0';
	MENU.out((char*) _macro);
	MENU.tab();
	LoRa_send_blob((uint8_t*) _macro, len+1);
	if(do_locally) {
	  yield();
	  MENU.ln();
	  MENU.play_KB_macro(_macro+2);
	}
	free(_macro);
      } else
	MENU.malloc_error();
    }
    break;

  case 'O': case 'o':
    LoRa_send_ping();
    break;
#endif

  default:
    return 0;		// token not found in this menu
  }
  return 1;		// token found in this menu
} // LoRa_menu_reaction()
