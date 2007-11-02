#include "OSDLTextWidget.h"

#include "OSDLFont.h"       // for Font


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>     // for OSDL_DEBUG_WIDGET and al 
#endif // OSDL_USES_CONFIG_H


using std::string ;

using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;


const Length TextWidget::DefaultEdgeWidth = 3 ;


const Length TextWidget::_TextWidthOffset  = 4 ;
const Length TextWidget::_TextHeightOffset = 4 ;



TextWidget::TextWidget( 
		Surface & container, 
		const Point2D & relativePosition, 
		Length width, 
		Length maximumHeight, 
		Shape shape, 
		Pixels::ColorDefinition textColor,
		Pixels::ColorDefinition edgeColor, 
		Pixels::ColorDefinition backgroundColor, 
		const std::string & text, 
		Text::Font & font,
		bool minimumHeight,
		bool verticallyAligned,
		bool justified,
		Text::Font::RenderQuality quality,
		const string & title,
		bool minMaximizable, 
		bool draggable, 
		bool wrappable, 
		bool closable ) throw( VideoException ):
	BackBufferedWidget( 
			 container,
			 relativePosition, 
			 width, 
			 maximumHeight   /* height and maximum height start equal */, 
			 NotInitialized  /* will be set later in this constructor */,
			 /* will be set later in this constructor for RoundCorners: */
			 backgroundColor, 
			 title,
			 minMaximizable,
			 draggable, 
			 wrappable, 
			 closable ),
		_minimumHeight( minimumHeight ),	 
		_maximumHeight( maximumHeight ),
		_shape( shape ),
		_textColor( textColor ),
		_edgeColor( edgeColor ),
		_backColorForRoundCorners( backgroundColor ),
		_roundOffset(),
		_text( text ),
		_font( & font ),
		_verticallyAligned( verticallyAligned ) ,
		_justified( justified ),
		_quality( quality ),
		_currentIndex( 0 )
{


#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "Text widget constructor" ) ; 
#endif // OSDL_DEBUG_WIDGET
	
	switch( _shape )
	{
	
		case SquareCorners:
			setBaseColorMode( BackgroundColor, backgroundColor ) ;
			break ;
			
		case RoundCorners:			
			// Activates a color key which cannot collide with other colors:
			setBaseColorMode( Colorkey, 
				Pixels::selectColorDifferentFrom( _textColor, _edgeColor,
				_backColorForRoundCorners ) ) ;
			break ;
			
		default:
			LogPlug::error( 
				"TextWidget constructor: unexpected shape specified." ) ;
			break ;
			
	}		
	
	
	/*
	 * Must be called in this constructor as well, so that the overriden 
	 * version is used:
	 *
	 */
	updateClientArea() ;
		
	
	/*
	 * _needsRedraw set to true thanks to Surface constructor.
	 *
	 * _needsBackbufferRedraw set to true thanks to BackBufferedWidget
	 * constructor.
	 *
	 */
	
}



TextWidget::~TextWidget() throw()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "TextWidget widget destructor" ) ; 
#endif // OSDL_DEBUG_WIDGET
	
	// Do not touch to _font, which is not owned.

}


void TextWidget::setText( const std::string & newText ) throw()
{

	_text = newText ;
	
	// Reset the height to the user-provided one:
	setHeight( _maximumHeight ) ;
	
	updateClientArea() ;
	
}



const std::string & TextWidget::getText() const throw()
{

	return _text ;
	
}



Text::TextIndex TextWidget::getRenderIndex() const throw()
{

	return _currentIndex ;
	
}



