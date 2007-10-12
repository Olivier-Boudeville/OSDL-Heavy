#ifndef OSDL_AUDIO_H_
#define OSDL_AUDIO_H_


#include "OSDLTypes.h"       // for Flags
#include "OSDLException.h"   // for OSDL::Exception 

#include "Ceylan.h"          // for inheritance

#include <string>
#include <list>



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
				
				
								
		/**
		 * Root module for all audio services.
		 *
		 */
		class OSDL_DLL AudioModule: public Ceylan::Module
		{
		
		
			// The common module has to create the audio module.
			friend class OSDL::CommonModule ;
		
		
			public:


			
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
					
				
					
			protected:
	

				/// Keeps tracks of audio initialization calls.
				static bool _AudioInitialized ;



			private:


	
				/**
				 * Private constructor to be sure it will not be implicitly
				 * called.
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
		
	}	
	
}	



#endif // OSDL_AUDIO_H_

