#ifndef OSDL_UTILS_H_
#define OSDL_UTILS_H_


#include <string>


namespace OSDL
{


	/**
	 * Various convenient helpers are defined here.
	 * 
	 *
	 */
	namespace Utils
	{

		/**
		 * Returns a description of the back-end related internal error.
		 *
		 */
		const std::string getBackendLastError() throw() ;	
	
	}

}


#endif // OSDL_UTILS_H_ 
