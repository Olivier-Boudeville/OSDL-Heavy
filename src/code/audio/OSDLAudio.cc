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


#include "OSDLAudio.h"


#include "OSDLUtils.h"               // for getBackendLastError
#include "OSDLBasic.h"               // for OSDL::GetVersion, etc.

#include "OSDLAudioChannel.h"        // for AudioChannel
#include "OSDLMusic.h"               // for GetTypeOf
#include "OSDLMusicManager.h"        // for MusicManager



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


#include <list>
using std::list ;

using std::string ;
using std::vector ;

using Ceylan::Maths::Hertz ;


using namespace Ceylan::Log ;

using namespace OSDL::Audio ;



// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_VERBOSE_AUDIO_MODULE

#define LOG_DEBUG_AUDIO(message)   LogPlug::debug(message)
#define LOG_TRACE_AUDIO(message)   LogPlug::trace(message)
#define LOG_WARNING_AUDIO(message) LogPlug::warning(message)

#else // OSDL_DEBUG_AUDIO_PLAYBACK

#define LOG_DEBUG_AUDIO(message)
#define LOG_TRACE_AUDIO(message)
#define LOG_WARNING_AUDIO(message)

#endif // OSDL_DEBUG_AUDIO_PLAYBACK



string OSDL::Audio::AudioModule::AudioPathEnvironmentVariable = "AUDIO_PATH" ;



Ceylan::System::FileLocator OSDL::Audio::AudioModule::AudioFileLocator(
	AudioPathEnvironmentVariable ) ;




#if OSDL_USES_SDL

const SampleFormat AudioModule::Uint8SampleFormat		 = AUDIO_U8 ;
const SampleFormat AudioModule::Sint8SampleFormat		 = AUDIO_S8 ;


const SampleFormat AudioModule::LittleUint16SampleFormat = AUDIO_U16LSB ;
const SampleFormat AudioModule::LittleSint16SampleFormat = AUDIO_S16LSB ;

const SampleFormat AudioModule::BigUint16SampleFormat	 = AUDIO_U16MSB ;
const SampleFormat AudioModule::BigSint16SampleFormat	 = AUDIO_S16MSB ;

const SampleFormat AudioModule::NativeUint16SampleFormat = AUDIO_U16SYS ;
const SampleFormat AudioModule::NativeSint16SampleFormat = AUDIO_S16SYS ;

#else // OSDL_USES_SDL


// Same values as in SDL:

const SampleFormat AudioModule::Uint8SampleFormat		 = 0x0008 ;
const SampleFormat AudioModule::Sint8SampleFormat		 = 0x8008 ;


const SampleFormat AudioModule::LittleUint16SampleFormat = 0x0010 ;
const SampleFormat AudioModule::LittleSint16SampleFormat = 0x8010 ;

const SampleFormat AudioModule::BigUint16SampleFormat	 = 0x1010 ;
const SampleFormat AudioModule::BigSint16SampleFormat	 = 0x9010 ;


#if CEYLAN_DETECTED_LITTLE_ENDIAN

const SampleFormat AudioModule::NativeUint16SampleFormat = 0x0010 ;
const SampleFormat AudioModule::NativeSint16SampleFormat = 0x8010 ;

#else // CEYLAN_DETECTED_LITTLE_ENDIAN

const SampleFormat AudioModule::NativeUint16SampleFormat = 0x1010 ;
const SampleFormat AudioModule::NativeSint16SampleFormat = 0x9010 ;

#endif // CEYLAN_DETECTED_LITTLE_ENDIAN


#endif // OSDL_USES_SDL



const SampleFormat AudioModule::IMAADPCMSampleFormat     = 0x0012 ;



/*
 * These values are identifiers, not a number of channels: different formats
 * may have the same number of channels.
 *
 * @see GetChannelCountFor
 *
 * More than 2 channels means surround.
 *
 */
const ChannelFormat AudioModule::Mono	= 1 ;
const ChannelFormat AudioModule::Stereo = 2 ;


const Volume AudioModule::MinVolume = 0   ;
const Volume AudioModule::MaxVolume = 128 ;