void TextWidget::redrawBackBuffer() throw()
{

#if OSDL_DEBUG_WIDGET
	LogPlug::trace( "TextWidget::redrawBackBuffer" ) ; 
#endif // OSDL_DEBUG_WIDGET

	Coordinate lastOrdinateUsed ;
	Surface * renderedText = 0 ;
	
	Coordinate blitWidth, blitHeight ;
			
	try
	{
			
		// First have the text rendered in a separate surface:	
		renderedText = & _font->renderLatin1MultiLineText( 
			_clientArea.getWidth(), _clientArea.getHeight(), _text, 
			/* render index */ _currentIndex, lastOrdinateUsed, 
			_quality, _textColor, _justified ) ;
			
	}
	catch( const TextException & e )
	{
	
		LogPlug::error( "TextWidget::redrawBackBuffer failed: " 
			+ e.toString() ) ;
		
		return ;	
		
	}

	// Uncomment to see text box and to check lowest pixel row used by text:
	/*
	renderedText->drawEdges( Pixels::Green ) ;
	renderedText->drawHorizontalLine( 0, renderedText->getWidth(),
		lastOrdinateUsed, Pixels::Green ) ;					
	*/
	

	if ( _shape == SquareCorners )
	{	
		
		
		/*
		 * Here no color key is to be handled, since rectangular shapes 
		 * take the full room.
		 *
		 */
		

		/*
		 * Reduce the height if requested and possible 
		 * (i.e. if text smaller than client area):
		 *
		 */
		if ( _minimumHeight && ( lastOrdinateUsed < _clientArea.getHeight() ) )
		{
				
#if OSDL_DEBUG_WIDGET

			// Should never happen:
			if ( lastOrdinateUsed + 2 * _ClientOffsetHeight 
					+ _TitleBarOffsetOrdinate >	getHeight() )
				Ceylan::emergencyShutdown( "TextWidget::redrawBackBuffer: "
					"specified maximum height exceeded whereas shrinking" ) ;
					
#endif // OSDL_DEBUG_WIDGET
				
			/*
			 * Recompute total height from newer client height
			 * (automatically updates client area):
			 *
			 */
			setHeight( lastOrdinateUsed + 2 * _ClientOffsetHeight 
				+ _TitleBarOffsetOrdinate ) ;
			
			/*
			 * Resize operation erased all, hence draw the basic widget
			 * decorations in back-buffer:
			 *
			 */
			drawFundamentals( *this ) ;
 
 			if ( _decorated ) 
				drawDecorations( *this ) ;
				
		}	
				
		blitWidth  = _clientArea.getUpperLeftAbscissa() ;
		blitHeight = _clientArea.getUpperLeftOrdinate() ;
	
		if ( _verticallyAligned )
			blitHeight += ( _clientArea.getHeight() - lastOrdinateUsed ) / 2 ;
			
	}	
	else
	{

		// Here we suppose the shape is round corners.

		/*
		 * This code is somewhat hazardous, insofar we downsize the 
		 * round rectangle so that it contains just the actual surface 
		 * of rendered text, whereas the client area has been computed 
		 * with the older radius.
		 *
		 * However, since here the only change that can take place is 
		 * the height being decreased, though it is not stricty proven 
		 * it should at most lead to a decrease in the radius that
		 * should result in a client area large enough so that the 
		 * rendered text should fit.
		 * Moreover, the decrease of radius leads to a wider client area !
		 *
		 * It results in having a client area a bit wider and taller 
		 * than before the text was rendered, hence the need to align
		 * it both vertically and horizontally.
		 *
		 */
		 
		/*
		 * Reduce the height if requested and possible 
		 * (i.e. if text smaller than client area):
		 *
		 */
		if ( _minimumHeight && ( lastOrdinateUsed < _clientArea.getHeight() ) )
		{

#if OSDL_DEBUG_WIDGET

			LogPlug::trace( 
				"TextWidget::redrawBackBuffer for round rectangle: "
				"lastOrdinateUsed = " + Ceylan::toString( lastOrdinateUsed ) 
				+ ", client area height = " 
				+ Ceylan::toString( _clientArea.getHeight() ) ) ;
				
#endif // OSDL_DEBUG_WIDGET
				
			/*
			 * Recompute total height from newer client height 
			 * (automatically updates client area):
			 *
			 */
			setHeight( lastOrdinateUsed + 2 * _roundOffset ) ;

			
#if OSDL_DEBUG_WIDGET
			
			if ( _clientArea.getWidth() < renderedText->getWidth() )
				Ceylan::emergencyShutdown( "TextWidget::redrawBackBuffer: "
					"resize for round rectangle led to a client area "
					"not wide enough: client area width = " 
					+ Ceylan::toString( _clientArea.getWidth() ) 
					+ ", rendered text width = " 
					+ Ceylan::toString( renderedText->getWidth() ) ) ;
					
			if ( _clientArea.getHeight() < lastOrdinateUsed )
				Ceylan::emergencyShutdown( "TextWidget::redrawBackBuffer: "
					"resize for round rectangle led to a client area "
					"not tall enough: client area height = " 
					+ Ceylan::toString( _clientArea.getHeight() ) 
					+ ", rendered text height = " 
					+ Ceylan::toString( renderedText->getHeight() ) ) ;

#endif // OSDL_DEBUG_WIDGET

								
		}	
		
		/*
		 * Color key is to be managed so that the area outside the corners 
		 * does not hide anything behind it.
		 *
		 * Hence we fill this surface (the back-buffer) with the already
		 * selected color key:
		 *
		 */		
		fill( _baseColor ) ;
		
						
		// Let's draw the overall rounded rectangle:
		drawWithRoundedCorners( *this, DefaultEdgeWidth, 
			_edgeColor, _backColorForRoundCorners ) ;


		/*
		 * Force alignment for both dimensions (_verticallyAligned 
		 * deemed always true here):
		 *
		 */
				
		blitWidth  = _clientArea.getUpperLeftAbscissa() +
			( _clientArea.getWidth() - renderedText->getWidth() ) / 2 ;	
				
		blitHeight = _clientArea.getUpperLeftOrdinate() + 
			( _clientArea.getHeight() - lastOrdinateUsed ) / 2 ;
			
	}	
	

#if OSDL_DEBUG_WIDGET

	/*

	static Ceylan::Uint32 count = 1 ;
			
	renderedText->savePNG( "rendered-text-" 
		+ Ceylan::toString( count ) + ".png" ) ;
	
	count++ ;

	LogPlug::debug( "TextWidget::redrawBackBuffer: client area is " 
		+ _clientArea.toString() 
		+ ", starting abscissa is "+ Ceylan::toString( blitWidth ) 
		+ ", starting ordinate is "+ Ceylan::toString( blitHeight ) 
		+ ", rendered text is " + renderedText->toString() 
		+ ", last ordinate is " + Ceylan::toString( lastOrdinateUsed ) ) ;
		
	*/
		
#endif // OSDL_DEBUG_WIDGET
		

	// Uncomment to check client area:
	//drawBox( _clientArea, Pixels::Yellow, /* filled */ false ) ;
	//_overallSurface->drawBox( _clientArea, Pixels::Yellow, 
	//	/* filled */ false ) ;
		
	renderedText->blitTo( *this, blitWidth, blitHeight ) ;

	delete renderedText ;
		
		
	// Uncomment to see client area and save back-buffer / front-buffer:
	//drawBox( _clientArea, Pixels::Yellow, /* filled */ false ) ;
	/*
	savePNG( "backbuffer.png" ) ;	
	_overallSurface->savePNG( "frontbuffer.png" ) ;	
	*/
	
	setBackBufferRedrawState( false ) ;	
				
}



