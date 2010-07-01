/*
 * Copyright (C) 2003-2010 Olivier Boudeville
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


#include "OSDLWidget.h"

#include "OSDLFont.h"         // for HorizontalAlignment, WidthCentered, etc.
#include "OSDLFixedFont.h"    // for BasicFontCharacterWidth, etc.



using std::string ;

using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>       // for OSDL_DEBUG_WIDGET and al
#endif // OSDL_USES_CONFIG_H




RedrawRequestEvent::RedrawRequestEvent( Ceylan::EventSource & source ) :
	SurfaceEvent( source )
{

}



RedrawRequestEvent::~RedrawRequestEvent() throw()
{

}




Pixels::ColorDefinition Widget::_EdgeColor  = Pixels::Grey ;
Pixels::ColorDefinition Widget::_TitleColor = Pixels::Black ;


Text::HorizontalAlignment Widget::_TitleHorizontalAlignment
	= Text::WidthCentered ;

Text::VerticalAlignment Widget::_TitleVerticalAlignment
	= Text::HeightCentered ;



/*
 * By default, the client area takes almost the full room
 * (edges are one pixel thick):
 *
 */


// One more pixel on width increases readability:
Length Widget::_ClientOffsetWidth = 2 ;


// Not too much margin since too few lines could be displayed otherwise:
Length Widget::_ClientOffsetHeight = 1 ;



Coordinate Widget::_TitleOffsetAbscissa     = 5 ;
Coordinate Widget::_TitleOffsetOrdinate     = 5 ;
Coordinate Widget::_TitleBarOffsetOrdinate  = 15 ;

std::string Widget::_DefaultTitle = "(anonymous widget)" ;




Widget::Widget( Surface & container, const Point2D & relativePosition,
		Length width, Length height, BaseColorMode baseColorMode,
		Pixels::ColorDefinition baseColor,
		const string & title, bool minMaximizable, bool draggable,
		bool wrappable, bool closable ) :
	Surface(
		container.getFlags(),
		width,
		height,
		container.getBitsPerPixel(),
		container.getPixelFormat().Rmask,
		container.getPixelFormat().Gmask,
		container.getPixelFormat().Bmask,
		container.getPixelFormat().Amask ),
	EventListener( container ),
	_upperLeftCorner( relativePosition ),
	_clientArea( 0, 0, getWidth(), getHeight() ),
	_decorated( false ),
	_title( title ),
	_minMaximizable( minMaximizable ),
	_draggable( draggable ),
	_wrappable( wrappable ),
	_closable( closable ),
	_hasFocus( false ),
	_baseColorMode( NotInitialized ),
	_baseColor( Pixels::Black ),
	_actualBaseColor()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Widget constructor" ) ;
#endif // OSDL_DEBUG_WIDGET


	// _needsRedraw set to true thanks to Surface constructor.

	setBaseColorMode( baseColorMode, baseColor ) ;

	/*
	 * Speed boost: an alpha channel will be used iff the container
	 * too already uses it.
	 *
	 */
	convertToDisplay( /* useAlphaChannel */
		( getFlags() & Surface::AlphaBlendingBlit ) != 0 ) ;

	updateDecorationFlag() ;

	updateClientArea() ;

	// Force the container to have to be redrawn as well:
	getContainer().setRedrawState( true ) ;

}



Widget::~Widget() throw()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Widget destructor" ) ;
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Unsubscribing from container is automatic thanks to the
	 * EventListener inheritance.
	 *
	 */

}



void Widget::setWidth( Length newWidth )
{

	resize( newWidth, getHeight() ) ;

}



void Widget::setHeight( Length newHeight )
{

	resize( getWidth(), newHeight ) ;

}



void Widget::resize( Length newWidth, Length newHeight, bool ignored )
{

	/*
	 * This test is needed so that the client area is updated only if
	 * necessary:
	 *
	 */
	if ( ( newWidth == getWidth() ) && ( newHeight == getHeight() ) )
		return ;

	// Will set _needsRedraw to true:
	Surface::resize( newWidth, newHeight ) ;

	updateClientArea() ;

}



Widget::BaseColorMode Widget::getBaseColorMode() const
{

	return _baseColorMode ;

}



