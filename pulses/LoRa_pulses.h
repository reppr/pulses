/*
  LoRa_pulses.h
*/

#if ! defined LORA_PULSES_H

/* **************************************************************** */
#include <SPI.h>
#include <LoRa.h>


/* **************************************************************** */
#define LoRa_SCK	18
#define LoRa_MISO	19

#define LoRa_NSS	5	// 2
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


void show_pulses_LORA_conf() {
  MENU.out(F("pulses_LORA\t(size"));
  MENU.out(sizeof(pulses_LoRa_conf_t));
  MENU.outln(F(" bytes)"));

  MENU.out(F("bandwidth "));
  MENU.out(pulses_LORA.bandwidth);
  MENU.out(F("\tfrequency "));
  MENU.outln(pulses_LORA.frequency);

  MENU.out(F("gain "));
  MENU.out(pulses_LORA.gain);
  MENU.out(F("\t\tTxPower "));
  MENU.out(pulses_LORA.TxPower);
  MENU.out(F("\tspreading "));
  MENU.out(pulses_LORA.spreading);
  MENU.out(F("\tcoding_rate4 "));
  MENU.outln(pulses_LORA.coding_rate4);

  MENU.out(F("preamble_len "));
  MENU.out(pulses_LORA.preamble_len);
  MENU.out(F("\tsync_word "));
  MENU.out(pulses_LORA.sync_word);
  MENU.out(F("\tCRC_enabled "));
  MENU.out(pulses_LORA.CRC_enabled);
  MENU.out(F("\tinvertIQ "));
  MENU.outln(pulses_LORA.invertIQ);

  // pins
  MENU.out(F("MISO "));
  MENU.out(pulses_LORA.MISO);
  MENU.out(F("   SCK "));
  MENU.out(pulses_LORA.SCK);
  MENU.out(F("   NSS "));
  MENU.out(pulses_LORA.NSS);
  MENU.out(F("   RESET "));
  MENU.out(pulses_LORA.RESET);
  MENU.out(F("   DIO0 "));
  MENU.outln(pulses_LORA.DIO0);

  MENU.ln();
} // show_pulses_LORA_conf()


unsigned long LoRa_send_start_time=0;
unsigned long LoRa_send_duration=0;	// flag and duration

void onLoRaSent() {		// send callback
  LoRa_send_duration = micros() - LoRa_send_start_time;
  LoRa.receive();		// switch back to receive mode
}

int LoRa_send_blob(uint8_t* buf, size_t buflen) {
  if(LoRa_send_start_time)
    MENU.outln(F("previous sending not treated"));

  LoRa.beginPacket();
  int accepted_bytes = LoRa.write(buf, buflen);
  if(accepted_bytes == 0) {	// ERROR
    LoRa.receive();		// switch back to receive mode
    return 0;
  }

  // else do send it:
  LoRa.onTxDone(onLoRaSent);
  LoRa_send_start_time = micros();
  LoRa.endPacket(true);
  return accepted_bytes;
} // LoRa_send_blob()

void show_on_air_time() {
  MENU.out((float) LoRa_send_duration / 1000.0);
  MENU.outln(F(" ms on air"));
  LoRa_send_start_time = 0;
  LoRa_send_duration = 0;
} // show_on_air_time()


volatile int LoRa_packet_size_received=0;	// used as a flag

void onLoRaReceive(int packetSize) {
  LoRa_packet_size_received = packetSize;
}

#if defined  USE_LoRa_EXPLORING
  #include "LoRa_exploring.h"
#endif


void LoRa_has_received(int packetSize) {	// has received a packet
  #define LoRa_RECEIVE_BUF_SIZE	128		// TODO: quite big, just for testing (and LoRa chat...)
  static uint8_t LoRa_RX_buffer[LoRa_RECEIVE_BUF_SIZE] = {0};
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
  for (int i = 0; i < packetSize; i++) {
    LoRa_RX_buffer[i] =  c = LoRa.read();
    MENU.out((char) c);
  }
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
  LoRa_code_interpreter(code, (const char*) rx_quality);
#endif

  MENU.ln();	// ??
} // LoRa_has_received()


bool /*error=*/ setup_LoRa() {
  MENU.out(F("setup_LoRa();\t"));

  size_t maxlen=64;
  char txt[maxlen]={0};
  char* format =F("LoRa.setPins1(ss=%i, reset=%i, dio0=%i)\tfreq=%i");
  snprintf(txt, maxlen, format, pulses_LORA.NSS, pulses_LORA.RESET, pulses_LORA.DIO0, pulses_LORA.frequency);
  MENU.outln(txt);
  LoRa.setPins(pulses_LORA.NSS, pulses_LORA.RESET, pulses_LORA.DIO0);

  //LoRa.setSPIFrequency(pulses_LORA.frequency);

  MENU.out((int) pulses_LORA.frequency);
  MENU.tab();
  if (! LoRa.begin(pulses_LORA.frequency)) {
    MENU.outln("starting LoRa failed!\n");
    return -1;	// error
  }

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


  // register the receive callback
  LoRa.onReceive(onLoRaReceive);
  LoRa.onTxDone(onLoRaSent);

  MENU.outln(F("LoRa in receive mode\n"));
  LoRa.receive();

  return 0;
} // setup_LoRa()


// some extended LoRa menu functionality
#if ! defined ILLEGAL16
  #define ILLEGAL16	0xffff
#endif

unsigned long LoRa_sequence_start_time=0;
unsigned long LoRa_sequence_end_time=0;

uint32_t LoRa_tx_count=0;
uint16_t LoRa_tx_repetitions=4;		// ILLEGAL16 means forever
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
//     setup_LoRa();
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
