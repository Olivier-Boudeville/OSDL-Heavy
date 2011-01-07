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


#include "OSDLMusic.h"               // implies including Ceylan.h

#include "OSDLAudio.h"               // for AudioModule
#include "OSDLFileTags.h"            // for music file tag
#include "OSDLUtils.h"               // for createDataStreamFrom and al


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
 * SDL_rwops, on all PC-like platforms (including Windows and most UNIX),
 * the supported formats were WAVE, MOD, MIDI, OGG, and MP3.
 * OGG and, to a lesser extent, WAVE, were recommended for musics. 
 * 
 * Now, we want to be able to read musics from opaque archives (using
 * EmbeddedFile instances, thus PhysicsFS), and SDL_mixer just allows the
 * OGG (OggVorbis) file format to be used with the underlying SDL_rwops 
 * (MikMod could be supported as well).
 *
 * So, on the PC, one should only use OggVorbis-based musics now.
 *
 * @see Mix_LoadMUS_RW
 *
 *
 * - with SDL_mixer:
 * Mix_SetMusicCMD, Mix_HookMusic, Mix_GetMusicHookData not used, as no 
 * need felt.
 *
 * - with the DS:
 * 
 * MP3-encoded data is read from file (using libfat), on the ARM9, in a double
 * buffer whose halves are located in memory one after the other.
 *
 * When a playback is requested, the ARM9 sends to the ARM7 an IPC command that
 * specify the address of this encoded buffer, which contains MP3 frames. 
 *
 * As soon as the ARM7 starts reading a half buffer, it triggers back to the
 * ARM9 an IPC command, so that the ARM9 starts refilling the other buffer at
 * once (actually the ARM9 performs it asynchronously, as I/O operations should
 * not be done in an IRQ handler, thus the handler just sets a bit that will be
 * read and taken into account from the ARM9 main loop).
 *
 * The ARM7 reads whole chunks of memory (one MP3 frame after the other), and 
 * one has to ensure that, when arriving at the end of the second half buffer, 
 * it will be still able to read a whole frame, as encoded data is read without
 * knowing the boundaries of the MP3 frames (which are not fixed-size).
 * Thus the ARM7 must stop reading no later than: 
 * (end of buffer)-(delta) with delta = max size of an MP3 frame.
 * Looking at Helix example:
 * https://helixcommunity.org/viewcvs/datatype/mp3/codec/fixpt/testwrap/main.c?view=markup
 * 
 * they retain delta = 2*MAINBUF_SIZE, with MAINBUF_SIZE defined in mp3dec.h
 * as 1940, thus delta = 3880. 
 * So as long as the read pointer in encoding buffer is below boundary
 * B = (end of buffer)-(delta), at least one more frame can be decoded.
 * As soon as the delta boundary is crossed, the remaining bytes 
 * R = (end of buffer) - (last position + last frame size) with R < delta  
 * cannot be assumed to contain a whole frame, thus they have to be copied back
 * to the beginning at the encoded buffer. To do so, the first half must have
 * been refilled not from the buffer start, but from S = (buffer start) + delta.
 * The actual buffer start will be A = (buffer start) + delta - R.
 * The move of R bytes must be as fast as possible, as it is directly in the
 * critical path, R must thus be minimized.
 *
 */



extern const BitrateType OSDL::Audio::CBR = 1 ;
extern const BitrateType OSDL::Audio::VBR = 2 ;



MusicException::MusicException( const string & reason ) :
	AudibleException( reason )
{

}


	
MusicException::~MusicException() throw()
{

}




/// Starts with no current music:
Music * Music::_CurrentMusic = 0 ;




#if OSDL_ARCH_NINTENDO_DS

const OSDL::CommandManagerSettings * Music::_CommandManagerSettings = 0 ;

#endif // OSDL_ARCH_NINTENDO_DS




Music::Music( const std::string & musicFilename, bool preload ) :
	Audible( /* nothing loaded yet, hence not converted */ false ),	
	Ceylan::LoadableWithContent<LowLevelMusic>( musicFilename ),
	 _dataStream( 0 ),
	_isPlaying( false )
{

	if ( ! AudioModule::IsAudioInitialized() )
		throw MusicException( "Music constructor failed: "
			"audio module not already initialized" ) ;

	// Music is streamed on the DS, preloading cannot be chosen.
		 
#if ! OSDL_ARCH_NINTENDO_DS

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

#endif // OSDL_ARCH_NINTENDO_DS
			
}



