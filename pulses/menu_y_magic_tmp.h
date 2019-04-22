/*
  menu_y_magic_tmp.h
  temporary development file
*/

MENU.ln();
MENU.out(F("PRESET "));
input_value = MENU.numeric_input(1);

if(input_value == 0)		// selecting zero gives *random* preset  see: random_preset()
  play_random_preset();
 else
   if(! load_preset(input_value))	// no error?
     start_musicBox();
