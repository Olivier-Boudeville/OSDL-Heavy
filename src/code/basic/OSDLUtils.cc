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
#include "SDL.h"                           // for SDL_GetError
#endif // OSDL_USES_SDL


#if OSDL_USES_PHYSICSFS
#include "physfs.h"                        // for PHYSFS_getDirSeparator and al
#endif // OSDL_USES_PHYSICSFS



using namespace OSDL ;
using namespace OSDL::Utils ;



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

