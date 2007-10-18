#include "OSDLAudible.h"


#include "OSDLUtils.h"   // for getBackendLastError
#include "OSDLBasic.h"   // for OSDL::GetVersion

#include "Ceylan.h"      // for GetOutputFormat


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"         // for SDL_InitSubSystem
#endif // OSDL_USES_SDL

#if OSDL_USES_SDL_MIXER
#include "SDL_mixer.h"   // for Mix_OpenAudio and al
#endif // OSDL_USES_SDL_MIXER



using std::string ;

using Ceylan::Maths::Hertz ;


using namespace Ceylan::Log ;

using namespace OSDL::Audio ;



AudibleException::AudibleException( const std::string & reason ) throw():
	AudioException( reason )
{

}


	
AudibleException::~AudibleException() throw()
{

}





Audible::Audible( bool convertedInOutputFormat ) throw( AudibleException ):
	_convertedInOutputFormat( convertedInOutputFormat )
{

}



Audible::~Audible() throw()
{

}




bool Audible::convertedInOutputFormat() throw()
{

	return _convertedInOutputFormat ;
	
}



const string Audible::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Audible" ;
	
	return res ;
	
}

