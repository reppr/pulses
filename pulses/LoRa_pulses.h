/*
  LoRa_pulses.h

  uses: https://github.com/sandeepmistry/arduino-LoRa
	https://github.com/khoih-prog/ESP32TimerInterrupt
*/

#if ! defined LORA_PULSES_H

/* **************************************************************** */
#include <SPI.h>
#include <LoRa.h>

#include "ESP32TimerInterrupt.h"	// see: https://github.com/khoih-prog/ESP32TimerInterrupt

/* **************************************************************** */
#define LoRa_SCK	18
#define LoRa_MISO	19

#define LoRa_NSS	32	// was 5, 2
#define LoRa_RESET	33	// tested: 14, 27, 32, 33
#define LoRa_DIO0	37	// tested: 2, 27,  INPUTS: 35, 37, 38	(0 fails)

#define LoRa_FREQUENCY	868E6
#define LoRa_BANDWIDTH	125E3

typedef struct pulses_LoRa_conf_t {
  uint8_t version=0;
  bool exists=true;

  // pins
  uint8_t SCK=LoRa_SCK;
  uint8_t MISO=LoRa_MISO;
  uint8_t NSS=LoRa_NSS;
  uint8_t RESET=LoRa_RESET;
  uint8_t DIO0=LoRa_DIO0;

  // parameters
  uint8_t gain=0;		// 1-6, 0=AGC	default off
  uint8_t TxPower=17;		// in dB	default 17
  uint8_t spreading=7;		// 6-12		default 7
  uint8_t coding_rate4=5;	// 5-8		default 5
  uint8_t preamble_len=8;	// 6-65535	default 8
  uint8_t sync_word=0x12;	// default 0x12
  uint8_t reserved13=0;

  bool CRC_enabled=false;	// default false, no CRC
  bool invertIQ=false;		// default false, not inverted

  uint32_t bandwidth=LoRa_BANDWIDTH;	// default 125E3	// % 16
  // bandwidth supported values are `7.8E3`, `10.4E3`, `15.6E3`, `20.8E3`, `31.25E3`, `41.7E3`, `62.5E3`, `125E3`, `250E3`, and `500E3`.

  uint32_t frequency=LoRa_FREQUENCY;

} pulses_LoRa_conf_t;

pulses_LoRa_conf_t pulses_LORA ;

/*
void LORA_conf_reset() {
  pulses_LoRa_conf_t LORA_default;
  pulses_LORA = LORA_default;
}
*/

void show_pulses_LORA_conf(pulses_LoRa_conf_t* LORA_conf) {
  MENU.out(F(" pulses_LORA (size "));
  MENU.out(sizeof(pulses_LoRa_conf_t));
  MENU.out(F(" bytes)"));

  // pins
  MENU.out(F("\tMISO "));
  MENU.out(LORA_conf->MISO);
  MENU.out(F("   SCK "));
  MENU.out(LORA_conf->SCK);
  MENU.out(F("   NSS "));
  MENU.out(LORA_conf->NSS);
  MENU.out(F("   RESET "));
  MENU.out(LORA_conf->RESET);
  MENU.out(F("   DIO0 "));
  MENU.outln(LORA_conf->DIO0);
  MENU.ln();

  // parameters
  MENU.out(F("'B'=bandwidth "));
  MENU.out(LORA_conf->bandwidth);
  MENU.out(F("\t\t'F'=frequency "));
  MENU.outln(LORA_conf->frequency);
  MENU.outln(F(" bandwidth supported: 7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000"));
  MENU.ln();

  MENU.out(F("'G'=gain "));
  MENU.out(LORA_conf->gain);
  MENU.out(F("\t'X'=TxPower "));
  MENU.out(LORA_conf->TxPower);
  MENU.out(F("\t'P'=spreading "));
  MENU.out(LORA_conf->spreading);
  MENU.out(F("\t\t'T'=coding_rate4 "));
  MENU.outln(LORA_conf->coding_rate4);

  MENU.out(F("'M'=preamble "));
  MENU.out(LORA_conf->preamble_len);
  MENU.out(F("\t'W'=sync wd "));
  MENU.out(LORA_conf->sync_word);
  MENU.out(F("\t'Y'=CRC_enabled "));
  MENU.out(LORA_conf->CRC_enabled);
  MENU.out(F("\t'Q'=invertIQ "));
  MENU.outln(LORA_conf->invertIQ);
  MENU.ln();
} // show_pulses_LORA_conf()


