#include "OSDLPoint3D.h"




using std::string ;
using std::list ;

using namespace OSDL::Video::TwoDimensional ;


const Point3D Point3D::Origin( static_cast<Coordinate>( 0 ), 0, 0 ) ;



Point3D::Point3D( Coordinate x, Coordinate y, Coordinate z ) throw()
{

	_coord[0] = x ;
	_coord[1] = y ;
	_coord[2] = z ;
	
}


Point3D::Point3D( const Point3D & source ) throw() :
	Point()
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ;   
	_coord[2] = source._coord[2] ;
	
}


Point3D::Point3D( const Ceylan::Maths::Linear::Vector3 & source ) throw()
{

	_coord[0] = static_cast<Coordinate>( source.getElementAt( 0 ) ) ;
	_coord[1] = static_cast<Coordinate>( source.getElementAt( 1 ) ) ;
	_coord[2] = static_cast<Coordinate>( source.getElementAt( 2 ) ) ;
	
}


Point3D::Point3D( FloatingPointCoordinate x, FloatingPointCoordinate y,
	FloatingPointCoordinate z ) throw()
{

	_coord[0] = static_cast<Coordinate>( x ) ;
	_coord[1] = static_cast<Coordinate>( y ) ;
	_coord[2] = static_cast<Coordinate>( z ) ;
	
}

	
Point3D::~Point3D() throw()
{

}


void Point3D::setFrom( const Point3D & source ) throw()
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ;	
	_coord[2] = source._coord[2] ;	
		
}


void Point3D::setFrom( const Ceylan::Maths::Linear::Vector3 & source ) throw()
{

	_coord[0] = static_cast<Coordinate>( source.getElementAt( 0 ) ) ;
	_coord[1] = static_cast<Coordinate>( source.getElementAt( 1 ) ) ;
	_coord[2] = static_cast<Coordinate>( source.getElementAt( 2 ) ) ;
	
}


const string Point3D::toString( Ceylan::VerbosityLevels level ) const throw()
{

    return ( string( " ( " ) 
		+ _coord[0] 
		+ string( " ; " ) 
		+ _coord[1]
		+ string( " ; " ) 
		+ _coord[2] 
		+ string( " )" ) ) ;

}


void Point3D::translate( Offset x, Offset y, Offset z ) throw() 
{

	_coord[0] += x ;
	_coord[1] += y ;
	_coord[2] += z ;
	
}


void Point3D::flip() throw()
{

	_coord[0] = - _coord[0] ;
	_coord[1] = - _coord[1] ;
	_coord[2] = - _coord[2] ;
		
}


void Point3D::flipX() throw() 
{

	_coord[0] = - _coord[0] ;
	
}


void Point3D::flipY() throw()
{

	_coord[1] = - _coord[1] ;
	
}


void Point3D::flipZ() throw()
{

	_coord[2] = - _coord[2] ;
	
}



void Point3D::Translate( list<Point3D *> & pointList, 
	Offset x, Offset y, Offset z ) throw()
{

	for ( list<Point3D *>::iterator it = pointList.begin(); 
			it != pointList.end(); it++ )
		(*it)->translate( x, y, z ) ;
		
}



std::ostream & operator << ( std::ostream & os, const Point3D & p ) throw()
{

    return os << p.toString() ;
	
}

