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


#ifndef OSDL_AUDIO_H_
#define OSDL_AUDIO_H_


#include "OSDLAudioCommon.h" // for AudioException, etc.
#include "OSDLMusic.h"       // for MusicType, etc.

#include "Ceylan.h"          // for inheritance, Hertz

#include <string>
#include <vector>




namespace OSDL
{

	
	
	// Manager of the audio module.
	class CommonModule ;
		
	
	
	namespace Audio 
	{
			
			
		
		// The audio module uses channels to mix input sources.
		class AudioChannel ;
					
	
	
		/**
		 * Returns a textual description of the specified sample format.
		 *
		 * @throw AudioException if the format is not known.
		 *
		 */
		std::string sampleFormatToString( SampleFormat format ) ;
			
			
	
		/**
		 * Returns a textual description of the specified channel format.
		 *
		 * @throw AudioException if the format is not known.
		 *
		 */
		std::string channelFormatToString( ChannelFormat format ) ;

	
	
								
		/**
		 * Root module for all audio services.
		 * 
		 * The audio module will manage as well the music manager.
		 *
		 * @see OSDL site for more information on sound: add 
		 * main/documentation/rendering/SDL-audio.html to the base URL
		 * (http://osdl.sourceforge.net).
		 *
		 */
		class OSDL_DLL AudioModule: public Ceylan::Module
		{
		
		
		
			// The common module has to create the audio module.
			friend class OSDL::CommonModule ;
		
			
		
			/**
			 * This friend function is intended to be the usual means of
			 * getting a reference to the audio module, which must already
			 * exist. It is called in contexts where this module has to exist.
			 *
			 * If not, a fatal error will be triggered (not an exception to 
			 * avoid handling it in all user methods).
			 * 
			 * @note This function is mainly useful for the OSDL internals, 
			 * for example for channel callbacks.
			 *
			 * @note This method is not static to avoid pitfalls of static
			 * initializer ordering.
			 *
			 */			
			OSDL_DLL friend AudioModule & getExistingAudioModule() ;
		
		
		
			public:



				/*
				 * Sample format section.
				 *
				 * Unless specified otherwise, sample formats are PCM-based.
				 *
				 */
				
				
				/// Unsigned 8-bit sample.
				static const SampleFormat Uint8SampleFormat ;
				
				
				/// Signed 8-bit sample.
				static const SampleFormat Sint8SampleFormat ;
				

				
				/// Unsigned 16-bit sample, little-endian byte order.
				static const SampleFormat LittleUint16SampleFormat ;


				/// Signed 16-bit sample, little-endian byte order.
				static const SampleFormat LittleSint16SampleFormat ;



				/// Unsigned 16-bit sample, big-endian byte order.
				static const SampleFormat BigUint16SampleFormat ;
			
			
				/// Signed 16-bit sample, big-endian byte order.
				static const SampleFormat BigSint16SampleFormat ;
			
			
			
				/// Unsigned 16-bit sample, native byte order (big or little).
				static const SampleFormat NativeUint16SampleFormat ;
			
			
				/// Signed 16-bit sample, native byte order (big or little).
				static const SampleFormat NativeSint16SampleFormat ;
			
			
			
				/// IMA ADPCM (not Microsoft) sample.
				static const SampleFormat IMAADPCMSampleFormat ;
			




				// Channel format section.
				
			
			
				/// Monophonic (one channel).
				static const ChannelFormat Mono ;


				/// Stereophonic (two channels).
				static const ChannelFormat Stereo ;
				
				

				/// The smallest volume possible (mute).
				static const Volume MinVolume ;
				
				
				/// The highest volume possible (full throttle).
				static const Volume MaxVolume ;



				/// Allows to store the number (identifier) of a mixing channel.
				typedef Ceylan::Uint16 ChannelNumber ;
				
				
								
