/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#ifndef OSDL_BACK_BUFFERED_WIDGET_H_
#define OSDL_BACK_BUFFERED_WIDGET_H_


#include "OSDLWidget.h"          // for inheritance
#include "OSDLVideoTypes.h"      // for Length, etc.


#include <string>



namespace OSDL
{

	namespace Video
	{


		// A BackBufferedWidget is and owns a Surface :
		class Surface ;
		
		
		namespace TwoDimensional
		{

	
			// To locate a widget in its container :
			class Point2D ;
			
			
			

			/**
			 * A BackBufferedWidget is a Widget, hence a Surface, which 
			 * contains only its own rendering (the one of its subwidgets 
			 * is not on it) and which owns another Surface, which 
			 * contains this widget rendered alongside the full tree of 
			 * its subwidgets, which acts as a cache.
			 *
			 * Hence when a BackBufferedWidget has to be redrawn whereas 
			 * this widget itself did not change (this happen for example
			 * whenever one of its subwidgets changes), instead of 
			 * triggering a new rendering from scratch of this widget, 
			 * its own cached rendering (which is cached in the inherited
			 * surface, called here the back-buffer) is blitted directly 
			 * to the overall surface (the surface this back-buffered widget
			 * owns) and the usual mechanism for subwidgets apply, so 
			 * that they are rendered on this latter surface.
			 *
			 * The role of the inherited and the owned surfaces should not 
			 * be reversed, since the current organization allows to use 
			 * all the inherited drawing primitives without any
			 * modification : they apply directly to the inherited 
			 * back-buffer surface, which is the source surface from which 
			 * the overall one is created.
			 *
			 * Caching the own rendering of a widget in a back-buffer is
			 * especially useful whenever this rendering demands resources,
			 * for example when complex text is to be rendered.
			 *
			 * To repeat for the sake of clarity, rendering specific to this
			 * widget is done in the so-called 'back-buffer' (which is 
			 * stored in the inherited surface), as opposed to the 'overall'
			 * owned surface which refers to this widget <b>and</b> all its
			 * subwidgets.
			 *
			 */
			class OSDL_DLL BackBufferedWidget : public Widget
			{
			
					
				public:
						
			
					/**
					 * Creates a new back-buffered Widget.
					 *
					 * Its container, which is always a surface (since 
					 * Widgets are themselves specialized surfaces) will own
					 * this widget, therefore will deallocate it when
					 * appropriate.
					 *
					 * Most of the widget attributes, such as bits per pixel,
					 * will be deduced from the corresponding attributes 
					 * of the container surface.
					 * 
					 * @param container this Widget's container surface, 
					 * to which it will automatically be subscribed to, as 
					 * an EventListener for redraw operations.
					 *
					 * @param relativePosition tells where the upper-left 
					 * corner of the widget is located inside its container.
					 *
					 * @param width this widget's width, in pixels.
					 *
					 * @param height this widget's height, in pixels.
					 *
					 * @param baseColorMode is the chosen mode for base color.
					 *
					 * @param baseColor is the base color of the widget.
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
					 * wrapped (only the widget decoration is displayed) by 
					 * the user.
					 *
					 * @param closable tells whether the widget can be closed 
					 * by the user.
					 *
					 * @throw VideoException if an error occured.
					 *
					 * @note The stacking is set to the top position on
					 * creation. 
					 * The stack level used to be inaccurately modeled as 
					 * a third coordinate (depth), instead of which it
					 * is now managed thanks to an ordered list in the 
					 * parent container.
					 *
					 * @note Parent surface cannot be 'const' since the 
					 * widget may issue redraw triggers for example.
					 * 
					 */
					 BackBufferedWidget( 
					 	Surface & container, 
						const Point2D & relativePosition,
					 	Length width, 
						Length height, 
						BaseColorMode baseColorMode, 
						Pixels::ColorDefinition baseColor, 
						const std::string & title = "",
						bool minMaximizable = true, 
						bool draggable = true, 
						bool wrappable = true, 
						bool closable = true ) 
							throw( VideoException ) ;
								
							
					 /// Basic virtual destructor.	
					 virtual ~BackBufferedWidget() throw() ;
						
							
					/**
					 * Resizes both this widget and its back-buffer so that 
					 * their new dimensions are the specified ones.
					 *
					 * If the new dimensions are the same as the current 
					 * ones, then nothing special will be done, surfaces 
					 * stay as they are. 
					 * Otherwise, they will be resized and their content 
					 * will be lost, replaced by a pure black surface. 
					 * The 'needs redraw' attribute will be set to true,
					 * both for the widget itself and for its back-buffer, 
					 * and the client area will be updated.
					 *
					 * @param newWidth the new width.
					 *
					 * @param newHeight the new height.
					 *
					 * @param ignored is ignored.
					 *
					 */
					virtual void resize( Length newWidth, Length newHeight, 
						bool ignored = false ) throw() ;
										 

