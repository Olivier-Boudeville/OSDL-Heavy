/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_RENDERER_H_
#define OSDL_RENDERER_H_


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLException.h"            // for Exception

#include "Ceylan.h"                   // for inheritance

#include <string>




namespace OSDL
{



	namespace Rendering 
	{
	
		
				
		/**
		 * Exception to be thrown when the rendering task encounters an abnormal
		 * situation.
		 *
		 */
		class OSDL_DLL RenderingException : public OSDL::Exception
		{
			public:

				explicit RenderingException( const std::string & reason ) ;
				
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
		 * The privileged way of organizing the rendering is to have 
		 * renderers taking care of all multimedia aspects (video, but also
		 * audio, etc.). 
		 *
		 * Renderers are not necessarily singletons, but having only one
		 * centralized instance for all views might be simpler.
		 *
		 * In this case it is called a root renderer.
		 *
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
		 * it is in charge of, but only them (not their model, for instance).
		 *
		 * However no data structure is provided with this generic renderer,
		 * since it may vary a lot depending on the need, from simple list 
		 * to BSP trees, etc.
		 *
		 * Views are not owned by the renderer.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 * @see Ceylan::View
		 *
		 */	
		class OSDL_DLL Renderer : public Ceylan::Object
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
				explicit Renderer( bool registerToScheduler = true ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~Renderer() throw() ;
			
			

				/**
				 * Triggers the actual rendering of all views, for specified
				 * rendering tick, if any.
				 *
				 * This default implementation does nothing except some logging
				 * and accounting of rendereding ticks.
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
					Events::RenderingTick currentRenderingTick = 0 ) ;

				
				
				/**
				 * Allows the renderer to be aware that a rendering step 
				 * had to be skipped.
				 *
				 * It may be a chance for it to trigger counter-measures, 
				 * such as decreasing the level of detail in order not to 
				 * slow down the whole process, or simply notifying the views
				 * of the skip.
				 *
				 * @param skippedRenderingTick the rendering tick that had 
				 * to be skipped.
				 *
				 */
				virtual void onRenderingSkipped( 
					Events::RenderingTick skippedRenderingTick ) ;
				
				
					
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
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;





				// Static section.



				/**
				 * Tells whether a root renderer is available.
				 *
				 * @return true iff a root renderer is already available.
				 *
				 */
				static bool HasExistingRootRenderer() ;
				 
				 
				 
	       	   /**
	         	* Returns the one and only one root renderer instance that 
				* may be already available.
	         	*
	         	* @note The returned value is a reference and not a pointer, 
				* to help avoiding any abnormal deallocation by its users, that 
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
	        	static Renderer & GetExistingRootRenderer() ;



				/**
				 * Deletes the existing root renderer.
				 *
				 * @throw RenderingException if not renderer was available.
				 *
				 */
	       		static void DeleteExistingRootRenderer() ;
		
		
		
				/**
				 * Deletes the root renderer, if any.
				 *
				 */
	       		static void DeleteRootRenderer() ;
					
		
		
					
					
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
		


				// No data structure enforced for views here.

				 
				 
			
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Renderer( const Renderer & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Renderer & operator = ( const Renderer & source ) ;
				
				
				
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

