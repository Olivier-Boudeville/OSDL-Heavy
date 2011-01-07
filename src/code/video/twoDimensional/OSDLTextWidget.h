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


#ifndef OSDL_TEXT_WIDGET_H_
#define OSDL_TEXT_WIDGET_H_


#include "OSDLBackBufferedWidget.h"   // for inheritance
#include "OSDLFont.h"                 // for RenderingQuality, etc.

#include <string>




namespace OSDL
{



	namespace Video
	{
		
		
		
		namespace TwoDimensional
		{

	
	
			namespace Text
			{
		
				// TextWidget instances use fonts to render text.
				class Font ;
				
			}
			
			
			

			/**
			 * Text widgets are back-buffered widget which display full 
			 * texts, i.e. sets of paragraphs.
			 *
			 * Many attributes of the text (ex: text color) are 
			 * determined from the provided font.
			 *
			 */
			class OSDL_DLL TextWidget: public BackBufferedWidget
			{
			
					
				public:
					
					
					
					/**
					 * Describes the shape of a text container:
					 *	- SquareCorners: the text is in a rectangular box
					 *  - RoundCorners: the text is in a box with round 
					 * corners
					 *
					 */
					enum Shape { SquareCorners, RoundCorners } ;	
			
			
			
					/// The default edge width for the 'RoundCorners' shape:
					static const Length DefaultEdgeWidth ;
			
					
					
					
					/**
					 * Creates a back-buffered widget whose role is to 
					 * display text.
					 *
					 * Its container, which is always a surface (since 
					 * Widgets are themselves specialized surfaces) will own
					 * this widget, therefore will deallocate it when
					 * appropriate.
					 *
					 * Most of the widget attributes, such as bits per pixel,
					 * will be deduced from
					 * the corresponding attributes of its container.
					 * 
					 * @param container this Widget's container surface, to
					 * which it will automatically be subscribed to, as an
					 * EventListener for redraw operations.
					 *
					 * @param relativePosition tells where the upper-left 
					 * corner of the widget is located inside its container.
					 *
					 * @param width this widget's width, in pixels.
					 *
					 * @param maximumHeight this widget's height, in pixels. 
					 * The widget can never have a bigger height, but if
					 * 'minimumHeight' is true, it may be smaller.
					 *
					 * @param shape the desired shape of the text container. 
					 * If implies the choice of base color mode.
					 *
					 * @param textColor is the color of the text to be 
					 * rendered in this widget.
					 *
					 * @param edgeColor is the color of the container edges. 
					 * It is the only edge color used, and Widget::EdgeColor 
					 * is not impacted for other types of widgets.
					 *
					 * @param backgroundColor is the color of the background
					 * behind the text.
					 *
					 * @param title the title which may be drawn, if non 
					 * empty, on the top of the widget.
					 *
					 * @param minMaximizable tells whether the widget can 
					 * be minimized or maximized by the user.
					 *
					 * @param text the text which is to be rendered in this
					 * widget.
					 *
					 * @param font the font that should be used. 
					 * It will be used by this widget but not owned, hence 
					 * the font will not be deleted when this widget will be
					 * deleted.
					 *
					 * @param minimumHeight tells whether, if the rendered 
					 * text takes less than than the full provided height, 
					 * this widget should have an height just big enough to
					 * contain the text (if true), or if the user-provided
					 * height should be kept, hence creating a widget whose 
					 * last pixel rows are empty (if false).
					 * In all cases the actual height will be no bigger than 
					 * the provided 'maximumHeight'.
					 *
					 * @param verticallyAligned tells whether the text 
					 * should be vertically aligned. 
					 * Even if set to false, if the text uses all the 
					 * container final height it could use, then the text 
					 * will be nonetheless vertically aligned to avoid the 
					 * false feeling of having enough room for one more line.
					 *
					 * @param justified tells whether text should be 
					 * justified in the client area of this widget.
					 *
					 * @param quality the chosen quality for text rendering.
					 *
					 * @param title the title which may be drawn, if non 
					 * empty, on the top of the widget.
					 *
					 * @param minMaximizable tells whether the widget can 
					 * be minimized or maximized by the user.
					 *
					 * @param draggable tells whether the widget can be 
					 * dragged by the user.
					 *
					 * @param wrappable tells whether the widget can be 
					 * wrapped (only the widget decoration is displayed) 
					 * by the user.
					 *
					 * @param closable tells whether the widget can be 
					 * closed by the user.
					 *
					 * @throw VideoException if an error occured.
					 *
					 * @note The stacking is set to the top position on
					 * creation. 
					 *
					 * The stack level used to be inaccurately modeled as 
					 * a third coordinate (depth), instead of which it
					 * is managed thanks to an ordered list in the parent
					 * container.
					 *
					 * @note Parent surface cannot be 'const' since the 
					 * widget may issue redraw triggers for example.
					 * 
					 */
					TextWidget( Surface & container, 
					 	const Point2D & relativePosition,
					 	Length width, 
						Length maximumHeight, 
						Shape shape,
						Pixels::ColorDefinition textColor,
						Pixels::ColorDefinition edgeColor,
						Pixels::ColorDefinition backgroundColor, 
						const std::string & text, 
						Text::Font & font, 
						bool minimumHeight = true,
						bool verticallyAligned = true,
						bool justified = true, 
						Text::Font::RenderQuality quality = Text::Font::Blended,
						const std::string & title = "",
						bool minMaximizable = false, 
						bool draggable = false, 
						bool wrappable = false, 
						bool closable = false ) ;
								
							
							
