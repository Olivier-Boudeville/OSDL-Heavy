/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#include "OSDLInputDevice.h"


#include "OSDLController.h"     // for Controller


using std::string ;

using namespace OSDL::Events ;



InputDevice::InputDevice() throw() :
	Ceylan::InputDevice()
{

}
	
	
InputDevice::InputDevice( OSDL::MVC::Controller & actualController ) throw() :
	Ceylan::InputDevice( actualController )
{

}


InputDevice::~InputDevice() throw()
{

}
	
		
OSDL::MVC::Controller & InputDevice::getActualController() 
	const throw( EventsException )
{
	return * dynamic_cast<OSDL::MVC::Controller *>( & getController() ) ;
}


const string InputDevice::toString( Ceylan::VerbosityLevels level ) 
	const throw() 
{
	
	if ( _controller == 0 )
		return "OSDL actual input device currently "
			"not linked to any controller" ;
	else
		return "OSDL actual input device currently "
			"linked with following controller : " 
			+ _controller->toString( level ) ;
				
}

