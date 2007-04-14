#include "OSDLHeaderVersion.h"

// Allowed because on Windows (hence not installed) :
#include "OSDLConfig.h"  // for OSDL_LIBTOOL_VERSION

/*
 * This file exists only for the Windows build, as on
 * UNIX the OSDLHeaderVersion.h header file declares
 * and defines actualOSDLHeaderLibtoolVersion with no
 * multiple definitions when linking.
 *
 */
const std::string OSDL::actualOSDLHeaderLibtoolVersion 
	= OSDL_LIBTOOL_VERSION ;

