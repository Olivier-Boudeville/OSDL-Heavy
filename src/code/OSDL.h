#ifndef OSDL_H_
#define OSDL_H_


/*
 * This is the OSDL library, a free portable high-level open-source C++ library for
 * multimedia and game programming.
 *
 * Copyright (C) 2006 Olivier Boudeville.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * You can contact the author at olivier.boudeville@esperide.com
 *
 * More informations available at http://osdl.sourceforge.net/OSDL-latest/
 *
 */

/**
 * Main header file for OSDL Project (Object-oriented Simple DirectMedia Layer, 
 * see http://osdl.sourceforge.net).
 * 
 * Add below all include files corresponding to OSDL modules which are to be explicitly
 * exported in OSDL's library.
 * 
 * Each top-level module may include others modules as well, and so on.
 *
 */


/**
 * This symbol is defined so that applications compiled against this set of header files can know
 * which were the OSDL headers used.  
 *
 * The version number is updated accordingly when OSDL, included its headers, is installed after
 * having been built.
 *
 * Another useful version information is the one of the OSDL library linked with the application.
 * It can be retrieved thanks to a call to OSDL::GetVersion, declared in OSDLBasic.h.
 *
 * @note A OSDL-using application may be compiled against OSDL header files of a particular
 * version, and dynamically linked with a OSDL library of a different version, even though one 
 * should avoid that.
 *
 * As retro-compatibility is sought after, using OSDL header files of an older version than the
 * one of the OSDL library being actually linked should work, in theory. This is false the other
 * way round.
 *
 */
#define COMPILED_WITH_OSDL_VERSION "OSDL_HEADER_VERSION"


#include "OSDLBasicIncludes.h"
#include "OSDLVideoIncludes.h"
#include "OSDLAudioIncludes.h"
#include "OSDLEventsIncludes.h"
#include "OSDLEngineIncludes.h"


#endif // OSDL_H_
