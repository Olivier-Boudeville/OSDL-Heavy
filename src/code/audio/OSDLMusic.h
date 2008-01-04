#ifndef OSDL_MUSIC_H_
#define OSDL_MUSIC_H_


#include "OSDLAudioCommon.h"    // for MusicType, etc.
#include "OSDLAudible.h"        // for AudibleException, inheritance


#include "Ceylan.h"             // for LoadableWithContent


#include <string>




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


#if defined(CEYLAN_ARCH_NINTENDO_DS) && CEYLAN_ARCH_NINTENDO_DS == 1

#include "OSDLCommandManager.h" // for friend declaration.

#endif // defined(CEYLAN_ARCH_NINTENDO_DS) && CEYLAN_ARCH_NINTENDO_DS == 1



namespace OSDL
{

		
	
	namespace Audio 
	{

			
		/// For buffer sizes, in bytes (prefer full 32-bit words, faster).
		typedef Ceylan::Uint32 BufferSize ;


#if ! defined(OSDL_USES_SDL_MIXER) || OSDL_USES_SDL_MIXER 

		/// Low-level music being used internally.
		typedef ::_Mix_Music LowLevelMusic ;
		
#else // OSDL_USES_SDL_MIXER	

		
		/**
		 * Double-buffered music.
		 *
		 */
		struct LowLevelMusic
		{
		
			// Music-specific section.
		
			/// The file from which music samples will be streamed:
			Ceylan::System::File * _musicFile ;
			
			/// The music sampling frequency, in Hertz:
			Ceylan::Uint16 _frequency ;

			/// The music bit depth, in bits (8bit/16bit):
			Ceylan::Uint8 _bitDepth ;

			/**
			 * The mode, i.e. the number of channels (mono:1/stereo:2):
			 * (only mono supported currently)
			 *
			 */
			Ceylan::Uint8 _mode ;


			/// The total size of music, in bytes:
			BufferSize _size ;
			
			
			// Buffer-specific section.

			/// The size of a (simple) buffer, in bytes:
			BufferSize _bufferSize ;
			
			/// The smallest upper bound chosen to a MP3 frame size.
			BufferSize _frameSizeUpperBound ;
			
			
			/**
			 * The actual double sound buffer, two simple buffers, one after the
			 * other (so the first half buffer has the same address as this
			 * double one)
			 *
			 */
			Ceylan::Byte * _doubleBuffer ;
		
		
			/// Tells whether the first buffer is filled with unplayed content.
			bool _firstFilled ;
			
			/// Tells how many bytes can be read from first buffer.
			BufferSize _availableInFirst ;
			
			/*
			 * Precomputes the start of the first buffer, after the delta 
			 * zone.
			 *
			 */
			Ceylan::Byte * _startAfterDelta ;
			
			/// Precomputes delta-wise the refill size of first buffer.
			BufferSize _firstActualRefillSize ;
			
			
			/// The address of the second buffer:
			Ceylan::Byte * _secondBuffer ;

			/// Tells whether the second buffer is filled with unplayed content.
			bool _secondFilled ;
		
			/// Tells how many bytes can be read from second buffer.
			BufferSize _availableInSecond ;
			
			
		} ;

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
	
#if defined(CEYLAN_ARCH_NINTENDO_DS) && CEYLAN_ARCH_NINTENDO_DS == 1

			// So that the manager can call onPlaybackEnded and al:
			friend class OSDL::CommandManager ;

#endif // defined(CEYLAN_ARCH_NINTENDO_DS) && CEYLAN_ARCH_NINTENDO_DS == 1
	
			
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
				 * @note On some platforms, like the Nintendo DS, too few
				 * RAM is available to load a full music in it. Thus the music
				 * will always be streamed, and the preload flag will be
				 * ignored. 
				 *
				 * @throw MusicException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Music( const std::string & musicFile, 
						bool preload = true ) throw( MusicException ) ;
				
				
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
				 *
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
				 * On the Nintendo DS, a command manager is expected to be
				 * already available before playback.
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
				 * @param position the position to play from, expressed in
				 * seconds.
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
				 * @note Works only for OggVorbis-encoded musics.
				 *
				 */
				virtual void playWithFadeInFromPosition( 
						Ceylan::System::Millisecond fadeInMaxDuration,
						MusicPosition position,
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) ; 
		

