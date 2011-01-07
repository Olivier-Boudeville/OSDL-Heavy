/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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
		extern OSDL_DLL const MouseNumber DefaultMouse ;



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
		extern OSDL_DLL const MouseButtonNumber LeftButton ;
				
				
		/// Identifies the middle button of a mouse.
		extern OSDL_DLL const MouseButtonNumber MiddleButton ;
				
				
		/// Identifies the right button of a mouse.
		extern OSDL_DLL const MouseButtonNumber RightButton ;
				
				
				
				
		/**
		 * Identifies the virtual button of a mouse corresponding to its
		 * wheel being up.
		 *
		 */
		extern OSDL_DLL const MouseButtonNumber WheelUp ;
		
			
				
		/**
		 * Identifies the virtual button of a mouse corresponding to its
		 * wheel being down.
		 *
		 */
		extern OSDL_DLL const MouseButtonNumber WheelDown ;
				
				
			
				
		
		/// Mother class for all mouse exceptions. 		
		class OSDL_DLL MouseException: public EventsException 
		{
			public: 
			
				explicit MouseException( const std::string & reason )  ;
				 
				virtual ~MouseException() throw() ; 
				
		} ;
		
	
	}	
	
}



#endif // OSDL_MOUSE_COMMON_H_

