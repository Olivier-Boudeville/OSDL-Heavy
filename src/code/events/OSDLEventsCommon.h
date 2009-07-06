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


#ifndef OSDL_EVENTS_COMMON_H_
#define OSDL_EVENTS_COMMON_H_



/*
 * Typedef cannot be forward-declared apparently, they therefore have to 
 * be gathered separately.
 *
 * @note These are SDL-related typedefs, and there is also 
 * EventsException defined here.
 *
 */
 

#include "OSDLException.h"   // for OSDL::Exception

#include "Ceylan.h"          // for string operators
				
#include <string>

#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 
#include "SDL.h"             // for SDL events
#endif // OSDL_USES_SDL



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 


namespace OSDL
{

	namespace Events
	{
	


		/// Encapsulated basic event, encapsulates all basic events.
		typedef union SDL_Event BasicEvent ;
			
	
		/// Application losed or gained visibility.
		typedef struct SDL_ActiveEvent FocusEvent ;


		/// Key pressed or released.
		typedef struct SDL_KeyboardEvent KeyboardEvent ;


		/// Mouse moved.
		typedef struct SDL_MouseMotionEvent MouseMotionEvent ;


		/// Mouse button pressed or released.
		typedef struct SDL_MouseButtonEvent MouseButtonEvent ;


		/// Joystick axis moved.
		typedef struct SDL_JoyAxisEvent JoystickAxisEvent ;
	
	
		/// Joystick trackball moved.
		typedef struct SDL_JoyBallEvent JoystickTrackballEvent ;


		/// Joystick hat position changed.
		typedef struct SDL_JoyHatEvent JoystickHatEvent ;


		/// Joystick button pressed or released.
		typedef struct SDL_JoyButtonEvent JoystickButtonEvent ;


		/// Quit is requested.
		typedef struct SDL_QuitEvent UserRequestedQuitEvent ;


		/// A system specific window manager event has been received.
		typedef struct SDL_SysWMEvent SystemSpecificWindowManagerEvent ;



		/**
		 * Window resized, application is responsible for setting a new video
		 * mode with the new width and height.
		 *
		 */
		typedef struct SDL_ResizeEvent WindowResizedEvent ;



		/// Screen has to be redrawn.
		typedef struct SDL_ExposeEvent ScreenExposedEvent ;
	
	

		/// User-defined event.
		typedef struct SDL_UserEvent UserEvent ;



		#else // OSDL_USES_SDL


		/// Encapsulated basic event, encapsulates all basic events.
		typedef union BasicEvent {} ;
	
	
		/// Application losed or gained visibility.
		typedef struct FocusEvent {} ;


		/// Key pressed or released.
		typedef struct KeyboardEvent {} ;


		/// Mouse moved.
		typedef struct MouseMotionEvent {} ;


		/// Mouse button pressed or released.
		typedef struct MouseButtonEvent {} ;


		/// Joystick axis moved.
		typedef struct JoystickAxisEvent {} ;
	
	
		/// Joystick trackball moved.
		typedef struct JoystickTrackballEvent {} ;


		/// Joystick hat position changed.
		typedef struct JoystickHatEvent {} ;


		/// Joystick button pressed or released.
		typedef struct JoystickButtonEvent {} ;


		/// Quit is requested.
		typedef struct UserRequestedQuitEvent {} ;


		/// A system specific window manager event has been received.
		typedef struct SystemSpecificWindowManagerEvent {} ;



		/**
		 * Window resized, application is responsible for setting a new video
		 * mode with the new width and height.
		 *
		 */
		typedef struct WindowResizedEvent {} ;



		/// Screen has to be redrawn.
		typedef struct ScreenExposedEvent {} ;
	
	

		/// User-defined event.
		typedef struct UserEvent {} ;


		#endif // OSDL_USES_SDL




		// Unicode values defined in Ceylan.



		/// Mother class for all event-related exceptions.
		class OSDL_DLL EventsException: public OSDL::Exception
		{

			public:
	
				explicit EventsException( const std::string & reason ) :
					OSDL::Exception( "Event exception: " + reason )
				{
	
				}
	
	
				virtual ~EventsException() throw()
				{
	
				}
	
	
		} ;


	}
	
	
}


#endif // OSDL_EVENTS_COMMON_H_

