#ifndef OSDL_SOUND_H_
#define OSDL_SOUND_H_


#include "OSDLAudible.h"     // for AudibleException, inheritance

#include "Ceylan.h"          // for Hertz

#include <string>
#include <list>


#if ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 

// No need to include SDL_Mixer header here:
struct Mix_Chunk ; 

#endif //  ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 


namespace OSDL
{

	
		
	
	namespace Audio 
	{
			

		/// Low-level sound being used internally.
#if ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 

		typedef ::Mix_Chunk LowLevelSound ;
		
#else // OSDL_USES_SDL_MIXER	

		struct LowLevelSound {} ;

#endif // OSDL_USES_SDL_MIXER


			
		/// Exceptions raised by Sound instances. 		
		class OSDL_DLL SoundException: public AudibleException 
		{ 
			public: 
			
				SoundException( const std::string & reason ) throw() ; 
				virtual ~SoundException() throw() ; 
		} ;
			
			
				
												
		/**
		 * Sound is punctual audible content, as opposed to music.  
		 * Both are Audible instances.
		 *
		 * They are also objects containing a low level sound sample, which
		 * can be loaded, unloaded, reloaded from file at will.
		 * Therefore a Sound is (indirectly) a Loadable instance (mother class
		 * of the LoadableWithContent template).
		 *
		 * @see Music
		 *
		 */
		class OSDL_DLL Sound: public Audible, 
			public Ceylan::LoadableWithContent<LowLevelSound>
		{
		
		
		
			public:
				

				/**
				 * Creates a new sound instance from specified file.
				 *
				 * @param soundFile the file contained the targeted sound.
				 *
				 * @param preload the sound will be loaded directly by this
				 * constructor iff true, otherwise only its path will be
				 * stored for later loading.
				 *
				 * @throw SoundException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Sound( const std::string & soundFile, 
						bool preload = true )
					throw( SoundException ) ;
				
				
				/// Virtual destructor.
				virtual ~Sound() throw() ;
		
		
		
				// LoadableWithContent template instanciation.
		
				
				/**
				 * Loads the sound from file.
				 *
				 * @return true iff the sound had to be actually loaded
				 * (otherwise it was already loaded and nothing was done).
				 *
				 * @throw LoadableException whenever the loading fails.
				 *
				 */
				virtual bool load() throw( LoadableException ) ;
		
		
 	           /**
				* Unloads the sound that may be contained by this instance.
				*
				* @return true iff the sound had to be actually unloaded
				* (otherwise it was not already available and nothing was done).
				*
				* @throw LoadableException whenever the unloading fails.
				*
				*/
				virtual bool unload() throw( LoadableException ) ;
		



				// Audible implementation.
				
				
				/**
				 * Returns the volume associated to this sound instance.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no previously loaded sound is
				 * available.
				 *
				 */
				virtual Volume getVolume() const throw( SoundException ) ;
		
		
				/**
				 * Sets the volume associated to this sound instance.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no previously loaded sound is
				 * available.
				 *
				 */
				virtual void setVolume( Volume newVolume ) 
					throw( SoundException ) ;
		
		
		
		
				// Play section.
				
				
				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playReturnChannel if the chosen channel needs to be
				 * known.
				 *
				 */
				virtual void play( PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 


				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number, and returns that channel number.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playReturnChannel( 
					PlaybackCount playCount = 1 ) throw( SoundException ) ; 


				/**
				 * Plays this sound instance at once on specified mixing
				 * channel.
				 *
				 * @param mixingChannelNumber number of the mixing channel
				 * this sound should be played on.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void play( 
					ChannelNumber mixingChannelNumber, 
					PlaybackCount playCount = 1 ) throw( SoundException ) ; 

		
				/**
				 * Plays this sound instance at once on any channel number
				 * within specified duration.
				 *
				 * @param maxDuration the maximum duration during which this
				 * sound will be played, in milliseconds. 
				 * It may stop earlier if: 
				 * (sound duration).playCount < maxDuration
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void playForAtMost( 
						Ceylan::Millisecond maxDuration, 
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
	
	
				/**
				 * Plays this sound instance at once on specified mixing channel
				 * within specified duration.
				 *
				 * @param maxDuration the maximum duration during which this
				 * sound will be played, in milliseconds. 
				 * It may stop earlier if: 
				 * (sound duration).playCount < maxDuration
				 *
				 * @param mixingChannelNumber number of the mixing channel
				 * this sound should be played on.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void playForAtMost(
						Ceylan::Millisecond maxDuration, 
						ChannelNumber mixingChannelNumber,
						PlaybackCount playCount = 1 ) 
					throw( SoundException ) ; 
		
		
		
				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number, beginning with a fade-in effect.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
		
				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number, beginning with a fade-in effect. 
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @return the channel number on which this sound is played.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playWithFadeInReturnChannel( 
						Ceylan::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( SoundException ) ; 
		
				
				/**
				 * Plays this sound instance at once on specified 
				 * channel number, beginning with a fade-in effect.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param mixingChannelNumber number of the mixing channel
				 * this sound should be played on.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::Millisecond fadeInMaxDuration,
						ChannelNumber mixingChannelNumber,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 


				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number, beginning with a fade-in effect.
				 *
				 * @param playbackMaxDuration the maximum duration during which
				 * this sound will be played, in milliseconds.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInForAtMostReturnChannel if the chosen
				 * channel needs to be known.
				 *
				 */
				virtual void playWithFadeInForAtMost( 
						Ceylan::Millisecond playbackMaxDuration,
						Ceylan::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 


				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number, beginning with a fade-in effect.
				 *
				 * @param playbackMaxDuration the maximum duration during which
				 * this sound will be played, in milliseconds.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @return the channel number on which this sound is played.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *				 
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playWithFadeInForAtMostReturnChannel( 
						Ceylan::Millisecond playbackMaxDuration,
						Ceylan::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( SoundException ) ; 


				/**
				 * Plays this sound instance at once on specified 
				 * channel number, beginning with a fade-in effect.
				 *
				 * @param playbackMaxDuration the maximum duration during which
				 * this sound will be played, in milliseconds.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param mixingChannelNumber number of the mixing channel
				 * this sound should be played on.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::Millisecond playbackMaxDuration,
						Ceylan::Millisecond fadeInMaxDuration,
						ChannelNumber mixingChannelNumber,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
		
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
			
			
				/// The internal audio chunk is inherited from the template.
				
			
			private:


	
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Sound( const Sound & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				Sound & operator = ( const Sound & source ) throw() ;
				
			
		} ;
		
	}	
	
}	



#endif // OSDL_SOUND_H_

