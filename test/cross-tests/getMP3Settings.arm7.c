

/**
 * ARM7 side of the OSDL MP3 settings test.
 *
 * @see testOSDLRawSound.arm9.cc for the peer implementation
 * 
 * @note Several warnings are still there:
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
 * Not using OSDL, as this executable is made to extract settings for it
 * (needing special code for that, different from the OSDL base).
 *
 */
 
/* To have Ceylan defines so that Ceylan C headers can be directly included: */
#include "OSDLConfigForNintendoDS.h"

/* For Ceylan ARM base: */
#include "CeylanARM7Base.h"

/* For Helix decoder: */
#include "Helix/mp3dec.h"


/* Defines the actual OSDL ARM7 status words and error codes. */
#include "OSDLARM7Codes.h"

/* Defines OSDL IPC command identifiers. */
#include "OSDLIPCCommands.h"


#include "stdlib.h" /* for malloc */
#include "string.h" /* for memmove */



/*
 * Implementation notes:
 * 
 * Uses a different ARM9CommandID PlayMusicRequest = 34 than OSDL, to report
 * more information about the MP3 playback, and a different load request, as
 * no header can be used here (th goal being to find the values to put in that
 * header).
 *
 */



/* Definitions of global variables */



/* Music decoding section */


/** Tells whether a MP3 play command is being processed. */
volatile bool startMP3PlaybackRequested = false ;


/**
 * The state of the Helix decoder instance.
 *
 */
volatile HMP3Decoder currentDecoder ;


/** Gathers informations about a decoded MP3 frame, needed for playback: */
volatile MP3FrameInfo frameInfo ;



/* Read encoded MP3 data subsection. */


/** The size of a (simple) buffer, in bytes: */
volatile BufferSize bufferSize ;


/** The smallest upper bound chosen to a MP3 frame size: */
volatile BufferSize frameSizeUpperBound ;


/** The minimum size read for a MP3 frame, including the frame and its sync: */
volatile BufferSize minWholeFrameLength = (BufferSize) -1 ;

/** The maximum size read for a MP3 frame, including the frame and its sync: */
volatile BufferSize maxWholeFrameLength = 0 ;



/**
 * The actual double sound buffer, two simple buffers, one after the
 * other (so the first half buffer has the same address as this
 * double one).
 *
 */
volatile Byte * doubleBuffer ;


/**
 * The actual address of the second sound buffer.
 *
 */
volatile Byte * secondBuffer ;


/** The pointer to current bytes being read in the encoded buffer: */
volatile Byte * readPointer ;


/* *
 * Tells whether the first buffer is being read (if true) or the second one 
 * (if false).
 */
volatile bool readingFirstBuffer ;


/** 
 * The pointer to the byte in the encoded buffer from which no more MP3
 * frame should be read, as the remaining space could be too small to contain
 * a full frame: 
 *
 */
volatile Byte * endOfSafeRead ;


/** Precomputes an offset for the moving of encoded data: */
volatile Byte * destinationOffset ;

/** Precomputes an offset for the moving of encoded data: */
volatile Byte * sizeOffset ;


/** Tells whether the end of the MP3 stream has been detected by the ARM9. */
volatile bool endOfStreamDetected ;

/** Tells whether the end of the MP3 stream has been detected by the ARM9. */
volatile bool endOfLastBufferReached ;

/** Tells whether the decoding encountered a fatal error. */
volatile bool decodingInError ;



/* Output decoded samples subsection. */


/**
 * The buffer where decoded music will be output.
 * Will be directly played by the sound hardware.
 *
 * @note Allocated directly from the ARM7.
 *
 */
volatile Byte * decodedBuffer ;

/* The pointer to the place in decodedBuffer where decoding should be output: */
volatile Byte * decodePointer ;


/* *
 * Each decoded MP3 frame should result in samples of this size, in bytes: 
 * (at least all the mono mp3 streams generated by lame we tested output
 * 576 samples per frame, not depending on the bit rate).
 * (multiplied by two as 16-bit).
 *
 */
const uint16 DecodedFrameLength = 576 * sizeof(uint16) ;


/** Last channel (15) is the one reserved to music (mono) */
const uint8 MusicChannel = 15 ;




/* Helper functions */


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



/** 
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
		else
			endOfLastBufferReached = true ;	

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
	if ( readingFirstBuffer && ( readPointer > secondBuffer ) )
	{	
	
		if ( endOfStreamDetected )
		{
	
			endOfLastBufferReached = true ;
		
		}
		else
		{	
				
			triggerFirstBufferRefill() ;
			readingFirstBuffer = false ;
		
		}	
		
	}
	
	
	/* Updates frame informations: */
	MP3GetLastFrameInfo( currentDecoder, &frameInfo ) ;
	
	/* Sends the current frame size from time to time: */
	if ( counter++ % 11 == 0 )
	{
	
		InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;
	
		writeBlocking( prepareFIFOCommand( MusicFrameInformation )
			| ( frameInfo.outputSamps & 0x0000ffff ) ) ;
	
		writeBlocking( deltaReadPointer ) ;
	
		setEnabledInterrupts( previous ) ;

		notifyCommandToARM9() ;
	
	}
	
	if ( sizeof(uint16) * frameInfo.outputSamps != DecodedFrameLength )
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
	endOfLastBufferReached = false ;
	decodingInError = false ;
	
	/* 
	 * Must be able to contain two full decoded frames: 
	 * MAX_NCHAN = 2 (stereo), MAX_NGRAN = 2 granules, MAX_NSAMP = 576 samples,
	 * hence 2*1152 = 2*DecodedFrameLength
	 *
	 * @note Use only  2 * MAX_NGRAN * MAX_NSAMP = 2 * 1152 if mono.
	 *
	 */
	decodedBuffer = (Byte *) malloc( 
		2 * /* MAX_NCHAN */ 2 * MAX_NGRAN * MAX_NSAMP ) ;
	
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
	 
	if ( commandID == PlayMusicRequest )
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






/**
 * Sets-up the sound hardware and software.
 *
 */
void initOSDL()
{

	/* Includes IPC and basic sound setup: */
	initCeylan() ;

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
		
	} while ( ! endOfLastBufferReached && ! decodingInError ) ;
	
	/* Wait for the playback of final frame: */
	CurrentFrameCounter = ( TIMER2_DATA + 1 ) ;
	
	while ( TIMER2_DATA == CurrentFrameCounter )
		atomicSleep() ;
	
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

	initOSDL() ;

	
	/*
	 * Wait for ever, otherwise the runtime will believe the ROM has crashed.
	 *
	 */
	while( true )
	{
	
		atomicSleep() ;
		
		/* Set if a handlePlayMusicRequest call was triggered: */
		if ( startMP3PlaybackRequested ) 
			manageMP3Playback() ;
		
	}
	
}

