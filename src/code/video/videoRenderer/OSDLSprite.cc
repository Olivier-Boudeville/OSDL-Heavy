#include "OSDLSprite.h"


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::Rendering ;

using std::string ;


#ifdef OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message) LogPlug::debug( message ) ;
#else

#define OSDL_RENDER_LOG(message)

#endif



Sprite::Sprite( bool ownBoundingBox ) throw() :
	View(),
	_ownBoundingBox( ownBoundingBox ),
	_renderingDone( false )
{

}


Sprite::~Sprite() throw()
{

}


const string Sprite::toString( Ceylan::VerbosityLevels level ) const throw() 
{

	string res = "Sprite which " ;
	
	if ( _ownBoundingBox )
		res += "owns" ;
	else
		res += " does not own" ;
	
	res += " its bounding box. " + View::toString( level ) ;		
			
	return res ;
	
}

