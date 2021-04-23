/*
  LoRa_pulses.h
*/

#include <SPI.h>
#include <LoRa.h>


#if ! defined LoRa_PULSES_H

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
  MENU.out(F("pulses_LORA\t"));
  MENU.out(sizeof(pulses_LoRa_conf_t));
  MENU.outln(F(" bytes"));

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


volatile int LoRa_packet_size_received=0;	// used as a flag

void onLoRaReceive(int packetSize) {
  LoRa_packet_size_received = packetSize;
  MENU.outln(F("onLoRaReceive"));
}

void LoRa_has_received(int packetSize) {	// TODO: just for first tests ################
  // received a packet
  static uint8_t buffer[64]={0};

  MENU.out("LoRa received ");
  MENU.out(packetSize);
  MENU.out(" bytes  '");

  // read packet
  uint8_t c;
  for (int i = 0; i < packetSize; i++) {
    buffer[i] =  c = LoRa.read();
    MENU.out((char) c);
  }

  // print RSSI of packet
  MENU.out("'  RSSI ");
  MENU.out(LoRa.packetRssi());

  float snr = LoRa.packetSnr();
  MENU.out("\tsnr ");
  MENU.out(snr);

  long freqErr = LoRa.packetFrequencyError();
  MENU.out("\tfreqErr ");
  MENU.outln(freqErr);

  MENU.out("uint8_t  ");
  for (int i = 0; i < packetSize; i++) {
    MENU.out((uint8_t) buffer[i]);
    MENU.out(", ");
  }
  MENU.outln("");

  MENU.out("uint16_t ");
  uint16_t * i16_p;
  for (int i = 0; i < packetSize; i += 2) {
    i16_p = (uint16_t *) &buffer[i];
    MENU.out((uint16_t) *i16_p);
    MENU.out(", ");
  }
  MENU.outln("\n");
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

  MENU.outln(F("LoRa in receive mode\n"));
  LoRa.receive();

  return 0;
} // setup_LoRa()


#define LoRa_PULSES_H
#endif