Music::~Music() throw()
{

	if ( _CurrentMusic == this )
	{

		try
		{	
			manageNoMoreCurrent() ;
		
		}
		catch( const AudioException & e )
		{
		
			LogPlug::error( "Music destructor failed "
				"while unsetting 'current music' status: " + e.toString() ) ;
				
		}
				
	}
	
	
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
	
	//Ceylan::checkpoint( "Music deallocated." ) ;
	
	//LogPlug::trace( "Music deallocated." ) ;
	
}





// LoadableWithContent template instanciation.



bool Music::load()
{
	
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Music::load failed: "
		"not supported on the ARM7" ) ;
		
#elif defined(OSDL_RUNS_ON_ARM9)

	/*
	 * Preparing MP3 encoded streaming from the ARM9.
	 *
	 * Loading is to be performed only when about to play.
	 *
	 */
	 
	/*
	 * We know here that a command manager is available (implied by the audio
	 * module availability, checked by the Music constructor).
	 *
	 */
	
	// _dataStream not used here.
	
	_content = new LowLevelMusic() ;

	LowLevelMusic & music = *_content ;
	
		
	try
	{

		
		// Inits all _content members here, some settings are hardcoded:
		
		music._musicFile = & File::Open( _contentPath ) ;

		// First check the OSDL music tag:
		FileTag readTag = music._musicFile->readUint16() ;
		
		if ( readTag != MusicTag )
			throw Ceylan::LoadableException( 
				"Music::load: expected music tag not found ("
				+ Ceylan::toString( MusicTag ) + "), read instead "
				+ Ceylan::toString( readTag ) + ", which corresponds to: "
				+ DescribeFileTag( readTag ) ) ;
		
		LOG_DEBUG_AUDIO( "Music::load: correct tag found." ) ;
			
		// Frequency:
		music._frequency = music._musicFile->readUint16() /* Hz */ ;
				
		// Always 16-bit:		
		music._bitDepth	= 16 /* bits */ ;
		
		// Most probably mono:
		music._mode	= music._musicFile->readUint16() ;
		
		// Bitrate type (CBR/VBR) not used:
		music._musicFile->readUint8() ;
		
		music._size = music._musicFile->size() ; 

		// Upper bound, as determined by the getMP3Settings program:
		music._frameSizeUpperBound = music._musicFile->readUint16() ;


		// _CommandManagerSettings already created at audio module startup:
		if ( _CommandManagerSettings->_bufferSize <=
				music._frameSizeUpperBound )
			throw Ceylan::LoadableException( "Music::load: "
				"buffer to small compared to frame size upper bound." ) ;
					
		
		music._startAfterDelta = _CommandManagerSettings->_doubleBuffer 
			+ music._frameSizeUpperBound ;
		
		music._firstActualRefillSize = 
			_CommandManagerSettings->_bufferSize - music._frameSizeUpperBound ;	
			
		/*
		 * First buffer will be filled now, second after a corresponding ARM7
		 * request:
		 *
		 */
		fillFirstBuffer() ;
					
		music._requestFillOfSecondBuffer = true  ;
		
		// Not to let it uninitialized:
		music._playbackCount = 1 ;
		
	}
	catch( const Ceylan::System::SystemException & e )
	{
	
		throw Ceylan::LoadableException( "Music::load failed: "
			+ e.toString() ) ;
	
	}

	LOG_DEBUG_AUDIO( "Music::load: read first chunk of " + _contentPath + 
		+ " (" + Ceylan::toString( music._firstActualRefillSize) 
		+ " out of a total of "	+ Ceylan::toString( music._size ) 
		+ " bytes)." ) ;
	
	
	_convertedToOutputFormat = true ;

	return true ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER


	/*
	 * Supported: OggVorbis only. WAV and MP3 *not* supported for musics
	 * when using SDL_rwops with SDL_mixer's (undocumented) Mix_LoadMUS_RW.
	 *
	 */
    
	if ( hasContent() )
		return false ;

	try
	{

        /*
         * Not used anymore, so that musics can be loaded from embedded files
         * as well:
        _content = ::Mix_LoadMUS( FindAudiblePath( _contentPath ).c_str() ) ;
         *

		string musicFile = FindAudiblePath( _contentPath ) ;
		_content = ::Mix_LoadMUS_RW( 
        	SDL_RWFromFile( musicFile.c_str(), "rb" ) ) ;          

         */

        Ceylan::System::File & musicFile = File::Open( _contentPath ) ;
        
		/*
        LogPlug::trace( "loaded " + musicFile.toString() + ", size = "
        	+ Ceylan::toString( musicFile.size() ) ) ;
         */
		 
		// Creates the appropriate SDL_rwops and feeds it to SDL_mixer:

		_dataStream = & Utils::createDataStreamFrom( musicFile ) ;

		_content = ::Mix_LoadMUS_RW( _dataStream ) ;

		// _content checked afterwards.
	
	}
	catch( const Ceylan::Exception & e )
	{
	
		throw Ceylan::LoadableException( "Music::load failed: "
			"unable to load from '" + _contentPath + "': " + e.toString() ) ;
			
	}	

	if ( _content == 0 )
		throw Ceylan::LoadableException( "Music::load failed: "
			+ string( ::Mix_GetError() ) ) ;
	
	_convertedToOutputFormat = true ;
	
	return true ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Music::load failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