				/**
				 * Tries to sets up an audio mode with the specified output
				 * sampling frequency, output sample format, number of sound
				 * channels in output and bytes used per output sample.
				 *
				 * Initializes the internal mixer.
				 *
				 * The specified settings may or may not be obtained. 
				 * @see getObtainedMode to know the actual settings having been
				 * set.
				 *
				 * @note This specifies how the overall final mixed sound will
				 * be output to the audio card. This does not imply that the
				 * mixed channels have to follow the same settings (even if it
				 * would be generally interesting to do so to avoid 
				 * conversions).
				 *
				 * @param outputFrequency the output sampling frequency, in
				 * samples per second (Hz). A common frequency is 22 050 Hz,
				 * usually a good choice for games. 44 100 Hz (i.e. 44.1 kHz,
				 * the CD audio rate) requires a lot of CPU power and bus
				 * bandwidth.
				 *
				 * @param outputSampleFormat the sample format to output, i.e.
				 * the encoding of the final samples sent to the audio card
				 * (ex: 16-bit little endian unsigned samples, i.e.
				 * LittleUint16SampleFormat).
				 *
				 * @param outputChannel the channel output format, i.e.
				 * the number and role of the output streams to target 
				 * (ex: stereo).
				 *
				 * @param outputBufferSize the size, in bytes, of the resulting
				 * mixed buffer. Divide this size by the size of a sample
				 * (corresponding to the specified sample format), and you
				 * obtain the number of samples in the output buffer. Choosing
				 * the right buffer size is a tricky trade-off: if too small,
				 * the callback for sound mixing may be called too frequently
				 * for the running platform to catch the pace, and sound may
				 * skip. If too big, latency might become noticeable: sound
				 * effects will lag behind the action more. For music output 
				 * only, the buffer may be chosen quite big (say, 2048 samples
				 * at 22 050 Hz for each channel). 
				 *
				 * @param mixingChannelNumber the number of mixer (input)
				 * channels that must be created.
				 *
				 * @throw AudioException if the setting of the new mode
				 * failed, including if the operation is not supported.
				 *
				 */			
				virtual void setMode( 
					Ceylan::Maths::Hertz outputFrequency,
					SampleFormat         outputSampleFormat,
					ChannelFormat        outputChannel,
					ChunkSize            outputBufferSize,
					ChannelNumber        mixingChannelNumber = 16 ) ;



				/**
				 * Returns in specified parameters the actual settings of the
				 * mixer.
				 *
				 * @param actualOutputFrequency will be set to the output
				 * sampling frequency, in samples per second (Hz).
				 *
				 * @param actualOutputSampleFormat will be set to the sample
				 * format that will be output.
				 *
				 * @param actualOutputChannelNumber will be set to the number of
				 * output channels obtained, usually one, for mono, or two,
				 * for stereo (note that is not the channel format).
				 *
				 * @return the mean latency, in milliseconds, that corresponds
				 * to the actual settings and to the chunk size.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual Ceylan::System::Millisecond getObtainedMode( 
					Ceylan::Maths::Hertz & actualOutputFrequency,
					SampleFormat & actualOutputSampleFormat,
					ChannelNumber & actualOutputChannelNumber ) ;
				 
				 
				 
				/**
				 * Unsets an audio mode previously set.
				 *
				 * Stops the internal mixer.
				 *
				 * @throw AudioException of the operation failed.
				 *
				 */
				virtual void unsetMode() ; 


					
				/**
				 * Activates stereo panning and sets the specified distribution
				 * between left/right output channels for the overall mixing
				 * output.
				 * All input mixing channels and the music output will be
				 * therefore panned that same way.
				 * 
				 * @note This effect will only work on stereo audio output. On
				 * mono audio device, nothing will be done.
				 *
				 * @param leftPercentage, between 0 and 100 (%). Right 
				 * percentage will be equal to 100 - leftPercentage.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setPanning( 
					Ceylan::Maths::Percentage leftPercentage ) ;
					
					
					
				/**
				 * Deactivates stereo panning effect.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetPanning()	;



				/**
				 * Activates or deactivates reverse stereo: if activated, 
				 * swaps left and right channel output channels.
				 * 
				 * @note This effect will only work on stereo audio output. On
				 * mono audio device, nothing will be done.
				 *
				 * @param reverse if true, left channel will be output into
				 * right one, and right channel into left one.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setReverseStereo( bool reverse = true ) ;
					

					
				/**
				 * Activates distance attenuation for the overall mixing
				 * output.
				 * All input mixing channels and the music output will be
				 * therefore attenuated that same way.
				 * 
				 * @param distance corresponds to the distance between the 
				 * audio sources (deemed all at the same location) and the
				 * listener. It ranges for 0 (closest possible) to 255 (far).
				 * A distance of 0  unregisters this effect (volume continuity
				 * is then ensured).
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 * @see ListenerDistance
				 *
				 */
				virtual void setDistanceAttenuation( 
					ListenerDistance distance ) ;
					
				
					
