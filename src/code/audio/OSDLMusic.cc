#include "OSDLMusic.h"

#include "OSDLAudio.h"               // for AudioModule



#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS

#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#include "OSDLCommandManager.h"      // for CommandManager

#endif // OSDL_ARCH_NINTENDO_DS



#include "Ceylan.h"                  // for prepareFIFOCommand, etc.


#if OSDL_USES_SDL
#include "SDL.h"                     // for SDL_InitSubSystem
#endif // OSDL_USES_SDL

#if OSDL_USES_SDL_MIXER
#include "SDL_mixer.h"               // for Mix_OpenAudio and al
#endif // OSDL_USES_SDL_MIXER



using std::string ;

using Ceylan::System::Millisecond ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Audio ;


/**
 * Implementation notes:
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



MusicException::MusicException( const string & reason ) throw():
	AudibleException( reason )
{

}


	
MusicException::~MusicException() throw()
{

}



/// Starts with no current music:
Music * Music::_CurrentMusic = 0 ;




Music::Music( const std::string & musicFile, bool preload ) 
		throw( MusicException ):
	Audible( /* nothing loaded yet, hence not converted */ false ),	
	Ceylan::LoadableWithContent<LowLevelMusic>( musicFile ),
	_isPlaying( false )
{

	if ( ! AudioModule::IsAudioInitialized() )
		throw MusicException( "Music constructor failed: "
			"audio module not already initialized" ) ;
			 
#if OSDL_ARCH_NINTENDO_DS

	// Music is streamed on the DS, preloading or not cannot be chosen:
	if ( true )
	
#else // OSDL_ARCH_NINTENDO_DS

	if ( preload )

#endif // OSDL_ARCH_NINTENDO_DS
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
			
}



