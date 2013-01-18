/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#ifndef OSDL_AUDIO_RENDERER_H_
#define OSDL_AUDIO_RENDERER_H_



#include "OSDLRenderer.h"           // for inheritance





namespace OSDL
{
	
	
		
	namespace Rendering 
	{
				
			
				
		/**
		 * This basic audio renderer manages the sound rendering of all
		 * registered views.
		 * 
		 * Its rendering is mainly triggered by the root renderer, if it 
		 * exists and if this audio renderer has been registered to this
		 * root renderer.
		 *
		 * Specialized audio renderers can take in charge for example 
		 * sound propagation, multi-channel output and points of perception.
		 *
		 * @see Ceylan::View
		 *
		 */	
		class OSDL_DLL AudioRenderer : public Rendering::Renderer
		{
		
		
			public:
			
			
			
				/**
				 * Constructs a new audio renderer.
				 *
				 * @param registerToRootRenderer iff true, this audio 
				 * renderer will register itself automatically to the root
				 * renderer, which must implement the MultimediaRenderer
				 * interface so that this audio renderer can be plugged.
				 *
				 * @throw RenderingException if registerToRootRenderer is 
				 * true and no root renderer is available, or if the root
				 * renderer is not able to register audio renderers 
				 * (which means it is not a Multimedia renderer).
				 *
				 * @see MultimediaRenderer
				 *
				 */
				explicit AudioRenderer( bool registerToRootRenderer = true ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~AudioRenderer() throw() ;
			

					
				/**
				 * Triggers the actual audio rendering of all views, for
				 * specified rendering tick, if any.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. If the renderer is 
				 * called from a basic event loop (no scheduler is used),
				 * then the rendering tick is meaningless and is always zero.
				 *
				 */
				virtual void render( 
					Events::RenderingTick currentRenderingTick = 0 ) ;
	
						
				
				
				/**
				 * Allows the audio renderer to be aware that a rendering 
				 * step has to be skipped.
				 *
				 * It may be a chance of it to trigger counter-measures, 
				 * such as decreasing the level of detail in order not to 
				 * slow down the whole process.
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

			
			
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				AudioRenderer( const AudioRenderer & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				AudioRenderer & operator = ( const AudioRenderer & source ) ;
					
				
		} ;
		

	}

}



#endif // OSDL_AUDIO_RENDERER_H_

