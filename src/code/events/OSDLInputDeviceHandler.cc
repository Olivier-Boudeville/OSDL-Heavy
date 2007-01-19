#include "OSDLInputDeviceHandler.h"


using namespace OSDL::Events ;


InputDeviceHandlerException::InputDeviceHandlerException( 
		const std::string & reason ) throw() :
	EventsException( reason )
{

}


InputDeviceHandlerException::~InputDeviceHandlerException() throw()
{

}




InputDeviceHandler::InputDeviceHandler() throw( InputDeviceHandlerException )
{

}


InputDeviceHandler::~InputDeviceHandler() throw()
{

}

