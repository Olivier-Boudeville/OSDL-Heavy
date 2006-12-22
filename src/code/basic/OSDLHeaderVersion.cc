#include "OSDLHeaderVersion.h"


#ifdef OSDL_RUNS_ON_WINDOWS

#include "OSDLConfigForWindows.h"    // for OSDL_LIBTOOL_VERSION

#include <string> 

/*
 * This is the OSDLHeaderVersion.cc version for Windows.
 *
 * On UNIX platforms, this file will be overwritten by the configure-generated
 * OSDLHeaderVersion.cc
 *
 */

const std::string OSDL::actualOSDLHeaderLibtoolVersion
    = OSDL_LIBTOOL_VERSION ;

#else // OSDL_RUNS_ON_WINDOWS

#error "This version of OSDLHeaderVersion.cc is for Windows platforms only."

#endif // OSDL_RUNS_ON_WINDOWS
