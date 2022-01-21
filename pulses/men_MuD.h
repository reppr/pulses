/*
  men_MuD.h
  temporary test menu UI during development
*/

ERROR_ln("Men_MuD.h");

log_battery_level();
show_logfile();

// #if defined DEBUG_ePAPER_MORSE_FEEDBACK
//   MC_display_message("MESSAGE");
//   sleep(3000);
//
//   set_used_font(big_font_p);
//   ePaper_print_at(0,0, "ePaper_print_at()", 0);	// *do* call set_used_font() before using that
//   sleep(5000);
//   MC_print_at(0,2, "MC HERE");
//   MC_print_1line_at(4, "MC there");
// break;
// #endif // DEBUG_ePAPER_MORSE_FEEDBACK

#if defined USE_RGB_LED_STRIP
  pulses_RGB_LED_string_init();	// needed after wake up from light sleep
#endif

#if defined BATTERY_LEVEL_CONTROL_PIN
// MENU.out(F("men_MuD.h	battery level "));
// MENU.outln(analogRead(BATTERY_LEVEL_CONTROL_PIN));	// pin *may* be incorrect
// MENU.outln(BATTERY_LEVEL_CONTROL_PIN);	// pin *may* be incorrect

  MENU.ln();
  battery_conf_UI_display(false);
  MENU.ln();
  battery_conf_UI_display(true);
  MENU.ln();
#endif

#if defined STARTUP_DELAY
  MENU.out(F("STARTUP_DELAY "));
  MENU.outln(STARTUP_DELAY);
  MENU.ln();
#endif

// show_metric_cents_list();	// TODO: move to Tuning hierarchy
break;

/*void dumpSysInfo()*/ {
  esp_chip_info_t sysinfo;
  esp_chip_info(&sysinfo);
  Serial.print("Model: ");
  Serial.print((int)sysinfo.model);
  Serial.print("; Features: 0x");
  Serial.print((int)sysinfo.features, HEX);
  Serial.print("; Cores: ");
  Serial.print((int)sysinfo.cores);
  Serial.print("; Revision: r");
  Serial.println((int)sysinfo.revision);
}


{
  esp_chip_info_t sysinfo;
  MENU.out(F("cores "));
  MENU.outln((int) sysinfo.cores);
}
break;

// #include "driver/timer.h"
//
// uint64_t timer_val=0;
// esp_err_t status;
// for(int g=0; g<2; g++) {
//   MENU.out("g="); MENU.outln(g);
//   for(int i=0; i<2; i++) {
//     MENU.out("\ti="); MENU.outln(i);
//     status = timer_get_counter_value((timer_group_t) g, (timer_idx_t) i, &timer_val);
//     MENU.out("\tok? "); MENU.outln(status);
//     MENU.out("\tn="); MENU.out((int) (timer_val &0xffffffff)); MENU.tab(); MENU.out((int) ((timer_val >> 32) &0xffffffff));
//
//     MENU.ln(2);
//   }
//  }
// break;

#if defined USE_LEDC_AUDIO //	very first tests

MENU.out(F("LEDC test duty "));
#if defined try_ARDUINO_LEDC_version
  MENU.outln(F("(ARDUINO version)"));
#else
  MENU.outln(F("(ESP IDF version)"));
  esp_err_t status;
#endif

uint32_t value;

for (int i=1; i<17; i++) {
  value=(ledc_audio_max - 1) / i;
  MENU.out(value);

#if defined try_ARDUINO_LEDC_version
  //  ledcWrite(ledc_audio_channel_0, value);
  pulses_ledc_write(ledc_audio_channel_0, value);
  MENU.out(F(" read: "));
  MENU.outln(ledcRead(ledc_audio_channel_0));
#else
  // alias  pulses_ledc_write(uint8_t channel, uint32_t value) here  *with error check*
  // esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
  status = ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, value);
  MENU.out(F(" set "));
  esp_err_info(status);
  status = ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
  MENU.out(F("update "));
  esp_err_info(status);
  delay(100);

  // uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
  MENU.out(F("get: "));
  MENU.outln(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));
#endif

  MENU.ln();
  //  delay(1000);
 }

break;


// esp_err_t ledc_set_duty_and_update(ledc_mode_tspeed_mode, ledc_channel_tchannel, uint32_t duty, uint32_t hpoint)
#endif	// USE_LEDC_AUDIO very first tests

#if defined USE_MIDI
midi_all_notes_off();

double hz = MENU.float_input(0.0);
// if(hz==0.0) {
//   midi_all_notes_off();
//   break;
//  }

midi_pitch_t note;
if(hertz_2_midi_note_and_bend(hz, &note)) {
  midi_note_on_send(0, note.midi_note_i, 127);
  midi_pitch_bend_send(0, note.pitch_bend_value);
 }
break;

//	for(int i=21; i<99; i++) {
//	  MENU.out(i); MENU.space();
//	  MENU.outln(midi_note_name(i));
//	 }
//	break;

