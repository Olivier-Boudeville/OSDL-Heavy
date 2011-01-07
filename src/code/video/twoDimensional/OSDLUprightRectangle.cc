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


#include "OSDLUprightRectangle.h"

#include "OSDLPoint2D.h"       // for Point2D
#include "OSDLSurface.h"       // for Surface

#include "Ceylan.h"            // for LogPlug



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>        // for OSDL_DEBUG_RECTANGLES and al 
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL_GFX
#include "SDL_gfxPrimitives.h" // for rectangleRGBA
#endif // OSDL_USES_SDL_GFX




using std::string ;

using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Video::TwoDimensional ;




UprightRectangle::UprightRectangle( const Point2D & upperLeftCorner,
		const Point2D & lowerRightCorner ) :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width( static_cast<Length>( 
		lowerRightCorner.getX() - upperLeftCorner.getX() ) ),
	_height( static_cast<Length>(
		lowerRightCorner.getY() - upperLeftCorner.getY() ) )
{

	if ( lowerRightCorner.getX() < upperLeftCorner.getX() )
		throw VideoException( 
			"UprightRectangle constructor: width is negative ("
			+ Ceylan::toString( static_cast<Ceylan::SignedLongInteger>( 
				lowerRightCorner.getX() - upperLeftCorner.getX() ) )
			+ ")." ) ;
		
	if ( lowerRightCorner.getY() < upperLeftCorner.getY() )
		throw VideoException(
			"UprightRectangle constructor: height is negative ("
			+ Ceylan::toString( static_cast<Ceylan::SignedLongInteger>(
				lowerRightCorner.getY() - upperLeftCorner.getY() ) )
			+ " )." ) ;
		
}



UprightRectangle::UprightRectangle( const Point2D & upperLeftCorner,
		Length width, Length height ) :
	_x( upperLeftCorner.getX() ),
	_y( upperLeftCorner.getY() ),
	_width( width ),
	_height( height )
{

}	
	
	
			
UprightRectangle::UprightRectangle( Coordinate x, Coordinate y,
		 Length width, Length height ) :
	_x( x ),
	_y( y ),
	_width( width ),
	_height( height )
{

}



#if OSDL_USES_SDL


UprightRectangle::UprightRectangle( const LowLevelRect & source ) :
	_x( source.x ),
	_y( source.y ),
	_width( source.w ),
	_height( source.h )
{

}


#else // OSDL_USES_SDL


UprightRectangle::UprightRectangle( const LowLevelRect & source ) 
{
	
	throw VideoException( 
		"UprightRectangle constructor from LowLevelRect failed: "
		"no SDL support available" ) ;
		
}


#endif // OSDL_USES_SDL



UprightRectangle::~UprightRectangle() throw()
{

}




// Section for upper-left corner.



Point2D UprightRectangle::getUpperLeftCorner() const
{

	return Point2D( _x, _y ) ;
	
}



void UprightRectangle::setUpperLeftCorner( Point2D & newUpperLeftCorner )
{

	_x = newUpperLeftCorner.getX() ;
	_y = newUpperLeftCorner.getY() ;	
	
}



Coordinate UprightRectangle::getUpperLeftAbscissa() const
{

	return _x ;
	
}


 
void UprightRectangle::setUpperLeftAbscissa( Coordinate newAbscissa )
{

	_x = newAbscissa ;
	
}



Coordinate UprightRectangle::getUpperLeftOrdinate() const
{

	return _y ;
	
}

 
 
void UprightRectangle::setUpperLeftOrdinate( Coordinate newOrdinate )
{

	_y = newOrdinate ;
	
}




// Section for lower-right corner.



Point2D UprightRectangle::getLowerRightCorner() const
{

	return Point2D( static_cast<Coordinate>( _x + getWidth() ), 
		static_cast<Coordinate>( _y + getHeight() ) ) ;
		
}



void UprightRectangle::setLowerRightCorner( Point2D & newLowerRightCorner )
{

	if ( newLowerRightCorner.getX() < _x || newLowerRightCorner.getY() < _y )
		throw VideoException( "UprightRectangle::setLowerRightCorner: "
			"misplaced lower right corner given (" 
			+ newLowerRightCorner.toString() 
			+ ") for rectangle: " + toString() ) ;
			
	_width  = newLowerRightCorner.getX() - _x ;
	_height = newLowerRightCorner.getY() - _y ;	

}



