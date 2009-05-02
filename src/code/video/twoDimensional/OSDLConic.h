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


#ifndef OSDL_CONIC_H_
#define OSDL_CONIC_H_


#include "OSDLPixel.h"   // for Pixels::put, Pixels::Pixel




namespace OSDL
{

	
	
	namespace Video
	{



		// Conics are drawn in Surfaces.
		class Surface ;
				
		
		
		namespace TwoDimensional
		{
					
		
			/**
		 	 * Conics section includes support for:
			 *  - circle
			 *  - ellipse
			 *
			 */


			/*
			 * Methods could be inlined in a dedicated file for faster
			 * processing.
			 *
			 */
	
	
			/**
			 * Draws a circle whose center is (xCenter,yCenter) of 
			 * specified radius, with specified RGBA color, on specified
			 * surface.
			 *
			 * @param filled tells whether the circle should be filled (disc).
			 *
			 * @param blended tells whether, for each pixel of the disc, the
			 * specified color should be blended with the one of the target
			 * pixel (if true), or if the disc color should replace the target
			 * one, regardless of any blending (if false). 
			 * Note that only discs may be drawn without being blended: 
			 * circles are always blended, therefore if filled is false, 
			 * blended does not matter.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note A circle will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawCircle( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true, bool blended = true ) ;
	
	
	
			/**
			 * Draws a circle whose center is (xCenter,yCenter) of 
			 * specified radius, with specified color definition, on specified
			 * surface.
			 *
			 * @param colorDef the color definition of the drawing.
			 *
			 * @param filled tells whether the circle should be filled (disc).
			 *
			 * @param blended tells whether, for each pixel of the disc, the
			 * specified color should be blended with the one of the target
			 * pixel (if true), or if the disc color should replace the target
			 * one, regardless of any blending (if false). 
			 * Note that only discs may be drawn without being blended: 
			 * circles are always blended, therefore if filled is false, 
			 * blended does not matter.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note A circle will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawCircle( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Pixels::ColorDefinition colorDef, bool filled = true, 
				bool blended = true ) ;
	
	
		
			/**
			 * Draws a disc whose center is (xCenter,yCenter), of specified
			 * radius (totalRadius), filled with specified color discColorDef,
			 * with a ring in it, on its border, i.e. starting from 
			 * innerRadius to totalRadius, colored with ringColorDef, on
			 * specified surface. 
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param xCenter the abscissa of the center of the disc.
			 *
			 * @param yCenter the ordinate of the center of the disc.
			 *
			 * @param outerRadius the overall disc radius, including the 
			 * border ring.
			 *
			 * @param innerRadius, the radius from which the ring is drawn,
			 * until totalRadius is reached.
			 *
			 * @param ringColorDef the ring color, the color of the border 
			 * of this disk.
			 *
			 * @param discColorDef the inner color of the disc.
			 *
			 * @param blended tells whether, for each pixel of the edged 
			 * disc, the specified color should be blended with the one 
			 * of the target pixel (if true), or if the specified color 
			 * should replace the former one, regardless of any blending 
			 * (if false).
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note This method is rather expensive (slow), especially if
			 * 'discColorDef' is not fully opaque and if 'blended' is false.
			 *
			 * @note Locks surface if needed, clipping is performed, 
			 * disc will be antialiased iff antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawDiscWithEdge( Surface & targetSurface,
				Coordinate xCenter, Coordinate yCenter, 
				Length outerRadius, Length innerRadius, 
				Pixels::ColorDefinition ringColorDef = Pixels::Blue, 
				Pixels::ColorDefinition discColorDef = Pixels::White, 
				bool blended = true ) ;
		
		
		
			/**
			 * Draws an ellipse whose center is (xCenter,yCenter), with
			 * specified horizontal and vertical radius, with specified 
			 * RGBA color, on specified surface.
			 *
			 * @param filled tells whether the ellipse should be filled.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note An unfilled ellipse will be antialiased if antialias
			 * mode is set.
			 *
			 * @note The ellipse axes will be either orthogonal or parallel to 
			 * the screen axes.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawEllipse( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter,
				Length horizontalRadius, Length verticalRadius,				
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true ) ;



			/**
			 * Draws an ellipse whose center is (xCenter,yCenter), with
			 * specified horizontal and vertical radius, with specified 
			 * color definition, on specified surface.
			 *
			 * @param filled tells whether the ellipse should be filled.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note An unfilled ellipse will be antialiased iff antialias 
			 * mode is set.
			 *
			 * @note The ellipse axes will be either orthogonal or parallel to 
			 * the screen axes.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawEllipse( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, 
				Length horizontalRadius, Length verticalRadius,
				Pixels::ColorDefinition colorDef, bool filled = true ) ;

				
		}
		
	}
	
}



#endif // OSDL_CONIC_H_

