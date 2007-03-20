#ifndef OSDL_HEADER_VERSION_H_
#define OSDL_HEADER_VERSION_H_

#include <string>



/*
 * This file is dedicated to the Windows version, as on UNIX it is overwritten
 * by the configure-time generated one.
 *
 */
 
 

// Allowed because on Windows (hence not installed) :
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
	 */
	OSDL_DLL const std::string actualOSDLHeaderLibtoolVersion 
		= OSDL_LIBTOOL_VERSION ;
		
}


#endif // OSDL_HEADER_VERSION_H_
