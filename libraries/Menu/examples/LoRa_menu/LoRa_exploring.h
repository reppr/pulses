/*
  LoRa_exploring.h
*/

#if ! defined LORA_EXPLORING_H

#if defined PULSES_SYSTEM && defined USE_BATTERY_LEVEL_CONTROL
  #define LORA_DEFAULT_VOLTAGE_PIN	BATTERY_LEVEL_CONTROL_PIN	// 'V 255' does analogRead(LORA_DEFAULT_VOLTAGE_PIN)
#else
  #define LORA_DEFAULT_VOLTAGE_PIN	35				// 'V 255' does analogRead(LORA_DEFAULT_VOLTAGE_PIN)
#endif

#define  LORA_CODE_INFO		'"'	// " info, chat send and show reception
#define  LORA_CODE_INIT		'S'	// S init default, stop and reset
#define  LORA_CODE_STOP		';'	// ; stop sequence
#define  LORA_CODE_PING		'>'	// > send ping
#define  LORA_CODE_PONG		'<'	// < pong with RSSI and snr
#define  LORA_CODE_UNTIL_END	'+'	// + repeat until hard end time
#define  LORA_CODE_FOREVER	'*'	// * endless repeating, no automatic ending
#define  LORA_CODE_KB_MACRO	'!'	// ! play KB macro in ::L
#define  LORA_CODE_NO_FALLBACK	'Z'	// do LoRa_stop_fallbacks()
#define  LORA_CODE_ASK_VOLTAGE	'V'	// ask to read LORA_DEFAULT_VOLTAGE_PIN (or another pin)

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
  case LORA_CODE_NO_FALLBACK:	// 'Z'
    MENU.out(F("no fallback "));
    break;
  case LORA_CODE_ASK_VOLTAGE:	// 'V'
    MENU.out(F("ask voltage "));
    break;
  }
} // show_LoRa_code_name()


void LoRa_send_ping() {
  MENU.out(F("\nLoRa_send_ping()\t"));

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_PING);
  LoRa_send_packet();
} // LoRa_send_ping()


void LoRa_send_pong(const char* rx_quality) {
  MENU.out(F("\nLoRa_send_pong()\t"));

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_PONG);
  LoRa.write(' ');
  if(/*accepted_bytes*/ LoRa.write((const uint8_t*) rx_quality, strlen(rx_quality))) {
    if(0 == LoRa_send_packet())
      return;			// OK
  }
  LoRa.receive();		// switch back to receive mode
  MENU.outln(F("failed"));
} // LoRa_send_pong()


// fallback to a working state
pulses_LoRa_conf_t LORA_conf_ok;	// fallback data for failed experiments

uint16_t LoRa_ok_cnt=0;


ESP32Timer LoRa_fallback_timer(2);	// fallback to a known working state
uint32_t LoRa_fallback_timer_sec=60;

volatile bool LoRa_do_fallback_flag=false;
void LoRa_fallback_ISR() {
  LoRa_do_fallback_flag=true;
  LoRa_fallback_timer.stopTimer();
}

void LoRa_fallback() {
  MENU.outln(F("LoRa_fallback()"));
  extern bool /*error=*/ setup_LoRa(pulses_LoRa_conf_t* LORA_conf);
  setup_LoRa(&LORA_conf_ok);
  LoRa_ok_cnt=0;
}

void LoRa_maybe_fallback() {
  if(LoRa_ok_cnt) {
    LoRa_fallback_timer.restartTimer();
  } else {
    LoRa_fallback();
  }
  LoRa_ok_cnt=0;
};

void IRAM_ATTR noop_ISR() {	// just a hack to stop the thing!
  ;
}

void LoRa_stop_fallbacks() {
  MENU.outln(F("LoRa_stop_callback()"));
  LoRa_fallback_timer.attachInterruptInterval(LoRa_fallback_timer_sec * 1000000, &noop_ISR);
  //  LoRa_fallback_timer.stopTimer();		// FIXME: crash
  //  LoRa_fallback_timer.disableTimer();	// FIXME: crash
  //  LoRa_fallback_timer.detachInterrupt();	// FIXME: crash
  LoRa_do_fallback_flag=false;
};

void LoRa_setup_fallback() {		// call that from setup_LoRa_default()
  LORA_conf_ok = pulses_LORA;		// init LORA configuration structure (only)
  LoRa_ok_cnt=0;
  LoRa_fallback_timer.attachInterruptInterval(LoRa_fallback_timer_sec * 1000000, &LoRa_fallback_ISR);
}

void LoRa_send_no_fallback() {	// stops fallback in receiving system
  MENU.out(F("\nLoRa_send_no_fallback()\t"));

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_NO_FALLBACK);
  LoRa_send_packet();
} // LoRa_send_no_fallback()


void LoRa_send_voltage_inquiry(uint8_t pin) {
  MENU.out(F("ask voltage pin "));
  MENU.out(pin);
  MENU.tab();

  LoRa.beginPacket();
  LoRa.write(LORA_CODE_ASK_VOLTAGE);
  LoRa.print(' ');
  LoRa.print(pin);
  LoRa_send_packet();
} // LoRa_send_voltage_inquiry()

void LoRa_send_voltage(uint8_t pin) {
  if(pin == 255)
    pin = LORA_DEFAULT_VOLTAGE_PIN;

  int voltage = analogRead(pin);
  LoRa.beginPacket();
  LoRa.print(F("voltage="));
  LoRa.print(voltage);
  LoRa.print(F("\tpin "));
  LoRa.print(pin);
  LoRa_send_packet();
} // LoRa_send_voltage()


void LoRa_code_interpreter(uint8_t code, void* data, const char* rx_quality) {	// react on received messages, see: LoRa_has_received()
  switch(code) {
  case LORA_CODE_PING:		// '>'
    LoRa_send_pong(rx_quality);
    break;
  case LORA_CODE_KB_MACRO:	// '!'  }
    LoRa_setup_fallback();
    extern uint8_t* LoRa_RX_buffer;
    MENU.play_KB_macro((char*) &LoRa_RX_buffer+2 /*'! '*/);
    break;
  case LORA_CODE_NO_FALLBACK:	// 'Z'
    LoRa_stop_fallbacks();
    break;
  case LORA_CODE_ASK_VOLTAGE:	// 'V' <nn>
    LoRa_send_voltage(/*pin=*/ atoi((char*) data + 2));
    break;
  case LORA_CODE_PONG:		// '<'
  case LORA_CODE_INFO:		// '\"'
    // just display
    break;
  default:	// no known code
    return;	// done
    break;
  }
  LoRa_ok_cnt++;	// was a known code
} // LoRa_code_interpreter()

#define LORA_EXPLORING_H
#endif
