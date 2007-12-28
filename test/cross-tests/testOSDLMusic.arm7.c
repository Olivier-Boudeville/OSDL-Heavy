

/**
 * ARM7 side of the OSDL raw test.
 *
 * @see testOSDLRawSound.arm9.cc for the peer implementation
 * 
 * @note Several warning are still there:
 *  - cast discards qualifiers from pointer target type
 *  - passing argument 2 of 'MP3Decode' from incompatible pointer type
 *  - passing argument 2 of 'MP3GetLastFrameInfo' discards qualifiers from
 * pointer target type
 *
 * As they do not seem to be removable with syntactic sugar, we preferred 
 * keeping them instead of deactivating otherwise useful warnings.
 *
 */
 
 
/*
 * Apparently the overhead due to the C++, to the STL and (marginally) to
 * the OSDL library itself leads to having an ARM7 executable too big to
 * fit in its IWRAM.
 *
 * Hence for the moment the inclusion of the OSDL header is commented out
 * and libnds is directly used instead.
 *
 */
 
#define USE_OSDL 0 

#if USE_OSDL

#include "OSDL.h"

#else /* USE_OSDL */


#define ARM7

/* For libnds: */
#include "nds.h"

/* For Helix decoder: */
#include "Helix/mp3dec.h"


#endif // USE_OSDL


/* Defines the actual ARM7 status words and error codes */
#include "OSDLARM7Codes.h"

/* Defines IPC command identifiers */
#include "OSDLIPCCommands.h"


#include "stdlib.h" /* for rand */
#include "string.h" /* for memmove */



/*
 * Implementation notes:
 * 
 * @see libnds include/nds/ipc.h for defines.
 *
 */


/* Defines the actual ARM7 status words and error codes, and CEYLAN_SAFE_FIFO */
#include "CeylanARM7Codes.h"

/* Defines IPC command identifiers */
#include "CeylanIPCCommands.h"



/*
 * Directly obtained from libnds ARM7 template.
 * This is a stripped-down version (no sound) with FIFO support.
 *
 * @see testCeylanFIFO.arm9.cc and CeylanFIFO.cc
 *
 */


touchPosition first, tempPos ;


/* libnds IPC uses shared variables in the transfer region */
void VcountHandler() 
{

	/* Updates the button state and the touchscreen: */
	
	static int lastbut = -1 ;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0 ;

	but = REG_KEYXY ;

	if ( ! ( (but ^ lastbut) & (1<<6) ) ) 
	{
 
		tempPos = touchReadXY() ;

		if ( tempPos.x == 0 || tempPos.y == 0 ) 
		{
		
			but |= (1 <<6) ;
			lastbut = but ;
			
		} 
		else 
		{
		
			x   = tempPos.x ;
			y   = tempPos.y ;
			xpx = tempPos.px ;
			ypx = tempPos.py ;
			z1  = tempPos.z1 ;
			z2  = tempPos.z2 ;
			
		}
		
	} 
	else 
	{
		lastbut = but ;
		but |= (1 <<6) ;
	}

	IPC->touchX	  = x ;
	IPC->touchY	  = y ;
	IPC->touchXpx = xpx ;
	IPC->touchYpx = ypx ;
	IPC->touchZ1  = z1 ;
	IPC->touchZ2  = z2 ;
	IPC->buttons  = but ;

}




/* Ceylan FIFO-based IPC section */



/* CEYLAN_SAFE_FIFO read from CeylanARM7Codes.h */


/* No CEYLAN_DEBUG_FIFO here */



/* Type definitions */


/* One entry of the FIFO */
typedef uint32 FIFOElement ;

/* Describes a number of FIFO commands. */
typedef uint8 FIFOCommandCount ;


/* For buffers */
typedef char Byte ;

/* For buffers */
typedef uint32 BufferSize ;


/* Masks describing which interrupts are enabled. */
typedef int InterruptMask ;




/* Instance definitions */


/* To specify that all interrupts are to disabled. */
const InterruptMask AllInterruptsDisabled = 0 ;



/* Definitions of global variables */

/*
 * 'ARM7StatusWord volatile * statusWordPointer = 0 ;' would not be enough:
 * it would correctly manage the fact that the ARM9 can change the pointed
 * value, but the pointer itself must be volatile too, as it can be change
 * in the IRQ handler after a StatusInitRequest command
 * has been received.
 *
 */

/* Pointer to the ARM7 shared status word, allocated by the ARM9 */
ARM7StatusWord volatile * volatile statusWordPointer = 0 ;

/* Pointer to the ARM7 shared error code, allocated by the ARM9 */
ARM7ErrorCode volatile * volatile errorWordPointer = 0 ;


volatile bool IPCRunning = false ;



/* Normal command counts */


/*
 * Automatically incremented when using the prepareFIFOCommand function, hence
 * not to be especially managed by user code.
 *
 */
volatile FIFOCommandCount localCommandCount = 0 ;


/*
 * Automatically incremented by the handleReceivedCommand function, hence not 
 * to be especially managed by user code.
 *
 */
volatile FIFOCommandCount remoteCommandCount = 0 ;



