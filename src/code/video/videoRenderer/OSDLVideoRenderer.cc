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


#include "OSDLVideoRenderer.h"


#include "OSDLScheduler.h"             // for GetExistingScheduler
#include "OSDLMultimediaRenderer.h"    // for MultimediaRenderer

//#include "OSDLCamera.h"               for Camera



using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>                // for OSDL_DEBUG_VIDEO_RENDERER and al
#endif // OSDL_USES_CONFIG_H



#if OSDL_DEBUG_VIDEO_RENDERER

#define OSDL_VIDEO_RENDER_LOG(message) send( message ) ;

#else // OSDL_DEBUG_VIDEO_RENDERER

#define OSDL_VIDEO_RENDER_LOG(message)

#endif // OSDL_DEBUG_VIDEO_RENDERER




VideoRenderingException::VideoRenderingException( const string & message ) :
	RenderingException( "Video rendering exception: " + message )
{

}



VideoRenderingException::~VideoRenderingException() throw()
{

}





VideoRenderer::VideoRenderer( bool registerToRootRenderer ) :
	Renderer( /* registerToScheduler */ false )
	//,_internalCamera( 0 )
{

	if ( registerToRootRenderer )
	{

		Renderer * renderer ;

		try
		{

			// Retrieve the root renderer:
			renderer = & Renderer::GetExistingRootRenderer() ;

		}
		catch( const RenderingException & e )
		{
			throw VideoRenderingException( "VideoRenderer constructor: "
				"no already existing root renderer ("
				+ e.toString()
				+ ") whereas registering had been requested." ) ;
		}

		// Check it is a multimedia renderer indeed:
		MultimediaRenderer * multimediaRenderer =
			dynamic_cast<MultimediaRenderer *>( renderer ) ;

		if ( multimediaRenderer == 0 )
			throw VideoRenderingException( "VideoRenderer constructor: "
				"root renderer is not a multimedia renderer, "
				"no registering possible." ) ;

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
bool hasCamera() const
{
	return ( _internalCamera != 0 ) ;
}



Camera & VideoRenderer::getCamera() const
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



void VideoRenderer::render( RenderingTick currentRenderingTick )
{

	OSDL_VIDEO_RENDER_LOG( "Video rendering! " ) ;

	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler is
	 * used.
	 *
	 */
	_renderingDone++ ;

}



void VideoRenderer::onRenderingSkipped( RenderingTick skippedRenderingTick )
{

	OSDL_VIDEO_RENDER_LOG( "Video rendering skipped." ) ;

	_renderingSkipped++ ;

}



const string VideoRenderer::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Video renderer, last rendering tick was "
		+ Ceylan::toString( _lastRender )
		+ ", having performed "
		+ Ceylan::toString( _renderingDone ) + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;

	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion: "
			+ Ceylan::toString(
				100 * _renderingDone / ( _renderingDone + _renderingSkipped ) )
			+ "%)" ;

	/*
	if ( _internalCamera != 0 )
		res += ". Following camera is being used: "
			+ _internalCamera->toString( level ) ;
	else
		res += ". No camera is currently used" ;
	*/

	return res ;

}
