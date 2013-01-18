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
		class OSDL_DLL InputDeviceHandlerException : public EventsException 
		{
			public: 
			
				explicit InputDeviceHandlerException( 
					const std::string & reason ) ; 
					
				virtual ~InputDeviceHandlerException() throw() ; 
				
		} ;



		
		/**
		 * Handler for all kinds of input devices.
		 *
		 * @note Usually the input device must be specified in the call to 
		 * CommonModule::getCommonModule so that its support is enabled 
		 * (ex: UseJoystick, UseKeyboard, etc.).
		 *
		 */
		class OSDL_DLL InputDeviceHandler : public Ceylan::Object
		{
		
		
			
			public:



				/**
				 * Constructs a new intput device handler.
				 *
				 * @throw InputDeviceHandlerException if the initialization 
				 * of the input device subsystem failed.
				 *
				 */
				explicit InputDeviceHandler() ;
				
				
				
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
					const InputDeviceHandler & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				InputDeviceHandler & operator = ( 
					const InputDeviceHandler & source ) ;
				

		} ;
		
	
	}	
	
}



#endif // OSDL_INPUT_DEVICE_HANDLER_H_

