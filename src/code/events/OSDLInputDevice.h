#ifndef OSDL_INPUT_DEVICE_H_
#define OSDL_INPUT_DEVICE_H_


#include "OSDLEvents.h"           // for EventException
#include "OSDLEventsCommon.h"     // for typedef


#include "Ceylan.h"               // for inheritance


#include <string>



namespace OSDL
{


	namespace MVC
	{

		// Controllers are linked to input devices.
		class Controller ;
		
	}
	
	
	namespace Events
	{
	
			
		
		
		/**
		 * Models an actual OSDL input device, instead of an abstract Ceylan
		 * input device.
		 *
		 * The difference lies in the fact that OSDL controllers have to be
		 * aware of all the low level events that can occur, therefore they 
		 * cannot be basic Ceylan::Controller. 
		 * These last ones cannot be extended with low level events since it
		 * would make Ceylan depend on multimedia back-end (namely SDL). 
		 *
		 * The result is that input devices such as joysticks cannot be 
		 * linked with abstract Ceylan::Controller instances, which as such
		 * would be useless. 
		 * In order to have access to the specialized controllers we need, 
		 * an OSDL input device linked with an OSDL controller is an 
		 * interesting solution, insofar as it ensures that all actual input
		 * devices (ex : joysticks) can call, say, the axisChanged method 
		 * of their controller : each actual input device will have to 
		 * implement the getActualController method.
		 *
		 */
		class OSDL_DLL InputDevice : public Ceylan::InputDevice
		{
		
			
			public:


	
				/**
				 * Constructs an actual input device not linked to any 
				 * OSDL controller.
				 *
				 */
				InputDevice() throw() ;
	
	
				/**
				 * Constructs an actual input device that will send events 
				 * to the specified OSDL controller.
				 *
				 */
				explicit InputDevice( 
					OSDL::MVC::Controller & actualController ) throw() ;
	
	
				/// Basic virtual destructor.
				virtual ~InputDevice() throw() ;
		
	
				/**
				 * Returns the actual (OSDL) controller this input device 
				 * should be linked to.
				 *
				 * @throw EventException if this input device is not 
				 * linked to any controller, or if the controller is not 
				 * an OSDL one.
				 *
				 * @see isLinkedToController
				 *
				 */
				virtual OSDL::MVC::Controller & getActualController() 
					const throw( EventsException ) ;
	
	
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;
	
	

			private:
	
	
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit InputDevice( const InputDevice & source ) throw() ;
	
	
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				InputDevice & operator = ( const InputDevice & source ) 
					throw() ;
				
				
		} ;
	
	}	
	
}



#endif // OSDL_INPUT_DEVICE_H_

