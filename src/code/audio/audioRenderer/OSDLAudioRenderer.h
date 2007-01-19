#ifndef OSDL_AUDIO_RENDERER_H_
#define OSDL_AUDIO_RENDERER_H_



#include "OSDLRenderer.h"           // for inheritance



namespace OSDL
{


	namespace Rendering 
	{
	
		
		/*
		 * Audio renderers use a point of perception to select relevant 
		 * objects which should be rendered.
		 *
		 */
		class Camera ;
				
				
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
		class AudioRenderer : public Rendering::Renderer
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
				explicit AudioRenderer( bool registerToRootRenderer = true ) 
					throw( RenderingException ) ;
				
				
				/**
				 * Virtual destructor, no view is destroyed.
				 *
				 */
				virtual ~AudioRenderer() throw() ;
			

				/**
				 * Returns whether this audio renderer has an internal 
				 * camera available.
				 *
				 */
				//virtual bool hasCamera() const throw() ;
				
				 				
				/**
				 * Returns the internal camera of this audio renderer.
				 *
				 * @throw RenderingException iff no camera is available.
				 */
				//virtual Camera & getCamera() const 
				//	throw( RenderingException ) ; 	
					
					
				/**
				 * Sets the internal camera of this audio renderer.
				 *
				 * @param newCamera the new camera to use from now on.
				 *
				 * @note The audio renderer takes ownership of the provided
				 * camera, and will delete it when itself deleted, or when
				 * replaced by another camera.
				 *
				 */
				//virtual void setCamera( Camera & newCamera ) throw() ; 	
					
					
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
					Events::RenderingTick currentRenderingTick = 0 ) throw() ;
	
				
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


					
			protected:
			
			
				/// The internal camera which is used to render, if any.
				//Camera * _internalCamera ;
			
			
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				AudioRenderer( const AudioRenderer & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				AudioRenderer & operator = ( const AudioRenderer & source )
					throw() ;
					
				
		} ;

	}

}


#endif // OSDL_AUDIO_RENDERER_H_
