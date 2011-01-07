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


#include "OSDLMouseCommon.h"


using std::string ;

using namespace OSDL::Events ;

#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_VERBOSE_JOYSTICK_HANDLER and al
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for SDL_BUTTON_* and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"                // for SDL_JoystickEventState, etc.

// The mapping between SDL button number and actual buttons:

const MouseButtonNumber OSDL::Events::LeftButton   = SDL_BUTTON_LEFT ;
const MouseButtonNumber OSDL::Events::MiddleButton = SDL_BUTTON_MIDDLE ;
const MouseButtonNumber OSDL::Events::RightButton  = SDL_BUTTON_RIGHT ;

const MouseButtonNumber OSDL::Events::WheelUp      = SDL_BUTTON_WHEELUP ;
const MouseButtonNumber OSDL::Events::WheelDown    = SDL_BUTTON_WHEELDOWN ;

#else // OSDL_USES_SDL

// Same mapping as SDL:

const MouseButtonNumber OSDL::Events::LeftButton   = 1 ;
const MouseButtonNumber OSDL::Events::MiddleButton = 2 ;
const MouseButtonNumber OSDL::Events::RightButton  = 3 ;

const MouseButtonNumber OSDL::Events::WheelUp      = 4 ;
const MouseButtonNumber OSDL::Events::WheelDown    = 5 ;

#endif // OSDL_USES_SDL


// Only one mouse managed for the moment:
const MouseNumber OSDL::Events::DefaultMouse = 0 ;




MouseException::MouseException( const std::string & reason ) :
	EventsException( "Mouse exception: " + reason ) 
{
	
}



MouseException::~MouseException() throw()
{

}

