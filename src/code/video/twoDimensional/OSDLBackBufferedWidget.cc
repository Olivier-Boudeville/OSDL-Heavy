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


#include "OSDLBackBufferedWidget.h"


#include "OSDLSurface.h"            // for Surface
#include "OSDLPoint2D.h"            // for Point2D


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_DEBUG_WIDGET and al 
#endif // OSDL_USES_CONFIG_H




using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

using std::string ;




BackBufferedWidget::BackBufferedWidget( 
		Surface & container, 
		const Point2D & relativePosition, 
		Length width,
		Length height, 
		BaseColorMode baseColorMode, 
		Pixels::ColorDefinition baseColor,
		const string & title,
		bool minMaximizable, 
		bool draggable, 
		bool wrappable, 
		bool closable ) :
	Widget(
		container, 
		relativePosition, 
		width, 
		height,
		baseColorMode,
		baseColor,
		title, 
		minMaximizable, 
		draggable, 
		wrappable, 
		closable ),	
	_overallSurface( 0 ),
	_needsBackBufferRedraw( true )
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "Back-buffered widget constructor" ) ; 
	
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Creates a shadow surface that will act as a cache for this widget 
	 * own rendering: clones the container except for the dimensions.
	 *
	 */
	_overallSurface = new Surface( 
		container.getFlags(), 
		width, 
		height, 
		container.getBitsPerPixel(), 
		container.getPixelFormat().Rmask, 
		container.getPixelFormat().Gmask,
		container.getPixelFormat().Bmask,
		container.getPixelFormat().Amask
	) ;	
	

	/*
	 * Speed boost: an alpha channel will be used iff the backbuffer 
	 * (hence, the container too) already use it.
	 *
	 */
	_overallSurface->convertToDisplay( /* useAlphaChannel */
		( getFlags() & Surface::AlphaBlendingBlit ) != 0 ) ;
		
}



BackBufferedWidget::~BackBufferedWidget() throw()
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "Back-buffered widget destructor" ) ; 
	
#endif // OSDL_DEBUG_WIDGET

	delete _overallSurface ;
	
}



void BackBufferedWidget::resize( Length newWidth, Length newHeight, 
	bool ignored )
{	
	
	// Sets the 'need for redraw' state to true, and updates the client area:	
	Widget::resize( newWidth, newHeight ) ;
	
	_overallSurface->resize( newWidth, newHeight ) ;
	
	setBackBufferRedrawState( true ) ;
	
}



void BackBufferedWidget::setBaseColorMode( BaseColorMode newBaseColorMode, 
	Pixels::ColorDefinition newBaseColor )
{
	
	/*
	 * Same as Widget::setBaseColorMode except the back-buffer is modified 
	 * as well:
	 *
	 */

	bool mustUpdateColor = ( ( _baseColorMode == NotInitialized )
			 && ( newBaseColorMode != NotInitialized ) ) 
		|| ( ! Pixels::areEqual( newBaseColor, _baseColor ) ) ;
		
	bool mustUpdateColorKey = false ;
	 
	 
	if ( newBaseColorMode != _baseColorMode )
	{
	
		// Here the base color mode changed:
		
		// Deactivate color key if needed:
		if ( _baseColorMode == Colorkey )
		{
			
			// Thus newBaseColorMode != Colorkey, so:
			
			setColorKey( /* no more color-keying */ 0, 
				/* do not care */ _actualBaseColor ) ;
				
			_overallSurface->setColorKey( /* no more color-keying */ 0, 
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
			getPixelFormat(), _baseColor ) ;
		
		// Sets the widget redraw state to true as well:	
		setBackBufferRedrawState( true ) ;
				
	}
	
		
	if ( mustUpdateColorKey )
	{
				
		/*
		 * Comment following two lines to see the colorkey 
		 * (generally, Pixels::Red):
		 *
		 */
		setColorKey( ColorkeyBlit | RLEColorkeyBlit, _actualBaseColor ) ;
		
		_overallSurface->setColorKey( ColorkeyBlit | RLEColorkeyBlit,
			_actualBaseColor ) ;
		
		// Sets the widget redraw state to true as well:	
		setBackBufferRedrawState( true ) ;
	
	}	
		
	
}



void BackBufferedWidget::redraw()
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "BackBufferedWidget::redraw: needs redraw attribute is " 
		+ Ceylan::toString( getRedrawState() ) + "." ) ; 
		
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Triggers its own redraw, then the full recursive redraw for 
	 * all internal subwidgets:
	 *
	 */
	Surface::redraw() ;
	
	// Once done, draw the result on the container:
	try
	{
		
		// Uncomment to debug blits:
		/*
		
		savePNG( Ceylan::toString( this ) + "-redrawn-backbuffer.png" ) ;
				
		_overallSurface->savePNG( Ceylan::toString( this ) 
			+ "-redrawn-frontbuffer.png" ) ;		
			
		*/
		
		_overallSurface->blitTo( getContainer().getWidgetRenderTarget(),
			_upperLeftCorner ) ;
	}
	catch( const VideoException & e )
	{
		LogPlug::error( 
			"BackBufferedWidget::redraw: blit to container failed: " 
			+ e.toString() ) ;
	}	
	
}