const string TextWidget::toString( Ceylan::VerbosityLevels level ) const throw()
{

	std::list<string> textWidgetList ;
	
	
	string shapeText = "Widget shape is " ;
	
	
	switch( _shape )
	{
	
		case SquareCorners:
			shapeText += "a rectangle with square corners" ;
			break ;

		case RoundCorners:
			shapeText += "a rectangle with round corners" ;
			break ;
			
		default:	
			shapeText += "unknown (abnormal)" ;
			break ;
	
	}
	
	
	textWidgetList.push_back( shapeText ) ;
	
	if ( _minimumHeight )
	{
		textWidgetList.push_back( "Widget height (currently: " 
			+ Ceylan::toString( getHeight() )
			+ ") will shrink, if text is short enough, "
			"for recorded maximum height, "
			+ Ceylan::toString( _maximumHeight ) ) ; 
	}		
	else
	{
			 
		textWidgetList.push_back( "Widget height (" 
			+ Ceylan::toString( getHeight() ) 
			+ ") will not shrink even if text short enough." ) ;
	}
				
	textWidgetList.push_back( "Text color is " 
		+ Pixels::toString( _textColor )  ) ; 

	textWidgetList.push_back( "Edge color is " 
		+ Pixels::toString( _edgeColor )  ) ; 

	textWidgetList.push_back( "Text to display is '" + _text + "'." ) ;
	
	textWidgetList.push_back( "Font informations: " 
		+ _font->toString( level ) ) ;
	
	if ( _justified )	
		textWidgetList.push_back( "Text will be justified" ) ;
	else
		textWidgetList.push_back( "Text will not be justified" ) ;
		
	if ( _verticallyAligned )	
		textWidgetList.push_back( "Text will be vertically aligned" ) ;
	else
		textWidgetList.push_back( "Text will not be vertically aligned "
			"unless it takes the full height of client area" ) ;
		
	switch( _quality )
	{
	
		case Text::Font::Solid:
			textWidgetList.push_back( 
				"Text will be rendered in Solid quality" ) ;
			break ;
			
		case Text::Font::Shaded:
			textWidgetList.push_back( 
				"Text will be rendered in Shaded quality" ) ;
			break ;
			
		case Text::Font::Blended:
			textWidgetList.push_back( 
				"Text will be rendered in Blended quality" ) ;
			break ;
			
		default:	
			textWidgetList.push_back( 
				"Text will be rendered in unknown quality (abnormal)" ) ;
			break ;
			
	}
	
		
	string res = "Text widget: " + Ceylan::formatStringList( textWidgetList ) ;
	
	if ( level == Ceylan::low )		
		return res ;
		
	return res + BackBufferedWidget::toString( level ) ;
	
}



