#ifndef OSDL_MUSIC_H_
#define OSDL_MUSIC_H_


#include "OSDLAudioCommon.h" // for MusicType, etc.
#include "OSDLAudible.h"     // for AudibleException, inheritance


#include "Ceylan.h"          // for LoadableWithContent


#include <string>



#if ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 

/*
 * No need to include SDL_Mixer header here.
 *
 * 'struct Mix_Music' could not be forward-declared, as SDL_mixer.h defines 
 * it as 'typedef struct _Mix_Music Mix_Music;' which leads to the error
 * "'struct Mix_Music' has a previous declaration as 'struct Mix_Music'",
 * as soon as an implementation file (ex: OSDLAudio.cc) includes SDL_mixer.h.
 * So we use _Mix_Music instead of Mix_Music.
 *
 */
struct _Mix_Music ;

#endif //  ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 




namespace OSDL
{

		
	
	namespace Audio 
	{

			

		/// Low-level music being used internally.
#if ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 

		typedef ::_Mix_Music LowLevelMusic ;
		
#else // OSDL_USES_SDL_MIXER	

		struct LowLevelMusic {} ;

#endif // OSDL_USES_SDL_MIXER


			
		/// Exceptions raised by Music instances. 		
		class OSDL_DLL MusicException: public AudibleException
		{ 
			public: 
			
				MusicException( const std::string & reason ) throw() ; 
				virtual ~MusicException() throw() ; 
		} ;
			
							
		/**
		 * Describes a position in a music.
		 *
		 * For MOD musics, it is used as a pattern number in the module.
		 * Passing zero is similar to rewinding the music.
		 *
		 * For OGG musics, it is used to jump to position seconds from the
		 * beginning of the music.
		 *
		 * For MP3 musics, it is used to jump to position seconds from the
		 * current position in the music. One may use the rewind method to start
		 * from the beginning.
		 *
		 *
		 */
		typedef Ceylan::Uint32 MusicPosition ;

		
				
		/**
		 * Describes all the known music types.
		 *
		 */
		enum MusicType
		{
		
			/**
			 * Waveform audio format (WAVE/RIFF).
			 *
			 * Usual extension is .wav.
			 *
			 * @see http://en.wikipedia.org/wiki/WAV
			 *
			 */
			Wave,
			
			
			/**
			 * Soundtrack Module.
			 *
			 * Usual extension is .mod.
			 *
			 * @see http://en.wikipedia.org/wiki/MOD_(file_format)
			 *
			 */
			MOD,
			
			
			/**
			 * Musical Instrument Digital Interface.
			 *
			 * Usual extension is .mid.
			 *
			 * @see http://en.wikipedia.org/wiki/Midi
			 *
			 */
			MIDI,
			
			
			/**
			 * Vorbis encoding over Ogg container.
			 *
			 * Usual extension is .ogg.
			 *
			 * @see http://en.wikipedia.org/wiki/Vorbis
			 *
			 */
			OggVorbis,
			
			
			/**
			 * MPEG-1 Audio Layer 3.
			 *
			 * Usual extension is .mp3.
			 *
			 * @see http://en.wikipedia.org/wiki/MP3
			 *
			 */
			MP3,
			
			
			/**
			 * Music managed externally by a third-party player, triggered
			 * by a command-line call.
			 *
			 */
			CommandBased,
			
			
			/**
			 * To be used to mean no music is available hence no type can be
			 * used (ex: when wanting to know the type of the current music
			 * being played).
			 *
			 */
			NoMusic,
			
			
			/**
			 * Unknown music type.
			 *
			 */
			Unknown
		
		} ;
	
		
												
