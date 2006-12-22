#include "OSDLGLUprightRectangle.h"

#include "SDL_opengl.h"     	// for glRectf


using std::string ;

using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;
using namespace OSDL::Video::TwoDimensional ;


using Ceylan::Maths::Linear::Bipoint ;



UprightRectangleGL::UprightRectangleGL( const Bipoint & upperLeftCorner,
		const Bipoint & lowerRightCorner ) throw( VideoException ) :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width(  ( GLLength ) lowerRightCorner.getX() - upperLeftCorner.getX() ),
	_height( ( GLLength ) lowerRightCorner.getY() - upperLeftCorner.getY() )
{

#ifdef OSDL_DEBUG

	if ( lowerRightCorner.getX() < upperLeftCorner.getX() )
		throw VideoException( 
			string ( "UprightRectangleGL constructor : width is negative ( " ) 
				+  ( long ) ( lowerRightCorner.getX() - upperLeftCorner.getX() ) + " ) " ) ;
		
	if ( lowerRightCorner.getY() < upperLeftCorner.getY() )
		throw VideoException( 
			string ( "UprightRectangleGL constructor : height is negative ( " )
				+ ( long ) ( lowerRightCorner.getY() - upperLeftCorner.getY() ) + " ) " ) ;
		
#endif

}


UprightRectangleGL::UprightRectangleGL( const Bipoint & upperLeftCorner,
		GLLength width, GLLength height ) throw() :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width( width ),
	_height( height )
{

}	
	
			
UprightRectangleGL::UprightRectangleGL( GLCoordinate x, GLCoordinate y,
		 GLLength width, GLLength height ) throw() :
	_x( x ),
	_y( y ),
	_width( width ),
	_height( height )
{

}


UprightRectangleGL::~UprightRectangleGL() throw()
{

}


Bipoint UprightRectangleGL::getUpperLeftCorner() const throw()
{
	return Bipoint( _x, _y ) ;
}


void UprightRectangleGL::setUpperLeftCorner( Bipoint & newUpperLeftCorner ) throw()
{
	_x = newUpperLeftCorner.getX() ;
	_y = newUpperLeftCorner.getY() ;	
}


GLCoordinate UprightRectangleGL::getUpperLeftAbscissa() const throw()
{
	return _x ;
}
 
 
GLCoordinate UprightRectangleGL::getUpperLeftOrdinate() const throw()
{
	return _y ;
}
 
 
GLLength UprightRectangleGL::getWidth() const throw()
{
	return _width ;
}


void UprightRectangleGL::setWidth( GLLength newWidth ) throw()
{
	_width = newWidth ;
}


GLLength UprightRectangleGL::getHeight() const throw()
{
	return _height ;
}


void UprightRectangleGL::setHeight( GLLength newHeight ) throw()
{
	_height = newHeight ;
}
		
		
bool UprightRectangleGL::draw() const throw()
{
	
	#ifdef OSDL_HAVE_OPENGL
	glRectf( _x, _y, _x + _width, _y + _height ) ;
	#else
	Ceylan::emergencyShutdown( "UprightRectangleGL::draw "
		"called whereas no OpenGL support available." ) ;
	#endif
	
	return true ;
	
}


const string UprightRectangleGL::toString( Ceylan::VerbosityLevels level ) const throw()
{
	return "OpenGL rectangle whose upper-left corner is " + Bipoint( _x, _y ).toString( level ) 
		+ " ( width = "  + Ceylan::toString( _width )
		+ " ; height = " + Ceylan::toString( _height ) + " )" ;
}
	


std::ostream & operator << ( std::ostream & os, UprightRectangleGL & rect ) throw()
{
    return os << rect.toString() ;
}


