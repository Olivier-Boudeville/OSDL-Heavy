#include "OSDLEmbeddedFileSystemManager.h"


#include "OSDLEmbeddedFile.h"        // for EmbeddedFile
#include "OSDLEmbeddedDirectory.h"   // for EmbeddedDirectory


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                  // for PHYSFS_getDirSeparator and al
#endif // OSDL_USES_PHYSICSFS


#include "Ceylan.h"                  // for all Ceylan services



using std::string ;
using std::list ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL ;



/*
 * Implementation notes:
 *
 * Based on PhysicsFS. Thanks to Ryan Gordon!
 * @see http://icculus.org/physfs/
 * 
 * Not integrated yet:
 *  - PHYSFS_addToSearchPath: deprecated in favor of PHYSFS_mount 
 *  - PHYSFS_eof: not really needed for the moment
 *  - PHYSFS_tell: not really needed for the moment
 *  - PHYSFS_seek: not really needed for the moment
 *  - PHYSFS_setBuffer: not really needed for the moment
 *  - PHYSFS_flush: not really needed for the moment
 *  - PHYSFS_swap*: not really needed for the moment
 *  - PHYSFS_read*: not really needed for the moment
 *  - PHYSFS_write*: not really needed for the moment
 *  - PHYSFS_isInit: not needed
 *  - PHYSFS_symbolicLinksPermitted: not needed
 *  - PHYSFS_setAllocator: not needed
 *  - various callbacks: not needed
 *  - string conversion: not needed
 *
 */


// 0b01010101 = 0x55
const Ceylan::Byte EmbeddedFileSystemManager::XORByte = 0x55 ;


// These constants apply to the virtual filesystem:
const string EmbeddedFileSystemManager::RootDirectoryPrefix   = ""   ;
const Ceylan::Latin1Char EmbeddedFileSystemManager::Separator = '/'  ;




EmbeddedFileSystemManager *
	EmbeddedFileSystemManager::_EmbeddedFileSystemManager = 0 ;



string EmbeddedFileSystemManager::ArchivePathEnvironmentVariable = 
	"ARCHIVE_PATH" ;

Ceylan::System::FileLocator EmbeddedFileSystemManager::ArchiveFileLocator(
	ArchivePathEnvironmentVariable ) ;




// Implementation of EmbeddedFileSystemManager specific methods.



void EmbeddedFileSystemManager::chooseBasicSettings(
		const string & organizationName,
		const string & applicationName,
		const string & archiveExtension,
		bool archiveFirst,
		bool includeInsertedMedia ) 
	throw( EmbeddedFileSystemManagerException )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

    int res = PHYSFS_setSaneConfig(
    	organizationName.c_str(),
        applicationName.c_str(),
        archiveExtension.c_str(),
        (includeInsertedMedia ? 1 : 0 ),
        (archiveFirst ? 1 : 0 ) ) ;

    if ( res == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::chooseBasicSettings failed: "
            + GetBackendLastError() ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::chooseBasicSettings failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

} 



bool EmbeddedFileSystemManager::hasWriteDirectory() const throw()
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

    const char * res = PHYSFS_getWriteDir() ;

	return (res != 0) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::hasWriteDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    
			
 
            
std::string EmbeddedFileSystemManager::getWriteDirectory() const 
	throw( EmbeddedFileSystemManagerException )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    const char * res = PHYSFS_getWriteDir() ;
    
    if ( res == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::getWriteDirectory failed: "
            "no write directory set." ) ;
    
    return string( res ) ;
            
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::getWriteDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    

			

void EmbeddedFileSystemManager::setWriteDirectory( 
		const std::string & newWriteDirectory ) 
	throw( EmbeddedFileSystemManagerException )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    if ( PHYSFS_setWriteDir( newWriteDirectory.c_str() ) == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::setWriteDirectory failed: "
            + GetBackendLastError() ) ;
    
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::setWriteDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    
     
         
                