		/**
		 * Music is continuous audible content, as opposed to sound.  
		 * Both are Audible instances.
		 *
		 * They are also objects containing a low level sample (here a music),
		 * which can be loaded, unloaded, reloaded from file at will.
		 * Therefore a Music is (indirectly) a Loadable instance (mother class
		 * of the LoadableWithContent template).
		 *
		 * Several music instances can exist, but at any given time up to one
		 * can be active.
		 *
		 * Methods for playing musics are non-blocking: they trigger the 
		 * playing and return immediately, the execution continues while the
		 * playback goes on, unless specified otherwise.
		 *
		 * @see Sound
		 *
		 */
		class OSDL_DLL Music: public Audible, 
			public Ceylan::LoadableWithContent<LowLevelMusic>
		{
		
				
			
			public:
				

				/**
				 * Creates a new music instance from specified file.
				 *
				 * @param musicFile the file containing the targeted music.
				 * On all PC-like platforms (including Windows and most UNIX),
				 * the supported formats are WAVE, MOD, MIDI, OGG, and MP3.
				 * OGG and, to a lesser extent, WAVE, are recommended for 
				 * musics. 
				 *
				 * @param preload the music will be loaded directly by this
				 * constructor iff true, otherwise only its path will be
				 * stored to allow for later loading.
				 *
				 * @throw MusicException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Music( const std::string & musicFile, 
						bool preload = true )
					throw( MusicException ) ;
				
				
				/// Virtual destructor.
				virtual ~Music() throw() ;
		
		
				// LoadableWithContent template instanciation.
		
				
				/**
				 * Loads the music from file.
				 *
				 * @return true iff the music had to be actually loaded
				 * (otherwise it was already loaded and nothing was done).
				 *
				 * @throw Ceylan::LoadableException whenever the loading fails.
				 *
				 */
				virtual bool load() throw( Ceylan::LoadableException ) ;
		
		
 	           /**
				* Unloads the music that may be contained by this instance.
				* If the music is available (loaded) and if it is playing, it
				* will be halted.
				* If the music is fading out, then this method will wait
				* (blocking) until the fade out is complete.
				*
				* @return true iff the music had to be actually unloaded
				* (otherwise it was not already available and nothing was done).
				*
				* @throw Ceylan::LoadableException whenever the unloading fails.
				*
				*/
				virtual bool unload() throw( Ceylan::LoadableException ) ;
		



				// Audible implementation.
				
				
				/**
				 * Returns the volume associated to this music instance.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported or if no previously loaded music is
				 * available.
				 *
				 */
				virtual Volume getVolume() const throw( MusicException ) ;
		
		
				/**
				 * Sets the volume associated to this music instance.
				 * Setting the volume during a fade will do nothing.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported or if no previously loaded music is
				 * available.
				 *
				 */
				virtual void setVolume( Volume newVolume ) 
					throw( MusicException ) ;
		
		
				/**
				 * Returns the type of this music (ex: OggVorbis).
				 *
				 * @throw AudioException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual MusicType getType() const throw( AudioException ) ;
		
		
		
				/*
				 * Play section.
				 *
				 * There is no concept of input channel here, as if one was 
				 * reserved for music.
				 *
				 */
				
				
				// Simple play subsection.
				
				
				/**
				 * Plays this music instance at once.
				 * The previous music will be halted, or, if fading out, this
				 * music waits (blocking) for the previous to finish.
				 *
				 * @param playCount the number of times this music should be 
				 * played from start to finish, unless stopped. The previous
				 * music will be halted, or, if fading out, this method waits
				 * (blocking) for that to finish. A count of -1 means forever. 
				 * Otherwise it must be strictly positive (exception thrown if
				 * zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported .
				 *
				 */
				virtual void play( PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 


	
				// Play with fade-in subsection.
		
		
				/**
				 * Plays this music instance at once.
				 * The previous music will be halted, or, if fading out, this
				 * music waits (blocking) for the previous to finish.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume. The fade in effect only applies to the first
				 * loop.
				 *
				 * @param playCount the number of times this music should be 
				 * played from start to finish, unless stopped. The previous
				 * music will be halted, or, if fading out, this method waits
				 * (blocking) for that to finish. A count of -1 means forever. 
				 * Otherwise it must be strictly positive (exception thrown if
				 * zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::System::Millisecond fadeInMaxDuration,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
		
				/**
				 * Plays this music instance at once from specified position.
				 * The previous music will be halted, or, if fading out, this
				 * music waits (blocking) for the previous to finish.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param position the position to play from.
				 *
				 * @param playCount the number of times this music should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed, including
				 * if not supported.
				 *
				 * @see Position for its meaning, which depends on the type of
				 * the music stream.
				 *
				 * @note Cannot be inherited from Audible as they have no
				 * concept of a position.
				 *
				 */
				virtual void playWithFadeInFromPosition( 
						Ceylan::System::Millisecond fadeInMaxDuration,
						MusicPosition position,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		
				
				
				/** 
				 * Pauses the music playback. 
				 * Paused music can be halted.
				 * Music can only be paused if it is actively playing.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void pause() throw( MusicException ) ; 
		
		
				/** 
				 * Unpauses the music.
				 * This is safe to use on halted, paused, and already playing
				 * music.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void resume() throw( MusicException ) ; 
		
		
		
				/** 
				 * Rewinds the music to the start.
				 * This is safe to use on halted, paused, and already playing
				 * music.
				 * It is not useful to rewind the music immediately after
				 * starting playback, because it starts at the beginning by
				 * default. 
				 *
				 * This methods works only for these streams: MOD, OGG, MP3,
				 *  Native MIDI.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void rewind() throw( MusicException ) ; 
		
		
				/** 
				 * Sets the position of the currently playing music.
				 *
				 * @param newPosition the position to play from.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported by OSDL, by the back-end or the underlying
				 * codec.
				 *
				 * @see MusicPosition for its meaning, which depends on the 
				 * type of the music stream.
				 *
				 */
				virtual void setPosition( MusicPosition newPosition ) 
					throw( MusicException ) ; 
		
		
				/** 
				 * Halt playback of music.
				 * This interrupts music fader effects. 
				 *
				 * @note Audio::onMusicPlaybackFinished will be automatically
				 * called when the current music (be it this one or another)
				 * stops.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void halt() throw( MusicException ) ; 

				
				/** 
				 * Gradually fades out the music over specified duration,
				 * starting from now.
				 * Does nothing if not playing or already fading.
				 * The music will be halted after the fade out is completed.
				 *
				 * @param fadeOutMaxDuration duration in milliseconds that the
				 * fade-out effect should take to go to silence, starting now.
				 *
				 * @note Audio::onMusicPlaybackFinished will be automatically
				 * called when the current music (be it this one or another)
				 * stops.
				 *
				 * @throw MusicException if the operation failed, including
				 * if not supported.
				 *
				 */
				virtual void fadeOut( 
						Ceylan::System::Millisecond fadeOutMaxDuration ) 
					throw( MusicException ) ; 

				
			
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
			
			
				/**
				 * Returns the type of the specified music (if parameter is not
				 * null), or the one of the currently played music (if null).
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 * @note Used to avoid code duplication with Audio module,
				 * prefer using directly the getType method.
				 *
				 */
				static MusicType GetTypeOf( const Music * music )
					throw( AudioException ) ;
					
			
			
			protected:
			
			
				/**
				 * The internal low level music is defined through the
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
				explicit Music( const Music & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				Music & operator = ( const Music & source ) throw() ;
				
			
		} ;
		
	}	
	
}	



#endif // OSDL_MUSIC_H_

