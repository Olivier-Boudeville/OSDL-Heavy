/*
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_HEADER_VERSION_H_
#define OSDL_HEADER_VERSION_H_

#include <string>


/*
 * This file is dedicated to the Windows version, as on UNIX it is overwritten
 * by the configure-time generated one.
 *
 * @note This Windows version needs OSDLHeaderVersion.cc, whereas the UNIX one
 * does not.
 *
 */


// Allowed because on Windows (hence not installed):
#include "OSDLConfig.h"  // for  OSDL_LIBTOOL_VERSION


namespace OSDL
{


	/**
	 * This is the Windows-compatible libtool version of the OSDL headers, as
	 * defined in the configure step.
	 *
	 * Allows to detect run-time mismatches between the OSDL headers a program
	 * or a library was compiled with, and the actual OSDL library it is then
	 * linked to.
	 *
	 * @note Cannot declare here:
	 * 'extern OSDL_DLL const std::string actualOSDLHeaderLibtoolVersion
	 *	= OSDL_LIBTOOL_VERSION ;' because, with Visual C++, it leads to
	 * multiple definitions for actualOSDLHeaderLibtoolVersion.
	 *
	 */
	extern OSDL_DLL const std::string actualOSDLHeaderLibtoolVersion ;

}


#endif // OSDL_HEADER_VERSION_H_