void Widget::setBaseColorMode( BaseColorMode newBaseColorMode,
	Pixels::ColorDefinition newBaseColor )
{

	bool mustUpdateColor = ( newBaseColorMode != NotInitialized ) ||
		! Pixels::areEqual( newBaseColor, _baseColor, /* use alpha */ true ) ;

	bool mustUpdateColorKey = false ;


	if ( newBaseColorMode != _baseColorMode )
	{

		// Here the base color mode changed:

		// Deactivate color key if needed:
		if ( _baseColorMode == Colorkey )
		{

			setColorKey( /* no more color-keying */ 0,
				/* do not care */ _actualBaseColor ) ;

		}
		else
		{

			// Activate color key if needed:
			if ( newBaseColorMode == Colorkey )
				mustUpdateColorKey = true ;

		}

		_baseColorMode = newBaseColorMode ;

	}
	else
	{
		// Here the base color mode did not change, but base color may have:

		if ( ( _baseColorMode == Colorkey ) && mustUpdateColor )
			mustUpdateColorKey = true ;

	}

	if ( mustUpdateColor )
	{

		_baseColor = newBaseColor ;
		_actualBaseColor = Pixels::convertColorDefinitionToPixelColor(
			getContainer().getPixelFormat(), _baseColor ) ;

		setRedrawState( true ) ;

	}


	if ( mustUpdateColorKey )
	{

		setColorKey( ColorkeyBlit | RLEColorkeyBlit, _actualBaseColor ) ;

		setRedrawState( true ) ;

	}

}



Pixels::ColorDefinition Widget::getBaseColor() const
{

	return _baseColor ;

}



void Widget::setDecorationStatus( bool newDecorationStatus )
{

	_decorated = newDecorationStatus ;

}



const UprightRectangle & Widget::getClientArea() const
{

	return _clientArea ;

}



bool Widget::clean()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Widget::clean" ) ;
#endif // OSDL_DEBUG_WIDGET


	switch( _baseColorMode )
	{

		// Both are the same here:
		case BackgroundColor:
		case Colorkey:
			return ( fill( _baseColor ) ) ;
			break ;

		case None:
			return true ;
			break ;

		case NotInitialized:
			LogPlug::error(
				"Widget::clean: base color mode not initialized." ) ;
			return false ;

		default:
			LogPlug::error( "Widget::clean: unknown base color mode." ) ;
			return false ;
			break ;

	}

}



void Widget::beNotifiedOf( const Ceylan::Event & newEvent )
{

	const RedrawRequestEvent * redrawRequestEvent
		= dynamic_cast<const RedrawRequestEvent *>( & newEvent ) ;

	if ( redrawRequestEvent != 0 )
	{

#if OSDL_DEBUG_WIDGET
		LogPlug::trace( "Widget::beNotifiedOf: redraw event received" ) ;
#endif // OSDL_DEBUG_WIDGET

		// Takes in charge the recursive redraw as well for sub-widgets:
		redraw() ;

		return ;
	}

	LogPlug::error( "Widget::beNotifiedOf: unexpected event received: "
		+ newEvent.toString( Ceylan::high ) ) ;

}



void Widget::setRedrawState( bool needsToBeRedrawn )
{

	/*
	 * If redraw state goes from false to true, then propagate the
	 * need of redraw to the container:
	 *
	 */
	if ( ( ! getRedrawState() ) && needsToBeRedrawn )
		getContainer().setRedrawState( true ) ;

	Surface::setRedrawState( needsToBeRedrawn ) ;

}



void Widget::redraw()
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "Widget::redraw: needs redraw attribute is "
		+ Ceylan::toString( getRedrawState() ) + "." ) ;

#endif // OSDL_DEBUG_WIDGET

	/*
	 * Triggers its own redraw then the full recursive redraw for any
	 * internal subwidgets:
	 *
	 */
	Surface::redraw() ;

	// Once done, draw the result on the container:
	try
	{

#if OSDL_DEBUG_WIDGET
		LogPlug::trace( "Widget::redraw: blitting to container" ) ;
#endif // OSDL_DEBUG_WIDGET

		blitTo( getContainer().getWidgetRenderTarget(), _upperLeftCorner ) ;

	}
	catch( const VideoException & e )
	{
		LogPlug::error( "Widget::redraw: blit to container failed: "
			+ e.toString() ) ;
	}

}



