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
	
		
OSDL::MVC::Controller & InputDevice::getActualController() const throw( EventsException )
{
	return * dynamic_cast<OSDL::MVC::Controller *>( & getController() ) ;
}


const string InputDevice::toString( Ceylan::VerbosityLevels level ) const throw() 
{
	
	if ( _controller == 0 )
		return "OSDL actual input device currently not linked to any controller" ;
	else
		return "OSDL actual input device currently linked with following controller : "
			+ _controller->toString( level ) ;
				
}
