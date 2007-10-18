#include "OSDLSound.h"


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



SoundException::SoundException( const std::string & reason ) throw():
	AudibleException( reason )
{

}


	
SoundException::~SoundException() throw()
{

}





Sound::Sound( const std::string & soundFile, bool preload ) 
		throw( SoundException ):
	Audible( /* nothing loaded yet, hence not converted */ false ),	
	Ceylan::LoadableWithContent<LowLevelSound>( soundFile )
{

	if ( ! AudioModule::IsAudioInitialized() )
		throw SoundException( "Sound constructor failed: "
			"audio module not already initialized" ) ;
			 
#if OSDL_USES_SDL_MIXER

	if ( preload )
	{
	
		try
		{
		
			load() ;
			
		}
		catch( const LoadableException & e )
		{
		
			throw SoundException( "Sound constructor failed while preloading: "
				+ e.toString() ) ;
		}
			
	}
	
#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound constructor failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



Sound::~Sound() throw()
{

	try
	{
	
		if ( hasContent() )
			unload() ;
	
	}
	catch( const LoadableException & e )
	{
		
		LogPlug::error( "Sound destructor failed: " + e.toString() ) ;
		
	}
	
}




// LoadableWithContent template instanciation.


bool Sound::load() throw( LoadableException )
{

	if ( hasContent() )
		return false ;
		
#if OSDL_USES_SDL_MIXER

	_content = ::Mix_LoadWAV( _contentPath.c_str() ) ;
	
	if ( _content == 0 )
		throw LoadableException( "Sound::load failed: "
			+ string( ::Mix_GetError() ) ) ;
	
	_convertedInOutputFormat = true ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw LoadableException( 
		"Sound::load failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



bool Sound::unload() throw( LoadableException )
{

	if ( ! hasContent() )
		return false ;

#if OSDL_USES_SDL_MIXER

	::Mix_VolumeChunk( _content ) ;
	_content = 0 ;
	
#else // OSDL_USES_SDL_MIXER

	throw LoadableException( 
		"Sound::unload failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}




// Audible implementation.


Volume Sound::getVolume() const throw( SoundException )
{

#if OSDL_USES_SDL_MIXER
	
	try
	{
	
		if ( hasContent() )
			return ::Mix_VolumeChunk( & getExistingContent(), 
				/* just read it */ -1 ) ;
		else
			throw SoundException( 
				"Sound::getVolume failed: no sound already loaded." ) ;
				
	}
	catch( const LoadableException & e )
	{
	
		throw SoundException( "Sound::getVolume failed: " + e.toString() ) ;
		
	}
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( 
		"Sound::getVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::setVolume( Volume newVolume ) throw( SoundException )
{

#if OSDL_USES_SDL_MIXER
	
	try
	{
	
		if ( hasContent() )
			::Mix_VolumeChunk( & getExistingContent(), 
				static_cast<int>( newVolume ) ) ;
		else
			throw SoundException( 
				"Sound::setVolume failed: no sound already loaded." ) ;
				
	}
	catch( const LoadableException & e )
	{
	
		// For getExistingContent:
		throw SoundException( "Sound::setVolume failed: " + e.toString() ) ;
		
	}
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( 
		"Sound::setVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::play( PlaybackCount playCount ) throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	// Return value ignored:
	playReturnChannel( playCount ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw AudibleException( 
		"Sound::play failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}


ChannelNumber Sound::playReturnChannel( PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	int loops ;
	
	if ( playCount == 0 || playCount < -1 )
		return ;
 
 	if ( playCount == -1 )
		loops == -1 ;
	else
		loops = playCount-- ;			
		
	int number = ::Mix_PlayChannel( /* first free unreserved channel */ -1, 
		_content, loops ) ;
	
	if ( number == -1 )		
		throw AudibleException( "Sound::playReturnChannel failed: "
			+ string( ::Mix_GetError() ) ) ;
	else
		return number ;
			
#else // OSDL_USES_SDL_MIXER

	throw AudibleException( 
		"Sound::playReturnChannel failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::play( ChannelNumber mixingChannelNumber, PlaybackCount playCount )
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	int loops ;
	
	if ( playCount == 0 || playCount < -1 )
		return ;
 
 	if ( playCount == -1 )
		loops == -1 ;
	else
		loops = playCount-- ;			
		
	if ( ::Mix_PlayChannel( mixingChannelNumber, _content, loops ) == -1 )
		throw AudibleException( "Sound::play failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw AudibleException( 
		"Sound::play failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::playForAtMost( Ceylan::Millisecond maxDuration, 
	PlaybackCount playCount ) throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	int loops ;
	
	if ( playCount == 0 || playCount < -1 )
		return ;
 
 	if ( playCount == -1 )
		loops == -1 ;
	else
		loops = playCount-- ;			
		
	if ( ::Mix_PlayChannelTimed( mixingChannelNumber, _content, 
			loops, maxDuration ) == -1 )
		throw AudibleException( "Sound::playForAtMost failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw AudibleException( 
		"Sound::playForAtMost failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



const string Sound::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	try
	{
	
		if ( hasContent() )
		{
		
			Volume v = getVolume() ;
	
			return "Loaded sound whose volume is " + Ceylan::toString( v )
				+ "(" + Ceylan::toString( 100 * v / ( VolumeMax - VolumeMin ) )
				+ "%)" ;
			
		}
		else
		{
		
			return "Sound currently not loaded" ;
			
		}		
	
	}
	catch(  const Ceylan::Exception & e )
	{
	
		return "Sound::toString failed (abnormal): " + e.toString() ;
		 
	}
	
}

