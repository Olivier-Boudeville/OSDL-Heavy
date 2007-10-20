#include "OSDLJoystickCommon.h"


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"         // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


using std::string ;

using namespace OSDL::Events ;


#if OSDL_USES_SDL

#include "SDL.h"                // for SDL_HAT_*

// The eight possible directions for a hat:

extern const HatPosition OSDL::Events::Centered  = SDL_HAT_CENTERED ;
extern const HatPosition OSDL::Events::Up		 = SDL_HAT_UP ;
extern const HatPosition OSDL::Events::Right	 = SDL_HAT_RIGHT ;
extern const HatPosition OSDL::Events::Down 	 = SDL_HAT_DOWN ;
extern const HatPosition OSDL::Events::Left 	 = SDL_HAT_LEFT ;

extern const HatPosition OSDL::Events::RightUp   = SDL_HAT_RIGHTUP ;
extern const HatPosition OSDL::Events::RightDown = SDL_HAT_RIGHTDOWN ;
extern const HatPosition OSDL::Events::LeftUp	 = SDL_HAT_LEFTUP ;
extern const HatPosition OSDL::Events::LeftDown  = SDL_HAT_LEFTDOWN ;

#else // OSDL_USES_SDL

// Same values as SDL:

extern const OSDL::Events::HatPosition OSDL::Events::Centered  = 0x00 ;
extern const OSDL::Events::HatPosition OSDL::Events::Up        = 0x01 ;
extern const OSDL::Events::HatPosition OSDL::Events::Right     = 0x02 ;
extern const OSDL::Events::HatPosition OSDL::Events::Down      = 0x04 ;
extern const OSDL::Events::HatPosition OSDL::Events::Left      = 0x08 ;

extern const OSDL::Events::HatPosition OSDL::Events::RightUp   =
	(OSDL::Events::Right|OSDL::Events::Up) ;
	
extern const OSDL::Events::HatPosition OSDL::Events::RightDown =
	(OSDL::Events::Right|OSDL::Events::Down) ;
	
extern const OSDL::Events::HatPosition OSDL::Events::LeftUp    =
	(OSDL::Events::Left|OSDL::Events::Up) ;
	
extern const OSDL::Events::HatPosition OSDL::Events::LeftDown  =
	(OSDL::Events::Left|OSDL::Events::Down) ; ;

#endif // OSDL_USES_SDL





JoystickException::JoystickException( const std::string & reason ) throw():
	EventsException( "Joystick exception: " + reason ) 
{
	
}


JoystickException::~JoystickException() throw()
{

}