Coordinate UprightRectangle::getLowerRightAbscissa() const
{

	return _x + getWidth() ;
	
}


 
void UprightRectangle::setLowerRightAbscissa( Coordinate newAbscissa ) 
{

	if ( newAbscissa < _x )
		throw VideoException( "UprightRectangle::setLowerRightAbscissa: "
			"misplaced lower right corner given (abscissa is " 
			+ Ceylan::toString( newAbscissa ) 
			+ ") for rectangle: " + toString() ) ;

	_width = newAbscissa - _x ;
	
}



Coordinate UprightRectangle::getLowerRightOrdinate() const
{

	return _y + getHeight() ;
	
}

 
 
void UprightRectangle::setLowerRightOrdinate( Coordinate newOrdinate ) 
{

	if ( newOrdinate < _y )
		throw VideoException( "UprightRectangle::setLowerRightOrdinate: "
			"misplaced lower right corner given (ordinate is " 
			+ Ceylan::toString( newOrdinate ) 
			+ ") for rectangle: " + toString() ) ;

	_height = newOrdinate - _y ;
	
}



 
Length UprightRectangle::getWidth() const
{

	return _width ;
	
}



void UprightRectangle::setWidth( Length newWidth )
{

	_width = newWidth ;
	
}



Length UprightRectangle::getHeight() const
{

	return _height ;
	
}



void UprightRectangle::setHeight( Length newHeight )
{

	_height = newHeight ;
	
}
		
	
		
bool UprightRectangle::draw( Surface & target, 
	ColorElement red, ColorElement blue, ColorElement green, 
	ColorElement alpha,	bool filled ) const
{
	
	return draw( target, 
		Pixels::convertRGBAToColorDefinition( red, green, blue, alpha ),
		filled ) ;
	
}



