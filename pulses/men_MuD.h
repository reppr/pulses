/*
  men_MuD.h
  temporary test menu UI during development
*/

#if defined USE_MIDI

//	for(int i=21; i<99; i++) {
//	  MENU.out(i); MENU.space();
//	  MENU.outln(midi_note_name(i));
//	 }
//	break;

int note;
double note_f;
double period_f;
for(int i = musicBoxConf.lowest_primary; i <= musicBoxConf.highest_primary; i++) {
  MENU.out(i);	MENU.tab();

  period_f = PULSES.pulses[i].period;
  period_f /= 4096;
  MENU.out("pf ");
  MENU.out(period_f, 6);	MENU.tab();

  note_f = period_2_midi_note(period_f);
  MENU.out("nf ");
  MENU.out(note_f, 6);	MENU.tab();

  note = (int) (note_f + 0.5);
  MENU.out(note); MENU.space();
  MENU.outln(midi_note_name(note));

  if(note<127) {
    note_on_send(0, note, 0x45);
//    Serial2.write(0x90);
//    Serial2.write(note);
//    Serial2.write(0x45);
    delay(330);

    note_off_send(0, note);
//    Serial2.write(0x90);
//    Serial2.write(note);
//    Serial2.write(0x00);
  }
 }
break;


//MENU.out(F("hertz_2_midi_note()\t"));
//MENU.out(hz); MENU.tab(); MENU.outln(midi_note);
hertz_2_midi_note(32.703);	// 24
period_2_midi_note(1e6 / 32.703);
//hertz_2_midi_note(174.61);	// 53
hertz_2_midi_note(440.0);	// 69
period_2_midi_note(1e6 / 440.0);
//hertz_2_midi_note(4186.0);	// 108
break;

MENU.outln(F("MIDI test Serial2  TX17"));
//midi_setup(39, 19);

for (int note = 0x1E; note < 0x5A; note ++) {
  //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
  //  noteOn(0x90, note, 0x45);
  MENU.outln(note);
  Serial2.write(0x90);
  Serial2.write(note);
  Serial2.write(0x45);
  delay(330);
  //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
  //  noteOn(0x90, note, 0x00);
  Serial2.write(0x90);
  Serial2.write(note);
  Serial2.write(0x00);
  delay(110);
 }
break;
#endif


MENU.out(MENU.float_input(-1.0),9);
MENU.ln();
break;

double input_f;
//char* input_string = F("1637.4835unggiftig");

char* input_string;	// = F("1637.4835");
char* end=NULL;

input_string = "1369.7254chabislätz!";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


input_string = "1369.72540ohalätz!";
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


input_string = "12.3";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();

input_string = "-2.5e-3";
MENU.outln(input_string);
input_f = strtod(input_string, &end);
MENU.out(input_f, 6);
MENU.tab();
MENU.outln(end);
//
input_f = atof(input_string);
MENU.out(input_f, 6);
MENU.ln();


input_string = "3.1415926535878323";
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
