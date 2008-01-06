#ifndef OSDL_COMMAND_MANAGER_H_
#define OSDL_COMMAND_MANAGER_H_



#include "OSDLException.h"  // for OSDL::Exception


#include "Ceylan.h"         // for inheritance

#include <string>



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
			 * Requests the ARM7 to play the specified music at once.
			 *
			 * If a music was already playing, stop it and start immediately
			 * the specified one.
			 *
			 * Returns just after having sent the request, i.e. without waiting
			 * for the music to finish or even to start.
			 *
			 */
			virtual void playMusic( Audio::Music & music ) 
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
			 * Notifies the ARM7 that the end of encoded stream was reached
			 * while streaming music.
			 *
			 * @throw CommandException if the operation failed.
			 *
			 */
			virtual void notifyEndOfEncodedStreamReached() 
				throw( CommandException ) ;
				
				
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



#endif // OSDL_COMMAND_MANAGER_H_