/* 4-bit only command counts */

/*
 * Automatically incremented by the handleReceivedCommand function, hence not 
 * to be especially managed by user code.
 *
 */
volatile FIFOCommandCount processedCount = 0 ;


/*
 * Automatically incremented by the notifyCommandToARM9 function, hence not 
 * to be especially managed by user code.
 *
 */
volatile FIFOCommandCount sentCount = 0 ;







/* Music decoding section */


/* Tells whether a MP3 play command is being processed. */
volatile bool startMP3PlaybackRequested = false ;


/*
 * The state of the Helix decoder instance.
 *
 */
volatile HMP3Decoder currentDecoder ;

/* Gathers informations about a decoded MP3 frame, needed for playback: */
volatile MP3FrameInfo frameInfo ;



/* Read encoded MP3 data subsection. */


/* The size of a (simple) buffer, in bytes: */
volatile BufferSize bufferSize ;


/* The smallest upper bound chosen to a MP3 frame size: */
volatile BufferSize frameSizeUpperBound ;


/* The minimum size read for a MP3 frame, including the frame and its sync: */
volatile BufferSize minWholeFrameLength = (BufferSize) -1 ;

/* The maximum size read for a MP3 frame, including the frame and its sync: */
volatile BufferSize maxWholeFrameLength = 0 ;



/*
 * The actual double sound buffer, two simple buffers, one after the
 * other (so the first half buffer has the same address as this
 * double one).
 *
 */
volatile Byte * doubleBuffer ;


/*
 * The actual address of the second sound buffer.
 *
 */
volatile Byte * secondBuffer ;


/* The pointer to current bytes being read in the encoded buffer: */
volatile Byte * readPointer ;


/* 
 * Tells whether the first buffer is being read (if true) or the second one 
 * (if false).
 */
volatile bool readingFirstBuffer ;


/* 
 * The pointer to the byte in the encoded buffer from which no more MP3
 * frame should be read, as the remaining space could be too small to contain
 * a full frame: 
 */
volatile Byte * endOfSafeRead ;


/* Precomputes an offset for the moving of encoded data: */
volatile Byte * destinationOffset ;

/* Precomputes an offset for the moving of encoded data: */
volatile Byte * sizeOffset ;


/* Tells whether the end of the MP3 stream has been detected by the ARM9. */
volatile bool endOfStreamDetected ;

/* Tells whether the decoding encountered a fatal error. */
volatile bool decodingInError ;



/* Output decoded samples subsection. */


/*
 * The buffer where decoded music will be output.
 * Will be directly played by the sound hardware.
 *
 */
volatile Byte * decodedBuffer ;

/* The pointer to the place in decodedBuffer where decoding should be output: */
volatile Byte * decodePointer ;

/* Each decoded MP3 frame should result in samples of this size, in bytes: */
const uint16 DecodedFrameLength = 1152 ;

/* Last channel (15) is the one reserved to music (mono) */
const uint8 MusicChannel = 15 ;




/* Helper functions */


/* Waits a short time slice. Needs the VBlank IRQ to be enabled. */ 
void atomicSleep()
{

	swiWaitForVBlank() ;
	
}



/*
 * Returns the number of first free channel, otherwise -1 is all are busy.
 * @note Channel 15 is reserved (for music).
 *
 */
s32 getFreeSoundChannel()
{

	int i ;
	
	/* Music channel never free: */
	for ( i=0; i<15; i++ )
	{
	
		if ( ( SCHANNEL_CR(i) & SCHANNEL_ENABLE ) == 0 )
			return i ;
			
	}

	return -1 ;
	
}



/**
 * Sets the ARM7 status word, for the ARM9.
 *
 * @note If previous status was ARM7InError, will be left as is.
 *
 */
void setStatusWord( ARM7StatusWord newStatus )
{

	if ( statusWordPointer != 0 )
	{
	
		if ( *statusWordPointer != ARM7InError )
			*statusWordPointer = newStatus ;
			
	}
	
}



/**
 * Sets the ARM7 error code, for the ARM9.
 * Updates the status word accordingly.
 *
 * @note If previous error code was not NoError, will be left as is.
 *
 */
void setError( ARM7ErrorCode newError )
{
	
	setStatusWord( ARM7InError ) ;
	
	if ( errorWordPointer != 0 )
	{
	
		if ( *errorWordPointer == NoError )
			*errorWordPointer = newError ;
			
	}
	
}



/**
 * Unset any previous error status, for example when the error code has been
 * taken into account already.
 *
 */
void unsetErrorStatus()
{


	if ( statusWordPointer != 0 )
		*statusWordPointer = NoStatusAvailable ;
		
	if ( errorWordPointer != 0 )
		*errorWordPointer = NoError ;
		
}

 
 
FIFOElement prepareFIFOCommand( FIFOCommandID id )
{
		
	FIFOElement res = 0 ;

#if CEYLAN_SAFE_FIFO
	
	res |= ( id << 24 ) | ( localCommandCount << 16 ) ;

	/* Prepare for next command: */
	localCommandCount++ ;
	
#else // CEYLAN_SAFE_FIFO

	res |= id << 24 ;

#endif // CEYLAN_SAFE_FIFO
	
	return res ;
	
}



