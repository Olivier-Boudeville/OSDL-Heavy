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


#ifndef OSDL_UPRIGHT_RECTANGLE_H_
#define OSDL_UPRIGHT_RECTANGLE_H_


#include "OSDLVideoTypes.h"    // for Length, Coordinate, etc.
#include "OSDLPixel.h"         // for Pixels::ColorElement

#include "Ceylan.h"            // for Point2D

#include <iostream>            // for ostream
#include <string>


#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_Rect ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 


namespace OSDL 
{

	namespace Video 
	{
		
		
		namespace TwoDimensional
		{
		
		
			// Forward declaration to be able to define the operator ==.
			class UprightRectangle ;
			
			
			#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL
			
			// The internal actual rectangle object.
			typedef ::SDL_Rect LowLevelRect ;
				
			#else // OSDL_USES_SDL

			struct LowLevelRect {} ;
				
			#endif // OSDL_USES_SDL
			
		}
				
	}
	
}


// Operators should not be defined in a namespace.

/// Equality operator defined.
OSDL_DLL bool operator == ( 
	const OSDL::Video::TwoDimensional::UprightRectangle & first, 
	const OSDL::Video::TwoDimensional::UprightRectangle & second ) throw() ;


/**
 * Unequality operator defined (as if by default it could not be 
 * inferred from operator '==' !).
 *
 */
OSDL_DLL bool operator != ( 
	const OSDL::Video::TwoDimensional::UprightRectangle & first, 
	const OSDL::Video::TwoDimensional::UprightRectangle & second ) throw() ;




namespace OSDL 
{

	namespace Video 
	{
		
		
		// UprightRectangle instances can be drawn into surfaces.
		class Surface ;
		
		
		namespace TwoDimensional
		{
		
		
			// UprightRectangle instances are defined thanks to 2D points.
			class Point2D ;
		
	 
			
			/**
			 * This classes defines a rectangle whose sides are parallel 
			 * or orthogonal to the screen's sides.
			 *
			 * It is the user responsability to provide a set of 
			 * coordinates so that the upper left corner is indeed higher 
			 * than the lower right corner, and at its left, in usual
			 * OSDL 2D referential.
			 *
			 * Failure to do so results in an exception to be raised.
			 *
			 * @note When width or height for an upright rectangle is to 
			 * be retrieved, even in its own implementation or in the one 
			 * of its children, getWidth() and getHeight() should always
			 * be used, instead of _width and _height, since one child 
			 * class may overload these two get methods (see Surface for 
			 * an example).
			 *
			 */
			class OSDL_DLL UprightRectangle: public Ceylan::TextDisplayable
			{
					
									
				friend bool::operator == ( const UprightRectangle & first, 
					const UprightRectangle & second ) throw() ;
						
						
				public:



					/// Two corners define an UprightRectangle.
					UprightRectangle( const Point2D & upperLeftCorner,
							const Point2D & lowerRightCorner ) 
						throw( VideoException ) ;
					
					
					/**
					 * One corner, width and height define an 
					 * UprightRectangle.
					 *
					 */
					UprightRectangle( const Point2D & upperLeftCorner, 
						Length width, Length height ) throw() ;
					
					
					/**
					 * Two coordinates, width and height define an
					 * UprightRectangle.
					 *
					 */
					UprightRectangle( Coordinate x, Coordinate y, Length width,
						 Length height ) throw() ;
					 
					 
					/**
					 * A LowLevelRect defines an UprightRectangle.
					 *
					 * @throw VideoException if the operation is not supported.
					 *
					 */
					explicit UprightRectangle( const LowLevelRect & source ) 
						throw( VideoException ) ;
		
		
		
					/// Basic destructor, so that it remains virtual.
					virtual ~UprightRectangle() throw() ;
	
	
	
					
					// Section for upper-left corner.
					
	
					/// Returns this UprightRectangle's upper left corner.
					virtual Point2D getUpperLeftCorner() const throw() ; 
					
					
					/**
					 * Sets this UprightRectangle's upper left corner, 
					 * width and height do not change.
					 *
					 */
					virtual void setUpperLeftCorner( 
						Point2D & newUpperLeftCorner ) throw() ; 
					
					
					
					/// Returns directly the abscissa of upper left corner.
					virtual Coordinate getUpperLeftAbscissa() const throw() ;


					/**
					 * Sets directly the abscissa of the upper left 
					 * corner of this rectangle.
					 *
					 */
					virtual void setUpperLeftAbscissa( 
						Coordinate newAbscissa ) throw() ;


					
					/// Returns directly the ordinate of upper left corner.
					virtual Coordinate getUpperLeftOrdinate() const throw() ;
					
					
					/**
					 * Sets directly the ordinate of the upper left 
					 * corner of this rectangle.
					 *
					 */
					virtual void setUpperLeftOrdinate( 
						Coordinate newOrdinate ) throw() ;
					
					

					
					// Section for lower-right corner.
					
	
					/// Returns this UprightRectangle's lower right corner.
					virtual Point2D getLowerRightCorner() const throw() ; 
					
					
					/**
					 * Sets directly the lower right corner of this rectangle.
					 *
					 * @throw VideoException if width or height would 
					 * become negative.
					 *
					 */
					virtual void setLowerRightCorner( 
							Point2D & newLowerRightCorner ) 
						throw( VideoException) ; 
					
					
					
					/// Returns directly the abscissa of lower right corner.
					virtual Coordinate getLowerRightAbscissa() const throw() ;


