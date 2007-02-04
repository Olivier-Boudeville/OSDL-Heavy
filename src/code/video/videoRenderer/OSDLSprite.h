#ifndef OSDL_SPRITE_H_
#define OSDL_SPRITE_H_


#include "OSDLRenderer.h"            // for RenderingException


/*
#include "Ceylan.h"                   // for inheritance


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLException.h"            // for Exception

*/


#include <string>
#include <list>



namespace OSDL
{


	namespace Rendering 
	{
	


		/**
		 * A sprite is a specialized view corresponding to a 2D 
		 * bitmap-based graphical element.
		 *
		 */	
		class OSDL_DLL Sprite : public Ceylan::View
		{
		
		
			public:
			
			
				/**
				 * Constructs a new sprite.
				 *
				 * @param ownBoundingBox tells whether this sprite owns 
				 * its bounding box, i.e. generally whether this is a local
				 * execution scheme, as opposed to a client/server scheme.
				 *
				 */
				explicit Sprite( bool ownBoundingBox = true ) throw() ;
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~Sprite() throw() ;
			

	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
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


					
			protected:
			

				/**
				 * Tells whether this sprite owns the bounding box(es) it 
				 * refers to.
				 *
				 * On a local scheme, the bounding box is shared between 
				 * the view(s) and the model, since it may use both for
				 * rendering and collision detection.
				 *
				 * On a distributed context (typically, client/server), 
				 * multiple bounding boxes exist : one on each client, 
				 *shared between the views, and one on the server, for 
				 * the model.
				 *
				 */
				bool _ownBoundingBox ;
				
				
				/**
				 * Each sprite is linked with a bounding box, which 
				 * graphically contains it.
				 *
				 * @note For the moment, each sprite has only one (2D) 
				 * bounding box.
				 *
				 */
				BoundingBox2D _renderingDone ;
		
		
						
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				Sprite( const Sprite & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Sprite & operator = ( const Sprite & source ) throw() ;
				
		} ;

	}

}


#endif // OSDL_SPRITE_H_

