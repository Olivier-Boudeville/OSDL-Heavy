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


#include "OSDLAudioRenderer.h"


#include "OSDLScheduler.h"             // for GetExistingScheduler
#include "OSDLMultimediaRenderer.h"    // for MultimediaRenderer


using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;

using std::string ;



#ifdef OSDL_DEBUG_AUDIO_RENDERER

#define OSDL_AUDIO_RENDER_LOG(message) send( message ) ;

#else

#define OSDL_AUDIO_RENDER_LOG(message)

#endif




AudioRenderer::AudioRenderer( bool registerToRootRenderer ) :
	Renderer( /* registerToScheduler */ false )
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
			throw RenderingException( "AudioRenderer constructor: "
				"no already existing root renderer ("
				+ e.toString() 
				+ ") whereas registering had been requested." ) ;
		} 
		
		// Check it is a multimedia renderer indeed:
		MultimediaRenderer * multimediaRenderer = 
			dynamic_cast<MultimediaRenderer *>( renderer ) ;
				
		if ( multimediaRenderer == 0 )
			throw RenderingException( "AudioRenderer constructor: "
				"root renderer is not a multimedia renderer, "
				"no registering possible." ) ;
					
		multimediaRenderer->setAudioRenderer( * this ) ;

	}
	
}



AudioRenderer::~AudioRenderer() throw()
{
			
}



void AudioRenderer::render( RenderingTick currentRenderingTick )
{

	OSDL_AUDIO_RENDER_LOG( "Audio rendering! " ) ;
	
		
	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler 
	 * is used.
	 *
	 */
	_renderingDone++ ;
	
}



void AudioRenderer::onRenderingSkipped( RenderingTick skippedRenderingTick )
{

	OSDL_AUDIO_RENDER_LOG( "Audio rendering skipped." ) ;
	_renderingSkipped++ ;
	
}



const string AudioRenderer::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Audio renderer, whose last rendering tick was " 
		+ Ceylan::toString( _lastRender )
		+ ", having performed " + Ceylan::toString( _renderingDone ) 
		+ " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;
	
	if ( ( _renderingDone + _renderingSkipped ) != 0 )
		res += " (rendering proportion: " 
			+ Ceylan::toString( 100 * _renderingDone 
				/ ( _renderingDone + _renderingSkipped ) )
		 	+ "%)" ;
			
	return res ;
	
}

