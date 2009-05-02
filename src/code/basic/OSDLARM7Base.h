/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


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



/**
 * Manages the MP3 playback when requested.
 *
 */
void manageMP3Playback() ;



#endif // OSDL_ARM7_BASE_H_

