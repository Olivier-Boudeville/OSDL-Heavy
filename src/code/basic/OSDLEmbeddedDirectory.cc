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
	throw( DirectoryLookupFailed )
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
	throw( DirectoryLookupFailed )
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
	throw( DirectoryLookupFailed )
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


			
					
void EmbeddedDirectory::getSubdirectories( list<string> & subDirectories )
	const throw( DirectoryLookupFailed )
{

	/*
     * Could be implemented if PHYSFS_enumerateFiles returned directory
     * (to check)
     *
     */
    throw DirectoryLookupFailed( " EmbeddedDirectory::getSubdirectories: "
    	"no PhysicsFS support available." ) ;  
    	

}

					
										
void EmbeddedDirectory::getFiles( list<string> & files )
	const throw( DirectoryLookupFailed )
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

	
					
void EmbeddedDirectory::getEntries( list<string> & entries )
	const throw( Ceylan::System::DirectoryLookupFailed )
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
		list<string> & files, list<string> & otherEntries )
	const throw( DirectoryLookupFailed )
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
	const throw()
{

	return "Embedded directory referring to path '" + _path + "'" ;
 
}
					
		
					
					
// Factory section.										


EmbeddedDirectory & EmbeddedDirectory::Create( const string & newDirectoryName )
	throw( DirectoryException )
{

	return * new EmbeddedDirectory( newDirectoryName, /* create */ true ) ;

}


					
EmbeddedDirectory & EmbeddedDirectory::Open( const string & directoryName ) 
	throw( DirectoryException )
{

	return * new EmbeddedDirectory( directoryName, /* create */ false ) ;

}
			

			
EmbeddedDirectory::EmbeddedDirectory( const string & directoryName,
		bool create ) throw( DirectoryException ):
	Directory( directoryName )
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


FileSystemManager & EmbeddedDirectory::getCorrespondingFileSystemManager()
	const throw( DirectoryDelegatingException )
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
																		