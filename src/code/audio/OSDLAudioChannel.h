#ifndef OSDL_AUDIO_CHANNEL_H_
#define OSDL_AUDIO_CHANNEL_H_


#include "OSDLAudio.h"       // for AudioException

#include "Ceylan.h"          // for inheritance

#include <string>




namespace OSDL
{

	
		
	
	namespace Audio 
	{
			


			
		/// Exceptions raised by AudioChannel instances. 		
		class OSDL_DLL AudioChannelException: public AudioException 
		{ 
			public: 
			
				AudioChannelException( const std::string & reason ) throw() ; 
				virtual ~AudioChannelException() throw() ; 
		} ;
			
			
			
		/// Describes a fading status.	
		enum FadingStatus
		{
		
			In,
			Out,
			None
			
		} ;
			
				
												
		/**
		 * Corresponds to a mixing audio channel.
		 *
		 * Audio channel instances are owned by the audio module.
		 *
		 * On a given channel, up to one sample can be played at a time.
		 *
		 * @note All play* methods of Sound and Music could be accessible from
		 * channel instances as well.
		 *
		 */
		class OSDL_DLL AudioChannel: public Ceylan::TextDisplayable
		{
		
		
			public:
				

				/**
				 * Creates a new audio channel instance.
				 *
				 * On creation, the channel volume is the maximum one.
				 *
				 * @param channelNumber the number of the channel.
				 *
				 * @throw AudioChannelException if the operation failed or 
				 * is not supported.
				 *
				 */
				explicit AudioChannel( ChannelNumber channelNumber )
					throw( AudioChannelException ) ;
				
				
				/// Virtual destructor.
				virtual ~AudioChannel() throw() ;
		
		
				/**
				 * Returns the number of this mixing channel.
				 *
				 */				
				virtual ChannelNumber getNumber() const throw() ;
		
		
		
				/**
				 * Returns the volume associated to this channel.
				 *
				 * @throw AudioChannelException if the operation failed.
				 *
				 */
				virtual Volume getVolume() const 
					throw( AudioChannelException ) ;

		
				/**
				 * Sets the volume associated to this channel.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setVolume( Volume newVolume ) 
					throw( AudioChannelException ) ;
		
		
		
				/**
				 * Returns true iff this channel is currently playing (not
				 * halted), including if it is playing but paused.
				 *
				 */
				virtual bool isPlaying() const throw() ;
				
				
				/**
				 * Returns true iff this channel is currently paused, 
				 * including if it has been halted afterwards.
				 *
				 */
				virtual bool isPaused() const throw() ;
				
				
				/**
				 * Returns the fading status of this channel.
				 * 
				 * @note Does not tell whether it is playing anything, or 
				 * paused, etc., it must be tested separately.
				 * 
				 */
				virtual FadingStatus getFadingStatus() const throw() ;
				
				
				/**
				 * Pauses this channel.
				 *
				 * You may still halt a paused channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void pause() throw( AudioChannelException ) ;
		
		
				/**
				 * Resumes the playing on this supposedly paused channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void resume() throw( AudioChannelException ) ;
		
				
				/**
				 * Halts the playing on this channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void halt() throw( AudioChannelException ) ;
		
		
				/**
				 * Makes that channel stop after the specified duration is
				 * elapsed.
				 *
				 * @param expireDuration the duration, in milliseconds, until
				 * this channel is stopped.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void expireIn( 
						Ceylan::System::Millisecond expireDuration )
					throw( AudioChannelException ) ;
		
		
				/**
				 * Makes that channel stop after the specified duration is
				 * elapsed.
				 *
				 * @param fadeOutDuration the duration, in milliseconds, of the
				 * fade-out, which will start immediately.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void fadeOutDuring( 
						Ceylan::System::Millisecond fadeOutDuration )
					throw( AudioChannelException ) ;
		
					
		
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
				 * Tbis method will be automatically called as soon as this
				 * channel stops playback.
				 *
				 * This method is meant to be overriden, in order to be able 
				 * to react when this event occurs (like a call-back).
				 *
				 * @note Never call back-end functions (ex: SDL or SDL_mixer)
				 * from the callback.
				 *
				 * This default implementation is do-nothing.
				 *
				 */
				virtual void onPlaybackFinished()
					throw( AudioChannelException ) ;

			
				/// The number of this channel in channel list.
				ChannelNumber _channelNumber ;
				
				
				
			private:


				/**
				 * Callback to catch the end of a channel playback.
				 *
				 * This signature is mandatory.
				 *
				 */
				static void HandleFinishedPlaybackCallback( 
					int channelNumber ) ;
				

	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit AudioChannel( const AudioChannel & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				AudioChannel & operator = ( const AudioChannel & source )
					throw() ;
				
			
		} ;
		
	}	
	
}	



#endif // OSDL_AUDIO_CHANNEL_H_

