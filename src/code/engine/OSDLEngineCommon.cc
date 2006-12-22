#include "OSDLEngineCommon.h"

using std::string ;

using namespace OSDL::Engine ;


EngineException::EngineException( const string & reason ) throw() :
	OSDL::Exception( reason )
{

}


EngineException::~EngineException() throw()
{

}


SchedulingException::SchedulingException( const string & reason ) throw() :
	EngineException( "SchedulingException : " + reason )
{

}


SchedulingException::~SchedulingException() throw()
{

}

