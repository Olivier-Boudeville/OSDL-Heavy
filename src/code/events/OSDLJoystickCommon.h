/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#ifndef OSDL_JOYSTICK_COMMON_H_
#define OSDL_JOYSTICK_COMMON_H_


#include "OSDLEvents.h"   // for EventsException

#include "Ceylan.h"       // for Uint8, etc.


#include <string>
#include <list>




namespace OSDL
{



	namespace Events
	{
	


		/// Number (index) of a joystick, or joystick count.
		typedef Ceylan::Uint32 JoystickNumber ;


		/// Number of joystick axis.
		typedef Ceylan::Uint32 JoystickAxesCount ;


		/// Number of joystick trackballs.
		typedef Ceylan::Uint32 JoystickTrackballsCount ;


		/// Number of joystick hats.
		typedef Ceylan::Uint32 JoystickHatsCount ;


		/// Number of joystick buttons.
		typedef Ceylan::Uint32 JoystickButtonsCount ;


				
		/**
		 * Axis position, which ranges from -32768 to 32767.
		 *
		 */
		typedef Ceylan::Sint16 AxisPosition ;



		/**
		 * Position of a hat, can be combined (OR'd) to make the eight 
		 * possible directions.
		 *
		 */
		typedef Ceylan::Uint8 HatPosition ;



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
		typedef Ceylan::Sint32 BallMotion ;
		
		
		
		/// Mother class for all joystick exceptions. 		
		class OSDL_DLL JoystickException: public EventsException 
		{
			public: 
			
				explicit JoystickException( const std::string & reason ) ;
				 
				virtual ~JoystickException() throw() ; 
				
		} ;
		
	
	}	
	
}



#endif // OSDL_JOYSTICK_COMMON_H_