FIFOCommandID getFIFOCommandIDFrom( FIFOElement element )
{

	return ( element & 0xff000000 ) >> 24 ;
	
}



FIFOCommandCount getFIFOCommandCountFrom( FIFOElement element )
{

	return ( element & 0x00ff0000 ) >> 16 ;
	
}



FIFOCommandCount getARM7ProcessedCount()
{
	
	/* IPC Remote Status is in bits 0-3: */
	return REG_IPC_SYNC & 0x0f ;
	
}



FIFOCommandCount getARM9ProcessedCount()
{
	
	/* IPC Local Status is in bits 8-11: */
	return ( REG_IPC_SYNC & 0x0f00 ) >> 8 ;
	
}



FIFOCommandCount getProcessedCount()
{

	/* Will be: 0000abcd */
	return ( processedCount & 0x0f ) ;
	
}



FIFOCommandCount getSentCount()
{

	return ( sentCount & 0x0f ) ;

}



void sendSynchronizeInterruptToARM9()
{

	/* Triggers on IRQ on the ARM9 and specifies the local processed count: */
	REG_IPC_SYNC = (REG_IPC_SYNC & 0xf0ff) | (getProcessedCount() << 8) 
		| IPC_SYNC_IRQ_REQUEST ;

}



void incrementProcessCount()
{

	processedCount++ ;
	
	/*
	 * Updates the local processed count in IPC register: 
	 *
	 * @note: no IPC_SYNC_IRQ_REQUEST here, ARM9 not notified.
	 *
	 */
	REG_IPC_SYNC = (REG_IPC_SYNC & 0xf0ff) | (getProcessedCount() << 8) ;
		
}



/**
 * Sets the current set of interrupts enabled.
 *
 * @param newMask the masks describing all the interrupts that are
 * to be enabled.
 *
 * @return The previous mask that was used, before being replaced by
 * the specified one.
 *
 */
InterruptMask setEnabledInterrupts( InterruptMask newMask )
{

	InterruptMask previousMask = REG_IME ;
	
	REG_IME = newMask ;
	
	return previousMask ;

}



void notifyCommandToARM9()
{

	sentCount++ ;
	sendSynchronizeInterruptToARM9() ;

}



bool dataAvailableForReading()
{

	return ! ( REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY ) ;
	
}



bool spaceAvailableForWriting()
{

	return ! ( REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL ) ;

}




FIFOElement read()
{

#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileReading ) ;

#endif // CEYLAN_SAFE_FIFO
	
	
	if ( ! dataAvailableForReading() )
		setError( FIFOErrorWhileReading ) ;
		
	FIFOElement res = REG_IPC_FIFO_RX ;


#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileReading ) ;

#endif // CEYLAN_SAFE_FIFO

	return res ;
	
}




FIFOElement readBlocking()
{

#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileReading ) ;

#endif // CEYLAN_SAFE_FIFO

	
	uint32 attemptCount = 100000 ;
	
	/* Active waiting preferred to atomicSleep(): */

	while ( ! dataAvailableForReading() && attemptCount > 0 )
		 attemptCount-- ;
		
	/* readBlocking: never ending ? */
	if ( attemptCount == 0 )
	{
	
		setError( FIFOTimeOutWhileReading ) ;
				 
		/* Active waiting preferred to atomicSleep(): */
		while( ! dataAvailableForReading() )
			;

		/* Recovered: */
		unsetErrorStatus() ;
		setStatusWord( StatusReset ) ;
	
	}

	
#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileReading ) ;

#endif // CEYLAN_SAFE_FIFO
		
		
	FIFOElement res = REG_IPC_FIFO_RX ;
	
	
#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileReading ) ;

#endif // CEYLAN_SAFE_FIFO


	return res ;
	
}



void write( FIFOElement toSend )
{
	
#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO


	if ( ! spaceAvailableForWriting() )
		setError( FIFOErrorWhileWriting ) ;
		
		
#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO
		
		
	REG_IPC_FIFO_TX = toSend ;


#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO
	
}



void writeBlocking( FIFOElement toSend )
{

#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO


	uint32 attemptCount = 100000 ;
	
	/* Active waiting preferred to atomicSleep(): */

	while ( ! spaceAvailableForWriting() && attemptCount > 0 )
		 attemptCount-- ;
	
	
	/* writeBlocking: never ending ? */
	if ( attemptCount == 0 )
	{
	
		setError( FIFOTimeOutWhileWriting ) ;
		
		/* Triggers the ARM9 if it can help to make some FIFO room: */
		sendSynchronizeInterruptToARM9() ;
		
		while ( ! spaceAvailableForWriting() )
			;

		/* Recovered: */
		unsetErrorStatus() ;
		setStatusWord( StatusReset ) ;
			
	}
		
#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO

	
	REG_IPC_FIFO_TX = toSend ;


#if CEYLAN_SAFE_FIFO

	if ( REG_IPC_FIFO_CR & IPC_FIFO_ERROR )
		setError( FIFOErrorWhileWriting ) ;

#endif // CEYLAN_SAFE_FIFO
	
}





