#include "OSDLUtils.h"

#include "SDL.h"       // for SDL_GetError



using namespace OSDL::Utils ;


using std::string ;


const string OSDL::Utils::getBackendLastError() throw()
{
	return SDL_GetError() ;
}