//midi_all_notes_off();	// does not work?
//break;

//while (true)
//  {
//    if(midi_available())
//      MENU.outln(midi_receive());
//  }

//	int note;
//	double note_f;
//	double period_f;
//	for(int i = musicBoxConf.lowest_primary; i <= musicBoxConf.highest_primary; i++) {
//	  MENU.out(i);	MENU.tab();
//
//	  period_f = PULSES.pulses[i].period;
//	  period_f /= 4096;
//	  MENU.out("pf ");
//	  MENU.out(period_f, 6);	MENU.tab();
//
//	  note_f = period_2_midi_note(period_f);
//	  MENU.out("nf ");
//	  MENU.out(note_f, 6);	MENU.tab();
//
//	  note = (int) (note_f + 0.5);
//	  MENU.out(note); MENU.space();
//	  MENU.outln(midi_note_name(note));
//
//	  if(note<127) {
//	    midi_note_on_send(0, note, 0x45);
//	//    Serial2.write(0x90);
//	//    Serial2.write(note);
//	//    Serial2.write(0x45);
//	    delay(330);
//
//	    midi_note_off_send(0, note);
//	//    Serial2.write(0x90);
//	//    Serial2.write(note);
//	//    Serial2.write(0x00);
//	  }
//	 }
//	break;


//	//MENU.out(F("hertz_2_midi_note()\t"));
//	//MENU.out(hz); MENU.tab(); MENU.outln(midi_note);
//	hertz_2_midi_note(32.703);	// 24
//	period_2_midi_note(1e6 / 32.703);
//	//hertz_2_midi_note(174.61);	// 53
//	hertz_2_midi_note(440.0);	// 69
//	period_2_midi_note(1e6 / 440.0);
//	//hertz_2_midi_note(4186.0);	// 108
//	break;
//
//	MENU.outln(F("MIDI test Serial2  TX17"));
//	//midi_setup(39, 19);
//
//	for (int note = 0x1E; note < 0x5A; note ++) {
//	  //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
//	  //  noteOn(0x90, note, 0x45);
//	  MENU.outln(note);
//	  Serial2.write(0x90);
//	  Serial2.write(note);
//	  Serial2.write(0x45);
//	  delay(330);
//	  //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
//	  //  noteOn(0x90, note, 0x00);
//	  Serial2.write(0x90);
//	  Serial2.write(note);
//	  Serial2.write(0x00);
//	  delay(110);
//	 }
//	break;
#endif // USE_MIDI


MENU.out(MENU.float_input(-1.0),9);
MENU.ln();
break;

double input_f;
//char* input_string = F("1637.4835unggiftig");

char* input_string;	// = F("1637.4835");
char* end=NULL;

input_string = (char*) "1369.7254chabislätz!";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


input_string = (char*) "1369.72540ohalätz!";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
MENU.outln((int) end - (int) input_string);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


input_string = (char*) "12.3";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();

input_string = (char*) "-2.5e-3";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


input_string = (char*) "3.1415926535878323";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


break;

//	for(int i=0; i<80; i++) {
//	  MENU.out(i);
//	  MENU.out(" MC_mux ");
//	  MENU.outln(uxSemaphoreGetCount(MC_mux));	// free==1	taken==0
//	  delay(100);
//	 }
//	break;


//	ERROR_ln(F("dasch aber falsch!"));
//	delay(2000);
//	ERROR_ln(F("123456789012345678901234567890"));
//	break;

#if defined HAS_ePaper && defined DEBUG_ePAPER
//	MC_print_1line_at(1, "aha!");
//	MC_display_message("row 3    message from Alibaba");
//	  break;

//ePaper_font_test();

// set_used_font(&Picopixel);
// ePaper_line_matrix();
// break;

set_used_font(&FreeSansBold9pt7b);
ePaper_line_matrix();
delay(1000);
//MENU.print_free_RAM(); MENU.ln();
MC_print_at(0,1, "MALLOC() FREE()");	// adding, no clear
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(3, "malloc() free()");
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(0, "   at the TOP");	// clearing
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
int rows = ePaper.height() / used_font_p->yAdvance;
MC_print_1line_at(rows-1, "   BOTTOM");
delay(1000);
MC_print_1line_at(rows, "BELOW");
delay(12000);

set_used_font(&FreeSansBold12pt7b);
ePaper_line_matrix();
delay(1000);
//MENU.print_free_RAM(); MENU.ln();
MC_print_at(0,1, "MALLOC() FREE()");	// adding, no clear
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(3, "malloc() free()");
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
MC_print_1line_at(0, "   at the TOP");	// clearing
//MENU.print_free_RAM(); MENU.ln();
delay(1000);
rows = ePaper.height() / used_font_p->yAdvance;
MC_print_1line_at(rows-1, "   BOTTOM");
delay(1000);
MC_print_1line_at(rows, "BELOW");
delay(12000);

