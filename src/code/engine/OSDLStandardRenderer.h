/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#ifndef OSDL_STANDARD_RENDERER_H_
#define OSDL_STANDARD_RENDERER_H_


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLRenderer.h"             // for inheritance


#include <string>
#include <list>



namespace Ceylan
{		

	namespace MVC
	{
	
	
		// A standard renderer handles views.
		class BaseView ;		
	
	}	

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
		 * Any view is supposed to be registered only once, or not at all.
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
					bool registerToScheduler = true ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~StandardRenderer() throw() ;
			
			
				
				/**
				 * Registers specified view to this renderer.
				 * Ownership not taken.
				 *
				 * @param view the view to register.
				 *
				 */
				virtual void registerView( Ceylan::MVC::BaseView & view ) ;
			
			
			
				/**
				 * Unregisters specified view to this renderer.
				 *
				 * @param view the view to unregister.
				 *
				 * @throw RenderingException if this view was not already 
				 * registered.
				 *
				 */
				virtual void unregisterView( Ceylan::MVC::BaseView & view ) ;
			
			

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
					Events::RenderingTick currentRenderingTick = 0 ) ;

								
				
				/**
				 * Allows the renderer to be aware that a rendering step 
				 * had to be skipped.
				 *
				 * All registered views will be notified of the skip.
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

					
					
					
			protected:
			
			

				/// The screen surface that will have to be updated.
				Video::Surface * _screen ;
				
				
				
				/**
				 * Records all the currently registered views that shall 
				 * be managed specifically by this renderer.
				 *
				 * @note Views are not owned by the renderer.
				 *
				 */
				std::list<Ceylan::MVC::BaseView *> _registeredViews ;
				 
				 
				 
				 
			
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit StandardRenderer( const StandardRenderer & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				StandardRenderer & operator = ( 
					const StandardRenderer & source ) ;
				
				
				
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

