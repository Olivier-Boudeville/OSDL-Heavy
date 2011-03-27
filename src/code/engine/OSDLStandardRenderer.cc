/*
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDLStandardRenderer.h"

#include "OSDLScheduler.h"    // for GetExistingScheduler
#include "OSDLSurface.h"      // for Surface



using namespace OSDL::Events ;
using namespace OSDL::Rendering ;
using namespace OSDL::Engine ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG_RENDERER and al
#endif // OSDL_USES_CONFIG_H



#include "Ceylan.h"           // for BaseView

using namespace Ceylan::Log ;

using std::string ;
using std::list ;




#if OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message) send( message ) ;

#else // OSDL_DEBUG_RENDERER

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_RENDERER





StandardRenderer::StandardRenderer( Video::Surface & screen,
		bool registerToScheduler ) :
	Renderer( registerToScheduler ),
	_screen( & screen )
{

	string message = "StandardRenderer created, " ;

	if ( ! registerToScheduler )
		message += "not " ;

	message += "registred to scheduler." ;

	send( message ) ;

}



StandardRenderer::~StandardRenderer() throw()
{

	send( "Deleting StandardRenderer." ) ;

	// Views are not owned.

	Ceylan::Uint32 viewCount = _registeredViews.size() ;

	if ( viewCount != 0 )
		LogPlug::warning( "StandardRenderer destructor: "
			+ Ceylan::toString( viewCount ) + " view(s) still registered." ) ;

	send( "StandardRenderer deleted." ) ;

}




void StandardRenderer::registerView( Ceylan::MVC::BaseView & view )
{

	_registeredViews.push_back( & view ) ;

}



void StandardRenderer::unregisterView( Ceylan::MVC::BaseView & view )
{

	list<Ceylan::MVC::BaseView *>::iterator it ;

	for ( it = _registeredViews.begin(); it != _registeredViews.end(); it++ )
	{

		if ( (*it) == & view )
		{

			_registeredViews.erase( it ) ;

			return ;

		}

	}

	if ( it == _registeredViews.end() )
		throw RenderingException( "StandardRenderer::unregisterView: view '"
			+ view.toString() + "' was not registered." ) ;

}



void StandardRenderer::render( RenderingTick currentRenderingTick )
{

	OSDL_RENDER_LOG( "Standard rendering!" ) ;

	_screen->clear() ;


	for ( list<Ceylan::MVC::BaseView *>::iterator it = _registeredViews.begin();
			it != _registeredViews.end(); it++ )
		(*it)->render() ;

	_screen->update() ;

	/*
	 * Beware, currentRenderingTick might be always zero if no scheduler is
	 * used.
	 *
	 */
	_renderingDone++ ;

}



void StandardRenderer::onRenderingSkipped(
	Events::RenderingTick skippedRenderingTick )
{

	OSDL_RENDER_LOG( "Rendering tick "
		+ Ceylan::toString( skippedRenderingTick ) + " had to be skipped." ) ;

	/*
	 TO-DO: add an OSDL base view, inheriting from Ceylan's one
	 *and* from OSDL::Engine::ScheduledView which supports an onSkip/1 method

	for ( list<Ceylan::MVC::BaseView *>::iterator it = _registeredViews.begin();
			it != _registeredViews.end(); it++ )
		(*it)->onSkip( skippedRenderingTick ) ;
	 */
}



const string StandardRenderer::toString( Ceylan::VerbosityLevels level ) const
{

	string res = "Standard renderer, last rendering tick was "
		+ Ceylan::toString( _lastRender )
		+ ", having performed "
		+ Ceylan::toString( _renderingDone )    + " rendering(s) for "
		+ Ceylan::toString( _renderingSkipped ) + " skip(s)" ;

	if ( _renderingDone + _renderingSkipped != 0 )
		res += " (rendering proportion: "
			+ Ceylan::toString( 100 * _renderingDone / ( _renderingDone
				+ _renderingSkipped ) ) + "%)" ;

	return res ;

}