					/// Basic virtual destructor.	
					virtual ~TextWidget() throw() ;
					
							
							
					/**
					 * Sets the new text this widget should render.
					 *
					 * @param newText the text that will be rendered, 
					 * at least partially.
					 *
					 */
					virtual void setText( const std::string & newText ) ;
					
					
					
					/**
					 * Returns the text which is currently being displayed 
					 * by this widget.
					 *
					 */
					virtual const std::string & getText() const ;
					
					
					
					/**
					 * Returns the current index of the first character 
					 * of current text that could not be rendered in this
					 * widget, if a rendering has already been done.
					 * Otherwise returns zero, as long as no text had to be
					 * rendered.
					 * 
					 * Therefore if the full text could be displayed, returns
					 * the size of the string returned by the getText method.
					 * Less than that means the text is too long for the
					 * display.
					 *
					 */
					virtual Text::TextIndex getRenderIndex() const ;
					
					
					
					/**
					 * Triggers the actual internal rendering.
					 *
					 * @note This method is meant to be overridden, so 
					 * that this widget is able to paint itself, i.e. only 
					 * its background, its widgets excluded.
					 *
					 */
					virtual void redrawBackBuffer() ;
			
					
					
	 	            /**
		             * Returns an user-friendly description of the state 
					 * of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
				
					
					
											
				protected:
						


					/**
					 * Updates client area with regard to current shape 
					 * and decoration status.
					 *
					 */
					virtual void updateClientArea() ;



					/// Horizontal offset on both sides.
					static const Length _TextWidthOffset ;
					
					
					/// Vertical offset on both sides.
					static const Length _TextHeightOffset ;
					
					
					
					/**
					 * Tells whether the height of this widget should be
					 * smallest possible one.
					 *
					 */
					Length _minimumHeight ;
					
					
					
					/// The upper bound of the height of this widget.
					Length _maximumHeight ;
					
					
					
					/// Records the shape of this text widget.
					Shape _shape ;
							
																				
					/// The color of rendered text in this widget.
					Pixels::ColorDefinition _textColor ;
					
					
					/// The color of the edges of this text widget.
					Pixels::ColorDefinition _edgeColor ;
					
					
					
					/**
					 * The background color, only for RoundCorners shape, 
					 * since the basecolor is already used for the colorkey.
					 *
					 */
					Pixels::ColorDefinition _backColorForRoundCorners ;
					
					
					
					/**
					 * Precomputation of the round offset associated with 
					 * round corners.
					 *
					 * Updated when client area itself updated, and 
					 * reused afterwards.
					 *
					 */
					Length _roundOffset ;
					
					 
					 
					/// The text which is to be displayed by this widget.
					std::string _text ;
					
					
					/// The font which is to be used for text rendering.
					Text::Font * _font ;


					
					/// Tells whether the text should be vertically aligned.
					bool _verticallyAligned ;
					
					
					/// Tells whether the text should be justified.
					bool _justified ;
					
					
					
					
					/// Quality of text rendering.
					Text::Font::RenderQuality _quality ;
					
					
					
					/**
					 * The current index of text that could not fit in 
					 * the last display, or zero if no text has been 
					 * rendered yet.
					 *
					 */
					Text::TextIndex _currentIndex ;
					



				private:	



					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit TextWidget( const TextWidget & source ) ;
			
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					TextWidget & operator = ( const TextWidget & source ) ;
					
					
			} ;		
				
											
		}
	
	}

}				



#endif // OSDL_TEXT_WIDGET_H_

