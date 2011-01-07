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


#include "OSDLBezier.h"

#include "OSDLSurface.h"        // for Surface
#include "OSDLPoint2D.h"        // for Point2D
#include "OSDLVideo.h"          // for VideoModule

#include "Ceylan.h"             // for Ceylan::LogPlug



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_USES_SDL_GFX and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL_GFX and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL_GFX

#include "SDL_gfxPrimitives.h"  // for graphic primitives exported by SDL_gfx

#endif // OSDL_USES_SDL_GFX



using namespace OSDL::Video ;

using namespace Ceylan::Log ;




// A return code of 0 for SDL_gfx functions means no error, contrary to -1.




bool TwoDimensional::drawBezierCurve( 
	Surface & targetSurface, 
	const listPoint2D & controlPoints,
	Ceylan::Uint16 numberOfSteps, 
	Pixels::ColorElement red, 
	Pixels::ColorElement green, 
	Pixels::ColorElement blue, 
	Pixels::ColorElement alpha )
{
		
	return drawBezierCurve( targetSurface, controlPoints, numberOfSteps, 
		Pixels::convertRGBAToColorDefinition( red, green, blue, alpha ) ) ;

}	



bool TwoDimensional::drawBezierCurve( 
	Surface & targetSurface, 
	const listPoint2D & controlPoints,
	Ceylan::Uint16 numberOfSteps, 
	Pixels::ColorDefinition colorDef )
{

#if OSDL_USES_SDL_GFX

	/*
	 * If a large number of summits is to be used, dynamic allocation
	 * (new/delete) would be better.
	 *
	 */
	Ceylan::System::Size vertexCount = controlPoints.size() ;
	
	/*
	 * Used to be allocated on the stack, but 
	 * 'ISO C++ forbids variable-size array'...
	 *
	 */
	Coordinate * abscissaArray = new Coordinate[ vertexCount ] ;
	Coordinate * ordinateArray = new Coordinate[ vertexCount ] ;
	
	vertexCount = 0 ;
	
	for ( listPoint2D::const_iterator it = controlPoints.begin(); 
		it != controlPoints.end(); it++ )
	{
		abscissaArray[ vertexCount ] = (*it)->getX() ;
		ordinateArray[ vertexCount ] = (*it)->getY() ;
		
		vertexCount++ ;
	}
	
	int res = ::bezierColor( & targetSurface.getSDLSurface(), 
		abscissaArray, ordinateArray, 
		static_cast<int>( vertexCount ), numberOfSteps, 
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) ;
	
	delete [] abscissaArray ;
	delete [] ordinateArray ;
		
	return ( res == 0 ) ;	
	
#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawBezierCurve failed: "
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX

}	

