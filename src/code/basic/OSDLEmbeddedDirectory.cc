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


#include "OSDLEmbeddedDirectory.h"


#include "OSDLEmbeddedFileSystemManager.h" // for EmbeddedFileSystemManager


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"                    // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h"       // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                        // for PHYSFS_getDirSeparator and al
#endif // OSDL_USES_PHYSICSFS


#include "Ceylan.h"                        // for all Ceylan services



/*
 * Implementation notes.
 *
 * @note All internal paths kept in directory references (this object)
 * should be absolute paths.
 *
 * @note In a non-static method, no static method should be used, as the former
 * is expected to use the embedded filesystem manager, whereas the latter shall
 * use the default filesystem manager, which may or may not be the embedded
 * one.
 * 
 */
 
 
using std::string ;
using std::list ;

using namespace Ceylan::System ;

using namespace OSDL ;




// EmbeddedDirectory implementation.


EmbeddedDirectory::~EmbeddedDirectory() throw()
{

	// Nothing special here for embedded directories.

}




// EmbeddedDirectory implementation section.


// Instance methods.
				

			
				
// Directory content subsection.



bool EmbeddedDirectory::hasDirectory( const string & subdirectoryName ) const
{

#if OSDL_USES_PHYSICSFS

	FileSystemManager * fsManager ;
	
	try
	{
	
		fsManager = & getCorrespondingFileSystemManager() ;
		
	}
	catch( const DirectoryDelegatingException & e )
	{
	
		throw DirectoryLookupFailed( 
			"EmbeddedDirectory::hasDirectory failed: " + e.toString() ) ;
			 
	}	
	
	string tmpPath = fsManager->joinPath( _path, subdirectoryName ) ;
	
	return ( PHYSFS_isDirectory( tmpPath.c_str() ) != 0 ) ;

#else // OSDL_USES_PHYSICSFS

	throw DirectoryLookupFailed( "EmbeddedDirectory::hasDirectory failed: "
    	"no PhysicsFS support available." ) ;  
          	
#endif // OSDL_USES_PHYSICSFS

}



bool EmbeddedDirectory::hasFile( const string & fileName ) const
{

#if OSDL_USES_PHYSICSFS

	FileSystemManager * fsManager ;
	
	try
	{
	
		fsManager = & getCorrespondingFileSystemManager() ;
		
	}
	catch( const DirectoryDelegatingException & e )
	{
	
		throw DirectoryLookupFailed( 
			"EmbeddedDirectory::hasFile failed: " + e.toString() ) ;
			 
	}	

	// It is a file or a symlink if it exists and is not a directory:
	
	string tmpPath = fsManager->joinPath( _path, fileName ) ;
	
    if ( PHYSFS_exists( tmpPath.c_str() ) == 0 )
    	return false ;
    
    // Here it exists, thus it must not be a directory to be a file:    
	return ( PHYSFS_isDirectory( tmpPath.c_str() ) == 0 ) ;

#else // OSDL_USES_PHYSICSFS

	throw DirectoryLookupFailed( "EmbeddedDirectory::hasFile failed: "
    	"no PhysicsFS support available." ) ;  
          	
#endif // OSDL_USES_PHYSICSFS

}



bool EmbeddedDirectory::hasEntry( const string & entryName ) const
{

#if OSDL_USES_PHYSICSFS

	FileSystemManager * fsManager ;
	
	try
	{
	
		fsManager = & getCorrespondingFileSystemManager() ;
		
	}
	catch( const DirectoryDelegatingException & e )
	{
	
		throw DirectoryLookupFailed( 
			"EmbeddedDirectory::hasEntry failed: " + e.toString() ) ;
			 
	}	
	
	string tmpPath = fsManager->joinPath( _path, entryName ) ;
	
	return ( PHYSFS_exists( tmpPath.c_str() ) != 0 ) ;

#else // OSDL_USES_PHYSICSFS

	throw DirectoryLookupFailed( "EmbeddedDirectory::hasEntry failed: "
    	"no PhysicsFS support available." ) ;  
          	
#endif // OSDL_USES_PHYSICSFS

}


			
					