unsigned long LoRa_send_start_time=0;
unsigned long LoRa_on_air_duration=0;		// flag and duration,  triggers show_on_air_time()

void onLoRaSent() {						// LoRa TX callback
  LoRa_on_air_duration = micros() - LoRa_send_start_time;	// triggers show_on_air_time()
  LoRa.receive();						// switch back to receive mode
}

bool /*error*/ LoRa_send_packet() {	// end a packet, remember send time, reconfigure appropriate mode
  if (LoRa.endPacket(true)) {
    LoRa_send_start_time = micros();
    LoRa.onTxDone(onLoRaSent);
    MENU.out(F("ok\t"));
    return 0;
  } else {			// ERROR
    LoRa.receive();		// switch back to receive mode
    MENU.outln(F("failed"));
    return 1;
  }
} // LoRa_send_packet()

int /*bytes_sent*/ LoRa_send_blob(uint8_t* buf, size_t buflen) {
  if(LoRa_send_start_time)
    MENU.outln(F("previous sending not treated"));

  LoRa.beginPacket();
  int accepted_bytes = LoRa.write(buf, buflen);
  if(accepted_bytes == 0) {	// ERROR
    LoRa.receive();		// switch back to receive mode
    return 0;
  }

  // else do send it:
  if(LoRa_send_packet())
    return 0;			// ERROR

  return accepted_bytes;	// OK, bytes sent
} // LoRa_send_blob()

void LoRa_send_str(char* str) {		// TODO: test ################
  if(str)
    LoRa_send_blob((uint8_t*) str, strlen(str));
} // LoRa_sent_str()

void show_on_air_time() {	// triggered by (LoRa_on_air_duration != 0)
  MENU.out((float) LoRa_on_air_duration / 1000.0);
  MENU.outln(F(" ms on air"));
  LoRa_send_start_time = 0;
  LoRa_on_air_duration = 0;
} // show_on_air_time()

volatile int LoRa_packet_size_received=0;	// also flag to trigger LoRa_has_received(LoRa_packet_size_received);

void onLoRaReceive(int packetSize) {
  LoRa_packet_size_received = packetSize;	// triggers LoRa_has_received(LoRa_packet_size_received);
}

#if defined  USE_LoRa_EXPLORING
  #include "LoRa_exploring.h"
#endif

#if ! defined LoRa_RECEIVE_BUF_SIZE
  #define LoRa_RECEIVE_BUF_SIZE	128	// TODO: quite big, just for testing (and LoRa chat...)
#endif
uint8_t LoRa_RX_buffer[LoRa_RECEIVE_BUF_SIZE] = {0};

void LoRa_has_received(int packetSize) {	// has received a packet, triggered by (LoRa_packet_size_received != 0)
  if((packetSize + 1) > LoRa_RECEIVE_BUF_SIZE) {
    MENU.out(packetSize);
    MENU.space();
    MENU.error_ln(F("LoRa rx buf too small"));
    return;
  }
  LoRa_packet_size_received = 0;

#if defined  USE_LoRa_EXPLORING
  uint8_t code = LoRa.peek();	// first byte might be a LoRa_function_code
#endif

  MENU.out(F("\nLoRa received "));
#if defined  USE_LoRa_EXPLORING
  show_LoRa_code_name(code);	// known codes display name
#endif
  MENU.out(packetSize);
  MENU.out(F(" bytes  '"));

  // read packet
  uint8_t c;
  int i;
  for (i = 0; i < packetSize; i++) {
    LoRa_RX_buffer[i] =  c = LoRa.read();
    MENU.out((char) c);
  }
  LoRa_RX_buffer[i]='\0';
  MENU.out(F("'\t"));

  char rx_quality[36];
  char* format = F("RSSI=%i  snr=%4.2f  freqErr=%i");
  snprintf(rx_quality, 36, format, LoRa.packetRssi(), LoRa.packetSnr(), LoRa.packetFrequencyError());
  MENU.outln(rx_quality);

  if(MENU.verbosity > VERBOSITY_SOME) {
    MENU.out("uint8_t  ");
    for (int i = 0; i < packetSize; i++) {
      MENU.out((uint8_t) LoRa_RX_buffer[i]);
      MENU.out(", ");
    }
    MENU.outln("");

    if(MENU.verbosity > VERBOSITY_MORE) {
      MENU.out("uint16_t ");
      uint16_t * i16_p;
      for (int i = 0; i < packetSize; i += 2) {
	i16_p = (uint16_t *) &LoRa_RX_buffer[i];
	MENU.out((uint16_t) *i16_p);
	MENU.out(", ");
      }
      MENU.ln();
    }
  } // VERBOSITY, show content data

#if defined  USE_LoRa_EXPLORING
  LoRa_code_interpreter(code, (void*) LoRa_RX_buffer, (const char*) rx_quality);
#endif
  // MENU.ln();	// *no* maybe on air time will follow (i.e. after sending pong)
} // LoRa_has_received()


