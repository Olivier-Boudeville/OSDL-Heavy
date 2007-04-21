#ifndef OSDL_H_
#define OSDL_H_


/*
 * This is the OSDL library, a free portable high-level open-source C++ 
 * library for multimedia and game programming.
 *
 * Copyright (C) 2007 Olivier Boudeville.
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
 * Add below all include files corresponding to OSDL modules which are to 
 * be explicitly exported in OSDL's library.
 * 
 * Each top-level module may include others modules as well, and so on.
 *
 */


/**
 * A OSDL-using application or library may be compiled against OSDL 
 * header files of a particular version, and dynamically linked with 
 * a OSDL library of a different version. 
 *
 * Even if retro-compatibility is searched after, necessary API changes often
 * break backward compatibility, which can be determined thanks to interface
 * versions and ages, according to Libtool conventions.
 *
 * To know to which OSDL version a set of installed headers corresponds, 
 * the OSDLHeaderVersion.h file (generated at configure time) should be read, 
 * for the actualOSDLHeaderLibtoolVersion variable. Including only OSDL.h 
 * is enough to have everything from OSDL available. 
 *
 * The OSDLConfig.h file (generated at configure time) cannot be read for
 * version informations, as it is on purpose not installed, to avoid clashes. 
 *
 * To know to which OSDL version an installed library corresponds, 
 * its OSDL::GetVersion function should be called. It is defined in
 * trunk/src/code/basic/OSDLBasic.h, and retrieves the version 
 * information which was defined in its OSDL_LIBTOOL_VERSION when it was
 * compiled.
 *
 * On Windows, the OSDLConfig.h file in the sources is used, it includes
 * in turn OSDLConfigForWindows.h which have hardcoded version values.
 *
 * On UNIX, OSDLConfig.h is generated at configure time and overwrites the
 * default OSDLHeaderVersion.h file in the sources.
 *
 * A OSDL-using program can make use of the CHECK_OSDL_VERSIONS (defined in
 * trunk/src/code/basic/OSDLBasic.h) to check automatically whether
 * versions of library and headers match.
 *
 * It the test fails (header and library versions are not compatible), at
 * runtime the issue will be detected thanks to the macro, and a message will
 * explain the situation, ex :
 * """
 * Fatal error : OSDL is performing an emergency shutdown since an abnormal
 * situation occured. OSDL library version currently linked (Libtool version :
 * current interface number = 0, current revision number = 5, interface age = 0)
 * is not compatible with the one read from the OSDL header files used to
 * compile this application (Libtool version : current interface number = 1,
 * current revision number = 6, interface age = 0), aborting.
 * """
 *
 * @see OSDLConfig.h, OSDLConfigForWindows.h, and basic/OSDLBasic.h
 * in the common module for more informations, including the macro
 * definition. 
 *
 * @see test/basic/testOSDLBasic.cc for the macro use.
 *
 */


#include "OSDLAudioIncludes.h"
#include "OSDLBasicIncludes.h"
#include "OSDLEngineIncludes.h"
#include "OSDLEventsIncludes.h"
#include "OSDLVideoIncludes.h"

// Help the user code staying clean :
#include "OSDLIncludeCorrecter.h"

#endif // OSDL_H_

