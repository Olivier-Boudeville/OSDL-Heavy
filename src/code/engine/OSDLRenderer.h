#ifndef OSDL_RENDERER_H_
#define OSDL_RENDERER_H_


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLException.h"            // for Exception

#include "Ceylan.h"                   // for inheritance

#include <string>
#include <list>





namespace OSDL
{


	namespace Rendering 
	{
	
				
		/**
		 * Exception to be thrown when rendering encounters an abnormal
		 * situation.
		 *
		 */
		class RenderingException : public OSDL::Exception
		{
			public:

				explicit RenderingException( const std::string & reason )
					throw() ;
				virtual ~RenderingException() throw() ;

		} ;

		
		/**
		 * Records a number of render actions.
		 *
		 */
		typedef Ceylan::Uint32 RenderCount ;		
				
				
				
		/**
		 * This basic renderer manages the rendering of all registered views.
		 * Note that renderers and views are not dedicated to the graphical
		 * output, since they have to take in charge all multimedia fields,
		 * including sounds and, possibly, other media. 
		 *
		 * The privilegied way of organizing the rendering is to have 
		 * renderers taking care of all its aspects (video, but also audio,
		 * etc.). 
		 *
		 * Renderers are not necessarily singletons, but having only one
		 * centralized instance for all views might be simpler.
		 * In this case it is called a root renderer.
		 * A root renderer may delegate part of its task to specialized
		 * sub-renderers.
		 *
		 * More precisely, the scheduler will call only one of them, the 
		 * root renderer, which may delegate a part of its charge to any 
		 * number of specialized renderers, for example if multiple viewports
		 * are used in the same window, or for the sound, or to take advantage
		 * of multicore processors.
		 *
		 * A renderer knows all the views (as defined by the MVC framework) 
		 * it is in charge of, but only them (not their model for instance).
		 *
		 * However no data structure is provided with this generic renderer,
		 $ since it may vary a lot depending on the need, from simple list 
		 * to BSP trees, etc.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 * @see Ceylan::View
		 *
		 */	
		class Renderer : public Ceylan::Object
		{
		
		
			public:
			
			
				/**
				 * Constructs a new renderer.
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
				explicit Renderer( bool registerToScheduler = true ) 
					throw( RenderingException ) ;
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~Renderer() throw() ;
			

				/**
				 * Triggers the actual rendering of all views, for specified
				 * rendering tick, if any.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 * If the renderer is called from a basic event loop (no
				 * scheduler is used), then the rendering tick is 
				 * meaningless and is always zero.
				 *
				 */
				virtual void render( 
					Events::RenderingTick currentRenderingTick = 0 ) throw() ;

				
				/**
				 * Allows the renderer to be aware that a rendering step 
				 * has to be skipped.
				 *
				 * It may be a chance for it to trigger counter-measures, 
				 * such as decreasing the level of detail in order not to 
				 * slow down the whole process.
				 *
				 * @param skippedRenderingTick the rendering tick that had 
				 * to be skipped.
				 *
				 */
				virtual void onRenderingSkipped( 
					Events::RenderingTick skippedRenderingTick ) throw() ;
				
					
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



				// Static section.


				/**
				 * Tells whether a root renderer is available.
				 *
				 * @return true iff a root renderer is already available.
				 *
				 */
				static bool HasExistingRootRenderer() throw() ;
				 
				 
	       	   /**
	         	* Returns the one and only one root renderer instance that 
				* may be already available.
	         	*
	         	* @note The returned value is a reference and not a pointer, 
				* to avoid any abnormal deallocation by its users, that 
				* should never deallocate the root renderer.
	         	*
				* @throw RenderingException if there is no root renderer 
				* already available.
				*
				* @note There is no 'GetRootRenderer' method that would 
				* create a root renderer if none was already set, since 
				* various renderers could be chosen as the root one.
				*
	         	*/
	        	static Renderer & GetExistingRootRenderer() 
					throw( RenderingException ) ;


				/**
				 * Deletes the existing root renderer.
				 *
				 * @throw RenderingException if not renderer was available.
				 *
				 */
	       		static void DeleteExistingRootRenderer() 
					throw( RenderingException ) ;
		
		
				/**
				 * Deletes the root renderer, if any.
				 *
				 */
	       		static void DeleteRootRenderer() throw() ;
					
		
					
					
			protected:
			

				/**
				 * Counts the number of renderings achieved.
				 *
				 */
				RenderCount _renderingDone ;
		
		
				/**
				 * Counts the number of renderings skipped.
				 *
				 */
				RenderCount _renderingSkipped ;
		
		
				/**
				 * Records the last rendering tick.
				 *
				 * Useful to check whether no rendering tick was missed.
				 *
				 */
				Events::RenderingTick _lastRender ;
		

				/**
				 * Records all the currently registered views that shall 
				 * be managed specifically by this renderer.
				 *
				 */
				 //std::list<View *> _registeredViews ;
				 
				 
			
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Renderer( const Renderer & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Renderer & operator = ( const Renderer & source ) throw() ;
				
				
				/**
				 * The internal single instance of renderer, at the top 
				 * of the renderer hierarchy, if any.
				 *
				 */
				static Renderer * _internalRootRenderer ;
				
				
		} ;

	}

}


#endif // OSDL_RENDERER_H_
