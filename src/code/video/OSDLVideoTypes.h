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


#ifndef OSDL_VIDEO_TYPES_H_
#define OSDL_VIDEO_TYPES_H_


#include "OSDLException.h"   // for OSDL::Exception 

#include "Ceylan.h"          // for the various basic types




/**
 * Definition of the main OSDL video types.
 *
 * As much as possible, consider using strong typing.
 *
 * @note These units are not chosen so that they are completely accurate,
 * as some physic computations would require. 
 * Their purpose is only to efficiently fulfill most multimedia needs.
 *
 */
namespace OSDL
{



	namespace Video
	{
	
	
	
		/// Length, unit of measure, unsigned.
		typedef Ceylan::Uint16 Length ;
	
	
		/// Length, unit of measure, signed.
		typedef Ceylan::Sint16 SignedLength ;
	
	
		/// Pixel pitch, unit of measure.
		typedef Ceylan::Uint16 Pitch ;



		/**
		 * Coordinate, unit of measure, integer, signed.
		 *
		 * @note Not having 'Coordinate' set to 'Ceylan::Sint16' would break
		 * functions using SDL_gfx (example: vertex array elements are 
		 * expected to store 'Ceylan::Sint16').
		 *
		 */
		typedef Ceylan::Sint16 Coordinate ;
		
		
		
		/// Offset, difference between coordinates.
		typedef Coordinate Offset ;



		/**
		 * Coordinate unit of measure, floating-point.
		 *
		 * @note Useful with OpenGL.
		 *
		 */
		typedef Ceylan::Float32 FloatingPointCoordinate ;



		/// Color depth, bits per pixel.
		typedef Ceylan::Uint8 BitsPerPixel ;
	
	
		/// Color depth, bytes per pixel.
		typedef Ceylan::Uint8 BytesPerPixel ;
		
		
		
		// Angle units and ratio already available thanks to Ceylan.
		


		/// Mother class for all video exceptions. 		
		class OSDL_DLL VideoException: public OSDL::Exception 
		{ 
		
			public:
				
				explicit VideoException( const std::string & reason ) : 
					OSDL::Exception( reason )
				{

				}


				virtual ~VideoException() throw()
				{
				
				}
				
		} ;	
		
			
	
	}
	
}



#endif // OSDL_VIDEO_TYPES_H_

