#ifndef OSDL_INPUT_DEVICE_HANDLER_H_
#define OSDL_INPUT_DEVICE_HANDLER_H_



#include "OSDLEventsCommon.h"     // for EventsException

#include "Ceylan.h"               // for inheritance

#include <string>



namespace OSDL
{

		
	namespace Events
	{
			



		/// Mother class for all keyboard exceptions. 		
		class InputDeviceHandlerException : public EventsException 
		{
			public: 
				explicit InputDeviceHandlerException( 
					const std::string & reason ) throw() ; 
				virtual ~InputDeviceHandlerException() throw() ; 
				
		} ;


		
		/**
		 * Handler for all kinds of input devices.
		 *
		 * @note Usually the input device must be specified in the call to 
		 * CommonModule::getCommonModule so that its support is enabled 
		 * (ex : UseJoystick, UseKeyboard, etc.).
		 *
		 */
		class InputDeviceHandler : public Ceylan::Object
		{
		
			
			public:


				/**
				 * Constructs a new intput device handler.
				 *
				 * @throw InputDeviceHandlerException if the initialization 
				 * of the input device subsystem failed.
				 *
				 */
				explicit InputDeviceHandler() 
					throw( InputDeviceHandlerException ) ;
				
				
				/// Virtual destructor.
				virtual ~InputDeviceHandler() throw() ;



			private:
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit InputDeviceHandler( 
					const InputDeviceHandler & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				InputDeviceHandler & operator = ( 
					const InputDeviceHandler & source ) throw() ;
				

		} ;
	
	}	
	
}



#endif // OSDL_INPUT_DEVICE_HANDLER_H_
