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


#ifndef OSDL_UTILS_H_
#define OSDL_UTILS_H_



#include "OSDLException.h"  // for OSDL::Exception

#include "Ceylan.h"         // for Ceylan::Latin1Char

#include <string>



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_RWops ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 




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
		OSDL_DLL const std::string getBackendLastError() ;	
	
	
    
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
		OSDL_DLL Ceylan::Latin1Char getNativeDirectorySeparator() ;	
	
    
	
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
		OSDL_DLL std::string getProgramPath() ;	
	
    
	
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
		OSDL_DLL std::string getUserDirectory() ;	
	
    
    
    	/**
         * A DataStream is a way of writing and/or reading for an opaque
         * data storage, which can be actually a file, a buffer in memory,
         * or anything else.
         *
         * DataStream abstracts back-end specific SDL_RWops.
         *
         */        
    	typedef ::SDL_RWops DataStream ;
        
        
		
		/**
		 * Returns a DataStream corresponding to specific file object, which
         * can be of any sort (ex: standard file, embedded file, etc.).
		 *
         * @param sourceFile the file from which the DataStream will be
         * created.
         *
         * @note Ownership of the specified is taken, thus the user should
         * not do anything with it afterwards.
         *
         * @throw OSDL::Exception if the operation failed.
         *
		 */
		OSDL_DLL DataStream & createDataStreamFrom( 
        	Ceylan::System::File & sourceFile ) ;	
	
	

		/**
		 * Deletes specified datastream.
         *
         * @throw OSDL::Exception if the operation failed.
         *
		 */
		OSDL_DLL void deleteDataStream( DataStream & datastream ) ;	
	
	
	    
	}
	

}



#endif // OSDL_UTILS_H_ 

