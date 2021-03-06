/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLSound.h"

#include "OSDLAudio.h"               // for AudioModule
#include "OSDLFileTags.h"            // for sound file tag


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




// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_DEBUG_AUDIO_PLAYBACK

#define LOG_DEBUG_AUDIO(message)   LogPlug::debug(message)
#define LOG_TRACE_AUDIO(message)   LogPlug::trace(message)
#define LOG_WARNING_AUDIO(message) LogPlug::warning(message)

#else // OSDL_DEBUG_AUDIO_PLAYBACK

#define LOG_DEBUG_AUDIO(message)
#define LOG_TRACE_AUDIO(message)
#define LOG_WARNING_AUDIO(message)

#endif // OSDL_DEBUG_AUDIO_PLAYBACK



using std::string ;

using Ceylan::System::Millisecond ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Audio ;





/**
 * Implementation notes:
 *
 * Before, when SDL_mixer was to operate on filenames rather than directly on
 * SDL_rwops, on all PC-like platforms (including Windows and most UNIX), the
 * supported formats were WAVE, AIFF, RIFF, OGG, and VOC. WAVE and, to a lesser
 * extent, OGG, were recommended for sounds.
 *
 * Now, we want to be able to read sounds from opaque archives (using
 * EmbeddedFile instances, thus PhysicsFS), and SDL_mixer just allows the OGG
 * (OggVorbis), WAV, OGG, MP3, MOD or MIDI file formats to be used with the
 * underlying SDL_rwops.
 *
 * Thus, WAVE and, to a lesser extent, OGG, are still recommended for sounds, on
 * the PC.
 *
 * @see Mix_LoadWAV_RW
 *
 *
 * Neither channel grouping nor effects used, as no need felt.
 *
 * On the Nintendo DS, only one format is supported, the .osdl.sound format.
 *
 */



SoundException::SoundException( const string & reason ) :
	AudibleException( reason )
{

}



SoundException::~SoundException() throw()
{

}





Sound::Sound( const std::string & soundFile, bool preload ) :
	Audible( /* nothing loaded yet, hence not converted */ false ),
	Ceylan::LoadableWithContent<LowLevelSound>( soundFile ),
	_dataStream( 0 )
{

	if ( ! AudioModule::IsAudioInitialized() )
		throw SoundException( "Sound constructor failed: "
			"audio module not already initialized" ) ;

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

	//LogPlug::trace( "Sound deallocated." ) ;

}





// LoadableWithContent template instanciation.



