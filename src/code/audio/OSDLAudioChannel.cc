#include "OSDLAudioChannel.h"


#include "OSDLAudio.h"               // for getMixingChannelAt


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


/*
 * Implementation notes: Mix_GetChunk not used as potentially unstable.
 * Groupings of mixer channels not integrated as not needed currently.
 *
 */

using std::string ;



using namespace Ceylan::Log ;

using namespace OSDL::Audio ;



AudioChannelException::AudioChannelException( const string & reason ) throw():
	AudioException( reason )
{

}



AudioChannelException::~AudioChannelException() throw()
{

}




AudioChannel::AudioChannel( ChannelNumber channelNumber ) 
		throw( AudioChannelException ):
	Ceylan::TextDisplayable(),
	_channelNumber( channelNumber )
{

#if OSDL_USES_SDL_MIXER

	// Registers callback-like method:
	
	::Mix_ChannelFinished( HandleFinishedPlaybackCallback ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel constructor failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER
	
}	



AudioChannel::~AudioChannel() throw()
{
	
	// Nothing special to do.
		
}



ChannelNumber AudioChannel::getNumber() const throw()
{

	return _channelNumber ;
		
}



Volume AudioChannel::getVolume() const throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	return static_cast<Volume>( ::Mix_Volume( _channelNumber, -1 ) ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::getVolume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::setVolume( Volume newVolume ) 
	throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Volume( _channelNumber, newVolume ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setVolume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



bool AudioChannel::isPlaying() const throw()
{

#if OSDL_USES_SDL_MIXER
	
	return ( ::Mix_Playing( _channelNumber ) != 0 ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPlaying failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



bool AudioChannel::isPaused() const throw()
{

#if OSDL_USES_SDL_MIXER
	
	return ( ::Mix_Paused( _channelNumber ) != 0 ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



FadingStatus AudioChannel::getFadingStatus() const throw()
{

#if OSDL_USES_SDL_MIXER
	
	switch( ::Mix_FadingChannel( _channelNumber ) )
	{
	
		case MIX_NO_FADING:
			return None;
			break ;	
	
		case MIX_FADING_OUT:
			return Out;
			break ;	
	
		case MIX_FADING_IN:
			return In;
			break ;	
	
		default:
			LogPlug::error( 
				"AudioChannel::getFadingStatus: unexpected status" ) ;
			return None;
			break ;	
				
	}
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::pause() throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Pause( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::pause failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::resume() throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Resume( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::resume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::halt() throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_HaltChannel( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::halt failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::expireIn( Ceylan::System::Millisecond expireDuration )
	throw( AudioChannelException )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_ExpireChannel( _channelNumber, expireDuration ) != 1 )
		throw AudioChannelException( "AudioChannel::expireIn failed: "
			+ string( ::Mix_GetError() ) ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::expireIn failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}

	
					
void AudioChannel::fadeOutDuring( Ceylan::System::Millisecond fadeOutDuration )
	throw( AudioChannelException )										
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_FadeOutChannel( _channelNumber, fadeOutDuration ) != 1 )
		throw AudioChannelException( "AudioChannel::fadeOutDuring failed: "
			+ string( ::Mix_GetError() ) ) ;
	
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::fadeOutDuring failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::onPlaybackFinished()	throw( AudioChannelException )			
{

	/*
	 * Does nothing, meant to be overriden.
	 *
	 * @note Nevercall SDL_Mixer functions, nor SDL_LockAudio, from a callback
	 * function.
	 *
	 */
}


		
const string AudioChannel::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
		
	return "Mixing channel #" + Ceylan::toString( _channelNumber ) ;
	
}





// Non-static method or classical function needed for call-back:
void AudioChannel::HandleFinishedPlaybackCallback( int channelNumber )
{

	try
	{
	
		OSDL::Audio::getExistingAudioModule().getMixingChannelAt(
			static_cast<ChannelNumber>(	channelNumber ) ).onPlaybackFinished() ;
			
	}
	catch( const AudioException & e )
	{
	
		LogPlug::error( "AudioChannel::HandleFinishedPlaybackCallback failed: "
			+ e.toString() ) ;
	
	}
	
}