void TextWidget::updateClientArea() throw()
{

#if OSDL_DEBUG_WIDGET	
	LogPlug::trace( "TextWidget::updateClientArea" ) ;
#endif // OSDL_DEBUG_WIDGET
	
	// For square corners, client area is the basic one:
	if ( _shape == SquareCorners )
	{
		Widget::updateClientArea() ;
		return ;
	}	
		
	// Here we suppose _shape == RoundCorners:
	
	/*
	 * We have to compute what is the biggest upright rectangle that 
	 * fits into the one with round corners (the one that is closest 
	 * to the inner part of the widget, due to edge width).
	 *
	 * Each corner of the rectangular client area should be half-way 
	 * of its associated arc of circle: the corner is defined by 
	 * the radius  DefaultEdgeWidth.
	 *
	 * @see UprightRectangle::drawWithRoundedCorners
	 *
	 */
	Length radius = 0 ;
	
	try
	{ 
		radius = computeRadiusForRoundRectangle( DefaultEdgeWidth ) ;
	}
	catch( const VideoException & e )
	{
	
		LogPlug::error( "TextWidget::updateClientArea: no possible radius: " 
			+ e.toString() ) ;
		// Defaults to the whole rectangular shape (emergency measure):	
		Widget::updateClientArea() ; 
	
	}

	// cos( 45 degrees ) = sqrt(2)/2			
	Length projectedRadius = static_cast<Length>( 
		radius * Ceylan::Maths::Sqrt_2 / 2 ) ;
		
	_roundOffset = DefaultEdgeWidth + radius - projectedRadius ;
	
	_clientArea.setUpperLeftAbscissa( _roundOffset ) ;
	_clientArea.setWidth( getWidth() - 2 * _roundOffset ) ;
	
	_clientArea.setUpperLeftOrdinate( _roundOffset ) ;
	_clientArea.setHeight( getHeight() - 2 * _roundOffset ) ;
	
	//LogPlug::debug( "Client area:" + _clientArea.toString() ) ;
	
}

