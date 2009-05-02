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


#include "OSDLAudible.h"


#include "OSDLAudio.h" // for AudioModule


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



using std::string ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Audio ;


// Corresponds to an infinite count.
extern const PlaybackCount OSDL::Audio::Loop = -1 ;

extern const Volume OSDL::Audio::MinVolume = 0 ;


#if OSDL_ARCH_NINTENDO_DS

extern const Volume OSDL::Audio::MaxVolume = 127 ;

#else // OSDL_ARCH_NINTENDO_DS

// SDL_mixer convention:
extern const Volume OSDL::Audio::MaxVolume = 128 ;

#endif // OSDL_ARCH_NINTENDO_DS



AudibleException::AudibleException( const std::string & reason ) :
	AudioException( reason )
{

}


	
AudibleException::~AudibleException() throw()
{

}






Audible::Audible( bool convertedToOutputFormat ) :
	_convertedToOutputFormat( convertedToOutputFormat )
{

}



Audible::~Audible() throw()
{

}



bool Audible::isConvertedToOutputFormat() const
{

	return _convertedToOutputFormat ;
	
}



const string Audible::toString( Ceylan::VerbosityLevels level ) const
{
	
	string res = "Audible " ;
	
	if ( ! _convertedToOutputFormat )
		res += "not " ;
		
	res += "converted to output format" ;
		
	return res ;
	
}



string Audible::FindAudiblePath( const string & audibleFilename ) 
{
	
	string audibleFullPath = audibleFilename ;

	// Searches directly in current working directory:
	if ( ! File::ExistsAsFileOrSymbolicLink( audibleFullPath ) )
	{
		
		// On failure, uses the audio locator:
		try
		{
		
			audibleFullPath = AudioModule::AudioFileLocator.find( 
				audibleFullPath ) ;
				
		}
		catch( const FileLocatorException & e )
		{
				
			// Not found!
				
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
					+ "' is not a regular file nor a symbolic link "
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

