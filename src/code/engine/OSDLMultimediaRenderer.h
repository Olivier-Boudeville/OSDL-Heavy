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


#ifndef OSDL_MULTIMEDIA_RENDERER_H_
#define OSDL_MULTIMEDIA_RENDERER_H_


#include "OSDLRenderer.h"             // for inheritance

#include <string>




namespace OSDL
{



	namespace Rendering 
	{
	
	
		
		// A video renderer may be used by multimedia renderers.
		class VideoRenderer ;
		
		// An audio renderer may be used by multimedia renderers.
		class AudioRenderer ;
		
		
				
		/**
		 * This renderer manages all multimedia concerns for the views it 
		 * is in charge of.
		 *
		 * It is triggered by the scheduler, so that at each rendering tick 
		 * it gets activated.
		 *
		 * This multimedia renderer delegates all graphical concerns to an
		 * internal video renderer, if any, and all sound concerns to an
		 * internal audio renderer, if any.
		 *
		 * This kind of renderer is often used as a root renderer, since it 
		 * is able to federate all multimedia needs for the views.
		 *
		 * @see OSDL::Engine::Scheduler
		 *
		 * @see Ceylan::View
		 *
		 */	
		class OSDL_DLL MultimediaRenderer : public Renderer
		{
		
		
			public:
			
			
			
				/**
				 * Constructs a new multimedia renderer.
				 *
				 * @param registerToScheduler iff true, then a scheduler 
				 * is supposed to exist already, and this renderer is
				 * automatically registered to the scheduler so that it 
				 * gets called at each rendering tick.
				 *
				 * @throw RenderingException if registerToScheduler is true 
				 * and no scheduler is available.
				 *
				 */
				explicit MultimediaRenderer( bool registerToScheduler = true ) ;
				
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~MultimediaRenderer() throw() ;
			
			

				/**
				 * Returns whether a video renderer is available.
				 *
				 */
				virtual bool hasVideoRenderer() const ;
				
				
				
				/**
				 * Returns the video renderer currently been used by this
				 * multimedia renderer.
				 *
				 * @throw RenderingException iff no video renderer is available.
				 *
				 */
				virtual VideoRenderer & getVideoRenderer() const ;
				
				
				
				/** 
				 * Sets the video renderer that should be used by this
				 * multimedia renderer.
				 *
				 * @param newVideoRenderer the new video renderer.
				 *
				 * If a renderer was already registered, it is deallocated 
				 * first: this renderer takes ownership of its sub-renderers
				 * and manages their life cycle.
				 *
				 */
				virtual void setVideoRenderer( 
					VideoRenderer & newVideoRenderer ) ;
				
				
				
				/**
				 * Returns whether a audio renderer is available.
				 *
				 */
				virtual bool hasAudioRenderer() const ;
				
				
				
				/**
				 * Returns the audio renderer currently been used by 
				 * this multimedia renderer.
				 *
				 * @throw RenderingException iff no audio renderer is available.
				 *
				 */
				virtual AudioRenderer & getAudioRenderer() const ;
				
				
				
				/** 
				 * Sets the audio renderer that should be used by this
				 * multimedia renderer.
				 *
				 * @param newAudioRenderer the new audio renderer.
				 *
				 * If a renderer was already registered, it is deallocated 
				 * first: this renderer takes ownership of its sub-renderers
				 * and manages their life cycle.
				 *
				 */
				virtual void setAudioRenderer( 
					AudioRenderer & newAudioRenderer ) ;
				
				
				
				/**
				 * Triggers the actual rendering of all views, for specified
				 * rendering tick, if any.
				 *
				 * This method will manage both the video and audio 
				 * renderings, should the dedicated specialized renderers 
				 * be available.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 * If the renderer is called from a basic event loop (no
				 * scheduler is used), then the rendering tick is meaningless
				 * and is always zero.
				 *
				 * @see renderVideo, renderAudio
				 *
				 */
				virtual void render( 
					Events::RenderingTick currentRenderingTick = 0 ) ;



				/**
				 * Triggers the actual video rendering of all views, for
				 * specified rendering tick, if any.
				 *
				 * This method will manage the video rendering, and is to 
				 * be called by the overall <b>render</b> method, implemented
				 * by all renderers.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 * If the renderer is called from a basic event loop (no
				 * scheduler is used), then the rendering tick is meaningless
				 * and is always zero.
				 *
				 * @see render, renderAudio
				 *
				 * @note This method is used instead of calling directly 
				 * the internal video renderer so that informations can 
				 * be more easily shared between the multimedia renderer 
				 * and its specialized counterparts.
				 *
				 */
				virtual void renderVideo( 
					Events::RenderingTick currentRenderingTick = 0 ) = 0 ;
				
				
				
				/**
				 * Triggers the actual audio rendering of all views, for
				 * specified rendering tick, if any.
				 *
				 * This method will manage the audio rendering, and is to be
				 * called by the overall <b>render</b> method, implemented 
				 * by all renderers.
				 *
				 * @param currentRenderingTick the rendering tick 
				 * corresponding to this render step. 
				 * If the renderer is called from a basic event loop (no
				 * scheduler is used), then the rendering tick is meaningless
				 * and is always zero.
				 *
				 * @see render, renderVideo
				 *
				 * @note This method is used instead of calling directly 
				 * the internal audio renderer so that informations can be 
				 * more easily shared between the multimedia renderer and 
				 * its specialized counterparts.
				 *
				 */
				virtual void renderAudio( 
					Events::RenderingTick currentRenderingTick = 0 ) = 0 ;
				
				
				
				/**
				 * Allows the renderer to be aware that a rendering step 
				 * has to be skipped.
				 *
				 * It may be a chance for it to trigger counter-measures, 
				 * such as decreasing the level of detail in order not to 
				 * slow down the whole process.
				 *
				 * @param skippedRenderingTick the rendering tick that 
				 * had to be skipped.
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
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;



				
					
			protected:
			
			      
				/**
				 * The internally used video renderer being used, if any.
				 *
				 */
				VideoRenderer * _videoRenderer ;
				
				
				
				/**
				 * The internally used audio renderer being used, if any.
				 *
				 */
				AudioRenderer * _audioRenderer ;
				
				

			
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit MultimediaRenderer( 
					const MultimediaRenderer & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				MultimediaRenderer & operator = ( 
					const MultimediaRenderer & source ) ;
				
				
				
		} ;
		

	}

}



#endif // OSDL_MULTIMEDIA_RENDERER_H_

