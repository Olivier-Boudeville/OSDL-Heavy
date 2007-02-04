#ifndef OSDL_HEADER_VERSION_H_
#define OSDL_HEADER_VERSION_H_

#include <string>


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
		= "0.4.0" ;
		
}


#endif // OSDL_HEADER_VERSION_H_
