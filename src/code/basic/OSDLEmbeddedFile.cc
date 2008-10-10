#include "OSDLEmbeddedFile.h"

#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                  // for PHYSFS_close and al
#endif // OSDL_USES_PHYSICSFS


#include "OSDLEmbeddedFileSystemManager.h" // for the FileSystemManager


#include "Ceylan.h"                  // for all Ceylan services



/*
 * Implementation notes.
 *
 * @note In a non-static method, no static method should be used, as the former
 * is expected to use the embedded filesystem manager, whereas the latter shall
 * use the default filesystem manager, which may or may not be the embedded
 * one.
 * 
 */


using std::string ;

using namespace Ceylan::System ;
using namespace Ceylan::Log ;

using namespace OSDL ;



EmbeddedFileException::EmbeddedFileException( const string & reason ) throw():
	FileException( reason )
{

}


EmbeddedFileException::~EmbeddedFileException() throw()
{

}
	
	
	
		
// EmbeddedFile implementation.


EmbeddedFile::~EmbeddedFile() throw()
{

	try
	{
		close() ;
	}
	catch( const Stream::CloseException & e )
	{
		LogPlug::error( "EmbeddedFile destructor: close failed: " 
			+ e.toString() ) ;
	}
		
}




// Constructors are in protected section.	

	

// Implementation of instance methods inherited from File.	
	


