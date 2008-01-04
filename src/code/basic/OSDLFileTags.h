#ifndef OSDL_FILE_TAGS_H_
#define OSDL_FILE_TAGS_H_


#include "OSDLException.h"  // for OSDL::Exception


#include "Ceylan.h"         // for Uint16

#include <string>



namespace OSDL
{

	/*
	 * File tags are used to identify OSDL-specific file formats.
	 *
	 * They are expected to be named *.osdl.<format type>, for example 
	 * hello.osdl.sound or christmas.osdl.music.
	 *
	 * However their nature (their type) is detected not depending on their 
	 * name (ex: renaming christmas.osdl.music to christmas.wav will not confuse
	 * OSDL, even if it is not a wave file) but based on the first two bytes of
	 * the file, which are the OSDL file tag, which tells OSDL what the actual
	 * type is. Based on that, OSDL can decode the rest of the header so that 
	 * it can handle this file appropriately.
	 *
	 *
	 */


	/// Describes a tag used to identify the type of an OSDL-specific file.
	typedef Ceylan::Uint16 FileTag ;
	
	
	
	/**
	 * Tag corresponding to OSDL punctual sounds, not longer musics.
	 *
	 * It is the lightweight counterpart of a Wave file, based on a header
	 * followed by the raw samples.
	 *
	 * The corresponding header after this tag is:
	 * - 1 Uint16: frequency, in Hertz (ex: 22 050 Hz)
	 * - 1 Uint16: format, i.e. bit depth (ex: 8 bit/16 bit), supposed signed
	 * little endian
	 * - 1 Uint16: mode, i.e. number of channels (ex: mono, stereo)
	 *
	 * @see code/audio/OSDLSound.cc, in Sound::load for the actual decoding.
	 *
	 */
	extern OSDL_DLL const FileTag SoundTag ;
	
	
	/**
	 * Tag corresponding to OSDL musics, not shorter punctual sounds.
	 *
	 * It is the lightweight counterpart of a Wave file.
	 *
	 * The corresponding header after this tag is:
	 *
	 * @see code/audio/OSDLSound.cc, in Sound::load for the actual decoding.
	 *
	 */
	extern OSDL_DLL const FileTag MusicTag ;
	
	
	/**
	 * Returns a textual description of specified file tag.
	 *
	 * @throw OSDL::Exception if the tag is not known.
	 *
	 */
	const std::string & DescribeFileTag( FileTag tag ) 
		throw( OSDL::Exception ) ;
		
		
}


#endif // OSDL_FILE_TAGS_H_
