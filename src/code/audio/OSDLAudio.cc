#include "OSDLAudio.h"


#include "OSDLUtils.h"               // for getBackendLastError
#include "OSDLBasic.h"               // for OSDL::GetVersion, etc.

#include "OSDLAudioChannel.h"        // for AudioChannel
#include "OSDLMusic.h"               // for GetTypeOf



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
	throw( AudioException )
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
		
		default:
			throw AudioException( "OSDL::Audio::sampleFormatToString failed: "
				"unknown sample format (" + Ceylan::toString( format ) + ")" ) ;
			break ;	
	
	}

}




std::string OSDL::Audio::channelFormatToString( SampleFormat format )
	throw( AudioException )
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




AudioModule::AudioModule() throw( AudioException ):
	Ceylan::Module( 
		"OSDL Audio module",
		"This is the root audio module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"LGPL" ),
	_mixerInitialized( false ),
	_chunkSize( 0 )			
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


	// Registers callback-like method:
	::Mix_HookMusicFinished( HandleMusicPlaybackFinishedCallback ) ;

	
	send( "Using SDL_mixer backend, compiled against the " 
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.major) + "."
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.minor) + "."
		+ Ceylan::toNumericalString( compileTimeSDLMixerVersion.patch) 
		+ " version, linked against the "
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.major) + "."
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.minor) + "."
		+ Ceylan::toNumericalString( linkTimeSDLMixerVersion.patch) 
		+ " version." ) ;
	
#endif // OSDL_USES_SDL_MIXER


#else // OSDL_USES_SDL

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	// Nothing special to initialize on the ARM7.
	
#elif defined(OSDL_RUNS_ON_ARM9)

	// Nothing special to initialize on the ARM9.

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
	throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	if ( _mixerInitialized )
		throw AudioException( "AudioModule::setMode failed: "
			"mixing mode already set, and not unset yet" ) ;
			
	if ( ::Mix_OpenAudio( outputFrequency, outputSampleFormat,
			GetChannelCountFor( outputChannel ), outputBufferSize ) != 0 )
		throw AudioException( "AudioModule::setMode failed: "
			+ string( ::Mix_GetError() ) ) ;

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
	ChannelNumber & actualOutputChannelCount ) throw( AudioException )
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



void AudioModule::unsetMode() throw( AudioException )
{

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
		
		LogPlug::warning( "AudioModule::unsetMode called "
			"whereas not already initialized, nothing done." ) ;
		
	}
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::unsetMode failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::setPanning( Ceylan::Maths::Percentage leftPercentage )
	throw( AudioException )
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



void AudioModule::unsetPanning() throw( AudioException )
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


	
void AudioModule::setReverseStereo( bool reverse ) throw( AudioException )	
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
	throw( AudioException )
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



