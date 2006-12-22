#ifndef OSDL_POINT_H_
#define OSDL_POINT_H_


#include "Ceylan.h"            // for MatrixIndex, TextDisplayable

#include <string>


namespace OSDL
{

	
	namespace Video
	{
	
	
		namespace TwoDimensional
		{
		
		
		
			/**
		     * Represents an abstract point whose coordinates are integers.
			 *
			 * @note These integer points are especially useful to handle graphical user interface
			 * elements, such as widgets.
			 *
			 */
	    	class Point: public Ceylan::TextDisplayable
	   		{

	        	public:
	
		
					/// Basic abstract constructor of a point .
	            	Point() throw() ;
										

					/// Virtual destructor of a point.
	            	virtual ~Point() throw() ;
	

					/// Flips the coordinates of this point (negates them all).
					virtual void flip() throw() = 0 ;


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
						

					/**
					 * Tells what is the dimension of the space this point is defined in.
					 *
					 */
					static const Ceylan::Maths::Linear::MatrixIndex Dimensions = 0 ;
					
	    	} ;
				
		}

	}
	
}


/// Stream operator for serialization.
std::ostream & operator << ( std::ostream & os, 
	const OSDL::Video::TwoDimensional::Point & p ) ;



#endif // OSDL_POINT_H_