// automatic repeated transmissions
ESP32Timer LoRa_timer(1);	// automatic LoRa_transmissions
uint8_t* repeated_TX_payload=NULL;
short repeated_TX_size=0;
int TX_repetitions=3;
volatile uint32_t TX_repetition_number=0;

bool /*ok=*/ LoRa_repeat_set_new_payload(uint8_t* payload, short size) {
  if(repeated_TX_payload) {
    free(repeated_TX_payload);
    repeated_TX_payload = NULL;
    repeated_TX_size=0;
  }
  if(payload == NULL)
    return true;	// ok, no payload

  if(repeated_TX_payload = (uint8_t*) malloc(size)) {
    repeated_TX_size=size;
    for(int i=0; i<size; i++) {
      repeated_TX_payload[i] = payload[i];
      MENU.out((char) repeated_TX_payload[i]);
    }
    MENU.ln();
    return true;		// ok, payload
  } else {
    MENU.error_ln(F("LoRa new payload"));
    return false;		// ERROR
  }
} // LoRa_repeat_set_new_payload()

void LoRa_send_repeated_TX() {
  if(LoRa_send_start_time) {
    MENU.outln(F("previous sending not treated"));
    LoRa_send_start_time=0;
  }

  LoRa.beginPacket();
  if(repeated_TX_payload && repeated_TX_size) {
    int accepted_bytes = LoRa.write(repeated_TX_payload, repeated_TX_size);
    if(accepted_bytes == 0) {	// ERROR
      LoRa.receive();		// switch back to receive mode
      MENU.error_ln(F("LoRa sending"));
      // TX_repetitions = 0; ################
      return;
    }
    LoRa.write('\t');
  } // payload?

  LoRa.write(TX_repetition_number);

  MENU.out(TX_repetition_number);
  MENU.out('/');
  MENU.out(TX_repetitions);
  MENU.out('\t');

  LoRa_send_packet();
} // LoRa_send_repeated_TX()

void IRAM_ATTR LoRa_repeated_TX_ISR() {
  TX_repetition_number++;
  extern void LoRa_stop_repeated_TX();
  if(TX_repetition_number > TX_repetitions)
    LoRa_stop_repeated_TX();
} // LoRa_repeated_TX_ISR()


bool /*did send*/ LoRa_maybe_repeat_TX() {
  static uint32_t TX_repetition_number_was=0;
  if(TX_repetition_number_was != TX_repetition_number) {
    if(TX_repetition_number)	// do not send *after* it was just stopped
      LoRa_send_repeated_TX();
    TX_repetition_number_was = TX_repetition_number;
    return true;
  } else
    return false;
} // LoRa_maybe_repeat_TX()

bool /*ok=*/ LoRa_start_repeated_TX(int repetitions, uint32_t interval_sec, uint8_t* blob, short size) {
  if(repetitions == 0) {
    extern void LoRa_stop_repeated_TX();
    LoRa_stop_repeated_TX();
    return false;	// dummy
  }
  MENU.outln(F("LoRa_start_repeated_TX()"));
  TX_repetitions = repetitions;
  TX_repetition_number = 0;
  if(LoRa_repeat_set_new_payload(blob, size)) {
    if(LoRa_timer.attachInterruptInterval(interval_sec * 1000000, LoRa_repeated_TX_ISR)) {	// ok?
      LoRa_timer.restartTimer();
      TX_repetition_number++;			// trigger immediate first transmission
      return true;				// OK
    }
  } //else
  MENU.error_ln(F("LoRa start repeat"));	// ERROR
  return false;
} // LoRa_start_repeated_TX()

void LoRa_stop_repeated_TX() {
  MENU.outln(F("LoRa_stop_repeated_TX()"));
  LoRa_timer.stopTimer();
  TX_repetition_number=0;
  /*
    // *NO* this is delayed until LoRa_repeat_set_new_payload()
    if(repeated_TX_payload) {
      free(repeated_TX_payload);
      repeated_TX_payload=NULL;
      repeated_TX_size=0;
    }
  */
} // LoRa_stop_repeated_TX()