void BackBufferedWidget::redrawInternal()
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "BackBufferedWidget::redrawInternal: "
		"needs back-buffer redraw attribute is " 
		+ Ceylan::toString( getBackBufferRedrawState() ) + "." ) ; 
		
#endif // OSDL_DEBUG_WIDGET

	/*
	 * Here we know that this widget and its subwidgets have to be redrawn 
	 * as a whole.
	 *
	 * But this widget may or may not reuse any previously cached own 
	 * rendering.
	 * If not, it will have to paint itself first: 
	 *
	 */
	if ( getBackBufferRedrawState() )
	{
	
		/*
		 * Here the widget has to paint itself again:
		 * (will ultimately set the back-buffer need for redraw state to false)
		 *
		 */
		redrawBackBuffer() ;
	
	}
	else
	{	
	
#if OSDL_DEBUG_WIDGET

		LogPlug::trace( "BackBufferedWidget::redrawInternal:"
			" back-buffer already available." ) ; 
			
#endif // OSDL_DEBUG_WIDGET
		
	}


#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "BackBufferedWidget::redrawInternal: "
		"blitting back-buffer to front buffer." ) ;
		
#endif // OSDL_DEBUG_WIDGET
	
	
	/*
	 * We can now reuse our own previously cached rendering on both cases. 
	 * But before blitting from back-buffer to overall surface, we must 
	 * fill the latter with the color key, if any, otherwise color-keing 
	 * will be lost for the blit from this overall surface to the container.
	 *
	 */
	
	if ( getBaseColorMode() == Colorkey ) 
		_overallSurface->fill( _baseColor ) ;
		
	blitTo( *_overallSurface ) ;
	
	setRedrawState( false ) ;
	
}



void BackBufferedWidget::redrawBackBuffer()
{

#if OSDL_DEBUG_WIDGET

	LogPlug::trace( "BackBufferedWidget::redrawBackBuffer" ) ; 
	
#endif // OSDL_DEBUG_WIDGET


	// (...) [real rendering should take place here in overriden methods]
	
	// This inherited Surface (not _overallSurface!) should be updated here.
		
	
	/*
	 * This mostly empty method should be overriden, but all versions 
	 * should end with:	
	 *
	 */
	setBackBufferRedrawState( false ) ;
	
}



Surface & BackBufferedWidget::getWidgetRenderTarget()
{	

	// Sub-widgets must target overall surface, not back-buffer:
	return *_overallSurface ;
	
}



const string BackBufferedWidget::toString( Ceylan::VerbosityLevels level ) const
{

	string res ;
	
	if ( getBackBufferRedrawState() )
		res += "needs to be redrawn. " ;
	else		
		res += "does not need to be redrawn. " ;
	
	res += "Overall surface is: " + _overallSurface->toString( level ) ;
		
	return "Back-buffered widget description whose back-buffer " 
		+ res + Widget::toString( level ) ;
	
}



void BackBufferedWidget::setBackBufferRedrawState( bool needsToBeRedrawn )
{

	/*
	 * If back-buffer redraw state goes from false to true, then the 
	 * general redraw state should do as well, so that it propagates 
	 * the need of redraw to the container:
	 *
	 */
	if ( ( getBackBufferRedrawState() == false ) && needsToBeRedrawn )
	{
		_needsBackBufferRedraw = true ;
		setRedrawState( true ) ;
	}	

}

	
	
bool BackBufferedWidget::getBackBufferRedrawState() const
{

	return _needsBackBufferRedraw ;
	
}

