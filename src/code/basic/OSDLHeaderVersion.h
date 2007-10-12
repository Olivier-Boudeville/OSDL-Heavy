#ifndef OSDL_HEADER_VERSION_H_
#define OSDL_HEADER_VERSION_H_

#include <string>



/*
 * This file is dedicated to the Windows version, as on UNIX it is overwritten
 * by the configure-time generated one.
 *
 * @note This Windows version needs OSDLHeaderVersion.cc, whereas the UNIX
 * one does not.
 *
 */
 

// Allowed because on Windows (hence not installed):
#include "OSDLConfig.h"  // for  OSDL_LIBTOOL_VERSION

 
namespace OSDL
{


	/**
	 * This is the libtool version of the OSDL headers, as defined in the
	 * configure step.
	 *
	 * Allows to detect run-time mismatches between the OSDL headers a 
	 * program or a library was compiled with, and the actual OSDL library
	 * it is then linked to.
	 *
	 * @note Cannot declare here:
	 * 'extern OSDL_DLL const std::string actualOSDLHeaderLibtoolVersion 
	 * 	= OSDL_LIBTOOL_VERSION ;' because with Visual C++ it leads to
	 * multiple definitions for actualOSDLHeaderLibtoolVersion.
	 *
	 */
	extern OSDL_DLL const std::string actualOSDLHeaderLibtoolVersion ;
		
}


#endif // OSDL_HEADER_VERSION_H_

