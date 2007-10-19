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



int Audible::GetLoopsForPlayCount( PlaybackCount playCount )
	throw( AudibleException )
{

	if ( playCount == 0 || playCount < -1 )
		throw AudibleException( "Audible::GetLoopsForPlayCount failed: "
			"the play count is out of bounds (" 
			+ Ceylan::toString( playCount ) + ")" ) ;
 
 	if ( playCount == -1 )
		return  -1 ;
	else
		return ( playCount - 1 ) ;	
				
}

