#ifndef OSDL_BASIC_H_
#define OSDL_BASIC_H_


#include "OSDLException.h"  // for OSDL::Exception


#include "Ceylan.h"         // for Ceylan::Module, Version, etc.



namespace OSDL
{


	/** 
	 * Allows to ensure that the actual OSDL library being linked with is
	 * compatible with the one expected by the code that will use it (be it a
	 * library itself or a program).
	 *
	 * The OSDL version is directly encoded in the library, whereas the 
	 * version expected by a user program is found in this OSDL header file,
	 * according to the version being used to compile it.
	 *
	 * Use this macro in your application that way : 
	 * 'CHECK_OSDL_VERSIONS() ;'
	 * for example in the first lines of your 'main' function. Of course the 
	 * main OSDL header file ('OSDL.h') should have been included
	 * previously.
	 *
	 * This is a macro since it has to be evaluated within the user code 
	 * environment, not when the OSDL library is built.
	 *
	 */
	#define CHECK_OSDL_VERSIONS()                                            \
        OSDL::LibtoolVersion headerVersion(                                  \
			OSDL::actualOSDLHeaderLibtoolVersion ) ;                         \
        if ( ! /* library version */                                         \
                OSDL::GetVersion().isCompatibleWith( headerVersion ) )       \
            OSDL::emergencyShutdown(                                         \
                "OSDL library version currently linked ("                    \
                + OSDL::GetVersion().toString()                              \
                + ") is not compatible with the one read from the OSDL "     \
                "header files used to compile this application ("            \
                + headerVersion.toString() + "), aborting." ) ;


	
	
	namespace Video
	{
	
		// Managed by the common module.
		class VideoModule ;
		
	}
	
	
	namespace Events
	{
	
		// Managed by the common module.
		class EventsModule ;
		
	}

		
	namespace Audio
	{
	
		// Managed by the common module.
		class AudioModule ;
		
	}
			
			
	// Managed by the common module.
	class CDROMDriveHandler ;
	
	
	/// Returns the version of the OSDL library currently linked.
	OSDL_DLL Ceylan::Version & GetVersion() throw() ;

	
	/// Shutdowns all OSDL services.
	OSDL_DLL void stop() throw() ;
		
		
		
	/**
	 * Root of all OSDL modules. 
	 *
	 * From this common module, all other modules can be triggered.
	 *
	 */
	class OSDL_DLL CommonModule : public Ceylan::Module
	{



		/**
		 * This friend function is intended to be the initial means of getting
		 * a reference to the common module : this is the entry point to 
		 * the whole OSDL system.
		 *
		 * @param flags The requested flags for this common root module, 
		 * any OR'd combination of CommonModule static flags, so that 
		 * associated subsystems are instanciated.
		 *
		 * @see UseTimer, UseAudio, UseVideo, UseCDROM, UseJoystick,
		 * UseEverything, NoParachute, UseEventThread, etc.
		 * 
		 * @note UseVideo is required to start the event loop, so it is 
		 * implied by the UseJoystick flag.
		 * 
		 * @note This method is not static to avoid pitfalls of static
		 * initializer ordering.
		 *
		 */			
		OSDL_DLL friend CommonModule & getCommonModule( Ceylan::Flags flags )
			throw() ;
		
		
		/**
		 * Tells whether there already exists a common module.
		 *
		 */
		OSDL_DLL friend bool hasExistingCommonModule() throw() ;
		
		
		/**
		 * This friend function is intended to be the usual means of
		 * getting a reference to the common module, which must already exist.
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
		OSDL_DLL friend CommonModule & getExistingCommonModule() throw() ;
		
		
		
		
		/// This friend function allows to shutdown all OSDL services.
		OSDL_DLL friend void stop() throw() ;



		public:
		
		
			/// Data type of back-end return code :
			typedef Ceylan::Sint32 BackendReturnCode ;


			/// Exported logical value for back-end success.
			static const BackendReturnCode BackendSuccess ;
			
			
			/**
			 * Exported logical value for back-end success/
			 *
			 * Prefer only testing against SDLSuccess :
			 * <code>if ( [...] != BackendSuccess ) ...</code>
			 *
			 */
			static const BackendReturnCode BackendError ;
		
		
			/// Tells whether a video module is available.
			virtual bool hasVideoModule() const throw() ; 

					
			/**
			 * Returns current video module.
			 *
			 * @throw OSDL::Exception is not video module is available.
			 *
			 */
			virtual Video::VideoModule & getVideoModule() 
				const throw( OSDL::Exception ) ;
			
			
			
			/// Tells whether a video module is available.
			virtual bool hasEventsModule() const throw() ; 

			
			/**
			 * Returns current events module.
			 *
			 * @throw OSDL::Exception is not events module is available.
			 *
			 */
			virtual Events::EventsModule & getEventsModule() 
				const throw( OSDL::Exception ) ;
			


			/// Tells whether a video module is available.
			virtual bool hasAudioModule() const throw() ; 

			
			/**
			 * Returns current audio module.
			 *
			 * @throw OSDL::Exception is not audio module is available.
			 *
			 */
			virtual Audio::AudioModule & getAudioModule() 
				const throw( OSDL::Exception ) ;
			
			
			
			/// Returns flags used for this common module.
			virtual Ceylan::Flags getFlags() const throw() ;




