#include "OSDLBackBufferedWidget.h"


using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;

using std::string ;




BackBufferedWidget::BackBufferedWidget( Surface & container, const Point2D & relativePosition, 
		Length width, Length height, 
		BaseColorMode baseColorMode, Pixels::ColorDefinition baseColor,
		const string & title,
		bool minMaximizable, bool draggable, bool wrappable, bool closable ) 
			throw( VideoException ) :
	Widget( container, 
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

	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "Back-buffered widget constructor" ) ; 
	#endif

	/*
	 * Creates a shadow surface that will act as a cache for this widget own rendering :
	 * (a clone operation could be used as well)
	 *
	 */
	_overallSurface = new Surface( container.getFlags(), 
		 width, 
		 height, 
		 container.getBitsPerPixel(), 
		 container.getPixelFormat().Rmask, 
		 container.getPixelFormat().Gmask,
		 container.getPixelFormat().Bmask,
		 container.getPixelFormat().Amask ) ;	

	/*
	 * Speed boost : an alpha channel will be used iff the backbuffer (hence, the container too)
	 * already use it.
	 *
	 */
	_overallSurface->convertToDisplay( /* useAlphaChannel */ static_cast<bool>( 
		getFlags() & Surface::AlphaBlendingBlit ) ) ;
		
}


BackBufferedWidget::~BackBufferedWidget() throw()
{

	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "Back-buffered widget destructor" ) ; 
	#endif

	delete _overallSurface ;
	
}


void BackBufferedWidget::resize( Length newWidth, Length newHeight, bool ignored ) throw()	
{	
	
	// Sets the 'need for redraw' state to true, and updates the client area :	
	Widget::resize( newWidth, newHeight ) ;
	
	_overallSurface->resize( newWidth, newHeight ) ;
	
	setBackBufferRedrawState( true ) ;
	
}


void BackBufferedWidget::setBaseColorMode( BaseColorMode newBaseColorMode, 
	Pixels::ColorDefinition newBaseColor ) throw( VideoException )
{
	
	// Same as Widget::setBaseColorMode except the back-buffer is modified as well :

	bool mustUpdateColor = ( 
		( _baseColorMode == NotInitialized ) && ( newBaseColorMode != NotInitialized ) ) 
		|| ( ! Pixels::areEqual( newBaseColor, _baseColor ) ) ;
		
	bool mustUpdateColorKey = false ;
	 
	 
	if ( newBaseColorMode != _baseColorMode )
	{
	
		// Here the base color mode changed :
		
		// Deactivate color key if needed :
		if ( _baseColorMode == Colorkey )
		{
			
			setColorKey( /* no more color-keying */ 0, /* do not care */ _actualBaseColor ) ;
			_overallSurface->setColorKey( /* no more color-keying */ 0, 
				/* do not care */ _actualBaseColor ) ;
		}	
		else
		{
			// Activate color key if needed :
			if ( newBaseColorMode == Colorkey )
				mustUpdateColorKey = true ;
		}
		
		_baseColorMode = newBaseColorMode ;
		
	}
	else
	{
		// Here the base color mode did not change, but base color may have :
		
		if ( ( _baseColorMode == Colorkey ) && mustUpdateColor )
			mustUpdateColorKey = true ;
	}
	
	if ( mustUpdateColor )
	{
	
		_baseColor = newBaseColor ;
		_actualBaseColor = Pixels::convertColorDefinitionToPixelColor(
			getPixelFormat(), _baseColor ) ;
		
		// Sets the widget redraw state to true as well :	
		setBackBufferRedrawState( true ) ;
				
	}
	
		
	if ( mustUpdateColorKey )
	{
				
		// Comment following two lines to see the colorkey (generally, Pixels::Red) :
		setColorKey( ColorkeyBlit | RLEColorkeyBlit, _actualBaseColor ) ;
		_overallSurface->setColorKey( ColorkeyBlit | RLEColorkeyBlit, _actualBaseColor ) ;
		
		// Sets the widget redraw state to true as well :	
		setBackBufferRedrawState( true ) ;
	
	}	
		
	
}


