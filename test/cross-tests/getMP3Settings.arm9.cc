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


#include "Ceylan.h"         // as OSDL cannot be used here (special mechanisms)

#include <iostream>

#include <string>
using std::string ;

#include <list>
using std::list ;

#include <exception>


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;


// Not having OSDLMusic.h:
typedef Ceylan::Uint32 BufferSize ;



/**
 * This program reads vanilla mp3 files, whereas all other OSDL programs read
 * OSDL.music files, i.e. mp3 files with an OSDL header.
 *
 * The purpose of this program is to determine the best decoding settings of a
 * given mp3 file so that its playback by OSDL uses as few resources as
 * possible.
 *
 * As such it has to use its own routines, not the OSDL ones.
 *
 * To use it, just put at the root of your removable card the mp3 file whose
 * settings have to be determined, renamed as 'test.mp3', and run this program.
 *
 * The mp3 will be played-back from start to end, then the settings will be
 * displayed. Next step would then be to run the tools/media/mp3ToOSDLMusic.exe
 * tool to generate the appropriate *.osdl.music file.
 *
 */


/*
 *
 * @note taken from OSDLMusic.h
 */
struct LowLevelMusic
{

	// Music-specific section.

	/// The file from which music samples will be streamed:
	Ceylan::System::File * _musicFile ;

	/// The music sampling frequency, in Hertz:
	Ceylan::Uint16 _frequency ;

	/// The music bit depth, in bits (8bit/16bit):
	Ceylan::Uint8 _bitDepth ;

	/**
	 * The mode, i.e. the number of channels (mono:1/stereo:2):
	 * (only mono supported currently)
	 *
	 */
	Ceylan::Uint8 _mode ;


	/// The total size of music, in bytes:
	BufferSize _size ;


	// Buffer-specific section.

	/// The size of a (simple) buffer, in bytes:
	BufferSize _bufferSize ;

	/// The smallest upper bound chosen to a MP3 frame size.
	BufferSize _frameSizeUpperBound ;


	/**
	 * The actual double sound buffer, two simple buffers, one after the other
	 * (so the first half buffer has the same address as this double one)
	 *
	 */
	Ceylan::Byte * _doubleBuffer ;


	/// Tells whether the filling of first buffer has been requested.
	volatile bool _requestFillOfFirstBuffer ;


	/*
	 * Precomputes the start of the first buffer, after the delta zone.
	 *
	 */
	Ceylan::Byte * _startAfterDelta ;

	/// Precomputes delta-wise the refill size of first buffer.
	BufferSize _firstActualRefillSize ;


	/// The address of the second buffer:
	Ceylan::Byte * _secondBuffer ;

	/// Tells whether the filling of second buffer has been requested.
	volatile bool _requestFillOfSecondBuffer ;


} ;


// From ARM9:
const FIFOCommandID PlayMusicRequest          = 34 ;
const FIFOCommandID EndOfEncodedStreamReached = 36 ;


// From ARM7:
const FIFOCommandID NoAvailableChannelNotification = 33 ;
const FIFOCommandID FirstBufferFillRequest         = 34 ;
const FIFOCommandID SecondBufferFillRequest        = 35 ;
const FIFOCommandID MusicEndedNotification         = 36 ;
const FIFOCommandID MusicFrameInformation          = 37 ;



/**
 * IPC-based command manager, between the two ARMs of the Nintendo DS,
 * simplified for this OSDL-less test.
 *
 */
class SimplifiedCommandManager : public Ceylan::System::FIFO
{

	public:


	  SimplifiedCommandManager() throw( FIFOException ):
		FIFO(),
		_isPlaying(false),
		_message()
	  {

	  }


	  virtual ~SimplifiedCommandManager() throw()
	  {

		// Must be called from FIFO child class:
		deactivate() ;

	  }


	  void stopPlaying()
	  {

		_isPlaying = false ;

	  }


	  bool isPlaying()
	  {
		return _isPlaying ;
	  }



