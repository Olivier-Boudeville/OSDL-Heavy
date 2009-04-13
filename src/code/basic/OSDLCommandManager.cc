/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#include "OSDLCommandManager.h"


#include "OSDLSound.h"               // for Sound
#include "OSDLMusic.h"               // for Music

#include "OSDLIPCCommands.h"         

#include "OSDLARM7Codes.h"           // for Helix error codes


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS





using namespace OSDL ;
using namespace OSDL::Audio ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;


using std::string ;


CommandManager * CommandManager::_IPCManager = 0 ;



CommandException::CommandException( const string & reason ) throw() : 
	OSDL::Exception( reason )
{

}	



CommandException::~CommandException() throw()
{

}	



CommandManager::CommandManager() throw( CommandException ):
	FIFO(),
	_currentMusic(0),
	_doubleBuffer(0),
	_bufferSize(4096 * 2),
	_settings(0)
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9

	// _bufferSize could be optimized according to libfat cache size.
	enableMusicSupport() ;
	
	if ( _IPCManager != 0 )
		throw CommandException( "CommandManager constructor failed: "
			"there is already a CommandManager instance registered" ) ;


	// Last action is to register this manager:
	_IPCManager = this ;		

	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager constructor failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager constructor failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



CommandManager::~CommandManager() throw()	
{

	// _currentMusic not to be managed by the command manager (cached value).
	
	disableMusicSupport() ;
		
}




// Audio section.


void CommandManager::playSound( Audio::Sound & sound ) throw( CommandException )
{

	if ( ! sound.hasContent() )
		throw CommandException( "CommandManager::playSound failed: "
			"specified sound has no content." ) ;
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
		
	/*
	 * We hereby suppose the Ceylan FIFO safe mode is deactivated, hence only
	 * the first byte of the command element is used.
	 *
	 */
	FIFOElement commandElement = prepareFIFOCommand( PlaySoundRequest ) ;

	const Audio::LowLevelSound & actualSound = sound.getContent() ;
	
	/*
	 * Add bit depth in first four bits of the second byte, encoded that way:
	 *   0b0000 = 0x0: IMA ADPCM (bit depth of 4 by convention)
	 *   0b0001 = 0x1: 8 bit
	 *   0b0010 = 0x2: 16 bit
	 *
	 */
	 
	// if ( actualSound._bitDepth == 4 ): nothing to do (already zero)
	if ( actualSound._bitDepth == 8 )
		commandElement |= 0x00100000 ; 
	else if ( actualSound._bitDepth == 16 )
		commandElement |= 0x00200000 ; 


	// Add channel number in second four bits of the second byte:
	if ( actualSound._mode == /* Mono */ 1 )
		commandElement |= 0x00010000 ; 
	else // suppose Stereo (2):
		commandElement |= 0x00020000 ; 
	
	
	// Add the frequency in command last two bytes:
	commandElement |= sound.getContent()._frequency ;
	
	writeBlocking( commandElement ) ;

	// Sends the address of the sample buffer:
	writeBlocking( reinterpret_cast<FIFOElement>( 
		sound.getContent()._samples ) ) ;
		
	// Sends the size in bytes of the sample buffer:
	writeBlocking( static_cast<FIFOElement>( sound.getContent()._size ) ) ;
		
	SetEnabledInterrupts( previous ) ;
				
	notifyCommandToARM7() ;	
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::playSound failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::playSound failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}



void CommandManager::enableMusicSupport() throw( CommandException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Force music buffer to match the boundaries of ARM9 cache line:		
	_doubleBuffer = CacheProtectedNew( _bufferSize * 2 ) ;

	_settings = new CommandManagerSettings() ;

	_settings->_commandManager = this ; 
	_settings->_bufferSize = _bufferSize ; 
	_settings->_doubleBuffer = _doubleBuffer ; 
	_settings->_secondBuffer = _doubleBuffer + _bufferSize ; 
	
	Music::SetCommandManagerSettings( *_settings ) ;
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::enableMusicSupport failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::enableMusicSupport failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}



