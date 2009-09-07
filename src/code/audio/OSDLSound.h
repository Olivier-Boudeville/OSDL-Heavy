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


#ifndef OSDL_SOUND_H_
#define OSDL_SOUND_H_


#include "OSDLAudible.h"     // for AudibleException, inheritance
#include "OSDLUtils.h"          // for Datastream, no easy forward declaration


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
		
		
			/// The sound sampling frequency, in Hertz:
			Ceylan::Uint16 _frequency ;


			/// The sound bit depth, in bit (8bit/16bit):
			Ceylan::Uint8 _bitDepth ;


			/// The mode, i.e. the number of channels (mono:1/stereo:2):
			Ceylan::Uint8 _mode ;
			
			
			/// The actual sound data:
			Ceylan::Byte * _samples ;
		
		
			/// The size, in bytes, of sound data:
			Ceylan::Uint32 _size ;
			
					 
		} ;
		

#endif // OSDL_USES_SDL_MIXER



			
		/// Exceptions raised by Sound instances. 		
		class OSDL_DLL SoundException: public AudibleException 
		{ 
			public: 
			
				SoundException( const std::string & reason ) ; 
				virtual ~SoundException() throw() ; 
		} ;
			
			
				
		// Forward-declaration for next counted pointer:
		class Sound ;
		
		
		/// Sound counted pointer.
		typedef Ceylan::CountedPointer<Sound> SoundCountedPtr ;
			
			
				
												
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
		 * On the PC platform, sounds can be either WAV or OggVorbis.
		 *
		 * In the embedded case (Nintendo DS platform), sounds can be either
		 * RAW with OSDL header, or MP3.
		 *
		 * Sounds, as opposed to musics, are read once for all, as a whole,
		 * in memory, not streamed chunk after chunk from the media. 
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
				 * WAVE and OGG are recommended for sounds. 
				 * On the Nintendo DS, only one format is supported, the 
				 * .osdl.sound format.
				 *
				 * @see trunk/tools/mediaraw2osdlsound.cc for a description.
				 *
				 * @param preload the sound will be loaded directly by this
				 * constructor iff true, otherwise only its path will be
				 * stored to allow for a later loading.
				 *
				 * @throw SoundException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Sound( const std::string & soundFile, 
					bool preload = true ) ;
				
				
				
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
				virtual bool load() ;
		
		
		
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
				virtual bool unload() ;
		



				// Audible implementation.
				
				
				
				/**
				 * Returns the volume associated to this sound instance.
				 *
				 * @throw SoundException if the operation failed, including
				 * if not supported or if no previously loaded sound is
				 * available.
				 *
				 */
				virtual Volume getVolume() const ;
		
		
		
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
				virtual void setVolume( Volume newVolume ) ;
		
		
		
		
				/*
				 * Play section.
				 *
				 * Each subsection includes three methods:
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
				virtual void play( PlaybackCount playCount = 1 ) ; 



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
					PlaybackCount playCount = 1 ) ; 



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
					PlaybackCount playCount = 1 ) ; 




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
					PlaybackCount playCount = 1 ) ; 
	
	
		
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
					PlaybackCount playCount = 1 ) ; 
	
	
	
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
					PlaybackCount playCount = 1 ) ; 
		
		
	
		
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
					PlaybackCount playCount = 1 ) ; 
		
		
		
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
					PlaybackCount playCount = 1 ) ; 
		
		
				
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
					PlaybackCount playCount = 1 ) ; 




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
					PlaybackCount playCount = 1 ) ; 



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
					PlaybackCount playCount = 1 ) ; 



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
					PlaybackCount playCount = 1 ) ; 
		
		
		
		
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
			
			
				/**
				 * The internal low level sound is defined through the
				 * template.
				 *
				 */
				
				
				/**
				 * The datastream corresponding to this sound.
				 *
				 */
				OSDL::Utils::DataStream * _dataStream ;
			
			
			
			
			private:

	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Sound( const Sound & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				Sound & operator = ( const Sound & source ) ;
				
			
		} ;
		
		
	}	
	
}	



#endif // OSDL_SOUND_H_