bool Sound::load()
{

	if ( hasContent() )
		return false ;

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Sound::load failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)


	/*
	 * We expect here a file respecting the .osdl.sound format.
	 *
	 * @see trunk/tools/media/rawToOSDLSound.cc
	 *
	 */

	try
	{

		_content = new LowLevelSound() ;

		// Inits all _content members here.

		File & soundFile = File::Open( _contentPath ) ;

		// First check the OSDL sound tag:
		FileTag readTag = soundFile.readUint16() ;

		if ( readTag != SoundTag )
			throw Ceylan::LoadableException(
				"Sound::load: expected sound tag not found ("
				+ Ceylan::toString( SoundTag ) + "), read instead "
				+ Ceylan::toString( readTag ) + ", which corresponds to: "
				+ DescribeFileTag( readTag ) ) ;

		// Frequency:
		_content->_frequency = soundFile.readUint16() /* Hz */ ;


		// Format, bit depth (8 bit/16 bit PCM, IMA ADPCM, etc.):
		SampleFormat readFormat = soundFile.readUint16() ;

		/*
		 * Commented out because of:
		 * 'OSDL::Audio::AudioModule::LittleSint16SampleFormat' cannot appear
		 * in a constant-expression...

		switch( readFormat )
		{

			case AudioModule::LittleSint16SampleFormat:
				_content->_bitDepth = 16 ;
				break ;

			case AudioModule::Sint8SampleFormat:
				_content->_bitDepth = 8 ;
				break ;

			default:
				throw Ceylan::LoadableException( "Sound::load failed: "
					"unexpected bit depth (" + Ceylan::toString( readFormat )
					+ ") read from sound file '" + _contentPath + "'" ) ;
				break ;

		}
		*/

		if ( readFormat == AudioModule::LittleSint16SampleFormat )
			_content->_bitDepth = 16 ;
		else if ( readFormat == AudioModule::Sint8SampleFormat )
			_content->_bitDepth = 8 ;
		else if ( readFormat == AudioModule::IMAADPCMSampleFormat )
			_content->_bitDepth = 4 /* convention for IMA ADPCM */ ;
		else
			throw Ceylan::LoadableException( "Sound::load failed: "
				"unexpected bit depth (" + Ceylan::toString( readFormat )
				+ ") read from sound file '" + _contentPath + "'" ) ;


		// Mode (number of channels):
		ChannelFormat readMode = soundFile.readUint16() ;

		if ( readMode != AudioModule::Mono
				&& readMode != AudioModule::Stereo )
			throw Ceylan::LoadableException( "Sound::load failed: "
				"unexpected channel format read from sound file '"
				+ _contentPath + "'" ) ;

		_content->_mode = static_cast<Ceylan::Uint8>( readMode ) ;

		// Substracts the tag plus header size:
		Ceylan::Uint32 readSize = soundFile.size() - 4*sizeof(Ceylan::Uint16) ;

		LOG_DEBUG_AUDIO( "Sound::load: for '" + _contentPath
			+ "', bit depth is "
			+ Ceylan::toNumericalString( _content->_bitDepth )
			+ ", channel mode is "
			+ Ceylan::toNumericalString( _content->_mode )
			+ ", frequency is " + Ceylan::toString( _content->_frequency )
			+ " Hz, size of all samples is " + Ceylan::toString( readSize )
			+ " bytes." ) ;

		_content->_size = readSize ;

		// Force sound buffer to match the boundaries of ARM9 cache line:
		_content->_samples = CacheProtectedNew( readSize ) ;

		soundFile.readExactLength( _content->_samples, readSize ) ;

		// Flush the ARM9 data cache for the ARM7:
		DC_FlushRange( (void*) _content->_samples, readSize ) ;

		delete & soundFile ;

	}
	catch( const Ceylan::System::SystemException & e )
	{

		throw Ceylan::LoadableException( "Sound::load failed: "
			+ e.toString() ) ;

	}

	_convertedToOutputFormat = true ;

	return true ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER

	/*
	 * Supported: WAVE, MOD, MIDI, OGG and MP3, when using SDL_rwops with
	 * SDL_mixer's Mix_LoadWAV_RW.
	 *
	 */

	if ( hasContent() )
		return false ;

	try
	{

		//LogPlug::debug( "Sound::load: loading " + _contentPath ) ;

		/*
		 * Not used anymore, so that sounds can be loaded from embedded files as
		 * well:

		// Misleading, supports WAVE but other formats as well:
		_content = ::Mix_LoadWAV( FindAudiblePath( _contentPath ).c_str() ) ;

		 */

		Ceylan::System::File & soundFile = File::Open( _contentPath ) ;

		/*
		LogPlug::trace( "loaded " + soundFile.toString() + ", size = "
			+ Ceylan::toString( soundFile.size() ) ) ;
		 */

		// Creates the appropriate SDL_rwops and feeds it to SDL_mixer:

		_dataStream = & Utils::createDataStreamFrom( soundFile ) ;

		/*
		 * Let's call F the boolean 'free the source memory after loading' and A
		 * the boolean 'deleteDataStream is called in unload method'.
		 *
		 * If F is true:
		 *
		 *  - if A is true, we have a segmentation fault (already removed?)
		 *
		 *  - if A is false, no crash (hopefully no memory leak; we checked, for
		 * example the EmbeddedFile destructor is called indeed)
		 *
		 * If F is false:
		 *
		 *  - if A is true, we have 'mount failed: Files still open' which
		 * implies the file has not been closed and the SDL_rwops was not
		 * deleted (this is sadly the settings we wanted to select)
		 *
		 *  - if A is false, we must have a memory leak
		 *
		 * F = false, A = true being not functional, we had to go for F = true,
		 * A = false.
		 *
		 */

		_content = ::Mix_LoadWAV_RW( _dataStream,
			/* free the source memory after loading */ true ) ;


		// _content checked afterwards.


	}
	catch( const Ceylan::Exception & e )
	{

		throw Ceylan::LoadableException( "Sound::load failed: '"
			"unable to locate '" + _contentPath + "': " + e.toString() ) ;

	}

	if ( _content == 0 )
		throw Ceylan::LoadableException( "Sound::load failed: "
			+ string( ::Mix_GetError() ) ) ;

	_convertedToOutputFormat = true ;

	return true ;

