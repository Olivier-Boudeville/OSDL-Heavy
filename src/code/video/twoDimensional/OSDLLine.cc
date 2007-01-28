#include "OSDLLine.h"


#include "OSDLSurface.h"  // for Surface
#include "OSDLPoint2D.h"  // for Point2D
#include "OSDLVideo.h"    // for VideoModule

// for graphic primitives exported by SDL_gfx :
#include "SDL_gfxPrimitives.h"  

// for graphic primitives not exported by SDL_gfx :
#include "OSDLFromGfx.h"        



using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan ;


// A return code of 0 for SDL_gfx functions means no error, contrary to -1.




bool Line::drawHorizontal( Surface & targetSurface, 
	Coordinate xStart, Coordinate xStop, Coordinate y, 
	ColorElement red, ColorElement green, ColorElement blue, 
	ColorElement alpha ) throw()
{

	// Anti-aliasing of horizontal lines does not make sense !
	
	return ( ::hlineColor( & targetSurface.getSDLSurface(), xStart, xStop, y, 
		( static_cast<Ceylan::Uint32>( red   ) << 24 ) | 
		( static_cast<Ceylan::Uint32>( green ) << 16 ) | 
		( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) | 
		( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;

}



bool Line::drawHorizontal( Surface & targetSurface, 
	Coordinate xStart, Coordinate xStop, Coordinate y, 
	Pixels::PixelColor actualColor ) throw()
{

	// Anti-aliasing of horizontal lines does not make sense !

	return ( ::hlineColorStore( & targetSurface.getSDLSurface(), 
		xStart, xStop, y, actualColor) == 0 ) ;
		
}
	
	

bool Line::drawHorizontal( Surface & targetSurface, 
	Coordinate xStart, Coordinate xStop, Coordinate y, 
	ColorDefinition colorDef ) throw()
{

	// Anti-aliasing of horizontal lines does not make sense !
	
	return ( ::hlineColor( & targetSurface.getSDLSurface(), xStart, xStop, y, 
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

}



bool Line::drawVertical( Surface & targetSurface, Coordinate x, 
	Coordinate yStart, Coordinate yStop, 
	ColorElement red, ColorElement blue, ColorElement green, 
	ColorElement alpha ) throw()
{

	// Anti-aliasing of vertical lines does not make sense !
	
	return ( ::vlineColor( & targetSurface.getSDLSurface(), x, yStart, yStop,
		( static_cast<Ceylan::Uint32>( red   ) << 24 ) | 
		( static_cast<Ceylan::Uint32>( green ) << 16 ) | 
		( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) | 
		( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;

}



bool Line::drawVertical( Surface & targetSurface, Coordinate x, 
	Coordinate yStart, Coordinate yStop, ColorDefinition colorDef ) throw()
{

	// Anti-aliasing of vertical lines does not make sense !
	
	return ( ::vlineColor( & targetSurface.getSDLSurface(), x, yStart, yStop, 
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

}



bool Line::draw( Surface & targetSurface, Coordinate xStart, Coordinate yStart,
	Coordinate xStop, Coordinate yStop, 
	ColorElement red, ColorElement green, ColorElement blue, 
	ColorElement alpha ) throw()
{
		
	if ( VideoModule::GetAntiAliasingState() )
	{
	
		return ( ::aalineColorInt( & targetSurface.getSDLSurface(), 
			( static_cast<Ceylan::Uint32>( red   ) << 24 ) | 
			( static_cast<Ceylan::Uint32>( green ) << 16 ) | 
			( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) | 
			( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;
			
	}
	else
	{
	
		/*
		 * Warning : VideoModule::GetEndPointDrawState() cannot be taken 
		 * into account :
		 *
		 */
		
		return ( ::lineColor( & targetSurface.getSDLSurface(), 
			xStart, yStart, xStop, yStop,
			( static_cast<Ceylan::Uint32>( red   ) << 24 ) | 
			( static_cast<Ceylan::Uint32>( green ) << 16 ) | 
			( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) | 
			( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;
			
	}
	
}					



bool Line::draw( Surface & targetSurface, Coordinate xStart, Coordinate yStart,
	Coordinate xStop, Coordinate yStop, ColorDefinition colorDef ) throw()
{
	
	if ( VideoModule::GetAntiAliasingState() )
	{
	
		return ( ::aalineColorInt( & targetSurface.getSDLSurface(), 
			xStart, yStart, xStop, yStop,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ),
			VideoModule::GetEndPointDrawState() ) == 0 ) ;	
				
	}
	else
	{
	
		/*
		 * Warning : VideoModule::GetEndPointDrawState() cannot be taken 
		 * into account :
		 *
		 */
		
		return ( ::lineColor( & targetSurface.getSDLSurface(), 
			xStart, yStart, xStop, yStop,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
	}
	
}					



bool Line::draw( Surface & targetSurface, 
	Point2D & firstPoint, Point2D & secondPoint,
	Pixels::ColorElement red, Pixels::ColorElement green, 
	Pixels::ColorElement blue, Pixels::ColorElement alpha )
		throw()
{

	return draw( targetSurface, firstPoint.getX(), firstPoint.getY(), 
		secondPoint.getX(), secondPoint.getY(), red, green, blue, alpha ) ;
		
}	



bool Line::draw( Surface & targetSurface, 
	Point2D & firstPoint, Point2D & secondPoint,
	Pixels::ColorDefinition colorDef ) throw() 
{

	return draw( targetSurface, firstPoint.getX(), firstPoint.getY(), 
		secondPoint.getX(), secondPoint.getY(), colorDef ) ;
		
}	



bool Line::drawCross( Surface & targetSurface, const Point2D & center,
	Pixels::ColorDefinition colorDef, Length squareEdge ) throw() 
{

	return drawCross( targetSurface, center.getX(), center.getY(), 
		colorDef, squareEdge ) ;
		 
}	



bool Line::drawCross( Surface & targetSurface, 
	Coordinate xCenter, Coordinate yCenter,
	Pixels::ColorDefinition colorDef, Length squareEdge ) throw() 
{

	// Draws '\' :
	
	const FloatingPointCoordinate demiEdge = 
		static_cast<FloatingPointCoordinate>( squareEdge / 2 ) ;
		
	bool result = draw( targetSurface, 
		static_cast<Coordinate>( xCenter - demiEdge ),
		static_cast<Coordinate>( yCenter - demiEdge ),
		static_cast<Coordinate>( xCenter + demiEdge ),
		static_cast<Coordinate>( yCenter + demiEdge ),
		colorDef ) ;
		
	// Draws '/' :
	return result && draw( targetSurface, 
		static_cast<Coordinate>( xCenter - demiEdge ),
		static_cast<Coordinate>( yCenter + demiEdge ),
		static_cast<Coordinate>( xCenter + demiEdge ),
		static_cast<Coordinate>( yCenter - demiEdge ),
		colorDef ) ;		
		 
}	