std::string OSDL::Audio::sampleFormatToString( SampleFormat format )
{

	switch( format )
	{
	
		case AudioModule::Uint8SampleFormat:
			return "unsigned 8-bit" ;
			break ;
			
		case AudioModule::Sint8SampleFormat:
			return "signed 8-bit" ;
			break ;
			
		case AudioModule::LittleUint16SampleFormat:
#if CEYLAN_DETECTED_LITTLE_ENDIAN
			return "little-endian unsigned 16-bit (native)" ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
			return "little-endian unsigned 16-bit (non native)" ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
			break ;
			
		case AudioModule::LittleSint16SampleFormat:
#if CEYLAN_DETECTED_LITTLE_ENDIAN
			return "little-endian signed 16-bit (native)" ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
			return "little-endian signed 16-bit (non native)" ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
			break ;
			
		case AudioModule::BigUint16SampleFormat:
#if CEYLAN_DETECTED_LITTLE_ENDIAN
			return "big-endian unsigned 16-bit (non native)" ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
			return "big-endian unsigned 16-bit (native)" ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
			break ;
			
		case AudioModule::BigSint16SampleFormat:
#if CEYLAN_DETECTED_LITTLE_ENDIAN
			return "big-endian signed 16-bit (non native)" ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
			return "big-endian signed 16-bit (native)" ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
			break ;
		
		case AudioModule::IMAADPCMSampleFormat:
			return "IMA ADPCM (non Microsoft)" ;
			break ;
			
		default:
			throw AudioException( "OSDL::Audio::sampleFormatToString failed: "
				"unknown sample format (" + Ceylan::toString( format ) + ")" ) ;
			break ;	
	
	}

}



std::string OSDL::Audio::channelFormatToString( SampleFormat format )
{

	switch( format )
	{
	
		case AudioModule::Mono:
			return "mono" ;
			break ;
			
		case AudioModule::Stereo:
			return "stereo" ;
			break ;
			
		default:
			throw AudioException( "OSDL::Audio::channelFormatToString failed: "
				"unknown channel format (" 
				+ Ceylan::toString( format ) + ")" ) ;
			break ;	
	
	}

}
	

			


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




#if OSDL_USES_SDL

/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const string AudioModule::SDLEnvironmentVariables[] = 
{
	
	"AUDIODEV",
	"SDL_AUDIODRIVER",
	"SDL_DISKAUDIOFILE",
	"SDL_DISKAUDIODELAY",
	"SDL_DSP_NOSELECT",
	"SDL_PATH_DSP"
	
} ;

#else // OSDL_USES_SDL

const string AudioModule::SDLEnvironmentVariables[] = {} ;

#endif // OSDL_USES_SDL




const Ceylan::Uint16 AudioModule::DriverNameMaximumLength = 50 ;





AudioModule::AudioModule() :
	Ceylan::Module( 
		"OSDL audio module",
		"This is the audio module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"disjunctive LGPL/GPL" ),
	_mixerInitialized( false ),
	_chunkSize( 0 ),
	_controlMusicManager( true ),
	_isGuiEnabled( false )
{

	send( "Initializing audio subsystem." ) ;

#if OSDL_USES_SDL

	if ( SDL_InitSubSystem( 
			CommonModule::UseAudio ) != CommonModule::BackendSuccess )
		throw AudioException( "AudioModule constructor: "
			"unable to initialize audio subsystem: " 
			+ Utils::getBackendLastError() ) ;

#if OSDL_USES_SDL_MIXER
	
	SDL_version compileTimeSDLMixerVersion ;
	MIX_VERSION( & compileTimeSDLMixerVersion ) ;

	SDL_version linkTimeSDLMixerVersion = *Mix_Linked_Version() ;

	send( "Using SDL_mixer backend, compiled against the " 
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.major) + "."
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.minor) + "."
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.patch) 
		+ " version, linked against the "
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.major) + "."
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.minor) + "."
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.patch) 
		+ " version." ) ;


	// Initializing mixer, for Ogg playback:
	::Mix_Init( MIX_INIT_OGG ) ;


	// Registers callback-like method:
	::Mix_HookMusicFinished( HandleMusicPlaybackFinishedCallback ) ;

	
	// Always true by default:
	if ( _controlMusicManager )
	{
		
		if ( MusicManager::_CurrentMusicManager != 0 )
			throw AudioException( "AudioModule constructor failed: "
				"there is already a music manager registered." ) ;

		MusicManager::_CurrentMusicManager = new MusicManager() ;		
	
	}
	
