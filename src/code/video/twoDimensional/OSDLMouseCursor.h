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


#ifndef OSDL_MOUSE_CURSOR_H_
#define OSDL_MOUSE_CURSOR_H_


#include "OSDLVideoTypes.h"  // for VideoException

#include "Ceylan.h"          // for inheritance


#include <string>



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_Cursor ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 



namespace OSDL
{

	namespace Video
	{

		
		#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 
	
		/*
		 * No way of forward declaring LowLevelCursor apparently:
		 * we would have liked to specify 'struct LowLevelThing ;' here and 
		 * in the implementation file (.cc): 'typedef BackendThing
		 * LowLevelThing' but then the compiler finds they are conflicting
		 * declarations.
		 *
		 */
		 
		/// The internal actual surface.
		typedef SDL_Cursor LowLevelCursor ;

		#else // OSDL_USES_SDL

		struct LowLevelCursor {} ;

		#endif // OSDL_USES_SDL

		
		// Cursor shape may be a surface.
		class Surface ;
		
		
		namespace OpenGL
		{
		
			// Cursor shape may be a surface.
			class GLTexture ;
		
		}
		
		
		namespace TwoDimensional
		{

	

			/**
			 * Exception raised whenever a mouse cursor operation failed.
			 *
			 */
			class OSDL_DLL MouseCursorException: public VideoException
			{
				public:
				
					MouseCursorException( const std::string & exception ) 
						throw() ;
						
					virtual ~MouseCursorException() throw() ;
					
			} ;




			/**
			 * Encapsulates a mouse cursor.
			 *
			 * There are two types of mouse cursors:
			 *   - system (hardware-based) ones, which are black and white only
			 *   - OSDL-powered ones, that are color cursors, and that are
			 * managed as specific surfaces/textured quads being rendered
			 * automatically, as soon as they are linked with a Mouse object.
			 *
			 * System cursor shapes are specified thanks to background data 
			 * and a mask, which both represent the same rectangle enclosing the
			 * cursor shape, with one bit per pixel, and a width which must 
			 * be a multiple of 8. 
			 * On a given coordinate, if the bit in the data is:
			 *   - 0, then if the bit in the mask is 0, the corresponding pixel
			 * will be transparent. If the bit in the mask is 1, the
			 * corresponding pixel is white
			 *   - 1, then if the bit in the mask is 0, the corresponding pixel
			 * will be the inverted cursor color, if possible, otherwise black.
			 * If the bit in the mask is 1, the corresponding pixel is black.
			 *
			 * OSDL-powered cursor are specified as a colorkeyed surface.
			 * Depending on OpenGL being used or not, they are rendered as
			 * textured quads or as blitted surfaces.
			 *
			 * All cursors must be associated with a hot-spot, which is the
			 * point of the cursor shape which will be aligned with the
			 * actual mouse pointer. 
			 * Hence the user will have to align this hot-spot with the target
			 * onscreen area to click it, and the cursor shape will be blitted
			 * with an offset so that the hotspot matches the actual mouse
			 * pointer.
			 *
			 */
			class OSDL_DLL MouseCursor: public Ceylan::TextDisplayable
			{
			
					
				public:
						
					
					/**
					 * Describes the different cursor types:
					 *    - SystemCursor: the cursor is black-and-white,
					 * and it is managed by the system directly,
					 *    - BlittedCursor: the cursor is defined by a surface,
					 * which is blitted automatically by OSDL whenever 
					 * associated with a mouse,
					 *    - TexturedCursor: the cursor is rendered
					 * automatically by OSDL through OpenGL as a textured
					 * quad, whenever associated with a mouse.
					 *
					 */
					enum CursorType 
					{
					
						SystemCursor,
						BlittedCursor,
						TexturedCursor
					
					} ;
						
							
					/**
					 * Creates a new system mouse cursor.
					 *
					 * @param width the width of the cursor-enclosing 
					 * rectangle. The width must be a multiple of 8.
					 *
					 * @param height the height of the cursor-enclosing 
					 * rectangle.
					 *
					 * @param data the bitplane describing the cursor shape. 
					 * Its size must be width * height bits. The cursor takes
					 * ownership of this data.
					 * 
					 * @param mask the bitplane describing the cursor mask,
					 * which must have the same size as the data bitplane.
					 * The cursor takes ownership of this mask.
					 *
					 * @param hotSpotAbscissa the abscissa of the hotspot, 
					 * which must lie in the cursor-enclosing rectangle
					 * ( 0 <= hotSpotAbscissa < width ).
					 * 
					 * @param hotSpotOrdinate the ordinate of the hotspot, 
					 * which must lie in the cursor-enclosing rectangle
					 * ( 0 <= hotSpotOrdinate < height ).
					 *
					 * @throw MouseCursorException if the operation failed.
					 *
					 */
					MouseCursor( 
							Length width,
							Length height,
							const Ceylan::Uint8 & data,
							const Ceylan::Uint8 & mask,
							Coordinate hotSpotAbscissa,
							Coordinate hotSpotOrdinate )
						throw( MouseCursorException ) ;
					
					
						
					/**
					 * Creates a new OSDL-powered mouse cursor.
					 *
					 * @param cursorSurface a surface containing the cursor,
					 * which should have a colorkey or an alpha channel for 
					 * transparent parts of the cursor. The cursor takes
					 * ownership of this surface.
					 *
					 * @param hotSpotAbscissa the abscissa of the hotspot, 
					 * which must lie in the cursor-enclosing rectangle
					 * ( 0 <= hotSpotAbscissa < surface width).
					 * 
					 * @param hotSpotOrdinate the ordinate of the hotspot, 
					 * which must lie in the cursor-enclosing rectangle
					 * ( 0 <= hotSpotOrdinate < surface height).
					 *
					 * @throw MouseCursorException if the operation failed.
					 *
					 */
					MouseCursor( const Surface & cursorSurface,
							Coordinate hotSpotAbscissa,
							Coordinate hotSpotOrdinate )
						throw( MouseCursorException ) ;
						
					
					/// Virtual destructor.
					virtual ~MouseCursor() throw() ;
					
					
					
					/**
					 * Returns the type of this mouse cursor.
					 *
					 * @see CursorType
					 *
					 */	
					CursorType getType() const throw() ;
					
					
	 	            /**
		             * Returns an user-friendly description of the state 
					 * of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
							Ceylan::VerbosityLevels level = Ceylan::high ) 
						const throw() ;
				
						
							
											
				protected:
						
						
					
					CursorType _type ;
					
					
					/**
					 * Back-end cursor, for SystemCursor type.
					 *
					 */
					LowLevelCursor * _systemCursor ;
					
										
					
					/// The width of the cursor, for SystemCursor only.
					Length _width ;
					
					/// The height of the cursor, for SystemCursor only.
					Length _height ;
					
					
					
					/**
					 * The cursor shape, for BlittedCursor type.
					 *
					 */
					Surface * _surface ;
					
					
					/**
					 * The cursor shape, for TexturedCursor type.
					 *
					 */
					OpenGL::GLTexture * _texture ;
					
					
					
					/// The abscissa of the cursor hot spot.
					Coordinate _hotSpotAbscissa ;
					
					/// The ordinate of the cursor hot spot.
					Coordinate _hotSpotOrdinate ;
					
					
					
				private:	


					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit MouseCursor( const MouseCursor & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					MouseCursor & operator = ( const MouseCursor & source )
						throw() ;
					
					
			} ;			
											
		}
	
	}

}				


#endif // OSDL_MOUSE_CURSOR_H_

