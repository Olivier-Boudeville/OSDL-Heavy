/*
 * Must be in first position as OSDLCommandManager.h refers to Ceylan.h for
 * the FIFO class definition, and this header needs CEYLAN_ARCH_NINTENDO_DS.
 *
 */
#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS

#include "OSDLCommandManager.h"


#include "OSDLSound.h"               // for Sound
#include "OSDLMusic.h"               // for Music


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H



/**
 * For IPC commands, must be after OSDLConfigForNintendoDS.h include as some
 * Ceylan defines are needed for its included IPC counterpart.
 */
#include "OSDLIPCCommands.h"         


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
	_currentMusic(0)
{


#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
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



void CommandManager::playMusic( Audio::Music & music ) throw( CommandException )
{

	// @fixme: on music end notification, make it ready to be reloaded
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9
	
	music.setAsCurrent() ;
	
	// Forget any past cached current music:
	_currentMusic = & music ;
	
	InterruptMask previous = SetEnabledInterrupts( AllInterruptsDisabled ) ;
	 
	LowLevelMusic & actualMusic = music.getContent() ;

	/*
	 *
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
	writeBlocking( 
		reinterpret_cast<FIFOElement>( actualMusic._doubleBuffer ) ) ;

	// Sends the size of a half buffer and the one of the 'delta zone':
	writeBlocking( (FIFOElement) ( 
		( ( (Ceylan::Uint16) actualMusic._bufferSize ) << 16 ) 
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



void CommandManager::stopMusic() throw( CommandException )
{
			
#if OSDL_ARCH_NINTENDO_DS

#ifdef OSDL_RUNS_ON_ARM9

	// Counterpart of the reference value, Music::_CurrentMusic:
	
	// Anything to stop ?
	if ( _currentMusic == 0 )
		return ;	

	// @fixme send IPC stop
	//Music * toStop = _currentMusic ;
	//toStop->onNoMoreCurrent() ;
	_currentMusic = 0 ;
	
	
#else // OSDL_RUNS_ON_ARM9

	throw CommandException( "CommandManager::stopMusic failed: "
		"not available on the ARM7." ) ;

#endif // OSDL_RUNS_ON_ARM9
		
		
#else // OSDL_ARCH_NINTENDO_DS

	throw CommandException( "CommandManager::stopMusic failed: "
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



string CommandManager::interpretLastARM7ErrorCode() throw()
{

	ARM7ErrorCode error = getLastARM7ErrorCode() ;
	
	// Delegates to Ceylan if a Ceylan code:
	if ( error < 1024 )
		return FIFO::interpretLastARM7ErrorCode() ;
	
	switch( error )
	{
			
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
			LogPlug::debug( "Music ended, minimum whole frame length was "
				+ Ceylan::toString( readBlocking() ) + " bytes, maximum was "
				+ Ceylan::toString( readBlocking() ) + " bytes." ) ;
			_currentMusic->onPlaybackEnded() ;
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

	