				/**
				 * Tells whether this music is being played.
				 *
				 * @note A paused music is deemed playing.
				 *
				 */
				virtual bool isPlaying() throw() ;
				
				
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
				 * Sets this music as the current one. 
				 *
				 * Any already current music will be replaced immediately, and
				 * be notified of it.
				 *
				 * @see onNoMoreCurrent
				 *
				 */
				virtual void setAsCurrent() throw( AudioException ) ;

			
				/**
				 * Sets a corresponding flag in this music instance to notify
				 * the main loop the first buffer should be refilled.
				 *
				 * Called by the CommandManager from an IRQ.
				 *
				 */
				virtual void requestFillOfFirstBuffer() throw() ;
				
				
				/**
				 * Sets a corresponding flag in this music instance to notify
				 * the main loop the second buffer should be refilled.
				 *
				 * Called by the CommandManager from an IRQ.
				 *
				 */
				virtual void requestFillOfSecondBuffer() throw() ;
				
								
				 
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
				 * Manages the current music, including regarding the refilling
				 * of buffers.
				 *
				 * To be called from the main loop.
				 *
				 */
				static void ManageCurrentMusic() throw( AudioException ) ;
				
				 
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
					
					
				/**
				 * Returns a textual description of the specified music type.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				static std::string DescribeMusicType( MusicType type )
					throw( AudioException ) ;
					
			
			
			protected:
			
			
				/// Tells whether this music is being played.
				bool _isPlaying ;
				
				
				/// To trigger a refill by the main loop.
				bool _requestFillOfFirstBuffer ;
				
				/// To trigger a refill by the main loop.
				bool _requestFillOfSecondBuffer ;
				
			
				/// The music currently being played (if any).
				static Music * _CurrentMusic ;
				
				
				
				/**
				 * Callback automatically called whenever the playback of this
				 * music is over.
				 *
				 * @note Made to be overriden.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual void onPlaybackEnded() throw( AudioException ) ;
				
				
				/**
				 * Callback automatically called whenever this music was the
				 * current one, but it is not anymore.
				 *
				 * @note Made to be overriden.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 * @see setAsCurrent
				 *
				 */
				virtual void onNoMoreCurrent() throw( AudioException ) ;
				
				
				/**
				 * Triggers any buffer refill, if necessary.
				 *
				 * Helper method to be used by the main loop.
				 *
				 * @throw AudioException if a refill failed.
				 *
				 */
				virtual void manageBufferRefill() throw( AudioException ) ;


				
				/**
				 * Returns the first encoded buffer of this music.
				 *
				 * @throw AudioException if the operation failed, including 
				 * if no buffer is available.
				 *
				 */
				//Ceylan::Byte * getFirstBuffer() throw( AudioException ) ;


				/**
				 * Returns the number of bytes that are available to read in 
				 * first encoded buffer.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				//BufferSize getAvailableSizeInFirstBuffer() 
				//	throw( AudioException ) ;
				
				
				/**
				 * Returns the second encoded buffer of this music.
				 *
				 * @throw AudioException if the operation failed, including 
				 * if no buffer is available.
				 *
				 */
				//Ceylan::Byte * getSecondBuffer() throw( AudioException ) ;
				
				
				/**
				 * Returns the number of bytes that are available to read in 
				 * second encoded buffer.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				//BufferSize getAvailableSizeInSecondBuffer() 
				//	throw( AudioException ) ;
				
				
				/**
				 * Fills the first half of the double buffer with
				 * encoded data read from the music file.
				 *
				 * Updates the corresponding size available for reading.
				 *
				 */
				void fillFirstBuffer() throw( AudioException ) ; 
				
				
				/**
				 * Fills the second half of the double buffer with
				 * encoded data read from the music file.
				 *
				 * Updates the corresponding size available for reading.
				 *
				 */
				void fillSecondBuffer() throw( AudioException ) ; 
				
						
				
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

