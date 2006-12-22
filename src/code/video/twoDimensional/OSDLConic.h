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
		 	 * Conics section includes support for :
			 * - circle
			 * - ellipse
			 *
			 */


			// Methods could be inlined in a dedicated file for faster processing.
	
	
			/**
			 * Draws a circle whose center is (xCenter,yCenter) of specified radius, with 
			 * specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the circle should be filled (disc).
			 *
			 * @param blended tells whether, for each pixel of disc, the specified color should be
			 * blended with the one of the target pixel (if true), or if the specified color should
			 * replace the former one, regardless of any blending (if false). Note that only discs
			 * may be drawn without being blended : circles are always blended, therefore if filled
			 * is false, blended does not matter.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note A circle will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			bool drawCircle( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true, bool blended = true ) throw() ;
	
	
			/**
			 * Draws a circle whose center is (xCenter,yCenter) of specified radius, with 
			 * specified RGBA color, on specified surface.
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param filled tells whether the circle should be filled (disc).
			 *
			 * @param blended tells whether, for each pixel of disc, the specified color should be
			 * blended with the one of the target pixel (if true), or if the specified color should
			 * replace the former one, regardless of any blending (if false). Note that only discs
			 * may be drawn without being blended : circles are always blended, therefore if filled
			 * is false, blended does not matter.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note A circle will be antialiased if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			bool drawCircle( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, Length radius, 
				Pixels::ColorDefinition colorDef, bool filled = true, bool blended = true ) 
					throw() ;
	
		
			/**
			 * Draws a disc whose center is (xCenter,yCenter), of specified radius
			 * (totalRadius), filled with specified color discColorDef, with a ring in it, on
			 * its border, i.e. starting from innerRadius to totalRadius, colored with 
			 * ringColorDef, on specified surface. 
			 *
			 * @param targetSurface the surface where to draw.
			 *
			 * @param xCenter the abscissa of the center of the disc.
			 *
			 * @param yCenter the ordinate of the center of the disc.
			 *
			 * @param outerRadius the overall disc radius, including the border ring.
			 *
			 * @param innerRadius, the radius from which the ring is drawn, until totalRadius is
			 * reached.
			 *
			 * @param ringColorDef the ring color, the color of the border of this disk.
			 *
			 * @param discColorDef the inner color of the disc.
			 *
			 * @param blended tells whether, for each pixel of the edged disc, the specified
			 * color should be blended with the one of the target pixel (if true), or if the
			 * specified color should replace the former one, regardless of any blending (if
			 * false).
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note This method is rather expensive (slow), especially if 'discColorDef' is
			 * not fully opaque and if 'blended' is false.
			 *
			 * @note Locks surface if needed, cipping is performed, disc will be antialiased 
			 * if antialias mode is set.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			bool drawDiscWithEdge( Surface & targetSurface,
				Coordinate xCenter, Coordinate yCenter, 
				Length outerRadius, Length innerRadius, 
				Pixels::ColorDefinition ringColorDef = Pixels::Blue, 
				Pixels::ColorDefinition discColorDef = Pixels::White, bool blended = true )
					throw() ;
		
		
			/**
			 * Draws an ellipse whose center is (xCenter,yCenter), with specified horizontal and 
			 * vertical radius, with specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the ellipse should be filled.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note An unfilled ellipse will be antialiased if antialias mode is set.
			 *
			 * @note Ellipse's axis will be orthogonal or parallel to screen axis.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			bool drawEllipse( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter,
				Length horizontalRadius, Length verticalRadius,				
				Pixels::ColorElement red, Pixels::ColorElement green, 
				Pixels::ColorElement blue, Pixels::ColorElement alpha = Pixels::AlphaOpaque,
				bool filled = true ) throw() ;


			/**
			 * Draws an ellipse whose center is (xCenter,yCenter), with specified horizontal and 
			 * vertical radius, with specified RGBA color, on specified surface.
			 *
			 * @param filled tells whether the ellipse should be filled.
			 *
			 * @return false if and only if something went wrong (ex : surface lock failed).
			 * 
			 * @note Locks surface if needed.
			 *
			 * @note Clipping is performed.
			 *
			 * @note An unfilled ellipse will be antialiased if antialias mode is set.
			 *
			 * @note Ellipse's axis will be orthogonal or parallel to screen axis.
			 *
			 * @see VideoModule::GetAntiAliasingState
			 *
			 */
			bool drawEllipse( Surface & targetSurface, 
				Coordinate xCenter, Coordinate yCenter, 
				Length horizontalRadius, Length verticalRadius,
				Pixels::ColorDefinition colorDef, bool filled = true ) throw() ;

				
		}
		
	}
	
}


#endif // OSDL_CONIC_H_
