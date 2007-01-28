#ifndef OSDL_UPRIGHT_RECTANGLE_GL_H_
#define OSDL_UPRIGHT_RECTANGLE_GL_H_



#include "OSDLOpenGL.h"        // for GLLength, GLCoordinate, etc.
#include "OSDLPixel.h"         // for ColorElement

#include "Ceylan.h"            // for Bipoint


#include <iostream>           // for ostream
#include <string>



namespace Ceylan 
{

	namespace Maths 
	{
	
		namespace Linear 
		{

			// UprightRectangleGL can be defined thanks to Bipoint instances.
			class Bipoint ;
		
		}
	
	}
	
}		
		


namespace OSDL 
{

	namespace Video 
	{
		
		
		
		// UprightRectangleGL instances can be drawn into surfaces.
		class Surface ;
		
		
		namespace TwoDimensional
		{
		
		
			
			/**
			 * This classes defines a rectangle whose sides are parallel 
			 * or orthogonal to the screen's sides, according to OpenGL
			 * conventions.
			 *
			 * The rectangle lies in the plane z=0 and has sides parallel 
			 * to the x and y axes.
			 *
			 * @note As long as no different platform is used, it happens 
			 * that Ceylan::Maths::Real, which itself is a Ceylan::Float32, 
			 * i.e. a float, is the same type as GLfloat.
			 * Therefore Ceylan::Maths::Linear::Bipoint instances, which 
			 * rely on Real, can be used directly.
			 * If this relation had to change, then C++ template should be
			 * used instead.
			 *
			 */
			class UprightRectangleGL : public Ceylan::TextDisplayable
			{
			
					
				/*
				 * No equality operator defined, since relying on 
				 * floating point values.
				 *
				 */
						
						
				public:


					/// Two corners define an UprightRectangleGL.
					UprightRectangleGL( 
						const Ceylan::Maths::Linear::Bipoint & upperLeftCorner,
						const Ceylan::Maths::Linear::Bipoint & lowerRightCorner
					) throw( VideoException ) ;
					
					
					/**
					 * One corner, width and height define an
					 * UprightRectangleGL.
					 *
					 */
					UprightRectangleGL( 
						const Ceylan::Maths::Linear::Bipoint & upperLeftCorner, 
						OpenGL::GLLength width, OpenGL::GLLength height )
							throw() ;
					
					
					/**
					 * Two coordinates, width and height define an
					 * UprightRectangleGL.
					 *
					 */
					UprightRectangleGL( 
						OpenGL::GLCoordinate x, OpenGL::GLCoordinate y,
						OpenGL::GLLength width, OpenGL::GLLength height )
							throw() ;
					 
					 
					/// Basic destructor, so that it remains virtual.
					virtual ~UprightRectangleGL() throw() ;
	
	
					/// Returns this UprightRectangleGL's upper left corner.
					virtual Ceylan::Maths::Linear::Bipoint 
						getUpperLeftCorner() const throw() ; 
					
					
					/**
					 * Sets this UprightRectangleGL's upper left corner, 
					 * width and height do not change.
					 *
					 */
					virtual void setUpperLeftCorner( 
						Ceylan::Maths::Linear::Bipoint & newUpperLeftCorner )
							throw() ; 
					
					
					
					/// Returns directly the abscissa of upper left corner.
					virtual OpenGL::GLCoordinate getUpperLeftAbscissa() 
						const throw() ;


					/// Returns directly the ordiante of upper left corner.
					virtual OpenGL::GLCoordinate getUpperLeftOrdinate() 
						const throw() ;
					
					
					
					/// Returns this UprightRectangleGL's width.
					virtual OpenGL::GLLength getWidth() const throw() ;
				
				
					/// Sets this UprightRectangleGL's width.
					virtual void setWidth( OpenGL::GLLength newWidth ) throw() ;
	
	
	
					/// Returns this UprightRectangleGL's height.
					virtual OpenGL::GLLength getHeight() const throw() ;
				
				
					/// Sets this UprightRectangleGL's height.
					virtual void setHeight( OpenGL::GLLength newHeight ) 
						throw() ;
						
	
					/**
					 * Draws directly this UprightRectangleGL thanks to OpenGL.
					 *
					 */
					virtual bool draw() const throw() ;
					
					
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
					OpenGL::GLCoordinate _x ;
				
				
					/// Upper left corner's ordinate (vertical coordinate).
					OpenGL::GLCoordinate _y ;
				
				
					/// Width is counted from current abscissa, going right.
					OpenGL::GLLength _width ;
				
				
					/// Height is count from current ordinate, going down.
					OpenGL::GLLength _height ;			
			
			
			
				private:
				
				
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit UprightRectangleGL( 
						const UprightRectangleGL & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					UprightRectangleGL & operator = ( 
						const UprightRectangleGL & source ) throw() ;
	
	
			} ;	

		}
		
	}
	
}

	

/// To output its state in an output stream.
std::ostream & operator << ( std::ostream & os, 
	OSDL::Video::TwoDimensional::UprightRectangleGL & rect ) throw() ;



#endif // OSDL_UPRIGHT_RECTANGLE_GL_H_
