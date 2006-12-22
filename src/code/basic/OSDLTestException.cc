#include "OSDLTestException.h"


using std::string ;

using namespace OSDL ;


TestException::TestException( const string & reason ) throw() : Exception( reason )
{

}


TestException::~TestException() throw ()
{

}



std::ostream & operator << ( std::ostream & os, const OSDL::TestException & e )
{
    return os << e.toString() ;
}

