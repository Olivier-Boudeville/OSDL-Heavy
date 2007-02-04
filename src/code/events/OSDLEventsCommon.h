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


#include "SDL.h"             // for SDL_*
				
#include <string>


/// Encapsulated basic event, encapsulates all basic events.
typedef SDL_Event BasicEvent ;
			
				
/// Application losed or gained visibility.
typedef SDL_ActiveEvent FocusEvent ;

/// Key pressed or released.
typedef SDL_KeyboardEvent KeyboardEvent ;

/// Mouse moved.
typedef SDL_MouseMotionEvent MouseMotionEvent ;

/// Mouse button pressed or released.
typedef SDL_MouseButtonEvent MouseButtonEvent ;

/// Joystick axis moved.
typedef SDL_JoyAxisEvent JoystickAxisEvent ;
 
/// Joystick trackball moved.
typedef SDL_JoyBallEvent JoystickTrackballEvent ;

/// Joystick hat position changed.
typedef SDL_JoyHatEvent JoystickHatEvent ;

/// Joystick button pressed or released.
typedef SDL_JoyButtonEvent JoystickButtonEvent ;

/// Quit is requested.
typedef SDL_QuitEvent UserRequestedQuitEvent ;

/// A system specific window manager event has been received.
typedef SDL_SysWMEvent SystemSpecificWindowManagerEvent ;


/**
 * Window resized, application is responsible for setting a new video
 * mode with the new width and height.
 *
 */
typedef SDL_ResizeEvent WindowResizedEvent ;


/// Screen has to be redrawn.
typedef SDL_ExposeEvent ScreenExposedEvent ;
	

/// User-defined event.
typedef SDL_UserEvent UserEvent ;


// Unicode values defined in Ceylan.


/// Mother class for all event-related exceptions. 		
class OSDL_DLL EventsException: public OSDL::Exception 
{ 

	public: 
	
		explicit EventsException( const std::string & reason ) throw() :
			OSDL::Exception( "Event exception : " + reason )
		{
		
		} 
		
		
		virtual ~EventsException() throw()
		{
		
		}
		
		
} ;


#endif // OSDL_EVENTS_COMMON_H_

