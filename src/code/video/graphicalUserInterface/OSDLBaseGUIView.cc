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


#include "OSDLBaseGUIView.h"



using namespace OSDL::Rendering ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG_RENDERER and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_USES_AGAR

#include <agar/config/have_opengl.h>

#include <agar/core.h>
#include <agar/gui.h>
#include <agar/gui/opengl.h>

#endif // OSDL_USES_AGAR


#include "Ceylan.h"           // for BaseView

using namespace Ceylan::Log ;

using std::string ;



BaseGUIView::BaseGUIView()
{

	/*
	 * Nothing is done here, each child class is expected to create the widget
	 * it will need in its own constructor.
	 *
	 */

}



BaseGUIView::~BaseGUIView() throw()
{

}



void BaseGUIView::render()
{

#if OSDL_USES_AGAR

  // Triggers the rendering of all Agar elements:

  AG_Driver * agarDriver = AGDRIVER( agDriverSw ) ;

  AG_BeginRendering( agarDriver ) ;

  AG_LockVFS( agarDriver ) ;

  AG_Window * win ;

  AG_FOREACH_WINDOW( win, agarDriver )
  {

	AG_ObjectLock( win ) ;
	AG_WindowDraw( win ) ;
	AG_ObjectUnlock( win ) ;

  }

  AG_UnlockVFS( agarDriver ) ;

  AG_EndRendering( agarDriver ) ;

  // Refresh rate (rCur, rNom, etc.) not used here.

#else // OSDL_USES_AGAR

	throw GUIException(
		"BaseGUIView::render failed: no GUI support available." ) ;

#endif // OSDL_USES_AGAR

}



const std::string BaseGUIView::toString( Ceylan::VerbosityLevels level ) const
{

	return "Base GUI View." ;

}
