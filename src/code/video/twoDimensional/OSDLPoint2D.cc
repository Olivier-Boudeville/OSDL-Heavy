#include "OSDLPoint2D.h"




using std::string ;
using std::list ;

using namespace OSDL::Video::TwoDimensional ;


const Point2D Point2D::Origin( static_cast<Coordinate>( 0 ), 0 ) ;


Point2D::Point2D( Coordinate x, Coordinate y ) throw()
{

	_coord[0] = x ;
	_coord[1] = y ;
	
}


Point2D::Point2D( const Point2D & source ) throw() 
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ; 
	  
}


Point2D::Point2D( const Ceylan::Maths::Linear::Bipoint & source ) throw()
{

	_coord[0] = static_cast<Coordinate>( source.getX() ) ;
	_coord[1] = static_cast<Coordinate>( source.getY() ) ;
	
}


Point2D::Point2D( const Ceylan::Maths::Linear::Vector2 & source ) throw()
{

	_coord[0] = static_cast<Coordinate>( source.getX() ) ;
	_coord[1] = static_cast<Coordinate>( source.getY() ) ;
	
}



Point2D & Point2D::CreateFrom( FloatingPointCoordinate x,
	FloatingPointCoordinate y ) throw()
{

	return * new Point2D( static_cast<Coordinate>( x ), 
		static_cast<Coordinate>( y ) ) ;
}

	
Point2D::~Point2D() throw()
{

}


void Point2D::setTo( Coordinate x, Coordinate y ) throw() 
{

	_coord[0] = x ;
	_coord[1] = y ;	
	
}


void Point2D::setTo( FloatingPointCoordinate x, FloatingPointCoordinate y )
	throw() 
{

	_coord[0] = static_cast<Coordinate>( x ) ;
	_coord[1] = static_cast<Coordinate>( y ) ; 
	
}



void Point2D::setFrom( const Point2D & source ) throw()
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ;	
	
}


void Point2D::setFrom( const Ceylan::Maths::Linear::Vector2 & source ) throw()
{

	_coord[0] = static_cast<Coordinate>( source.getElementAt( 0 ) ) ;
	_coord[1] = static_cast<Coordinate>( source.getElementAt( 1 ) ) ;
	
}


const string Point2D::toString( Ceylan::VerbosityLevels level ) const throw()
{

    return ( string( " ( " ) 
		+ _coord[0] 
		+ string( " ; " ) 
		+ _coord[1]
		+ string( " )" ) ) ;

}


void Point2D::translate( Offset x, Offset y ) throw() 
{

	_coord[0] += x ;
	_coord[1] += y ;
	
}


void Point2D::flip() throw()
{

	_coord[0] = - _coord[0] ;
	_coord[1] = - _coord[1] ;
	
}


void Point2D::flipX() throw() 
{

	_coord[0] = - _coord[0] ;
	
}


void Point2D::flipY() throw()
{

	_coord[1] = - _coord[1] ;
	
}



void Point2D::Translate( list<Point2D *> & pointList, Offset x, Offset y )
	throw()
{

	for ( list<Point2D *>::iterator it = pointList.begin(); 
			it != pointList.end(); it++ )
		(*it)->translate( x, y ) ;
		
}


std::ostream & operator << ( std::ostream & os, const Point2D & p )
{

    return os << p.toString() ;
	
}