					/**
					 * Sets directly the abscissa of the lower right 
					 * corner of this rectangle.
					 *
					 * @throw VideoException if width or height would 
					 * become negative.
					 *
					 */
					virtual void setLowerRightAbscissa( 
						Coordinate newAbscissa ) throw( VideoException ) ;


					
					/// Returns directly the ordinate of lower right corner.
					virtual Coordinate getLowerRightOrdinate() const throw() ;
					
					
					/**
					 * Sets directly the ordinate of the lower right 
					 * corner of this rectangle.
					 *
					 * @throw VideoException if width or height would 
					 * become negative.
					 *
					 */
					virtual void setLowerRightOrdinate( 
						Coordinate newOrdinate ) throw( VideoException ) ;
					

					
					
					/// Returns this UprightRectangle's width.
					virtual Length getWidth() const throw() ;
				
				
					/// Sets this UprightRectangle's width.
					virtual void setWidth( Length newWidth ) throw() ;
	
	
	
					/// Returns this UprightRectangle's height.
					virtual Length getHeight() const throw() ;
				
				
					/// Sets this UprightRectangle's height.
					virtual void setHeight( Length newHeight ) throw() ;
						
	
					/**
					 * Draws this UprightRectangle to specified surface 
					 * with specified RGBA color.
					 * 
					 * @param target the surface to which this rectangle 
					 * will be rendered.
					 *
					 * @param filled tells whether the drawn rectangle 
					 * should be filled with specified color, or if only 
					 * its sides should be drawn.
					 *
					 * @note Locks surface if needed.
					 *
					 */
					virtual bool draw( 
						Surface & target, 
						Pixels::ColorElement red, 
						Pixels::ColorElement blue, 
						Pixels::ColorElement green, 
						Pixels::ColorElement alpha = Pixels::AlphaOpaque,
						bool filled = true ) const throw() ;
					
	
	
					/**
					 * Draws this UprightRectangle to specified surface 
					 * with specified RGBA color.
					 * 
					 * @param target the surface to which this rectangle 
					 * will be rendered.
					 *
					 * @param filled tells whether the drawn rectangle 
					 * should be filled with specified color, or if only 
					 * its sides should be drawn.
					 *
					 * @note Locks surface if needed.
					 *
					 */
					virtual bool draw( Surface & target, 
							Pixels::ColorDefinition colorDef= Pixels::White,
							bool filled = true ) 
						const throw() ;
					
					
					
					/**
					 * Draws this UprightRectangle to specified surface 
					 * with rounded corners, drawn with specified color, 
					 * above a background of specified color. 
					 *
					 * The width of the borders can be chosen, and should 
					 * not be incompatible with the dimensions of this
					 * rectangle.
					 * 
					 * @param target the surface to which this rectangle 
					 * will be rendered.
					 *
					 * @param edgeWidth the width of the edges of this
					 * rectangle.
					 *
					 * @param edgeColorDef the color definition of the 
					 * edges of this rectangle.
					 *
					 * @param backgroundColorDef the color definition 
					 * of the background of this rectangle.
					 *
					 * @note Locks surface if needed.
					 *
					 */
					virtual bool drawWithRoundedCorners( 
							Surface & target, Length edgeWidth = 3, 
							Pixels::ColorDefinition edgeColorDef 
								= Pixels::White, 
							Pixels::ColorDefinition backgroundColorDef 
								= Pixels::Transparent ) 
						const throw() ;
					
					
					
					/**
					 * Computes the preferred radius of round corners for 
					 * this rectangle, with the specified edge width. 
					 *
					 * @param edgeWidth the edge width to take into 
					 * account for the radius.
					 *
					 * @return a suitable radius, whose dimension is 
					 * deemed appropriate.
					 *
					 * @throw VideoException if no radius can fullfill 
					 * the set of constraints due to this rectangle and 
					 * the edge width.
					 *
					 */
					virtual Length computeRadiusForRoundRectangle( 
							Length edgeWidth ) const
						throw( VideoException ) ;
					
					
					
					/**
					 * Returns the back-end counterpart of this
					 * UprightRectangle.
					 *
					 * It is up to the caller to release memory for this
					 * LowLevelRect.
					 *
					 * @example: LowLevelRect * myRect =
					 * aSurface.toLowLevelRect();
					 * ...
					 $ delete myRect ;
					 *
					 */
					virtual LowLevelRect * toLowLevelRect() const 
						throw( VideoException ) ;

				
				
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
				
				
					/// Upper left corner's abscissa (horizontal coordinate).
					Coordinate _x ;
				
				
					/// Upper left corner's ordinate (vertical coordinate).
					Coordinate _y ;
				
			
			
				private:
				
				
					/// Width is counted from current abscissa, going right.
					Length _width ;
				
				
					/// Height is count from current ordinate, going down.
					Length _height ;			
				
				
					/**
					 * Copy constructor made private to ensure that it 
					 * will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined constructor is called, implicitly or not.
					 * 
					 */			 
					explicit UprightRectangle( 
						const UprightRectangle & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that 
					 * it will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined operator is called, implicitly or not.
					 * 
					 */			 
					UprightRectangle & operator = ( 
						const UprightRectangle & source ) throw() ;
	
	
	
			} ;	

		}
		
	}
	
}




/// To output its state in an output stream.
std::ostream & operator << ( std::ostream & os, 
	OSDL::Video::TwoDimensional::UprightRectangle & rect ) throw() ;



#endif // OSDL_UPRIGHT_RECTANGLE_H_

