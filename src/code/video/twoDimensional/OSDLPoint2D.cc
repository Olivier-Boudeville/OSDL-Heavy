/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


#include "OSDLPoint2D.h"




using std::string ;
using std::list ;


using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;



const Point2D Point2D::Origin( static_cast<Coordinate>( 0 ), 0 ) ;



Point2D::Point2D( Coordinate x, Coordinate y )
{

	_coord[0] = x ;
	_coord[1] = y ;
	
}



Point2D::Point2D( const Point2D & source ) :
	Point()
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ; 
	  
}



Point2D::Point2D( const Ceylan::Maths::Linear::Bipoint & source )
{

	_coord[0] = static_cast<Coordinate>( source.getX() ) ;
	_coord[1] = static_cast<Coordinate>( source.getY() ) ;
	
}



Point2D::Point2D( const Ceylan::Maths::Linear::Vector2 & source )
{

	_coord[0] = static_cast<Coordinate>( source.getX() ) ;
	_coord[1] = static_cast<Coordinate>( source.getY() ) ;
	
}



Point2D & Point2D::CreateFrom( FloatingPointCoordinate x,
	FloatingPointCoordinate y )
{

	return * new Point2D( static_cast<Coordinate>( x ), 
		static_cast<Coordinate>( y ) ) ;
}


	
Point2D::~Point2D() throw()
{

}



void Point2D::setTo( Coordinate x, Coordinate y ) 
{

	_coord[0] = x ;
	_coord[1] = y ;	
	
}



void Point2D::setTo( FloatingPointCoordinate x, FloatingPointCoordinate y )
{

	_coord[0] = static_cast<Coordinate>( x ) ;
	_coord[1] = static_cast<Coordinate>( y ) ; 
	
}



void Point2D::setFrom( const Point2D & source )
{

	_coord[0] = source._coord[0] ;
	_coord[1] = source._coord[1] ;	
	
}



void Point2D::setFrom( const Ceylan::Maths::Linear::Vector2 & source )
{

	_coord[0] = static_cast<Coordinate>( source.getElementAt( 0 ) ) ;
	_coord[1] = static_cast<Coordinate>( source.getElementAt( 1 ) ) ;
	
}



const string Point2D::toString( Ceylan::VerbosityLevels level ) const
{

    return ( string( " ( " ) 
		+ _coord[0] 
		+ string( " ; " ) 
		+ _coord[1]
		+ string( " )" ) ) ;

}



void Point2D::translate( Offset x, Offset y ) 
{

	_coord[0] += x ;
	_coord[1] += y ;
	
}



void Point2D::flip()
{

	_coord[0] = - _coord[0] ;
	_coord[1] = - _coord[1] ;
	
}



void Point2D::flipX() 
{

	_coord[0] = - _coord[0] ;
	
}



void Point2D::flipY()
{

	_coord[1] = - _coord[1] ;
	
}



void Point2D::Translate( list<Point2D *> & pointList, Offset x, Offset y )
{

	for ( list<Point2D *>::iterator it = pointList.begin(); 
			it != pointList.end(); it++ )
		(*it)->translate( x, y ) ;
		
}



std::ostream & operator << ( std::ostream & os, const Point2D & p )
{

    return os << p.toString() ;
	
}