/* Section dedicated to system-specific IPC */


	
void handleStatusInitRequest()
{
	
	if ( statusWordPointer != 0 || IPCRunning )
	{
	
		setError( IPCAlreadyStarted ) ;
		return ;
		
	}	
	
	
	/* 
	 * The ARM9 will send the address of the shared ARM7
	 * status word in next element: 
	 */
	statusWordPointer = (volatile ARM7StatusWord*) readBlocking() ;
	
	if ( *statusWordPointer != NoStatusAvailable )
	{
	
		setError( IncorrectInitialStatus ) ;
		return ;
		
	}	
		
	*statusWordPointer = ARM7Running ;
	
	
	errorWordPointer = (volatile ARM7ErrorCode*) 
		readBlocking() ;
	
	if ( *errorWordPointer != NoErrorAvailable )
	{
	
		setError( IncorrectInitialError ) ;
		return ;
		
	}	
		
	*errorWordPointer = NoError ;

	/* Set it last to avoid main loop firing too soon: */
	IPCRunning = true ;
								
} 



void handleShutdownIPCRequest()
{
	
	if ( statusWordPointer == 0 || errorWordPointer == 0 || ! IPCRunning )
	{
	
		setError( IPCAlreadyStopped ) ;
		return ;
		
	}	
	
	IPCRunning = false ;
	
	/* 
	 * Stop the mechanism on the ARM7 side.
	 * 
	 * Ends with ARM7IPCShutdown status (and NoError error 
	 * code) to perform last handshake with ARM9.
	 *
	 * @see FIFO::deactivate
	 *
	 */
	*statusWordPointer = ARM7IPCShutdown ;
	*errorWordPointer = NoError ;

	statusWordPointer = 0 ;
	errorWordPointer = 0 ;
	
	
	REG_IPC_FIFO_CR = REG_IPC_FIFO_CR & ~IPC_FIFO_ENABLE ;
	
	irqDisable( IRQ_IPC_SYNC ) ;
	
	/* 
	 * IRQ_VBLANK not disabled, as can be used for other reasons 
	 *
	 */
	
}



