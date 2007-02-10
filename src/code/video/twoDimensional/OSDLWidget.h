#ifndef OSDL_WIDGET_H_
#define OSDL_WIDGET_H_


#include "OSDLSurface.h"   // for Surface, SurfaceEvent
#include "OSDLPixel.h"     // for ColorDefinition
#include "OSDLPoint2D.h"   // for Point2D
#include "OSDLFont.h"      // for HorizontalAlignment, etc.


#include "Ceylan.h"        // for EventSource


#include <string>



namespace OSDL
{

	namespace Video
	{
		
		namespace TwoDimensional
		{

	

			/**
			 * Event sent by a Surface to a widget it contains so that 
			 * the widget redraws itself.
			 *
			 */
			class OSDL_DLL RedrawRequestEvent : public SurfaceEvent
			{
				public:
				
					RedrawRequestEvent( Ceylan::EventSource & source ) throw() ;
					virtual  ~RedrawRequestEvent() throw() ;
			} ;



			/* To be added later, maybe :
			class WidgetSettings : public Ceylan::TextDisplayable
			{
			
				public:
				
					WidgetSettings() throw() ;
					virtual ~WidgetSettings() throw() ;
				private:
				
			} ;

			*/


			/**
			 * Widgets are graphical components that are drawn in their 
			 * parent container, which is a surface. 
			 *
			 * Therefore widgets can have their own widgets too, insofar 
			 * they are surfaces as well.
			 *
			 * So, towards its parent surface, the widget is an event 
			 * listener. 
			 *
			 * Being also a Surface, it can have widgets itself, and 
			 * therefore would behave as an event source too.
			 *
			 * When a widget is to be drawn, it has to be managed by its 
			 * parent surface, which is the only object which is able 
			 * to know how to cope with stacking and overlapping elements.
			 *
			 * Therefore, when a widget changes, the information climbs 
			 * up the surface composition tree to its root, which will 
			 * result back, during the next redraw phase, to a selective 
			 * redraw.
			 *
			 * This drawing of the graphical tree is depth-first : child
			 * elements are sorted from bottom to top, they are completely 
			 * drawn in turn.
			 *
			 * Besides the inherited 'need for redraw' propagated state, 
			 * which determines whether the widget needs to copy back its
			 * internal video buffer to its parent surface, a widget will 
			 * have to be re-rendered on request, when its redrawInternal 
			 * method will be called. 
			 * Advanced widgets could rely on a backbuffer and a state 
			 * attribute telling whether the widget changed, so that the
			 * redrawInternal method triggers a new rendering of this widget
			 * only when appropriate. 
			 * The interest of this method depends on the balance to be found
			 * between the cost of memory resource (the back-buffer) and
			 * of CPU usage (the useless redrawings). 
			 * 
			 * @see the BackBufferedWidget class for this behaviour.
			 *
			 * Widgets can handle window decorations, notably for being
			 * minimized, maximized, dragged, wrapped or closed by the user. 
			 * The corresponding icons may be used.
			 * Default ones are provided as 18*18 PNG icons.
			 * If one of the previous features is activated for a widget, it 
			 * will trigger automatically the widget decoration.
			 *
			 * Each widget can have either a background color (which is 
			 * used to fill the widget dedicated area prior to any rendering),
			 * or a colorkey (which is used to fill as well its area and 
			 * set as its colorkey, hence making it fully transparent 
			 * as a whole before any special rendering which allows for
			 * non-rectangular shapes), or not color at all, so
			 * that the widget can exactly draw itself as wanted.
			 *
			 * The meaning of this base color is determined by the base 
			 * color mode. 
			 *
			 * When a colorkey is used, RLE encoding is activated to 
			 * gain speed.
			 *
			 */
			class OSDL_DLL Widget : public Surface, 
				public Ceylan::EventListener
			{
			
					
				public:
						
			
					/*
					 * Describes the available modes for handling the 
					 * base color of a widget.
					 *
					 * Hence the base color can be the background color 
					 * of a widget ('BackgroundColor'), or the color key 
					 * to be used if automatic color-keying is activated
					 * ('Colorkey'). 
					 * The base color can be ignored thanks to 'None'.
					 *
					 * The 'NotInitialized' value is only to be used in
					 * constructors.
					 *
					 */
					enum BaseColorMode { 
						BackgroundColor, 
						Colorkey, 
						None, 
						NotInitialized } ;
					
					
					
					/**
					 * Creates a new Widget, whose container surface is
					 * <b>container</b>. 
					 *
					 * Its container, which is always a surface (since Widgets
					 * are themselves specialized surfaces) will own this
					 * widget, therefore will deallocate it when appropriate.
					 *
					 * Most of the widget attributes, such as bits per pixel,
					 * will be deduced from the corresponding attributes 
					 * of the container surface.
					 * 
					 * A Widget is a (special) surface. 
					 * This surface stores the result of the rendering of 
					 * this widget and of all the renderings of subwidgets 
					 * this widget may have. 
					 *
					 * As a result, a basic widget has no surface of its 
					 * own, where it could store its own rendering for later
					 * use. 
					 * Therefore when at least one of its subwidgets changes, 
					 * it has to be veritably redrawn too, instead of which it
					 * could simply blit its own rendering, if it were stored.
					 *
					 * To rely on such behaviour, see the BackBufferedWidget
					 * class.
					 *
					 * Even if there is not reason for that widget to be
					 * decorated (no title, no special attributes such as
					 * maximizable activated, etc.), decoration rendering 
					 * can be forced after widget creation thanks to the
					 * setDecorationStatus method.
					 *
					 * Widgets are created by default with a pure black 
					 * colorkey (Pixels::Black) and with color-keying disabled.
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
					 * @param height this widget's height, in pixels.
					 *
					 * @param baseColorMode is the chosen mode for base color.
					 *
					 * @param baseColor is the base color of the widget.
					 *
					 * @param title the title which may be drawn, if non 
					 * empty, on the top of this widget.
					 *
					 * @param minMaximizable tells whether the widget can be
					 * minimized or maximized by the user.
					 *
					 * @param draggable tells whether the widget can be 
					 * dragged by the user.
					 *
					 * @param wrappable tells whether the widget can be 
					 * wrapped (only the widget decoration is displayed) 
					 * by the user.
					 *
					 * @param closable tells whether the widget can be closed 
					 * by the user.
					 *
					 * @throw VideoException if an error occured, for 
					 * example with colorkey activation.
					 *
					 * @note The stacking is set to the top position on
					 * creation. 
					 * The stack level used to be inaccurately modeled as a
					 * third coordinate (depth), instead of which it
					 * is managed thanks to an ordered list in the parent
					 * container.
					 *
					 * @note Parent surface cannot be 'const' since the 
					 * widget may issue redraw triggers for example.
					 * 
					 */
					 Widget( 
					 	Surface & container, 
						const Point2D & relativePosition,
					 	Length width, Length height,
						BaseColorMode baseColorMode, 
						Pixels::ColorDefinition baseColor,
						const std::string & title = "",
						bool minMaximizable = true, bool draggable = true, 
						bool wrappable = true, bool closable = true ) 
							throw( VideoException ) ;
								
							
					 /// Basic virtual destructor.	
					 virtual ~Widget() throw() ;
						
						
						
					 // Resize section.
					 
					 		
					 /**
					  * Sets the current width of this widget.
					  *
					  * If the new width is different from the current one,
					  * triggers a resize that will invalidate this widget
					  * rendering : 'needs redraw' will be true afterwards.
					  * 
					  * The client area will be automatically adjusted.
					  *
					  */
					 virtual void setWidth( Length newWidth ) throw() ;
				
				
					 /**
					  * Sets the current height of this widget.
					  *
					  * If the new height is different from the current one,
					  * triggers a resize that will invalidate this widget
					  * rendering : 'needs redraw' will be true afterwards. 
					  *
					  * The client area will be automatically adjusted.
					  *
					  */
					 virtual void setHeight( Length newHeight ) throw() ;
					 

					/**
					 * Resizes this widget so that its new dimensions are 
					 * the specified ones.
					 *
					 * If the new dimensions are the same as the current
					 * ones, then nothing special will be done, widget 
					 * stays as is. 
					 *
					 * Otherwise, it will be resized and will invalidate 
					 * this widget rendering : 'needs redraw' will be true 
					 * afterwards. 
					 *
					 * The client area will be automatically adjusted.
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
					  
					
					
					// Base color (color key and background color) section.
					
					  
					/**
					 * Returns the current base color mode.
					 *
					 */
					virtual BaseColorMode getBaseColorMode() const throw() ;   
					
					
					/**
					 * Sets the current base color mode and base color. 
					 *
					 * Based on previous values, only the necessary 
					 * operations will be performed.
					 *
					 * @exampl, setting the same colorkey twice will 
					 * activate the key only once.
					 *
					 * @note Sets the 'needsRedraw' flag to true if a 
					 * change occured.
					 *
					 * @param newBaseColorMode the new base color mode.
					 *
					 * @param newBaseColor the new base color.
					 *
					 * @throw VideoException if a change in color-keying 
					 * failed.
					 *
					 */
					virtual void setBaseColorMode( 
							BaseColorMode newBaseColorMode,
							Pixels::ColorDefinition newBaseColor ) 
						throw( VideoException ) ;
					
					 
					 
					/**
					 * Returns the current base color.
					 *
					 */ 
					virtual Pixels::ColorDefinition getBaseColor() 
						const throw() ;
											
					
					/*
					 * No setBaseColor since it has to be modified thanks to
					 * 'setBaseColorMode'.
					 *
					 */
								
								
									 
					/**
					 * Sets the current decoration status, so that this 
					 * widget will have decorations or not after this 
					 * method is called.
					 *
					 * @param newDecorationStatus the new decoration status.
					 *
					 */
					virtual void setDecorationStatus( 
						bool newDecorationStatus ) throw() ; 
					 
					 
					 
					/**
					 * Returns an upright rectangle which describes the 
					 * client area, in widget referential.
					 *
					 * Client area is the total usable area that is left 
					 * for this widget specific renderings.
					 *
					 * @note The client area might not be the whole widget 
					 * area, for example if the widget is decorated : at 
					 * least the title bar will not be taken in client area.
					 *
					 */ 
					virtual const UprightRectangle & getClientArea() 
						const throw() ; 
					
										
					/**
					 * Cleans the widget by blanking it with a background 
					 * color, or a colorkey, or does nothing, depending 
					 * on base color mode being respectively
					 * BackgroundColor, or Colorkey, or None. 
					 *
					 * @note The '_needsRedraw' flag is not changed.
					 *
					 */
					virtual bool clean() throw() ; 
					       
					       
					/**
					 * Notifies this widget of a new event whose source 
					 * is its container.
					 *
					 * @note This event remains property of the container, 
					 * which will take care of its life cycle.
					 *
					 */
					virtual void beNotifiedOf( const Ceylan::Event & newEvent )
						throw() ;
						

					/**
					 * Sets redraw state.
					 *
					 * @note On transition from the 'needs redraw' flag 
					 * from false to true, parent container is notified.
					 *
					 */
					virtual void setRedrawState( bool needsToBeRedrawn ) 
						throw() ;
									 
					 					 
					/**
					 * Redraws this widget, first thanks to a blit of its
					 * surface to its parent, then for all its sub-widgets, 
					 * if any, from bottom to top.
					 *
					 * @note If necessary (if the widget changed), then 
					 * prior to any action, the widget will update its 
					 * surface, thanks to redrawInternal.
					 *
					 */
					virtual void redraw() throw() ;
				
										 	
					/**
					 * Basic redraw method for internal rendering.
					 *
					 * This basic implementation only fills the overall 
					 * widget surface with the background color, draws the 
					 * edges and renders the caption (a text below an
					 * horizontal line). 
					 *
					 * @note This method is meant to be overridden, so 
					 * that this widget is able to paint itself, i.e. only 
					 * its background, its widgets excluded.
					 *
					 * @note Such overriding method must, once done, 
					 * set the '_needsRedraw' flag to false, otherwise 
					 * its container(s) will be drawn again and again,
					 * uselessly.
					 *
					 */
					virtual void redrawInternal() throw() ;
			
					
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
				
				
				
					// Static section.
				
				
					/**
					 * Returns the current default edge color for all 
					 * new widgets.
					 *
					 */
					static Pixels::ColorDefinition GetEdgeColor() throw() ;
					
					
					/**
					 * Sets the edge color for all new widgets.
					 *
					 * @param edgeColorDef the color definition of the 
					 * edges.
					 *
					 */
					static void SetEdgeColor( 
						Pixels::ColorDefinition edgeColorDef ) throw() ;
					
						
							
											
				protected:
						
	
	
					/**
					 * Updates the decoration flag, depending on the 
					 * various services requested.
					 *
					 */
					virtual void updateDecorationFlag() throw() ;
												
					
					/**
					 * Updates client area with regard to current 
					 * decoration status.
					 *
					 */
					virtual void updateClientArea() throw() ;
					 	
						
					/**
					 * Returns this widget's container surface.
					 *
					 * @note The container of this widget is stored as 
					 * its only event source.
					 *
					 */
					virtual Surface & getContainer() throw() ;
								
																
					/**					
					 * Returns this widget's container surface as a 
					 * 'const' object.
					 *
					 * @note The container of this widget is stored as 
					 * its only event source.
					 *
					 * @note This 'const' method is useful so that 
					 * calling method (ex : toString) can remain 'const' 
					 * as well.
					 *
					 */
					virtual const Surface & getConstContainer() const throw() ;
																
					
					/**
					 * Draws the fundamental parts of a widget : widget 
					 * area cleaned accordingly to the current base color 
					 * mode, and edges are drawn.
					 *
					 * @note The 'needs redraw' flag is not changed.
					 *
					 * Remains not static to allow easy override, 
					 * and needs widget attributes.
					 *
					 */
					virtual void drawFundamentals( Surface & targetSurface )
						throw() ;
					
					
					/**
					 * Tells whether this widget is decorated. 
					 *
					 * @note A widget is decorated if and only if at least 
					 * one of its possible features (ex : draggable, movable,
					 * etc.) is activated.
					 *
					 */
					virtual bool isDecorated() const throw() ;		
					
					
					/**
					 * Draws the decorations for this widget to specified
					 * surface.
					 *
					 * @param targetSurface must be specified since 
					 * decorations are not always to be rendered directly 
					 * on widget surface : for instance for back-buffered
					 * widgets, another surface is targeted.
					 *
					 * @note Remains not static to allow easy override, 
					 * and needs widget attributes.
					 *
					 */
					virtual void drawDecorations( Surface & targetSurface )
						throw() ;
							
							
							
							
					// Attributes section.		
					
					
					/**
					 * This widget's upper left corner, relatively to its
					 * container.
					 *
					 */
					Point2D _upperLeftCorner ;		


					/**
					 * The rectangular client area, defined in the 
					 * referential of this widget.
					 *
					 * This area is the one left for this widget specific
					 *  renderings (ex : any decorations excluded).
					 *
					 */
					UprightRectangle _clientArea ;
					
										

					/**
					 * Tells whether this widget rendering should include
					 * decorations.
					 *
					 */
					bool _decorated ;
					
					
					/// This Widget's title.
					std::string _title ;


					/**
					 * Tells whether this widget can be minimized and 
					 * maximized by the user.
					 *
					 * @note If set, this triggers the display of the
					 * corresponding minimize/maximize icon : it implies 
					 * that the widget will be decorated.
					 *
					 */
					bool _minMaximizable ;		

							
					/**
					 * Tells whether this widget can be dragged by the user.
					 *
					 * @note If set, this triggers the display of the
					 * corresponding draggable icon : it implies that 
					 * the widget will be decorated.
					 *
					 */
					bool _draggable ;		
						
																	
					/**
					 * Tells whether this widget can be wrapped by the user,
					 * hiding the whole widget but its decoration.
					 *
					 * @note If set, this triggers the display of the
					 * corresponding wrappable icon : it implies that the 
					 * widget will be decorated.
					 *
					 */
					bool _wrappable ;		
																	
							
					/**
					 * Tells whether this widget can be closed by the user.
					 *
					 * @note If set, this triggers the display of the
					 * corresponding closable icon : it implies that 
					 * the widget will be decorated.
					 *
					 */
					bool _closable ;		
	
	
					/**
					 * Tells whether this widget has currently the 
					 * focus (i.e. is selected by the user).
					 *
					 * @note Multiple widgets may have the focus simultaneously.
					 *
					 */
					bool _hasFocus ;							
					
							
					
					/**
					 * Tells whether a background color, color-keying or 
					 * nothing special shall be used for this widget.
					 *
					 * @see BaseColorMode, _baseColor
					 *
					 */
					BaseColorMode _baseColorMode ;
					
					
					/**
					 * Stores the base color of this widget, which is
					 *  either used as a background color or a colorkey,
					 * depending on '_baseColorMode'.
					 *
					 * @see _baseColorMode
					 *
					 */
					Pixels::ColorDefinition _baseColor ;
					
					
					/**
					 * The base color, converted to this widget current 
					 * pixel format, inherited from container.
					 *
					 */
					Pixels::PixelColor _actualBaseColor ;
					
					
					/*
					 * Static attributes section.
					 *
					 * Most are non-const since may be modified at runtime.
					 *
					 */
					
					
					/// Color of widget edges.
					static Pixels::ColorDefinition _EdgeColor ;

					/// Color of widget title.
					static Pixels::ColorDefinition _TitleColor ;


					/**
					 * Defines how a widget title should be horizontally 
					 * aligned (default : Center).
					 *
					 */
					static Text::HorizontalAlignment _TitleHorizontalAlignment ;


					/**
					 * Defines how a widget title should be vertically aligned.
					 * (default : Center).
					 *
					 */
					static Text::VerticalAlignment _TitleVerticalAlignment ;



					/**
					 * Default width offset for client area relative 
					 * to widget vertical edges.
					 * 
					 */
					static Length _ClientOffsetWidth ;
					

					/**
					 * Default height offset for client area relative to 
					 * widget horizontal edges.
					 * 
					 * @note For decorated widgets, the top widget edge 
					 * taken into account is the title bar.
					 *
					 */
					static Length _ClientOffsetHeight ;
					
				

					/**
					 * Default abscissa offset relative to widget upper 
					 * left corner used to print widget title with left
					 * alignment.
					 *
					 */
					static Coordinate _TitleOffsetAbscissa ;
					
					
					/**
					 * Default ordinate offset relative to widget upper 
					 * left corner used to print widget title.
					 *
					 */
					static Coordinate _TitleOffsetOrdinate ;
					
					
					/**
					 * Default ordinate offset relative to widget upper 
					 * left corner used to draw widget title bar.
					 *
					 */
					static Coordinate _TitleBarOffsetOrdinate ;
					
					
					/**
					 * Default title to be used when a titleless widget 
					 * has however to be rendered decorated.
					 *
					 */
					static std::string _DefaultTitle ;
					
					
					
				private:	


					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit Widget( const Widget & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					Widget & operator = ( const Widget & source ) throw() ;
					
					
			} ;			
											
		}
	
	}

}				


#endif // OSDL_WIDGET_H_

