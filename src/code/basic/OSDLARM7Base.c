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


#pragma GCC diagnostic ignored "-Wsign-compare"


/* To have Ceylan defines so that Ceylan C headers can be directly included: */
#include "OSDLConfigForNintendoDS.h"


/* Must be on second position (needs Ceylan defines). */
#include "OSDLARM7Base.h"



/* Defines the actual OSDL ARM7 status words and error codes. */
#include "OSDLARM7Codes.h"


/* Defines OSDL IPC command identifiers. */
#include "OSDLIPCCommands.h"



/*
 * Implementation notes:
 *
 * CeylanARM7Base.c counterpart, gathers all OSDL-specific add-ons to the IPC
 * system, notably for sounds and musics, thanks to the Helix MP3 decoder.
 *
 */
#include "stdlib.h" /* for malloc */
#include "string.h" /* for memmove */



/* For Helix decoder: */
#include "helix/pub/mp3dec.h"




/* Music decoding section */


/** Tells whether a MP3 play command is being processed. */
volatile bool startMP3PlaybackRequested = false ;


/** Tells whether a MP3 pause command is being processed. */
volatile bool pauseMP3PlaybackRequested = false ;



/**
 * The state of the Helix decoder instance.
 *
 */
volatile HMP3Decoder currentDecoder ;



/** Gathers informations about a decoded MP3 frame, needed for playback: */
volatile MP3FrameInfo frameInfo ;


typedef uint16 SampleRate ;


/** The frequency, in Hz, of (decoded) output samples, a.k.a. sample rate: */
volatile SampleRate outputSampleRate ;


const SampleRate DefaultSampleRate = 22050 /* Hz */ ;



/* Read encoded MP3 data subsection. */


/** The size of a (simple) buffer, in bytes: */
volatile BufferSize bufferSize ;


/** The smallest upper bound chosen to a MP3 frame size: */
volatile BufferSize frameSizeUpperBound ;



/**
 * The actual double sound buffer, two simple buffers, one after the other (so
 * the first half buffer has the same address as this double one).
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


/**
 * Tells whether the first buffer is being read (if true) or whether it is
 * the second one (if false).
 */
volatile bool readingFirstBuffer ;



/**
 * The pointer to the byte in the encoded buffer from which no more MP3 frame
 * should be read, as the remaining space could be too small to contain a full
 * frame:
 */
volatile Byte * endOfSafeRead ;



/** Precomputes an offset for the moving of encoded data: */
volatile Byte * destinationOffset ;


/** Precomputes an offset for the moving of encoded data: */
volatile Byte * sizeOffset ;



/** Tells whether the end of the MP3 stream has been detected by the ARM9. */
volatile bool endOfStreamDetected ;


/** Tells whether, after an end of stream, we are reading last buffer. */
volatile bool readingLastBuffer ;



/** The number of volume level increments during a music fade-in. */
volatile uint8 musicFadeInIncrement ;


/** The number of volume level decrements during a music fade-pout. */
volatile uint8 musicFadeOutDecrement ;



/**
 * Tells whether the end of processing of mp3 frames is requested, either
 * because the last frame of the last buffer has been processed or the ARM9
 * requested to stop.
 *
 */
volatile bool endOfProcessingReached ;



/** Tells whether the decoding encountered a fatal error. */
volatile bool decodingInError ;





/* Output decoded samples subsection. */


/**
 * The buffer where decoded music will be output.
 *
 * Will be directly played by the sound hardware.
 *
 * @note Allocated directly from the ARM7.
 *
 */
volatile Byte * decodedBuffer ;



/** The size of PCM final output buffer.
 *
 * Must be able to contain two full decoded frames:
 * MAX_NCHAN = 2 (stereo), MAX_NGRAN = 2 granules, MAX_NSAMP = 576 samples,
 * hence 2*1152 = 2*DecodedFrameLength
 *
 * @note Use only 2 * MAX_NGRAN * MAX_NSAMP = 2 * 1152 if mono, otherwise
 * use 2 * MAX_NCHAN * MAX_NGRAN * MAX_NSAMP if stereo.
 *
 */
const uint32 DecodedBufferSize = 2 * MAX_NGRAN * MAX_NSAMP ;



/**
 * The pointer to the place in decodedBuffer where decoding should be output:
 */
