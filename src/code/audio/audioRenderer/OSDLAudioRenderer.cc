#include "OSDLAudioRenderer.h"


#include "OSDLScheduler.h"             // for GetExistingScheduler
#include "OSDLMultimediaRenderer.h"    // for MultimediaRenderer

//#include "OSDLCamera.h"       // for Camera



using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;


#ifdef OSDL_DEBUG_AUDIO_RENDERER

#define OSDL_AUDIO_RENDER_LOG(message) send( message ) ;

#else

#define OSDL_AUDIO_RENDER_LOG(message)

#endif




AudioRenderer::AudioRenderer( bool registerToRootRenderer ) 
		throw( RenderingException ) :
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
			throw RenderingException( "AudioRenderer constructor : "
				"no already existing root renderer ("
				+ e.toString() 
				+ ") whereas registering had been requested." ) ;
		} 
		
		// Check it is a multimedia renderer indeed :
		MultimediaRenderer * multimediaRenderer = 
			dynamic_cast<MultimediaRenderer *>( renderer ) ;
				
		if ( multimediaRenderer == 0 )
			throw RenderingException( "AudioRenderer constructor : "
				"root renderer is not a multimedia renderer, "
				"no registering possible." ) ;
					
		multimediaRenderer->setAudioRenderer( * this ) ;

	}
	
}



AudioRenderer::~AudioRenderer() throw()
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


Camera & AudioRenderer::getCamera() const throw( RenderingException )
{
	if ( _internalCamera == 0 )
		return * _internalCamera ;
}


void AudioRenderer::setCamera( Camera & newCamera ) throw()
{
	if ( _internalCamera != 0 )
		delete _internalCamera ;
	
	_internalCamera = & newCamera ;
	
}
*/

void AudioRenderer::render( Events::RenderingTick currentRenderingTick ) throw()
{

	OSDL_AUDIO_RENDER_LOG( "Audio rendering ! " ) ;
	
	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler 
	 * is used.
	 *
	 */
	_renderingDone++ ;
	
}


void AudioRenderer::onRenderingSkipped( RenderingTick skippedRenderingTick )
	throw()
{

	OSDL_AUDIO_RENDER_LOG( "Audio rendering skipped." ) ;
	_renderingSkipped++ ;
	
}


const string AudioRenderer::toString( Ceylan::VerbosityLevels level ) 
	const throw() 
{

	string res = "Audio renderer, last rendering tick was " 
		+ Ceylan::toString( _lastRender )
		+ ", having performed " + Ceylan::toString( _renderingDone ) 
		+ " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion : " 
			+ Ceylan::toString( 100 * _renderingDone 
				/ ( _renderingDone + _renderingSkipped ) )
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

