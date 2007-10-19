#include "OSDLSound.h"

#include "OSDLAudio.h"               // for AudioModule


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"                     // for SDL_InitSubSystem
#endif // OSDL_USES_SDL

#if OSDL_USES_SDL_MIXER
#include "SDL_mixer.h"               // for Mix_OpenAudio and al
#endif // OSDL_USES_SDL_MIXER



using std::string ;

using Ceylan::System::Millisecond ;


using namespace Ceylan::Log ;

using namespace OSDL::Audio ;


/**
 * Implementation notes:
 *
 * Neither channel grouping nor effects used, as no need felt.
 *
 */


SoundException::SoundException( const string & reason ) throw():
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
		catch( const Ceylan::LoadableException & e )
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
	catch( const Ceylan::LoadableException & e )
	{
		
		LogPlug::error( "Sound destructor failed while unloading: " 
			+ e.toString() ) ;
		
	}
	
}




// LoadableWithContent template instanciation.


bool Sound::load() throw( Ceylan::LoadableException )
{

	if ( hasContent() )
		return false ;
		
#if OSDL_USES_SDL_MIXER

	// Misleading, supports WAVE but other formats as well:
	_content = ::Mix_LoadWAV( _contentPath.c_str() ) ;
	
	if ( _content == 0 )
		throw Ceylan::LoadableException( "Sound::load failed: "
			+ string( ::Mix_GetError() ) ) ;
	
	_convertedToOutputFormat = true ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Sound::load failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



bool Sound::unload() throw( Ceylan::LoadableException )
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:
	
#if OSDL_USES_SDL_MIXER

	::Mix_FreeChunk( _content ) ;
	_content = 0 ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
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
			return ::Mix_VolumeChunk( const_cast<LowLevelSound *>( 
				& getExistingContentAsConst() ), /* just read it */ -1 ) ;
		else
			throw SoundException( 
				"Sound::getVolume failed: no loaded sound available." ) ;
				
	}
	catch( const Ceylan::LoadableException & e )
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
				"Sound::setVolume failed: no loaded sound available." ) ;
				
	}
	catch( const Ceylan::LoadableException & e )
	{
	
		// For getExistingContent:
		throw SoundException( "Sound::setVolume failed: " + e.toString() ) ;
		
	}
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( 
		"Sound::setVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}





// Play section.



// Simple play subsection.


void Sound::play( PlaybackCount playCount ) throw( AudibleException )
{

	// Returned value ignored:
	playReturnChannel( playCount ) ;

}



ChannelNumber Sound::playReturnChannel( PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ! hasContent() )
		throw AudibleException( "Sound::playReturnChannel failed: "
			"no loaded sound available." ) ;
			
	int channelNumber = ::Mix_PlayChannel( 
		/* first free unreserved channel */ -1, _content, 
		GetLoopsForPlayCount( playCount ) ) ;
	
	if ( channelNumber == -1 )		
		throw SoundException( "Sound::playReturnChannel failed: "
			+ string( ::Mix_GetError() ) ) ;
	else
		return static_cast<ChannelNumber>( channelNumber ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( 
		"Sound::playReturnChannel failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::play( ChannelNumber mixingChannelNumber, PlaybackCount playCount )
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER

	if ( ! hasContent() )
		throw AudibleException( "Sound::play failed: "
			"no loaded sound available." ) ;
			
	if ( ::Mix_PlayChannel( mixingChannelNumber, _content, 
			GetLoopsForPlayCount( playCount ) ) == -1 )
		throw SoundException( "Sound::play failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( 
		"Sound::play failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}




// Play with time-out subsection.



void Sound::playForAtMost( Ceylan::System::Millisecond maxDuration, 
	PlaybackCount playCount ) throw( AudibleException )
{

	// Returned value ignored:
	playForAtMostReturnChannel( maxDuration, playCount ) ;

}



ChannelNumber Sound::playForAtMostReturnChannel( 
		Ceylan::System::Millisecond maxDuration, PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
		
	if ( ! hasContent() )
		throw AudibleException( "Sound::playForAtMostReturnChannel failed: "
			"no loaded sound available." ) ;

	int channelNumber = ::Mix_PlayChannelTimed( 
		/* first free unreserved channel */ -1, _content, 
		GetLoopsForPlayCount( playCount ), maxDuration ) ;
		
	if ( channelNumber == -1 )		
		throw SoundException( "Sound::playForAtMostReturnChannel failed: "
			+ string( ::Mix_GetError() ) ) ;
	else
		return static_cast<ChannelNumber>( channelNumber ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playForAtMostReturnChannel failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::playForAtMost( Ceylan::System::Millisecond maxDuration, 
		ChannelNumber mixingChannelNumber, PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
				
	if ( ! hasContent() )
		throw AudibleException( "Sound::playForAtMost failed: "
			"no loaded sound available." ) ;

	if ( ::Mix_PlayChannelTimed( mixingChannelNumber, _content,
			GetLoopsForPlayCount( playCount ), maxDuration ) == -1 )		
		throw SoundException( "Sound::playForAtMost failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playForAtMost failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}




// Play with fade-in subsection.



void Sound::playWithFadeIn( Ceylan::System::Millisecond fadeInMaxDuration,
	PlaybackCount playCount ) throw( AudibleException )
{

	// Returned value ignored:
	playWithFadeInReturnChannel( fadeInMaxDuration, playCount ) ;

	
}



ChannelNumber Sound::playWithFadeInReturnChannel( 
		Ceylan::System::Millisecond fadeInMaxDuration, PlaybackCount playCount ) 	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
		
	if ( ! hasContent() )
		throw AudibleException( "Sound::playWithFadeInReturnChannel failed: "
			"no loaded sound available." ) ;

	int channelNumber = ::Mix_FadeInChannel( 
		/* first free unreserved channel */ -1, _content, 
		GetLoopsForPlayCount( playCount ), fadeInMaxDuration ) ;
		
	if ( channelNumber == -1 )		
		throw SoundException( "Sound::playWithFadeInReturnChannel failed: "
			+ string( ::Mix_GetError() ) ) ;
	else
		return static_cast<ChannelNumber>( channelNumber ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playWithFadeInReturnChannel failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Sound::playWithFadeIn( Ceylan::System::Millisecond fadeInMaxDuration,
		ChannelNumber mixingChannelNumber, PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
		
	if ( ! hasContent() )
		throw AudibleException( "Sound::playWithFadeIn failed: "
			"no loaded sound available." ) ;

	if ( ::Mix_FadeInChannel( mixingChannelNumber, _content, 
			GetLoopsForPlayCount( playCount ), fadeInMaxDuration ) == -1 )		
		throw SoundException( "Sound::playWithFadeIn failed: "
			+ string( ::Mix_GetError() ) ) ;

#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playWithFadeIn failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

	
}




// Play with time-out and fade-in subsection.


void Sound::playWithFadeInForAtMost( 
		Ceylan::System::Millisecond playbackMaxDuration,
		Ceylan::System::Millisecond fadeInMaxDuration,
		PlaybackCount playCount ) 
	throw( AudibleException )
{

	// Returned value ignored:
	playWithFadeInForAtMostReturnChannel( playbackMaxDuration,
		fadeInMaxDuration, playCount ) ;

}

	
					
ChannelNumber Sound::playWithFadeInForAtMostReturnChannel( 
		Ceylan::System::Millisecond playbackMaxDuration,
		Ceylan::System::Millisecond fadeInMaxDuration,
		PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ! hasContent() )
		throw AudibleException( 
			"Sound::playWithFadeInForAtMostReturnChannel failed: "
			"no loaded sound available." ) ;

	int channelNumber = ::Mix_FadeInChannelTimed( 
		/* first free unreserved channel */ -1, _content, 
		GetLoopsForPlayCount( playCount ), fadeInMaxDuration,
		playbackMaxDuration ) ;
		
	if ( channelNumber == -1 )		
		throw SoundException( 
			"Sound::playWithFadeInForAtMostReturnChannel failed: "
			+ string( ::Mix_GetError() ) ) ;
	else
		return static_cast<ChannelNumber>( channelNumber ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playWithFadeInForAtMostReturnChannel failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}


					
void Sound::playWithFadeInForAtMost( 
		Ceylan::System::Millisecond playbackMaxDuration,
		Ceylan::System::Millisecond fadeInMaxDuration,
		ChannelNumber mixingChannelNumber,
		PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
		
	if ( ! hasContent() )
		throw AudibleException( "Sound::playWithFadeInForAtMost failed: "
			"no loaded sound available." ) ;
			
	if ( ::Mix_FadeInChannelTimed( mixingChannelNumber, _content, 
			GetLoopsForPlayCount( playCount ), fadeInMaxDuration,
			playbackMaxDuration ) == -1 )		
		throw SoundException( "Sound::playWithFadeInForAtMost failed: "
			+ string( ::Mix_GetError() ) ) ;

#else // OSDL_USES_SDL_MIXER

	throw SoundException( "Sound::playWithFadeInForAtMost failed: "
		"no SDL_mixer support available." ) ;
	
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
				+ "(" + Ceylan::toString( 100 * v 
					/ ( AudioModule::MaxVolume - AudioModule::MinVolume ) )
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

