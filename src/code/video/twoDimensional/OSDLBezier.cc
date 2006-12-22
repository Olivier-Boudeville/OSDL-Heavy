#include "OSDLBezier.h"

#include "OSDLSurface.h"        // for Surface
#include "OSDLPoint2D.h"        // for Point2D
#include "OSDLVideo.h"          // for VideoModule

#include "Ceylan.h"             // for Ceylan::LogPlug

#include "SDL_gfxPrimitives.h"  // for graphic primitives exported by SDL_gfx


using namespace OSDL::Video ;

using namespace Ceylan::Log ;



// A return code of 0 for SDL_gfx functions means no error, contrary to -1.



bool TwoDimensional::drawBezierCurve( Surface & targetSurface, const listPoint2D & controlPoints,
	Ceylan::Uint16 numberOfSteps, Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha ) throw()
{
		
	return drawBezierCurve( targetSurface, controlPoints, numberOfSteps, 
		Pixels::convertRGBAToColorDefinition( red, green, blue, alpha ) ) ;

}	


bool TwoDimensional::drawBezierCurve( Surface & targetSurface, const listPoint2D & controlPoints,
	Ceylan::Uint16 numberOfSteps, Pixels::ColorDefinition colorDef ) throw()
{


	/*
	 * If a large number of summits is to be used, dynamic allocation (new/delete) would be better.
	 *
	 */
	Ceylan::System::Size vertexCount = controlPoints.size() ;
	 
	Coordinate abscissaArray[ vertexCount ] ;
	Coordinate ordinateArray[ vertexCount ] ;
	
	vertexCount = 0 ;
	
	for ( listPoint2D::const_iterator it = controlPoints.begin(); it != controlPoints.end(); it++ )
	{
		abscissaArray[ vertexCount ] = (*it)->getX() ;
		ordinateArray[ vertexCount ] = (*it)->getY() ;
		
		vertexCount++ ;
	}
	
	return ( ::bezierColor( & targetSurface.getSDLSurface(), abscissaArray, ordinateArray,
		vertexCount, numberOfSteps, 
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

}	