void EmbeddedFileSystemManager::mount( 
		const string & newActualFilesystemElement,
		const string & mountPointInVirtualTree, 
        bool append ) 
	throw( EmbeddedFileSystemManagerException )              
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    try
    {
    
	    StandardFileSystemManager & standardManager =
        	StandardFileSystemManager::GetStandardFileSystemManager() ;
            
		if ( ! standardManager.existsAsEntry( newActualFilesystemElement ) )
    		throw EmbeddedFileSystemManagerException( 
    			"EmbeddedFileSystemManager::mount failed "
                "for non-existing filesystem element '"
	            + newActualFilesystemElement + "'." ) ;
    }
    catch( const Ceylan::Exception & e )
    {
    
    	throw EmbeddedFileSystemManagerException( 
    		"EmbeddedFileSystemManager::mount failed for filesystem element '"
            + newActualFilesystemElement + "': "
        	+ e.toString() ) ;
    }    
    
    
    if ( PHYSFS_mount( 
    		newActualFilesystemElement.c_str(),
			mountPointInVirtualTree.c_str(),
        	append ) == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::mount failed: "
            + GetBackendLastError() ) ;
    
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::mount failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    
                
                

void EmbeddedFileSystemManager::umount( 
		const std::string & actualFilesystemElement ) 
	throw( EmbeddedFileSystemManagerException )  
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    if ( PHYSFS_removeFromSearchPath( actualFilesystemElement.c_str() ) == 0 )
    	throw EmbeddedFileSystemManagerException( 
    		"EmbeddedFileSystemManager::umount failed: "
            + GetBackendLastError() ) ;
            
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::umount failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    
    
     
                
std::string EmbeddedFileSystemManager::getMountPointFor( 
		const string & actualFilesystemElement ) 
	const throw( EmbeddedFileSystemManagerException )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    const char * res = PHYSFS_getMountPoint( actualFilesystemElement.c_str() ) ;
    
    if ( res == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::getMountPointFor failed: "
            + GetBackendLastError() ) ;
    
    return string( res ) ;
    
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::getMountPointFor failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}    


                
std::list<std::string> EmbeddedFileSystemManager::getSearchPath() 
	const throw( EmbeddedFileSystemManagerException )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
    char ** paths = PHYSFS_getSearchPath() ;
    
    if ( paths == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::getSearchPath failed: "
            + GetBackendLastError() ) ;
	
 	list<string> res ;
    
	
    for ( ; *paths != 0; paths++ )
    	res.push_back( string( *paths ) ) ;
    
	/*
	 * Cannot use it, otherwise a core dump (invalide glib pointer)
	 * is triggered:
	 
	PHYSFS_freeList( paths ) ;
	
	 * (sorry for the small memory leak!)
	 *
	 */	     
	 
    return res ;
    
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::getSearchPath failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS


}    
                                

                
// Implementation of the FileSystemManager mother class.


