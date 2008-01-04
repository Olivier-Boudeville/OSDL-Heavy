#include "OSDLARM7Base.h"

/*
 * Implementation notes:
 * 
 *
 */


/* Defines the actual OSDL ARM7 status words and error codes. */
#include "OSDLARM7Codes.h"

/* Defines OSDL IPC command identifiers. */
#include "OSDLIPCCommands.h"




/*
 * Returns the number of the first free channel, otherwise -1 is all are busy.
 *
 * @note Channel 15 is reserved (for music).
 *
 */
s32 getFreeSoundChannel()
{

	int i ;
	
	for ( i=0; i<15; i++ )
	{
	
		if ( ( SCHANNEL_CR(i) & SCHANNEL_ENABLE ) == 0 )
			return i ;
			
	}

	return -1 ;
	
}



/* Section dedicated to library integration of OSDL IPC. */


/* Manages an IPC request to play an OSDL sound. */	
void handlePlaySoundRequest( FIFOElement firstElement )
{
	
	s32 channel = getFreeSoundChannel() ;	
	
	if ( channel == -1 )
	{
	
		/* Notifies the ARM9 the playback could not be performed: */
		
		/* Interrupts already disabled in this handler*/

		writeBlocking( prepareFIFOCommand( NoAvailableChannelNotification ) ) ;
	
		notifyCommandToARM9() ;
		
		return ;
		
	}
	
	/* Frequency of sound, in Hz (beware: must be a uint16): */
	SCHANNEL_TIMER(channel) = 
		SOUND_FREQ( (uint16) (firstElement & 0x0000fffff) ) ;

	/* Start address: */
	SCHANNEL_SOURCE(channel) = (uint32) readBlocking() ;

	/* Size, in bytes: */
	SCHANNEL_LENGTH(channel) = ( (uint32) readBlocking() ) >> 2 ;
	
	/* Stereo not supported yet (needs the use of two channels) */
	
	/* Selecting sample format: */
	uint32 sampleFormat ;
	
	switch( (firstElement & 0x00f00000) >> 5 /* figures */ * 4 /* bits */ )
	{
	
		case 0x0:
			sampleFormat = SOUND_FORMAT_ADPCM ;
			break ;
			
		case 0x1:
			sampleFormat = SOUND_FORMAT_8BIT ;
			break ;
			
		case 0x2:
			sampleFormat = SOUND_FORMAT_16BIT ;
			break ;
			
		default:
			setError( UnexpectedSampleFormat ) ;
			return ;
			
	}
	
	SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_ONE_SHOT	| sampleFormat
		| SOUND_VOL(0x7F) ;	

}




/** OSDL-specific command handler. */
void handleReceivedIntegratingLibrarySpecificCommand( FIFOCommandID commandID,
	FIFOElement firstElement )
{
		
	/*
	 * Here we are dealing with an OSDL-specific command.
	 *
	 * @note Cannot use switch here, as if using:
	 * const int MyConstant = 1 ;
	 * switch ( aValue )
	 * {
	 *   case MyConstant:
	 * etc.
	 *
	 * gcc says: 'case label does not reduce to an integer constant'
	 *
	switch ( commandID )
	{
				
		default:
			setError( UnexpectedOSDLCommand ) ;
			break ;
				
	}
	
	*/
	 
	if ( commandID == PlaySoundRequest )
		handlePlaySoundRequest( firstElement ) ;
	else
		setError( UnexpectedOSDLCommand ) ;
		
}



/**
 * Sets-up OSDL and its prerequesites.
 *
 */
void initOSDL()
{

	initCeylan() ;
	

}

