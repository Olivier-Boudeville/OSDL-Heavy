#include "OSDLAudio.h"


#include "OSDLUtils.h"          // for getBackendLastError
#include "OSDLBasic.h"   // for OSDL::GetVersion


#include "SDL.h"


#include <list>
using std::list ;

using std::string ;



using namespace Ceylan::Log ;

using namespace OSDL::Audio ;


bool AudioModule::_AudioInitialized = false ;


/* untested code follows... 
Uint32 getChunkTimeMilliseconds(Mix_Chunk *chunk)
{
    Uint32 points = 0;
    Uint32 frames = 0;
    int freq = 0;
    Uint16 fmt = 0;
    int chans = 0;
    // Chunks are converted to audio device format... 
    if (!Mix_QuerySpec(&freq, &fmt, &chans))
        return 0;  //never called Mix_OpenAudio()?!

    // bytes / samplesize == sample points 
    points = (chunk->alen / ((fmt & 0xFF) / 8));

    // sample points / channels == sample frames 
    frames = (points / chans);

    // (sample frames * 1000) / frequency == play length in ms 
    return (frames * 1000) / freq);
}

*/


/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
string AudioModule::SDLEnvironmentVariables[] = 
{
	
	"AUDIODEV",
	"SDL_AUDIODRIVER",
	"SDL_DISKAUDIOFILE",
	"SDL_DISKAUDIODELAY",
	"SDL_DSP_NOSELECT",
	"SDL_PATH_DSP"
	
} ;



AudioException::AudioException( const string & reason ) throw() :
	OSDL::Exception( reason )
{

}


AudioException::~AudioException() throw()
{

}


AudioModule::AudioModule() throw( AudioException ) :
	Ceylan::Module( 
		"OSDL Audio module",
		"This is the root audio module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"LGPL" )		
{

	send( "Initializing audio subsystem." ) ;

	if ( SDL_InitSubSystem( 
			CommonModule::UseAudio ) != CommonModule::BackendSuccess )
		throw AudioException( "AudioModule constructor : "
			"unable to initialize audio subsystem : " 
			+ Utils::getBackendLastError() ) ;

	_AudioInitialized = true ;
	
	send( "Audio subsystem initialized." ) ;
	
	dropIdentifier() ;
	
}	


AudioModule::~AudioModule() throw()
{

	send( "Stopping audio subsystem." ) ;
	
	SDL_QuitSubSystem( CommonModule::UseAudio ) ;
	
	send( "Audio subsystem stopped." ) ;
	
}


const string AudioModule::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Audio module" ;
		
	if ( level == Ceylan::low )
		return res ;
				
	res += ". " + Ceylan::Module::toString() ;	
	
	return res ;
	
}


string AudioModule::DescribeEnvironmentVariables() throw()
{

	Ceylan::Uint16 varCount = 
		sizeof( SDLEnvironmentVariables ) / sizeof (char * ) ;
		
	string result = "Examining the " + Ceylan::toString( varCount )
		+ " audio-related environment variables for SDL backend :" ;
	
	list<string> variables ;
		
	string var, value ;

	bool htmlFormat = Ceylan::TextDisplayable::GetOutputFormat() ;
	
	for ( Ceylan::Uint16 i = 0; i < varCount; i++ ) 
	{
		var = SDLEnvironmentVariables[ i ] ;
		value = Ceylan::System::getEnvironmentVariable( var ) ;
		
		if ( value.empty() )
		{
			if ( htmlFormat )
			{
				variables.push_back( "<em>" + var + "</em> is not set." ) ;
			}
			else
			{
				variables.push_back( var + " is not set." ) ;			
			}	
		}
		else
		{			
			if ( htmlFormat )
			{
				variables.push_back( "<b>" + var + "</b> set to [" 
					+ value + "]." ) ;
			}
			else
			{
				variables.push_back( var + " set to [" + value + "]." ) ;
			}	
		}	
	
	}
	
	return result + Ceylan::formatStringList( variables ) ;
	
}


bool AudioModule::IsAudioInitialized() throw()
{

	return _AudioInitialized ;
	
}

