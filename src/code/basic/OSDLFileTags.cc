#include "OSDLFileTags.h" 



using namespace OSDL ;


using std::string ;

extern const FileTag OSDL::SoundTag      = 1 ;
extern const FileTag OSDL::MusicTag      = 2 ;
extern const FileTag OSDL::FrameTag      = 3 ;

const FileTag              FirstFreeTag  = 4 ;


const std::string SoundTagDescription	 = "sound (PCM or IMA ADPCM)" ;
const std::string MusicTagDescription    = "music (MP3)" ;
const std::string FrameTagDescription    = "animation frame" ;

const std::string UnknownTagDescription  = "unknown file format" ;



bool OSDL::IsAValidOSDLFileTag( FileTag tag ) throw( OSDL::Exception )
{

	return ( tag < FirstFreeTag ) ;
	
}


const std::string & OSDL::DescribeFileTag( FileTag tag ) throw( Exception )
{

	switch( tag )
	{
	
		case SoundTag:
			return SoundTagDescription ;
			break ;
			
		case MusicTag:
			return MusicTagDescription ;
			break ;
			
		case FrameTag:
			return FrameTagDescription ;
			break ;
			
		default:
			return UnknownTagDescription ;
			break ;
	
	}
	
}

