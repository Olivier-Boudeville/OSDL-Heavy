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
 * Request the ARM7 to fade-in current music, from volume zero to full volume.
 *
 * The last 16 bits of the command element contain the requested fade-in
 * duration, in milliseconds.
 *
 * No answer expected.
 *
 * @see FadeOutMusicRequest
 *
 */
const ARM9CommandID FadeInMusicRequest = 35 ;


/**
 * Request the ARM7 to fade-out current music, from current volume to zero
 * volume.
 *
 * The last 16 bits of the command element contain the requested fade-out
 * duration, in milliseconds.
 *
 * No answer expected.
 *
 * @see FadeInMusicRequest
 *
 */
const ARM9CommandID FadeOutMusicRequest = 36 ;


/**
 * Request the ARM7 to stop the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see PlayMusicRequest
 *
 */
const ARM9CommandID StopMusicRequest = 37 ;


/**
 * Request the ARM7 to pause the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see UnpauseMusicRequest
 *
 */
const ARM9CommandID PauseMusicRequest = 38 ;


/**
 * Request the ARM7 to unpause the playback of current music.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 * @see PauseMusicRequest
 *
 */
const ARM9CommandID UnpauseMusicRequest = 39 ;


/**
 * Notification to the ARM7: the end of encoded stream has been reached while
 * refilling buffers.
 *
 * No parameters set in the command element, no other element sent, no answer
 * expected.
 *
 */
const ARM9CommandID EndOfEncodedStreamReached = 40 ;


/**
 * Request the ARM7 to set the volume of current music to specified value.
 *
 * The new volume is set in the last byte of command element.
 *
 */
const ARM9CommandID SetMusicVolumeRequest = 41 ;



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

