#include "OSDLFileTags.h" 



using namespace OSDL ;


using std::string ;

extern const FileTag OSDL::SoundTag     = 1 ;
extern const FileTag OSDL::MusicTag     = 2 ;


const std::string SoundTagDescription	= "raw sound" ;
const std::string MusicTagDescription	= "MP3-based music" ;

const std::string UnknownTagDescription = "unknown file format" ;



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
			
		default:
			return UnknownTagDescription ;
			break ;
	
	}
	
}

