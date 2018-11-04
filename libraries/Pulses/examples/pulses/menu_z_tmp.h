/*
  menu_z_tmp.h

  temporary test code for pulses menu 'z'
*/

#if defined MAGICAL_MUSIC_BOX	// TODO: move to magic stuff ################
    MENU.out(F("autoMagic end "));
    if(magic_autochanges = !magic_autochanges)
      MENU.out(F("ON\t"));
    else
      MENU.out(F("OFF\t"));
    MENU.outln(MAGICAL_PERFORMACE_SECONDS);
#endif