void Widget::redrawInternal()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Widget::redrawInternal" ) ;
#endif // OSDL_DEBUG_WIDGET

	drawFundamentals( /* target directly the surface widget */ *this ) ;

	// _needsRedraw is updated:
	if ( _decorated )
		drawDecorations( /* target directly the surface widget */ *this ) ;


	/*
	 * Here _needsRedraw has to be false, otherwise	the whole path to
	 * root widget will be redrawn again and again, with no use.
	 *
	 */
	setRedrawState( false ) ;

}



const string Widget::toString( Ceylan::VerbosityLevels level ) const
{

	std::list<string> widgetList ;

	if ( getRedrawState() )
		widgetList.push_back( "Needs to be redrawn." ) ;
	else
		widgetList.push_back( "Does not need to be redrawn." ) ;


	widgetList.push_back( "Upper-left corner located at "
		+ _upperLeftCorner.toString( Ceylan::medium )
		+ " in the referential of its container." ) ;

	widgetList.push_back( "Widget dimensions: ( width = "
		+ Ceylan::toString( getWidth() )
		+ " ; height = " + Ceylan::toString( getHeight() ) + " )" ) ;

	if ( _decorated )
		widgetList.push_back( "Widget is decorated." ) ;
	else
		widgetList.push_back( "Widget is not decorated." ) ;

	widgetList.push_back( "Widget client area: " + _clientArea.toString() ) ;

	if ( _title.empty() )
		widgetList.push_back( "Widget has no title." ) ;
	else
		widgetList.push_back( "The widget title is '" + _title + "'." ) ;

	if ( _minMaximizable )
		widgetList.push_back(
			"Widget can be minimized and maximized by the user." ) ;
	else
		widgetList.push_back(
			"Widget cannot be minimized and maximized by the user." ) ;

	if ( _draggable )
		widgetList.push_back( "Widget can be dragged by the user." ) ;
	else
		widgetList.push_back( "Widget cannot be dragged by the user." ) ;

	if ( _wrappable )
		widgetList.push_back( "Widget can be wrapped by the user." ) ;
	else
		widgetList.push_back( "Widget cannot be wrapped by the user." ) ;

	if ( _closable )
		widgetList.push_back( "Widget can be closed by the user." ) ;
	else
		widgetList.push_back( "Widget cannot be closed by the user. " ) ;

	if ( _hasFocus )
		widgetList.push_back( "Widget has the focus." ) ;
	else
		widgetList.push_back( "Widget does not have the focus." ) ;


	switch( _baseColorMode )
	{

		case BackgroundColor:
			widgetList.push_back( "Widget uses a background color, which is "
				+ Pixels::toString( _baseColor ) ) ;
			break ;

		case Colorkey:
			widgetList.push_back( "Widget uses a colorkey, which is "
				+ Pixels::toString( _baseColor ) ) ;
			break ;

		case None:
			widgetList.push_back(
				"Widget does not use its base color (which is "
				+ Pixels::toString( _baseColor ) + ")" ) ;
			break ;

		default:
			widgetList.push_back( "Unknown base color mode (abnormal)" ) ;
			break ;

	}


	widgetList.push_back( "Widget internal "
		+ Surface::toString( Ceylan::medium ) ) ;


	widgetList.push_back( "Widget container is: "
		+ getConstContainer().toString( Ceylan::low ) ) ;

	return "Widget: " + Ceylan::formatStringList( widgetList ) ;

}




// Static (public) section.



Pixels::ColorDefinition Widget::GetEdgeColor()
{

	return _EdgeColor ;

}



void Widget::SetEdgeColor( Pixels::ColorDefinition edgeColorDef )
{

	_EdgeColor = edgeColorDef ;

}





// Protected section.



void Widget::updateDecorationFlag()
{

	// True iff already true or at least a decorated attribute selected:
	if ( ! _decorated &&
		( _minMaximizable || _draggable || _wrappable || _closable
			|| ( ! _title.empty() ) ) )
		_decorated = true ;

}