	  /**
	   * Mimics an oversimplified CommandManager::playMusic.
	   *
	   */
	  void playMusic( LowLevelMusic & actualMusic )
	  {

		  _music = & actualMusic ;

		  _isPlaying = true ;

		  // Fill only the first buffer:
		  _music->_requestFillOfFirstBuffer  = true  ;
		  _music->_requestFillOfSecondBuffer = false  ;

		  // Fill the first buffer:
		  manageBuffers() ;

		  // Plan second refill:
		  _music->_requestFillOfSecondBuffer = true  ;


		  InterruptMask previous =
			SetEnabledInterrupts( AllInterruptsDisabled ) ;

		  /*
		   * A boolean parameter is set in the command element: its last bit tel
		   * whether the playback should start from first buffer (if 1) or from
		   * second (if 0).
		   *
		   * The second element is a pointer to the cache-aligned sound buffer,
		   * third is composed of the full size of the encoded buffer (16 first
		   * bits) then the delta value (the MP3 frame size upper bound, 16 last
		   * bits).
		   *
		   */
		  FIFOElement commandElement = prepareFIFOCommand( PlayMusicRequest ) ;

		  // Uses last bit to tell to start with first buffer:
		  writeBlocking( ( commandElement | 0x00000001 ) ) ;

		  // Specifies to the ARM7 the address of encoded double buffer:
		  writeBlocking(
			  reinterpret_cast< FIFOElement >( actualMusic._doubleBuffer ) ) ;

		  // Sends the size of a half buffer and the one of the 'delta zone':
		  writeBlocking( (FIFOElement) (
			  ( ( (Ceylan::Uint16) actualMusic._bufferSize ) << 16 )
				  | ( (Ceylan::Uint16) actualMusic._frameSizeUpperBound ) ) ) ;

		  SetEnabledInterrupts( previous ) ;

		  notifyCommandToARM7() ;

		  /*
		   * Here the decoding will be triggered on the ARM7, the ARM9 main loop
		   * is responsible for the refilling of encoded buffers.
		   *
		   */

	  }



	  void manageBuffers()
	  {

		try
		{

		  if ( _music->_requestFillOfFirstBuffer )
		  {

			  //LogPlug::trace( "Filling first buffer." ) ;

			  _music-> _requestFillOfFirstBuffer = false ;

			  /*
			  LogPlug::trace( "manageBuffers: Attempting to read "
				+ Ceylan::toString( _music->_firstActualRefillSize )
				+ " bytes." ) ;
			   */

			  BufferSize readSize = _music->_musicFile->read(
				/* start after delta zone */ _music->_startAfterDelta,
				/* max length */ _music->_firstActualRefillSize ) ;

			  /*
			  LogPlug::trace( "manageBuffers: read "
				+ Ceylan::toString(readSize) + " bytes." ) ;
			   */

			  /*
			   * Zero-pad to avoid finding false sync word after last frame
			   * (from previous data in this first buffer):
			   */
			  if ( readSize < _music->_firstActualRefillSize )
			  {

				  LogPlug::trace( "Padding first buffer." ) ;

				  ::memset(
					  /* start */ _music->_startAfterDelta + readSize,
					  /* filler */ 0,
					  /* length */ _music->_bufferSize
						- _music->_frameSizeUpperBound - readSize ) ;


				  // Flush the ARM9 data cache for the ARM7 (full first buffer):
				  DC_FlushRange( (void*) _music->_startAfterDelta,
					  _music->_firstActualRefillSize ) ;

				  InterruptMask previous =
					SetEnabledInterrupts( AllInterruptsDisabled ) ;

				  // Notify the ARM7 that the end of encoded data was reached:
				  writeBlocking( prepareFIFOCommand(
						EndOfEncodedStreamReached ) ) ;

				  SetEnabledInterrupts( previous ) ;

				  notifyCommandToARM7() ;

				  //LogPlug::trace( "Padding first buffer ok." ) ;

			  }
			  else
			  {

				  //LogPlug::trace( "Flushing." ) ;

				  /*
				   * Full or encoded data, flush the ARM9 data cache for the
				   * ARM7:
				   *
				   */
				  DC_FlushRange( (void*) _music->_startAfterDelta,
					  _music->_firstActualRefillSize ) ;

			  }

		  }


		  if (  _music->_requestFillOfSecondBuffer )
		  {

			  //LogPlug::trace( "Filling second buffer." ) ;

			   _music->_requestFillOfSecondBuffer = false ;

			  /*
			   * Start at the half of double buffer to its end:
			   */
			  BufferSize readSize = _music->_musicFile->read(
				  /* start */ _music->_secondBuffer,
				  /* max length */ _music->_bufferSize ) ;

			  /*
			   * Zero-pad to avoid finding false sync word after last frame
			   * (from previous data in this first buffer):
			   */
			  if ( readSize < _music->_bufferSize )
			  {

				  LogPlug::trace( "Padding second buffer." ) ;

				  ::memset(
					  /* start */ _music->_secondBuffer + readSize,
					  /* filler */ 0,
					  /* length */ _music->_bufferSize - readSize
				  ) ;


				  // Flush the ARM9 data cache for the ARM7:
				  DC_FlushRange( (void*) _music->_secondBuffer,
					_music->_bufferSize ) ;

				  InterruptMask previous =
					SetEnabledInterrupts( AllInterruptsDisabled ) ;

				  // Notify the ARM7 that the end of encoded data was reached:
				  writeBlocking(
						prepareFIFOCommand( EndOfEncodedStreamReached ) ) ;

				  SetEnabledInterrupts( previous ) ;

				  notifyCommandToARM7() ;

			  }
			  else
			  {

				  // Flush the ARM9 data cache for the ARM7:
				  DC_FlushRange( (void*) _music->_secondBuffer,
					_music->_bufferSize ) ;

			  }

		  }

		}
		catch( const Ceylan::Exception & e )
		{

			LogPlug::error( "manageBuffers failed: " + e.toString() ) ;

		}

	  }



