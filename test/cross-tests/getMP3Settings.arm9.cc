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
 * Double-buffered music.
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
	 * The actual double sound buffer, two simple buffers, one after the
	 * other (so the first half buffer has the same address as this
	 * double one)
	 *
	 */
	Ceylan::Byte * _doubleBuffer ;

	
	/// Tells how many bytes can be read from first buffer.
	BufferSize _availableInFirst ;
	
	/*
	 * Precomputes the start of the first buffer, after the delta 
	 * zone.
	 *
	 */
	Ceylan::Byte * _startAfterDelta ;
	
	/// Precomputes delta-wise the refill size of first buffer.
	BufferSize _firstActualRefillSize ;
	
	
	/// The address of the second buffer:
	Ceylan::Byte * _secondBuffer ;


	/// Tells how many bytes can be read from second buffer.
	BufferSize _availableInSecond ;
	
	
} ;



const FIFOCommandID PlayMusicRequest = 34 ;
const FIFOCommandID EndOfEncodedStreamReached = 36 ;
	
const FIFOCommandID NoAvailableChannelNotification = 33 ;
const FIFOCommandID FirstBufferFillRequest = 34 ;
const FIFOCommandID SecondBufferFillRequest = 35 ;
const FIFOCommandID MusicEndedNotification = 36 ;
const FIFOCommandID MusicFrameInformation = 37 ;



/**
 * IPC-based command manager, between the two ARMs of the Nintendo DS,
 * simplified for this OSDL-less test.
 *
 */
class SimplifiedCommandManager: public Ceylan::System::FIFO
{

	public:
	
	
	  SimplifiedCommandManager() throw( FIFOException ):
		FIFO(),
	  	_isPlaying(false),
		_requestFillOfFirstBuffer( true ),
		_requestFillOfSecondBuffer( true )
		
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
		  
		  // Fill the two buffers:
		  manageBuffers() ;
		  
		  _isPlaying = true ;
		  
		   
	      InterruptMask previous = 
		  	SetEnabledInterrupts( AllInterruptsDisabled ) ;
	
	      /*
	       *
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

		  if ( _requestFillOfFirstBuffer )
		  {
	
			  LogPlug::trace( "Filling first buffer." ) ;
			  
		      _requestFillOfFirstBuffer = false ;

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
			    	  /* length */ _music->_bufferSize - (BufferSize)
			    		  ( _music->_startAfterDelta + readSize )
			      ) ;
	
			      // Flush the ARM9 data cache for the ARM7 (full first buffer):
			      DC_FlushRange( (void*) _music->_startAfterDelta,
			    	  _music->_firstActualRefillSize  ) ;
	
				  // Notify the ARM7 that the end of encoded data was reached:
				  writeBlocking( prepareFIFOCommand( 
						EndOfEncodedStreamReached ) ) ; 
	
				  notifyCommandToARM7() ;	
				  
			  }			  
			  else
			  {
	
				  LogPlug::trace( "Flushing." ) ;
				  
			      /*
				   * Full or encoded data, flush the ARM9 data cache for the
				   * ARM7:
				   *
				   */
			      DC_FlushRange( (void*) _music->_startAfterDelta,
			    	  _music->_firstActualRefillSize ) ;
	
			  }
	
		  }
	
	
		  if ( _requestFillOfSecondBuffer )
		  {
	
			  LogPlug::trace( "Filling second buffer." ) ;
			  
		      _requestFillOfSecondBuffer = false ;
			  
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
				  	_music->_bufferSize  ) ;

				  // Notify the ARM7 that the end of encoded data was reached:
				  writeBlocking( prepareFIFOCommand( 
						EndOfEncodedStreamReached ) ) ; 
	
				  notifyCommandToARM7() ;	
	
	    	  }
	    	  else
	    	  {
				  
	    		  // Flush the ARM9 data cache for the ARM7:
	    		  DC_FlushRange( (void*) _music->_secondBuffer, 
				  	_music->_bufferSize  ) ;
	
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
				_requestFillOfFirstBuffer = true ;
				break ;
	
			case SecondBufferFillRequest:
				_requestFillOfSecondBuffer = true ;
				break ;
	
			case MusicEndedNotification:
				LogPlug::info( "Music ended, minimum whole frame length was "
					+ Ceylan::toString( readBlocking() ) 
					+ " bytes, maximum was "
					+ Ceylan::toString( readBlocking() ) + " bytes "
					"(this number can be given to mp3ToOSDLMusic.exe)."	) ;
				break ;
	
			case MusicFrameInformation:
				LogPlug::debug( "Music frame informations: "
					+ Ceylan::toString( firstElement & 0x0000ffff )
					+ " output samples, "
					+ Ceylan::toString( readBlocking() ) + " bytes read." ) ;
				_isPlaying = false ;
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
	 
	 	
	
	  private:
	  	
		volatile LowLevelMusic * _music ;
		
		volatile bool _isPlaying ;
		
		volatile bool _requestFillOfFirstBuffer ;
		
		volatile bool _requestFillOfSecondBuffer ;

} ;


/**
 * Test for the ideal settings for a MP3 file encoded in a *.osdl.music file
 * on the Nintendo DS.
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
				
		
		bool interactive = true ;
				
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
			
		
		// Fill first buffer before playing:
		LogPlug::trace( "Initial fill." ) ;		

		
		LogPlug::info( "Sending to the ARM7 a command request to play it." ) ;
		
		LogPlug::info( "Current ARM7 status before playing: "
			 + myManager.interpretLastARM7StatusWord() ) ;

		myManager.playMusic( *music ) ;
		
		
		/*
		 * The main loop has to refill buffers when appropriate, as it should
		 * not be done in IRQ handlers.
		 *
		 */
		
		
		LogPlug::info( "Refilling now buffers while playing." ) ;
		
		while ( myManager.isPlaying() )
		{
		
			LogPlug::info( "Current ARM7 status while playing: "
			  + myManager.interpretLastARM7StatusWord() ) ;
			  
			myManager.manageBuffers() ;
			
			atomicSleep() ;
		
		}

		LogPlug::info( "Current ARM7 status after playing: "
			 + myManager.interpretLastARM7StatusWord() ) ;
		
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to stop waiting" ) ;
			waitForKey() ;
		
		}
		
		LogPlug::info( "Current ARM7 status: "
			 + myManager.interpretLastARM7StatusWord() ) ;

		bool testFailed = false ;		
		
							
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to stop waiting" ) ;
			waitForKey() ;
		
		}
				
				
		if ( testFailed )
			throw TestException( "Test failed because of error(s) "
				"previously displayed." ) ;
								
		if ( interactive )
		{
		
			LogPlug::info( "Press any key to end the test" ) ;
			waitForKey() ;
		
		}
			
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
	
    return Ceylan::ExitSuccess ;

}

