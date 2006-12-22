#ifndef OSDL_JOYSTICK_COMMON_H_
#define OSDL_JOYSTICK_COMMON_H_


#include <string>
#include <list>


#include "OSDLEvents.h"   // for EventsException

#include "Ceylan.h"       // for Uint8


namespace OSDL
{


	namespace Events
	{
	

		/// Number (index) of a joystick, or joystick count.
		typedef unsigned int JoystickNumber ;

				
		/**
		 * Axis position, which ranges from 32768 to 32767.
		 *
		 */
		typedef Sint16 AxisPosition ;


		/// Position of a hat, can be combined (OR'd) to make the eight possible directions.
		typedef Uint8 HatPosition ;


		/// State of a joystick hat.
		extern const HatPosition Centered ;
		
		/// State of a joystick hat.
		extern const HatPosition Up ;
		
		/// State of a joystick hat.
		extern const HatPosition Right ;
		
		/// State of a joystick hat.
		extern const HatPosition Down ;
		
		/// State of a joystick hat.
		extern const HatPosition Left ;
		
		/// State of a joystick hat.
		extern const HatPosition RightUp ;
		
		/// State of a joystick hat.
		extern const HatPosition RightDown ;
		
		/// State of a joystick hat.
		extern const HatPosition LeftUp ;
		
		/// State of a joystick hat.
		extern const HatPosition LeftDown ;
		

		/** 
		 * Relative trackball motion.
		 *
		 */				
		typedef int BallMotion ;
		
		
		/// Mother class for all joystick exceptions. 		
		class JoystickException: public EventsException 
		{
			public: 
				explicit JoystickException( const std::string & reason ) throw() ; 
				virtual ~JoystickException() throw() ; 
				
		} ;
	
	}	
	
}



#endif // OSDL_JOYSTICK_COMMON_H_