void handlePlaySoundRequest( FIFOElement firstElement )
{
	
	s32 channel = getFreeSoundChannel() ;	
	
	if ( channel == -1 )
	{
	
		/* Notifies the ARM9 the playback could not be performed: */
		
		/* Interrupts already disabled in this handler */

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
	
	/* Stereo not supported yet (needs the use of two CHANNELs) */
	
	SCHANNEL_CR(channel) = SCHANNEL_ENABLE | SOUND_ONE_SHOT 
		| ( ( (firstElement & 0x00f00000) == 0x00100000 ) ? 
			( SOUND_8BIT ) : ( SOUND_16BIT ) )
		| SOUND_VOL(0x7F) ;	
		
}



/* 
 * Triggers a refill by the ARM9 of the first part of the encoded buffer.
 *
 */
void triggerFirstBufferRefill()
{
				
	InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;

	writeBlocking( prepareFIFOCommand( FirstBufferFillRequest ) ) ; 

	setEnabledInterrupts( previous ) ;
	
	notifyCommandToARM9() ;

}



/* 
 * Triggers a refill by the ARM9 of the second part of the encoded buffer.
 *
 */
void triggerSecondBufferRefill()
{
				
	InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;

	writeBlocking( prepareFIFOCommand( SecondBufferFillRequest ) ) ; 
	
	setEnabledInterrupts( previous ) ;

	notifyCommandToARM9() ;

}


int counter = 0 ;

/**
 * Decodes next MP3 frame and handles state variables accordingly.
 *
 * @return true if succeeded, false otherwise.
 *
 */
bool decodeNextMP3Frame()
{

	/* Manage end of encoded buffer: */
	if ( readPointer > endOfSafeRead )
	{
	
		/*
		 * Here last decoding ate enough bytes to put the read pointer is the
		 * unsafe zone (not sure there is enough data for a whole encoded MP3
		 * frame), we have to move this delta zone from the end of second
		 * buffer to the beginning of first one:
		 * 
		 * - for the destination: let alpha be the number of remaining bytes 
		 * if read buffer. alpha = doubleBuffer + 2*bufferSize - readPointer
		 * Destination is ( doubleBuffer + frameSizeUpperBound - alpha ) so that
		 * these remaining bytes will be moved just before the next ones to 
		 * be played in first buffer.
		 * Thus Destination is finally: 
		 * doubleBuffer + frameSizeUpperBound -doubleBuffer -2*bufferSize 
		 *     + readPointer
		 *   = readPointer - ( 2*bufferSize - frameSizeUpperBound )
		 *   = readPointer - destinationOffset
		 *
		 * - for the size: from readPointer to the end of buffer, Size is:
		 * doubleBuffer + 2 * bufferSize - readPointer 
		 * i.e. Size = sizeOffset - readPointer
		 *
		 * @note Unable to remove the warning
		 * 'cast discards qualifiers from pointer target type' for
		 * second argument (volatile):
		 */
		memmove( 
			/* destination */ (Byte*) (readPointer - destinationOffset), 
			/* source */ ((Byte *) readPointer), 
			/* size */ (size_t) ( sizeOffset - (int) readPointer ) ) ;
		
		/* 
		 * Preparing that second buffer refill immediately.
		 * 
		 * The ARM9 should have already refilled the first buffer and possibly
		 * triggered the end of stream, thus last data will be in first buffer:
		 * (and second buffer just fully taken into account now)
		 *
		 */
		if ( ! endOfStreamDetected )
			triggerSecondBufferRefill() ;

		/* Go to the memmove-destination point in first buffer: */
		readPointer = (Byte *) (readPointer - ((Byte *) destinationOffset)) ;
		
		readingFirstBuffer = true ;
			
	}

	BufferSize deltaReadPointer = (BufferSize) readPointer ;
	
	
	/*
	 * Locates the next byte-aligned sync word in the raw mp3 stream: 
	 *
	 * @note We suppose here that the ARM9 already refilled at least the 
	 * beginning of second buffer if needed, we do not track precisely the
	 * number of readable (valid) bytes.
	 *
	 */
	int syncOffset = MP3FindSyncWord( 
		/* start */ (unsigned char *) readPointer,
		/* max readable length: full rest of buffer, memmove-size */ 
			sizeOffset - readPointer ) ;
	
	if ( syncOffset < 0 )
	{
		setError( HelixSyncWordNotFound ) ;
		decodingInError = true ;
		return false ;
	}
	
	readPointer += syncOffset ;
	
	/* 
	 * Same assumption here, already refilled if needed: 
	 * (variable will be updated by Helix, but not used here afterwards)
	 *
	 */
	int bytesLeft = sizeOffset - readPointer ;
	
		
	/* Decodes one frame of MP3 data: */
	int errorCode = MP3Decode( 
		/* decoder instance */ currentDecoder, 
		/* pointer to encoded buffer */ &readPointer,
		/* number of valid bytes */ &bytesLeft,
		/* buffer for decoded data */ (short*) decodePointer,
		/* useSize: normal MPEG format */ 0 ) ;

	if ( errorCode != 0 )
	{
		
		decodingInError = true ;

		switch ( errorCode )
		{
	
			case ERR_MP3_INDATA_UNDERFLOW:
				setError( HelixFoundTruncatedFrame ) ;
				return false ;
				break ;
			
			case ERR_MP3_MAINDATA_UNDERFLOW:
				setError( HelixLacksDataInBitReservoir ) ;
				return false ;
				break ;

			case ERR_MP3_FREE_BITRATE_SYNC:
				setError( HelixLacksFreeBitrateSlot ) ;
				return false ;
				break ;
		
			/* For:
			 	ERR_MP3_NULL_POINTER, 
				ERR_MP3_INVALID_FRAMEHEADER, 
				ERR_MP3_INVALID_SIDEINFO, 
				ERR_MP3_INVALID_SCALEFACT, 
				ERR_MP3_INVALID_HUFFCODES, 
				ERR_MP3_INVALID_DEQUANTIZE, 
				ERR_MP3_INVALID_IMDCT, 
				ERR_MP3_INVALID_SUBBAND 
				and any other
			 */
			default:
				setError( HelixDecodingError ) ;
				return false ;
				break ;
			
		}
	}
	
	/* Toggle halves in decoded buffer: */
	if ( decodePointer == decodedBuffer )
		decodePointer += DecodedFrameLength ;
	else
		decodePointer = decodedBuffer ;
			
		
	/*
	 * @note Must stay deactivated as readPointer already updated by MP3Decode.
	 *
	 * Takes into account the data read by MP3Decode: MP3Decode used 
	 * U =( sizeOffset - readPointer ) - bytesLeft bytes, thus 
	 * readPointer = readPointer + U = sizeOffset - bytesLeft
	readPointer = sizeOffset - bytesLeft ;
	 */
	deltaReadPointer = ( (BufferSize) readPointer ) - deltaReadPointer ;
	
	if ( deltaReadPointer > maxWholeFrameLength )
		maxWholeFrameLength = deltaReadPointer ;
	
	if ( deltaReadPointer < minWholeFrameLength )
		minWholeFrameLength = deltaReadPointer ;
			
	
	/* Detects transition from first to second read buffer: */
	if ( readingFirstBuffer && ( readPointer > secondBuffer ) 
		&& ( ! endOfStreamDetected ) )
	{	
	
		triggerFirstBufferRefill() ;
		readingFirstBuffer = false ;
		
	}	

	/* Updates frame informations: */
	MP3GetLastFrameInfo( currentDecoder, &frameInfo ) ;
	
	if ( counter++ % 20 == 0 )
	{
	
		InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;
	
		writeBlocking( prepareFIFOCommand( MusicFrameInformation )
			| ( frameInfo.outputSamps & 0x0000ffff ) ) ;
	
		writeBlocking( deltaReadPointer ) ;
	
		setEnabledInterrupts( previous ) ;

		notifyCommandToARM9() ;
	
	}

	if ( 2 * frameInfo.outputSamps != DecodedFrameLength )
		setError( HelixUnexpectedDecodedLength ) ;
	
	return true ;
	
}


 
/**
 * Request the ARM7 to play specified music.
 *
 * A boolean parameter is set in the command element: its last bit tells 
 * whether the playback should start from first buffer (if 1) or from 
 * second (if 0).
 * 
 * The second element is a pointer to the cache-aligned sound buffer, 
 * third is composed of the full size of the encoded buffer (16 first bits),
 * then the delta value (the MP3 frame size upper bound, 16 last bits).
 *
 * No answer expected.
 *
 * @see StopCurrentMusic
 *
 */
void handlePlayMusicRequest( FIFOElement firstElement )
{
	
	readingFirstBuffer = (bool) (firstElement & 0x00000001) ;
	
	/* Reads address of the ARM9-allocated buffer: */
	doubleBuffer = (Byte *) readBlocking() ;
	
	uint32 secondElement = (uint32)	readBlocking() ;
	
	bufferSize = ( secondElement >> 16 ) ;
		
	secondBuffer = doubleBuffer + bufferSize ;

	frameSizeUpperBound = secondElement & 0x0000fffff ;
	

	/* Either start after delta or at the beginning of the second buffer: */
	if ( readingFirstBuffer )
		readPointer = doubleBuffer + frameSizeUpperBound ;
	else
		readPointer = secondBuffer ;
	
	/* Precomputations: */
	destinationOffset = (Byte *) ( 2*bufferSize - frameSizeUpperBound ) ;
	sizeOffset = doubleBuffer + 2*bufferSize ;

	/* Points to the beginning of the delta zone in second buffer: */
	endOfSafeRead = doubleBuffer + (int) destinationOffset ;
	
	/* Set control flags: */
	startMP3PlaybackRequested = true ;
	endOfStreamDetected = false ;
	decodingInError = false ;
	
	/* 
	 * Must be able to contain two full decoded frames: 
	 * MAX_NCHAN = 2 (stereo), MAX_NGRAN = 2 granules, MAX_NSAMP = 576 samples,
	 * hence 2*1152 = 2*DecodedFrameLength
	 *
	 * @note Use only  2 * MAX_NGRAN * MAX_NSAMP = 2 * 1152 if mono.
	 *
	 */
	decodedBuffer = (Byte *) malloc( 2 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP ) ;
	
	/* Starts the output at the beginning: */
	decodePointer = decodedBuffer ;
	
}



/**
 * Handles the ARM9 notification that the end of encoded buffer has been 
 * reached.
 *
 */
void handleEndOfEncodedStreamReached()
{

	endOfStreamDetected = true ;
	
}


/* Ceylan system-specific command handler */
void handleReceivedSystemSpecificCommand( FIFOCommandID commandID, 
	FIFOElement firstElement )
{

	/*
	 * Here we are dealing with a system-specific (Ceylan) command.
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
	
	switch( commandID )
	{
	
		case HelloToTheARM7:
			// Corresponds to reading zero in the FIFO: 
			setError( UnexpectedBehaviour ) ;
			break ;
			
		case ShutdownIPCRequest:
			handleShutdownIPCRequest() ;
			break ;
			
		case StatusInitRequest:
			handleStatusInitRequest() ;
			break ;
								
		default:
			// Unexpected system command id: 
			setError( UnexpectedSystemCommand ) ;
			break ;		
	
	}

	*/

	if ( commandID == HelloToTheARM7 )
		setError( UnexpectedBehaviour ) ;
	else if ( commandID == ShutdownIPCRequest )
		handleShutdownIPCRequest() ;
	else if ( commandID == StatusInitRequest )
		handleStatusInitRequest() ;
	else
		setError( UnexpectedSystemCommand ) ;
		
}



/* OSDL-specific command handler */
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
				
		case PlaySoundRequest:
			handlePlaySoundRequest( firstElement ) ;
			break ;
			
		case PlayMusicRequest:
			handlePlayMusicRequest( firstElement ) ;
			break ;
			
		case EndOfEncodedStreamReached:
			handleEndOfEncodedStreamReached() ;
			break ;
			
		default:
			setError( UnexpectedOSDLCommand ) ;
			break ;
				
	}
	
	*/
	 
	if ( commandID == PlaySoundRequest )
		handlePlaySoundRequest( firstElement ) ;
	else if ( commandID == PlayMusicRequest )
		handlePlayMusicRequest( firstElement ) ;
	else if ( commandID == EndOfEncodedStreamReached )
		handleEndOfEncodedStreamReached() ;
	else
		setError( UnexpectedOSDLCommand ) ;
		
}



