#ifndef OSDL_MOUSE_COMMON_H_
#define OSDL_MOUSE_COMMON_H_


#include "OSDLEvents.h"   // for EventsException

#include "Ceylan.h"       // for Uint8, etc.


#include <string>
#include <list>



namespace OSDL
{


	namespace Events
	{
	

		/// Number (index) of a mouse, or mice count.
		typedef Ceylan::Uint8 MouseNumber ;

		/// Identifies the default mouse (the first one).
		extern const MouseNumber DefaultMouse ;


		/// Number (index) of a mouse button, or buttons count.
		typedef Ceylan::Uint8 MouseButtonNumber ;

		/// Number (index) of a mouse wheel, or wheels count.
		typedef Ceylan::Uint8 MouseWheelNumber ;


		/**
		 * Button mask, each bit tells whether the associated button 
		 * is pressed.
		 *
		 */
		typedef Ceylan::Uint8 MouseButtonMask ;


		/// Identifies the left button of a mouse.
		extern const MouseButtonNumber LeftButton ;
				
		/// Identifies the middle button of a mouse.
		extern const MouseButtonNumber MiddleButton ;
				
		/// Identifies the right button of a mouse.
		extern const MouseButtonNumber RightButton ;
				
				
		/**
		 * Identifies the virtual button of a mouse corresponding to its
		 * wheel being up.
		 *
		 */
		extern const MouseButtonNumber WheelUp ;
		
				
		/**
		 * Identifies the virtual button of a mouse corresponding to its
		 * wheel being down.
		 *
		 */
		extern const MouseButtonNumber WheelDown ;
				
				
				
		
		/// Mother class for all mouse exceptions. 		
		class OSDL_DLL MouseException: public EventsException 
		{
			public: 
				explicit MouseException( const std::string & reason ) 
					throw() ; 
				virtual ~MouseException() throw() ; 
				
		} ;
		
	
	}	
	
}



#endif // OSDL_MOUSE_COMMON_H_