void AudioModule::unsetDistanceAttenuation() throw( AudioException )
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
	ListenerAngle angle ) throw( AudioException )
{

#if OSDL_USES_SDL_MIXER
	
	if ( ::Mix_SetPosition( /* postmix stream */ MIX_CHANNEL_POST, 
			distance, angle ) == 0 )
		throw AudioException( "AudioModule::setPositionAttenuation failed: " 
			+ string( ::Mix_GetError() ) ) ;
		
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setPositionAttenuation failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



void AudioModule::unsetPositionAttenuation() throw( AudioException )
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
	
	
	
MusicType AudioModule::getTypeOfCurrentMusic() const throw( AudioException )	
{

#if OSDL_USES_SDL_MIXER

	return Music::GetTypeOf( /* current music */ 0 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getTypeOfCurrentMusic failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}				


					




// Channel section.



ChannelNumber AudioModule::getMixingChannelCount() const throw( AudioException )
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



AudioChannel & AudioModule::getMixingChannelAt( ChannelNumber index )
	const throw( AudioException )
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
	throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	::Mix_Volume( /* all channels */ -1, newVolume ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::setVolumeForAllMixingChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



ChannelNumber AudioModule::getPlayingChannelCount() const 
	throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_Playing( /* all channels */ -1 ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getPlayingChannelCount failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



ChannelNumber AudioModule::getPausedChannelCount() const 
	throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_Paused( /* all channels */ -1 ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::getPausedChannelCount failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}
	
					

void AudioModule::pauseAllChannels() throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	::Mix_Pause( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::pauseAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



void AudioModule::resumeAllChannels() throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	::Mix_Resume( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::resumeAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



void AudioModule::haltAllChannels() throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	::Mix_HaltChannel( /* all channels */ -1 ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::haltAllChannels failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}
	
	
	
ChannelNumber AudioModule::expireAllChannelsIn( 
	Ceylan::System::Millisecond expireDuration ) throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_ExpireChannel( /* all channels */ -1, expireDuration ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::expireAllChannelsIn failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}
	
	
					
ChannelNumber AudioModule::fadeOutAllChannelsDuring( 
	Ceylan::System::Millisecond fadeOutDuration ) throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ::Mix_FadeOutChannel( /* all channels */ -1, fadeOutDuration ) ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::fadeOutAllChannelsDuring failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER
	
}



bool AudioModule::isMusicPlaying() const throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ( ::Mix_PlayingMusic() == 1 ) ;  ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::isMusicPlaying failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



bool AudioModule::isMusicPaused() const throw( AudioException )
{

#if OSDL_USES_SDL_MIXER

	return ( ::Mix_PausedMusic() == 1 ) ;  ;

#else // OSDL_USES_SDL_MIXER

	throw AudioException( "AudioModule::isMusicPaused failed: "
		"no SDL_mixer support available" ) ;
		
#endif // OSDL_USES_SDL_MIXER

}



FadingStatus AudioModule::getMusicFadingStatus() const throw( AudioException )
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

	
											
std::string AudioModule::getDriverName() const throw( AudioException )
{

	return AudioModule::GetDriverName() ;

}



const string AudioModule::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Audio module, " ;
	
	if ( _mixerInitialized )
		res += "mixer is initialized" ;
	else
		res += "mixer not initialized" ;
	
	res += ". Chunk size is " + Ceylan::toString( _chunkSize ) 
		+ " bytes. There are " + Ceylan::toString( _inputChannels.size() )
		+ " input mixing channel(s)" ;
	 	
	if ( level == Ceylan::low )
		return res ;
				
	res += ". " + Ceylan::Module::toString() ;	
	
	return res ;
	
}



string AudioModule::DescribeEnvironmentVariables() throw()
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



bool AudioModule::IsAudioInitialized() throw()
{

	return _AudioInitialized ;
	
}



string AudioModule::GetDriverName() throw( AudioException )
{

#if OSDL_USES_SDL

	char driverName[ DriverNameMaximumLength + 1 ]  ;
	
	if ( SDL_AudioDriverName( driverName, DriverNameMaximumLength ) == 0 )
		throw AudioException( "AudioModule::GetDriverName failed: "
			"the audio driver is probably not initialized." ) ;
			
	return std::string( driverName ) ;
	
#else // OSDL_USES_SDL

	return "unknown (not using SDL)" ;
	
#endif // OSDL_USES_SDL
		
}



void AudioModule::onMusicPlaybackFinished() throw( AudioException )
{

	/*
	 * Does nothing on purpose, meant to be overriden.
	 *
	 * @note Nevercall SDL_Mixer functions, nor SDL_LockAudio, from a callback
	 * function.
	 *
	 */
	 
}




// Static section.

ChannelNumber AudioModule::GetChannelCountFor( ChannelFormat format )
	throw( AudioException )
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
	throw( AudioException )
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

AudioModule & OSDL::Audio::getExistingAudioModule() throw()
{

	try
	{
		
		return OSDL::getExistingCommonModule().getAudioModule() ;
	
	}
	catch( const OSDL::Exception & e )
	{
		
		Ceylan::emergencyShutdown( "OSDL::Audio::getExistingAudioModule: "
			"no audio module available: " + e.toString() ) ;
	
	}
	
}