void BackBufferedWidget::redraw() throw() 
{

	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "BackBufferedWidget::redraw : needs redraw attribute is " 
		+ Ceylan::toString( getRedrawState() ) + "." ) ; 
	#endif

	// Triggers its own redraw then the full recursive redraw for any internal subwidgets :
	Surface::redraw() ;
	
	// Once done, draw the result on the container :
	try
	{
		
		// Uncomment to debug blits :
		/*
		savePNG( Ceylan::toString( this ) + "-redrawn-backbuffer.png" ) ;		
		_overallSurface->savePNG( Ceylan::toString( this ) + "-redrawn-frontbuffer.png" ) ;		
		*/
		
		_overallSurface->blitTo( getContainer().getWidgetRenderTarget(), _upperLeftCorner ) ;
	}
	catch( const VideoException & e )
	{
		LogPlug::error( "BackBufferedWidget::redraw : blit to container failed : " 
			+ e.toString() ) ;
	}	
	
}


void BackBufferedWidget::redrawInternal() throw() 
{

	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "BackBufferedWidget::redrawInternal : needs back-buffer redraw attribute is " 
		+ Ceylan::toString( getBackBufferRedrawState() ) + "." ) ; 
	#endif

	/*
	 * Here we know that this widget and its subwidgets have to be redrawn as a whole.
	 * But this widget may or may not reuse any previously cached own rendering.
	 * If not, it will have to paint itself first : 
	 *
	 */
	if ( getBackBufferRedrawState() )
	{
	
		/*
		 * Here the widget has to paint itself again :
		 * (will ultimately set the back-buffer need for redraw state to false)
		 *
		 */
		redrawBackBuffer() ;
	
	}
	else
	{	
	
		#ifdef OSDL_DEBUG_WIDGET
		LogPlug::trace( "BackBufferedWidget::redrawInternal : back-buffer already available." ) ; 
		#endif
		
	}


	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "BackBufferedWidget::redrawInternal : blitting back-buffer to front buffer." ) ;
	#endif
	
	/*
	 * We can reuse our own previously cached rendering on both cases. But before blitting from
	 * back-buffer to overall surface, we must fill the latter with the color key, if any
	 * (otherwise color-keing will be lost for the blit from this overall surface to the container)
	 *
	 */
	
	if ( getBaseColorMode() == Colorkey ) 
		_overallSurface->fill( _baseColor ) ;
		
	blitTo( *_overallSurface ) ;
	
	setRedrawState( false ) ;
	
}


void BackBufferedWidget::redrawBackBuffer() throw()
{

	#ifdef OSDL_DEBUG_WIDGET
	LogPlug::trace( "BackBufferedWidget::redrawBackBuffer" ) ; 
	#endif

	// (...) [real rendering]
	
	// This inherited Surface (not _overallSurface !) should be updated here.
		
	
	// This mostly empty method should be overriden, but all versions should end with :	
	setBackBufferRedrawState( false ) ;
	
}


Surface & BackBufferedWidget::getWidgetRenderTarget() throw()
{	
	// Sub-widgets must target overall surface, not back-buffer :
	return *_overallSurface ;
}


const string BackBufferedWidget::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res ;
	
	if ( getBackBufferRedrawState() )
		res += "needs to be redrawn. " ;
	else		
		res += "does not need to be redrawn. " ;
	
	res += "Overall surface is : " + _overallSurface->toString( level ) ;
		
	return "Back-buffered widget description whose back-buffer " + res + Widget::toString( level ) ;
	
}


void BackBufferedWidget::setBackBufferRedrawState( bool needsToBeRedrawn ) throw()
{

	/*
	 * If back-buffer redraw state goes from false to true, then the general redraw state should 
	 * do as well, so that it propagates the need of redraw to the container :
	 *
	 */
	if ( ( ! getBackBufferRedrawState() ) && needsToBeRedrawn )
	{
		_needsBackBufferRedraw = true ;
		setRedrawState( true ) ;
	}	

}

	
bool BackBufferedWidget::getBackBufferRedrawState() const throw()
{
	return _needsBackBufferRedraw ;
}

	