	  void handleReceivedIntegratingLibrarySpecificCommand(
			FIFOCommandID commandID, Ceylan::System::FIFOElement firstElement )
		throw()
	 {


		// Here we are dealing with a OSDL-specific command.
		switch( commandID )
		{

			case NoAvailableChannelNotification:
				LogPlug::warning(
					"No more free sound channel, playback cancelled" ) ;
				break ;

			/*
			 * @note Filling buffers must be done in the main loop, not in this
			 * IRQ handler, thus just requesting the filling, not performing it:
			 *
			 */

			case FirstBufferFillRequest:
				_music->_requestFillOfFirstBuffer = true ;
				break ;

			case SecondBufferFillRequest:
				_music->_requestFillOfSecondBuffer = true ;
				break ;

			case MusicEndedNotification:
				_isPlaying = false ;
				_message = "Music ended, minimum whole frame length was "
					+ Ceylan::toString( readBlocking() )
					+ " bytes, maximum was "
					+ Ceylan::toString( readBlocking() ) + " bytes "
					"(this last number can be given to mp3ToOSDLMusic.exe)." ;
				break ;

			case MusicFrameInformation:
				if ( firstElement & 0x0000ffff != 576 )
					_message = "MP3 frame output an unexpected number "
						"of sample: "
						+ Ceylan::toString( firstElement & 0x0000ffff )
						+ " samples, with "
						+ Ceylan::toString( readBlocking() )
						+ " bytes read." ;
				else
					_message = "MP3 frame: "
						+ Ceylan::toString( readBlocking() )
						+ " bytes read." ;
				break ;

			default:
				LogPlug::error(
					"handleReceivedIntegratingLibrarySpecificCommand: "
					"unexpected command: "
					+ Ceylan::toNumericalString( commandID )
					+ ", ignored." ) ;
				break ;

		}
	  }


	  std::string & getMessage()
	  {

		return _message ;

	  }

	  private:

		volatile LowLevelMusic * _music ;

		volatile bool _isPlaying ;

		std::string _message ;

} ;


