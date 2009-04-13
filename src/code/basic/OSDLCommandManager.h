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


#ifndef OSDL_COMMAND_MANAGER_H_
#define OSDL_COMMAND_MANAGER_H_



#include "OSDLAudioCommon.h" // for BufferSize       
#include "OSDLException.h"   // for OSDL::Exception


#include "Ceylan.h"          // for inheritance

#include <string>


// Command management only needed for the Nintendo DS:
#if defined(OSDL_ARCH_NINTENDO_DS) && OSDL_ARCH_NINTENDO_DS


namespace OSDL
{

	
	namespace Audio
	{
	
		class Sound ;
		class Music ;
		
	}
					
	
	/// Mother class for all command-related exceptions.
	class CommandException: public OSDL::Exception
	{ 
		public: 
	 	 
	 		explicit CommandException( const std::string & reason )
	 			throw() ;
	 		 
			virtual ~CommandException() throw() ; 
	} ;
	
	
	// Defined afterwards.
	class CommandManager ;
	
		
	/**
	 * Persistant settings about the command manager, not related to a 
	 * particular music.
     *
     * @note References to buffers point towards the internal buffers of
     * the command manager.
     *
     */
    struct CommandManagerSettings
    {

    	/**
    	 * Actual sample buffers are managed directly by the command manager
    	 * so that if no music is played, no memory is reserved, but the
    	 * first played music will trigger the buffer reservation, once for
    	 * all.
    	 *
    	 */

    	/// Pointer to the shared command manager (cache).
    	CommandManager * _commandManager ;


    	/// The size of a (simple) CommandManager buffer, in bytes:
    	Audio::BufferSize _bufferSize ;


    	/**
    	 * The actual double sound buffer, two simple buffers, one after the
    	 * other (so the first half buffer has the same address as this
    	 * double one).
    	 *
    	 * @note Pointer to the internal buffer of the CommandManager.
    	 *
    	 */
    	Ceylan::Byte * _doubleBuffer ;


    	/// The address of the second buffer:
    	Ceylan::Byte * _secondBuffer ;

    } ;

	
			
	/**
	 * IPC-based command manager, between the two ARMs of the Nintendo DS.
	 *
	 */
	class OSDL_DLL CommandManager: public Ceylan::System::FIFO
	{



		public:
		
		
			/**
			 * Creates a new command manager, which is expected to be a
			 * singleton.
			 *
			 * @throw CommandException if the operation failed.
			 *
			 */
			CommandManager() throw( CommandException ) ;
			

			/// Virtual destructor.
			virtual ~CommandManager() throw() ;
			
			
			
			// Audio section.
			
			
			/**
			 * Requests the ARM7 to play the specified sound at once.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the sound to finish or even to start.
			 *
			 * @param sound the sound to play. Its content (samples) must have
			 * been loaded already.
			 *
			 */
			virtual void playSound( Audio::Sound & sound ) 
				throw( CommandException ) ;
			
			
			
			/**
			 * Allocates the adequate memory for music playback, and creates
			 * a shared structure to expose settings needed by musics.
			 *
			 */
			virtual void enableMusicSupport() throw( CommandException ) ;
			
			
			/**
			 * Deallocates the memory dedicated to music playback, and
			 * deallocates the shared structure to expose settings needed by
			 * musics.
			 *
			 */
			virtual void disableMusicSupport() throw( CommandException ) ;
			
			
			
			/**
			 * Returns the size of an internal (simple) buffer for music, in
			 * bytes.
			 *
			 */
			virtual Audio::BufferSize getMusicBufferSize() const throw() ;


			/**
			 * Returns the address of internal double buffer for music.
			 *
			 * @throw CommandException if no buffer is available.
			 *
			 */
			virtual Ceylan::Byte * getMusicBuffer() const
				throw( CommandException ) ;


			/**
			 * Requests the ARM7 to play the specified music at once.
			 *
			 * If a music was already playing, stop it and start immediately
			 * the specified one.
			 *
			 * @param music the music to play.
			 *		 
			 * Returns just after having sent the request, i.e. without waiting
			 * for the music to finish or even to start.
			 *
			 */
			virtual void playMusic( Audio::Music & music ) 
				throw( CommandException ) ;
			
			
			/**
			 * Requests the ARM7 to play the specified music with a fade-in.
			 *
			 * If a music was already playing, stop it and start immediately
			 * the specified one.
			 *
			 * @param music the music to play.
			 *
			 * @param fadeInMaxDuration duration in milliseconds during
			 * which the fade-in effect should take to go from silence to
			 * full volume. The fade in effect only applies to the first
			 * loop.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the music to finish or even to start.
			 *
			 */
			virtual void playMusicWithFadeIn( Audio::Music & music, 
					Ceylan::System::Millisecond fadeInMaxDuration ) 
				throw( CommandException ) ;
			
			
			
