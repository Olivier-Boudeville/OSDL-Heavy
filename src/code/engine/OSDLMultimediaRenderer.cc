#include "OSDLMultimediaRenderer.h"


#include "OSDLVideoRenderer.h"  // for VideoRenderer
#include "OSDLAudioRenderer.h"  // for AudioRenderer


using namespace Ceylan::Log ;

using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;


#ifdef OSDL_DEBUG_RENDERER

#define OSDL_MULTIMEDIA_RENDER_LOG(message) send( message ) ;

#else

#define OSDL_MULTIMEDIA_RENDER_LOG(message)

#endif



MultimediaRenderer::MultimediaRenderer( bool registerToScheduler ) throw( RenderingException ) :
	Renderer( registerToScheduler ),
	_videoRenderer( 0 ),
	_audioRenderer( 0 )
{

	
}


MultimediaRenderer::~MultimediaRenderer() throw()
{
	// Views are not owned.
}



bool MultimediaRenderer::hasVideoRenderer() const throw()
{
	return ( _videoRenderer != 0 ) ;
}


VideoRenderer & MultimediaRenderer::getVideoRenderer() const throw( RenderingException )
{
	if ( _videoRenderer == 0 )
		throw RenderingException( "MultimediaRenderer::getVideoRenderer : "
			"no video renderer registered." ) ;
			
	return * _videoRenderer	;
	
}


void MultimediaRenderer::setVideoRenderer( VideoRenderer & newVideoRenderer ) throw()
{
	if ( _videoRenderer != 0 )
		delete _videoRenderer ;
	
	_videoRenderer = & newVideoRenderer ;
}


bool MultimediaRenderer::hasAudioRenderer() const throw()
{
	return ( _audioRenderer != 0 ) ;
}


AudioRenderer & MultimediaRenderer::getAudioRenderer() const throw( RenderingException )
{
	if ( _audioRenderer == 0 )
		throw RenderingException( "MultimediaRenderer::getAudioRenderer : "
			"no audio renderer registered." ) ;
			
	return * _audioRenderer	;
	
}


void MultimediaRenderer::setAudioRenderer( AudioRenderer & newAudioRenderer ) throw()
{
	if ( _audioRenderer != 0 )
		delete _audioRenderer ;
	
	_audioRenderer = & newAudioRenderer ;
}


void MultimediaRenderer::render( Events::RenderingTick currentRenderingTick ) throw()
{
	OSDL_MULTIMEDIA_RENDER_LOG( "Rendering ! " ) ;
	
	if ( _videoRenderer != 0 )
		renderVideo( currentRenderingTick ) ;
		
	if ( _audioRenderer != 0 )	
		renderAudio( currentRenderingTick ) ;
		
	// Beware, currentRenderingTick might be always zero if no scheduler is used.
	_renderingDone++ ;
	
}


void MultimediaRenderer::onRenderingSkipped( RenderingTick skippedRenderingTick )	throw()
{

	OSDL_MULTIMEDIA_RENDER_LOG( "Rendering skipped." ) ;
	_renderingSkipped++ ;
}


const string MultimediaRenderer::toString( Ceylan::VerbosityLevels level ) const throw() 
{

	string res = "Multimedia renderer, " ;
	
	if ( _videoRenderer != 0 )
		res += "having a video renderer (" + _videoRenderer->toString( Ceylan::low ) + ")" ;
	else                                                                                                                                                                                                                                                            
		res += "with no video renderer registered" ;
	
	if ( _audioRenderer != 0 )
		res += ", having an audio renderer (" + _audioRenderer->toString( Ceylan::low ) + ")" ;
	else
		res += ", with no audio renderer registered" ;
		
	if ( level == Ceylan::low )
		return res ;
		
	res += ". Last rendering tick was " + Ceylan::toString( _lastRender )
		+ ", having performed " + Ceylan::toString( _renderingDone ) + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion : " 
			+ Ceylan::toString( 100 * _renderingDone / ( _renderingDone + _renderingSkipped ) )
		 	+ "%)" ;
			
	return res ;
	
}



