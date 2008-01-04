#ifndef OSDL_ARM7_BASE_H_
#define OSDL_ARM7_BASE_H_


/*
 * OSDL ARM7 code base centralizing all main code for that processor, notably
 * for sound management, including PCM and IMA ADPCM sounds, and MP3 music.
 *
 * User code is expected to define only one callback function:
 * 'void handleReceivedApplicationCommand( FIFOCommandID commandID, 
 *     FIFOElement element )', which can be void or better can only contain:
 * 'setError( UnexpectedApplicationCommand ) ;'
 * and, of course, the main() function, which can be inspired from the examples.
 *
 * @see testOSDLSound.arm7.c
 *
 */


#include "CeylanARM7Base.h"  // for Ceylan IPC base



/**
 * Integrating library-specific (ex: OSDL) command handler, declared here but
 * to be defined by user code, typically the OSDL library, if used.
 *
 * @see testCeylanSound.arm7.c for an example
 *
 * @note Already declared by Ceylan, defined by this OSDL ARM7 base.
 *
void handleReceivedIntegratingLibrarySpecificCommand( FIFOCommandID commandID,
	FIFOElement firstElement ) ;
 */


/* Initializes OSDL and its prerequesites. */
void initOSDL() ;


#endif // OSDL_ARM7_BASE_H_

