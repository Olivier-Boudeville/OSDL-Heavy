#ifndef OSDL_POINT_2D_H_
#define OSDL_POINT_2D_H_


#include "OSDLVideoTypes.h"    // for Coordinate
#include "OSDLPoint.h"         // for inheritance

#include "OSDLTypes.h"

#include "Ceylan.h"            // for MatrixIndex, TextDisplayable

#include <string>
#include <list>



namespace OSDL
{

	
	namespace Video
	{
	
	
		namespace TwoDimensional
		{
		

		
			/**
		     * Represents a point whose coordinates are integers, in a two-dimensional space.
			 *
			 * @note These integer points are especially useful to handle graphical user interface
			 * elements, such as widgets.
			 *
			 */
	    	class Point2D : public Point
	   		{

	        	public:
	
		
		
					/// Constructor of a two dimensional point from two coordinates.
	            	explicit Point2D( Coordinate x = 0, Coordinate y = 0 ) throw() ;


					/**
					 * Copy constructor.
					 *
					 * @note Cannot use the 'explicit' qualifier since it would prohibit expressions
					 * such as : 'return Point2D( _x, _y ) ;'
					 *
					 */
					Point2D( const Point2D & source ) throw() ;
					
					
					/**
					 * Creates a two dimensional point from a bipoint (conversion operator)
					 *
					 * @note Floating-point coordinates are rounded to nearest integers.
					 *
					 */
					explicit Point2D( const Ceylan::Maths::Linear::Bipoint & source ) throw() ;
					
					
					/**
					 * Creates a two dimensional point from a 2D vector (conversion operator)
					 *
					 * @note Floating-point coordinates are rounded to nearest integers.
					 *
					 */
					explicit Point2D( const Ceylan::Maths::Linear::Vector2 & source ) throw() ;
					
					 
					 
					/**
					 * Factory to create of a two dimensional point from two floating-point
					 * coordinates.
					 *
					 * Floating-point coordinates are rounded to nearest integer coordinates.
					 *
					 * @param x the point abscissa.
					 *
					 * @param y the point ordinate.
					 *
					 * This is not a constructor since otherwise it would collide with basic 
					 * Coordinate-based constructor and force the user to cast integer values to
					 * Coordinates.
					 *
					 * @return a newly allocated Point2D instance that the caller will have to 
					 * deallocate eventually. 
					 *
					 */
	            	static Point2D & CreateFrom( FloatingPointCoordinate x = 0, 
						FloatingPointCoordinate y = 0 )	throw() ;


					/// Virtual destructor of a two dimensional point.
	            	virtual ~Point2D() throw() ;
	
	
					/**
					 * Sets the coordinates of this point to specified coordinates.
					 *
					 */
					virtual void setTo( Coordinate x, Coordinate y ) throw() ;
					
					
					/**
					 * Sets the coordinates of this point to specified floating-point coordinates.
					 *
					 * @note Floating-point coordinates are rounded to nearest integer coordinates.
					 *
					 */
					virtual void setTo( FloatingPointCoordinate x, 
						FloatingPointCoordinate y ) throw() ;


					/**
					 * Sets this point coordinates from specified point, whose coordinates are
					 * copied.
					 *
					 */
					virtual void setFrom( const Point2D & source ) throw() ;
					

					/**
					 * Sets this point coordinates from specified 2D vector, whose coordinates are
					 * copied.
					 *
					 * @note Floating-point coordinates are rounded to nearest integers.
					 *
					 */
					virtual void setFrom( const Ceylan::Maths::Linear::Vector2 & source ) throw() ;

					
					/// Returns the first coordinate (abscissa) of the point.
					inline Coordinate getX() const throw() ;
					
					/// Sets the first coordinate (abscissa) of the point.
					inline void setX( Coordinate newX ) throw() ;
	
	
					/// Returns the second coordinate (ordinate) of the point.	
					inline Coordinate getY() const throw();
					
					/// Sets the second coordinate (ordinate) of the point.
					inline void setY( Coordinate newY ) throw() ;


					/**
					 * Translates this point of vector [x,y] : adds x to this point's abscissa,
					 * and adds y to its ordinate.
					 *
					 */
					void translate( Offset x, Offset y ) throw() ;
					
					
					/// Flips the coordinates of the point.
					virtual void flip() throw() ;
					
					
					/// Flips the abscissa of this point.
					virtual void flipX() throw() ;
					
					/// Flips the oridinate of this point.
					virtual void flipY() throw() ;
					
					
	 	            /**
		             * Returns an user-friendly description of the state of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const throw() ;
						


					// Static section.
					
					
					/// The origin of a 2D referential.
					static const Point2D Origin ;
					
					
					/**
					 * Translates all the points in the list of vector [x,y] : adds x to each
					 * point's abscissa, and adds y to their ordinates.
					 *
					 */
					static void Translate( std::list<Point2D *> & pointList, Offset x, Offset y )
						throw() ;
					

					/**
					 * Tells what is the dimension of the space this point is defined in.
					 *
					 */
					static const Ceylan::Maths::Linear::MatrixIndex Dimensions = 2 ;
					
					
					
	       		protected:
	
	
					/// Coordinates of the 2D point in space (abscissa, ordinate).
	            	Coordinate _coord[ Dimensions ] ;
					

	    	} ;
	
	
	
	
			// Inline section.
			
	
			Coordinate Point2D::getX() const throw()
			{
				return _coord[0] ;
			}
	
			void Point2D::setX( Coordinate newX ) throw()
			{
				_coord[0] = newX ;
			}
	
	
			Coordinate Point2D::getY() const throw()
			{
				return _coord[1] ;
			}
	
			void Point2D::setY( Coordinate newY ) throw()
			{
				_coord[1] = newY ;
			}
			
		}

	}
	
}


/// Stream operator for serialization.
std::ostream & operator << ( std::ostream & os, const OSDL::Video::TwoDimensional::Point2D & p ) ;


#endif // OSDL_POINT_2D_H_