#endif // OSDL_USES_SDL_MIXER


#else // OSDL_USES_SDL

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	// @see testOSDLSound.arm7.c if C++ OSDL is ported to the ARM7.
	
#elif defined(OSDL_RUNS_ON_ARM9)

	CommandManager::GetCommandManager().enableMusicSupport() ;

#endif // OSDL_RUNS_ON_ARM7
	


#endif // OSDL_ARCH_NINTENDO_DS
	
#endif // OSDL_USES_SDL

	_AudioInitialized = true ;
	
	
	send( "Audio subsystem initialized." ) ;
	
	dropIdentifier() ;
	
}	



AudioModule::~AudioModule() throw()
{

	send( "Stopping audio subsystem." ) ;
	
	if ( _controlMusicManager )
	{

		if ( MusicManager::_CurrentMusicManager != 0 )
		{
	
			delete MusicManager::_CurrentMusicManager ;
			MusicManager::_CurrentMusicManager = 0 ;
		
		}
		else
		{
		
			LogPlug::error( "AudioModule destructor: "
				"no music manager found." ) ;
				
		}		
		
	}
	
	
	if ( _mixerInitialized )
	{
	
		try 
		{
		
			unsetMode() ;
		
		}
		catch( const AudioException & e )
		{
		
			LogPlug::error( "AudioModule destructor: "
				"unsetting of the mixer failed: " + e.toString() ) ;
		}
	
	}	

	
#if OSDL_USES_SDL

	SDL_QuitSubSystem( CommonModule::UseAudio ) ;

#else // OSDL_USES_SDL

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

#elif defined(OSDL_RUNS_ON_ARM9)

#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS
	
#endif // OSDL_USES_SDL
	
	_AudioInitialized = false ;
	
	send( "Audio subsystem stopped." ) ;
	
}