volatile Byte * decodePointer ;



/* Defines a channel volume. */
typedef uint8 ChannelVolume ;


/*
 * Music volume, 0..127=silent..loud.
 * Default: 127, loudest possible.
 *
 */
volatile ChannelVolume currentMusicVolume = 0x7f ;



/*
 * Overall sound volume, 0..127=silent..loud.
 * Default: 127, loudest possible.
 *
 */
const volatile ChannelVolume defaultSoundVolume = 0x7f ;




/* Defines a channel panning between speakers. */
typedef uint8 ChannelPanning ;



/*
 * Left/right music dispatching, 0..127=left..right, 64 is half volume on both
 * speakers.
 *
 * Default: 0, balanced volume.
 *
 */
volatile ChannelPanning currentMusicPanning = 0 ;



/*
 * Left/right music dispatching, 0..127=left..right, 64 is half volume on both
 * speakers.
 *
 * Default: 0, balanced volume.
 *
 */
volatile ChannelPanning defaultSoundPanning = 0 ;



/**
 * Each decoded MP3 frame should result in samples of this size, in bytes: (at
 * least all the mono mp3 streams generated by lame we tested output 576 samples
 * per frame, not depending on the bit rate).
 *
 * (multiplied by two as 16-bit, so 1152).
 *
 */
const uint16 DecodedFrameLength = 576 * sizeof(uint16) ;



/**
 * First music channel (0) is the one reserved to music (mono PCM).
 *
 * Using preferably the first one as channels 8-15 are special channels: 8-13
 * are the only channels that can also play PSG sounds and 14-15 the only that
 * can generate noise.
 *
 */
const uint8 MusicChannel = 0 ;



/**
 * Returns the number of the first free channel, otherwise -1 is all are busy.
 *
 * @note Channel 15 is reserved (for music).
 *
 */
s32 getFreeSoundChannel()
{

	int i ;

	/* Music channel never free (start at 1): */
	for ( i=1; i<16; i++ )
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
		 * frame), we have to move this delta zone from the end of second buffer
		 * to the beginning of first one:
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
		 * @note Unable to remove the warning 'cast discards qualifiers from
		 * pointer target type' for second argument (volatile):
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
		{

			triggerSecondBufferRefill() ;

		}
		else
		{

			if ( readingLastBuffer )
				endOfProcessingReached = true ;
			else
				readingLastBuffer = true ;

		}

		/* Go to the memmove-destination point in first buffer: */
		readPointer = (Byte *) (readPointer - ((Byte *) destinationOffset)) ;

		readingFirstBuffer = true ;

	}

	if ( endOfProcessingReached )
		return true ;

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

		/*
		 * Decoding proven to be functional, not freezing the ARM7 if the mp3
		 * encoding is incorrect:
		 *
		 */
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

#warning Next two warnings (MP3Decode/MP3GetLastFrameInfo), due to volatile type, cannot be removed.

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

		/*
		 * Decoding proven to be functional, not freezing the ARM7 if the mp3
		 * encoding is incorrect:
		 *
		 */

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

			// For:
			//	ERR_MP3_NULL_POINTER,
			//	ERR_MP3_INVALID_FRAMEHEADER,
			//	ERR_MP3_INVALID_SIDEINFO,
			//	ERR_MP3_INVALID_SCALEFACT,
			//	ERR_MP3_INVALID_HUFFCODES,
			//	ERR_MP3_INVALID_DEQUANTIZE,
			//	ERR_MP3_INVALID_IMDCT,
			//	ERR_MP3_INVALID_SUBBAND
			//	and any other

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


	/* Detects transition from first to second read buffer: */
	if ( readingFirstBuffer && ( readPointer > secondBuffer ) )
	{

		if ( endOfStreamDetected )
		{

			if ( readingLastBuffer )
				endOfProcessingReached = true ;
			else
				readingLastBuffer = true ;

		}
		else
		{

			triggerFirstBufferRefill() ;

		}

		readingFirstBuffer = false ;

	}

	/* Updates frame informations: */
	MP3GetLastFrameInfo( currentDecoder, &frameInfo ) ;

	/*
	 * Should never occur:
	 *

	if ( 2 * frameInfo.outputSamps != DecodedFrameLength )
		setError( HelixUnexpectedDecodedLength ) ;
	 *
	 */

	return true ;

}




