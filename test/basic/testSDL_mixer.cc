#include "OSDL.h"     // just for basic primitives such as getBackendLastError
using namespace OSDL ;

using namespace Ceylan::Log ;

#include "SDL_mixer.h"


#include <string>
using std::string ;


#define SDL_SUCCESS  0
#define SDL_ERROR   -1


/*
 * Sound directory is defined relatively to OSDL documentation tree, 
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string soundDirFromExec = "../../src/doc/web/common/sounds" ;


/*
 * Sound directory is defined relatively to OSDL documentation tree, 
 * usually this pathname relative to the install directory where this
 * test executable should lie is :
 * (to be reached from OSDL/OSDL-${OSDL_VERSION}/src/code)
 *
 */
const std::string soundDirForPlayTests = "../src/doc/web/common/sounds" ;


const std::string musicFile = "welcome-to-OSDL.ogg" ;


/*
 * This module is made to test just plain SDL_mixer : nothing here should 
 * depend on OSDL !
 *
 * It is inspired from playwave.c and playmus.c from the SDL_mixer package.
 *
 */



int main( int argc, char * argv[] )
{

	LogHolder myLog( argc, argv ) ;

	bool loop = false ;
	
	try 
	{
	
		LogPlug::info( "Testing basic SDL_mixer" ) ;
		
		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			if ( token == "--loop" )
			{
				LogPlug::info( "Loop mode selected" ) ;
				loop = true ;
				tokenEaten = true ;
			}
			
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument : " + token ) ;
			}
		
		}
		
		LogPlug::info( "Starting SDL (base, audio and video)" ) ;

    	if ( SDL_Init( SDL_INIT_AUDIO ) != SDL_SUCCESS ) 
		{
			LogPlug::fatal( "Unable to initialize SDL : " 
				+ Utils::getBackendLastError() ) ;
 			return Ceylan::ExitFailure ;
		}
		
		LogPlug::info( "SDL successfully initialized" ) ;
		
		int audio_rate = 22050 ;
		Uint16 audio_format = AUDIO_S16 ;
		
		int audio_channels = 2 ;
		
		// High buffer size indeed, latency does not matter :
		int audio_buffers = 4096 ;
		
		int audio_volume = MIX_MAX_VOLUME ;
		
		/* Open the audio device */
		if ( Mix_OpenAudio( audio_rate, audio_format, audio_channels,
			audio_buffers ) < 0 ) 
		{
			LogPlug::fatal( "Could not open audio : " 
				+ Utils::getBackendLastError() ) ;
 			return Ceylan::ExitFailure ;
		}
			
		Mix_QuerySpec( & audio_rate, & audio_format, & audio_channels ) ;
		
		LogPlug::info( "Opened audio at " + Ceylan::toString( audio_rate ) 
			+ " Hz, " + Ceylan::toString( audio_format & 0xFF ) 
			+ string( " bit " )
			+ ( (audio_channels > 2 ) ? "surround" : (audio_channels > 1 ) ?
				"stereo" : "mono" )
			+ string( ", with ") + Ceylan::toString( audio_buffers ) 
			+ " bytes audio buffer." ) ; 

		/* Set the music volume */
		Mix_VolumeMusic( audio_volume ) ;

		Ceylan::System::FileLocator soundLocator ;
		soundLocator.addPath( soundDirFromExec ) ;
		soundLocator.addPath( soundDirForPlayTests ) ;

		Mix_Music * music = Mix_LoadMUS( 
			soundLocator.find( musicFile ).c_str() ) ;
		
		if ( music == 0 ) 
		{
			LogPlug::fatal( "Could not load " + musicFile + " : " 
				+ Utils::getBackendLastError() ) ;
 			return Ceylan::ExitFailure ;
		}
		
		Ceylan::Uint32 seconds = 0 ;
		
		
		Mix_PlayMusic( music, loop ? -1 : 1 ) ;
		
		/*
		 * Necessary to wait, otherwise next test is evaluated *before* 
		 * music starts playing :
		 *
		 */
		SDL_Delay( 100 ) ;
		
		while ( Mix_PlayingMusic() ) 
		{
		
			SDL_Delay( 1000 /* milliseconds */ ) ;
			LogPlug::info( "Playing for " + Ceylan::toString( seconds ) 
				+ " second(s)." ) ;
			seconds++ ;
			
		}
		
		Mix_FreeMusic( music ) ;
		
    	LogPlug::info( "Stopping SDL_mixer" ) ;
		Mix_CloseAudio() ;
		
    	LogPlug::info( "Stopping SDL" ) ;
    	SDL_Quit() ;
    	LogPlug::info( "SDL successfully stopped." ) ;

		LogPlug::info( "End of basic SDL_mixer test." ) ;
	
 
    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}

