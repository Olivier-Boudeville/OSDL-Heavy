#ifndef OSDL_CONFIG_FOR_NINTENDO_DS
#define OSDL_CONFIG_FOR_NINTENDO_DS


/*
 * Common defines for cross-compilation to the Nintendo DS.
 *
 * The library user just has to ensure that either OSDL_RUNS_ON_ARM7 or
 * or OSDL_RUNS_ON_ARM9 is set to 1, the other one being set to 0.
 *
 * Not related in any way to autoconf.
 *
 */

 
// Auto-set the arch flags expected by libnds:
#ifdef OSDL_RUNS_ON_ARM7

	#define ARM7
	
	#define OSDL_DS_LOG(messageString)
	
#else // OSDL_RUNS_ON_ARM7

	#ifdef OSDL_RUNS_ON_ARM9

		#define ARM9
	
		// For iprintf and al:
		#include <stdio.h> 

		#define OSDL_DS_LOG(messageString) ::iprintf( "[Debug] %s\n", ((messageString).c_str()) )

	#else // OSDL_RUNS_ON_ARM9

		#error OSDLConfigForNintendoDS.h: either OSDL_RUNS_ON_ARM7 or OSDL_RUNS_ON_ARM9 must be defined.

	#endif // OSDL_RUNS_ON_ARM9

#endif // OSDL_RUNS_ON_ARM7


// For iprintf and al:
#include <stdio.h> 

// For libnds (discriminates between ARM7/ARM9):
#include "nds.h"


//�Not available on the DS:
#define OSDL_USES_SDL_GFX 0
#define OSDL_USES_SDL_TTF 0
#define OSDL_USES_SDL_IMAGE 0
#define OSDL_USES_LIBPNG 0
#define OSDL_USES_SDL 0

 
#endif // OSDL_CONFIG_FOR_NINTENDO_DS
