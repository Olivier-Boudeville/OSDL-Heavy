#include "OSDLSprite.h"


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::Rendering ;

using std::string ;


#if OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message) LogPlug::debug( message ) ;

#else // OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_SPRITE



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

