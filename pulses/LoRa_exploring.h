/*
  LoRa_exploring.h
*/

#if ! defined LORA_EXPLORING_H

#define  LORA_CODE_INFO		'"'	// " info, chat send and show reception
#define  LORA_CODE_INIT		'S'	// S init default, stop and reset
#define  LORA_CODE_STOP		';'	// ; stop sequence
#define  LORA_CODE_PING		'>'	// > send ping
#define  LORA_CODE_PONG		'<'	// < pong with RSSI and snr
#define  LORA_CODE_UNTIL_END	'+'	// + repeat until hard end time
#define  LORA_CODE_FOREVER	'*'	// * endless repeating, no automatic ending
#define  LORA_CODE_KB_MACRO	'!'	// ! play KB macro in ::L

void show_LoRa_code_name(uint8_t code) {
  switch(code) {
  case LORA_CODE_PING:		// '>'
    MENU.out(F("ping "));
    break;
  case LORA_CODE_PONG:		// '<'
    MENU.out(F("pong "));
    break;
  case LORA_CODE_INFO:		// '"'
    MENU.out(F("info "));
    break;
  case LORA_CODE_KB_MACRO:	// '!'
    MENU.out(F("macro "));
    break;
  default:
    // MENU.out(F("unknown "));
    ;
  }
} // show_LoRa_code_name()


void LoRa_send_ping() {
  MENU.out(F("\nLoRa_send_ping()\t"));

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_PING);
  if (LoRa.endPacket(true)) {
    LoRa.onTxDone(onLoRaSent);
    LoRa_send_start_time = micros();
    MENU.out(F("ok\t"));
  } else {			// ERROR
    LoRa.receive();		// switch back to receive mode
    MENU.outln(F("failed"));
  }
} // LoRa_send_ping()


void LoRa_send_pong(const char* rx_quality) {
  MENU.out(F("\nLoRa_send_pong()\t"));

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_PONG);
  LoRa.write(' ');
  if(/*accepted_bytes*/ LoRa.write((const uint8_t*) rx_quality, strlen(rx_quality))) {
    if (LoRa.endPacket(true)) {
      LoRa_send_start_time = micros();
      LoRa.onTxDone(onLoRaSent);
      MENU.out(F("ok\t"));
      return;			// OK
    }
  }

  // else			// ERROR
  LoRa.receive();		// switch back to receive mode
  MENU.outln(F("failed"));
} // LoRa_send_pong()


void LoRa_code_interpreter(uint8_t code, const char* rx_quality) {	// react on received messages
  switch(code) {
  case LORA_CODE_PING:		// '>'
    LoRa_send_pong(rx_quality);
    break;
  case LORA_CODE_KB_MACRO:	// '!'  }
    extern uint8_t* LoRa_RX_buffer;
    MENU.play_KB_macro((char*) &LoRa_RX_buffer+2 /*'! '*/);
    break;
  }
} // LoRa_code_interpreter()


pulses_LoRa_conf_t LORA_conf_ok;	// fallback for failed experiments

uint16_t LORA_ok_cnt=0;

void LoRa_experiments_setup() {		// call that from setup_LoRa()
  LORA_conf_ok = pulses_LORA;		// init LORA configuration structure (only)
}

#define LORA_EXPLORING_H
#endif