bool UprightRectangle::draw( Surface & target, 
	Pixels::ColorDefinition colorDef, bool filled ) const 
{

#if OSDL_USES_SDL_GFX

#if OSDL_DEBUG_RECTANGLES

	LogPlug::trace( "Drawing a " + ( filled ? string( "" ): string( "non " ) ) 
		+ "filled rectangle from [" 
		+ Ceylan::toString( _x ) + ";" + Ceylan::toString( _y ) 
		+ "] to [" 
		+ Ceylan::toString( _x + getWidth() ) + ";" 
		+ Ceylan::toString( _y + getHeight() ) 
		+ "] with color " + Pixels::toString( colorDef ) ) ;
		
#endif // OSDL_DEBUG_RECTANGLES
	
	
	/*
	 * Do not forget to substract one from both lengths, since starting 
	 * from zero:
	 * (ex: starting at 10, length 3 implies draw 10, 11, 12, and 
	 * 12 = 10 + 3 - 1)
	 *
	 */
	
	if ( filled )
	{
	
		return ( ::boxColor( & target.getSDLSurface(), _x, _y, 
			_x + getWidth() - 1, _y + getHeight() - 1,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;
			
	}
	else
	{
	
		return ( ::rectangleColor( & target.getSDLSurface(), _x, _y, 
			_x + getWidth() - 1, _y + getHeight() - 1,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

	}
		
#else // OSDL_USES_SDL_GFX

	return false ;
			
#endif // OSDL_USES_SDL_GFX

}



bool UprightRectangle::drawWithRoundedCorners( Surface & target, 
	Length edgeWidth, Pixels::ColorDefinition edgeColorDef,
	Pixels::ColorDefinition backgroundColorDef ) const
{

	/*
	 * This method has led to way too many issues with color correctness.
	 *
	 * After much efforts, the result is pixel perfect at last, after much
	 * trouble, no matter if alphablending is to be taken into account or not.
	 *
	 */
	 	
	// Uncomment to debug boundaries:
	// draw( target, Pixels::Yellow, /* filled */ false ) ;
	
	/*
	 * First render a full disc with a ring in it (with correct edge and
	 * background colors), then split it in four equal parts to build the
	 * corners, then finish with the drawing of the edges.
	 *
     */
	
	Length radius ;
	 
	try
	{
		radius = computeRadiusForRoundRectangle( edgeWidth ) ;
	}
	catch( const VideoException & e )
	{
		LogPlug::error( 
			"UprightRectangle::drawWithRoundedCorners: nothing drawn: " 
			+ e.toString() ) ;

		return false ;

	}
		
			
	// Prepare the disc:
		
			
	/*
	 * Note that 2 * radius is not always the same as 
	 * Min( getWidth(), getHeight() ) due to rounding.
	 *
	 */
	
	Surface * disc ;
	
	Pixels::ColorMask redMask, greenMask, blueMask, alphaMask ;
	
	Pixels::getCurrentColorMasks( target.getPixelFormat(), 
		redMask, greenMask, blueMask, alphaMask ) ;
	
	if ( ( edgeColorDef.unused != Pixels::AlphaOpaque ) 
		|| ( backgroundColorDef.unused != Pixels::AlphaOpaque ) )
	{
	

		/*
		 * We are here in the case where alphablending must be managed.
		 * 
		 * Using alpha used to lead to nasty rounding errors in colors 
		 * that make corners (actually the whole split disc) appear slightly
		 * differently from other parts of the rectangle.
		 *
		 * One way of avoiding having different rounding errors for colors
		 * between the disc and the rectangles would be to have the same 
		 * pixel format for the disc than the target surface.
		 * However the target surface may not have an alpha channel or room 
		 * for it (ex: already fully packed 16 bit per pixel), hence we 
		 * use a full 32 bit to make any new alpha fit in overall pixel.
		 *
		 * So the ugly artifact is corrected now, at the expense of a 
		 * rather complicated code, which may fail with (very) uncommon 
		 * pixel formats where there would be no room for a leading 
		 * or trailing byte which could be dedicated to the alpha coordinate 
		 * on a 32 bit pixel.
		 *
		 */
		 

		bool alphaShiftModified = false ;
		Ceylan::Uint8 alphaShift = 0 ;
		
		if ( alphaMask == 0 )
		{
		
			alphaShiftModified = true ;
			
			// Alpha coordinate must be preserved:
			alphaMask = 0xff000000 ;
			
			if ( alphaMask & ( redMask | greenMask | blueMask ) )
			{
			
				/*
				 * Damned, this one collides with other color masks, try 
				 * last chance:
				 *
				 */
				alphaMask = 0x000000ff ;
				
				if ( alphaMask & ( redMask | greenMask | blueMask ) )
				{
					LogPlug::error( 
						"UprightRectangle::drawWithRoundedCorners: "
						"unable to correct alpha, blit will be opaque." ) ;
					alphaMask = 0 ;	
				}
				else
				{
					/*
					 * Second alpha proposal is ok, let's keep it. 
					 * alphaShift is already ok (0).
					 *
					 */
				}
			}
			else
			{
			
				/*
				 * First alpha proposal is ok, let's keep it (3 byte offset 
				 * to have alpha):
				 *
				 */
				alphaShift = 24 ;
				
			}
		}
		
		
#if OSDL_DEBUG_RECTANGLES

		LogPlug::trace( 
			"UprightRectangle::drawWithRoundedCorners: alpha mask is: " 
			+ Ceylan::toString( alphaMask, /* bitfield */ true )
			+ ", alpha shift is: " 
			+ Ceylan::toNumericalString( alphaShift ) ) ;
			
#endif // OSDL_DEBUG_RECTANGLES
		
		disc = new Surface( Surface::Software | Surface::AlphaBlendingBlit, 
			2 * radius, 2 * radius, 32, 
			redMask, greenMask, blueMask, alphaMask ) ;
		
		if ( alphaShiftModified )	
			disc->getPixelFormat().Ashift = alphaShift ;
			
	}
	else
	{
	
		/*
		 * No alphablending issue (full opaque colors).
		 *
		 * As the previous method does not work properly in this case 
		 * (rounding errors which in this case make corners appear with a
		 * slightly different color from the inner color, again!),
		 * we use here a colorkey:
		 *
		 */

		disc = new Surface( Surface::Software | Surface::ColorkeyBlit, 
			2 * radius, 2 * radius, target.getBitsPerPixel(), 
			redMask, greenMask, blueMask, 0 ) ;
		
		/*
		 * Picking a color key not corresponding to any of the two disc 
		 * colors: rare colors (ex: Pixels::Bisque) are not used as 
		 * color keys to avoid bugs very hard to detect.
		 *
		 */		 
		ColorDefinition colorKey = Pixels::selectColorDifferentFrom(
			edgeColorDef, backgroundColorDef ) ;
		
		disc->setColorKey( Surface::ColorkeyBlit | Surface::RLEColorkeyBlit,
			colorKey ) ;
		
		disc->fill( colorKey ) ;
		
	}

		
#if OSDL_DEBUG_RECTANGLES

	LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
		"Pixel format of target surface is: " 
		+ Pixels::toString( target.getPixelFormat() ) ) ;
	
	LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
		"Pixel format of disc surface is: " 
		+ Pixels::toString( disc->getPixelFormat() ) ) ;
		
	LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
		"Center of disc location in target rectangle surface after creation: " 
		+ disc->describePixelAt( radius, radius ) 
		+ " in " + disc->toString() ) ;
		
	disc->savePNG( "disc-after-creation.png" ) ;
	
#endif // OSDL_DEBUG_RECTANGLES
		
	if ( ! disc->drawDiscWithEdge( 
			/* x */ radius, 
			/* y */ radius, 
			/* outer radius */ radius,
			/* inner radius */ radius - edgeWidth -1, 
			/* ring */ edgeColorDef, 
			/* disc */ backgroundColorDef, 
			/* blended */ false ) )
		 return false ;
		 
#if OSDL_DEBUG_RECTANGLES

	LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
		"Center of disc location in target rectangle surface after drawing: " 
		+ disc->describePixelAt( radius, radius ) 
		+ " in " + disc->toString() ) ;
		
	disc->savePNG( "disc-after-drawing.png" ) ;
	
#endif // OSDL_DEBUG_RECTANGLES

		
	/*
	 * Blits in place the four corners (clipping might occur if 'target' 
	 * is too small):
	 *
	 */
	
	try 
	{
	
		/*
		 * Both following '1' have be added so that the upper pixel of 
		 * the disk, alone in its row (ordinate) does not seem to be a
		 * disturbing visual artefact.
		 *
		 */
		 
		// North-west:
		UprightRectangle rectangleCorner( 0, 1, radius, radius ) ;
		Point2D drawingPoint( _x, _y + 1 ) ;	
		
		if ( ! disc->blitTo( target, rectangleCorner, drawingPoint ) )
			return false ;
	
	
		// North-east:
		
		// Width unchanged:
		rectangleCorner.setUpperLeftAbscissa( radius + 1 ) ;
		
		drawingPoint.setX( _x + getWidth() - radius + 1 ) ;
		if ( ! disc->blitTo( target, rectangleCorner, drawingPoint ) )
			return false ;

	
		// South-east:
		
		// Height unchanged:
		rectangleCorner.setUpperLeftOrdinate( radius ) ;
		
		drawingPoint.setY( _y + getHeight() - radius ) ;
		if ( ! disc->blitTo( target, rectangleCorner, drawingPoint ) )
			return false ;
	
		// South-west:
		rectangleCorner.setUpperLeftAbscissa( 0 ) ;
		drawingPoint.setX( _x ) ;
		if ( ! disc->blitTo( target, rectangleCorner, drawingPoint ) )
			return false ;

#if OSDL_DEBUG_RECTANGLES

		LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
			"Point in blitted disc on target surface: " 
			+ target.describePixelAt( _x + radius / 2, _y + radius / 2  ) ) ;
			
#endif // OSDL_DEBUG_RECTANGLES
	
	}
	catch( const VideoException & e )
	{
		delete disc ; 
		return false ;
	}
		
	delete disc ; 
	
	
	// Draw the inner part of the rounded rectangle:
	
	UprightRectangle longerInnerRectangle( _x + edgeWidth + 1, _y + radius + 1, 
		getWidth() - 2 * edgeWidth - 1, getHeight() - 2 * radius - 1 ) ;
		
	longerInnerRectangle.draw( target, backgroundColorDef, /* filled */ true ) ;
	
	UprightRectangle topInnerRectangle( _x + radius, _y + edgeWidth + 2, 
		getWidth() - 2 * radius + 1, radius - edgeWidth - 1 ) ;
	topInnerRectangle.draw( target, backgroundColorDef, /* filled */ true ) ;
	
	UprightRectangle bottomInnerRectangle( _x + radius, 
		_y + getHeight() - radius, getWidth() - 2 * radius + 1, 
		radius - edgeWidth ) ;
		
	bottomInnerRectangle.draw( target, backgroundColorDef, /* filled */ true ) ;