void LoRa_pause_repeated_TX() {
  MENU.outln(F("LoRa_pause_repeated_TX()"));
  LoRa_timer.stopTimer();
} // LoRa_stop_repeated_TX()


bool /*did something*/ check_for_LoRa_jobs() {	// put this in the loop()
  if(LoRa_packet_size_received) {
    LoRa_has_received(LoRa_packet_size_received);
    return true;
  }
  if(LoRa_on_air_duration) {
    show_on_air_time();
    return true;
  }

  if(LoRa_do_fallback_flag) {
    LoRa_do_fallback_flag=false;
    LoRa_maybe_fallback();
    return true;
  }
  if(LoRa_maybe_repeat_TX())
    return true;

  return false;
} // check_for_LoRa_jobs()


bool LoRa_is_functional=false;
bool /*error=*/ setup_LoRa_default() {
  MENU.out(F("setup_LoRa_default();\t"));

  size_t maxlen=64;
  char txt[maxlen]={0};
  char* format =F("LoRa.setPins1(ss=%i, reset=%i, dio0=%i)\tfreq=%i");
  snprintf(txt, maxlen, format, pulses_LORA.NSS, pulses_LORA.RESET, pulses_LORA.DIO0, pulses_LORA.frequency);
  MENU.outln(txt);
  LoRa.setPins(pulses_LORA.NSS, pulses_LORA.RESET, pulses_LORA.DIO0);

  //LoRa.setSPIFrequency(pulses_LORA.frequency);

//  MENU.out((int) pulses_LORA.frequency);
//  MENU.tab();
  if (! LoRa.begin(pulses_LORA.frequency)) {
    LoRa_is_functional=false;
    MENU.outln("starting LoRa failed!\n");
    return -1;	// error
  } else
    LoRa_is_functional=true;

  // set LNA Gain for better RX sensitivity, by default AGC (Automatic Gain Control) is used and LNA gain is not used.
  // Uncomment the next line to disable the default AGC and set LNA gain, values between 1 - 6 are supported
  // LoRa.setGain(6);

  // LoRa.setTxPower(txPower);			// in dB, default 17

  // LoRa.setSpreadingFactor(spreadingFactor);	// default 7
  //	Supported values are between `6` and `12`.
  //	If a spreading factor of `6` is set, implicit header mode must be used to transmit and receive packets.

  // LoRa.setSignalBandwidth(signalBandwidth);	// `signalBandwidth` - signal bandwidth in Hz, defaults to `125E3`.
  //	supported values are `7.8E3`, `10.4E3`, `15.6E3`, `20.8E3`, `31.25E3`, `41.7E3`, `62.5E3`, `125E3`, `250E3`, and `500E3`.

  // LoRa.setCodingRate4(codingRateDenominator);	// `codingRateDenominator` - denominator of the coding rate, defaults to `5`
  //	supported values are between `5` and `8`, these correspond to coding rates of `4/5` and `4/8`.
  //	The coding rate numerator is fixed at `4`.

  // LoRa.setPreambleLength(preambleLength);	// `preambleLength` - preamble length in symbols, defaults to `8`
  //	supported values are between `6` and `65535`.

  // LoRa.setSyncWord(syncWord);	// `syncWord` - byte value to use as the sync word, defaults to `0x12`

  // enable or disable CRC usage, by default a CRC is not used.
  // LoRa.enableCrc();
  // LoRa.disableCrc();

  // enable or disable Invert the LoRa I and Q signals, by default a invertIQ is not used.
  // LoRa.enableInvertIQ();
  // LoRa.disableInvertIQ();


  // register callbacks
  LoRa.onReceive(onLoRaReceive);
  LoRa.onTxDone(onLoRaSent);

  MENU.outln(F("LoRa in receive mode\n"));
  LoRa.receive();

#if defined  USE_LoRa_EXPLORING
  LORA_conf_ok = pulses_LORA;		// init LORA configuration structure (only)
  LoRa_ok_cnt=0;
#endif

  return 0;
} // setup_LoRa_default()


