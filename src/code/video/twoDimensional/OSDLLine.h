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



#ifndef OSDL_LINE_H_
#define OSDL_LINE_H_


#include "OSDLTypes.h"   // for Coordinate, etc.
#include "OSDLPixel.h"   // for Pixels::put, Pixels::Pixel




namespace OSDL
{

	
	
	namespace Video
	{



		// Lines are drawn into Surfaces.
		class Surface ;
				
		
		
		namespace TwoDimensional
		{
		
		
		
			class Point2D ;
			
		
		
			/**
		 	 * This namespace gathers all line-oriented graphics output.
			 *
			 * Colors expressed as raw (non-converted to surface's pixel format)
			 * 32-bit color must respect the 0xRRGGBBAA structure, i.e. 
			 * one byte for each of the four color components.
			 *
			 * @note Line clipping is elementary: a line will not be drawn
			 * unless both endpoints are in clipped area. 
			 * However, a line might be partially visible even if neither of
			 * them are in clipping area, therefore an improvement would be 
			 * to draw the onscreen remaining part.
			 *
			 */
			namespace Line
			{
	
	
				/*
				 * Methods could be inlined in a dedicated file for faster
				 * processing.
				 *
				 */
	
	
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), which is included if and 
				 * only if endpoint drawing mode is set, with specified RGBA
				 * color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState
				 *
				 */
				OSDL_DLL bool drawHorizontal( Surface & targetSurface, 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
	
	
	
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), which is included if and 
				 * only if endpoint drawing mode is set, with specified RGBA
				 * color, already expressed according to <b>targetSurface</b>
				 * pixel format.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState
				 *
				 */
				OSDL_DLL bool drawHorizontal( Surface & targetSurface, 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::PixelColor actualColor ) ;
	
	
	
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), which is included if and 
				 * only if endpoint drawing mode is set, with specified RGBA
				 * color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState
				 *
				 */
				OSDL_DLL bool drawHorizontal( Surface & targetSurface, 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::ColorDefinition = Pixels::White ) ;
	
	
	
				/**
				 * Draws a vertical line ranging from point (x;yStart),
				 * included, to point (x;yStop), which is included if and 
				 * only if endpoint drawing mode is set, with specified 
				 * RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, draw
				 *
				 * @see VideoModule::GetEndPointDrawState
				 *
				 */
				OSDL_DLL bool drawVertical( Surface & targetSurface, 
					Coordinate x, Coordinate yStart, Coordinate yStop, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
	
	
	
				/**
				 * Draws a vertical line ranging from point (x;yStart),
				 * included, to point (x;yStop), which is included if and 
				 * only if endpoint drawing mode is set, with specified 
				 * RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, draw
				 *
				 * @see VideoModule::GetEndPointDrawState
				 *
				 */
				OSDL_DLL bool drawVertical( Surface & targetSurface, 
					Coordinate x, Coordinate yStart, Coordinate yStop, 
					Pixels::ColorDefinition = Pixels::White ) ;



				/*
				 * There is no drawVertical with PixelColor, as the SDL_gfx 
				 * back-end does not provide it.
				 *
				 */
				
				
				
				/**
				 * Draws a line ranging from point (xStart;yStart), included,
				 * to point (xStop;yStop), which is included if and only 
				 * if endpoint drawing mode is set, with specified RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				OSDL_DLL bool draw( Surface & targetSurface, 
					Coordinate xStart, Coordinate yStart, 
					Coordinate xStop, Coordinate yStop, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
	
	
	
				/**
				 * Draws a line ranging from point (xStart;yStart), included,
				 * to point (xStop;yStop), which is included if and only 
				 * if endpoint drawing mode is set, with specified RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				OSDL_DLL bool draw( Surface & targetSurface, 
					Coordinate xStart, Coordinate yStart, 
					Coordinate xStop, Coordinate yStop, 
					Pixels::ColorDefinition colorDef = Pixels::White ) ;
					
					
					
				/**
				 * Draws a line ranging from firstpoint, included, to
				 * secondPoint, which is included if and only if endpoint
				 * drawing mode is set, with specified RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */					
				OSDL_DLL bool draw( Surface & targetSurface, 
					Point2D & firstPoint, Point2D & secondPoint,
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;
					
					
					
				/**
				 * Draws a line ranging from firstpoint, included, to
				 * secondPoint, which is included if and only if endpoint
				 * drawing mode is set, with specified RGBA color.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */					
				OSDL_DLL bool draw( Surface & targetSurface, 
					Point2D & firstPoint, Point2D & secondPoint,
					Pixels::ColorDefinition colorDef = Pixels::White ) ;
				
				
				
				/**
				 * Draws a cross centered in <b>center</b>, included in a 
				 * square whose length of edge is <b>squareEdge</b>.
				 *
				 * @note Useful to mark a specified pixel.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This cross will be antialiased if antialias mode 
				 * is set.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 */
				OSDL_DLL bool drawCross( Surface & targetSurface, 
					const Point2D & center,
					Pixels::ColorDefinition colorDef = Pixels::White, 
					Length squareEdge = 5 ) ;
					
					
					
				/**
				 * Draws a cross centered in <b>center</b>, included in a 
				 * square whose length of edge is <b>squareEdge</b>.
				 *
				 * @note Useful to mark a specified pixel.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This cross will be antialiased if antialias mode 
				 * is set.
				 *
				 * @return false if and only if something went wrong, for
				 * example if surface locking failed.
				 *
				 */
				OSDL_DLL bool drawCross( Surface & targetSurface, 
					Coordinate xCenter, Coordinate yCenter,
					Pixels::ColorDefinition colorDef = Pixels::White, 
					Length squareEdge = 5 ) ;
					
				
				
			}	
				
		}
		
	}
	
}



#endif // OSDL_LINE_H_

