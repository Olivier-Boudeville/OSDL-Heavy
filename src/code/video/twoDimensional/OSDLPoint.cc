#include "OSDLPoint.h"


#include <iostream>      // for ostream


using std::string ;
using std::list ;


using namespace OSDL::Video::TwoDimensional ;



Point::Point() throw()
{

}

	
Point::~Point() throw()
{

}


const string Point::toString( Ceylan::VerbosityLevels level ) const throw()
{

    return "Abstract point" ;
	
}


std::ostream & operator << ( std::ostream & os, const Point & p )
{

    return os << p.toString() ;
	
}

