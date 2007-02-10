#include "OSDLSprite.h"

#include "OSDLBoundingBox2D.h"   // for BoundingBox2D


using namespace Ceylan::Log ;

using namespace OSDL::Rendering ;

using std::string ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>     // for OSDL_DEBUG_SPRITE and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message) LogPlug::debug( message ) ;

#else // OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_SPRITE



Sprite::Sprite( bool ownBoundingBox ) throw() :
	View(),
	_ownBoundingBox( ownBoundingBox ),
	_box( 0 )
{

}


Sprite::~Sprite() throw()
{

	if ( _ownBoundingBox && ( _box != 0 ) )
		delete _box ;
		
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

