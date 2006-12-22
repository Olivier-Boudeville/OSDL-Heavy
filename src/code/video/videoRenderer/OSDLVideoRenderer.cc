#include "OSDLVideoRenderer.h"


#include "OSDLScheduler.h"             // for GetExistingScheduler
#include "OSDLMultimediaRenderer.h"    // for MultimediaRenderer

//#include "OSDLCamera.h"       // for Camera



using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;


#ifdef OSDL_DEBUG_VIDEO_RENDERER

#define OSDL_VIDEO_RENDER_LOG(message) send( message ) ;

#else

#define OSDL_VIDEO_RENDER_LOG(message)

#endif




VideoRenderer::VideoRenderer( bool registerToRootRenderer ) throw( RenderingException ) :
	Renderer( /* registerToScheduler */ false )
	//,_internalCamera( 0 )
{

	if ( registerToRootRenderer )
	{
	
		Renderer * renderer ;
		
		try
		{
		
			// Retrieve the root renderer :
			renderer = & Renderer::GetExistingRootRenderer() ;
			
		}
		catch( const RenderingException & e )
		{
			throw RenderingException( 
				"VideoRenderer constructor : no already existing root renderer ("
				+ e.toString() + ") whereas registering had been requested." ) ;
		} 
		
		// Check it is a multimedia renderer indeed :
		MultimediaRenderer * multimediaRenderer = 
			dynamic_cast<MultimediaRenderer *>( renderer ) ;
				
		if ( multimediaRenderer == 0 )
			throw RenderingException( "VideoRenderer constructor : "
				"root renderer is not a multimedia renderer, no registering possible." ) ;
					
		multimediaRenderer->setVideoRenderer( * this ) ;

	}
	
}



VideoRenderer::~VideoRenderer() throw()
{
/*
	if ( _internalCamera != 0 )
		delete _internalCamera ;
*/
}

/*
bool hasCamera() const throw() 
{
	return ( _internalCamera != 0 ) ;
}


Camera & VideoRenderer::getCamera() const throw( RenderingException )
{
	if ( _internalCamera == 0 )
		return * _internalCamera ;
}


void VideoRenderer::setCamera( Camera & newCamera ) throw()
{
	if ( _internalCamera != 0 )
		delete _internalCamera ;
	
	_internalCamera = & newCamera ;
	
}
*/

void VideoRenderer::render( Events::RenderingTick currentRenderingTick ) throw()
{
	OSDL_VIDEO_RENDER_LOG( "Video rendering ! " ) ;
	
	// Beware, currentRenderingTick might be always zero if no scheduler is used.
	_renderingDone++ ;
}


void VideoRenderer::onRenderingSkipped( RenderingTick skippedRenderingTick ) throw()
{

	OSDL_VIDEO_RENDER_LOG( "Video rendering skipped." ) ;
	_renderingSkipped++ ;
}


const string VideoRenderer::toString( Ceylan::VerbosityLevels level ) const throw() 
{

	string res = "Video renderer, last rendering tick was " + Ceylan::toString( _lastRender )
		+ ", having performed " + Ceylan::toString( _renderingDone ) + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion : " 
			+ Ceylan::toString( 100 * _renderingDone / ( _renderingDone + _renderingSkipped ) )
		 	+ "%)" ;
	/*		
	if ( _internalCamera != 0 )
		res += ". Following camera is being used : " 
			+ _internalCamera->toString( level ) ;
	else
		res += ". No camera is currently used" ;		 
	*/	
	return res ;
	
}