void Widget::updateClientArea()
{

	/*
	 * Defines the biggest possible inner upright rectangle available for
	 * rendering:
	 *
	 */

	_clientArea.setUpperLeftAbscissa( _ClientOffsetWidth ) ;

	_clientArea.setWidth( getWidth() - 2 * _ClientOffsetWidth ) ;

	if ( _decorated )
	{

		_clientArea.setUpperLeftOrdinate( _TitleBarOffsetOrdinate
			+ _ClientOffsetHeight ) ;

		_clientArea.setHeight( getHeight() -  _TitleBarOffsetOrdinate
			- 2 * _ClientOffsetHeight ) ;

	}
	else
	{

		_clientArea.setUpperLeftOrdinate( _ClientOffsetHeight ) ;
		_clientArea.setHeight( getHeight() - 2 * _ClientOffsetHeight ) ;

	}

}



Surface & Widget::getContainer()
{

	// There should be exactly one source in this listener list, its container.

#if OSDL_DEBUG_WIDGET

	if ( _sources.size() != 1 )
		Ceylan::emergencyShutdown( "Widget::getContainer: "
			"not exactly one registered source: "
			+ Ceylan::toString( _sources.size() )
			+ " sources in listener list." ) ;

#endif // OSDL_DEBUG_WIDGET

	// List with only one element:

	Surface * container = dynamic_cast<Surface *>( _sources.back() ) ;

	if ( container == 0 )
		Ceylan::emergencyShutdown( "Widget::getContainer: "
			"listener source was not a surface container." ) ;

	return * container ;

}



const Surface & Widget::getConstContainer() const
{

	// There should be exactly one source in this listener list, its container.

#if OSDL_DEBUG_WIDGET

	if ( _sources.size() != 1 )
		Ceylan::emergencyShutdown( "Widget::getConstContainer: "
			"not exactly one registered source: "
			+ Ceylan::toString( _sources.size() )
			+ " sources in listener list." ) ;

#endif // OSDL_DEBUG_WIDGET

	// List with only one element:

	const Surface * container = dynamic_cast<Surface *>( _sources.back() ) ;

	if ( container == 0 )
		Ceylan::emergencyShutdown( "Widget::getConstContainer: "
			"listener source was not a surface container." ) ;

	return * container ;

}



void Widget::drawFundamentals( Surface & targetSurface )
{

	clean() ;

	// Draw the widget window edges:
	targetSurface.drawEdges( _EdgeColor ) ;

	// Uncomment to see client area:
	//drawBox( _clientArea, Pixels::Yellow, /* filled */ false ) ;

}



bool Widget::isDecorated() const
{

	return _decorated ;

}



void Widget::drawDecorations( Surface & targetSurface )
{

	// Here we know that decorations have to be rendered.

	if ( _title.empty() )
		_title = _DefaultTitle ;

	Coordinate startingAbscissa ;
	Coordinate startingOrdinate ;


	switch( _TitleHorizontalAlignment )
	{

		// Right not implemented yet since mostly useless:

		case Text::Left:
			startingAbscissa = targetSurface.getUpperLeftAbscissa()
				+ _TitleOffsetAbscissa ;
			break ;

		case Text::WidthCentered:
		case Text::Right:
		default:
			// No clipping performed:
			startingAbscissa = targetSurface.getUpperLeftAbscissa()
				+ ( targetSurface.getWidth() -
						static_cast<Coordinate>( _title.size() )
					* Text::BasicFontCharacterWidth ) / 2 ;
			break ;

	}


	switch ( _TitleVerticalAlignment )
	{

		// Bottom not implemented yet since mostly useless:

		case Text::Top:
		 	startingOrdinate = targetSurface.getUpperLeftOrdinate()
				+ _TitleOffsetOrdinate ;
			break ;

		case Text::HeightCentered:
		case Text::Bottom:
		default:
			// No clipping performed:
			startingOrdinate = targetSurface.getUpperLeftOrdinate()
				+ /* to compensate for widget border */ 1
				+ ( _TitleBarOffsetOrdinate - Text::BasicFontCharacterHeight )
					/ 2 ;
			break ;

	}

	targetSurface.printText( _title, startingAbscissa,
		startingOrdinate, _TitleColor ) ;

	targetSurface.drawHorizontalLine( targetSurface.getUpperLeftAbscissa(),
		targetSurface.getWidth(),
		targetSurface.getUpperLeftOrdinate() + _TitleBarOffsetOrdinate,
		_EdgeColor ) ;


}
