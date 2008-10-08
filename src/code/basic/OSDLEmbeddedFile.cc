#include "OSDLEmbeddedFile.h"


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                  // for PHYSFS_getDirSeparator and al
#endif // OSDL_USES_PHYSICSFS


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


using namespace Ceylan ;
using namespace Ceylan::System ;
using namespace Ceylan::Log ;



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
           	 + GetBackendLastError() ) ;
    
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
	serialize( f._physfsHandle ) ;
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


}



Size EmbeddedFile::read( Ceylan::Byte * buffer, Size maxLength ) 
	throw( InputStream::ReadFailedException )
{
     
#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 objectCount = PHYSFS_read( _physfsHandle, 
    	static_cast<void *>( buffer ), /* object size */ 1, maxLength ) ;
    
    if ( objectCount == -1 )
		throw InputStream::ReadFailedException(
    		"EmbeddedFile::read failed: " + GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( objectCount ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw InputStream::ReadFailedException(
    	"EmbeddedFileSystemManager::read failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::write( const string & message ) 
	throw( OutputStream::WriteFailedException )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 objectCount = PHYSFS_write( _physfsHandle, 
    	static_cast<void *>( buffer ), /* object size */ 1, maxLength ) ;
    
    if ( objectCount == -1 )
		throw OutputStream::WriteFailedException(
    		"EmbeddedFile::write failed: " + GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( objectCount ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw OutputStream::WriteFailedException(
    	"EmbeddedFileSystemManager::write failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}



Size EmbeddedFile::write( const Ceylan::Byte * buffer, Size maxLength ) 
	throw( OutputStream::WriteFailedException )
{

#if CEYLAN_ARCH_NINTENDO_DS

	throw OutputStream::WriteFailedException( "EmbeddedFile::write:"
		"not supported on the Nintendo DS platform." ) ;
		

#else // CEYLAN_ARCH_NINTENDO_DS


#if CEYLAN_USES_FILE_DESCRIPTORS

	try
	{	
	
		return System::FDWrite( _physfsHandle, buffer, maxLength ) ;
		
	}
	catch( const Ceylan::Exception & e )
	{
		throw OutputStream::WriteFailedException( 
			"EmbeddedFile::write failed for file '" + _name + "': " 
			+ e.toString() ) ;
	}

#else // CEYLAN_USES_FILE_DESCRIPTORS	



#if CEYLAN_PREFERS_RDBUF_TO_DIRECT_FSTREAM
	 	
		
	SignedSize n = ( _fstream.rdbuf() )->sputn( buffer, 
		static_cast<std::streamsize>( maxLength ) ) ;

	if ( n < 0 )
		throw OutputStream::WriteFailedException( 
			"EmbeddedFile::write failed for file '" 
			+ _name + "': negative size written" ) ;

	Size realSize = static_cast<Size>( n ) ;
	
	if ( realSize < maxLength )
		throw OutputStream::WriteFailedException( 
			"EmbeddedFile::write failed for file '" + _name 
			+ "', fewer bytes wrote than expected: " + interpretState() ) ;
			
	/*
	 * Probably useless:
	 		
	if ( ! _fstream.good() )
		throw WriteFailed( "File::write failed for file '" + _name 
			+ "': " + interpretState() ) ;
	 *
	 */
	 
	 
	if ( Synchronous & _openFlag )		
		_fstream.flush() ;		
	
	return realSize ;


#else // CEYLAN_PREFERS_RDBUF_TO_DIRECT_FSTREAM
	
	
	_fstream.write( buffer, static_cast<std::streamsize>( maxLength ) ) ; 

	if ( ! _fstream.good() )
		throw WriteFailed( "EmbeddedFile::write failed for file '" + _name 
			+ "': " + interpretState() ) ;

	if ( Synchronous & _openFlag )		
		_fstream.flush() ;		

	/*
	 * Drawback of the fstream-based method: the written size is not 
	 * really known.
	 *
	 */
	return maxLength ;


#endif // CEYLAN_PREFERS_RDBUF_TO_DIRECT_FSTREAM
	
#endif // CEYLAN_USES_FILE_DESCRIPTORS	

#endif // CEYLAN_ARCH_NINTENDO_DS

}



// EmbeddedFile-specific methods.


Size EmbeddedFile::getSize() const 
	throw( Ceylan::System::FileSizeRequestFailed )
{

#if OSDL_USES_PHYSICSFS

	PHYSFS_sint64 size = PHYSFS_fileLength( _physfsHandle ) ;

	if ( size == - 1 )
		throw Ceylan::System::FileSizeRequestFailed(
 		   	"EmbeddedFile::getSize failed: " + GetBackendLastError() ) ;
 	else    
	    return static_cast<Size>( size ) ;
            		
#else // OSDL_USES_PHYSICSFS

	throw Ceylan::System::FileSizeRequestFailed(
    	"EmbeddedFile::getSize failed: "
    	"no PhysicsFS support available." ) ;
        
#endif // OSDL_USES_PHYSICSFS

}

    

void EmbeddedFile::serialize( PHYSFS_File * handle ) const 
	throw( EmbeddedFileException )
{

#if CEYLAN_ARCH_NINTENDO_DS

	throw EmbeddedFileException( "EmbeddedFile::serialize: "
		"not supported on the Nintendo DS platform." ) ;
	
#else // CEYLAN_ARCH_NINTENDO_DS

#if CEYLAN_USES_FILE_DESCRIPTORS

	// Let EmbeddedFileException propagate:
	FromPhysfsHandletoPhysfsHandle( _physfsHandle, handle, size() ) ;
	
#else // CEYLAN_USES_FILE_DESCRIPTORS

	throw EmbeddedFileException( "EmbeddedFile::serialize: "
		"file descriptor feature not available" ) ;
		
#endif // CEYLAN_USES_FILE_DESCRIPTORS

#endif // CEYLAN_ARCH_NINTENDO_DS

}



PHYSFS_File EmbeddedFile::getPhysicsFSHandle() const 
	throw( EmbeddedFileException )
{

#if CEYLAN_USES_FILE_DESCRIPTORS

	return _physfsHandle ;
	
#else // if CEYLAN_USES_FILE_DESCRIPTORS

	throw EmbeddedFileException( 
		"EmbeddedFile::getPhysicsFSHandle: "
		"file descriptor feature not available" ) ;
		
#endif // if CEYLAN_USES_FILE_DESCRIPTORS	

}




StreamID EmbeddedFile::getStreamID() const throw()
{

#if CEYLAN_USES_FILE_DESCRIPTORS

	return static_cast<StreamID>( getPhysicsFSHandle() ) ;
	
#else // if CEYLAN_USES_FILE_DESCRIPTORS

	// No appropriate identifier found:	
	return -1 ;
	
#endif // if CEYLAN_USES_FILE_DESCRIPTORS	

}



const std::string EmbeddedFile::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res = "Embedded file object for filename '" + _name + "'" ;

#if CEYLAN_USES_FILE_DESCRIPTORS
	res += ", with file descriptor " + Ceylan::toString( getPhysicsFSHandle() ) ;
#endif // CEYLAN_USES_FILE_DESCRIPTORS	
	
	res += ", with opening openFlags = " + Ceylan::toString( _openFlag ) 
		+ ", with mode openFlags = " + Ceylan::toString( _permissions ) ; 

#if CEYLAN_USES_FILE_LOCKS

	if ( _lockedForReading ) 
		res += ", locked for reading" ;
	else
		res += ", not locked for reading" ;
		
	if ( _lockedForWriting ) 
		res += ", locked for writing" ;
	else
		res += ", not locked for writing" ;
		
#endif // CEYLAN_USES_FILE_LOCKS

	return res ;
	
}



string EmbeddedFile::InterpretState( const ifstream & inputFile ) throw()
{

	if ( inputFile.good() )
		return "Embedded file is in clean state (no error)" ;
		
	string res = "Embedded file in error" ;
	
	if ( inputFile.rdstate() & ifstream::badbit )
		res += ", critical error in stream buffer" ;
		
	if ( inputFile.rdstate() & ifstream::eofbit )
		res += ", End-Of-File reached while extracting" ;
		
	if ( inputFile.rdstate() & ifstream::failbit )
		res += ", failure extracting from stream" ;
		
	return res ;

}



string EmbeddedFile::InterpretState( const fstream & file ) throw()
{

	if ( file.good() )
		return "Embedded file is in clean state (no error)" ;
		
	string res = "Embedded file in error" ;
	
	if ( file.rdstate() & fstream::badbit )
		res += ", critical error in stream buffer" ;
		
	if ( file.rdstate() & fstream::eofbit )
		res += ", End-Of-File reached while extracting" ;
		
	if ( file.rdstate() & fstream::failbit )
		res += ", failure extracting from stream" ;
		
	return res ;

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
	File( name, openFlag, permissions )
{

	// (File constructor may raise FileException)
	
#if OSDL_USES_PHYSICSFS

	EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;
    
	if ( openFlag && NonBlocking )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	+ "cannot open a file in non-blocking mode." ) ;
            
	if ( openFlag && Synchronous )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	+ "cannot open a file in synchronous mode." ) ;
            
	if ( ! ( openFlag && Binary ) )
    	throw Ceylan::System::FileException( "EmbeddedFile constructor failed: "
        	+ "cannot open a file in non-binary mode." ) ;
            
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
    
	if ( _openFlag && Ceylan::System::File::Write )
    {
    
    	file = PHYSFS_openWrite( _name.c_str() ) ;
        
        if ( file == 0 )
        	throw FileOpeningFailed( "EmbeddedFile::reopen for writing failed: "
            	+ GetBackendLastError() ) ;
    
    }
    else if ( _openFlag && Ceylan::System::File::Append )
    {
    
    	file = PHYSFS_openAppend( _name.c_str() ) ;
        
        if ( file == 0 )
        	throw FileOpeningFailed( 
            	"EmbeddedFile::reopen for appending failed: "
            	+ GetBackendLastError() ) ;
    
    }
    else if ( _openFlag && Ceylan::System::File::Read )
    {
    	file = PHYSFS_openWrite( _name.c_str() ) ;
        
        if ( file == 0 )
        	throw FileOpeningFailed( "EmbeddedFile::reopen for reading failed: "
            	+ GetBackendLastError() ) ;
    
    }
    else
    {
    
    	throw FileOpeningFailed( "EmbeddedFile::reopen failed: "
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

	
	
void EmbeddedFile::FromPhysfsHandletoPhysfsHandle( 
		PHYSFS_File & from, PHYSFS_File & to, Size length ) 
    throw( EmbeddedFileException )
{

	// Note nevertheless this could be implemented like FromFDtoFD.
    
	throw EmbeddedFileException( 
    	"EmbeddedFile::FromPhysfsHandletoPhysfsHandle: "
		"operation not available on embedded filesystems." ) ;
        	
}

