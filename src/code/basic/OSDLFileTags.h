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
	
	
	
	
	/*
	 * Audio-related tags.
	 *
	 * @note These tags describe audio file formats (ex: WAV, OggVorbis, MP3),
	 * not how they will be used, i.e. as punctual sounds or as musics (which
	 * are managed differently).
	 *
	 * Indeed an OggVorbis file, for example, can be used either as a sound
	 * (ex: for a long speech of a character) or as a music.
	 *
	 * The mapping, in the non-embedded case (PC platform) is:
	 *   - sounds can be either WAV or OggVorbis
	 *   - musics must be OggVorbis (MikMod could be added quite easily)
	 *
	 * In the embedded case (Nintendo DS platform), we have:
	 *   - sounds can be either RAW with OSDL header, or MP3
	 *   - musics can be either RAW with OSDL header, or MP3
	 * 
	 * @see also Audio::MusicType
	 *
	 */
	
	
	/**
	 * Tag corresponding to OSDL punctual sounds, not longer musics.
	 *
	 * It is the lightweight counterpart of a Wave file, based on an OSDL
	 * header followed by the raw PCM or IMA ADPCM (not Microsoft ADPCM) encoded
	 * samples.
	 *
	 * The corresponding header after this tag is:
	 * - 1 Uint16: frequency, in Hertz (ex: 22 050 Hz)
	 * - 1 Uint16: format, i.e. bit depth for PCM samples (ex: 8 bit/16 bit),
	 * supposed signed little endian, or IMA ADPCM.
	 * - 1 Uint16: mode, i.e. number of channels (ex: mono, stereo)
	 *
	 * @see code/audio/OSDLSound.cc, in Sound::load for the actual decoding.
	 *
	 * @note Only relevant for embedded platforms, as on PC the format is
	 * determined automatically.
	 *
	 */
	extern OSDL_DLL const FileTag SoundTag ;
		
	
	/**
	 * Tag corresponding to MP3-based OSDL musics, not shorter punctual sounds.
	 *
	 * It is a MP3 file prepended by a header.
	 *
	 * The corresponding header after this tag is defined in:
	 * trunk/tools/media/audio/mp3ToOSDLMusic.cc
	 *
	 * @see code/audio/OSDLMusic.cc, in Music::load for the actual decoding.
	 *
	 * @note Only relevant for embedded platforms, as on PC the format is
	 * determined automatically.
	 *
	 */
	extern OSDL_DLL const FileTag MusicTag ;
	


	// Video-related tags.


	/**
	 * Tag corresponding to an OSDL palette, useful for color indexed modes.
	 *
	 * The corresponding header after this tag is defined in:
	 * trunk/src/code/video/OSDLPalette.h.
	 *
	 * @see trunk/src/code/video/OSDLPalette.cc for the corresponding
	 * encoding/decoding.
	 *
	 */
	extern OSDL_DLL const FileTag PaletteTag ;

	
	/**
	 * Tag corresponding to an OSDL frame, part of an animation.
	 *
	 * It is a list of color index to a palette, prepended by a header.
	 *
	 * The corresponding header after this tag is defined in:
	 * trunk/tools/media/video/animation-management/pngToOSDLFrame.cc
	 *
	 * @see trunk/src/code/video/twoDimensional/OSDLAnimation.cc, in 
	 * Animation::loadFrame for the actual decoding.
	 *
	 */
	extern OSDL_DLL const FileTag FrameTag ;
	
	
	
	/**
	 * Tells whether specified tag is a valid OSDL one.
	 *
	 */
	OSDL_DLL bool IsAValidOSDLFileTag( FileTag tag ) throw( OSDL::Exception ) ;
	
		
	/**
	 * Returns a textual description of specified file tag.
	 *
	 * @throw OSDL::Exception if the tag is not known.
	 *
	 */
	OSDL_DLL const std::string & DescribeFileTag( FileTag tag ) 
		throw( OSDL::Exception ) ;
		
		
}


#endif // OSDL_FILE_TAGS_H_