/* Section dedicated to library integration of OSDL IPC. */



/** Manages an IPC request to play an OSDL sound. */
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
		| SOUND_VOL( defaultSoundVolume ) | SOUND_PAN( defaultSoundPanning ) ;

}



/**
 * Requests the ARM7 to play specified music.
 *
 * A boolean parameter is set in the command element: its last bit tells whether
 * the playback should start from first buffer (if 1) or from second (if 0).
 *
 * The second element is a pointer to the cache-aligned sound buffer, third is
 * composed of the full size of the encoded buffer (16 first bits), then the
 * delta value (the MP3 frame size upper bound, 16 last bits).
 *
 * No answer expected.
 *
 * @see StopMusicRequest
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
	pauseMP3PlaybackRequested = false ;
	endOfStreamDetected       = false ;
	readingLastBuffer         = false ;
	endOfProcessingReached    = false ;
	decodingInError           = false ;

	outputSampleRate = 0 ;

	musicFadeInIncrement  = 0 ;
	musicFadeOutDecrement = 0 ;

	/* decodedBuffer already allocated in initialization. */

	/* Starts the output at the beginning: */
	decodePointer = decodedBuffer ;

	/* Release it last, as this is the one the main loop is waiting for: */
	startMP3PlaybackRequested = true ;

}



/**
 * Triggers a music fade-in.
 *
 * The last 16 bits contains the requested fade-in duration, in milliseconds.
 *
 */
void handleFadeInMusicRequest( FIFOElement firstElement )
{

	/* Starts from mute: */
	currentMusicVolume = 0 ;

	uint16 requestedDuration = (firstElement & 0x0000ffff) ;

	SampleRate usedRate ;

	/* A mp3 frame already decoded? If yes, sample rate is known: */
	if ( outputSampleRate == 0 )
	{

		/* No, assuming default sample rate: */
		usedRate = DefaultSampleRate ;

	}
	else
	{

		usedRate = outputSampleRate ;

	}

	/*
	 * Number of frames necessary to go from zero volume to full volume:
	 * Ex: 2*3000*22050/(1000*1152) = 114
	 * (factor 2 is because DecodedFrameLength is in bytes whereas usedRate
	 * counts 16-bit samples)
	 *
	 */
	uint16 frameCount =
		2 * requestedDuration * usedRate / ( 1000 * DecodedFrameLength ) ;

	/* Actually 1.1 for the example, rounded to 1: */
	musicFadeInIncrement = 127 / frameCount ;

	if ( musicFadeInIncrement == 0 )
		musicFadeInIncrement = 1 ;

}



/**
 * Triggers a music fade-out.
 *
 * The last 16 bits contains the requested fade-out duration, in milliseconds.
 *
 */
void handleFadeOutMusicRequest( FIFOElement firstElement )
{

	uint16 requestedDuration = (firstElement & 0x0000ffff) ;

	SampleRate usedRate ;

	/* An mp3 frame already decoded? */
	if ( outputSampleRate == 0 )
	{

		/* No, assuming default sample rate (integer calculus): */
		usedRate = DefaultSampleRate ;

	}
	else
	{

		usedRate = outputSampleRate ;

	}

	/*
	 * Number of frames necessary far specified duration:
	 * Ex: 2*3000*22050/(1000*1152) = 114
	 * (factor 2 is because DecodedFrameLength is in bytes whereas usedRate
	 * counts 16-bit samples)
	 *
	 */
	uint16 frameCount = 2 * requestedDuration * usedRate
		/ ( 1000 * DecodedFrameLength ) ;

	/* Just for safety: */
	if ( frameCount == 0 )
		frameCount = 1 ;

	/* Starts from current volume: */
	musicFadeOutDecrement = currentMusicVolume / frameCount ;

	if ( musicFadeOutDecrement == 0 )
		musicFadeOutDecrement = 1 ;

}



/**
 * Requests the ARM7 to stop any music currently playing.
 *
 * No direct answer expected.
 *
 * @see PlayMusicRequest
 *
 */
void handleStopMusicRequest()
{

	endOfProcessingReached = true ;

}



void handlePauseMusicRequest()
{

	pauseMP3PlaybackRequested = true ;

}