/* Example of application-specific command handler */
void handleReceivedApplicationCommand( FIFOCommandID commandID, 
	FIFOElement element )
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
			setError( UnexpectedApplicationCommand ) ;
			break ;
				
	}

	*/	 

	setError( UnexpectedApplicationCommand ) ;
	
}



/*
 * Command dispatcher, between system-specific and application-specific ones. 
 *
 * Called by the IPCSYNC handler (main notification used).
 *
 * Called by the VBlank handler, to poll the FIFO queue, as IPC sync IRQ may
 * by lost if sent while managing a previous sync IRQ, delaying the processing.
 * until next sync IRQ or until the FIFO queue gets full.
 *
 */
void handleReceivedCommand()
{

	if ( ! dataAvailableForReading() )
	{
	
		/*
		 * Is not a real error, as may be triggered by the ARM9 read/write
		 * blocking operations when waiting for too long the ARM7:
		  
		setError( AwokenWithNothingToRead ) ;	
		 
		 *
		 */
		return ;
	
	}	

		
	/* 
	 * Interrupts (if used, FIFO, VBlank, IPCSync) are expected to be
	 * disabled when this function is called.	
	 *
	 */
	 
#if CEYLAN_SAFE_FIFO

	static bool CommandInProgress = false ;


	if ( ! CommandInProgress )
	{
	
		CommandInProgress = true ;

#endif // CEYLAN_SAFE_FIFO

			
		FIFOElement firstElement ;
		FIFOCommandID id ;
	
	
		/* At least one first element to read, maybe more: */
		while ( dataAvailableForReading() )
		{
	
			/* Read first the command identifier: */
	
	 		/* readBlocking instead of read: increased safety ? */
			firstElement = readBlocking() ;


#if CEYLAN_SAFE_FIFO
		
			FIFOCommandCount count = getFIFOCommandCountFrom( firstElement ) ;
	
			if ( count != remoteCommandCount )
			{
		
				setError( UnexpectedCommandCount ) ;
				
				/* CommandInProgress still true, hence frozen if in safe mode.*/
				return ;	
			
			}
		
			remoteCommandCount++ ;
			
#endif // CEYLAN_SAFE_FIFO


			id = getFIFOCommandIDFrom( firstElement ) ;
			
			if ( id > 127 )
			{
	
				/* It is an application-specific command, relay it: */
				handleReceivedApplicationCommand( id, firstElement ) ;
	
			}
			else 
			{
						
				if ( id < 33 )
				{
			
					/* It is a Ceylan-specific command, relay it: */
					handleReceivedSystemSpecificCommand( id, firstElement ) ;
				
				}
				else	
				{

					/* It is a command for OSDL, relay it: */
					handleReceivedIntegratingLibrarySpecificCommand( id,
						firstElement ) ;
				
				}
				
			} 
	
			incrementProcessCount() ;
			
					
		} /* end while */


#if CEYLAN_SAFE_FIFO

		CommandInProgress = false ;	
	
	}
	else
	{
	
		setError( CommandOverlapping ) ;
		
	}
	
#endif // CEYLAN_SAFE_FIFO
					
}