void AudioModule::setMode( Hertz outputFrequency, 
	SampleFormat outputSampleFormat, ChannelFormat outputChannel,
	ChunkSize outputBufferSize, ChannelNumber mixingChannelCount ) 
{

#if OSDL_USES_SDL_MIXER

	if ( _mixerInitialized )
		throw AudioException( "AudioModule::setMode failed: "
			"mixing mode already set, and not unset yet." ) ;
			
	/*
	 * In some cases Mix_GetError may return an unrelated previous SDL error:
	 * (and using OSDL::Utils::getBackendLastError() will not help)
	 *
	 */
	if ( ::Mix_OpenAudio( outputFrequency, outputSampleFormat,
			GetChannelCountFor( outputChannel ), outputBufferSize ) != 0 )
		throw AudioException( "AudioModule::setMode failed: "
			+ string( ::Mix_GetError() ) 
			+ ". Maybe the audio driver is used by another application?"  ) ;

	_chunkSize = outputBufferSize ;
	
	ChannelNumber channelCount = ::Mix_AllocateChannels( mixingChannelCount ) ;
	
	if ( channelCount != mixingChannelCount )
		throw AudioException( "AudioModule::setMode: wanted " 
			+ Ceylan::toString( mixingChannelCount ) 
			+ " input mixing channel(s), but got "  
			+ Ceylan::toString( _inputChannels.size() ) 
			+ " channel(s) instead" ) ;
	
	try
	{	
		
		for ( ChannelNumber i = 0; i < channelCount; i++ )
			_inputChannels.push_back( new AudioChannel( i ) ) ;
	
	}
	catch( const AudioChannelException & e )
	{
	
		throw AudioException( 
			"AudioModule::setMode failed when creating mixing channels: " 
			+ e.toString() ) ;
					
	}			
		 		
	_mixerInitialized = true ;	
	
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setMode failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



Ceylan::System::Millisecond AudioModule::getObtainedMode( 
	Hertz & actualOutputFrequency, SampleFormat & actualOutputSampleFormat,
	ChannelNumber & actualOutputChannelCount )
{

#if OSDL_USES_SDL_MIXER

	// Because of sign conversion:
	int channelNumber, frequency ;
	
	if ( ::Mix_QuerySpec( & frequency, & actualOutputSampleFormat, 
			& channelNumber ) == 0 )
		throw AudioException( "	AudioModule::getObtainedMode failed: "
			+  string( ::Mix_GetError() ) ) ;
	
	actualOutputChannelCount = channelNumber	;
	actualOutputFrequency = frequency ;
	
	// The mean latency is half the buffer to wait:
	return static_cast<Ceylan::System::Millisecond>( 
		1000.0f / 2 * _chunkSize / 
			( actualOutputFrequency * actualOutputChannelCount 
				* GetSampleSizeFor( actualOutputSampleFormat ) ) ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getObtainedMode failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
		
}



void AudioModule::unsetMode()
{

	//Ceylan::checkpoint( "AudioModule::unsetMode." ) ;
	
#if OSDL_USES_SDL_MIXER

	if ( _mixerInitialized )
	{
	
		::Mix_CloseAudio() ;
		_mixerInitialized = false ;
		_chunkSize = 0 ;
		
		for ( vector<AudioChannel *>::iterator it = _inputChannels.begin();
				it != _inputChannels.end(); it++ )
			delete (*it) ;
			
		_inputChannels.clear() ;
				
	}	
	else
	{
		
		LOG_WARNING_AUDIO( "AudioModule::unsetMode called "
			"whereas not already initialized, nothing done." ) ;
		
	}
	
	::Mix_Quit() ;
	
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::unsetMode failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::setPanning( Ceylan::Maths::Percentage leftPercentage )
{

#if OSDL_USES_SDL_MIXER

	Ceylan::Uint8 left = static_cast<Ceylan::Uint8>( 
		( leftPercentage * 254 ) / 100 ) ;
	
	if ( ::Mix_SetPanning( /* postmix stream */ MIX_CHANNEL_POST, left,
			/* right */ 254 - left ) == 0 )
		throw AudioException( "AudioModule::setPanning failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setPanning failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::unsetPanning()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPanning( /* postmix stream */ MIX_CHANNEL_POST, 255,
			255 ) == 0 )
		throw AudioException( "AudioModule::unsetPanning failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::unsetPanning failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}


	
void AudioModule::setReverseStereo( bool reverse )	
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetReverseStereo( /* postmix stream */ MIX_CHANNEL_POST, 
			( ( reverse == true ) ? 1 : 0 ) ) == 0 )
		throw AudioException( "AudioModule::setReverseStereo failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setReverseStereo failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}


	
void AudioModule::setDistanceAttenuation( ListenerDistance distance ) 
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetDistance( /* postmix stream */ MIX_CHANNEL_POST, distance ) 
			== 0 )
		throw AudioException( "AudioModule::setDistanceAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setDistanceAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::unsetDistanceAttenuation()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetDistance( /* postmix stream */ MIX_CHANNEL_POST, 0 ) == 0 )
		throw AudioException( "AudioModule::unsetDistanceAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::unsetDistanceAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}
	
	
	
void AudioModule::setPositionAttenuation( ListenerDistance distance,
	ListenerAngle angle )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPosition( /* postmix stream */ MIX_CHANNEL_POST, 
			angle, distance ) == 0 )
		throw AudioException( "AudioModule::setPositionAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setPositionAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::unsetPositionAttenuation()
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPosition( /* postmix stream */ MIX_CHANNEL_POST, 0, 0 ) == 0 )
		throw AudioException( "AudioModule::unsetPositionAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::unsetPositionAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}
	
	
	
bool AudioModule::isGUIEnabled() const
{

	return _isGuiEnabled ;

}



void AudioModule::setGUIEnableStatus( bool newStatus )
{

	_isGuiEnabled = newStatus ;

}


	
MusicType AudioModule::getTypeOfCurrentMusic() const
{

#if OSDL_USES_SDL_MIXER

	return Music::GetTypeOf( /* current music */ 0 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getTypeOfCurrentMusic failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}				


					




// Channel section.



ChannelNumber AudioModule::getMixingChannelCount() const
{

#if OSDL_USES_SDL_MIXER

#if OSDL_DEBUG

	if ( ::Mix_AllocateChannels( /* just read */ -1 ) != 
			static_cast<int>( _inputChannels.size() ) )
		throw AudioException( "AudioModule::getMixingChannelCount failed: "
			"inconsistency in channel count" ) ;
		
#endif // OSDL_DEBUG

	// Should match _inputChannels.size():
	return ::Mix_AllocateChannels( /* just read */ -1 ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getMixingChannelCount failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



AudioChannel & AudioModule::getMixingChannelAt( ChannelNumber index ) const
{

	try
	{
	
		return *_inputChannels.at( index ) ;
	
	}
	catch( const std::exception & e )
	{
	
		throw AudioException( "AudioModule::getMixingChannelAt out of bounds: "
			+ string( e.what() ) ) ;
	}
	
}



void AudioModule::setVolumeForAllMixingChannels( Volume newVolume )
{

#if OSDL_USES_SDL_MIXER

	::Mix_Volume( /* all channels */ -1, newVolume ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setVolumeForAllMixingChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



ChannelNumber AudioModule::getPlayingChannelCount() const 
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_Playing( /* all channels */ -1 ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getPlayingChannelCount failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



ChannelNumber AudioModule::getPausedChannelCount() const 
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_Paused( /* all channels */ -1 ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getPausedChannelCount failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}
	
					

void AudioModule::pauseAllChannels()
{

#if OSDL_USES_SDL_MIXER

	::Mix_Pause( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::pauseAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



void AudioModule::resumeAllChannels()
{

#if OSDL_USES_SDL_MIXER

	::Mix_Resume( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::resumeAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



void AudioModule::haltAllChannels()
{

#if OSDL_USES_SDL_MIXER

	::Mix_HaltChannel( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::haltAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}
	
	
	
ChannelNumber AudioModule::expireAllChannelsIn( 
	Ceylan::System::Millisecond expireDuration )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_ExpireChannel( /* all channels */ -1, expireDuration ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::expireAllChannelsIn failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}
	
	
					
ChannelNumber AudioModule::fadeOutAllChannelsDuring( 
	Ceylan::System::Millisecond fadeOutDuration )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_FadeOutChannel( /* all channels */ -1, fadeOutDuration ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::fadeOutAllChannelsDuring failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



bool AudioModule::isMusicPlaying() const
{

#if OSDL_USES_SDL_MIXER

	return ( ::Mix_PlayingMusic() == 1 ) ;  ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::isMusicPlaying failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



bool AudioModule::isMusicPaused() const
{

#if OSDL_USES_SDL_MIXER

	return ( ::Mix_PausedMusic() == 1 ) ;  ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::isMusicPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



FadingStatus AudioModule::getMusicFadingStatus() const
{

#if OSDL_USES_SDL_MIXER

	switch( ::Mix_FadingMusic() )
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
				" AudioModule::getMusicFadingStatus: unexpected status" ) ;
			return None ;
			break ;	
				
	}

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getMusicFadingStatus failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}

	
											
std::string AudioModule::getDriverName() const
{

	return AudioModule::GetDriverName() ;

}



const string AudioModule::toString( Ceylan::VerbosityLevels level ) const
{
	
	
	string res = "Audio module, " ;
	
	if ( ! _mixerInitialized )
	{
	
		res += "mixer not initialized" ;
	
	}	
	else
	{
	
		res += "mixer is initialized. Chunk size is " 
			+ Ceylan::toString( _chunkSize ) 
			+ " bytes. There are " + Ceylan::toString( _inputChannels.size() )
			+ " input mixing channel(s)" ;
	 
	}
	 	
	if ( level == Ceylan::low )
		return res ;
				
	res += ". " + Ceylan::Module::toString() ;	
	
	return res ;
	
}



string AudioModule::DescribeEnvironmentVariables()
{

#if OSDL_USES_SDL

	Ceylan::Uint16 varCount = 
		sizeof( SDLEnvironmentVariables ) / sizeof (string) ;
		
	string result = "Examining the " + Ceylan::toString( varCount )
		+ " audio-related environment variables for SDL backend:" ;
	
	list<string> variables ;
		
	string var, value ;

	TextOutputFormat htmlFormat = Ceylan::TextDisplayable::GetOutputFormat() ;
	
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
	
#else // OSDL_USES_SDL

	return "(not using SDL)" ;
	
#endif // OSDL_USES_SDL
	
}



bool AudioModule::IsAudioInitialized()
{

	return _AudioInitialized ;
	
}



string AudioModule::GetDriverName()
{

#if OSDL_USES_SDL

	char driverName[ DriverNameMaximumLength + 1 ]  ;
	
	if ( SDL_AudioDriverName( driverName, DriverNameMaximumLength ) == 0 )
		throw AudioException( "AudioModule::GetDriverName failed: "
			"the audio driver is probably not initialized, "
			"or used by another application." ) ;
			
	return std::string( driverName ) ;
	
#else // OSDL_USES_SDL

	return "unknown (not using SDL)" ;
	
#endif // OSDL_USES_SDL
		
}



void AudioModule::onMusicPlaybackFinished()
{

	/*
	 * Does nothing on purpose, meant to be overriden.
	 *
	 * Most of the time, this callback is never called, since being 
	 * overridden by the one of the music manager.
	 *
	 * @note Never call SDL_Mixer functions, nor SDL_LockAudio, from a callback
	 * function.
	 *
	 */
	 
}





// Static section.


ChannelNumber AudioModule::GetChannelCountFor( ChannelFormat format )
{

	switch( format )
	{
	
		case Mono:
			return 1 ;
			break ;
			
		case Stereo:
			return 2 ;
			break ;
			
		default:
			throw AudioException( "AudioModule::GetChannelCountFor failed: "
				"unknown channel format (" + Ceylan::toString( format ) 
				+ ")" ) ;
			break ;	
			
	}

}


	
ChunkSize AudioModule::GetSampleSizeFor( SampleFormat format ) 
{

	switch( format )
	{
	
		case Uint8SampleFormat:
		case Sint8SampleFormat:
			return 1 ;
			break ;
			
		case LittleUint16SampleFormat:
		case LittleSint16SampleFormat:
		case BigUint16SampleFormat:
		case BigSint16SampleFormat:
		/* 
			Are duplicates of a previous pair:
		case NativeUint16SampleFormat:
		case NativeSint16SampleFormat:
		*/
			return 2 ;
			break ;
			
		default:
			throw AudioException( "AudioModule::GetSampleSizeFor failed: "
				"unknown sample format (" + Ceylan::toString( format ) 
				+ ")" ) ;
			break ;	
			
	}

}



// Non-static method or classical function needed for call-back:
void AudioModule::HandleMusicPlaybackFinishedCallback()
{

	// Note: if a music manager is used, it will override this callback.
	
	try
	{
	
		OSDL::Audio::getExistingAudioModule().onMusicPlaybackFinished() ;
			
	}
	catch( const AudioException & e )
	{
	
		LogPlug::error( 
			"AudioModule::HandleMusicPlaybackFinishedCallback failed: "
			+ e.toString() ) ;
	
	}
	
}




// Friend function.


AudioModule & OSDL::Audio::getExistingAudioModule()
{

	try
	{
		
		return OSDL::getExistingCommonModule().getAudioModule() ;
	
	}
	catch( const OSDL::Exception & e )
	{
		
		throw AudioException( "OSDL::Audio::getExistingAudioModule: "
			"no audio module available: " + e.toString() ) ;
	
	}
	
}