#if OSDL_DEBUG_RECTANGLES

	LogPlug::trace( "UprightRectangle::drawWithRoundedCorners: "
		"Point in blitted rectangle on target surface: " 
		+ target.describePixelAt( _x + getWidth() / 2, 
			_y + getHeight() / 2  ) ) ;
			
#endif // OSDL_DEBUG_RECTANGLES
	
	
	// Draw the parallel edges between the corners:
	
	// Upper horizontal edge:
	
	UprightRectangle rectangle( _x + radius, _y + 1, 
		getWidth() - 2 * radius + 1, edgeWidth + 1 ) ;
	rectangle.draw( target, edgeColorDef, /* filled */ true ) ;
	
	
	// Lower horizontal edge:
	
	rectangle.setUpperLeftOrdinate( _y + getHeight() - edgeWidth  ) ;
	rectangle.draw( target, edgeColorDef, /* filled */ true ) ;
	
	
	// Left vertical edge:
	
	/*
	 * This edge seems, with careful watching, a little too on the left, but
	 * actually it is a consequence of the disc drawing, which expands 
	 * quite a lot of the left.
	 *
	 * Moving the edge one pixel to the right would lead to an offset with 
	 * the top and bottom fronteer with the disc.
	 *
	 */
	rectangle.setUpperLeftAbscissa( _x ) ;
	rectangle.setWidth( edgeWidth + 1 ) ;
	rectangle.setUpperLeftOrdinate( _y + radius + 1 ) ;
	rectangle.setHeight( getHeight() - 2 * radius - 1 ) ;
	rectangle.draw( target, edgeColorDef, /* filled */ true ) ;
	
	
	// Right vertical edge:
	
	/*
	 * With very thin edges, one may notice this edge is thinner. 
	 * It is the cost of having circles that can finish on each side 
	 * of a pixel. It can be hardly noticed, and prevents artefacts
	 * that are a lot more visually disturbing.
	 *
	 */
	rectangle.setUpperLeftAbscissa( _x + getWidth() - edgeWidth ) ;
	rectangle.setWidth( edgeWidth ) ;
	rectangle.setUpperLeftOrdinate( _y + radius + 1 ) ;
	rectangle.draw( target, edgeColorDef, /* filled */ true ) ;
		
	return true ;
		 
}

	
	
