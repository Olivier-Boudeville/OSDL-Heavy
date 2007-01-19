#ifndef OSDL_TEST_EXCEPTION_H_
#define OSDL_TEST_EXCEPTION_H_


#include "OSDLException.h"

#include <string>
#include <iostream>


namespace OSDL
{

	/// Test class for OSDL::Exception.
	class TestException : public Exception
	{

		public:
			explicit TestException( const std::string & reason ) throw() ;
			virtual ~TestException() throw() ;

    } ;

}


std::ostream & operator << ( std::ostream & os, 
	const OSDL::TestException & e ) ;


#endif // OSDL_TEST_EXCEPTION_H_