			/**
			 * Requests the ARM7 to stop at once any currently music being
			 * played.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the music to finish.
			 *
			 */
			virtual void stopMusic() throw( CommandException ) ;
			
			
			/**
			 * Requests the ARM7 to start at once a fade-in effect on the
			 * music over specified duration.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the fade-in to complete.
			 *
			 */
			virtual void fadeInMusic( 
					Ceylan::System::Millisecond fadeInMaxDuration ) 
				throw( CommandException ) ;
			
			
			/**
			 * Requests the ARM7 to start at once a fade-out effect on the
			 * music over specified duration.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the fade-out to complete.
			 *
			 */
			virtual void fadeOutMusic( 
					Ceylan::System::Millisecond fadeOutMaxDuration ) 
				throw( CommandException ) ;
			
			
			/**
			 * Requests the ARM7 to set the volume of music channel to
			 * specified value.
			 *
			 * @note Will affect all musics played afterwards this one as well.
			 *
			 */
			virtual void setMusicVolume( Audio::Volume newVolume ) 
				throw( CommandException ) ;



			/**
			 * Requests the ARM7 to pause the playback of current music 
			 * (if any) at once.
			 *
			 * @note Will last until unPauseMusic or stopMusic is called.
			 *
			 */
			virtual void pauseMusic() throw( CommandException ) ;

			
			/**
			 * Requests the ARM7 to unpause the playback of current music 
			 * (if any) at once, so that the playback resumes.
			 *
			 */
			virtual void unpauseMusic() throw( CommandException ) ;
			

			
			/**
			 * Notifies the ARM7 that the end of encoded stream was reached
			 * while streaming music.
			 *
			 * @throw CommandException if the operation failed.
			 *
			 */
			virtual void notifyEndOfEncodedStreamReached() 
				throw( CommandException ) ;
				
			
			
			/**
			 * Unsets specified music if it was current, so that it is not
			 * current anymore from this manager point of view.
			 *
			 */
			virtual void unsetCurrentMusic( Audio::Music & music ) throw() ;
			
			
			
			/**
			 * Returns an interpretation of the latest error code set by
			 * the ARM7, taking into account OSDL error codes as well.
			 *
			 * Reads the relevant shared variable.
			 *
			 */
			virtual std::string interpretLastARM7ErrorCode() throw() ;

			  
            /**
             * Returns an user-friendly description of the state of this object.
             *
			 * @param level the requested verbosity level.
			 *
			 * @note Text output format is determined from overall settings.
			 *
			 * @see Ceylan::TextDisplayable
             *
             */
	 		virtual const std::string toString( 
				Ceylan::VerbosityLevels level = Ceylan::high ) const throw() ;



			// Static section.


			/**
			 * Returns true iff the command manager is already available.
			 *
			 */
			static bool HasExistingCommandManager() throw() ;
			

			/**
			 * Returns the supposedly already existing command manager.
			 *
			 * @throw CommandException if the operation failed, including if
			 * no manager was already existing.
			 *
			 */
			static CommandManager & GetExistingCommandManager() 
				throw( CommandException ) ;

			
			/**
			 * Returns the command manager, creates it if needed.
			 *
			 * @throw CommandException if the operation failed.
			 *
			 */
			static CommandManager & GetCommandManager() 
				throw( CommandException ) ;
			



		protected:

		
			/**
			 * The current music being played, if any.
			 * 
			 * @note Local copy of static Music current music member, to avoid
			 * retrieving the pointer again and again while refilling buffers.
			 *
			 */
			Audio::Music * _currentMusic ;
		
		
			/**
			 * The actual double sound buffer for musics, two simple buffers, 
			 * one after the other (so the first half buffer has the same
			 * address as this double one).
			 *
			 */
			Ceylan::Byte * _doubleBuffer ;
		
		
			/// The size of a (simple) buffer, in bytes:
			Audio::BufferSize _bufferSize ;
		
		
			/// The settings to share with all musics.
			CommandManagerSettings * _settings ;
			
			
			/**
			 * Method responsible for the actual decoding and management of
			 * an incoming command specific to OSDL.
			 *
			 * Implements the library-specific protocol for these commands.
			 *
			 * @param commandID the library-specific command ID read
			 * from the first FIFO element of the command.
			 *
			 * @param firstElement the full (first) FIFO element
			 * corresponding to the command (thus containing commandID).
			 *
			 * @note Called automatically by handleReceivedCommand when
			 * relevant.
			 *
			 * @note Only lightweight operations should be performed here.
			 *
			 */
			virtual void handleReceivedIntegratingLibrarySpecificCommand(
					FIFOCommandID commandID, 
					Ceylan::System::FIFOElement firstElement )
				throw() ;
			
			
			/// The singleton manager.
			static CommandManager * _IPCManager ;

			
			
		private:		
		
		
			/**
			 * Copy constructor made private to ensure that it will be 
			 * never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * constructor is called, implicitly or not.
			 * 
			 */			 
			explicit CommandManager( const CommandManager & source )
				throw() ;
			
			
			/**
			 * Assignment operator made private to ensure that it will 
			 * be never called.
			 *
			 * The compiler should complain whenever this undefined 
			 * operator is called, implicitly or not.
			 * 
			 */			 
			CommandManager & operator = ( const CommandManager & source )
				throw() ;
			
					
		
	} ;		
		
	
}

#endif // defined(OSDL_ARCH_NINTENDO_DS) && OSDL_ARCH_NINTENDO_DS 


#endif // OSDL_COMMAND_MANAGER_H_

