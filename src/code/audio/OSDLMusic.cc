#include "OSDLMusic.h"

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
 * Mix_SetMusicCMD, Mix_HookMusic, Mix_GetMusicHookData not used, as no 
 * need felt.
 *
 */


MusicException::MusicException( const string & reason ) throw():
	AudibleException( reason )
{

}


	
MusicException::~MusicException() throw()
{

}





Music::Music( const std::string & musicFile, bool preload ) 
		throw( MusicException ):
	Audible( /* nothing loaded yet, hence not converted */ false ),	
	Ceylan::LoadableWithContent<LowLevelMusic>( musicFile )
{

	if ( ! AudioModule::IsAudioInitialized() )
		throw MusicException( "Music constructor failed: "
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
		
			throw MusicException( "Music constructor failed while preloading: "
				+ e.toString() ) ;
		}
			
	}
	
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music constructor failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



Music::~Music() throw()
{

	try
	{
	
		if ( hasContent() )
			unload() ;
	
	}
	catch( const Ceylan::LoadableException & e )
	{
		
		LogPlug::error( "Music destructor failed while unloading: " 
			+ e.toString() ) ;
		
	}
	
}




// LoadableWithContent template instanciation.


bool Music::load() throw( Ceylan::LoadableException )
{

	if ( hasContent() )
		return false ;
		
#if OSDL_USES_SDL_MIXER

	_content = ::Mix_LoadMUS( _contentPath.c_str() ) ;
	
	if ( _content == 0 )
		throw Ceylan::LoadableException( "Music::load failed: "
			+ string( ::Mix_GetError() ) ) ;
	
	_convertedToOutputFormat = true ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Music::load failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



bool Music::unload() throw( Ceylan::LoadableException )
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:
	
#if OSDL_USES_SDL_MIXER

	::Mix_FreeMusic( _content ) ;
	_content = 0 ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Music::unload failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}




// Audible implementation.


Volume Music::getVolume() const throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	try
	{
	
		if ( hasContent() )
			return ::Mix_VolumeMusic( /* just read it */ -1 ) ;
		else
			throw MusicException( 
				"Music::getVolume failed: no loaded music available." ) ;
				
	}
	catch( const Ceylan::LoadableException & e )
	{
	
		throw MusicException( "Music::getVolume failed: " + e.toString() ) ;
		
	}
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::getVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::setVolume( Volume newVolume ) throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	try
	{
	
		if ( hasContent() )
			::Mix_VolumeMusic( static_cast<int>( newVolume ) ) ;
		else
			throw MusicException( 
				"Music::setVolume failed: no loaded music available." ) ;
				
	}
	catch( const Ceylan::LoadableException & e )
	{
	
		// For getExistingContent:
		throw MusicException( "Music::setVolume failed: " + e.toString() ) ;
		
	}
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::setVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



MusicType Music::getType() const throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return GetTypeOf( this ) ;		
				
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::setVolume failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}




// Play section.



// Simple play subsection.


void Music::play( PlaybackCount playCount ) throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
			
	if ( ! hasContent() )
		throw AudibleException( "Music::play failed: "
			"no loaded music available" ) ;
			
	if ( ::Mix_PlayMusic( _content, GetLoopsForPlayCount( playCount ) ) == -1 )
		throw MusicException( "Music::play failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::play failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



// Play with fade-in subsection.


void Music::playWithFadeIn( Ceylan::System::Millisecond fadeInMaxDuration,
	PlaybackCount playCount ) throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
			
	if ( ! hasContent() )
		throw AudibleException( "Music::playWithFadeIn failed: "
			"no loaded music available" ) ;

	if ( ::Mix_FadeInMusic( _content, GetLoopsForPlayCount( playCount ),
			fadeInMaxDuration ) == -1 )
		throw MusicException( "Music::playWithFadeIn failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::playWithFadeIn failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

	
}



void Music::playWithFadeInFromPosition( 
		Ceylan::System::Millisecond fadeInMaxDuration,
		MusicPosition position, PlaybackCount playCount ) 
	throw( AudibleException )
{

#if OSDL_USES_SDL_MIXER
				
	if ( ! hasContent() )
		throw AudibleException( "Music::playWithFadeInFromPosition failed: "
			"no loaded music available" ) ;

	if ( ::Mix_FadeInMusicPos( _content, GetLoopsForPlayCount( playCount ),
			fadeInMaxDuration, static_cast<double>( position ) ) == -1 )
		throw MusicException( "Music::playWithFadeInFromPosition failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::playWithFadeInFromPosition failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::pause() throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_PauseMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::pause failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::resume() throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_ResumeMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::resume failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::rewind() throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_RewindMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::rewind failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}

	
																				
void Music::setPosition( MusicPosition newPosition ) throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	if ( ::Mix_SetMusicPosition( static_cast<double>( newPosition ) ) == -1 )
		throw MusicException( "Music::setPosition failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::setPosition failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::halt() throw( MusicException )		
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_HaltMusic() ;
				
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::halt failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



void Music::fadeOut( Ceylan::System::Millisecond fadeOutMaxDuration ) 
	throw( MusicException )
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	if ( ::Mix_FadeOutMusic( fadeOutMaxDuration ) == 0 )
		throw MusicException( "Music::fadeOut failed: "
			+ string( ::Mix_GetError() ) ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::halt failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}



const string Music::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	try
	{
	
		if ( hasContent() )
		{
		
			Volume v = getVolume() ;
	
			return "Loaded music whose volume is " + Ceylan::toString( v )
				+ "(" + Ceylan::toString( 100 * v 
					/ ( AudioModule::MaxVolume - AudioModule::MinVolume ) )
				+ "%)" ;
			
		}
		else
		{
		
			return "Music currently not loaded" ;
			
		}		
	
	}
	catch(  const Ceylan::Exception & e )
	{
	
		return "Music::toString failed (abnormal): " + e.toString() ;
		 
	}
	
}




// Protected section.


MusicType Music::GetTypeOf( const Music * music ) throw( AudioException )
{

#if OSDL_USES_SDL_MIXER
	
	LowLevelMusic * actualMusic ;
	
	if ( music == 0 )
	{
		actualMusic = 0 ;
	}	
	else
	{
		
		if ( music->hasContent() )
			actualMusic = const_cast<LowLevelMusic *>( 
				& music->getExistingContentAsConst() ) ;
		else
			throw AudioException( 
				"Music::GetTypeOf failed: no loaded music available" ) ;
		
	}
	
			
	switch( ::Mix_GetMusicType( /* current music if null */ actualMusic ) )
	{
	
		case MUS_WAV: 
			return Wave ;
			break ;
			
		case MUS_MOD:
			return MOD ;
			break ;
			
		case MUS_MID: 
			return MIDI ;
			break ;
			
		case MUS_OGG: 
			return OggVorbis ;
			break ;
			
		case MUS_MP3: 
			return MP3 ;
			break ;
						
		case MUS_CMD: 
			return CommandBased ;
			break ;
			
		case MUS_NONE: 
			return NoMusic ;
			break ;
			
		default: 
			return Unknown ;
			break ;
			
	}
	
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "Music::GetTypeOf failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}

