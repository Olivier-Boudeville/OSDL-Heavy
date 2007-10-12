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

const HatPosition Centered  = SDL_HAT_CENTERED ;
const HatPosition Up        = SDL_HAT_UP ;
const HatPosition Right     = SDL_HAT_RIGHT ;
const HatPosition Down      = SDL_HAT_DOWN ;
const HatPosition Left      = SDL_HAT_LEFT ;
const HatPosition RightUp   = SDL_HAT_RIGHTUP ;
const HatPosition RightDown = SDL_HAT_RIGHTDOWN ;
const HatPosition LeftUp    = SDL_HAT_LEFTUP ;
const HatPosition LeftDown  = SDL_HAT_LEFTDOWN ;

#else // OSDL_USES_SDL

// Same values as SDL:

const HatPosition Centered  = 0x00 ;
const HatPosition Up        = 0x01 ;
const HatPosition Right     = 0x02 ;
const HatPosition Down      = 0x04 ;
const HatPosition Left      = 0x08 ;
const HatPosition RightUp   = (OSDL::Events::Right|OSDL::Events::Up) ;
const HatPosition RightDown = (OSDL::Events::Right|OSDL::Events::Down) ;
const HatPosition LeftUp    = (OSDL::Events::Left|OSDL::Events::Up) ; ;
const HatPosition LeftDown  = (OSDL::Events::Left|OSDL::Events::Down) ; ;

#endif // OSDL_USES_SDL





JoystickException::JoystickException( const std::string & reason ) throw():
	EventsException( "Joystick exception: " + reason ) 
{
	
}


JoystickException::~JoystickException() throw()
{

}