Length UprightRectangle::computeRadiusForRoundRectangle( Length edgeWidth )
	const 
{

	/*
	 * Radius must be:
	 *  - necessarily, strictly superior to (edge width + 1): edge is in circle
	 *  - necessarily, less than half of the smallest dimensions 
	 * (otherwise cannot fit)
	 *  - preferably, not too big (otherwise too much room lost in container)
	 *  - preferably, not too close to edge width, so that there is really 
	 * an inner disc
	 *
	 */ 
	Length min = edgeWidth + 1 ;
	
	Length max = Ceylan::Maths::Min<Length>( getWidth(), getHeight() ) / 2 ;
	
	if ( min > max )
		throw VideoException( 
			"UprightRectangle::computeRadiusForRoundRectangle: "
			"no radius could be found for edge width = " 
			+ Ceylan::toString( edgeWidth ) + " and " + toString() ) ;
	
	// Mean value is a good choice: 
	return ( min + max ) / 2 ;
	
}


						
LowLevelRect * UprightRectangle::toLowLevelRect() const 
{

#if OSDL_USES_SDL

	LowLevelRect * res = new LowLevelRect() ;
	
#if OSDL_DEBUG

	if ( res == 0 )
		throw VideoException( 
			"UprightRectangle::toLowLevelRect: not enough memory." ) ;
		
#endif // OSDL_DEBUG
	
	res->x = _x ;
	res->y = _y ;
	res->w = getWidth() ;
	res->h = getHeight() ;
	
	return res ;
	
#else // OSDL_USES_SDL

	throw VideoException( "Image::UprightRectangle::toLowLevelRect failed: "
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL
	
}



const string UprightRectangle::toString( Ceylan::VerbosityLevels level ) const
{

	return "Rectangle whose upper-left corner is " 
		+ Point2D( _x, _y ).toString( level ) 
		+ " ( width = "  + Ceylan::toString( getWidth() )
		+ " ; height = " + Ceylan::toString( getHeight() ) + " )" ;
		
}
	
	

bool operator == ( const UprightRectangle & first, 
	const UprightRectangle & second ) 
{

	if ( first._x != second._x )
		return false ;
		
	if ( first._y != second._y )
		return false ;
	
	if ( first.getWidth() != second.getWidth() )
		return false ;
	
	if ( first.getHeight() != second.getHeight() )
		return false ;
		
	return true ;	
		
}



bool operator != ( const UprightRectangle & first, 
	const UprightRectangle & second ) 
{

	return ! ( first == second ) ;	
		
}



std::ostream & operator << ( std::ostream & os, UprightRectangle & rect )
{

    return os << rect.toString() ;
	
}

