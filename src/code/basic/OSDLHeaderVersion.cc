/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLHeaderVersion.h"

// Allowed because on Windows (hence not installed) :
#include "OSDLConfig.h"  // for OSDL_LIBTOOL_VERSION

/*
 * This file exists only for the Windows build, as on
 * UNIX the OSDLHeaderVersion.h header file declares
 * and defines actualOSDLHeaderLibtoolVersion with no
 * multiple definitions when linking.
 *
 */
const std::string OSDL::actualOSDLHeaderLibtoolVersion 
	= OSDL_LIBTOOL_VERSION ;