bool EmbeddedFile::close() throw( Stream::CloseException )
{

#if OSDL_USES_PHYSICSFS
	
	if ( _physfsHandle != 0 )
	{
    
    	if ( PHYSFS_close( _physfsHandle ) == 0 )
        	throw Stream::CloseException( "EmbeddedFile::close failed: "
           	 + EmbeddedFileSystemManager::GetBackendLastError() ) ;
    
    	return true ;
	}	
	else
	{

		throw Stream::CloseException( "EmbeddedFile::close: file '" +  _name 
			+ "' does not seem to have been already opened." ) ;
			
	}

#else // OSDL_USES_PHYSICSFS

	throw Stream::CloseException( "EmbeddedFile::close failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS
	
}



void EmbeddedFile::saveAs( const string & newName ) throw( FileException )
{

#if OSDL_USES_PHYSICSFS

	// Using the helper factory directly to access to the file descriptor:
	EmbeddedFile & f = EmbeddedFile::Create( newName ) ;
	serialize( *f._physfsHandle ) ;
	delete &f ;
	
#else // OSDL_USES_PHYSICSFS

	throw FileException( 
		"EmbeddedFile::saveAs: not implemented on this platform." ) ;
        
#endif // OSDL_USES_PHYSICSFS
	
}




// Locking section.



/**
 * No locking offered by PhysicsFS.
 *
 */



void EmbeddedFile::lockForReading() const throw( FileReadLockingFailed )
{

	throw FileReadLockingFailed( "EmbeddedFile::lockForReading: "
		"lock feature not available" ) ;
        
}



void EmbeddedFile::unlockForReading() const throw( FileReadUnlockingFailed )
{

	throw FileReadUnlockingFailed(
		"EmbeddedFile::unlockForReading: lock feature not available" ) ;
        	
}



void EmbeddedFile::lockForWriting() const throw( FileWriteLockingFailed )
{

	throw FileWriteLockingFailed( "EmbeddedFile::lockForWriting: "
		"lock feature not available" ) ;
	
}



void EmbeddedFile::unlockForWriting() const throw( FileWriteUnlockingFailed )
{

	throw FileWriteUnlockingFailed( "EmbeddedFile::unlockForWriting: "
		"lock feature not available" ) ;

}



bool EmbeddedFile::isLocked() const throw()
{

	return false ;

}



time_t EmbeddedFile::getLastChangeTime() const 
	throw( FileLastChangeTimeRequestFailed )
{

#if OSDL_USES_PHYSICSFS

	try
    {
    
    	return getCorrespondingFileSystemManager().getEntryChangeTime( _name ) ;
        
    } 
    catch( const Ceylan::Exception & e )
    {
    	throw FileLastChangeTimeRequestFailed( 
        	"EmbeddedFile::getLastChangeTime failed: "
            + e.toString() ) ;
    }    

#else // OSDL_USES_PHYSICSFS

	throw FileLastChangeTimeRequestFailed(
    	"EmbeddedFile::getLastChangeTime failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::read( Ceylan::Byte * buffer, Size maxLength ) 
	throw( InputStream::ReadFailedException )
{
     
#if OSDL_USES_PHYSICSFS
    
	PHYSFS_sint64 objectCount = PHYSFS_read( _physfsHandle, 
    	static_cast<void *>( buffer ), /* object size */ 1, maxLength ) ;
    
    if ( objectCount == -1 )
		throw InputStream::ReadFailedException(
    		"EmbeddedFile::read failed: " 
            + EmbeddedFileSystemManager::GetBackendLastError() ) ;
    else if ( objectCount < maxLength )
    {
    
    	LogPlug::warning( "EmbeddedFile::read: only read " 
        	+ Ceylan::toString( static_cast<Size>( objectCount ) ) + " bytes: "
            + EmbeddedFileSystemManager::GetBackendLastError()
            + string( ", end of file " )
            + ( ( PHYSFS_eof(_physfsHandle) == 0 ) ? "not ":"" )
            + string( "reached." ) ) ;
    
    }        
 	
    return static_cast<Size>( objectCount ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw InputStream::ReadFailedException(
    	"EmbeddedFile::read failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::write( const string & message ) 
	throw( OutputStream::WriteFailedException )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 objectCount = PHYSFS_write( _physfsHandle, 
    	static_cast<const void *>( message.c_str() ), /* object size */ 1, 
        message.size() ) ;
    
    if ( objectCount == -1 )
		throw OutputStream::WriteFailedException(
    		"EmbeddedFile::write failed: " 
            + EmbeddedFileSystemManager::GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( objectCount ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw OutputStream::WriteFailedException(
    	"EmbeddedFile::write failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::write( const Ceylan::Byte * buffer, Size maxLength ) 
	throw( OutputStream::WriteFailedException )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 objectCount = PHYSFS_write( _physfsHandle, 
    	static_cast<const void *>( buffer ), /* object size */ 1, maxLength ) ;
    
    if ( objectCount == -1 )
		throw OutputStream::WriteFailedException(
    		"EmbeddedFile::write failed: " 
            + EmbeddedFileSystemManager::GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( objectCount ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw OutputStream::WriteFailedException(
    	"EmbeddedFile::write failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



// EmbeddedFile-specific methods.

Size EmbeddedFile::size() const throw( FileException )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 fileSize = PHYSFS_fileLength( _physfsHandle ) ;

	if ( fileSize == - 1 )
		throw Ceylan::System::FileException(
 		   	"EmbeddedFile::size failed: " 
            + EmbeddedFileSystemManager::GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( fileSize ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::FileException(
    	"EmbeddedFile::size failed: no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

    

void EmbeddedFile::serialize( PHYSFS_File & handle ) 
	throw( EmbeddedFileException )
{

	// Let EmbeddedFileException propagate:
	FromHandletoHandle( *_physfsHandle, handle, size() ) ;
	
}



PHYSFS_File & EmbeddedFile::getPhysicsFSHandle() const 
	throw( EmbeddedFileException )
{

#if OSDL_USES_PHYSICSFS

	if ( _physfsHandle != 0 )
		return *_physfsHandle ;
	else
    	throw EmbeddedFileException( "EmbeddedFile::getPhysicsFSHandle failed: "
        	"null handle found." ) ;
			
#else // OSDL_USES_PHYSICSFS

	throw EmbeddedFileException( "EmbeddedFile::getPhysicsFSHandle: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}




StreamID EmbeddedFile::getStreamID() const throw()
{

#if OSDL_USES_PHYSICSFS

	return reinterpret_cast<StreamID>( & getPhysicsFSHandle() ) ;
	
#else // OSDL_USES_PHYSICSFS

	// No appropriate identifier found:	
	return -1 ;
	
#endif // OSDL_USES_PHYSICSFS

}



const std::string EmbeddedFile::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	return "Embedded file object for filename '" + _name + "'" ;

}



EmbeddedFile & EmbeddedFile::Create( const std::string & filename, 
		OpeningFlag createFlag,	PermissionFlag permissionFlag ) 
	throw( FileException )
{

	// Ensures creation is requested:
	return * new EmbeddedFile( filename, createFlag | File::CreateFile,
		permissionFlag ) ;

}


					
EmbeddedFile & EmbeddedFile::Open( const std::string & filename, 
	OpeningFlag openFlag ) throw( FileException )
{

	// Ensures creation is not requested:
	return * new EmbeddedFile( filename, openFlag & ~File::CreateFile
		/* use default permission flag */ ) ;

}

	
					
										
// Protected section.


EmbeddedFile::EmbeddedFile( const string & name, OpeningFlag openFlag, 
		PermissionFlag permissions ) throw( FileManagementException ):
	File( name, openFlag, permissions ),
    _physfsHandle( 0 )
{

	// (File constructor may raise FileException)

#if OSDL_USES_PHYSICSFS

	EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;
    
	if ( openFlag & NonBlocking )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	"cannot open a file in non-blocking mode." ) ;
            
	if ( openFlag & Synchronous )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	"cannot open a file in synchronous mode." ) ;
            
	if ( ! ( openFlag & Binary ) )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	"cannot open a file in non-binary mode." ) ;
            
	if ( openFlag != DoNotOpen )
		reopen() ;

    			
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS
		
}





// Implementations of inherited methods.


FileSystemManager & EmbeddedFile::getCorrespondingFileSystemManager()
	const throw( FileDelegatingException )
{

	try
	{
	
		return EmbeddedFileSystemManager::GetEmbeddedFileSystemManager() ;
	
	}
	catch( const EmbeddedFileSystemManagerException & e )
	{
	
		throw FileDelegatingException(
			"EmbeddedFile::getCorrespondingFileSystemManager failed: "
			+ e.toString() ) ;
		
	}
	
}
	
	
	
void EmbeddedFile::reopen() throw( FileOpeningFailed )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_File * file ;
        
	if ( _openFlag & Ceylan::System::File::Write )
    {
    
    	file = PHYSFS_openWrite( _name.c_str() ) ;
        
        if ( file == 0 )
        	throw FileOpeningFailed( 
            	"EmbeddedFile::reopen for writing failed for file '"
                + _name + "': "
            	+ EmbeddedFileSystemManager::GetBackendLastError() ) ;
    
    }
    else if ( _openFlag & Ceylan::System::File::AppendFile )
    {
    
    	file = PHYSFS_openAppend( _name.c_str() ) ;
        
        if ( file == 0 )
        	throw FileOpeningFailed( 
           		"EmbeddedFile::reopen for appending failed for file '"
                + _name + "': "
             	+ EmbeddedFileSystemManager::GetBackendLastError() ) ;
    
    }
    else if ( _openFlag & Ceylan::System::File::Read )
    {
    
    	file = PHYSFS_openRead( _name.c_str() ) ;
        
        if ( file == 0 )
        {
        	throw FileOpeningFailed( 
           		"EmbeddedFile::reopen for reading failed for file '"
                + _name + "': "
            	+ EmbeddedFileSystemManager::GetBackendLastError() ) ;
        }           
    }
    else
    {
    
    	throw FileOpeningFailed( 
			"EmbeddedFile::reopen for reading failed for file '" + _name + "': "
        	"only possible opening flags are writing, appending or reading." ) ;
    }
    
     
    _physfsHandle = file ;
    
    			
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::FileOpeningFailed( "EmbeddedFile::reopen failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS
		
}



std::string EmbeddedFile::interpretState() const throw()
{

	return "PhysicsFS-based embedded file" ;
    
}



// Private section.															

	
	
void EmbeddedFile::FromHandletoHandle( PHYSFS_File & from, PHYSFS_File & to,
	Size length ) throw( EmbeddedFileException )
{

	// Note nevertheless this could be implemented like FromFDtoFD.
    
	throw EmbeddedFileException( "EmbeddedFile::FromHandletoHandle: "
		"operation not available on embedded filesystems." ) ;
        	
}