			/// Tells whether a CD-ROM drive handler is available.
			virtual bool hasCDROMDriveHandler() const throw() ; 

			
			/**
			 * Returns current CD-ROM handler.
			 *
			 * @throw OSDL::Exception is not CD-ROM handler is available.
			 *
			 */
			virtual CDROMDriveHandler & getCDROMDriveHandler() 
				const throw( OSDL::Exception ) ;
			
			

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
			 * Describes specified top-level flags used to initialize 
			 * the SDL back-end.
			 *
			 * @param flags
			 *
			 * @note One shall not call this method on flags used 
			 * with setMode or for a surface.
			 * Use VideoModule::interpretFlags or Surface::interpretFlags
			 * instead, since their meaning is different.
			 *
			 */ 
			static std::string InterpretFlags( Ceylan::Flags flags ) throw() ;
	
	
			/**
			 * Returns a summary about the possible use of general-purpose*
			 * environment variables, for the selected back-end, 
			 * expressed in specified format.
			 *
			 * @note The SDL back-end can be partly driven by a set
			 * of environment variables.
			 *
			 */	
			static std::string DescribeEnvironmentVariables() throw() ;	


			static bool IsBackendInitialized() throw() ;
			

			/*
			 * These flags can apply to Surfaces <b>created by setMode</b>,
			 * i.e. all the display Surfaces.
			 * 
			 * The legal flags for setMode form a subset of the Surface 
			 * flags, with one flag being added, NoFrame. 
			 *
			 * @note These flags are not to be mixed up with the Surface 
			 * flags' ones.
			 *
			 * @see OSDL::Video::Surface			 
			 */


			/// Initializes the timer subsystem.
			static const Ceylan::Flags UseTimer ;			   

			/// Initializes the audio subsystem.
			static const Ceylan::Flags UseAudio ;
						   
			/// Initializes the video subsystem.
			static const Ceylan::Flags UseVideo ;
						   
			/// Initializes the CD-ROM subsystem.
			static const Ceylan::Flags UseCDROM ;			   


			/**
			 * Initializes the joystick subsystem, implies initializing 
			 * the video (UseVideo).
			 *
			 */
			static const Ceylan::Flags UseJoystick ;			   


			/**
			 * Initializes the keyboard subsystem, implies initializing 
			 * the video (UseVideo).
			 *
			 */
			static const Ceylan::Flags UseKeyboard ;			   


			/**
			 * Initializes the mouse subsystem, implies initializing 
			 * the video (UseVideo).
			 *
			 */
			static const Ceylan::Flags UseMouse ;			   


			/// Initializes all above subsystems.
			static const Ceylan::Flags UseEverything ;			   


			/**
			 * Do not catch fatal signals.
			 *
			 * By default fatal signals are caught, so that if your program
			 * crashes, the resulting exception will be intercepted in 
			 * order to clean up and return the display to a usable state. 
			 *
			 * However this system does not behave nicely with debuggers,
			 * so it might be disabled (mostly for debugging purpose) 
			 * with this flag.
			 *
			 * @note If this flag is used, fatal signals will not be 
			 * caught which may, on some platforms, force the user to reboot.
			 * For instance, if a X11 fullscreen application crashes or 
			 * exits without quitting properly, using the 'NoParachute' 
			 * flag leaves the desktop to the fullscreen application
			 * resolution, that is often far lower than the original desktop
			 * resolution, which annoys the user. 
			 *
			 */
			static const Ceylan::Flags NoParachute ;			   


			/// Initializes the event thread.
			static const Ceylan::Flags UseEventThread ;




		protected:
		
			
			/**
			 * Corrects the input flags, formatted as parameters used 
			 * to create the common module, so that they can be retain 
			 * directly, without further change.
			 *
			 * For example, if an event source such as a joystick is 
			 * selected, then the video module must be activated, even 
			 * if not explicitly selected, otherwise no event at all
			 * will be available. Thus the method will fix the flags so 
			 * that they are consistent.
			 *
			 */
			static Ceylan::Flags AutoCorrectFlags( Ceylan::Flags inputFlags )
				throw() ;
					
					
			/// Pointer to the current video module used, if any.
			Video::VideoModule * _video ;
			
			/// Pointer to the current events module used, if any.			
			Events::EventsModule * _events ;			
			
			/// Pointer to the current video module used, if any.
			Audio::AudioModule * _audio ;
			
			/// Flags used for this common module.
			Ceylan::Flags _flags ;	
			
			/// Pointer to the current CD-ROM drive handler used, if any.
			CDROMDriveHandler * _cdromHandler ;
			
			
			// Static attributes :	


			/// Tells whether the back-end used by OSDL is initialized.
			static bool _BackendInitialized ;
				
			/// The common module being currently used, if any.	
			static CommonModule * _CurrentCommonModule ;


			/**
			 * Array of all known general purpose environment variables, 
			 * for SDL back-end.
			 *
			 */
			static std::string _SDLEnvironmentVariables[] ;



		private:		
		
		
			/**
 			 * Initializes OSDL system common module.
			 *
 			 * @param flags parameters to initialize SDL.
			 * 
			 * @note To have the common module created, use getCommonModule.
			 *
 			 */
 			explicit CommonModule( Ceylan::Flags flags ) 
				throw ( OSDL::Exception ) ;
	
	
 			/**
 			 * Stops OSDL system common module.
			 *
 			 * @see SDL_Quit defined in http://sdldoc.csn.ul.ie/sdlquit.php.
			 *
 			 */
 			virtual ~CommonModule() throw() ;	
			
					
		
	} ;		
	
}



#endif // OSDL_BASIC_H_