bool EmbeddedFileSystemManager::existsAsEntry( const string & entryPath ) 
	const throw( EntryLookupFailed )
{

	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */

#if OSDL_USES_PHYSICSFS
	
	// Apparently will find files *or* directories, thus only entries:
    return ( PHYSFS_exists( entryPath.c_str() ) != 0 ) ;
         
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::existsAsEntry failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedFileSystemManager::createSymbolicLink( 
		const string & linkTarget, const string & linkName )
	throw( SymlinkFailed )
{

	throw SymlinkFailed( "EmbeddedFileSystemManager::createSymbolicLink: "
		"symbolic link feature not available on PhysicsFS." ) ;

}



time_t EmbeddedFileSystemManager::getEntryChangeTime( 
	const string & entryPath ) throw( GetChangeTimeFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 time = PHYSFS_getLastModTime( entryPath.c_str() ) ;
    
    if ( time == -1 )
		throw GetChangeTimeFailed( 
        	"EmbeddedFileSystemManager::getEntryChangeTime failed: "
            + GetBackendLastError() ) ;
    
    return static_cast<time_t>( time ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw GetChangeTimeFailed(
    	"EmbeddedFileSystemManager::getEntryChangeTime failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}
					



// Accessors to FilesystemManager constants.


const string & EmbeddedFileSystemManager::getRootDirectoryPrefix() const throw()
{

	return RootDirectoryPrefix ;
	
}

	    	   					
Ceylan::Latin1Char EmbeddedFileSystemManager::getSeparator() const throw()
{

	return Separator ;
	
}





// File-related section.



File & EmbeddedFileSystemManager::createFile( const string & filename, 
		OpeningFlag createFlag,	PermissionFlag permissionFlag ) 
	throw( FileException )
{

	return EmbeddedFile::Create( filename, createFlag, permissionFlag ) ;
	
}



File & EmbeddedFileSystemManager::openFile( const string & filename, 
	OpeningFlag openFlag ) throw( FileException )
{

	return EmbeddedFile::Open( filename, openFlag ) ;

}



string EmbeddedFileSystemManager::getActualLocationFor( 
		const string & filename ) const 
	throw( Ceylan::System::FileLookupFailed, 
    	EmbeddedFileSystemManagerException )
{
    
#if OSDL_USES_PHYSICSFS

    const char * res = PHYSFS_getRealDir( filename.c_str() ) ;

    if ( res == 0 )
    	throw Ceylan::System::FileLookupFailed( 
        	"EmbeddedFileSystemManager::getActualLocationFor failed: "
            + GetBackendLastError() ) ;
    
    string actualRes( res ) ;
    
    if ( actualRes.size() == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager::getActualLocationFor failed: "
            "file '" + actualRes + "' not found." ) ;
    
    return actualRes ;        
    			
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::FileLookupFailed( 
    	"EmbeddedFileSystemManager::getActualLocationFor failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



bool EmbeddedFileSystemManager::existsAsFileOrSymbolicLink( 
	const string & filename ) const throw( FileLookupFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
	 * Here we deem that if an entry exists and is not a directory, then 
	 * it is a file or a symlink.
	 *
     */
     
#if OSDL_USES_PHYSICSFS

	return ( ( ::PHYSFS_exists( filename.c_str() ) != 0 )
		&& ( ::PHYSFS_isDirectory( filename.c_str() ) == 0 ) ) ;
        		
#else // OSDL_USES_PHYSICSFS

	throw FileLookupFailed( 
    	"EmbeddedFileSystemManager::existsAsSymbolicLink failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



bool EmbeddedFileSystemManager::existsAsSymbolicLink( 
		const std::string & linkName ) const 
	throw( Ceylan::System::FileLookupFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

    return ( PHYSFS_isSymbolicLink( linkName.c_str() ) != 0 ) ;
        		
#else // OSDL_USES_PHYSICSFS

	throw FileLookupFailed( 
    	"EmbeddedFileSystemManager::existsAsSymbolicLink failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

		
}
                
                

void EmbeddedFileSystemManager::removeFile( const string & filename ) 
	throw( FileRemoveFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

    if ( PHYSFS_delete( filename.c_str() ) == 0 )
    	throw FileRemoveFailed( 
        	"EmbeddedFileSystemManager::removeFile failed: "
            + GetBackendLastError() ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw FileRemoveFailed( "EmbeddedFileSystemManager::removeFile failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS
		
}



void EmbeddedFileSystemManager::moveFile( const string & sourceFilename,
	const string & targetFilename ) throw( FileMoveFailed )
{

	throw FileMoveFailed( "EmbeddedFileSystemManager::moveFile failed: "
    	"not supported." ) ; 
				
}



void EmbeddedFileSystemManager::copyFile( const string & sourceFilename, 
	const string & targetFilename ) throw( FileCopyFailed )
{
	
	try 
	{

		Size fileSize = getSize( sourceFilename ) ;
	
		EmbeddedFile & sourceFile = EmbeddedFile::Open( sourceFilename ) ;
		
		// Not knowing the permissions to set here: 
		EmbeddedFile & targetFile = EmbeddedFile::Create(
			targetFilename, File::CreateToWriteBinary, File::OwnerReadWrite ) ;
		
		Size written = 0 ;
	
		Size bufferSize = ( fileSize > File::BigBufferSize ? 
			File::BigBufferSize: fileSize ) ;

		char * buf = new char[ bufferSize ] ;

		SignedSize readCount ;

		while ( written < fileSize )
		{
		
			Size toRead = fileSize - written ;

			if ( toRead > bufferSize )
				toRead = bufferSize ;

			try
			{	
				
				readCount = static_cast<SignedSize>( 
					sourceFile.read( buf, toRead ) ) ;
					
			}
			catch( const InputStream::ReadFailedException & e )
			{
			
				delete [] buf ;
				
				throw FileCopyFailed( "EmbeddedFileSystemManager::copyFile "
					"failed when copying '"	+ sourceFilename + "' to '" 
					+ targetFilename + "': " + e.toString() ) ;
					
			}


			try
			{	
				
				targetFile.write( buf, readCount ) ;
				
			}	
			catch( const OutputStream::WriteFailedException & e )
			{
			
				delete [] buf ;
				
				throw FileCopyFailed( "EmbeddedFileSystemManager::copyFile "
					"failed when copying '" + sourceFilename + "' to '" 
					+ targetFilename + "': " + e.toString() ) ;
					
			}

			written += readCount ;
		}

		delete [] buf ;
	
		targetFile.close() ;
		sourceFile.close() ;
		
	} 
	catch ( const SystemException & e )
	{
	
		throw FileCopyFailed( "EmbeddedFileSystemManager::copyFile "
			"failed when copying '"	+ sourceFilename + "' to '" 
			+ targetFilename + "': " + e.toString() ) ;
					
	}
	
 	
}



Size EmbeddedFileSystemManager::getSize( const string & filename ) 
	throw( FileSizeRequestFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

	/*
     * Cannot use directly; 
     
	EmbeddedFile tmpFile( filename, Ceylan::System::File::Read ) ;
        
     * as the constructor is protected.
     *
     */
    
    File & tmpFile = File::Create( filename, Ceylan::System::File::Read ) ;
    
    Size fileSize = tmpFile.size() ;
    
    delete & tmpFile ;
    
    return fileSize ;
    

#else // OSDL_USES_PHYSICSFS

	throw FileSizeRequestFailed(
    	"EmbeddedFileSystemManager::getSize failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

	
	
time_t EmbeddedFileSystemManager::getLastChangeTimeFile( 
	const string & filename ) throw( FileLastChangeTimeRequestFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 time = PHYSFS_getLastModTime( filename.c_str() ) ;
    
    if ( time == -1 )
		throw FileLastChangeTimeRequestFailed( 
        	"EmbeddedFileSystemManager::getLastChangeTimeFile failed: "
            + GetBackendLastError() ) ;
    
    return static_cast<time_t>( time ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw FileLastChangeTimeRequestFailed(
    	"EmbeddedFileSystemManager::getLastChangeTimeFile failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}	



void EmbeddedFileSystemManager::touch( const string & filename ) 
	throw( FileTouchFailed )
{

	throw FileTouchFailed( "EmbeddedFileSystemManager::touch "
		"operation not available on embedded filesystems." ) ;

}



void EmbeddedFileSystemManager::allowSymbolicFiles( bool newStatus ) 
	throw( EmbeddedFileSystemManagerException )
{                

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

    PHYSFS_permitSymbolicLinks( (newStatus ? 1:0) ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw FileRemoveFailed( 
    	"EmbeddedFileSystemManager::allowSymbolicFiles failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

									
	
// Directory-related section.
			

// Factory-related subsection.

										
Directory & EmbeddedFileSystemManager::createDirectory( 
	const string & newDirectoryName ) throw( DirectoryException )
{

	return EmbeddedDirectory::Create( newDirectoryName ) ;

}

	
					
Directory & EmbeddedFileSystemManager::openDirectory( 
	const string & directoryName ) throw( DirectoryException )
{

	return EmbeddedDirectory::Open( directoryName ) ;

}

	
						
bool EmbeddedFileSystemManager::existsAsDirectory( 
	const string & directoryPath ) const throw( DirectoryLookupFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
	 */

#if OSDL_USES_PHYSICSFS

	return ( PHYSFS_isDirectory( directoryPath.c_str() ) != 0 ) ;

#else // OSDL_USES_PHYSICSFS

	throw DirectoryLookupFailed( 
    	"EmbeddedFileSystemManager::existsAsDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedFileSystemManager::removeDirectory( const string & directoryPath, 
	bool recursive ) throw( DirectoryRemoveFailed )
{	

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
	 */

#if OSDL_USES_PHYSICSFS

	if ( directoryPath.empty() )
		throw DirectoryRemoveFailed(
        	"EmbeddedFileSystemManager::removeDirectory: "
            "void directory specified" ) ;

	if ( recursive )
		throw DirectoryRemoveFailed(
        	"EmbeddedFileSystemManager::removeDirectory: "
            "no recursive operation supported" ) ;

	if ( PHYSFS_delete( directoryPath.c_str() ) == 0 )
		throw DirectoryRemoveFailed( 
			"EmbeddedFileSystemManager::removeDirectory failed in 'rmdir' for " 
			+ directoryPath + ": " + GetBackendLastError() ) ;


#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
    	"EmbeddedFileSystemManager::removeDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



void EmbeddedFileSystemManager::moveDirectory( 
		const string & sourceDirectoryPath, const string & targetDirectoryPath )
	throw( DirectoryMoveFailed )
{

	throw DirectoryMoveFailed( "EmbeddedFileSystemManager::moveDirectory: "
		"operation not available on embedded filesystems." ) ;
        
}



void EmbeddedFileSystemManager::copyDirectory( 
		const string & sourceDirectoryPath, const string & targetDirectoryPath )
	throw( DirectoryCopyFailed )
{

	throw DirectoryCopyFailed( "EmbeddedFileSystemManager::copyDirectory: "
		"operation not available on embedded filesystems." ) ;
  		
}



time_t EmbeddedFileSystemManager::getLastChangeTimeDirectory( 
	const string & directoryPath ) throw( DirectoryLastChangeTimeRequestFailed )
{

 	/*
     * This is a member method, the embedded filesystem has thus already
     * been initialized in the EmbeddedFileSystemManager constructor.
     *
     */
     
#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 time = PHYSFS_getLastModTime( directoryPath.c_str() ) ;
    
    if ( time == -1 )
		throw DirectoryLastChangeTimeRequestFailed( 
        	"EmbeddedFileSystemManager::getLastChangeTimeDirectory failed: "
            + GetBackendLastError() ) ;
    
    return static_cast<time_t>( time ) ;
    		
#else // OSDL_USES_PHYSICSFS

	throw DirectoryLastChangeTimeRequestFailed(
    	"EmbeddedFileSystemManager::getLastChangeTimeDirectory failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

	

bool EmbeddedFileSystemManager::isAValidDirectoryPath( 
	const string & directoryString ) throw()
{

	// A priori correct:
	return true ;

}	
			
				
			
bool EmbeddedFileSystemManager::isAbsolutePath( const string & path ) throw()
{

	// Semantics depend on being in read or write context...
    
	if ( path.empty() )
		return false ;

	/*
	 * Starts with separator, or with prefix (if prefix is used):
	 * absolute path.
	 *
	 */
	 
	return ( path[0] == Separator ) ;

}	
			
			
			
std::string EmbeddedFileSystemManager::getCurrentWorkingDirectoryPath()	
	throw( DirectoryGetCurrentFailed )
{

	// Branch directly to the standard filesystem manager?
    
#ifdef CEYLAN_USES_GETCWD

	/*
	 * With following automatic variable, frame size is deemed 
	 * 'too large for reliable stack checking':
	 
	char buf[ PATH_MAX + 1 ] ;
	 
	 * Another solution would be to use a static string, but this method 
	 * would not be reentrant anymore.
	 *
	 * Hence using dynamic allocation, even if slower:
	 *
	 */

	char * buf = new char[ PATH_MAX + 1 ] ;
	
	if ( ::getcwd( buf, PATH_MAX ) )
	{
	
		string res( buf ) ;
		delete [] buf ;
		
		return res ;
		
	}
	else
	{
	
		delete [] buf ;
	
		throw DirectoryGetCurrentFailed(
			"EmbeddedFileSystemManager::getCurrentWorkingDirectoryPath: "
			"unable to determine current directory: " + explainError() ) ;
			
	}		
		 
#else // CEYLAN_USES_GETCWD

#ifdef CEYLAN_USES__GETCWD


	/*
	 * With following automatic variable, frame size is deemed 
	 * 'too large for reliable stack checking':
	 
	char buf[ PATH_MAX + 1 ] ;
	 
	 * Another solution would be to use a static string, but this method 
	 * would not be reentrant anymore.
	 *
	 * Hence using dynamic allocation, even if slower:
	 *
	 */

	char * buf = new char[ PATH_MAX + 1 ] ;
	
	if ( ::_getcwd( buf, PATH_MAX ) )
	{
	
		string res( buf ) ;
		delete [] buf ;
		
		return res ;
		
	}
	else
	{
	
		delete [] buf ;
	
		throw DirectoryGetCurrentFailed(
			"EmbeddedFileSystemManager::getCurrentWorkingdirectoryPath: "
			"unable to determine current directory: " + explainError() ) ;
			
	}		

#else // CEYLAN_USES__GETCWD

	throw DirectoryGetCurrentFailed(
		"EmbeddedFileSystemManager::getCurrentWorkingdirectoryPath: "
		"not available on this platform" ) ;

#endif // CEYLAN_USES__GETCWD

#endif // CEYLAN_USES_GETCWD		

}	



void EmbeddedFileSystemManager::changeWorkingDirectory( 
	const string & newWorkingDirectory ) throw( DirectoryChangeFailed )
{

	// Branch directly to the standard filesystem manager?

#ifdef CEYLAN_USES_CHDIR

	if ( ::chdir( newWorkingDirectory.c_str() ) != 0 )

#else // CEYLAN_USES_CHDIR

#ifdef CEYLAN_USES__CHDIR

	if ( ::_chdir( newWorkingDirectory.c_str() ) != 0 )

#else // CEYLAN_USES__CHDIR

	throw DirectoryChangeFailed(
		"EmbeddedFileSystemManager::changeWorkingDirectory: "
		"not supported on this platform" ) ;

#endif // CEYLAN_USES__CHDIR

#endif // CEYLAN_USES_CHDIR


		// The indendation is correct, we are after a 'if' !
		throw DirectoryChangeFailed(
			"EmbeddedFileSystemManager::changeWorkingDirectory: "
			"unable to change current working directory to "
			+ newWorkingDirectory + ": " + explainError() ) ;

}	




// EmbeddedFileSystemManager own section.


std::string EmbeddedFileSystemManager::GetBackendLastError() throw()
{

	// No special check of filesystem manager availability.
    
    return string( PHYSFS_getLastError() ) ;
    
}



Ceylan::Byte EmbeddedFileSystemManager::GetXORByte() throw()
{

    return XORByte ;
    
}



const string EmbeddedFileSystemManager::toString( 
	Ceylan::VerbosityLevels level ) const throw()
{

	return "Embedded filesystem manager, based on the PhysicsFS backend" ;
	
}
	


// Static section.


EmbeddedFileSystemManager &
		EmbeddedFileSystemManager::GetEmbeddedFileSystemManager( bool cypher ) 
	throw( EmbeddedFileSystemManagerException )
{

	if ( _EmbeddedFileSystemManager == 0 )
		_EmbeddedFileSystemManager = new EmbeddedFileSystemManager( 
        	/* use cypher */ cypher ) ;
	
	return *_EmbeddedFileSystemManager ;	
	
}



bool EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() 
	throw( EmbeddedFileSystemManagerException )
{

	// Returned value ignored, only side-effect wanted:
	GetEmbeddedFileSystemManager( /* use cypher */ true ) ;
	
    // Cyphering activated by default:
    return true ;
    	
}
	
	
	
void EmbeddedFileSystemManager::RemoveEmbeddedFileSystemManager() throw()
{

	if ( _EmbeddedFileSystemManager != 0 )
	{
	
		if ( FileSystemManager::_CurrentDefaultFileSystemManager ==
				_EmbeddedFileSystemManager )
			FileSystemManager::_CurrentDefaultFileSystemManager = 0 ;	
			
		delete _EmbeddedFileSystemManager ;
		_EmbeddedFileSystemManager = 0 ;
		
	}
	
}




EmbeddedFileSystemManager::EmbeddedFileSystemManager( bool cypherWritings ) 
		throw( EmbeddedFileSystemManagerException ):
    _cypher( cypherWritings )
{

	
#if OSDL_USES_PHYSICSFS
	
	PHYSFS_Version compileTimePhysicsFSVersion ;
	PHYSFS_VERSION( &compileTimePhysicsFSVersion ) ;

	PHYSFS_Version linkTimePhysicsFSVersion ;
    PHYSFS_getLinkedVersion( &linkTimePhysicsFSVersion ) ;
    
    LogPlug::debug( "Using PhysicsFS backend, compiled against the " 
		+ Ceylan::toNumericalString( compileTimePhysicsFSVersion.major ) + "."
		+ Ceylan::toNumericalString( compileTimePhysicsFSVersion.minor ) + "."
		+ Ceylan::toNumericalString( compileTimePhysicsFSVersion.patch ) 
		+ " version, linked against the "
		+ Ceylan::toNumericalString( linkTimePhysicsFSVersion.major ) + "."
		+ Ceylan::toNumericalString( linkTimePhysicsFSVersion.minor ) + "."
		+ Ceylan::toNumericalString( linkTimePhysicsFSVersion.patch ) 
        + " version." ) ;
    
    if ( PHYSFS_init( LogPlug::GetFullExecutablePath().c_str() ) == 0 )
    	throw EmbeddedFileSystemManagerException( 
        	"EmbeddedFileSystemManager constructor failed: "
        	+ GetBackendLastError() ) ;    	
	
	list<string> archiveTypes ;

	string res = "Supported archive types are: " ;

    const PHYSFS_ArchiveInfo ** i = PHYSFS_supportedArchiveTypes() ;
    
 	for ( ; *i != 0; i++ )
	{
    
    	archiveTypes.push_back( string( (*i)->extension ) + ", which is "
        	+ string( (*i)->description ) ) ;
            
	}
    
    res += Ceylan::formatStringList( archiveTypes ) ;
    
    LogPlug::debug( res ) ;
    
    if ( _cypher )
    {
    
    	LogPlug::debug( "Cyphered writes and reads will be performed." ) ;
        
    }
    else
    {
    	
        LogPlug::debug( 
        	"Raw (not cyphered) writes and reads will be performed." ) ;
    
    }
    
    
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileSystemManagerException( 
		"EmbeddedFileSystemManager constructor failed: "
    	"no PhysicsFS support available." ) ;  
          	
#endif // OSDL_USES_PHYSICSFS
		
}



EmbeddedFileSystemManager::~EmbeddedFileSystemManager() throw()
{

#if OSDL_USES_PHYSICSFS

	if ( PHYSFS_deinit() == 0 )
    	LogPlug::error( "EmbeddedFileSystemManager destructor failed: "
        	+ GetBackendLastError() ) ;

#endif // OSDL_USES_PHYSICSFS
            
}



string EmbeddedFileSystemManager::FindArchivePath( 
		const std::string & archiveFilename ) 
	throw( EmbeddedFileSystemManagerException )
{
	
	string archiveFullPath = archiveFilename ;

	// Searches directly in current working directory:
	if ( ! File::ExistsAsFileOrSymbolicLink( archiveFullPath ) )
	{
		
		// On failure, uses the archive locator:
		try
		{
		
			archiveFullPath = ArchiveFileLocator.find( 
				archiveFullPath ) ;
				
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
				
				throw EmbeddedFileSystemManagerException( 
					"Archive::FindArchivePath: unable to find '"
					+ archiveFilename 
					+ "', exception generation triggered another failure: "
					+ exc.toString() + "." ) ;
			}
				
			throw EmbeddedFileSystemManagerException( 
					"Archive::FindArchivePath: '" + archiveFilename 
					+ "' is not a regular file nor a symbolic link "
					"relative to the current directory (" + currentDir
					+ ") and cannot be found through archive locator ("
					+ ArchiveFileLocator.toString() 
					+ ") based on archive path environment variable ("
					+ ArchivePathEnvironmentVariable + ")." ) ;
					
		}		
	}

	return archiveFullPath ;
	
}


// Protected section.