void EmbeddedDirectory::getSubdirectories( list<string> & subDirectories ) const
{

	/*
     * Could be implemented if PHYSFS_enumerateFiles returned directory
     * (to check)
     *
     */
    throw DirectoryLookupFailed( " EmbeddedDirectory::getSubdirectories: "
    	"no PhysicsFS support available." ) ;  
    	
}

					
										
void EmbeddedDirectory::getFiles( list<string> & files ) const
{

#if OSDL_USES_PHYSICSFS

	char **rc = PHYSFS_enumerateFiles( _path.c_str() ) ;
    
	char **i ;
	
    for ( i = rc; *i != 0; i++ )
    	files.push_back( *i ) ;

	PHYSFS_freeList( rc ) ;		
    
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::DirectoryLookupFailed( 
    	"EmbeddedDirectory::getFiles failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

	
					
void EmbeddedDirectory::getEntries( list<string> & entries ) const
{

#if OSDL_USES_PHYSICSFS

	char **rc = PHYSFS_enumerateFiles( _path.c_str() ) ;
    
	char **i ;
	
    for ( i = rc; *i != 0; i++ )
    	entries.push_back( *i ) ;

	PHYSFS_freeList( rc ) ;		
    
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::DirectoryLookupFailed( 
    	"EmbeddedDirectory::getEntries failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedDirectory::getSortedEntries( list<string> & subDirectories,
	list<string> & files, list<string> & otherEntries ) const
{

	/*
     * Could be implemented if PHYSFS_enumerateFiles returned directory
     * (to check)
     *
     */
    throw DirectoryLookupFailed( "EmbeddedDirectory::getSortedEntries: "
    	"no PhysicsFS support available." ) ;  

}
	
	
	
					
// Other instance methods.

										
					
const std::string EmbeddedDirectory::toString( Ceylan::VerbosityLevels level )
	const
{

	return "Embedded directory referring to user-specified path '" 
		+ Ceylan::encodeToROT13( _path ) 
		+ "', corresponding to an actual path in archive '" + _path + "'" ;
 
}
					
		
		
					
					
// Factory section.										


EmbeddedDirectory & EmbeddedDirectory::Create( const string & newDirectoryName )
{

	return * new EmbeddedDirectory( newDirectoryName, /* create */ true ) ;

}


					
EmbeddedDirectory & EmbeddedDirectory::Open( const string & directoryName ) 
{

	return * new EmbeddedDirectory( directoryName, /* create */ false ) ;

}
			

			
EmbeddedDirectory::EmbeddedDirectory( const string & directoryName, 
		bool create ) :
	Directory( Ceylan::encodeToROT13(directoryName) )
{

#if OSDL_USES_PHYSICSFS

	EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;
    
	if ( create )
    {
    
    	if ( PHYSFS_mkdir( directoryName.c_str() ) == 0 )
        	throw DirectoryCreationFailed( 
            	"EmbeddedDirectory constructor failed: "
                + EmbeddedFileSystemManager::GetBackendLastError() ) ;

    }
     	
#else // OSDL_USES_PHYSICSFS

	throw DirectoryException( "EmbeddedDirectory constructor failed: "
    	"no PhysicsFS support available." ) ;  

#endif // OSDL_USES_PHYSICSFS

}

		
		
        	
					
// Protected section.



FileSystemManager & EmbeddedDirectory::getCorrespondingFileSystemManager() const
{

	try
	{
	
		return EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
	
	} 
	catch ( const EmbeddedFileSystemManagerException & e )
	{
	
		throw DirectoryDelegatingException(
			"EmbeddedDirectory::getCorrespondingFileSystemManager failed: "
			+ e.toString() ) ;
		
	}	
	
}
																		
