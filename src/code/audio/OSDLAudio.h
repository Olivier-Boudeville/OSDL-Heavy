#ifndef OSDL_AUDIO_H_
#define OSDL_AUDIO_H_


#include "OSDLException.h"   // for OSDL::Exception 

#include "Ceylan.h"          // for inheritance, Hertz

#include <string>
#include <vector>



namespace OSDL
{

	
	// Manager of the audio module.
	class CommonModule ;
		
	
	namespace Audio 
	{
			
			
		/// Mother class for all audio exceptions. 		
		class OSDL_DLL AudioException: public OSDL::Exception 
		{ 
			public: 
			
				AudioException( const std::string & reason ) throw() ; 
				virtual ~AudioException() throw() ; 
		} ;
			
		
		
		// The audio module uses channels to mix input sources.
		class AudioChannel ;
				
				
		/**
		 * Describes the encoding (format) of a given sample.
		 * A sample has a size (8-bit or 16-bit generally), is signed or not,
		 * and, if using more than one byte, an endianness (little or big).
		 *
		 * @example: Uint8SampleFormat, LittleSint16SampleFormat, etc.
		 *
		 */
		typedef Ceylan::Uint16 SampleFormat ;
	
	
		/**
		 * Returns a textual description of the specified sample format.
		 *
		 * @throw AudioException if the format is not known.
		 *
		 */
		std::string sampleFormatToString( SampleFormat format )
			throw( AudioException ) ;
			
			
		
		/**
		 * Specifies the channel organization of an audio output.
		 * 
		 * It includes the number of channels (ex: mono, stereo) and the role
		 * of each channel.
		 *
		 * @example: 
		 *
		 * @note This has nothing to do with mixing (input) channels.
		 *
		 */
		typedef Ceylan::Uint16 ChannelFormat ;

	
		/**
		 * Returns a textual description of the specified channel format.
		 *
		 * @throw AudioException if the format is not known.
		 *
		 */
		std::string channelFormatToString( ChannelFormat format )
			throw( AudioException ) ;

	
		/**
		 * Describe a number of channel, either for input (mixing channels) or
		 * for output (playback channels of a given channel format).
		 *
		 * @example: there can be 16 input mixing channels. Regarding output,
		 * for mono sound, there is 1 playback channel, and for stereo, 2.
		 *
		 */
		typedef Ceylan::Uint16 ChannelNumber ;
	
	
		/**
		 * Describes the size, in bytes, of a sample chunk.
		 *
		 */
		typedef Ceylan::Uint32 ChunkSize ;
		
		
		/**
		 * Describes the volume for the playback of an audible.
		 *
		 * Ranges from MinVolume (0) to MaxVolume (128).
		 *
		 */
		typedef Ceylan::Uint8 Volume ;
		
		
								
		/**
		 * Root module for all audio services.
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
			OSDL_DLL friend AudioModule & getExistingAudioModule() throw() ;
		
		
		
			public:



				// Sample format section.
				
				
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
						ChannelNumber        mixingChannelNumber = 16 ) 
					throw( AudioException ) ;


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
				 * channels obtained (note that is not the channel format)
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
						ChannelNumber & actualOutputChannelNumber ) 
					throw( AudioException ) ;
				 
				 
				/**
				 * Unsets an audio mode previously set.
				 *
				 * Stops the internal mixer.
				 *
				 * @throw AudioException of the operation failed.
				 *
				 */
				virtual void unsetMode() throw( AudioException ) ; 





				// Channel section.



				/**
				 * Returns the number of input mixing channels.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual ChannelNumber getMixingChannelCount() const 
					throw( AudioException ) ; 


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
					const throw( AudioException ) ; 


				/**
				 * Sets the volume for all the input mixing channels at once.
				 *
				 * @param newVolume the volume to set all channels to.
				 * 				 
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual void setVolumeForAllMixingChannels( Volume newVolume )
					throw( AudioException ) ;
					
				
				/**
				 * Returns the number of channels being currently playing
				 * (including those which are paused).
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual ChannelNumber getPlayingChannelCount() const 
					throw( AudioException ) ;
					
					
				/**
				 * Returns the number of channels being currently paused
				 * (including those which were paused and halted afterwards).
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual ChannelNumber getPausedChannelCount() const 
					throw( AudioException ) ;
					
					
				/**
				 * Pauses all mixing channels being actively playing.
				 *
				 * A paused channel can still be halted.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void pauseAllChannels() throw( AudioException ) ;
					
					
				/**
				 * Resumes the playing on all paused mixing channels.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void resumeAllChannels() throw( AudioException ) ;
					
					
				/**
				 * Halts the playing on all mixing channels.
				 *
				 * @throw AudioException if the operation failed or is not 
				 * supported.
				 *
				 */
				virtual void haltAllChannels() throw( AudioException ) ;
					
					
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
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual ChannelNumber expireAllChannelsIn( 
						Ceylan::System::Millisecond expireDuration )
					throw( AudioException ) ;
		
		
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
						Ceylan::System::Millisecond fadeOutDuration )
					throw( AudioException ) ;
					
					
					
					
				/**
				 * Returns the name of the audio driver being currently 
				 * used (example: 'dsp').
				 *
				 * @see GetDriverName
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				virtual std::string getDriverName() const 
					throw( AudioException ) ; 
		
			
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
			
			
			
				
				// Static section.
	
	
				/**
				 *
				 *
				 *
				 *
				 */
				
				 
				/**
				 * Returns a summary about the possible use of 
				 * video-related environment variables, for the selected
				 * back-end, expressed in specified format.
				 *
				 * @note The SDL back-end can be partly driven by a set 
				 * of environment variables.
				 *
				 */	
				static std::string DescribeEnvironmentVariables() throw() ;	
					
				
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
				static bool IsAudioInitialized() throw() ;
					

				/**
				 * Returns the name of the audio driver being currently 
				 * used (example: 'dsp').
				 *
				 * @see getDriverName.
				 *
				 * @throw AudioException if the operation failed.
				 *
				 */
				static std::string GetDriverName() throw( AudioException ) ;
				
				
				/// The maximum length for the name of the audio driver.
				static const Ceylan::Uint16 DriverNameMaximumLength ;
				
				
					
			protected:
	
				
				/// Tells whether the internal mixer is set.
				bool _mixerInitialized ;
				
				
				/// The number of bytes of a mixing chunk.
				ChunkSize _chunkSize ;
				
				
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
				static ChannelNumber GetChannelCountFor( ChannelFormat format )
					throw( AudioException ) ;
	
	
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
				static ChunkSize GetSampleSizeFor( SampleFormat format )
					throw( AudioException ) ;
	


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
				 * Private constructor to be sure it will not be implicitly
				 * called. Called by the CommonModule when audio is requested.
				 *
				 * @throw AudioException if the audio subsystem 
				 * initialization failed.
				 *
				 */
				AudioModule() throw( AudioException ) ;


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
				explicit AudioModule( const AudioModule & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				AudioModule & operator = ( const AudioModule & source ) 
					throw() ;
	


		
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
		 * If not, a fatal error will be triggered (not an exception to 
		 * avoid handling it in all user methods).
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
		OSDL_DLL AudioModule & getExistingAudioModule() throw() ;
		
		
	}	
	
}	



#endif // OSDL_AUDIO_H_

