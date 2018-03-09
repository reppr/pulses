/*
  pulses_sanity_checks.h

  check some pp macros
*/

#include "pulses_boards.h"

#if defined USE_DACs
 #if (USE_DACs > 2)
   #error only 2 DACs supported
 #endif

 #if defined BOARD_has_DACs
  #if (USE_DACs > BOARD_HAS_DACS)
   #error looks like your board has not enough DACs
  #endif

 #else	// BOARD_has_DACs is not defined
  #error BOARD_has_DACs is not defined
 #endif

 #if (USE_DACs == 0)
  #warning USE_DACs == 0
 #endif
#endif // USE_DACs
