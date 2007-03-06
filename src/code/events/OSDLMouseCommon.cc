#include "OSDLMouseCommon.h"

#include "SDL.h"             // for SDL_BUTTON_*


using std::string ;

using namespace OSDL::Events ;


// Only one mouse managed for the moment :
const MouseNumber OSDL::Events::DefaultMouse = 0 ;


// The mapping between SDL button number and actual buttons :

const MouseButtonNumber OSDL::Events::LeftButton   = SDL_BUTTON_LEFT ;
const MouseButtonNumber OSDL::Events::MiddleButton = SDL_BUTTON_MIDDLE ;
const MouseButtonNumber OSDL::Events::RightButton  = SDL_BUTTON_RIGHT ;

const MouseButtonNumber OSDL::Events::WheelUp      = SDL_BUTTON_WHEELUP ;
const MouseButtonNumber OSDL::Events::WheelDown    = SDL_BUTTON_WHEELDOWN ;



MouseException::MouseException( const std::string & reason ) throw() :
	EventsException( "Mouse exception : " + reason ) 
{
	
}


MouseException::~MouseException() throw()
{

}