void CommandManager::disableMusicSupport() throw( CommandException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	if ( _doubleBuffer != 0 )
	{
		
		CacheProtectedDelete( _doubleBuffer ) ;
		_doubleBuffer = 0 ;
	
	}	

	if ( _settings != 0 )
	{
	
		delete _settings ;
		_settings = 0 ;
		
	}
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::disableMusicSupport failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::disableMusicSupport failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}



BufferSize CommandManager::getMusicBufferSize() const throw()
{

	return _bufferSize ;
	
}



Ceylan::Byte * CommandManager::getMusicBuffer() const 
	throw( CommandException )
{

	if ( _doubleBuffer == 0 )
		throw CommandException( "CommandManager::getMusicBuffer failed: "
			"no available buffer." ) ;
			
	return _doubleBuffer ;
	
}



void CommandManager::playMusic( Audio::Music & music ) throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	music.setAsCurrent() ;
	
	// Forget any past cached current music:
	_currentMusic = & music ;
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	LowLevelMusic & actualMusic = music.getContent() ;

	/*
	 * A boolean parameter is set in the command element: its last bit tells
	 * whether the playback should start from first buffer (if 1) or from
	 * second (if 0).
	 *
	 * The second element is a pointer to the cache-aligned sound buffer,
	 * third is composed of the full size of the encoded buffer (16 first bits),
	 * then the delta value (the MP3 frame size upper bound, 16 last bits).
	 *
	 */
	FIFOElement commandElement = prepareFIFOCommand( PlayMusicRequest ) ;

	// Uses last bit to tell to start with first buffer:
	writeBlocking( ( commandElement | 0x00000001 ) ) ;

	// Specifies to the ARM7 the address of encoded double buffer:
	writeBlocking( reinterpret_cast<FIFOElement>( _doubleBuffer ) ) ;

	// Sends the size of a half buffer and the one of the 'delta zone':
	writeBlocking( (FIFOElement) ( ( ( (Ceylan::Uint16) _bufferSize ) << 16 ) 
			| ( (Ceylan::Uint16) actualMusic._frameSizeUpperBound ) ) ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;

	/*
	 * Here the decoding will be triggered on the ARM7, the ARM9 main loop is
	 * responsible for the refilling of encoded buffers.
	 *
	 */
	 	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::playMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::playMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::playMusicWithFadeIn( Audio::Music & music,
		Ceylan::System::Millisecond fadeInMaxDuration ) 
	throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	/*
	 * Like playMusic, but immediately followed by a fade-in request: 
	 *
	 * @note Most probably the fade-in request will arrive too soon to use the
	 * actual sample rate, the default one might be always taken instead.
	 *
	 */
	
	music.setAsCurrent() ;
	
	// Forget any past cached current music:
	_currentMusic = & music ;
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	LowLevelMusic & actualMusic = music.getContent() ;

	/*
	 * A boolean parameter is set in the command element: its last bit tells
	 * whether the playback should start from first buffer (if 1) or from
	 * second (if 0).
	 *
	 * The second element is a pointer to the cache-aligned sound buffer,
	 * third is composed of the full size of the encoded buffer (16 first bits),
	 * then the delta value (the MP3 frame size upper bound, 16 last bits).
	 *
	 */
	FIFOElement commandElement = prepareFIFOCommand( PlayMusicRequest ) ;

	// Uses last bit to tell to start with first buffer:
	writeBlocking( ( commandElement | 0x00000001 ) ) ;

	// Specifies to the ARM7 the address of encoded double buffer:
	writeBlocking( reinterpret_cast<FIFOElement>( _doubleBuffer ) ) ;

	// Sends the size of a half buffer and the one of the 'delta zone':
	writeBlocking( (FIFOElement) ( ( ( (Ceylan::Uint16) _bufferSize ) << 16 ) 
			| ( (Ceylan::Uint16) actualMusic._frameSizeUpperBound ) ) ) ;

	
	// Now adding the fade-in (second command in a row):
	writeBlocking( prepareFIFOCommand( FadeInMusicRequest ) 
		| ( fadeInMaxDuration & 0x0000ffff ) ) ;
	
	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
	
	/*
	 * Here the decoding will be triggered on the ARM7, the ARM9 main loop is
	 * responsible for the refilling of encoded buffers.
	 *
	 */
	
	 	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::playMusicWithFadeIn failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::playMusicWithFadeIn failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::stopMusic() throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Anything to stop ?
	if ( _currentMusic == 0 )
	{

		LogPlug::warning( 
			"CommandManager::stopMusic: no current music to stop." ) ;

		return ;	

	}
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( prepareFIFOCommand( StopMusicRequest ) ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
		
	/*
	 * manageNoMoreCurrent and al will be called when the ARM7 notifies the
	 * end of playback.
	 *
	 */
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::stopMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::stopMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::fadeInMusic(
		Ceylan::System::Millisecond fadeInMaxDuration )
	throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Anything to stop ?
	if ( _currentMusic == 0 )
	{

		LogPlug::warning( 
			"CommandManager::fadeInMusic: no current music to fade-in." ) ;

		return ;	

	}
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( 
		prepareFIFOCommand( FadeInMusicRequest ) | fadeInMaxDuration ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
		
	/*
	 * manageNoMoreCurrent and al will be called when the ARM7 notifies the
	 * end of playback.
	 *
	 */
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::fadeInMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::fadeInMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::fadeOutMusic(
		Ceylan::System::Millisecond fadeOutMaxDuration)
	throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Anything to stop ?
	if ( _currentMusic == 0 )
	{

		LogPlug::warning( 
			"CommandManager::fadeOutMusic: no current music to fade-out." ) ;

		return ;	

	}
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( 
		prepareFIFOCommand( FadeOutMusicRequest ) | fadeOutMaxDuration ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
		
	/*
	 * manageNoMoreCurrent and al will be called when the ARM7 notifies the
	 * end of playback.
	 *
	 */
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::fadeOutMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::fadeOutMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::setMusicVolume( Volume newVolume ) 
	throw( CommandException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( prepareFIFOCommand( SetMusicVolumeRequest ) | newVolume ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
		
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::setMusicVolume failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::setMusicVolume failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}


	
void CommandManager::pauseMusic() throw( CommandException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Anything to pause ?
	if ( _currentMusic == 0 )
		return ;	

	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( prepareFIFOCommand( PauseMusicRequest ) ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
			
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::pauseMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::pauseMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}


				
void CommandManager::unpauseMusic() throw( CommandException )
{

#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	// Anything to resume ?
	if ( _currentMusic == 0 )
		return ;	

	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	writeBlocking( prepareFIFOCommand( UnpauseMusicRequest ) ) ;

	SetEnabledInterrupts( previous ) ;
	
	notifyCommandToARM7() ;
			
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::unpauseMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::unpauseMusic failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS

}
								


void CommandManager::notifyEndOfEncodedStreamReached() throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9

	// Notify the ARM7 that the end of encoded data was reached:	
	
	writeBlocking( prepareFIFOCommand( EndOfEncodedStreamReached ) ) ; 
	
	notifyCommandToARM7() ;	
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( 
		"CommandManager::notifyEndOfEncodedStreamReached failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( 
		"CommandManager::notifyEndOfEncodedStreamReached failed: "
		"not available on this platform." ) ;

#endif // OSDL_ARCH_NINTENDO_DS
	
}



void CommandManager::unsetCurrentMusic( Music & music ) throw()
{

	if ( _currentMusic == &music )
		_currentMusic = 0 ;
		
}



string CommandManager::interpretLastARM7ErrorCode() throw()
{

	ARM7ErrorCode error = getLastARM7ErrorCode() ;
	
	// Delegates to Ceylan if a Ceylan code:
	if ( error < 1024 )
		return FIFO::interpretLastARM7ErrorCode() ;
	
	switch( error )
	{

		case HelixInitializationFailed:
			return "Helix initialization failed "
				"(most probably not enough memory available on the ARM7)" ;
			break ;

		case HelixSyncWordNotFound:
			return "Helix decoder could not find "
				"the next sync word in the mp3 stream" ;
			break ;
			
		case HelixFoundTruncatedFrame:
			return "Helix decoder is out of data, "
				"found a truncated or last frame in the mp3 stream" ;
			break ;
			
		case HelixLacksDataInBitReservoir:
			return "Helix decoder does not have enough data "
				"in bit reservoir from previous frames" ;
			break ;
			
		case HelixLacksFreeBitrateSlot:
			return "Helix decoder lacks a free bitrate slot" ;
			break ;
			
		case HelixDecodingError:
			return "Helix decoding failed" ;
			break ;
			
		case HelixUnexpectedDecodedLength:
			return 
				"Helix decoder found a decoded mp3 frame "
				"with an unexpected length" ;
			break ;
						
		default:
			return "unexpected OSDL ARM7 error code (" 
				+ Ceylan::toString( error ) + ")" ;
			break ;
	
	}
	
}



const string CommandManager::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res = "Command manager based on " + FIFO::toString( level ) 
		+ ". " ;
	
	if ( _currentMusic == 0 )
		res += "No music currently playing" ;
	else
		res += "A music is currently playing: " 
			+ _currentMusic->toString() ;
	
	return res ;

}




// Static section.


bool CommandManager::HasExistingCommandManager() throw()
{

	return _IPCManager != 0 ;
	
}



CommandManager & CommandManager::GetExistingCommandManager() 
	throw( CommandException )
{

	if ( _IPCManager == 0 )
		throw CommandException( "CommandManager::GetExistingCommandManager: "
			"no manager available" ) ;
	
	return *_IPCManager ;
			
}


	
CommandManager & CommandManager::GetCommandManager() throw( CommandException )
{

	if ( _IPCManager == 0 )
		_IPCManager = new CommandManager() ;
	
	return *_IPCManager ;
		
}



void CommandManager::handleReceivedIntegratingLibrarySpecificCommand(
	FIFOCommandID commandID, Ceylan::System::FIFOElement firstElement )	throw()
{


#if OSDL_ARCH_NINTENDO_DS
			
	// Here we are dealing with a OSDL-specific command.
	switch( commandID )
	{
	
		case NoAvailableChannelNotification:
			LogPlug::warning( 
				"No more free sound channel, playback cancelled" ) ;
			_currentMusic->managePlaybackEnded() ;
			break ;
		
		/*
		 * @note Filling buffers must be done in the main loop, not in this
		 * IRQ handler, thus just requesting the filling, not performing it:
		 *
		 */
		 
		case FirstBufferFillRequest:
			if ( _currentMusic != 0 )
			{
				try
				{
					_currentMusic->requestFillOfFirstBuffer() ;
				
				}
				catch( const AudioException & e )
				{
				
					LogPlug::error(
			"CommandManager::handleReceivedIntegratingLibrarySpecificCommand"
			" failed: " + e.toString() ) ;
			
				}	
			}
			else
			{
				LogPlug::error( 
			"CommandManager::handleReceivedIntegratingLibrarySpecificCommand"
			" failed: request received for first buffer refill whereas "
			"no music is currently playing" ) ;
			}
			break ;
			
		case SecondBufferFillRequest:
			if ( _currentMusic != 0 )
			{
				try
				{
					_currentMusic->requestFillOfSecondBuffer() ;
				
				}
				catch( const AudioException & e )
				{
				
					LogPlug::error(
			"CommandManager::handleReceivedIntegratingLibrarySpecificCommand"
			" failed: " + e.toString() ) ;
			
				}	
			}
			else
			{
				LogPlug::error( 
			"CommandManager::handleReceivedIntegratingLibrarySpecificCommand"
			" failed: request received for second buffer refill whereas "
			"no music is currently playing" ) ;
			}
			break ;
		
		case MusicEndedNotification:
			LogPlug::debug( "Music ended." ) ;
			if ( _currentMusic != 0 )
				_currentMusic->managePlaybackEnded() ;
			else 
				LogPlug::error( 
			"CommandManager::handleReceivedIntegratingLibrarySpecificCommand "
			"failed: notification of end of music received, but there is "
			"no current music" ) ;
			break ;
			
		case MusicFrameInformation:
			LogPlug::debug( "Music frame informations: "
				+ Ceylan::toString( firstElement & 0x0000ffff ) 
				+ " output samples, "
				+ Ceylan::toString( readBlocking() ) + " bytes read." ) ;
			break ;
			
		default:
			LogPlug::error( "handleReceivedIntegratingLibrarySpecificCommand: "
				"unexpected command: " + Ceylan::toNumericalString( commandID )
				+ ", ignored." ) ;
			break ;		
	
	}

#endif // OSDL_ARCH_NINTENDO_DS

}

	