/**
 * Test for the ideal settings for a MP3 file encoded in a *.osdl.music file on
 * the Nintendo DS.
 *
 * @note Useful to find the best settings when creating an OSDL music file.
 *
 * @see trunk/tools/media/mp3ToOSDLMusic.cc
 *
 */
int main( int argc, char * argv[] )
{


	LogHolder myLog( argc, argv ) ;


	try
	{


		const string musicFilename = "test.mp3" ;

		LogPlug::info( "Test of MP3 settings for OSDL, for the '"
			+ musicFilename + "' file." ) ;


		LogPlug::info( "Creating simplified command manager." ) ;

		SimplifiedCommandManager myManager ;

		LogPlug::info( "Activating this simplified command manager." ) ;
		myManager.activate() ;


		LowLevelMusic * music = new LowLevelMusic() ;

		music->_musicFile = & File::Open( musicFilename ) ;

		LogPlug::trace( "File opened." ) ;

		music->_frequency = 22050 /* kHz */ ;

		music->_bitDepth = 16 /* bits */ ;

		music->_mode = /* mono */ 1 ;

		music->_size = music->_musicFile->size() ;

		LogPlug::trace( "File size is " + Ceylan::toString( music->_size )
			+ " bytes." ) ;

		// Could be optimized according to libfat cache size:
		music->_bufferSize = 2 * 4096 ;

		/*
		 * Safest upper bound (from Helix example) retained here, goal is to
		 * determine optimal one:
		 *
		 */
		music->_frameSizeUpperBound = 2 * 1940 ;

		LogPlug::trace( "Allocating buffer." ) ;

		// Force music buffer to match the boundaries of ARM9 cache line:
		music->_doubleBuffer = CacheProtectedNew( music->_bufferSize * 2 ) ;

		music->_startAfterDelta =
			music->_doubleBuffer + music->_frameSizeUpperBound ;

		music->_firstActualRefillSize =
			music->_bufferSize - music->_frameSizeUpperBound ;

		LogPlug::trace( "First actual refill size is "
			+ Ceylan::toString( music->_firstActualRefillSize )
			+ " bytes." ) ;

		music->_secondBuffer = music->_doubleBuffer
			+ music->_bufferSize ;


		LogPlug::info( "Sending to the ARM7 a command request to play it." ) ;

		LogPlug::info( "Current ARM7 status before playing: "
			 + myManager.interpretLastARM7StatusWord() ) ;

		LogPlug::info( "Refilling now buffers while playing." ) ;
		myManager.playMusic( *music ) ;


		/*
		 * The main loop has to refill buffers when appropriate, as it should
		 * not be done in IRQ handlers.
		 *
		 */

		uint16 count = 0 ;

		while ( myManager.isPlaying() )
		{

			myManager.manageBuffers() ;

			// Avoid sending logs from an IRQ, as log system no reentrant:
			if ( count++ % 60 == 0 )
				LogPlug::info( myManager.getMessage() ) ;

			/*
			LogPlug::info( "Current ARM7 status while playing: "
			  + myManager.interpretLastARM7StatusWord() ) ;
			 */

			atomicSleep() ;

		}

		/*
		LogPlug::info( "Current ARM7 status after playing: "
			 + myManager.interpretLastARM7StatusWord() ) ;
		 */

		LogPlug::info( myManager.getMessage() ) ;


		/*
		LogPlug::info( "Current ARM7 status: "
			 + myManager.interpretLastARM7StatusWord() ) ;
		 */


		// LogHolder out of scope: log browser triggered.

	}

	catch ( const Ceylan::Exception & e )
	{

		LogPlug::error( "Ceylan exception caught: "
			+ e.toString( Ceylan::high ) ) ;

		return Ceylan::ExitFailure ;

	}

	catch ( const std::exception & e )
	{

		LogPlug::error( string( "Standard exception caught: " ) + e.what() ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( ... )
	{

		LogPlug::error( "Unknown exception caught" ) ;
		return Ceylan::ExitFailure ;

	}

	LogPlug::info( "Exit on success (no error)" ) ;

	OSDL::shutdown() ;

	return Ceylan::ExitSuccess ;

}