					/**
					 * Sets the current base color mode and base color, 
					 * for both surfaces (back-buffer and overall).
					 *
					 * Based on previous values, only the necessary 
					 * operations will be performed.
					 * For example, setting the same colorkey twice will
					 * activate the key only once.
					 *
					 * @note Sets the 'needsBackBufferRedraw' flag to true 
					 * if a change occured.
					 *
					 * @param newBaseColorMode the new base color mode.
					 *
					 * @param newBaseColor the new base color.
					 *
					 * @throw VideoException if a change in color-keing failed.
					 *
					 */
					virtual void setBaseColorMode( 
							BaseColorMode newBaseColorMode,
							Pixels::ColorDefinition newBaseColor ) 
						throw( VideoException ) ;
												


					/**
					 * Redraws this widget, by updating itself (if necessary)
					 * and then updating its sub-widgets, by blitting all 
					 * of them (this widget, then all sub-widgets from 
					 * bottom to top) to the back-buffer and then blitting 
					 * the resulting back-buffer in the parent container 
					 * of this widget.
					 *
					 */
					virtual void redraw() throw() ;

										 	
					/**
					 * This method manages this widget so that no useless
					 * internal rendering is performed. 
					 *
					 * If however the main surface has to be updated, then 
					 * the overriden 'redrawBackBuffer' will be called.
					 *
					 */
					virtual void redrawInternal() throw() ;
			
			
					/**
					 * Basic do-nothing redraw method for the actual 
					 * internal rendering into the back-buffer (the inherited
					 * surface).
					 *
					 * @note This method is meant to be overridden, so that 
					 * this widget is able to paint itself, its widgets
					 * excluded.
					 *
					 * @note All overriding implementations <b>must</b> set the
					 * '_needsBackbufferRedraw' attribute to false when the
					 * redraw succeeded (otherwise numerous useless redraws
					 * would be performed), and <b>should</b> convert the
					 * resulting cached surface to display pixel format to 
					 * avoid multiple useless conversions to this display
					 * format.
					 *
					 */
					virtual void redrawBackBuffer() throw() ;
					

					/**
					 * Returns the surface that should be targeted by 
					 * widgets having to blit their renderings to their
					 * container. 
					 *
					 * For back-buffered widgets, the render target is not 
					 * the instance itself (it would modify the 
					 * private back-buffer of this widget) but the overall 
					 * owned surface : subwidgets should only be blitted 
					 * into this overall surface, and should not affect 
					 * the back-buffer.
					 *
				 	 */
					Surface & getWidgetRenderTarget() throw() ; 

					
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
							Ceylan::VerbosityLevels level = Ceylan::high )
						const throw() ;
				
					
						
											
				protected:
											
					
					/**
					 * Sets the need for redraw state for the back-buffer.
					 *
					 */
					virtual void setBackBufferRedrawState( 
						bool needsToBeRedrawn ) throw() ;

				
					/**
					 * Returns the current need for redraw state for the
					 * back-buffer.
					 *
					 */
					virtual bool getBackBufferRedrawState() const throw() ;


					/**
					 * The overall surface where this widget (thanks to 
					 * the back-buffer) and all its sub-widget tree will 
					 * be rendered.
					 *
					 */
					Surface * _overallSurface ;
					
					
					
					
				private:	



					/**
					 * Tells whether the internal rendering of this widget
					 * should be updated.
					 *
					 * Made private to force the use of get/set methods 
					 * for child classes, otherwise callback notifications
					 * would not be triggered.
					 *
					 */
					bool _needsBackBufferRedraw ;



					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit BackBufferedWidget( 
						const BackBufferedWidget & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					BackBufferedWidget & operator = ( 
						const BackBufferedWidget & source )	throw() ;
					
					
			} ;			
											
		}
	
	}

}				


#endif // OSDL_BACK_BUFFERED_WIDGET_H_
