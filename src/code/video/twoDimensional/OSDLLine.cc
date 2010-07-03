/*
 * Copyright (C) 2003-2010 Olivier Boudeville
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


#include "OSDLLine.h"


#include "OSDLSurface.h"  // for Surface
#include "OSDLPoint2D.h"  // for Point2D
#include "OSDLVideo.h"    // for VideoModule


// for graphic primitives not exported by SDL_gfx:
#include "OSDLFromGfx.h"






// A return code of 0 for SDL_gfx functions means no error, contrary to -1.

#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"         // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS




#if OSDL_USES_SDL_GFX

// for graphic primitives exported by SDL_gfx:
#include "SDL_gfxPrimitives.h"

#endif // OSDL_USES_SDL_GFX



using namespace OSDL::Video ;
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan ;




bool Line::drawHorizontal( Surface & targetSurface,
	Coordinate xStart, Coordinate xStop, Coordinate y,
	ColorElement red, ColorElement green, ColorElement blue,
	ColorElement alpha )
{

#if OSDL_USES_SDL_GFX

	// Anti-aliasing of horizontal lines does not make sense!

	return ( ::hlineColor( & targetSurface.getSDLSurface(), xStart, xStop, y,
		( static_cast<Ceylan::Uint32>( red   ) << 24 ) |
		( static_cast<Ceylan::Uint32>( green ) << 16 ) |
		( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) |
		( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::drawHorizontal( Surface & targetSurface,
	Coordinate xStart, Coordinate xStop, Coordinate y,
	Pixels::PixelColor actualColor )
{

#if OSDL_USES_SDL_GFX

	// Anti-aliasing of horizontal lines does not make sense!

	return ( ::hlineColorStore( & targetSurface.getSDLSurface(),
		xStart, xStop, y, actualColor) == 0 ) ;

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::drawHorizontal( Surface & targetSurface,
	Coordinate xStart, Coordinate xStop, Coordinate y,
	ColorDefinition colorDef )
{

#if OSDL_USES_SDL_GFX

	// Anti-aliasing of horizontal lines does not make sense!

	return ( ::hlineColor( & targetSurface.getSDLSurface(), xStart, xStop, y,
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::drawVertical( Surface & targetSurface, Coordinate x,
	Coordinate yStart, Coordinate yStop,
	ColorElement red, ColorElement blue, ColorElement green,
	ColorElement alpha )
{

#if OSDL_USES_SDL_GFX

	// Anti-aliasing of vertical lines does not make sense!

	return ( ::vlineColor( & targetSurface.getSDLSurface(), x, yStart, yStop,
		( static_cast<Ceylan::Uint32>( red   ) << 24 ) |
		( static_cast<Ceylan::Uint32>( green ) << 16 ) |
		( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) |
		( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::drawVertical( Surface & targetSurface, Coordinate x,
	Coordinate yStart, Coordinate yStop, ColorDefinition colorDef )
{

#if OSDL_USES_SDL_GFX

	// Anti-aliasing of vertical lines does not make sense!

	return ( ::vlineColor( & targetSurface.getSDLSurface(), x, yStart, yStop,
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::draw( Surface & targetSurface, Coordinate xStart, Coordinate yStart,
	Coordinate xStop, Coordinate yStop,
	ColorElement red, ColorElement green, ColorElement blue,
	ColorElement alpha )
{

#if OSDL_USES_SDL_GFX

	if ( VideoModule::GetAntiAliasingState() )
	{

		return ( ::aalineColorInt( & targetSurface.getSDLSurface(),
			xStart, yStart, xStop, yStop,
			( static_cast<Ceylan::Uint32>( red   ) << 24 ) |
			( static_cast<Ceylan::Uint32>( green ) << 16 ) |
			( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) |
			( static_cast<Ceylan::Uint32>( alpha )       ),
			VideoModule::GetEndPointDrawState() ) == 0 ) ;

	}
	else
	{

		/*
		 * Warning: VideoModule::GetEndPointDrawState() cannot be taken
		 * into account:
		 *
		 */

		return ( ::lineColor( & targetSurface.getSDLSurface(),
			xStart, yStart, xStop, yStop,
			( static_cast<Ceylan::Uint32>( red   ) << 24 ) |
			( static_cast<Ceylan::Uint32>( green ) << 16 ) |
			( static_cast<Ceylan::Uint32>( blue  ) <<  8 ) |
			( static_cast<Ceylan::Uint32>( alpha ) ) ) == 0 ) ;

	}

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::draw( Surface & targetSurface, Coordinate xStart, Coordinate yStart,
	Coordinate xStop, Coordinate yStop, ColorDefinition colorDef )
{

#if OSDL_USES_SDL_GFX

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
		 * Warning: VideoModule::GetEndPointDrawState() cannot be taken
		 * into account:
		 *
		 */

		return ( ::lineColor( & targetSurface.getSDLSurface(),
			xStart, yStart, xStop, yStop,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

	}

#else // OSDL_USES_SDL_GFX

	// A putPixel* fallback could be added here.

	return false ;

#endif // OSDL_USES_SDL_GFX

}



bool Line::draw( Surface & targetSurface,
	Point2D & firstPoint, Point2D & secondPoint,
	Pixels::ColorElement red, Pixels::ColorElement green,
	Pixels::ColorElement blue, Pixels::ColorElement alpha )
{

	return draw( targetSurface, firstPoint.getX(), firstPoint.getY(),
		secondPoint.getX(), secondPoint.getY(), red, green, blue, alpha ) ;

}



bool Line::draw( Surface & targetSurface,
	Point2D & firstPoint, Point2D & secondPoint,
	Pixels::ColorDefinition colorDef )
{

	return draw( targetSurface, firstPoint.getX(), firstPoint.getY(),
		secondPoint.getX(), secondPoint.getY(), colorDef ) ;

}



bool Line::drawCross( Surface & targetSurface, const Point2D & center,
	Pixels::ColorDefinition colorDef, Length squareEdge )
{

	return drawCross( targetSurface, center.getX(), center.getY(),
		colorDef, squareEdge ) ;

}



bool Line::drawCross( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter,
	Pixels::ColorDefinition colorDef, Length squareEdge )
{

	// Draws '\':

	const FloatingPointCoordinate demiEdge =
		static_cast<FloatingPointCoordinate>( squareEdge / 2 ) ;

	bool result = draw( targetSurface,
		static_cast<Coordinate>( xCenter - demiEdge ),
		static_cast<Coordinate>( yCenter - demiEdge ),
		static_cast<Coordinate>( xCenter + demiEdge ),
		static_cast<Coordinate>( yCenter + demiEdge ),
		colorDef ) ;

	// Draws '/':
	return result && draw( targetSurface,
		static_cast<Coordinate>( xCenter - demiEdge ),
		static_cast<Coordinate>( yCenter + demiEdge ),
		static_cast<Coordinate>( xCenter + demiEdge ),
		static_cast<Coordinate>( yCenter - demiEdge ),
		colorDef ) ;

}
