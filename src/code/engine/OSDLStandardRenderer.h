#ifndef OSDL_STANDARD_RENDERER_H_
#define OSDL_STANDARD_RENDERER_H_


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLRenderer.h"             // for inheritance


#include <string>
#include <list>


namespace Ceylan
{		

	// A standard renderer handles views.
	class View ;		

}


namespace OSDL
{


	namespace Video 
	{
	
		/// A standard renderer needs to know the screen it updates.
		class Surface ;
		
	}
	

		
	namespace Rendering 
	{
	
				
				
		/**
		 * The standard renderer manages a list of views and offers a specific
		 * support for OpenGL.
		 *
		 * @see Renderer
		 *
		 */	
		class OSDL_DLL StandardRenderer : public Renderer
		{
		
		
		
			public:
			
			
				/**
				 * Constructs a new standard renderer.
				 *
				 * @param screen the screen that shall be updated by this
				 * renderer.
				 *
				 * @param registerToScheduler iff true, then a scheduler 
				 * is supposed to exist already, and this renderer is
				 * automatically registered to this scheduler so
				 * that it gets called at each rendering tick.
				 *
				 * @throw RenderingException if registerToScheduler is true 
				 * and no scheduler is available.
				 *
				 */
				explicit StandardRenderer( Video::Surface & screen,
						bool registerToScheduler = true ) 
					throw( RenderingException ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~StandardRenderer() throw() ;
			
			

				/**
				 * Triggers the actual rendering of all views, for specified
				 * rendering tick, if any.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 *
				 * @note If the renderer is called from a basic event loop (no
				 * scheduler is used), then the rendering tick is 
				 * meaningless and is always zero.
				 *
				 */
				virtual void render( 
					Events::RenderingTick currentRenderingTick = 0 ) throw() ;

								
					
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall 
				 * settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;

					
					
			protected:
			

				/// The screen surface that will have to be updated.
				Video::Surface * _screen ;
				
				
				/**
				 * Records all the currently registered views that shall 
				 * be managed specifically by this renderer.
				 *
				 */
				 std::list<Ceylan::View *> _registeredViews ;
				 
				 
				 
			
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit StandardRenderer( const StandardRenderer & source )
					throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				StandardRenderer & operator = ( 
					const StandardRenderer & source ) throw() ;
				
				
				/**
				 * The internal single instance of renderer, at the top 
				 * of the renderer hierarchy, if any.
				 *
				 */
				static Renderer * _internalRootRenderer ;
				
				
		} ;

	}

}


#endif // OSDL_STANDARD_RENDERER_H_

