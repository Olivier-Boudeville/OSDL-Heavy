#ifndef OSDL_UTILS_H_
#define OSDL_UTILS_H_


#include "OSDLException.h"  // for OSDL::Exception

#include "Ceylan.h"         // for Ceylan::Latin1Char

#include <string>



namespace OSDL
{


	/**
	 * Various convenient helpers are defined here.
	 * 
	 */
	namespace Utils
	{

		/**
		 * Returns a description of the main back-end related internal error.
		 *
         * @note Main back-end is SDL.
         *
		 */
		OSDL_DLL const std::string getBackendLastError() throw() ;	
	
    
		/**
		 * Returns the native directory separator, as a Latin-1 character.
		 *
 		 * @example Slash or backslash, i.e. '/' or '\'.
         *
         * @throw OSDL::Exception if the separator cannot be described as 
         * a Latin1 character (ex: if ir involves multiple characters)/
         *
         * @note Not to be confused with the separator used in an
         * embedded-in-a-file filesystem.
         *
		 */
		OSDL_DLL Ceylan::Latin1Char getNativeDirectorySeparator() 
        	throw( OSDL::Exception ) ;	
	
    
		/**
		 * Returns the path where the program resides.
		 *
		 * This is the program base directory, i.e. the directory where 
         * it was run from, which is probably the installation directory, 
         * and may or may not be the process's current working directory.
         *
         * @note This base directory can be included in the search path of
         * any embedded filesystem.
         *
         * @see OSDL::EmbeddedFilesystem.
         *
         * @throw OSDL::Exception if the operation failed.
         *
		 */
		OSDL_DLL std::string getProgramPath() throw( OSDL::Exception ) ;	
	
    
		/**
		 * Returns the path where the user's home directory resides.
		 *
		 * This is meant to be a suggestion of where a specific user
         * of the system can store files.
         * On Unix, this is her home directory.
         * On systems with no concept of multiple home directories (MacOS,
         * win95), this will default to something like:
         * "C:\mybasedir\users\username", where "username" will either be the
         * login name, or "default" if the platform does not support multiple
         * users, either.
         *
         * @note This user directory can be included in the write path of
         * any embedded filesystem, also be put near the beginning of the
         * search path.
         *
         * @see OSDL::EmbeddedFilesystem.
         *
         * @throw OSDL::Exception if the operation failed.
         *
		 */
		OSDL_DLL std::string getUserDirectory() throw( OSDL::Exception ) ;	
	
 	    
	}
	

}


#endif // OSDL_UTILS_H_ 

