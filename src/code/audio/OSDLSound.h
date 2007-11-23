#ifndef OSDL_SOUND_H_
#define OSDL_SOUND_H_


#include "OSDLAudioCommon.h" // for 
#include "OSDLAudible.h"     // for AudibleException, inheritance

#include "Ceylan.h"          // for LoadableWithContent

#include <string>



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

		/**
		 * The OSDL fallback sound structure.
		 *
		 */
		struct LowLevelSound 
		{
		
			/// The actual sound data:
			Ceylan::Byte * _samples ;
		
			/// The size, in bytes, of sound data:
			Ceylan::Uint32 _size ;
		
			/// The sound sampling frequency, in Hertz:
			Ceylan::Uint16 _frequency ;
			
			 
		} ;

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
		 * They are also objects containing a low level sample (here a sound),
		 * which can be loaded, unloaded, reloaded from file at will.
		 * Therefore a Sound is (indirectly) a Loadable instance (mother class
		 * of the LoadableWithContent template).
		 *
		 * Methods for playing sounds are non-blocking: they trigger the 
		 * playing and return immediately, the execution continues while the
		 * playback goes on, unless specified otherwise.
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
				 * @param soundFile the file containing the targeted sound.
				 * On all PC-like platforms (including Windows and most UNIX),
				 * the supported formats are WAVE, AIFF, RIFF, OGG, and VOC.
				 * WAVE and, to a lesser extent, OGG, are recommended for 
				 * sounds. 
				 *
				 * @param preload the sound will be loaded directly by this
				 * constructor iff true, otherwise only its path will be
				 * stored to allow for later loading.
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
				 * @throw Ceylan::LoadableException whenever the loading fails.
				 *
				 */
				virtual bool load() throw( Ceylan::LoadableException ) ;
		
		
 	           /**
				* Unloads the sound that may be contained by this instance.
				*
				* @return true iff the sound had to be actually unloaded
				* (otherwise it was not already available and nothing was done).
				*
				* @note One has to ensure the sound is not playing before
				* calling this method.
				*
				* @throw Ceylan::LoadableException whenever the unloading fails.
				*
				*/
				virtual bool unload() throw( Ceylan::LoadableException ) ;
		



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
				 * The volume setting will take effect when this sound will be
				 * used on a channel, being mixed into the output.
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
		
		
		
		
				/*
				 * Play section.
				 *
				 * Each subsection includes three methods :
				 *
				 * 1. one inherited from Audible with automatic channel choice,
				 * this choice not being returned
				 *
				 * 2. one with automatic channel choice, this choice being
				 * returned
				 *
				 * 3. one with the target channel being specified
				 *
				 */
				
				
				
				// Simple play subsection.
				
				
				/**
				 * Plays this sound instance at once on any appropriate 
				 * channel number.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @return the number of the channel number being used.
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playReturnChannel( 
					PlaybackCount playCount = 1 ) throw( AudibleException ) ; 


				/**
				 * Plays this sound instance at once on specified mixing
				 * channel.
				 *
				 * @param mixingChannelNumber number of the mixing channel
				 * this sound should be played on.
				 *
				 * @param playCount the number of times this sound should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void play( 
					ChannelNumber mixingChannelNumber, 
					PlaybackCount playCount = 1 ) throw( AudibleException ) ; 



				// Play with time-out subsection.

		
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void playForAtMost( 
						Ceylan::System::Millisecond maxDuration, 
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
	
		
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @return the number of the channel number being used.
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual ChannelNumber playForAtMostReturnChannel( 
						Ceylan::System::Millisecond maxDuration, 
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 */
				virtual void playForAtMost(
						Ceylan::System::Millisecond maxDuration, 
						ChannelNumber mixingChannelNumber,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
	
		
				// Play with fade-in subsection.
		
		
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::System::Millisecond fadeInMaxDuration,
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @return the channel number on which this sound is played.
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playWithFadeInReturnChannel( 
						Ceylan::System::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
				
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::System::Millisecond fadeInMaxDuration,
						ChannelNumber mixingChannelNumber,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 



				// Play with time-out and fade-in subsection.


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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInForAtMostReturnChannel if the chosen
				 * channel needs to be known.
				 *
				 */
				virtual void playWithFadeInForAtMost( 
						Ceylan::System::Millisecond playbackMaxDuration,
						Ceylan::System::Millisecond fadeInMaxDuration,
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @return the channel number on which this sound is played.
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *				 
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a channel.
				 *
				 */
				virtual ChannelNumber playWithFadeInForAtMostReturnChannel( 
						Ceylan::System::Millisecond playbackMaxDuration,
						Ceylan::System::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 


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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported or if no free channel is available.
				 *
				 * @see playWithFadeInReturnChannel if the chosen channel 
				 * needs to be known.
				 *
				 */
				virtual void playWithFadeInForAtMost( 
						Ceylan::System::Millisecond playbackMaxDuration,
						Ceylan::System::Millisecond fadeInMaxDuration,
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
			
			
				/**
				 * The internal low level sound is defined through the
				 * template.
				 *
				 */
				
			
			
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

