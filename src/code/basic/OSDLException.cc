#include "OSDLException.h"

using std::string ;

using namespace OSDL ;



Exception::Exception( const string & reason ) throw() : 
	Ceylan::Exception( reason ) 
{

}


Exception::~Exception() throw ()
{

}



std::ostream & operator << ( std::ostream & os, const OSDL::Exception & e )
	throw()
{
    return os << e.toString( Ceylan::high ) ;
}
