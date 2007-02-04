#ifndef OSDL_CIRCLE_BOUNDING_BOX_H_
#define OSDL_CIRCLE_BOUNDING_BOX_H_


#include "OSDLBoundingBox2D.h"   // for inheritance

#include "Ceylan.h"              // for VerbosityLevels, Bipoint


#include <string>
#include <list>



namespace OSDL
{


	namespace Engine 
	{
	
	
			
		/**
		 * Circular 2D bounding box.
		 *
		 */
		class OSDL_DLL CircleBoundingBox : public BoundingBox2D
		{
		
		
			public:
			
			
				/**
				 * Constructor of a 2D circular bounding box object.
				 *
				 * @param center the center of the bounding box.
				 *
				 * @param radius the radius of the circle.
				 *
				 */
				CircleBoundingBox( Ceylan::Locatable2D & father, 
					const Ceylan::Maths::Linear::Bipoint & center, 
					Ceylan::Maths::Real radius ) throw() ;
				
								
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~CircleBoundingBox() throw() ;


				/**
				 * Returns the radius of this circular bounding box.
				 *
				 */
				virtual Ceylan::Maths::Real getRadius() const throw() ;
				
				
				/**
				 * Sets the radius of this circular bounding box.
				 *
				 * @param newRadius the new radius.
				 *
				 */
				virtual void setRadius( Ceylan::Maths::Real newRadius ) 
					throw() ;
		
				 
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
				

				/**
				 * Tells what is the nature of the intersection between 
				 * this 2D bounding box and the specified one.
				 *
				 * Externally tangent circles (unique intersection) are 
				 * not deemed intersecting.
				 *
				 * Similarly, internally tangent circles (unique intersection)
				 * are deemed intersecting (no one is deemed stricly 
				 * contained by the second).
				 *
				 * @param other the other 2D bounding box.
				 * 
				 * @return the intersection result
				 *
				 * @throw BoundingBoxException if the type of the two 
				 * bounding boxes is not compatible (ex : 2D boxes cannot 
				 * be checked against 3D boxes), or not implemented.
				 *
				 * @see IntersectionResult
				 *
				 * @note Parameter cannot be 'const' since a referential 
				 * change may have to happen to compare the boxes.
				 *
				 */
				virtual IntersectionResult doesIntersectWith( 
					BoundingBox & other ) throw( BoundingBoxException ) ;
				
				
				
			protected:
			

				/**
				 * Helper method comparing two circular bounding boxes.
				 *
				 * @param other the second bounding box to intersect with.
				 *
				 * @note Parameter cannot be 'const' since a referential 
				 * change may have to happen to compare the boxes. 
				 * For the same reason, the method itself cannot be 'const'.
				 *
				 */
				virtual IntersectionResult compareWith( 
					CircleBoundingBox & other ) throw() ;

				
				/**
				 * Checks that specified bounding box is a circle
				 * (two-dimensional) one.
				 *
				 * @param box the bounding box to be checked.
				 *
				 * @throw BoundingBoxException if it is not a 2D circular
				 * bounding box.
				 *
				 * @return the successfully casted 2D circular box.
				 *
				 * @note Parameter should not be 'const' in order that 
				 * method to be useful for its caller.
				 *
				 */
				static CircleBoundingBox & CheckIsCircle( BoundingBox & box )
					throw (BoundingBoxException) ;			
					
								
				/// Radius of the bounding box.
				Ceylan::Maths::Real _radius ;
				
				
				
			private:		
		
				
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
	        	explicit CircleBoundingBox( const CircleBoundingBox & source )
					throw() ;


				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
	        	CircleBoundingBox & operator= ( 
					const CircleBoundingBox & source ) throw() ;
				
				
		} ;

	}

}


#endif // OSDL_CIRCLE_BOUNDING_BOX_H_