/**
 * Sets-up the IPC mechanism by synchronizing with the ARM9 and retrieving
 * the status and error variables used by the ARM7 to report its state.
 *
 */
void initCeylanIPC()
{


	/*
	 * IPC_SYNC_IRQ_ENABLE allows the ARM9 to trigger IPC_SYNC IRQ on this
	 * ARM7:
	 *
	 * ('=', not '|=', to nullify the rest of the register, not expecting to
	 * write on ARM9 settings)
	 *
	 */
	REG_IPC_SYNC = IPC_SYNC_IRQ_ENABLE ;


	/*
	 * First, set-up the FIFO.
	 *
	 * REG_IPC_FIFO_CR is the FIFO *control* register, and:
	 *  - IPC_FIFO_ENABLE enables the FIFO
	 *  - IPC_FIFO_SEND_CLEAR flushes the send FIFO
	 *  - (not used anymore) IPC_FIFO_RECV_IRQ request an IRQ to be triggered 
	 * on a transition, on the receive FIFO, from empty to not empty	 
	 *
	 */
  	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR ;


	/*
	 * FIFO not empty IRQ (IRQ_FIFO_NOT_EMPTY) not used anymore, as 
	 * IRQ_IPC_SYNC is considered more appropriate.
	 *
	 
	irqSet( IRQ_FIFO_NOT_EMPTY, syncHandlerForFIFO ) ; 
    irqEnable( IRQ_FIFO_NOT_EMPTY | etc.) ;
	
	 */
	 
	 
	/*
	 * Fully managed by the Ceylan FIFO system:
	 *
	 */

	irqSet( IRQ_IPC_SYNC, handleReceivedCommand ) ; 

	/* Needed for atomic sleeps and increased reliability: */
	irqSet( IRQ_VBLANK, handleReceivedCommand ) ;
	
	/* Unleashes these IRQ: */
	irqEnable( IRQ_IPC_SYNC | IRQ_VBLANK ) ;


	/*
	 * Wait until the IPC system is up and running (ARM9 handshake performed).
	 *
	 */
	while( ! IPCRunning )
		;

	/* Let some time elapse to get out of the IRQ handler for IPC startup: */
	atomicSleep() ;
	
}



/**
 * Sets-up the sound hardware and software.
 *
 */