void handleSetMusicVolumeRequest( FIFOElement firstElement )
{

	currentMusicVolume = firstElement & 0x000000ff ;

}



void handleUnpauseMusicRequest()
{

	pauseMP3PlaybackRequested = false ;

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

		case PlaySoundRequest:
			handlePlaySoundRequest( firstElement ) ;
			break ;

		case PlayMusicRequest:
			handlePlayMusicRequest( firstElement ) ;
			break ;

		case FadeInMusicRequest:
			handleFadeInMusicRequest( firstElement ) ;
			break ;

		case FadeOutMusicRequest:
			handleFadeOutMusicRequest( firstElement ) ;
			break ;

		case StopMusicRequest:
			handleStopMusicRequest() ;
			break ;

		case PauseMusicRequest:
			handlePauseMusicRequest() ;
			break ;

		case SetMusicVolumeRequest:
			handleSetMusicVolumeRequest( firstElement ) ;
			break ;

		case UnpauseMusicRequest:
			handleUnpauseMusicRequest() ;
			break ;

		case EndOfEncodedStreamReached:
			handleEndOfEncodedStreamReached() ;
			break ;

	}

	*/

	if ( commandID == PlaySoundRequest )
		handlePlaySoundRequest( firstElement ) ;
	else if ( commandID == PlayMusicRequest )
		handlePlayMusicRequest( firstElement ) ;
	else if ( commandID == FadeInMusicRequest )
		handleFadeInMusicRequest( firstElement ) ;
	else if ( commandID == FadeOutMusicRequest )
		handleFadeOutMusicRequest( firstElement ) ;
	else if ( commandID == StopMusicRequest )
		handleStopMusicRequest() ;
	else if ( commandID == PauseMusicRequest )
		handlePauseMusicRequest() ;
	else if ( commandID == SetMusicVolumeRequest )
		handleSetMusicVolumeRequest( firstElement ) ;
	else if ( commandID == UnpauseMusicRequest )
		handleUnpauseMusicRequest() ;
	else if ( commandID == EndOfEncodedStreamReached )
		handleEndOfEncodedStreamReached() ;
	else
		setError( UnexpectedOSDLCommand ) ;

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



void setMusicBasicSettings()
{

	/* Main point here is the repeat setting, over the decode buffer: */
	SCHANNEL_CR( MusicChannel ) = SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_16BIT
		| SOUND_VOL( currentMusicVolume ) | SOUND_PAN( currentMusicPanning ) ;

	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1 ;

}



void unsetMusicBasicSettings()
{

	SCHANNEL_CR( MusicChannel ) = 0 ;

	TIMER0_CR = 0 ;

	/* To avoid a fade effect takes previous (unrelated) sample rate: */
	outputSampleRate = 0 ;

}



void blankDecodedBuffer()
{

	memset( /* start */ (Byte*) decodedBuffer, /* filler */ 0,
		/* length */ DecodedBufferSize ) ;

}



/* Sets the relevant channel and timer settings. */
void enableMusicPlayback()
{

	/* To prevent nasty glitches at the playback start: */
	blankDecodedBuffer() ;

	SCHANNEL_REPEAT_POINT( MusicChannel ) = 0 ;

	/* Requests the audio hardware to read its data from decoded buffer: */
	SCHANNEL_SOURCE( MusicChannel ) = ( u32 ) decodedBuffer ;

	/*
	 * DecodedFrameLength samples per frame, 2 frames, 16 bits per sample:
	 * (length is in 32-bit words, hence the division by 4 with >>2)
	 *
	 */
	SCHANNEL_LENGTH( MusicChannel ) = ( DecodedFrameLength * 2 ) >> 2 ;

	outputSampleRate = frameInfo.samprate ;

	/* Reads sample rate from decoded frame info, set by decoding: */
	SCHANNEL_TIMER( MusicChannel ) = SOUND_FREQ( outputSampleRate ) ;

	setMusicBasicSettings() ;

	/* First timer must run at twice the sample rate, since 16-bit samples: */
	TIMER0_DATA = SOUND_FREQ( outputSampleRate )  ;

	/*
	 * Each of the previous ticks makes the second timer eat 1 out of
	 * DecodedFrameLength:
	 */
	TIMER1_DATA = 65536 - DecodedFrameLength ;
	TIMER1_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_CASCADE ;

	/* This third timer counts finally full MP3 frames: */
	TIMER2_DATA = 0 ;
	TIMER2_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_CASCADE ;

}



