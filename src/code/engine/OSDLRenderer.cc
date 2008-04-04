#include "OSDLRenderer.h"

#include "OSDLScheduler.h"    // for GetExistingScheduler



using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG_RENDERER and al 
#endif // OSDL_USES_CONFIG_H


#if OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message) send( message ) ;

#else // OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_RENDERER


Renderer * Renderer::_internalRootRenderer = 0 ;



RenderingException::RenderingException( const std::string & reason ) throw():
	OSDL::Exception( "RenderingException: " + reason )
{

}


RenderingException::~RenderingException() throw()
{

}



Renderer::Renderer( bool registerToScheduler ) throw( RenderingException ):
	Object(),
	_renderingDone( 0 ),
	_renderingSkipped( 0 ),
	_lastRender( 0 )
{

	if ( registerToScheduler )
	{
		try
		{
			Scheduler::GetExistingScheduler().setRenderer( * this ) ;
		}
		catch( const RenderingException & e )
		{
			throw RenderingException( 
				"Renderer constructor: no already existing scheduler ("
				+ e.toString() + ") whereas registering had been requested." ) ;
		} 
	}
	
}



Renderer::~Renderer() throw()
{

	// Views are not owned.
	
}



void Renderer::render( RenderingTick currentRenderingTick ) throw()
{

	OSDL_RENDER_LOG( "Rendering ! " ) ;
	
	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler 
	 * is used.
	 *
	 */
	_renderingDone++ ;
	
}



void Renderer::onRenderingSkipped( RenderingTick skippedRenderingTick )	throw()
{

	OSDL_RENDER_LOG( "Rendering skipped." ) ;
	_renderingSkipped++ ;
	
}



const string Renderer::toString( Ceylan::VerbosityLevels level ) const throw() 
{

	string res = "Basic renderer, last rendering tick was " 
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



bool Renderer::HasExistingRootRenderer() throw()
{

	return ( _internalRootRenderer != 0 ) ;
	
}



Renderer & Renderer::GetExistingRootRenderer() throw( RenderingException )
{

    if ( Renderer::_internalRootRenderer == 0 )
		throw RenderingException( 
			"Renderer::GetExistingRenderer: no Renderer available." ) ;
    return * Renderer::_internalRootRenderer ;

}



void Renderer::DeleteExistingRootRenderer() throw( RenderingException )
{

    if ( Renderer::_internalRootRenderer != 0 )
		throw RenderingException( "Renderer::DeleteExistingRenderer: "
			"there was no already existing Renderer." ) ;
			
    LogPlug::debug( "Renderer::DeleteExistingRenderer: effective deleting." ) ;

    delete Renderer::_internalRootRenderer ;
	Renderer::_internalRootRenderer = 0 ;

}



void Renderer::DeleteRootRenderer() throw()
{

    if ( Renderer::_internalRootRenderer != 0 )
    {
	
        LogPlug::debug( "Renderer::DeleteRenderer: effective deleting." ) ;

        delete Renderer::_internalRootRenderer ;
		Renderer::_internalRootRenderer = 0 ;
		
    }
    else
    {
	
        LogPlug::debug( "Renderer::DeleteRenderer: no deleting needed." ) ;
		
    }

}

