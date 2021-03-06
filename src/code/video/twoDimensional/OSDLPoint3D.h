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


#ifndef OSDL_POINT_3D_H_
#define OSDL_POINT_3D_H_


#include "OSDLPoint.h"        // for inheritance
#include "OSDLVideoTypes.h"   // for Coordinate

#include "Ceylan.h"           // for MatrixIndex, TextDisplayable

#include <string>
#include <list>




namespace OSDL
{


	
	namespace Video
	{
	
	
	
		namespace TwoDimensional
		{
		
		
		
		
			/**
		     * Represents a point whose coordinates are integers, in a
			 * three-dimensional space.
			 *
			 * This kind of points belongs to the 2D namespace, even if 
			 * they are 3D, since stacking or homogeneous matrices are 
			 * useful in a 2D context.
			 *
			 * These integer points are especially useful to handle 
			 * graphical user interface elements that can be stacked, 
			 * such as widgets.
			 *
			 */
	    	class OSDL_DLL Point3D : public Point
	   		{



	        	public:
	
		
					/**
					 * Constructor of a three dimensional point, from three
					 * coordinates.
					 *
					 */
	            	Point3D( Coordinate x, Coordinate y, Coordinate z ) ;



					/**
					 * Copy constructor.
					 *
					 * @note Cannot use the 'explicit' qualifier since it 
					 * would prohibit expressions such as:
					 * 'return Point3D( _coord[0], _coord[1], _coord[2] ) ;'
					 *
					 */
					Point3D( const Point3D & source ) ;
					
					
					
					/**
					 * Creates a two dimensional point from a 2D vector
					 * (conversion operator).
					 *
					 * @note Floating-point coordinates are rounded to
					 * nearest integers.
					 *
					 */
					explicit Point3D( 
						const Ceylan::Maths::Linear::Vector3 & source ) ;
					
					
					 
					/**
					 * Constructor of a three dimensional point from three
					 * floating-point coordinates.
					 *
					 * @note Floating-point coordinates are rounded to 
					 * nearest integers.
					 *
					 */
	            	Point3D( 
						FloatingPointCoordinate x, 
						FloatingPointCoordinate y,
						FloatingPointCoordinate z ) ;



					/// Virtual destructor of a three dimensional point.
	            	virtual ~Point3D() throw() ;
	
	
	
					/**
					 * Sets this point coordinates from specified point, 
					 * whose coordinates are copied.
					 *
					 */
					virtual void setFrom( const Point3D & source ) ;
					
					
		
					/**
					 * Sets this point coordinates from specified 3D 
					 * vector, whose coordinates are copied.
					 *
					 * @note Floating-point coordinates are rounded to
					 * nearest integers.
					 *
					 */
					virtual void setFrom( 
						const Ceylan::Maths::Linear::Vector3 & source ) ;
					
					
					
					/// Returns the first coordinate (abscissa) of the point.
					inline Coordinate getX() const ;
					
					
					/// Sets the first coordinate (abscissa) of the point.
					inline void setX( Coordinate newX ) ;
	
	
	
					/// Returns the second coordinate (ordinate) of the point.	
					inline Coordinate getY() const;
					
					
					/// Sets the second coordinate (ordinate) of the point.
					inline void setY( Coordinate newY ) ;



					/// Returns the third coordinate (depth) of the point.	
					inline Coordinate getZ() const;
					
					
					/// Sets the third coordinate (depth) of the point.
					inline void setZ( Coordinate newY ) ;



					/**
					 * Translates this point of vector [x,y,z]: adds x to 
					 * this point's abscissa, adds y to its ordinate, 
					 * and z to the depth coordinate.
					 *
					 */
					void translate( Offset x, Offset y, Offset z ) ;
					
					
					
					/// Flips the coordinates of the point.
					virtual void flip() ;
					
					
					
					/// Flips the abscissa of this point.
					void flipX() ;
					
					
					/// Flips the ordinate of this point.
					void flipY() ;
					
					
					/// Flips the depth coordinate of this point.
					void flipZ() ;
					
					
					
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
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
						




					// Static section.
	
	
	
					/// The origin of a 3D referential.
					static const Point3D Origin ;



					/**
					 * Translates all the points in the list of vector 
					 * [x,y,z]: adds x to each point's abscissa, adds y to
					 * their ordinates, and z to their depth coordinates.
					 *
					 */
					static void Translate( std::list<Point3D *> & pointList, 
						Offset x, Offset y, Offset z ) ;
					
					
					
					/**
					 * Tells what is the dimension of the space this 
					 * point is defined in.
					 *
					 */
					static const Ceylan::Maths::Linear::MatrixIndex Dimensions 
						= 3 ;




	       		protected:
	
	
	
					/**
					 * Coordinates of the 3D point in space 
					 * (abscissa, ordinate, depth coordinate).
					 *
					 */
	            	Coordinate _coord[ Dimensions ] ;
					
					

	    	} ;
	
	
	
	
			// Inline section.
			
			
			
			Coordinate Point3D::getX() const
			{
			
				return _coord[0] ;
				
			}
	
	
	
			void Point3D::setX( Coordinate newX )
			{
			
				_coord[0] = newX ;
				
			}
	
	
	
	
			Coordinate Point3D::getY() const
			{
			
				return _coord[1] ;
				
			}
	
	
	
			void Point3D::setY( Coordinate newY )
			{
				_coord[1] = newY ;
			}
			
			
			
			
			Coordinate Point3D::getZ() const
			{
			
				return _coord[2] ;
				
			}
			
			
	
			void Point3D::setZ( Coordinate newZ )
			{
			
				_coord[2] = newZ ;
				
			}
			
			
			
		}

	}
	
}



/// Stream operator for serialization.
OSDL_DLL std::ostream & operator << ( std::ostream & os, 
	const OSDL::Video::TwoDimensional::Point3D & p ) ;



#endif // OSDL_POINT_3D_H_