bool Music::unload()
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Music::unload failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	// Closes automatically the file if needed:
	if ( _content->_musicFile != 0 )
		delete _content->_musicFile ;
	
	delete _content ;
	
#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS
	
#if OSDL_USES_SDL_MIXER

	//Ceylan::checkpoint( "Music::unload: Mix_FreeMusic." ) ;
	
	// Deallocation of SDL_RWops inspired from playmus.c:
	::Mix_FreeMusic( _content ) ;
	
	// Already specified at the end of the method: _content = 0 ;
	
	/*
	 * Removed as the resulting SDL_FreeRW triggers a double free,
	 * for musics SDL_mixer apparently removes by itself the SDL_rwops:
	 *
	 * 
	 
	try
	{
		
		Utils::deleteDataStream( *_dataStream ) ;
		
	
	}
	catch( const OSDL::Exception & e )
	{
	
		throw Ceylan::LoadableException( "Music::unload failed: "
			+ e.toString() ) ;
	}
	
	*/
	
	_dataStream = 0 ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Music::unload failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

	_content = 0 ;
	
	return true ;

}





// Audible implementation.



Volume Music::getVolume() const
{

#if OSDL_USES_SDL_MIXER
	
	try
	{
	
		if ( hasContent() )
			return::Mix_VolumeMusic( /* just read it */ -1 ) ;
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



void Music::setVolume( Volume newVolume )
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( 
		"Music::setVolume failed: not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	
	_CommandManagerSettings->_commandManager->setMusicVolume( newVolume ) ;
	

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS


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

#endif // OSDL_ARCH_NINTENDO_DS

}



MusicType Music::getType() const
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


void Music::play( PlaybackCount playCount )
{
	
	if ( playCount == 0 )
		return ;
	
	_isPlaying = true ;
	
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::play failed: not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( hasContent() )
		unload() ;
		
	load() ;
	
	_content->_playbackCount = playCount ;

	try
	{

		if ( _content->_playbackCount != Loop )
			_content->_playbackCount-- ;
			
		_CommandManagerSettings->_commandManager->playMusic( *this ) ;
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::play failed: " + e.toString() ) ;
		
	}
	
	return ;
		
#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER
			
	if ( ! hasContent() )
		throw MusicException( "Music::play failed: "
			"no loaded music available" ) ;
			
	if ( ::Mix_PlayMusic( _content, GetLoopsForPlayCount( playCount ) ) == -1 )
		throw MusicException( "Music::play failed: "
			+ string( ::Mix_GetError() ) ) ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( 
		"Music::play failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}




// Play with fade-in subsection.


void Music::playWithFadeIn( Ceylan::System::Millisecond fadeInMaxDuration,
	PlaybackCount playCount )
{

	_isPlaying = true ;

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( 
		"Music::playWithFadeIn failed: not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( hasContent() )
		unload() ;
		
	load() ;
	
	_content->_playbackCount = playCount ;

	try
	{

		if ( _content->_playbackCount != Loop )
			_content->_playbackCount-- ;
			
		_CommandManagerSettings->_commandManager->playMusicWithFadeIn( *this,
			fadeInMaxDuration ) ;
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::playWithFadeIn failed: " 
			+ e.toString() ) ;
		
	}
	
	return ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS


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

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void Music::playWithFadeInFromPosition( 
	Ceylan::System::Millisecond fadeInMaxDuration,
	MusicPosition position, PlaybackCount playCount ) 
{

	_isPlaying = true ;

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



bool Music::isPlaying()
{

	return _isPlaying ;
	
}



void Music::pause()
{

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::pause failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	try
	{
		if ( hasContent() )	
			_CommandManagerSettings->_commandManager->pauseMusic() ;
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::pause failed: " + e.toString() ) ;
		
	}
	
	return ;

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_PauseMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::pause failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::unpause()
{

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::unpause failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)


	try
	{
		
		if ( hasContent() )	
			_CommandManagerSettings->_commandManager->unpauseMusic() ;
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::unpause failed: " + e.toString() ) ;
		
	}
	
	return ;

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_ResumeMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::unpause failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::rewind()
{

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	::Mix_RewindMusic() ;		
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::rewind failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

}

	
																				
void Music::setPosition( MusicPosition newPosition )
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



void Music::stop()	
{

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::stop failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)


	try
	{
		
		if ( hasContent() )
		{
		
			_content->_playbackCount = 0 ;	
			_CommandManagerSettings->_commandManager->stopMusic() ;
		
		}	
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::stop failed: " + e.toString() ) ;
		
	}
	
	return ;

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER

	//Ceylan::checkpoint( "Music::stop." ) ;
		
	// No test performed about music availability:
	::Mix_HaltMusic() ;
			
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::stop failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::fadeIn( Ceylan::System::Millisecond fadeInMaxDuration ) 
{

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::fadeIn failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	_CommandManagerSettings->_commandManager->fadeInMusic( fadeInMaxDuration ) ;

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS

	// No::Mix_FadeInMusic, even with OSDL_USES_SDL_MIXER.

	throw MusicException( "Music::fadeIn failed: "
		"not available on this platform." ) ;
	
#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::fadeOut( Ceylan::System::Millisecond fadeOutMaxDuration ) 
{

#if OSDL_ARCH_NINTENDO_DS
		
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::fadeOut: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	_CommandManagerSettings->_commandManager->fadeOutMusic( 
		fadeOutMaxDuration ) ;

#endif // OSDL_RUNS_ON_ARM7


#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER
	
	// No test performed about music availability:
	if ( ::Mix_FadeOutMusic( fadeOutMaxDuration ) == 0 )
		throw MusicException( "Music::fadeOut failed: "
			+ string( ::Mix_GetError() ) ) ;
				
#else // OSDL_USES_SDL_MIXER

	throw MusicException( "Music::fadeOut failed: "
		"no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::setAsCurrent()
{

	// A music can be repeated:
	if ( _CurrentMusic != 0 && _CurrentMusic != this )
		_CurrentMusic->manageNoMoreCurrent() ;
	
	/*
	 * The command manager (the caller) is expected to update its cached 
	 * current music as well:
	 *
	 */	
	_CurrentMusic = this ;	

	LOG_DEBUG_AUDIO( "Music::setAsCurrent: " + toString( Ceylan::low ) +
		" set as current." ) ;	
		
}



void Music::requestFillOfFirstBuffer()
{

#if ! OSDL_USES_SDL_MIXER

	_content->_requestFillOfFirstBuffer = true ;

#endif // OSDL_USES_SDL_MIXER
	
}



void Music::requestFillOfSecondBuffer()
{

#if ! OSDL_USES_SDL_MIXER

	_content->_requestFillOfSecondBuffer = true ;
	
#endif // OSDL_USES_SDL_MIXER

}



const string Music::toString( Ceylan::VerbosityLevels level ) const
{
	
	if ( level == Ceylan::low )
		return ( hasContent() ? string( "Loaded" ) : string( "Not loaded" ) )
			+ " music, whose content path is '" + _contentPath + "'" ;

	 
	try
	{
	
		if ( hasContent() )
		{
		
			Volume v = getVolume() ;
	
			return "Loaded music from content path '" + _contentPath 
				+ "', whose volume is " + Ceylan::toNumericalString( v )
				+ " (" + Ceylan::toNumericalString( 100 * v 
					/ ( AudioModule::MaxVolume - AudioModule::MinVolume ) )
				+ "%) and whose type is " + DescribeMusicType( getType() )
				+ string( ", currently " ) 
				+ ( _isPlaying ? "playing": "not playing" ) ;
			
		}
		else
		{
		
			return "Music currently not loaded, whose content path is '" 
				+ _contentPath + "'" ;
			
		}		
	
	}
	catch(  const Ceylan::Exception & e )
	{
	
		return "Music::toString failed (abnormal): " + e.toString() ;
		 
	}
	
}




#if OSDL_ARCH_NINTENDO_DS

void Music::SetCommandManagerSettings( const CommandManagerSettings & settings )
{

	_CommandManagerSettings = & settings ;
	
}

#endif // OSDL_ARCH_NINTENDO_DS



void Music::ManageCurrentMusic()
{

	if ( _CurrentMusic != 0 )
		_CurrentMusic->manageBufferRefill() ;
		
}





// Protected section.


MusicType Music::GetTypeOf( const Music * music )
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



string Music::DescribeMusicType( MusicType type )
{
	
	switch( type )
	{
	
		case Wave: 
			return "waveform audio format (WAVE/RIFF)" ;
			break ;
			
		case MOD:
			return "soundtrack Module (MOD)" ;
			break ;
			
		case MIDI: 
			return "Musical Instrument Digital Interface (MIDI)" ;
			break ;
			
		case OggVorbis: 
			return "Vorbis encoding over Ogg container (OggVorbis)" ;
			break ;
			
		case MP3: 
			return "MPEG-1 Audio Layer 3 (MP3)" ;
			break ;
						
		case CommandBased: 
			return "music managed externally by a third-party player" ;
			break ;
			
		case NoMusic: 
			return "no music" ;
			break ;
			
		default: 
			return "unknown music type (abnormal)" ;
			break ;
	
	
	}
	
	
}



string Music::DescribeBitrateType( BitrateType type )
{
	
	switch( type )
	{
	
		case CBR: 
			return "CBR (constant bitrate)" ;
			break ;
			
		case VBR:
			return "VBR (variable bitrate)" ;
			break ;
						
		default: 
			return "unknown bitrate type (abnormal)" ;
			break ;
	
	}
	
	
}



void Music::onPlaybackEnded()
{

	// Made to be overriden.
	LOG_TRACE_AUDIO( "Music::onPlaybackEnded" ) ;
		
}



void Music::managePlaybackEnded()
{
	
	_isPlaying = false ;
	
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::managePlaybackEnded: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)
		
	PlaybackCount count = _content->_playbackCount ;
	
	unload() ;
	onPlaybackEnded() ;
	
	LOG_DEBUG_AUDIO( "Music::managePlaybackEnded: count = " 
		+ Ceylan::toString(count) ) ;
		
	// Includes the -1 case (infinite); count already decremented:
	if ( count != 0 )
		play( count ) ;
	else
		manageNoMoreCurrent() ;
		
#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS

}



void Music::onNoMoreCurrent()
{

	LOG_WARNING_AUDIO( "Music::onNoMoreCurrent: " + toString( Ceylan::low ) 
		+ " not current anymore." ) ;
	
}



void Music::manageNoMoreCurrent()
{
	
	LOG_TRACE_AUDIO( "Music::manageNoMoreCurrent" ) ;
		
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::manageNoMoreCurrent: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	_isPlaying = false ;
	
	if ( _CurrentMusic == this )
	{
	
		_CurrentMusic = 0 ;
		_CommandManagerSettings->_commandManager->unsetCurrentMusic( *this ) ;
		
	}
	
#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	_isPlaying = false ;
	
	if ( _CurrentMusic == this )
		_CurrentMusic = 0 ;

#endif // OSDL_ARCH_NINTENDO_DS

	onNoMoreCurrent() ;
		
}



void Music::manageBufferRefill()
{
 
#if OSDL_ARCH_NINTENDO_DS

	if ( _content->_requestFillOfFirstBuffer )
		fillFirstBuffer() ;
	
	if ( _content->_requestFillOfSecondBuffer )
		fillSecondBuffer() ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}
 

		
void Music::fillFirstBuffer()
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::fillFirstBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)
		

	LOG_TRACE_AUDIO( "Music::fillFirstBuffer." ) ;

	LowLevelMusic & music = getContent() ;

	music._requestFillOfFirstBuffer = false ;			
	
	try
	{
			
		/*
		 * Start after the delta zone, left blank to allow for the move of the
		 * remainder at the end of second buffer:
		 */ 
		BufferSize readSize = music._musicFile->read( 
			/* start after delta zone */ music._startAfterDelta,
			/* max length */ music._firstActualRefillSize ) ;
		
		LOG_DEBUG_AUDIO( "Music::fillFirstBuffer: read " 
			+ Ceylan::toString( readSize ) + " bytes." ) ;
		 
		 		
		/*
		 * Zero-pad to avoid finding false sync word after last frame 
		 * (from previous data in this first buffer):
		 */
		if ( readSize < music._firstActualRefillSize )
		{
			
			LOG_TRACE_AUDIO( "Padding first buffer." ) ;
		
			::memset( 
				/* start */ music._startAfterDelta + readSize,
				/* filler */ 0, 
				/* length */ _CommandManagerSettings->_bufferSize 
					- music._frameSizeUpperBound - readSize ) ;
			
			// Flush the ARM9 data cache for the ARM7 (full first buffer):
			DC_FlushRange( (void*) music._startAfterDelta,
				music._firstActualRefillSize  ) ;
				
			try
			{	
			
	_CommandManagerSettings->_commandManager->notifyEndOfEncodedStreamReached()
		;
					
			}
			catch( const CommandException & e )
			{
			
				throw AudioException( "Music::fillFirstBuffer failed: "
					+ e.toString() ) ;
			
			}	
				    	
		}
		else
		{
					
			// Full or encoded data, flush the ARM9 data cache for the ARM7:
			DC_FlushRange( (void*) music._startAfterDelta,
				music._firstActualRefillSize ) ;
		
		}
		
		
	}
	catch( const SystemException & e )
	{
		
		throw AudioException( "Music::fillFirstBuffer failed: " 
			+ e.toString() ) ;
			
	}
	
#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void Music::fillSecondBuffer()
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::fillSecondBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)
	
	LOG_TRACE_AUDIO( "Music::fillSecondBuffer." ) ;
		
	LowLevelMusic & music = getContent() ;
	
	music._requestFillOfSecondBuffer = false ;
	
	try
	{
			
		/*
		 * Start at the half of double buffer to its end:
		 */ 
		BufferSize readSize = music._musicFile->read( 
			/* start */ _CommandManagerSettings->_secondBuffer,
			/* max length */ _CommandManagerSettings->_bufferSize ) ;
		
		LOG_DEBUG_AUDIO( "Music::fillSecondBuffer: read " 
			+ Ceylan::toString( readSize ) + " bytes." ) ;
		 
		/*
		 * Zero-pad to avoid finding false sync word after last frame 
		 * (from previous data in this first buffer):
		 */
		if ( readSize < _CommandManagerSettings->_bufferSize )
		{
			
			LOG_TRACE_AUDIO( "Padding second buffer." ) ;
		
			::memset( 
				/* start */ _CommandManagerSettings->_secondBuffer + readSize,
				/* filler */ 0, 
				/* length */_CommandManagerSettings->_bufferSize - readSize 
			) ;
				    		
					
			// Flush the ARM9 data cache for the ARM7:
			DC_FlushRange( (void*) _CommandManagerSettings->_secondBuffer,
				_CommandManagerSettings->_bufferSize  ) ;
				    	
			try
			{	
			
				_CommandManagerSettings->_commandManager->notifyEndOfEncodedStreamReached() ;
					
			}
			catch( const CommandException & e )
			{
			
				throw AudioException( "Music::fillSecondBuffer failed: "
					+ e.toString() ) ;
			
			}	
		
		}
		else
		{

			// Flush the ARM9 data cache for the ARM7:
			DC_FlushRange( (void*) _CommandManagerSettings->_secondBuffer,
				_CommandManagerSettings->_bufferSize  ) ;
		
		}
				
	
	}
	catch( const SystemException & e )
	{
		
		throw AudioException( "Music::fillSecondBuffer failed: " 
			+ e.toString() ) ;
			
	}
	
#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS
	
}

