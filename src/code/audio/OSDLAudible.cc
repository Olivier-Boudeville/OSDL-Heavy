#include "OSDLAudible.h"



using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Audio ;



AudibleException::AudibleException( const std::string & reason ) throw():
	AudioException( reason )
{

}


	
AudibleException::~AudibleException() throw()
{

}





Audible::Audible( bool convertedToOutputFormat ) throw( AudibleException ):
	_convertedToOutputFormat( convertedToOutputFormat )
{

}



Audible::~Audible() throw()
{

}




bool Audible::isConvertedToOutputFormat() const throw()
{

	return _convertedToOutputFormat ;
	
}



const string Audible::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Audible " ;
	
	if ( ! _convertedToOutputFormat )
		res += "not " ;
		
	res += "converted to output format" ;
		
	return res ;
	
}

