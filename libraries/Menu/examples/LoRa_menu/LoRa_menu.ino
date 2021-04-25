/*
  LoRa_menu.ino
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#if ! defined EOF32
  #define EOF32		0xffffffff
#endif

/* **************************************************************** */
/* BAUDRATE for Serial:	uncomment one of the following lines:	*/
#define BAUDRATE	500000		// works fine here
//#define BAUDRATE	115200		// works fine here
//#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600		// failsafe
//#define BAUDRATE	31250		// MIDI


/* **************************************************************** */
#include <SPI.h>
#include <LoRa.h>


/* **************************************************************** */

#if ! defined MENU_OUTSTREAM2
  // see: https://stackoverflow.com/questions/11826554/standard-no-op-output-stream
  #include <iostream>
  class NullBuffer :  public std::streambuf
  {
  public:
    int overflow(int c) { return c; }
    // streambuf::overflow is the function called when the buffer has to output data to the actual destination of the stream.
    // The NullBuffer class above does nothing when overflow is called so any stream using it will not produce any output.
  };

  NullBuffer null_buffer;
  //#define MENU_OUTSTREAM2		std::ostream null_stream(&null_buffer)
  #define MENU_OUTSTREAM2	(Stream &) null_buffer
#endif

/*
  This version definines the menu INPUT routine int men_getchar();
  in the *program* not inside the Menu class.
*/
int men_getchar() {
  if (!Serial.available())	// ARDUINO
    return EOF32;

  return Serial.read();
}


#include <Menu.h>
Menu MENU(64, 1, &men_getchar, Serial, MENU_OUTSTREAM2);


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


volatile int LoRa_packet_size_received=0;	// used as a flag

void onLoRaReceive(int packetSize) {
  LoRa_packet_size_received = packetSize;
}

void LoRa_has_received(int packetSize) {	  // has received a packet
  static uint8_t buffer[64]={0};
  LoRa_packet_size_received = 0;

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


unsigned long LoRa_send_start_time=0;
unsigned long LoRa_send_duration=0;	// flag and duration

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

void onLoRaSent() {		// send callback
  LoRa_send_duration = micros() - LoRa_send_start_time;
  LoRa.receive();		// switch back to receive mode
}

void show_on_air_time() {
  MENU.out((float) LoRa_send_duration / 1000.0);
  MENU.outln(F(" ms on air"));
  LoRa_send_start_time = 0;
  LoRa_send_duration = 0;
} // show_on_air_time()


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


void setup() {
  Serial.begin(BAUDRATE);	// Start serial communication.
  while (!Serial) { ;}		// wait for Serial to open

  extern void LoRa_menu_display();
  extern bool LoRa_menu_reaction(char token);
  MENU.add_page("LoRa menu", 'L', \
		&LoRa_menu_display, &LoRa_menu_reaction, '+');

  MENU.ln();
  MENU.outln(F("LoRa menu\thttp://github.com/reppr/pulses/"));
  MENU.ln();

  setup_LoRa();

  MENU.menu_display();		// display menu at startup
  MENU.ln();
} // setup()


/*
  All you have to from your Arduino sketch loop() is to call:
  MENU.lurk_then_do();
  This will *not* block the loop.

  It will lurk on menu input, but return immediately if there's none.
  On input it will accumulate one next input byte and return until it
  will see an 'END token'.

  When seeing one of the usual line endings it takes it as 'END token' \0.

  On receiving an 'END token' it will interpret the accumulated tokens and return.
*/

void loop() {	// ARDUINO
  MENU.lurk_then_do();

  if(LoRa_packet_size_received)
    LoRa_has_received(LoRa_packet_size_received);

  if(LoRa_send_duration)
    show_on_air_time();

  // Insert your own code here.
  // Do not let your program block program flow,
  // always return to the main loop as soon as possible.
} // loop()

#include "LoRa_menu_page.h"
