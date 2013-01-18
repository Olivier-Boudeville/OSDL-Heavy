/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLFileTags.h"



using namespace OSDL ;



using std::string ;

extern const FileTag OSDL::SoundTag      = 1 ;
extern const FileTag OSDL::MusicTag      = 2 ;
extern const FileTag OSDL::PaletteTag    = 3 ;
extern const FileTag OSDL::FrameTag      = 4 ;


const FileTag              FirstFreeTag  = 5 ;



const std::string SoundTagDescription    = "sound (PCM or IMA ADPCM)" ;
const std::string MusicTagDescription    = "music (MP3)" ;
const std::string PaletteTagDescription  = "color palette" ;
const std::string FrameTagDescription    = "animation frame" ;

const std::string UnknownTagDescription  = "unknown file format" ;




bool OSDL::IsAValidOSDLFileTag( FileTag tag )
{

	return ( tag < FirstFreeTag ) ;

}



const std::string & OSDL::DescribeFileTag( FileTag tag )
{

	switch( tag )
	{

		case SoundTag:
			return SoundTagDescription ;
			break ;

		case MusicTag:
			return MusicTagDescription ;
			break ;

		case PaletteTag:
			return PaletteTagDescription ;
			break ;

		case FrameTag:
			return FrameTagDescription ;
			break ;

		default:
			return UnknownTagDescription ;
			break ;

	}

}