				/**
				 * Deactivates distance attenuation effect for the overall
				 * mixing output.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetDistanceAttenuation() ;



				/**
				 * Activates attenuation based on distance and angle for the
				 * overall mixing output.
				 * All input mixing channels and the music output will be
				 * therefore attenuated that same way.
				 * 
				 * @param distance corresponds to the distance between the 
				 * audio sources (deemed all at the same location) and the
				 * listener. It ranges for 0 (closest possible) to 255 (far).
				 *
				 * @param angle corresponds to the angle between the 
				 * audio sources (deemed all at the same location) and the
				 * listener. Larger angles will be reduced to the [0..360[
				 * range using the 'modulo 360' (angle % 360 ) operator.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 * @see ListenerDistance, ListenerAngle
				 *
				 */
				virtual void setPositionAttenuation( ListenerDistance distance,
					ListenerAngle angle ) ;
					
					
					
				/**
				 * Deactivates position attenuation effect for the overall
				 * mixing output.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetPositionAttenuation() ;





				// Channel section.



				/**
				 * Returns the number of input mixing channels.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual ChannelNumber getMixingChannelCount() const ; 



				/**
				 * Returns the specified input mixing channel.
				 *
				 * @param index the number of the target channel.
				 *
				 * @throw AudioException if the operation failed, including if
				 * index is out of bounds.
				 *
				 */
				virtual AudioChannel & getMixingChannelAt( ChannelNumber index )
					const ; 



				/**
				 * Sets the volume for all the input mixing channels at once.
				 *
				 * @param newVolume the volume to set all channels to.
				 * 				 
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual void setVolumeForAllMixingChannels( Volume newVolume ) ;
					
				
				
				/**
				 * Returns the number of channels being currently playing
				 * (including those which are paused).
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual ChannelNumber getPlayingChannelCount() const ;
					
					
					
				/**
				 * Returns the number of channels being currently paused
				 * (including those which were paused and halted afterwards).
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual ChannelNumber getPausedChannelCount() const  ;
					
					
					
				/**
				 * Pauses all mixing channels being actively playing.
				 *
				 * A paused channel can still be halted.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void pauseAllChannels() ;
					
					
					
				/**
				 * Resumes the playing on all paused mixing channels.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void resumeAllChannels() ;
					
					
					
				/**
				 * Halts the playing on all mixing channels.
				 *
				 * @note AudioChannel::onPlaybackFinished will be 
				 * automatically called on halted channels.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void haltAllChannels() ;
					
					
					
				/**
				 * Makes all mixing channels stop after the specified 
				 * duration is elapsed.
				 *
				 * @param expireDuration the duration, in milliseconds, until
				 * the channels are stopped.
				 *
				 * @return the number of channels set to expire, whether or 
				 * not they are active.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual ChannelNumber expireAllChannelsIn( 
					Ceylan::System::Millisecond expireDuration ) ;
		
		
		
				/**
				 * Makes all mixing channels fade-out during the specified
				 * duration elapsed.
				 *
				 * @param fadeOutDuration the duration, in milliseconds, of the
				 * fade-out, which will start immediately.
				 *
				 * @return the number of channels set to fade-out, whether or 
				 * not they are active.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual ChannelNumber fadeOutAllChannelsDuring( 
					Ceylan::System::Millisecond fadeOutDuration ) ;
		
					
					
					
				
				// Music section.
				
				
				
				/**
				 * Returns the type of the music currently played (if any),
				 * otherwise returns NoMusic.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual MusicType getTypeOfCurrentMusic() const ;
					
					
				
				/**
				 * Returns whether music is actively playing.
				 *
				 * @note Does not check if the music has been paused.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual bool isMusicPlaying() const ;



				/**
				 * Returns whether music is paused.
				 *
				 * @note Does not check if the music has been halted after it
				 * was paused.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual bool isMusicPaused() const ;



				/**
				 * Returns whether music is fading in, out, or not at all. 
				 *
				 * @note Does not tell if the channel is playing anything, or
				 * paused, so that must be tested separately.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual FadingStatus getMusicFadingStatus() const ;
				
				
				
				/**
				 * Returns the name of the audio driver being currently 
				 * used (example: 'dsp').
				 *
				 * @see GetDriverName
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual std::string getDriverName() const ; 
		
		
			
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
			
			
			
			
				
				// Static section.
				
					
					
					
				// Audio locator section.
			
			

				/**
				 * The name of the environment variable that may 
				 * contain directory names that should contain audio
				 * files.
				 *
				 */
				static std::string AudioPathEnvironmentVariable  ;
				 
				 
				 
				/**
				 * Allows to keep track of audio directories.
				 *
				 * Automatically gathers the list of directories
				 * specified as the value of the environment 
				 * variable named as specified in 
				 * AudioPathEnvironmentVariable (AUDIO_PATH).
				 * 
				 */
				static Ceylan::System::FileLocator AudioFileLocator ;
				
				
				
