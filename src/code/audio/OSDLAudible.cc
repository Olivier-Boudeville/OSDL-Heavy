#include "OSDLAudible.h"


#include "OSDLAudio.h" // for AudioModule


using std::string ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Audio ;


// Corresponds to an infinite count.
extern const PlaybackCount OSDL::Audio::Loop = -1 ;

extern const Volume OSDL::Audio::MinVolume = 0 ;


#ifdef OSDL_ARCH_NINTENDO_DS

extern const Volume OSDL::Audio::MaxVolume = 127 ;

#else // OSDL_ARCH_NINTENDO_DS

// SDL_mixer convention:
extern const Volume OSDL::Audio::MaxVolume = 128 ;

#endif // OSDL_ARCH_NINTENDO_DS



AudibleException::AudibleException( const std::string & reason ) throw():
	AudioException( reason )
{

}


	
AudibleException::~AudibleException() throw()
{

}





Audible::Audible( bool convertedToOutputFormat ) throw( AudibleException ):
	_convertedToOutputFormat( convertedToOutputFormat )
{

}



Audible::~Audible() throw()
{

}




bool Audible::isConvertedToOutputFormat() const throw()
{

	return _convertedToOutputFormat ;
	
}



const string Audible::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Audible " ;
	
	if ( ! _convertedToOutputFormat )
		res += "not " ;
		
	res += "converted to output format" ;
		
	return res ;
	
}



string Audible::FindAudiblePath( const string & audibleFilename ) 
	throw( AudibleException )
{
	
	string audibleFullPath = audibleFilename ;

	// Search directly in current working directory:
	if ( ! File::ExistsAsFileOrSymbolicLink( audibleFullPath ) )
	{
		
		// On failure use the audio locator:
		try
		{
		
			audibleFullPath = AudioModule::AudioFileLocator.find( 
				audibleFullPath ) ;
				
		}
		catch( const FileLocatorException & e )
		{
				
			// Not found !
				
			string currentDir ;
				
			try
			{
				currentDir = Directory::GetCurrentWorkingDirectoryPath() ;
			}
			catch( const DirectoryException & exc )
			{
				
				throw AudibleException( 
					"Audible::FindAudiblePath: unable to find '"
					+ audibleFilename 
					+ "', exception generation triggered another failure: "
					+ exc.toString() + "." ) ;
			}
				
			throw AudibleException( "Audible::FindAudiblePath: '" 
					+ audibleFilename 
					+ "' is not a regular file or a symbolic link "
					"relative to the current directory (" + currentDir
					+ ") and cannot be found through audio locator ("
					+ AudioModule::AudioFileLocator.toString() 
					+ ") based on audio path environment variable ("
					+ AudioModule::AudioPathEnvironmentVariable + ")." ) ;
					
		}		
	}


	return audibleFullPath ;
	
}

					

int Audible::GetLoopsForPlayCount( PlaybackCount playCount )
	throw( AudibleException )
{

	if ( playCount == 0 || playCount < Loop )
		throw AudibleException( "Audible::GetLoopsForPlayCount failed: "
			"the play count is out of bounds (" 
			+ Ceylan::toString( playCount ) + ")" ) ;
 
 	if ( playCount == Loop )
		return Loop ;
	else
		return ( playCount - 1 ) ;	
				
}

