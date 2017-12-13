#ifndef RAM_IS_SCARE	// enough RAM?

  const char * jiffle_names[] = {
      "jiffle_RAM",	// 0
      "gling128",	// 1
      "jiffletab",	// 2
      "jiffletab_december",	// 3
      "jiffletab_december128",	// 4
      "jiffletab_december_pizzicato", // 5
      "jiffletab01",	// 6
      "jiffletab01",	// 7 ==6 ??? FIXME:
      "jiffletab02",	// 8
      "jiffletab03",	// 9
      "jiffletab04",	// 10
      "jiffletab05",	// 11
      "jiffletab06",	// 12
      "jiffletab06",	// 13 ==12 ??? FIXME:
      "gling128_0",	// 14
      "gling128_1",	// 15
      "gling128_2",	// 16
      "harmonics4",	// 17
      "ting1024",	// 18
      "ting4096",	// 19
      "arpeggio4096",	// 20
      "arpeggio4096down", // 21
      "arpeggio_cont",	// 22
      "arpeggio_and_down", // 23
      "stepping_down",	// 24
      "back_to_ground",	// 25
      "arpeggio_and_sayling", // 26
      "simple_theme",	// 27
      "pentatonic_rising", // 28
      "(invalid)",	// 29
  };

  #define n_jiffle_names (sizeof (jiffle_names) / sizeof (const char *))

  //      "// jiffle = mimic_japan_pentatonic",	// was 21
#endif

// jiffletabs *MUST* have 2 trailing zeros
