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


#ifndef OSDL_AUDIBLE_H_
#define OSDL_AUDIBLE_H_


#include "OSDLAudioCommon.h" // for AudioException

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
			
				AudibleException( const std::string & reason ) ; 
				
				virtual ~AudibleException() throw() ; 
		} ;
			
		
		
		/**
		 * Stores the number of times an Audible should be played.
		 * A value of Loop (-1) means forever (infinite loop).
		 * Values below -1 have no meaning and are ignored.
		 *
		 */
		typedef Ceylan::Sint32 PlaybackCount ;		
		
		
		
		/**
		 * This playback count corresponds to an infinite loop, unlimited
		 * repetitions (its value is -1).
		 *
		 */ 
		extern OSDL_DLL const PlaybackCount Loop ;  
			
				
				
		/**
		 * The minimum volume level (zero).
		 *
		 */ 
		extern OSDL_DLL const Volume MinVolume ;  
				
			
				
		/**
		 * The maximum volume level (depends on the platform, 127 or 128).
		 *
		 */ 
		extern OSDL_DLL const Volume MaxVolume ;  
			
				
									
												
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
				explicit Audible( bool convertedToOutputFormat = true ) ;
				
				
				/// Virtual destructor.
				virtual ~Audible() throw() ;
		
		
		
				/**
				 * Returns the volume associated to this audible instance.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual Volume getVolume() const = 0 ;
		
		
		
				/**
				 * Sets the volume associated to this audible instance.
				 *
				 * @param newVolume the new volume to be set.
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void setVolume( Volume newVolume ) = 0 ;
		
		
		
		
		
				// Play section.
				
				
				
				/**
				 * Plays this audible instance at once on any appropriate output
				 * (ex: channel number).
				 *
				 * @param playCount the number of times this audible should be 
				 * played, unless stopped by halt, fade out, expiration time or
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1 or
				 * below -1).
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void play( PlaybackCount playCount = 1 ) = 0 ; 
	
								
				
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
				 * audio module stop. -1 means forever. Otherwise it must be
				 * strictly positive (exception thrown if zero or below -1).
				 *
				 * @throw AudibleException if the operation failed or is not
				 * supported.
				 *
				 */
				virtual void playWithFadeIn( 
					Ceylan::System::Millisecond fadeInMaxDuration, 
					PlaybackCount playCount = 1 ) = 0 ; 
									
				
				
				/**
				 * Tells whether the internal samples of this audible have 
				 * already been converted to the sample format used for audio
				 * output.
				 * 
				 * @return true iff the samples are already converted.
				 *
				 */
				virtual bool isConvertedToOutputFormat() const ;
				
				
				
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

			
			
				/**
				 * Helper method to find the audible specified by its filename,
				 * using audio locators.
				 *
				 * @note Starts by searching the current directory, before
				 * using the locator paths.
				 *
				 * @param audibleFilename the filename of the file where
				 * the audible is stored.
				 *
				 * @throw AudibleException if the operation failed, including
				 * if the file could not be found despite audio path locator.
				 *
				 */
				static std::string FindAudiblePath( 
					const std::string & audibleFilename ) ;
				
			
			
			
			
			protected:
			
							
				/**
				 * Tells whether the internal samples have been converted
				 * already to the sample format used for audio output.
				 *
				 */
				bool _convertedToOutputFormat ;
			
			
				
				/**
				 * Helper method to factorize conversion of play counts into
				 * a number of loops.
				 *
				 * @throw AudibleException if the play count is out of bounds
				 * (must be either -1 for infinite looping or strictly 
				 * superior to zero).
				 *
				 */
				static int GetLoopsForPlayCount( PlaybackCount playCount ) ;
			
			
			
			
			private:
	
	
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * be never called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Audible( const Audible & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will be never called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 *
				 */			 
				Audible & operator = ( const Audible & source ) ;
				
			
		} ;
		
		
	}	
	
}	



#endif // OSDL_AUDIBLE_H_

