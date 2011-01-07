/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#ifndef OSDL_AUDIO_CHANNEL_H_
#define OSDL_AUDIO_CHANNEL_H_


#include "OSDLAudioCommon.h" // for AudioException, FadingStatus, etc.

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
			
				AudioChannelException( const std::string & reason ) ; 
				virtual ~AudioChannelException() throw() ; 
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
		class OSDL_DLL AudioChannel : public Ceylan::TextDisplayable
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
				 * @note Usually the user does not create his own channels,
				 * he retrieves them thanks to AudioModule::getMixingChannelAt.
				 *
				 */
				explicit AudioChannel( ChannelNumber channelNumber ) ;
				
				
				
				/// Virtual destructor.
				virtual ~AudioChannel() throw() ;
		
		
		
				/**
				 * Returns the number of this mixing channel.
				 *
				 */				
				virtual ChannelNumber getNumber() const ;
		
		
		
				/**
				 * Returns the volume associated to this channel.
				 *
				 * @throw AudioChannelException if the operation failed.
				 *
				 */
				virtual Volume getVolume() const ;


		
				/**
				 * Sets the volume associated to this channel.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setVolume( Volume newVolume ) ;
		
		
		
				/**
				 * Activates stereo panning and sets the specified distribution
				 * between left/right output channels for this input mixing
				 * channel.
				 * 
				 * @note This effect will only work on stereo audio output. On
				 * mono audio device, nothing will be done.
				 *
				 * @param leftPercentage, between 0 and 100 (%). Right 
				 * percentage will be equal to 100 - leftPercentage.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setPanning( 
					Ceylan::Maths::Percentage leftPercentage ) ;
					
					
					
				/**
				 * Deactivates stereo panning effect for this channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetPanning() ;
		
		

				/**
				 * Activates or deactivates reverse stereo: if activated, 
				 * swaps left and right output for this channel.
				 * 
				 * @note This effect will only work on stereo audio output. On
				 * mono audio device, nothing will be done.
				 *
				 * @param reverse if true, left channel will be output into
				 * right one, and right channel into left one, for this input
				 * mixing channel only.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void setReverseStereo( bool reverse = true ) ;
		
		
		
				/**
				 * Activates distance attenuation for this input mixing
				 * channel.
				 * 
				 * @param distance corresponds to the distance between the 
				 * audio sources (deemed all at the same location) and the
				 * listener. It ranges for 0 (closest possible) to 255 (far).
				 * A distance of 0  unregisters this effect (volume continuity
				 * is then ensured).
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 * @see ListenerDistance
				 *
				 */
				virtual void setDistanceAttenuation( 
					ListenerDistance distance ) ;
					
					
					
				/**
				 * Deactivates distance attenuation effect for this mixing
				 * channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetDistanceAttenuation() ;
		


				/**
				 * Activates attenuation based on distance and angle for 
				 * this input mixing channel.
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
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 * @see ListenerDistance, ListenerAngle
				 *
				 */
				virtual void setPositionAttenuation( ListenerDistance distance,
					ListenerAngle angle )  ;
					
					
					
				/**
				 * Deactivates position attenuation effect for this mixing
				 * channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void unsetPositionAttenuation() ;


		
				/**
				 * Returns true iff this channel is currently playing (not
				 * halted), including if it is playing but paused.
				 *
				 */
				virtual bool isPlaying() const ;
				
				
				
				/**
				 * Returns true iff this channel is currently paused, 
				 * including if it has been halted afterwards.
				 *
				 */
				virtual bool isPaused() const ;
				
				
				
				/**
				 * Returns the fading status of this channel.
				 * 
				 * @note Does not tell whether it is playing anything, or 
				 * paused, etc., it must be tested separately.
				 * 
				 */
				virtual FadingStatus getFadingStatus() const ;
				
				
				
				/**
				 * Pauses this channel.
				 *
				 * You may still halt a paused channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void pause() ;
		
		
		
				/**
				 * Resumes the playing on this supposedly paused channel.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void resume() ;
		
		
		
				/**
				 * Waits (synchronously, i.e. blocks) as long as this
				 * channel is playing.
				 *
				 * @note CPU-friendly busy waiting is performed.
				 *
				 * @throw AudioException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void waitEndOfPlayback() const ;
					

				
				/**
				 * Halts the playing on this channel.
				 *
				 * @note onPlaybackFinished will be automatically called.
				 *
				 * @throw AudioChannelException if the operation failed,
				 * including if not supported.
				 *
				 */
				virtual void halt() ;
		
		
		
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
					Ceylan::System::Millisecond expireDuration ) ;
		
		
		
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
					Ceylan::System::Millisecond fadeOutDuration ) ;
		
					
		
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
				 * This method will be automatically called as soon as this
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
				 * @throw AudioChannelException if the operation failed.
				 *
				 */
				virtual void onPlaybackFinished() ;

			
			
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
				explicit AudioChannel( const AudioChannel & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				AudioChannel & operator = ( const AudioChannel & source ) ;
				
			
		} ;
		
		
	}	
	
}	



#endif // OSDL_AUDIO_CHANNEL_H_

