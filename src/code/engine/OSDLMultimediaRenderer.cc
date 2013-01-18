/* 
 * Copyright (C) 2003-2013 Olivier Boudeville
 *
 * This file is part of the OSDL library.
 *
 * The OSDL library is free software: you can redistribute it and/or modify
 * it under the terms of either the GNU Lesser General Public License or
 * the GNU General Public License, as they are published by the Free Software
 * Foundation, either version 3 of these Licenses, or (at your option) 
 * any later version.
 *
 * The OSDL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License and the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License and of the GNU General Public License along with the OSDL library.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Boudeville (olivier.boudeville@esperide.com)
 *
 */


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

#else // OSDL_DEBUG_RENDERER

#define OSDL_MULTIMEDIA_RENDER_LOG(message)

#endif // OSDL_DEBUG_RENDERER




MultimediaRenderer::MultimediaRenderer( bool registerToScheduler ) :
	Renderer( registerToScheduler ),
	_videoRenderer( 0 ),
	_audioRenderer( 0 )
{

	
}



MultimediaRenderer::~MultimediaRenderer() throw()
{

	// Views are not owned.
	
}




bool MultimediaRenderer::hasVideoRenderer() const
{

	return ( _videoRenderer != 0 ) ;
	
}



VideoRenderer & MultimediaRenderer::getVideoRenderer() const 
{

	if ( _videoRenderer == 0 )
		throw RenderingException( "MultimediaRenderer::getVideoRenderer: "
			"no video renderer registered." ) ;
			
	return * _videoRenderer	;
	
}



void MultimediaRenderer::setVideoRenderer( VideoRenderer & newVideoRenderer )
{

	if ( _videoRenderer != 0 )
		delete _videoRenderer ;
	
	_videoRenderer = & newVideoRenderer ;
	
}



bool MultimediaRenderer::hasAudioRenderer() const
{

	return ( _audioRenderer != 0 ) ;
	
}



AudioRenderer & MultimediaRenderer::getAudioRenderer() const 
{

	if ( _audioRenderer == 0 )
		throw RenderingException( "MultimediaRenderer::getAudioRenderer: "
			"no audio renderer registered." ) ;
			
	return * _audioRenderer	;
	
}



void MultimediaRenderer::setAudioRenderer( AudioRenderer & newAudioRenderer )
{

	if ( _audioRenderer != 0 )
		delete _audioRenderer ;
	
	_audioRenderer = & newAudioRenderer ;
	
}



void MultimediaRenderer::render( RenderingTick currentRenderingTick )
{

	OSDL_MULTIMEDIA_RENDER_LOG( "Rendering ! " ) ;
	
	if ( _videoRenderer != 0 )
		_videoRenderer->render( currentRenderingTick ) ;
	else	
		renderVideo( currentRenderingTick ) ;
		
	if ( _audioRenderer != 0 )	
		_audioRenderer->render( currentRenderingTick ) ;
	else	
		renderAudio( currentRenderingTick ) ;
		
	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler 
	 * is used.
	 *
	 */
	_renderingDone++ ;
	
}



void MultimediaRenderer::onRenderingSkipped( 
	RenderingTick skippedRenderingTick )
{

	OSDL_MULTIMEDIA_RENDER_LOG( "Rendering skipped." ) ;
	_renderingSkipped++ ;
	
}



const string MultimediaRenderer::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Multimedia renderer, " ;
	
	if ( _videoRenderer != 0 )
		res += "having a video renderer (" 
			+ _videoRenderer->toString( Ceylan::low ) + ")" ;
	else
		res += "with no video renderer registered" ;
	
	if ( _audioRenderer != 0 )
		res += ", having an audio renderer (" 
			+ _audioRenderer->toString( Ceylan::low ) + ")" ;
	else
		res += ", with no audio renderer registered" ;
		
	if ( level == Ceylan::low )
		return res ;
		
	res += ". Last rendering tick was " + Ceylan::toString( _lastRender )
		+ ", having performed " + Ceylan::toString( _renderingDone ) 
		+ " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion: " 
			+ Ceylan::toString( 
				100 * _renderingDone / ( _renderingDone + _renderingSkipped ) )
		 	+ "%)" ;
			
	return res ;
	
}