void initOSDLSound()
{

	InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;

	powerON( POWER_SOUND ) ;

	writePowerManagement( PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG)
		& ~PM_SOUND_MUTE ) | PM_SOUND_AMP ) ;
	
	SOUND_CR = SOUND_ENABLE | SOUND_VOL( 0x7f ) ;
	
	IPC->soundData = 0 ;

	setEnabledInterrupts( previous ) ;
	
	/* Initializes the Helix MP3 decoder: */
	currentDecoder = MP3InitDecoder() ;

	if ( currentDecoder == 0 )
	{
		
		setError( HelixInitializationFailed ) ;
		return ;
	
	}	
		
	/* Wait a bit to let sound hardware be initialized: */
	atomicSleep() ;
	
}



/**
 * Shuts down the sound hardware and software.
 *
 */
void shutdownOSDLSound()
{

	if ( currentDecoder != 0 )
	{
		
		MP3FreeDecoder( currentDecoder ) ;
		currentDecoder = 0 ;
	
	}	
	
}



/**
 * Manages the MP3 playback when requested.
 *
 */
void manageMP3Playback()
{

	/* 
	 * The ARM7 main loop will be hijacked here as long as the music playback
	 * is not over.
	 *
	 */
	startMP3PlaybackRequested = false ;
	

	/* 
	 * Starts by decoding the first two frames, so that as soon as the second
	 * is played (overflow of timer 2), the third starts being decoded)
	 *
	 */
	if ( ! decodeNextMP3Frame() )
		return ;
	
	/* Second one: */
	decodeNextMP3Frame() ;
		
	SCHANNEL_REPEAT_POINT( MusicChannel ) = 0 ;

	/* Requests the audio hardware to read its data from decoded buffer: */	
	SCHANNEL_SOURCE( MusicChannel ) = ( u32 ) decodedBuffer ;
	
	/* 
	 * DecodedFrameLength samples per frame, 2 frames, 16 bits per sample: 
	 * (length is in 32-bit words, hence the division by 4 with >>2)
	 *
	 */
	SCHANNEL_LENGTH( MusicChannel ) = ( DecodedFrameLength * 2 ) >> 2 ;
	
	/* Reads sample rate from decoded frame info, set by decoding: */
	SCHANNEL_TIMER( MusicChannel ) = SOUND_FREQ( frameInfo.samprate ) ;
  	
	/* Main point here is the repeat setting, over the decode buffer: */
	SCHANNEL_CR( MusicChannel ) = SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_16BIT
		| SOUND_VOL( 0x7f ) | SOUND_PAN( 0 ) ;
	
	/* First timer must run at twice the sample rate since 16-bit samples: */	
	TIMER0_DATA = SOUND_FREQ( frameInfo.samprate )  ;
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1 ;
	
	/*
	 * Each of the previous ticks makes the second timer eat 1 out of
	 * DecodedFrameLength: 
	 */
	TIMER1_DATA = 65536 - DecodedFrameLength ;
	TIMER1_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_CASCADE ;
	
	/* This third timer counts finally full MP3 frames: */
	TIMER2_DATA = 0 ;
	TIMER2_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_CASCADE ;
	
	int CurrentFrameCounter ;
	int LastFrameCounter = 0 ;
	
	do 
	{
		
		CurrentFrameCounter = TIMER2_DATA ;
	
		/*
		 * As soon as we change of half in the output buffer, decode the other
		 * half:
		 *
		 */
		while ( CurrentFrameCounter > LastFrameCounter ) 
		{
		
			decodeNextMP3Frame() ;	
			LastFrameCounter++ ;
			
		}
		
	} while ( ! endOfStreamDetected && ! decodingInError ) ;
	
	/* Wait for the playback of final buffer : */
	CurrentFrameCounter = ( TIMER2_DATA + 1 ) ;
	
	while ( TIMER2_DATA == CurrentFrameCounter )
		swiWaitForVBlank() ;
	
	/* Stop channel: */
	SCHANNEL_CR( MusicChannel ) = 0 ;
	
	
	/* Let the ARM9 be notified of the music end here: */
	
	InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;

	writeBlocking( prepareFIFOCommand( MusicEndedNotification ) ) ; 
	
	writeBlocking( minWholeFrameLength ) ;
	writeBlocking( maxWholeFrameLength ) ;
	
	setEnabledInterrupts( previous ) ;

	notifyCommandToARM9() ;	
	
}



int main(int argc, char ** argv) 
{

	/* Reset the clock if needed: */
	rtcReset() ;

	irqInit() ;

	SetYtrigger( 80 ) ;
	irqSet( IRQ_VCOUNT, VcountHandler ) ;
	irqEnable( IRQ_VCOUNT ) ;

	IPC->mailBusy = 0 ;

	initCeylanIPC() ;
	
	initOSDLSound() ;

	
	/*
	 * Wait for ever, otherwise the runtime will believe the ROM has crashed.
	 *
	 */
	while( true )
	{
	
		atomicSleep() ;
		
		/* Sets if a handlePlayMusicRequest call is performed: */
		if ( startMP3PlaybackRequested ) 
			manageMP3Playback() ;
		
	}
	
}

