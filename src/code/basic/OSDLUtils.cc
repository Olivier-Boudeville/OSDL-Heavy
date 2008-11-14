#include "OSDLUtils.h"

#include "OSDLEmbeddedFileSystemManager.h" // for Flavour, etc.

#include "Ceylan.h"                        // for explainError


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"                    // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h"       // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"                           // for SDL_GetError, SDL_RWops, etc.
#endif // OSDL_USES_SDL


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                        // for PHYSFS_getDirSeparator and al
#endif // OSDL_USES_PHYSICSFS


#include <cstdio>   // for SEEK_SET, SEEK_CUR, SEEK_END, etc.


using namespace OSDL ;
using namespace OSDL::Utils ;

using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using std::string ;



const string OSDL::Utils::getBackendLastError() throw()
{

#if OSDL_USES_SDL

	return SDL_GetError() ;
	
#else // OSDL_USES_SDL

	// errno supposed used here:
	return Ceylan::System::explainError() ;

#endif // OSDL_USES_SDL

}


Ceylan::Latin1Char OSDL::Utils::getNativeDirectorySeparator() 
	throw( OSDL::Exception )
{

#if OSDL_USES_PHYSICSFS

	string res( PHYSFS_getDirSeparator() );
    
    switch( res.size() )
    {
    
    	case 0:
        	throw OSDL::Exception( "OSDL::Utils::getNativeDirectorySeparator "
            	"failed: empty separator found." ) ;
            break ;
                
    	case 1:
        	return static_cast<Ceylan::Latin1Char>( res[0] ) ;
            break ;
    	
    	default:
        	throw OSDL::Exception( "OSDL::Utils::getNativeDirectorySeparator "
            	"failed: multi-character separator found ("
                + res + ")." ) ;
            break ;

	}
    
    // Never executed:
    return 0 ;
    	
#else // OSDL_USES_PHYSICSFS

	throw OSDL::Exception( "OSDL::Utils::getNativeDirectorySeparator "
		"failed: no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS

}    
           
            
            
std::string OSDL::Utils::getProgramPath() throw( OSDL::Exception )
{

#if OSDL_USES_PHYSICSFS

	// PhysicsFS must be initialized beforehand:
	EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;

	return std::string( PHYSFS_getBaseDir() ) ;
    
#else // OSDL_USES_PHYSICSFS

	throw OSDL::Exception( "OSDL::Utils::getProgramPath failed:"
		"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS


}


std::string OSDL::Utils::getUserDirectory() throw( OSDL::Exception )
{

#if OSDL_USES_PHYSICSFS

	// PhysicsFS must be initialized beforehand:
	EmbeddedFileSystemManager::SecureEmbeddedFileSystemManager() ;

	return std::string( PHYSFS_getUserDir() ) ;
    
#else // OSDL_USES_PHYSICSFS

	throw OSDL::Exception( "OSDL::Utils::getUserDirectory failed:"
		"no PhysicsFS support available." ) ;

#endif // OSDL_USES_PHYSICSFS


}






/**
 * Callback section for all kinds of Ceylan::System::File.
 *
 * Useful to integrate to specific libraries.
 *
 * @note Not in Ceylan as this library must not depend on SDL, and
 * SDL_RWops is needed here.
 *
 */
  
 
 
/**
 * Callback for seek operations.
 *
 * @note No exception can be raised here, only using LogPlug.
 *
 */
static int seekCallback( DataStream * datastream, int offset, int whence )
{

	//LogPlug::debug( "OSDL::Utils::seekCallback called" ) ;
    
	Ceylan::System::File & myFile = * reinterpret_cast<Ceylan::System::File *>(
    	datastream->hidden.unknown.data1 ) ;
        
	Position pos ;

	string seek ;
    
	try
    {
    
		switch( whence )
	    {
    
    		case SEEK_SET:
				//seek = " from begin" ;
        		pos = offset ;
            	break ;
            
	   		case SEEK_CUR:
				//seek = " from current" ;
    	    	pos = myFile.tell() + offset ;
            	break ;
                
    		case SEEK_END:
				/*
				seek = " from end" ;
				LogPlug::debug( 
                	"OSDL::Utils::seekCallback setting to offset to end "
                	+ Ceylan::toString(offset) ) ;
				 */	
            	pos = myFile.size() + offset ;
            	break ;
                
            default:
				LogPlug::error( "OSDL::Utils seekCallback failed: "
                	"unexpected 'whence' parameter." ) ;
                return -1 ;    
            	break ;
    
	    }
    
		if ( pos < 0 )
    	{
		
        	LogPlug::error( "OSDL::Utils seekCallback failed: "
				"attempt to seek past start of file." ) ;
            
			return -1 ;    
        
		}

        
        myFile.seek( pos ) ;

		/*
		LogPlug::debug( "OSDL::Utils::seekCallback: seeking position #" 
        	+ Ceylan::toString( pos ) + seek + " with offset " 
            + Ceylan::toString( offset ) + ", new position is #" 
            + Ceylan::toString( myFile.tell() ) ) ;
         */
		 
	}
    catch( const Ceylan::Exception & e )
    {
    
    	LogPlug::error( "OSDL::Utils seekCallback failed: " + e.toString() ) ;
    
    }
    
    
    return pos ;
    
}    
    


/**
 * Callback for read operations.
 *
 * @note No exception can be raised here, only using LogPlug.
 *
 */
static int readCallback( DataStream * datastream, void *buffer, int chunkSize, 
	int chunkCount )
{

	//LogPlug::debug( "OSDL::Utils::readCallback called" ) ;

	Ceylan::System::File & myFile = * reinterpret_cast<Ceylan::System::File *>(
    	datastream->hidden.unknown.data1 ) ;

	Size readSize ;
    
	try
    {
    
		readSize = myFile.read( static_cast<Ceylan::Byte*>( buffer ),
        	chunkSize*chunkCount ) ;

	}
    catch( const Ceylan::Exception & e )
    {
    
    	LogPlug::error( "OSDL::Utils readCallback failed: " + e.toString() ) ;
    
    }
    
    if ( readSize % chunkSize != 0 )
    {
		
        LogPlug::error( "OSDL::Utils readCallback failed: "
			"fractional chunk read." ) ;
            
		return -1 ;    
        
	}
    
	/*
	LogPlug::debug( "OSDL::Utils::readCallback read " 
    	+ Ceylan::toString( readSize ) + " bytes, for " 
        + Ceylan::toString( chunkSize*chunkCount ) + " requested."  ) ;
     */
	     
    // Integer division:
	return ( readSize / chunkSize ) ;
    
}


 
/**
 * Callback for write operations.
 *
 * @note No exception can be raised here, only using LogPlug.
 *
 */
static int writeCallback( DataStream * datastream, const void *buffer, 
	int chunkSize, int chunkCount )
{

	//LogPlug::debug( "OSDL::Utils::writeCallback called" ) ;

	Ceylan::System::File & myFile = * reinterpret_cast<Ceylan::System::File *>(
    	datastream->hidden.unknown.data1 ) ;

	Size writtenSize ;
    
	try
    {
    
		writtenSize = myFile.write( static_cast<const Ceylan::Byte*>( buffer ),
        	chunkSize*chunkCount ) ;

	}
    catch( const Ceylan::Exception & e )
    {
    
    	LogPlug::error( "OSDL::Utils writeCallback failed: " + e.toString() ) ;
    
    }

    if ( writtenSize % chunkSize != 0 )
    {
		
        LogPlug::error( "OSDL::Utils writeCallback failed: "
			"fractional chunk read." ) ;
            
		return -1 ;    
        
	}
    
    // Integer division:
	return ( writtenSize / chunkSize ) ;

}



/**
 * Callback for close operations.
 *
 * @note No exception can be raised here, only using LogPlug.
 *
 */
static int closeCallback( DataStream * datastream )
{

	//LogPlug::debug( "OSDL::Utils::closeCallback called" ) ;

	Ceylan::System::File & myFile = * reinterpret_cast<Ceylan::System::File *>(
    	datastream->hidden.unknown.data1 ) ;

	try
    {
    
 		myFile.close() ;

	}
    catch( const Ceylan::Exception & e )
    {
    
    	LogPlug::error( "OSDL::Utils closeCallback failed: " + e.toString() ) ;

		return -1 ;    
    
    }
    
	/*
	LogPlug::debug( "Deleting a datastream corresponding to: " 
    	+ myFile.toString() ) ;
     */
	 
    delete & myFile ;

	SDL_FreeRW( datastream ) ;
    
    return 0 ;
    
}



OSDL::Utils::DataStream & OSDL::Utils::createDataStreamFrom( 
	Ceylan::System::File & sourceFile ) throw( OSDL::Exception )
{

#if OSDL_USES_SDL

	//LogPlug::debug( "Creating a datastream from: " + sourceFile.toString() ) ;
     
	/*
     * Largely inspired from SDL_sound's physfsrwops.h/physfsrwops.c.
     *
     */
     
	SDL_RWops * res = SDL_AllocRW() ;

	if ( res == 0 )
    	throw OSDL::Exception( "OSDL::Utils::createDataStreamFrom failed: "
    		"unable to create a SDL_RWops." ) ;
    
    // Using static methods as basic function pointers needed:
            
   	res->seek  = seekCallback ;
    res->read  = readCallback ;
	res->write = writeCallback ;
	res->close = closeCallback ;
    
	res->hidden.unknown.data1 = & sourceFile ;

	return *res ;
         	
#else // OSDL_USES_SDL

	throw OSDL::Exception( "OSDL::Utils::createDataStreamFrom failed: "
    	"no SDL support available." ) ;
        
#endif // OSDL_USES_SDL

}


void OSDL::Utils::deleteDataStream( DataStream & datastream ) 
        	throw( OSDL::Exception )
{

#if OSDL_USES_SDL

	SDL_FreeRW( &datastream ) ;
    
#else // OSDL_USES_SDL

	throw OSDL::Exception( "OSDL::Utils::deleteDataStream failed: "
    	"no SDL support available." ) ;
        
#endif // OSDL_USES_SDL

}

