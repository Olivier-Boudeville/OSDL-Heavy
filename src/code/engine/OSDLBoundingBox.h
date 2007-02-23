#ifndef OSDL_BOUNDING_BOX_H_
#define OSDL_BOUNDING_BOX_H_


#include "OSDLEngineCommon.h"     // for EngineException

#include "Ceylan.h"               // for VerbosityLevels, inheritance

#include <string>
#include <list>




namespace OSDL
{


	namespace Engine 
	{
	
	
	
		/// Exception raised by bounding boxes.
		class OSDL_DLL BoundingBoxException : public EngineException
		{
			public:
		
				explicit BoundingBoxException( const std::string & reason )
					throw() ;
				virtual ~BoundingBoxException() throw() ;
			
		} ;
		
		
		
		/**
		 * Describes the result of an intersection check between an object 
		 * and another object.
		 *
		 * For example, when comparing two bounding boxes B1 and B2, five 
		 * cases can occur :
		 *   - B1 and B2 have no intersection.
		 *   - B1 contains strictly B2 (i.e. all parts of B2 are in B1, 
		 * but some part of B1 is not in B2)
		 *   - B1 is strictly contained by B2 (i.e. all parts of B1 are 
		 * in B2, but some part of B2 is not in B1)
		 *   - B1 and B2 intersects each other, but a part of B1 is not 
		 * in B2, and a part of B2 is not in B1
		 *   - B1 and B2 are equal
		 *
		 * @note The collision check is not symetric, the first object 
		 * is the reference, whereas the second embodies the outside, 
		 * being contained is not the same thing as containing.
		 *
		 */
		enum IntersectionResult { 
			isSeparate, 
			contains, 
			isContained, 
			intersects, 
			isEqual 
		} ;
		
		
			
		/**
		 * Bounding boxes are geometrical shapes that verify the following 
		 * properties :
		 *   - they contain all the object(s) they correspond to (all points 
		 * in the object or set of objects are in the bounding box)
		 *	 - geometrical computations (intersection, distance, inclusion 
		 * test, etc.) are faster and/or easier than if they were to be
		 * performed on the actual associated object.
		 * 
		 * Bounding boxes are therefore simple shapes helping managing 
		 * complex objects.
		 *
		 * Various bounding boxes can be used, depending on the dimension of
		 * the space they are to be used (ex : 2D, 3D) and the precision 
		 * they are to provide (ex : cubes demand more processing power than
		 * spheres, but for some computations they give more accurate
		 * results).
		 *
		 * A bounding box is linked to at least an object and must have a
		 * reference onto it, since boxes are defined relatively to them, 
		 * as son Locatable depending on his father.
		 * Reciproqually, for a given object its bounding box needs to be
		 * found. As objects have to know their bounding box(es), it must be a 
		 * two-way link.
		 *
		 * All bounding boxes should be Locatable instancess, but the type of
		 * the Locatable depends on the space they are defined in.
		 * Therefore abstract bounding boxes should not inherit from any 
		 * Locatable at this point.
		 *
		 * Bounding boxes are notably useful to check collisions between them. 
		 *
		 * @see IntersectionResult
		 *
		 */
		class OSDL_DLL BoundingBox : public Ceylan::TextDisplayable
		{
		
		
			public:
			
			
				/**
				 * Constructor of a bounding box object.
				 *
				 * @note Not even common parameters are managed here 
				 * (ex : all bounding-boxes should have a center), since it
				 * would have to be for example a Point whereas actually a
				 * Bipoint or a Tripoint would be used. 
				 * This would not be convenient since it would require 
				 * numerous dynamic casts to transform this data member
				 * into a specialized forms.
				 *
				 */
				explicit BoundingBox() throw() ;
				
								
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~BoundingBox() throw() ;


				/**
				 * Tells what is the nature of the intersection between 
				 * this bounding box and the specified one.
				 *
				 * @param other the other bounding box.
				 * 
				 * @return the intersection result
				 *
				 * @throw BoundingBoxException if the type of the two 
				 * bounding boxes is not compatible (ex : 2D boxes cannot 
				 * be checked against 3D boxes), or not implemented.
				 *
				 * @see IntersectionResult
				 *
				 * @note This method can be called for heterogenous (but
				 * compatible) bounding boxes types. 
				 * For example, if it was implemented, circular and square
				 * (both being 2D boxes) could be searched for intersection.
				 *
				 * @note Parameter cannot be 'const' since a referential 
				 * change may have to happen to compare the boxes.
				 *
				 */
				virtual IntersectionResult doesIntersectWith( 
					BoundingBox & other ) throw( BoundingBoxException ) = 0 ;
				
				
				
				// Static section.
				
				
				/**
				 * Returns a textual description of specified intersection
				 *  result.
				 *
				 * @throw BoundingBoxException if the intersection result 
				 * is not known, and therefore cannot be interpreted.
				 *
				 */
				static std::string InterpretIntersectionResult(
					IntersectionResult result ) throw( BoundingBoxException ) ;
					
					
					
			private:		
		
				
			
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
	        	explicit BoundingBox( const BoundingBox & source ) throw() ;


				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
	        	BoundingBox & operator= ( const BoundingBox & source ) throw() ;
				
				
				
		} ;

	}

}


#endif // OSDL_BOUNDING_BOX_H_

