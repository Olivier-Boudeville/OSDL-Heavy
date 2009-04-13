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


#ifndef _SRC_CODE_OSDLCONFIG_H
#define _SRC_CODE_OSDLCONFIG_H 1


/*
 * This is the OSDLConfig.h version for Windows.
 *
 * On UNIX platforms, this file will be overwritten by the configure-generated
 * OSDLConfig.h
 *
 * @note Do not check-in this file if replaced by its UNIX configure-generated
 * counterpart.
 *
 */

#ifdef OSDL_RUNS_ON_WINDOWS
#include "OSDLConfigForWindows.h"
#endif // OSDL_RUNS_ON_WINDOWS

/* once: _SRC_CODE_OSDLCONFIG_H */
#endif // _SRC_CODE_OSDLCONFIG_H