				/**
				 * Returns a summary about the possible use of 
				 * audio-related environment variables, for the selected
				 * back-end, expressed in specified format.
				 *
				 * @note The SDL back-end can be partly driven by a set 
				 * of environment variables.
				 *
				 */	
				static std::string DescribeEnvironmentVariables() ;	
					
				
				
				/**
				 * Tells whether audio has already been initialized.
				 *
				 * @note This method is static so that calling it is 
				 * convenient: no need to explicitly retrieve the common
				 * module, then audio module before knowing the result. 
				 *
				 * The need to retrieve the right module from scratch at 
				 * each call is rather inefficient though.
				 *
				 */
				static bool IsAudioInitialized() ;
					


				/**
				 * Returns the name of the audio driver being currently 
				 * used (example: 'dsp').
				 *
				 * @see getDriverName.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				static std::string GetDriverName() ;
				
				
				
				/// The maximum length for the name of the audio driver.
				static const Ceylan::Uint16 DriverNameMaximumLength ;
				
				
					
					
			protected:
	
				
				
				/// Tells whether the internal mixer is set.
				bool _mixerInitialized ;
				
				
				/// The number of bytes of a mixing chunk.
				ChunkSize _chunkSize ;
				
				
				/**
				 * Tells whether the audio module controls the overall music
				 * manager.
				 *
				 */
				bool _controlMusicManager ;
				
				
/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable: 4251 )
			

				/// The input mixing channels.
				std::vector<AudioChannel *> _inputChannels ;				
				
#pragma warning( pop ) 
				
				
				
				/**
				 * This method will be automatically called as soon as the
				 * music playback stops.
				 *
				 * This method is meant to be overriden, in order to be able 
				 * to react when this event occurs (like a call-back).
				 *
				 * @note Never call back-end functions (ex: SDL or SDL_mixer)
				 * from the callback.
				 *
				 * This default implementation is do-nothing.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual void onMusicPlaybackFinished() ;
			
					
				
				/**
				 * Returns the number of channels corresponding to the specified
				 * channel format.
				 *
				 * @param format the channel format whose number of channels 
				 * is to be determined.
				 *
				 * @return the corresponding number of channels.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				static ChannelNumber GetChannelCountFor( ChannelFormat format );
	
	
	
				/**
				 * Returns the number of bytes corresponding a sample in
				 * specified sample format.
				 *
				 * @param format the sample format whose size in bytes
				 * is to be determined.
				 *
				 * @return the corresponding number of bytes.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				static ChunkSize GetSampleSizeFor( SampleFormat format ) ;
	


				/**
				 * Keeps tracks of audio initialization calls.
				 *
				 * @note This is for the audio subsystem, not for the mixer.
				 *
				 * The mixer to be set needs this audio subsystem to be on.
				 *
				 */
				static bool _AudioInitialized ;




			private:



				/**
				 * Callback to catch the end of the music playback.
				 *
				 * This signature is mandatory.
				 *
				 */
				static void HandleMusicPlaybackFinishedCallback() ;


	
				/**
				 * Private constructor to be sure it will not be implicitly
				 * called. Called by the CommonModule when audio is requested.
				 *
				 * @throw AudioException if the audio subsystem 
				 * initialization failed.
				 *
				 */
				AudioModule() ;



				/// Virtual destructor ensuring correct audio clean-up.
				virtual ~AudioModule() throw() ;
	
	
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit AudioModule( const AudioModule & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				AudioModule & operator = ( const AudioModule & source ) ;
	


		
				/**
				 * Array of all known environment variables related to audio,
				 * for SDL back-end.
				 *
				 */
				static const std::string SDLEnvironmentVariables[] ;
							
							
				// Common module must be able to create the audio module.		
				friend class CommonModule ;
			
			
			
		} ;

	
	
	
		/**
		 * This function is intended to be the usual means of
		 * getting a reference to the audio module, which must already exist.
		 * If not, an exception will be thrown.
		 * 
		 * @note This function is mainly useful for the OSDL internals, 
		 * if sub-modules, such as the event module, needed access to 
		 * the common module.
		 *
		 * @see hasCommonModule()
		 * 
		 * @note This method is not static to avoid pitfalls of static
		 * initializer ordering.
		 *
		 */			
		OSDL_DLL AudioModule & getExistingAudioModule() ;
		
		
		
	}	
	
	
}	



#endif // OSDL_AUDIO_H_

