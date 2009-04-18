#include "OSDLStandardRenderer.h"

#include "OSDLScheduler.h"    // for GetExistingScheduler
#include "OSDLSurface.h"      // for Surface


using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG_RENDERER and al 
#endif // OSDL_USES_CONFIG_H


#include "Ceylan.h"           // for View

using namespace Ceylan::Log ;

using std::string ;
using std::list ;



#if OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message) send( message ) ;

#else // OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_RENDERER





StandardRenderer::StandardRenderer( Video::Surface & screen, 
		bool registerToScheduler ) throw( RenderingException ):
	Renderer( registerToScheduler ),
	_screen( & screen )
{

	
}



StandardRenderer::~StandardRenderer() throw()
{

	// Views are not owned.
	
}



void StandardRenderer::render( RenderingTick currentRenderingTick ) throw()
{

	OSDL_RENDER_LOG( "Standard rendering! " ) ;
	
	_screen->clear() ;

	for ( list<Ceylan::View *>::iterator it = _registeredViews.begin() ;
			it != _registeredViews.end(); it++ )
		(*it)->renderModel() ;
		
	_screen->update() ;
	
	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler 
	 * is used.
	 *
	 */
	_renderingDone++ ;
	
}




const string StandardRenderer::toString( Ceylan::VerbosityLevels level ) 
	const throw() 
{

	string res = "Standard renderer, last rendering tick was " 
		+ Ceylan::toString( _lastRender )
		+ ", having performed " 
		+ Ceylan::toString( _renderingDone )    + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion: " 
			+ Ceylan::toString( 100 * _renderingDone / ( _renderingDone 
				+ _renderingSkipped ) )	+ "%)" ;
			
	return res ;
	
}



