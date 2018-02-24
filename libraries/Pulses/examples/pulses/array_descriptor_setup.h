// array_descriptor_setup.h

#define EDIT_SCALE_RAM_SIZE	31+1	// FIXME: maybe ;)
unsigned int edit_scale_RAM[EDIT_SCALE_RAM_SIZE] = {0};

REGISTER_SCALE(edit_scale_RAM);			// 0
REGISTER_SCALE(octaves);			// 1
REGISTER_SCALE(octaves_fifths);			// 2
REGISTER_SCALE(octaves_fourths_fifths);		// 3
REGISTER_SCALE(tetrachord);			// 4
REGISTER_SCALE(pentatonic_minor);		// 5
REGISTER_SCALE(european_pentatonic);		// 6
REGISTER_SCALE(mimic_japan_pentatonic);		// 7
REGISTER_SCALE(minor_scale);			// 8
REGISTER_SCALE(major_scale);			// 9
REGISTER_SCALE(scale_int);			// 10
REGISTER_SCALE(overnotes);			// 11
REGISTER_SCALE(scale_rationals);		// 12
REGISTER_SCALE(octaves_fourths);		// 13

/*
// void display_scales();
MENU.ln();
for (int i=0; SCALES[i].len ; i++)
  {
    MENU.out(i);
    MENU.space();
    if (i<10)
      MENU.space();

    MENU.out(SCALES[i].type);
    MENU.tab();

    MENU.out(SCALES[i].name);
    MENU.tab();

    MENU.out(SCALES[i].len);
    MENU.ln();
  }
*/