/* Manages the fade-in/fade-out effects by modifying the music volume. */
void manageMusicFadeEffect()
{

	if ( musicFadeInIncrement != 0 )
	{

		if ( currentMusicVolume + musicFadeInIncrement >= 127 )
		{
			currentMusicVolume = 127 ;
			musicFadeInIncrement = 0 ;
		}
		else
		{
			currentMusicVolume += musicFadeInIncrement ;
		}

		SCHANNEL_CR( MusicChannel ) = SCHANNEL_ENABLE | SOUND_REPEAT
			| SOUND_16BIT | SOUND_VOL( currentMusicVolume )
			| SOUND_PAN( currentMusicPanning ) ;

	}
	else if ( musicFadeOutDecrement != 0 )
	{

		/* currentMusicVolume - musicFadeOutDecrement would be negative: */
		if ( musicFadeOutDecrement >= currentMusicVolume )
		{
			currentMusicVolume = 0 ;
			musicFadeOutDecrement = 0 ;

			/* Stops decoding, halts the music: */
			endOfProcessingReached = true ;
		}
		else
		{
			currentMusicVolume -= musicFadeOutDecrement ;
		}


		SCHANNEL_CR( MusicChannel ) = SCHANNEL_ENABLE | SOUND_REPEAT
			| SOUND_16BIT | SOUND_VOL( currentMusicVolume )
			| SOUND_PAN( currentMusicPanning ) ;

	}

}



/* Disables the settings for music playback. */
void disableMusicPlayback()
{

	unsetMusicBasicSettings() ;

	/* Stop other timers as well: */
	TIMER1_CR = 0 ;
	TIMER2_CR = 0 ;

	TIMER0_DATA = 0 ;
	TIMER1_DATA = 0 ;
	TIMER2_DATA = 0 ;

}



/**
 * Manages the MP3 playback when requested.
 *
 */
void manageMP3Playback()
{

	/*
	 * The ARM7 main loop will be hijacked here as long as the music playback is
	 * not over.
	 *
	 */
	startMP3PlaybackRequested = false ;


	/*
	 * Starts by decoding the first two frames, so that as soon as the second is
	 * played (overflow of timer 2), the third starts being decoded.
	 *
	 */
	if ( ! decodeNextMP3Frame() )
		return ;

	/* Second one: */
	decodeNextMP3Frame() ;

	int CurrentFrameCounter ;
	int LastFrameCounter = 0 ;

	enableMusicPlayback() ;

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
			manageMusicFadeEffect() ;
			LastFrameCounter++ ;

		}

		/* Manages playback pause: */
		if ( pauseMP3PlaybackRequested )
		{

			unsetMusicBasicSettings() ;

			while ( pauseMP3PlaybackRequested )
				atomicSleep() ;

			setMusicBasicSettings() ;

		}


	} while ( ! endOfProcessingReached && ! decodingInError ) ;


	if ( ! decodingInError )
	{

		/* Wait for the playback of final buffer: */
		CurrentFrameCounter = ( TIMER2_DATA + 1 ) ;

		while ( TIMER2_DATA == CurrentFrameCounter )
			atomicSleep() ;

	}

	disableMusicPlayback() ;

	/* Let the ARM9 be notified of the music end here: */

	InterruptMask previous = setEnabledInterrupts( AllInterruptsDisabled ) ;

	writeBlocking( prepareFIFOCommand( MusicEndedNotification ) ) ;

	setEnabledInterrupts( previous ) ;

	notifyCommandToARM9() ;

}



/**
 * Sets-up the sound software, including the Helix decoder.
 *
 */
void initOSDLSound()
{

	/* Initializes the Helix MP3 decoder: */
	currentDecoder = MP3InitDecoder() ;

	if ( currentDecoder == 0 )
	{

		setError( HelixInitializationFailed ) ;
		return ;

	}


	decodedBuffer = (Byte *) malloc( DecodedBufferSize ) ;

	/* Wait a bit to let sound hardware be initialized: */
	atomicSleep() ;

}



/**
 * Sets-up OSDL and its prerequesites.
 *
 */
void initOSDL()
{

	initCeylan() ;

	initOSDLSound() ;

}
