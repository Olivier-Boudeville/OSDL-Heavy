#include "OSDLUtils.h"

#include "Ceylan.h"                  // for explainError

#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"         // for SDL_GetError
#endif // OSDL_USES_SDL


using namespace OSDL::Utils ;


using std::string ;


const string OSDL::Utils::getBackendLastError() throw()
{

#if OSDL_USES_SDL

	return SDL_GetError() ;
	
#else // OSDL_USES_SDL

	// errno supposed used here:
	return Ceylan::System::explainError() ;

#endif // OSDL_USES_SDL

}

