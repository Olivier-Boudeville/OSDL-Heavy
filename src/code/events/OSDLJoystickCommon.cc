#include "OSDLJoystickCommon.h"

#include "SDL.h"             // for SDL_HAT_*


using std::string ;

using namespace OSDL::Events ;


// The height possible directions for a hat :

const HatPosition Centered  = SDL_HAT_CENTERED ;
const HatPosition Up        = SDL_HAT_UP ;
const HatPosition Right     = SDL_HAT_RIGHT ;
const HatPosition Down      = SDL_HAT_DOWN ;
const HatPosition Left      = SDL_HAT_LEFT ;
const HatPosition RightUp   = SDL_HAT_RIGHTUP ;
const HatPosition RightDown = SDL_HAT_RIGHTDOWN ;
const HatPosition LeftUp    = SDL_HAT_LEFTUP ;
const HatPosition LeftDown  = SDL_HAT_LEFTDOWN ;



JoystickException::JoystickException( const std::string & reason ) throw() :
	EventsException( "Joystick exception : " + reason ) 
{
	
}


JoystickException::~JoystickException() throw()
{

}
