#ifndef OSDL_AUDIBLE_H_
#define OSDL_AUDIBLE_H_


#include "OSDLAudio.h"       // for AudioException

#include "Ceylan.h"          // for inheritance, Millisecond, etc.

#include <string>



namespace OSDL
{

	
		
	
	namespace Audio 
	{
			
			
		/// Mother class for all exceptions raised by Audible instances. 		
		class OSDL_DLL AudibleException: public AudioException 
		{ 
			public: 
			
				AudibleException( const std::string & reason ) throw() ; 
				virtual ~AudibleException() throw() ; 
		} ;
			
		
		
		/**
		 * Stores the number of times an Audible should be played.
		 * A value of -1 means forever (infinite loop).
		 * Values below -1 have no meaning and are ignored.
		 *
		 */
		typedef Ceylan::Sint32 PlaybackCount ;		
		
												
												
		/**
		 * Mother class of all audible content, including sound and music.
		 *
		 */
		class OSDL_DLL Audible: public Ceylan::TextDisplayable
		{
		
		
		
			public:
				

				/**
				 * Creates a new audible instance.
				 *
				 * @param convertedToOutputFormat must be true iff the internal
				 * samples of this audible have been converted already to the
				 * sample format used for audio output.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				explicit Audible( bool convertedToOutputFormat = true ) 
					throw( AudibleException ) ;
				
				
				/// Virtual destructor.
				virtual ~Audible() throw() ;
		
		
		
				/**
				 * Returns the volume associated to this audible instance.
				 *
				 */
				virtual Volume getVolume() const throw() = 0 ;
		
		
				/**
				 * Sets the volume associated to this audible instance.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 */
				virtual void setVolume( Volume newVolume ) throw() = 0 ;
		
		
		
		
				// Play section.
				
				
				/**
				 * Plays this audible instance at once on any appropriate output
				 * (ex: channel number).
				 *
				 * @param playCount the number of times this audible should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void play( PlaybackCount playCount = 1 ) 
					throw( AudibleException ) = 0 ; 
	
				
				/**
				 * Plays this audible instance at once on any appropriate output
				 * (ex: channel number) within specified duration.
				 *
				 * @param maxDuration the maximum duration during which this
				 * audible will be played, in milliseconds. 
				 * It may stop earlier if:
				 * (audible duration).playCount < maxDuration
				 *
				 * @param playCount the number of times this audible should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void playForAtMost( 
						Ceylan::System::Millisecond maxDuration, 
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) = 0 ; 
				
				
				/**
				 * Plays this audible instance at once on any appropriate output
				 * (ex: channel number), beginning with a fade-in effect.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this audible should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void playWithFadeIn( 
						Ceylan::System::Millisecond fadeInMaxDuration, 
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) = 0 ; 
					
					
				/**
				 * Plays this audible instance at once on any appropriate output
				 * (ex: channel number), beginning with a fade-in effect.
				 *
				 * @param playbackMaxDuration the maximum duration during which
				 * this audible will be played, in milliseconds.
				 *
				 * @param fadeInMaxDuration duration in milliseconds during
				 * which the fade-in effect should take to go from silence to
				 * full volume.
				 *
				 * @param playCount the number of times this audible should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void playWithFadeInForAtMost( 
						Ceylan::System::Millisecond playbackMaxDuration,
						Ceylan::System::Millisecond fadeInMaxDuration, 
						PlaybackCount playCount = 1 ) 
					throw( AudibleException ) = 0 ; 
				
				
				
				
				/**
				 * Tells whether the internal samples of this audible have 
				 * already been converted to the sample format used for audio
				 * output.
				 * 
				 * @return true iff the samples are already converted.
				 *
				 */
				virtual bool isConvertedToOutputFormat() const throw() ;
				
				
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
				 * Tells whether the internal samples have been converted
				 * already to the sample format used for audio output.
				 *
				 */
				bool _convertedToOutputFormat ;
			
			
			
			
			private:
	
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Audible( const Audible & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				Audible & operator = ( const Audible & source ) throw() ;
				
			
		} ;
		
	}	
	
}	



#endif // OSDL_AUDIBLE_H_

