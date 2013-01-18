/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#ifndef OSDL_BEZIER_H_
#define OSDL_BEZIER_H_


#include "OSDLPixel.h"     // for Pixels::Pixel
#include "OSDLPolygon.h"   // for listPoint2D




namespace OSDL
{

	
	
	namespace Video
	{



		// Bezier curves are drawn in Surfaces.
		class Surface ;
				
		
		
		
		namespace TwoDimensional
		{
					
			
			
			/**
			 * Draws a Bezier curve through specified control points, with
			 * specified number of steps, with specified RGBA color, on
			 * specified surface.
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param controlPoints a list of points controlling the shape 
			 * of the curve, which will start at the location of the first
			 * point, end at the location of the last point, but will not
			 * necessarily go through intermediate points: they will only
			 * "attract" the curve. 
			 * There must be at least three control points, i.e. at least
			 * one intermediate point.
			 *
			 * @param numberOfSteps determines how many steps will be 
			 * computed to interpolate the curve. 
			 * There must be at least two steps.
			 *
			 * @param red the red color coordinate of curve color.
			 *
			 * @param green the green color coordinate of curve color.
			 *
			 * @param blue the blue color coordinate of curve color.
			 *
			 * @param alpha the alpha color coordinate of curve color.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawBezierCurve( 
				Surface & targetSurface,
				const listPoint2D & controlPoints,
				Ceylan::Uint16 numberOfSteps,
				Pixels::ColorElement red, 
				Pixels::ColorElement green, 
				Pixels::ColorElement blue, 
				Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
	
	
	
			/**
			 * Draws a Bezier curve through specified control points, 
			 * with specified number of steps, with specified color definition, 
			 * on specified surface.
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param controlPoints a list of points controlling the shape 
			 * of the curve, which will start at the location of the first
			 * point, end at the location of the last point, but will not
			 * necessarily go through intermediate points: they will only
			 * "attract" the curve. 
			 * There must be at least three control points, i.e. at least
			 * one intermediate point.
			 *
			 * @param numberOfSteps determines how many steps will be 
			 * computed to interpolate the curve. 
			 * There must be at least two steps.
			 *
			 * @param colorDef the color definition of curve color.
			 *
			 * @return false if and only if something went wrong, for 
			 * example if surface locking failed.
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @throw VideoException if the operation is not supported.
			 *
			 */
			OSDL_DLL bool drawBezierCurve( 
				Surface & targetSurface, 
				const listPoint2D & controlPoints,
				Ceylan::Uint16 numberOfSteps, 
				Pixels::ColorDefinition colorDef ) ;

				
		}
		
	}
	
}



#endif // OSDL_BEZIER_H_