Music::~Music() throw()
{

	if ( _CurrentMusic == this )
	{

		try
		{	
			onNoMoreCurrent() ;
		
		}
		catch( const AudioException & e )
		{
		
			LogPlug::error( "Music destructor failed "
				"while unsetting 'current music' status: " + e.toString() ) ;
				
		}
		
		_CurrentMusic = 0 ;
		
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
	
}




// LoadableWithContent template instanciation.


bool Music::load() throw( Ceylan::LoadableException )
{

	if ( hasContent() )
		return false ;
		
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Sound::load failed: "
		"not supported on the ARM7" ) ;
		
#elif defined(OSDL_RUNS_ON_ARM9)

	// Preparing MP3 encoded streaming from the ARM9:
	
	_requestFillOfFirstBuffer  = false ;
	_requestFillOfSecondBuffer = true  ;
	
	try
	{

		_content = new LowLevelMusic() ;
		
		// Inits all _content members here, settings are hardcoded here:
		
		_content->_musicFile = & File::Open( _contentPath ) ;
		
		_content->_frequency = 22050 /* kHz */ ;	
		
		_content->_bitDepth	= 16 /* bits */ ;
		
		_content->_mode	= /* mono */ 1 ;
		
		_content->_size = _content->_musicFile->size() ; 
		
		// Could be optimized according to libfat cache size:
		_content->_bufferSize = 4096 ;
		//_content->_bufferSize = 4096 * 8 ;

		// Upper bound might be reduced ? Value used is from Helix example.
		// On a sample mp3, had 314 bytes
		//_content->_frameSizeUpperBound = 2 * 1940 ;
		_content->_frameSizeUpperBound = 350 ;
		
		if ( _content->_bufferSize <= _content->_frameSizeUpperBound )
			throw Ceylan::LoadableException( "Music::load: "
				"buffer to small compared to frame size upper bound." ) ;
					
		// Force music buffer to match the boundaries of ARM9 cache line:		
		_content->_doubleBuffer = 
			CacheProtectedNew( _content->_bufferSize * 2 ) ;
		
		_content->_startAfterDelta = 
			_content->_doubleBuffer + _content->_frameSizeUpperBound ;
		
		_content->_firstActualRefillSize = 
			_content->_bufferSize - _content->_frameSizeUpperBound ;			
			
		// Let's start by loading a first half:
		fillFirstBuffer() ;
		
		_content->_secondBuffer = _content->_doubleBuffer 
			+ _content->_bufferSize ;
			
		_content->_secondFilled = false ;
	
	}
	catch( const Ceylan::System::SystemException & e )
	{
	
		throw Ceylan::LoadableException( "Sound::load failed: "
			+ e.toString() ) ;
	
	}

	LogPlug::debug( "Music::load: read first chunk of " + _contentPath + 
		+ " (" + Ceylan::toString( _content->_firstActualRefillSize) 
		+ " out of a total of "	+ Ceylan::toString( _content->_size ) 
		+ " bytes)." ) ;
	
	
	_convertedToOutputFormat = true ;

	return true ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

#if OSDL_USES_SDL_MIXER

	try
	{
		// Misleading, supports WAVE but other formats as well:
		_content = ::Mix_LoadMUS( FindAudiblePath( _contentPath ).c_str() ) ;
	
	}
	catch( const AudibleException & e )
	{
	
		throw Ceylan::LoadableException( "Music::load failed: '"
			"unable to locate '" + _contentPath + "': " + e.toString() ) ;
			
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



bool Music::unload() throw( Ceylan::LoadableException )
{

	if ( ! hasContent() )
		return false ;

	// There is content to unload here:
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw Ceylan::LoadableException( "Music::unload failed: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content->_doubleBuffer != 0 )
		CacheProtectedDelete( _content->_doubleBuffer ) ;
	
	// Closes automatically the file if needed:
	if ( _content->_musicFile != 0 )
		delete _content->_musicFile ;
	
	delete _content ;
	
#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS
	
#if OSDL_USES_SDL_MIXER

	::Mix_FreeMusic( _content ) ;
	
#else // OSDL_USES_SDL_MIXER

	throw Ceylan::LoadableException( 
		"Music::unload failed: no SDL_mixer support available." ) ;
	
#endif // OSDL_USES_SDL_MIXER

#endif // OSDL_ARCH_NINTENDO_DS

	_content = 0 ;
	
	return true ;

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

	_isPlaying = true ;
	
#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw MusicException( "Music::play failed: not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	try
	{
	
		CommandManager::GetExistingCommandManager().playMusic( *this ) ;
		
	}
	catch( const CommandException & e )
	{
	
		throw MusicException( "Music::play failed: " + e.toString() ) ;
		
	}
	
		
#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

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

#endif // OSDL_ARCH_NINTENDO_DS

}



// Play with fade-in subsection.


void Music::playWithFadeIn( Ceylan::System::Millisecond fadeInMaxDuration,
	PlaybackCount playCount ) throw( AudibleException )
{

	_isPlaying = true ;

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



bool Music::isPlaying() throw()
{

	return _isPlaying ;
	
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



void Music::setAsCurrent() throw( AudioException )
{

	if ( _CurrentMusic != 0 )
		_CurrentMusic->onNoMoreCurrent() ;
		
	_CurrentMusic = this ;	

	LogPlug::debug( "Music::setAsCurrent: " + toString( Ceylan::low) +
		" set as current." ) ;	
		
}



void Music::requestFillOfFirstBuffer() throw() 
{

	_requestFillOfFirstBuffer = true ;
	
}



void Music::requestFillOfSecondBuffer() throw()
{

	_requestFillOfSecondBuffer = true ;
	
}



const string Music::toString( Ceylan::VerbosityLevels level ) const throw()
{
	
	if ( level == Ceylan::low )
		return "Music read from '" + _contentPath + "'" ;

	// @todo Manage _isPlaying
	 
	try
	{
	
		if ( hasContent() )
		{
		
			Volume v = getVolume() ;
	
			return "Loaded music whose volume is " + Ceylan::toString( v )
				+ "(" + Ceylan::toNumericalString( 100 * v 
					/ ( AudioModule::MaxVolume - AudioModule::MinVolume ) )
				+ "%) and whose type is " + DescribeMusicType( getType() ) ;
			
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



void Music::ManageCurrentMusic() throw( AudioException )
{

	if ( _CurrentMusic != 0 )
		_CurrentMusic->manageBufferRefill() ;
		
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



string Music::DescribeMusicType( MusicType type ) throw( AudioException )
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



void Music::onPlaybackEnded() throw( AudioException )
{

	LogPlug::trace( "Music::onPlaybackEnded" ) ;
	
	_isPlaying = false ;
	
}



void Music::onNoMoreCurrent() throw( AudioException )
{

	LogPlug::warning( "Music::onNoMoreCurrent: " + toString( Ceylan::low ) 
		+ " not current anymore." ) ;
	
}



void Music::manageBufferRefill() throw( AudioException )
{
 
	if ( _requestFillOfFirstBuffer )
	{
	
		_requestFillOfFirstBuffer = false ;
		fillFirstBuffer() ;
		
	}
	
	
	if ( _requestFillOfSecondBuffer )
	{
	
		_requestFillOfSecondBuffer = false ;
		fillSecondBuffer() ;
		
	}
	
}
 
 
 
/*
Ceylan::Byte * Music::getFirstBuffer() throw( AudioException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::getFirstBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content != 0 )
		return _content->_doubleBuffer ;
	else
		throw AudioException( "Music::getFirstBuffer: no content available" ) ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	throw AudioException( "Music::getFirstBuffer: "
		"not supported on this platform" ) ;
	
#endif // OSDL_ARCH_NINTENDO_DS
	
}



BufferSize Music::getAvailableSizeInFirstBuffer() throw( AudioException )
{

	// fixme use _firstFilled ?

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::getAvailableSizeInFirstBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content != 0 )
		return _content->_availableInFirst ;
	else
		throw AudioException( 
			"Music::getAvailableSizeInFirstBuffer: no content available" ) ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	throw AudioException( "Music::getAvailableSizeInFirstBuffer: "
		"not supported on this platform" ) ;
	
#endif // OSDL_ARCH_NINTENDO_DS
	
}


					
Ceylan::Byte * Music::getSecondBuffer() throw( AudioException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::getSecondBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content != 0 )
		return _content->_secondBuffer ;
	else
		throw AudioException( "Music::getSecondBuffer: no content available" ) ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	throw AudioException( "Music::getSecondBuffer: "
		"not supported on this platform" ) ;
	
#endif // OSDL_ARCH_NINTENDO_DS
	
}



BufferSize Music::getAvailableSizeInSecondBuffer() throw( AudioException )
{

	// fixme use _firstFilled ?

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::getAvailableSizeInSecondBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)

	if ( _content != 0 )
		return _content->_availableInSecond ;
	else
		throw AudioException( 
			"Music::getAvailableSizeInSecondBuffer: no content available" ) ;

#endif // OSDL_RUNS_ON_ARM7

#else // OSDL_ARCH_NINTENDO_DS

	throw AudioException( "Music::getAvailableSizeInSecondBuffer: "
		"not supported on this platform" ) ;
	
#endif // OSDL_ARCH_NINTENDO_DS
	
}

*/		


		
void Music::fillFirstBuffer() throw( AudioException )
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::fillFirstBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)
		
	LogPlug::trace( "Music::fillFirstBuffer." ) ;

	LowLevelMusic & music = getContent() ;
	
	try
	{
			
		/*
		 * Start after the delta zone, left blank to allow for the move of the
		 * remainder at the end of second buffer:
		 */ 
		BufferSize readSize = music._musicFile->read( 
			/* start after delta zone */ music._startAfterDelta,
			/* max length */ music._firstActualRefillSize ) ;
		
		/*
		 * Zero-pad to avoid finding false sync word after last frame 
		 * (from previous data in this first buffer):
		 */
		if ( readSize < music._firstActualRefillSize )
		{
		
			::memset( 
				/* start */ music._startAfterDelta + readSize,
				/* filler */ 0, 
				/* length */ music._bufferSize - (BufferSize)
					( music._startAfterDelta + readSize )
			) ;
			
			// Flush the ARM9 data cache for the ARM7 (full first buffer):
			DC_FlushRange( (void*) music._startAfterDelta,
				music._firstActualRefillSize  ) ;
				
			try
			{	
			
				CommandManager & commandManager =
					CommandManager::GetExistingCommandManager() ;
			
				// Here we know the stream has been fully read:
				commandManager.notifyEndOfEncodedStreamReached() ;
				
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



void Music::fillSecondBuffer() throw( AudioException )
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw AudioException( "Music::fillSecondBuffer: "
		"not supported on the ARM7" ) ;

#elif defined(OSDL_RUNS_ON_ARM9)
	
	LogPlug::trace( "Music::fillSecondBuffer." ) ;
		
	LowLevelMusic & music = getContent() ;
	
	try
	{
			
		/*
		 * Start at the half of double buffer to its end:
		 */ 
		BufferSize readSize = music._musicFile->read( 
			/* start */ music._secondBuffer,
			/* max length */ music._bufferSize ) ;
		
		/*
		 * Zero-pad to avoid finding false sync word after last frame 
		 * (from previous data in this first buffer):
		 */
		if ( readSize < music._bufferSize )
		{
		
			::memset( 
				/* start */ music._secondBuffer + readSize,
				/* filler */ 0, 
				/* length */ music._bufferSize - readSize 
			) ;
				    		
					
			// Flush the ARM9 data cache for the ARM7:
			DC_FlushRange( (void*) music._secondBuffer,	music._bufferSize  ) ;
				    	
			try
			{	
			
				CommandManager & commandManager =
					CommandManager::GetExistingCommandManager() ;
			
				commandManager.notifyEndOfEncodedStreamReached() ;
				
			}
			catch( const CommandException & e )
			{
			
				throw AudioException( "Music::fillFirstBuffer failed: "
					+ e.toString() ) ;
			
			}	
		
		}
		else
		{

			// Flush the ARM9 data cache for the ARM7:
			DC_FlushRange( (void*) music._secondBuffer,	music._bufferSize  ) ;
		
		}
				
	
	}
	catch( const SystemException & e )
	{
		
		throw AudioException( "Music::fillSecondBuffer failed: " 
			+ e.toString() ) ;
			
	}
	
#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS
	
}

