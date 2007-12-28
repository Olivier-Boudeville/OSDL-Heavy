#ifndef OSDL_ARM7_CODES_H_
#define OSDL_ARM7_CODES_H_


/*
 * For Ceylan base codes.
 *
 */
#include "CeylanARM7Codes.h"



/**
 * This include file centralizes all the ARM7 status words and error codes
 * added for OSDL.
 *
 * It is to be included by ARM7 C code (for setting) and ARM9 C++ code (for
 * interpretation).
 *
 * typedef for ARM7StatusWord and ARM7ErrorCode are expected to be already
 * defined (they cannot be defined here as their definition is specific to
 * each ARM: Ceylan::Uint16/uint16).
 *
 * In the ARM9 this is done automatically thanks to:
 *
 */ 



/**
 * ARM7 status word values.
 *
 */




/**
 * ARM7 error code values.
 *
 * Error codes in the [1024..2048] range are reserved for OSDL use, knowing 
 * that the [0..1023] range is reserved for Ceylan use.
 *
 */

/* ARM7 detected an unexpected OSDL-specific command */
const ARM7ErrorCode UnexpectedOSDLCommand = 1024 ;


/**
 * Helix-OSDL error codes are in the [1056..1088] range.
 *
 */
 
 
/*
 * The initialization of the Helix decoder failed (most probably: not enough
 * memory to malloc internal buffers
 */
const ARM7ErrorCode HelixInitializationFailed = 1056 ;


/* The Helix decoder could not find the next sync word in the raw mp3 stream */
const ARM7ErrorCode HelixSyncWordNotFound = 1057 ;


/* 
 * The Helix decoder is out of data, found a truncated or last frame in 
 * the raw mp3 stream */
const ARM7ErrorCode HelixFoundTruncatedFrame = 1058 ;


/* 
 * The Helix decoder does not have enough data in bit reservoir from previous
 * frames. Perhaps starting in the middle of a mp3 file ?
 *
 */
const ARM7ErrorCode HelixLacksDataInBitReservoir = 1059 ;


/* 
 * The Helix decoder lacks a free bitrate slot.
 *
 */
const ARM7ErrorCode HelixLacksFreeBitrateSlot = 1060 ;


/* 
 * The Helix decoding failed.
 *
 */
const ARM7ErrorCode HelixDecodingError = 1061 ;


/* 
 * The length of a decoded frame is not the expected one.
 *
 */
const ARM7ErrorCode HelixUnexpectedDecodedLength = 1062 ;


#endif // OSDL_ARM7_CODES_H_

