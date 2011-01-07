/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDLGLUprightRectangle.h"


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>     // for OSDL_USES_OPENGL and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_USES_OPENGL
#include "SDL_opengl.h"     // for glRectf
#endif // OSDL_USES_OPENGL



using std::string ;

using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;
using namespace OSDL::Video::TwoDimensional ;



using Ceylan::Maths::Linear::Bipoint ;




UprightRectangleGL::UprightRectangleGL( const Bipoint & upperLeftCorner,
		const Bipoint & lowerRightCorner ) :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width( static_cast<GLLength>( lowerRightCorner.getX() -
		upperLeftCorner.getX() ) ),
	_height( static_cast<GLLength>( lowerRightCorner.getY() -
		upperLeftCorner.getY() ) )
{

#if OSDL_DEBUG

	if ( lowerRightCorner.getX() < upperLeftCorner.getX() )
		throw VideoException( 
			"UprightRectangleGL constructor: width is negative ( " 
			+ Ceylan::toString( 
				lowerRightCorner.getX() - upperLeftCorner.getX() ) 
			+ " ) " ) ;
		
	if ( lowerRightCorner.getY() < upperLeftCorner.getY() )
		throw VideoException( 
			"UprightRectangleGL constructor: height is negative ( " 
			+ Ceylan::toString( 
				lowerRightCorner.getY() - upperLeftCorner.getY() ) 
			+ " ) " ) ;
		
#endif // OSDL_DEBUG

}



UprightRectangleGL::UprightRectangleGL( const Bipoint & upperLeftCorner,
		GLLength width, GLLength height ) :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width( width ),
	_height( height )
{

}	
	
	
			
UprightRectangleGL::UprightRectangleGL( GLCoordinate x, GLCoordinate y,
		 GLLength width, GLLength height ) :
	_x( x ),
	_y( y ),
	_width( width ),
	_height( height )
{

}



UprightRectangleGL::~UprightRectangleGL() throw()
{

}



Bipoint UprightRectangleGL::getUpperLeftCorner() const
{

	return Bipoint( _x, _y ) ;
	
}



void UprightRectangleGL::setUpperLeftCorner( 
	Bipoint & newUpperLeftCorner )
{

	_x = newUpperLeftCorner.getX() ;
	_y = newUpperLeftCorner.getY() ;	
	
}



GLCoordinate UprightRectangleGL::getUpperLeftAbscissa() const
{

	return _x ;
	
}
 
 
 
GLCoordinate UprightRectangleGL::getUpperLeftOrdinate() const
{

	return _y ;
	
}
 
 
 
GLLength UprightRectangleGL::getWidth() const
{
	return _width ;
}



void UprightRectangleGL::setWidth( GLLength newWidth )
{

	_width = newWidth ;
	
}



GLLength UprightRectangleGL::getHeight() const
{

	return _height ;
	
}



void UprightRectangleGL::setHeight( GLLength newHeight )
{

	_height = newHeight ;
	
}
		
	
		
bool UprightRectangleGL::draw() const
{
	
#ifdef OSDL_HAVE_OPENGL

	glRectf( _x, _y, _x + _width, _y + _height ) ;
	
#else // OSDL_HAVE_OPENGL

	Ceylan::emergencyShutdown( "UprightRectangleGL::draw "
		"called whereas no OpenGL support available." ) ;
		
#endif //OSDL_HAVE_OPENGL 
	
	return true ;
	
}



const string UprightRectangleGL::toString( Ceylan::VerbosityLevels level ) const
{

	return "OpenGL rectangle whose upper-left corner is " 
		+ Bipoint( _x, _y ).toString( level ) 
		+ " ( width = "  + Ceylan::toString( _width )
		+ " ; height = " + Ceylan::toString( _height ) + " )" ;
		
}
	


std::ostream & operator << ( std::ostream & os, UprightRectangleGL & rect )
{

    return os << rect.toString() ;
	
}

