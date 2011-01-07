/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


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



AudioChannelException::AudioChannelException( const string & reason ) :
	AudioException( reason )
{

}



AudioChannelException::~AudioChannelException() throw()
{

}




AudioChannel::AudioChannel( ChannelNumber channelNumber ) :
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



ChannelNumber AudioChannel::getNumber() const
{

	return _channelNumber ;
		
}



Volume AudioChannel::getVolume() const
{

#if OSDL_USES_SDL_MIXER
	
	return static_cast<Volume>( ::Mix_Volume( _channelNumber, -1 ) ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::getVolume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::setVolume( Volume newVolume ) 
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Volume( _channelNumber, newVolume ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setVolume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::setPanning( Ceylan::Maths::Percentage leftPercentage )
{

#if OSDL_USES_SDL_MIXER

	Ceylan::Uint8 left = static_cast<Ceylan::Uint8>( 
		( leftPercentage * 254 ) / 100 ) ;
	
	if ( ::Mix_SetPanning( _channelNumber, left,
			/* right */ 254 - left ) == 0 )
		throw AudioChannelException( "AudioChannel::setPanning failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setPanning failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}
	
					
					
void AudioChannel::unsetPanning()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPanning( _channelNumber, 255,	255 ) == 0 )
		throw AudioChannelException( "AudioChannel::unsetPanning failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::unsetPanning failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::setReverseStereo( bool reverse )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetReverseStereo( _channelNumber, 
			( ( reverse == true ) ? 1 : 0 ) ) == 0 )
		throw AudioException( "AudioChannel::setReverseStereo failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setReverseStereo failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::setDistanceAttenuation( ListenerDistance distance ) 
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetDistance( _channelNumber, distance ) == 0 )
		throw AudioChannelException( 
			"AudioChannel::setDistanceAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setDistanceAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::unsetDistanceAttenuation()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetDistance( _channelNumber, 0 ) == 0 )
		throw AudioChannelException( 
			"AudioChannel::unsetDistanceAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( 
		"AudioChannel::unsetDistanceAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::setPositionAttenuation( ListenerDistance distance,
	ListenerAngle angle )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPosition( _channelNumber, distance, angle ) == 0 )
		throw AudioChannelException( 
			"AudioChannel::setPositionAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::setPositionAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::unsetPositionAttenuation()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPosition( _channelNumber, 0, 0 ) == 0 )
		throw AudioChannelException( 
			"AudioChannel::unsetPositionAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( 
		"AudioChannel::unsetPositionAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



bool AudioChannel::isPlaying() const
{

#if OSDL_USES_SDL_MIXER
	
	return ( ::Mix_Playing( _channelNumber ) != 0 ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPlaying failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



bool AudioChannel::isPaused() const
{

#if OSDL_USES_SDL_MIXER
	
	return ( ::Mix_Paused( _channelNumber ) != 0 ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



FadingStatus AudioChannel::getFadingStatus() const
{

#if OSDL_USES_SDL_MIXER
	
	switch( ::Mix_FadingChannel( _channelNumber ) )
	{
	
		case MIX_NO_FADING:
			return None ;
			break ;	
	
		case MIX_FADING_OUT:
			return Out ;
			break ;	
	
		case MIX_FADING_IN:
			return In ;
			break ;	
	
		default:
			LogPlug::error( 
				"AudioChannel::getFadingStatus: unexpected status" ) ;
			return None ;
			break ;	
				
	}
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::isPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioChannel::pause()
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Pause( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::pause failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::resume()
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_Resume( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::resume failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::waitEndOfPlayback() const
{

	while ( isPlaying() )
		Ceylan::System::atomicSleep() ;
		
}



void AudioChannel::halt()
{

#if OSDL_USES_SDL_MIXER
	
	::Mix_HaltChannel( _channelNumber ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioChannelException( "AudioChannel::halt failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioChannel::expireIn( Ceylan::System::Millisecond expireDuration )
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



void AudioChannel::onPlaybackFinished()			
{

	/*
	 * Does nothing on purpose, meant to be overriden.
	 *
	 * @note Nevercall SDL_Mixer functions, nor SDL_LockAudio, from a callback
	 * function.
	 *
	 */
	 
}


		
const string AudioChannel::toString( Ceylan::VerbosityLevels level ) const
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