break;

delay(1000);
set_used_font(&FreeMonoBold9pt7b);
//monochrome_clear();
ePaper_line_matrix();
delay(1000);
ePaper_BIG_or_multiline(2, "HALLO DADA");
delay(1000);
break;

delay(1000);
set_used_font(&FreeMonoBold9pt7b);
monochrome_clear();
delay(1000);
MC_print_at(0,0,"DADAsveryverysuperlongsuperduperduperlongLinewithout a break or something else continues vorever DADAsveryverysuperlongsuperduperduperlongLinewithout a break or something else continues vorever");
delay(1000);
break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
MC_print_at(1,2," DADA ");
delay(1000);
ePaper_print_1line_at(4, "HumbuKumbu 4 ");
delay(1000);
MC_print_1line_at(1, "LINE 1");
delay(2000);

monochrome_clear();
delay(3000);

set_used_font(&FreeMonoBold12pt7b);
ePaper_line_matrix();
ePaper_print_1line_at(4, "HumbuKumbu 4 ");
delay(4000);
break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
ePaper_print_at(3, 2, " abcd_");
ePaper_print_at(0, 0, "NULL");
ePaper_print_at(5, 4, "- - ");	// ePaper.getTextBounds() takes this too literally ;)
delay(4000);
break;

set_used_font(&FreeMonoBold12pt7b);
ePaper_line_matrix();
ePaper_print_at(16, 4, "X");	// why is @(17,4,"X") not possible?
delay(4000);
//break;

set_used_font(&FreeMonoBold9pt7b);
ePaper_line_matrix();
ePaper_print_at(21, 6, "X");
delay(4000);
break;
/*
ePaper.setCursor(50,100);
ePaper.print("%%%%%");
ePaper.display();
delay(6000);
*/

//ePaper_musicBox_parameters(); break;

//ePaper_line_testing(); break;

//ePaper_print_1line(6*128/7, " *");

//setup_LILYGO_ePaper(); break;

//set_used_font(&FreeMonoBold9pt7b);
//ePaper_line_matrix();
//delay(500);
//ePaper_print_1line(6*128/7, "HumbuKumbu 6 ");

////for(int i=0; i<8; i++) {
//for(int i=0; i<5; i++) {
//  delay(2000);
//  ePaper_print_1line(i*128/7, "->XY5");
//  MENU.out("line ");
//  MENU.outln(i);
// }

//delay(1500);
//ePaper_print_str("DADA here i am ");
//delay(500);
//ePaper.println();
//delay(100);
//ePaper_print_str("here, there&everywhere");
//delay(250);
break;
//ePaper_print_1line(7*128/7, " ."); break;
//ePaper_print_1line(7*128/7, "Y."); break;

// ePaper_show_tuning(); delay(5000);

 #if defined COMPILE_FONTTEST
  ePaper_fonttest();
 #endif

if(MENU.check_next('D')) {	// DD
  if(MENU.check_next('D'))	// DDD
    do_on_other_core(&ePaper_musicBox_parameters);	// DDD
  else
    ePaper_musicBox_parameters();	// DD
} else
  MENU.play_KB_macro(F("187 P *6 N"));	// try out MELODY mode

#else
  MENU.play_KB_macro(F("187 P *6 N"));	// try out MELODY mode
#endif // HAS_ePaper or not

//#include "SPEED-TESTS.h"

//display_icode(melody0, sizeof(melody0));

#if defined USE_ADS1115_AT_ADDR
//ads1115_first_test_A0();
//ads1115_first_test_diff_A2_A3();
#endif

//show_metric_cents_list();	// TODO: move to Tuning hierarchy

//	{
//	  MENU.out(metric_mnemonic);
//	  char s[] = {0, 0};
//	  s[0] = scale_symbol_char();
//	  MENU.outln(s);
//	}


//MENU.out(F("normalised_pitch "));	// TODO: move to Tuning hierarchy
//MENU.out(get_normalised_pitch(), 6);
//MENU.ln();


//MENU.outln((int) esp_timer_get_time());

//  show_morse_event_buffer();

    //MENU.outln(F("HARDWARE configuration"));
    //show_hardware_conf(&HARDWARE);

    //display_peer_ID_list();
    //random_RGB_string(MENU.numeric_input(4));

    // random_HSV_LED_string();

    /*
    {
      int i=0;
      strand_t * strands [STRANDCNT];
      strands[i] = &STRANDS[i];

      MENU.out("LED STRING ");
      ulong t0 = micros();
      int s = digitalLeds_drawPixels(strands, 1);
      ulong d = micros() - t0;
      MENU.out(s);
      MENU.tab();
      MENU.outln(d);

//      randomStrands(strands, i, 100, 3000);
//      scanners(strands, i, 0, 2000);
//      scanners(strands, i, 0, 2000);
//      rainbows(strands, i, 0, 4000);

    }
    */
