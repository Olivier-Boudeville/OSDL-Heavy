#ifndef OSDL_IPC_COMMANDS_H_
#define OSDL_IPC_COMMANDS_H_


/**
 * This include file centralizes the OSDL-specific command
 * identifiers for the FIFO-based IPC, notably for the Nintendo DS.
 *
 * It is to be included by ARM7 C code and ARM9 C++ code.
 *
 * typedef for FIFOCommandID is expected to be already defined (it cannot be
 * defined here as its definition is specific to each ARM: Ceylan::Uint8/uint8).
 *
 * enum not used as are int, thus 32-bit, whereas we want 8 bit.
 *
 */ 


/**
 * OSDL-specific command identifier values are in the [32..96] reserved range.
 *
 * System-specific command identifier values range from 0 to 127.
 *
 * @note Ceylan subdivided this range into [0..32], reserved for its own use,
 * and [32..127], reserved for Ceylan-integrating libraries, notably OSDL,
 * which reserved [32..96]. 
 *
 * Hence other libraries can use [97..127], and each application can use
 * [128..255].
 *
 */


#include "CeylanIPCCommands.h"  // for ARM9CommandID, ARM7CommandID
 
 

// Section dedicated to commands from the ARM9 to the ARM7.

 
/**
 * Request the ARM7 to play specified sound.
 *
 * No parameters set in the command element, next element is a pointer to
 * the cache-aligned sound buffer, no answer expected.
 *
 */
const ARM9CommandID PlaySoundRequest = 33 ;


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
 * @see StopMusicRequest
 *
 */
const ARM9CommandID PlayMusicRequest = 34 ;


/**
 * Request the ARM7 to stop the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see PlayMusicRequest
 *
 */
const ARM9CommandID StopMusicRequest = 35 ;


/**
 * Request the ARM7 to pause the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see UnpauseMusicRequest
 *
 */
const ARM9CommandID PauseMusicRequest = 36 ;


/**
 * Request the ARM7 to unpause the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see PauseMusicRequest
 *
 */
const ARM9CommandID UnpauseMusicRequest = 37 ;


/**
 * Notification to the ARM7: the end of encoded stream has been reached while
 * refilling buffers.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 */
const ARM9CommandID EndOfEncodedStreamReached = 38 ;


/**
 * Notification to the ARM7: first music encoded buffer has been filled.
 *
 * The two last bytes of the command element correspond to the size available
 * for the ARM7 to read in this buffer, no other element sent, no direct answer
 * expected.
 *
 * @see FirstBufferFillRequest
 *
 */
//const ARM9CommandID FirstBufferFilled = ;


/**
 * Notification to the ARM7: second music encoded buffer has been filled.
 *
 * The two last bytes of the command element correspond to the size available
 * for the ARM7 to read in this buffer, no other element sent, no direct answer
 * expected.
 *
 * @see SecondBufferFillRequest
 *
 */
//const ARM9CommandID SecondBufferFilled = ;




////////////////////////////////////////////////////////////////////////////////


// Section dedicated to commands from the ARM7 to the ARM9.



/**
 * Notification of the ARM9 that a sound could not be played due to the lack 
 * of free audio channel (all are busy).
 *
 * No parameters set in the command element, no answer expected.
 *
 */
const ARM7CommandID NoAvailableChannelNotification = 33 ;


/**
 * Notification of the ARM9 that the first music encoded buffer is not used
 * currently and that it should be filled as soon as possible.
 *
 * No parameters set in the command element, no direct answer expected.
 *
 * @see FirstBufferFilled
 *
 */
const ARM7CommandID FirstBufferFillRequest = 34 ;


/**
 * Notification of the ARM9 that the second music encoded buffer is not used
 * currently and that it should be filled as soon as possible.
 *
 * No parameters set in the command element, no direct answer expected.
 *
 * @see SecondBufferFilled
 *
 */
const ARM7CommandID SecondBufferFillRequest = 35 ;


/**
 * Notification of the ARM9 that the current music playback is over.
 *
 * No parameters set in the command element, no direct answer expected.
 *
 */
const ARM7CommandID MusicEndedNotification = 36 ;


/**
 * Notification of the ARM9 of some informations regarding a MP3 frame.
 *
 * 
 */
const ARM7CommandID MusicFrameInformation = 37 ;



#endif // OSDL_IPC_COMMANDS_H_

