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




RenderingException::RenderingException( const std::string & reason ) :
	OSDL::Exception( "RenderingException: " + reason )
{

}



RenderingException::~RenderingException() throw()
{

}




Renderer::Renderer( bool registerToScheduler ) :
	Object(),
	_renderingDone( 0 ),
	_renderingSkipped( 0 ),
	_lastRender( 0 )
{

	if ( registerToScheduler )
	{
		try
		{

			// The scheduler tales ownership of the renderer:
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



void Renderer::render( RenderingTick currentRenderingTick )
{

	OSDL_RENDER_LOG( "Rendering! " ) ;

	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler is
	 * used.
	 *
	 */
	_renderingDone++ ;

}



void Renderer::onRenderingSkipped( RenderingTick skippedRenderingTick )
{

	OSDL_RENDER_LOG( "Rendering skipped." ) ;
	_renderingSkipped++ ;

}



const string Renderer::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Basic renderer, last rendering tick was "
		+ Ceylan::toString( _lastRender )
		+ ", having performed "
		+ Ceylan::toString( _renderingDone )    + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;

	RenderCount totalRendered = _renderingDone + _renderingSkipped ;

	if ( totalRendered != 0 )
		res += " (rendering proportion: "
			+ Ceylan::toString( 100 * _renderingDone / totalRendered ) + "%)" ;

	return res ;

}



bool Renderer::HasExistingRootRenderer()
{

	return ( _internalRootRenderer != 0 ) ;

}



Renderer & Renderer::GetExistingRootRenderer()
{

	if ( Renderer::_internalRootRenderer == 0 )
		throw RenderingException(
			"Renderer::GetExistingRenderer: no Renderer available." ) ;
	return * Renderer::_internalRootRenderer ;

}



void Renderer::DeleteExistingRootRenderer()
{

	if ( Renderer::_internalRootRenderer != 0 )
		throw RenderingException( "Renderer::DeleteExistingRenderer: "
			"there was no already existing Renderer." ) ;

	LogPlug::debug( "Renderer::DeleteExistingRenderer: effective deleting." ) ;

	delete Renderer::_internalRootRenderer ;
	Renderer::_internalRootRenderer = 0 ;

}



void Renderer::DeleteRootRenderer()
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