#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException(
		"Sound::load failed: no SDL_mixer support available." ) ;

#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



bool Sound::unload()
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Sound::unload failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content->_samples != 0 )
		CacheProtectedDelete( _content->_samples ) ;

	delete _content ;


#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER

	::Mix_FreeChunk( _content ) ;
	// Already specified at the end of the method: _content = 0 ;

	/*
	 * Option A is false (see comment in Sound::load):
	 *

	try
	{

		Utils::deleteDataStream( *_dataStream ) ;


	}
	catch( const OSDL::Exception & e )
	{

		throw Ceylan::LoadableException( "Sound::unload failed: "
			+ e.toString() ) ;
	}

	 *
	 */

	_dataStream = 0 ;

#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException(
		"Sound::unload failed: no SDL_mixer support available." ) ;

#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

	_content = 0 ;

	return true ;

}





// Audible implementation.



Volume Sound::getVolume() const
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



void Sound::setVolume( Volume newVolume )
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


void Sound::play( PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::play for " + toString() ) ;

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw SoundException( "Sound::play failed: not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)


	try
	{

		CommandManager::GetExistingCommandManager().playSound( *this ) ;

	}
	catch( const CommandException & e )
	{

		throw SoundException( "Sound::play failed: " + e.toString() ) ;

	}


#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	// Returned value ignored:
	playReturnChannel( playCount ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}



ChannelNumber Sound::playReturnChannel( PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playReturnChannel for " + toString() ) ;

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
{

	LOG_DEBUG_AUDIO( "Sound::play for " + toString() ) ;

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
	PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playForAtMost for " + toString() ) ;

	// Returned value ignored:
	playForAtMostReturnChannel( maxDuration, playCount ) ;

}



ChannelNumber Sound::playForAtMostReturnChannel(
	Ceylan::System::Millisecond maxDuration, PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playForAtMostReturnChannel for " + toString() ) ;

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
{

	LOG_DEBUG_AUDIO( "Sound::playForAtMost for " + toString() ) ;

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
	PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeIn for " + toString() ) ;

	// Returned value ignored:
	playWithFadeInReturnChannel( fadeInMaxDuration, playCount ) ;


}



ChannelNumber Sound::playWithFadeInReturnChannel(
	Ceylan::System::Millisecond fadeInMaxDuration, PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeInReturnChannel for " + toString() ) ;

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
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeIn for " + toString() ) ;

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
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeInForAtMost for " + toString() ) ;

	// Returned value ignored:
	playWithFadeInForAtMostReturnChannel( playbackMaxDuration,
		fadeInMaxDuration, playCount ) ;

}



ChannelNumber Sound::playWithFadeInForAtMostReturnChannel(
	Ceylan::System::Millisecond playbackMaxDuration,
	Ceylan::System::Millisecond fadeInMaxDuration,
	PlaybackCount playCount )
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeInForAtMostReturnChannel for "
	  + toString() ) ;

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
{

	LOG_DEBUG_AUDIO( "Sound::playWithFadeInForAtMost for "
	  + toString() ) ;

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



const string Sound::toString( Ceylan::VerbosityLevels level ) const
{

	if ( level == Ceylan::low )
		return ( hasContent() ? string( "Loaded" ) : string( "Not loaded" ) )
			+ " sound, whose content path is '" + _contentPath + "'" ;


	try
	{

		if ( hasContent() )
		{

			Volume v = getVolume() ;

			return "Loaded sound whose volume is "
				+ Ceylan::toNumericalString( v )
				+ " (" + Ceylan::toString( 100 * v
					/ ( AudioModule::MaxVolume - AudioModule::MinVolume ) )
				+ "%)" ;

		}
		else
		{

			return "Sound currently not loaded, whose content path is '"
				+ _contentPath + "'" ;

		}

	}
	catch(  const Ceylan::Exception & e )
	{

		return "Sound::toString failed (abnormal): " + e.toString() ;

	}

}
