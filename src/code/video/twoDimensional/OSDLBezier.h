#ifndef OSDL_BEZIER_H_
#define OSDL_BEZIER_H_


#include "OSDLPixel.h"   // for Pixels::Pixel
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
			 * Draws a Bezier curve through specified control points, with specified number of
			 * steps, with specified RGBA color, on specified surface.
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param controlPoints a list of points controlling the shape of the curve, which will
			 * start at the location of the first point, end at the location of the last point, but
			 * will not necessarily go through intermediate points : they will only "attract" the
			 * curve. There must be at least three control points.
			 *
			 * @param numberOfSteps determines how many steps will be computed to interpolate the
			 * curve. There must be at least two steps.
			 *
			 * @param red the red color coordinate of curve color.
			 *
			 * @param green the green color coordinate of curve color.
			 *
			 * @param blue the blue color coordinate of curve color.
			 *
			 * @param alpha the alpha color coordinate of curve color.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 */
			bool drawBezierCurve( Surface & targetSurface, const listPoint2D & controlPoints,
				Ceylan::Uint16 numberOfSteps,
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, Pixels::ColorElement alpha = Pixels::AlphaOpaque )
					throw() ;
	
	
	
			/**
			 * Draws a Bezier curve through specified control points, with specified number of
			 * steps, with specified RGBA color, on specified surface.
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param controlPoints a list of points controlling the shape of the curve, which will
			 * start at the location of the first point, end at the location of the last point, but
			 * will not necessarily go through intermediate points : they will only "attract" the
			 * curve. There must be at least three control points.
			 *
			 * @param numberOfSteps determines how many steps will be computed to interpolate the
			 * curve. There must be at least two steps.
			 *
			 * @param colorDef the color definition of curve color.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 */
			bool drawBezierCurve( Surface & targetSurface, const listPoint2D & controlPoints,
				Ceylan::Uint16 numberOfSteps, Pixels::ColorDefinition colorDef ) throw() ;

				
		}
		
	}
	
}


#endif // OSDL_BEZIER_H_