bool /*error=*/ setup_LoRa(pulses_LoRa_conf_t* LORA_conf) {
  MENU.outln(F("setup_LoRa()"));
  show_pulses_LORA_conf(LORA_conf);

  if (! LoRa.begin(LORA_conf->frequency)) {
    LoRa_is_functional=false;
    MENU.outln("starting LoRa failed!\n");
    return -1;	// error
  } else
    LoRa_is_functional=true;

  LoRa.setSignalBandwidth(LORA_conf->bandwidth);
  LoRa.setGain(LORA_conf->gain);
  LoRa.setTxPower(LORA_conf->TxPower);
  LoRa.setSpreadingFactor(LORA_conf->spreading);
  LoRa.setCodingRate4(LORA_conf->coding_rate4);
  LoRa.setPreambleLength(LORA_conf->preamble_len);
  LoRa.setSyncWord(LORA_conf->sync_word);
  if(LORA_conf->CRC_enabled)
    LoRa.enableCrc();
  else
    LoRa.disableCrc();
  if(LORA_conf->invertIQ)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();

  // register callbacks
  LoRa.onReceive(onLoRaReceive);
  LoRa.onTxDone(onLoRaSent);

  pulses_LORA = *LORA_conf;	// TODO: check ################
  LoRa_ok_cnt=0;

  MENU.outln(F("LoRa in receive mode\n"));
  LoRa.receive();

  return 0;
} // setup_LoRa(&LORA_conf)


// some extended LoRa menu functionality
#if ! defined ILLEGAL16
  #define ILLEGAL16	0xffff
#endif

unsigned long LoRa_sequence_start_time=0;
unsigned long LoRa_sequence_end_time=0;

uint32_t LoRa_tx_count=0;
//uint16_t LoRa_tx_repetitions=4;		// ILLEGAL16 means forever
uint16_t LoRa_tx_to_repeat=0;		// ILLEGAL16 means forever
uint16_t LoRa_tx_interval_seconds=3;	// seconds, for automatic sequences
uint16_t LoRa_tx_hard_end_minutes=60;	// stop automatic sequence

bool LoRa_interpret_codes=true;		// ################ TODO: menu UI

//enum LoRa_function_codes	// <= 31, these codes must *not* be printable ASCII chars!
//  {
//   LoRa_code_noop='"',		// " info, chat send and show reception
//   LoRa_code_init='S',		// 0 init default, stop and reset
//   LoRa_code_stop=';',		// ; stop sequence
//   LoRa_code_ping='>',		// > send ping
//   LoRa_code_pong='<',		// < pong with RSSI and snr
//   LoRa_code_until_end='+',	// + repeat until hard end time
//   LoRa_code_forever='*',	// * endless repeating, no automatic ending
//   LoRa_code_kb_macro='!',	// ! play KB macro in :L
//   LoRa_code_max,
//  };

// bool /*is LoRa code*/ LoRa_function_interpreter(uint8_t code, void* text=NULL) {
//   //  if(code > 31 || ! LoRa_interpret_codes)	// printable ASCII is just displayed
//   //    return false;
//
//   switch(code) {
//   case LoRa_code_noop:
//     MENU.out(F("<noop>\t"));
//     break;
//
//   case LoRa_code_init:
//     MENU.out(F("<init>\t"));
//     setup_LoRa_default();
//     LoRa_tx_to_repeat=0;
//     LoRa_sequence_start_time=0L;
//     LoRa_sequence_end_time=0L;
//     break;
//
//   case LoRa_code_stop:
//     MENU.out(F("<stop>\t"));
//     LoRa_tx_to_repeat=0;
//     LoRa_sequence_start_time=0L;
//     LoRa_sequence_end_time=0L;
//     break;
//
//   case LoRa_code_ping:
//     MENU.out(F("<ping>\t"));
//     if(text)
//     // send pong
//     break;
//
//   case LoRa_code_pong:
//     MENU.out(F("<pong>\t"));
//     // just show
//     break;
//
//   case LoRa_code_until_end:
//     MENU.out(F("<cont>\t"));
//     LoRa_tx_repetitions = ILLEGAL16;	// ILLEGAL16 means forever
//     break;
//
//   case LoRa_code_forever:
//     MENU.out(F("<forever>\t"));
//     LoRa_tx_repetitions = ILLEGAL16;	// ILLEGAL16 means forever
//     LoRa_sequence_end_time=0;		// 0 means no hard end
//     break;
//
//   case LoRa_code_kb_macro:
//     MENU.out(F("<macro>\t"));
//     break;
//
//     // case LoRa_code_max:
//   default:
//     MENU.out(code);
//     MENU.error_ln(F("LoRa code"));
//     return false;
//   }
//   return true;
// }


// LoRa_send_sequence()	// RTOS
//	include last on air time

#define LORA_PULSES_H
#endif
