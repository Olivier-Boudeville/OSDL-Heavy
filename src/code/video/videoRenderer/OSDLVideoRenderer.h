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


#ifndef OSDL_VIDEO_RENDERER_H_
#define OSDL_VIDEO_RENDERER_H_



#include "OSDLRenderer.h"           // for inheritance, RenderingException




namespace OSDL
{



	namespace Rendering 
	{
	
	
		
		/*
		 * Video renderers use a camera to select relevant objects 
		 * which should be rendered.
		 *
		 */
		class Camera ;
				
			
				
		/// Exception raised when a video rendering operation failed.
		class OSDL_DLL VideoRenderingException: public RenderingException
		{
		
			public:
			
				explicit VideoRenderingException( const std::string & message );
				
				virtual ~VideoRenderingException() throw() ;
							
		} ;

			
			
			
		/**
		 * This basic video renderer manages the graphical rendering of 
		 * all registered views.
		 * 
		 * Its rendering is mainly triggered by the root renderer, if 
		 * it exists and if this video renderer has been registered to 
		 * this root renderer.
		 *
		 * Specialized video renderers can take in charge for example 
		 * multiple viewports and cameras, BSP trees, etc.
		 *
		 * @see Ceylan::View
		 *
		 */	
		class OSDL_DLL VideoRenderer : public Rendering::Renderer
		{
		
		
			public:
			
			
			
				/**
				 * Constructs a new video renderer.
				 *
				 * @param registerToRootRenderer iff true, this video 
				 * renderer will register itself automatically to the 
				 * root renderer, which must implement the
				 * MultimediaRenderer interface so that this video 
				 * renderer can be plugged.
				 *
				 * @throw VideoRenderingException if registerToRootRenderer 
				 * is true and no root renderer is available, or if 
				 * the root renderer is not able to register video
				 * renderers (which means it is not a Multimedia renderer).
				 *
				 * @see MultimediaRenderer
				 *
				 */
				explicit VideoRenderer( bool registerToRootRenderer = true ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~VideoRenderer() throw() ;
			


				/**
				 * Returns whether this video renderer has an internal 
				 * camera available.
				 *
				virtual bool hasCamera() const ;
				 */
				
				
				 				
				/**
				 * Returns the internal camera of this video renderer.
				 *
				 * @throw VideoRenderingException iff no camera is available.
				virtual Camera & getCamera() const ;
				 */
			
					
					
				/**
				 * Sets the internal camera of this video renderer.
				 *
				 * @param newCamera the new camera to use from now on.
				 *
				 * @note The video renderer takes ownership of the 
				 * provided camera, and will delete it when itself deleted, 
				 * or when replaced by another camera.
				 *
				virtual void setCamera( Camera & newCamera ) ; 	
				 */
					
					
					
				/**
				 * Triggers the actual video rendering of all views, for
				 * specified rendering tick, if any.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 *
				 * If the renderer is called from a basic event loop 
				 * (no scheduler is used), then the rendering tick is
				 * meaningless and is always zero.
				 *
				 */
				virtual void render( 
					Events::RenderingTick currentRenderingTick = 0 ) ;
	
				
				
				/**
				 * Allows the video renderer to be aware that a rendering 
				 * step has to be skipped.
				 *
				 * It may be a chance for it to trigger counter-measures, 
				 * such as decreasing the level of detail in order not 
				 * to slow down the whole process.
				 *
				 * @param skippedRenderingTick the rendering tick that 
				 * had to be skipped.
				 *
				 */
				virtual void onRenderingSkipped( 
					Events::RenderingTick skippedRenderingTick ) ;
						
						
						
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
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;



					
			protected:
			
			
				/// The internal camera which is used to render, if any.
				//Camera * _internalCamera ;
			
			
			
			
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				VideoRenderer( const VideoRenderer & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				VideoRenderer & operator = ( const VideoRenderer & source ) ;
					
				
		} ;


	}

}



#endif // OSDL_VIDEO_RENDERER_H_

