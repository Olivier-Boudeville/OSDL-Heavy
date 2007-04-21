#ifndef OSDL_SURFACE_H_
#define OSDL_SURFACE_H_


#include "OSDLImage.h"              // for ImageException
#include "OSDLUprightRectangle.h"   // for UprightRectangle
#include "OSDLPalette.h"            // for ColorCount


#include "Ceylan.h"                 // for EventSource, Lockable, SmartResource

#include <string>
#include <list>

// To protect LoadImage :
#include "OSDLIncludeCorrecter.h"


// The internal actual surface.
struct SDL_Surface ;



namespace OSDL
{

	namespace Video 
	{
	

		namespace TwoDimensional
		{
		
			// Necessary for many drawing operations on surfaces.
			class Point2D ;
			
			// A surface can have widgets.
			class Widget ;
		
		}
		
		
		// Surfaces can be palettized.
		class Palette ;
		
		using namespace Pixels ;
		
		
		/**
		 * Mother class of all events sent by a Surface (event source) 
		 * to its widgets (listeners).
		 *
		 */
		class OSDL_DLL SurfaceEvent : public Ceylan::Event
		{
		
			public:
		
				explicit SurfaceEvent( Ceylan::EventSource & source ) throw() ;
				SurfaceEvent() throw() ;
			
				virtual ~SurfaceEvent() throw() ;
				
		} ;
		
	
		
		/// Thrown when video memory has been lost.
		class OSDL_DLL VideoMemoryLostException : public VideoException
		{
		
			public:
			
				VideoMemoryLostException( const std::string & message ) 
					throw() ;
				
				/**
				 * Constructor with no message, used when no particular 
				 * reason is known.
				 *
				 */
				VideoMemoryLostException() throw() ;
				
				virtual ~VideoMemoryLostException() throw() ;
			
		} ;
		
		
				
		/**
		 * Represents a video buffer. 
		 *
		 * Its display type depends on whether this buffer corresponds to 
		 * a back-buffer or to the screen (display buffer) and, in this case, 
		 * on whether it is OpenGL-based.
		 * 
		 * A Surface is an UprightRectangle for its shape.
		 * Note that it therefore inherits _width and _height attributes 
		 * from UprightRectangle, but they are not of use for Surfaces 
		 * since the only values that matter for them are to be read directly
		 * from the internal back-end surface. 
		 * Hence getWidth() and getHeight() should <b>always</b> be used, even
		 * for the implementation of Surface's own methods, instead of 
		 * _width and _height.
		 *
		 * The clipping area of a Surface is another UprightRectangle, 
		 * which must be entirely contained in its Surface.
		 *
		 * Surfaces are Lockable UpRightRectangle instances. 
		 * They are also Smart resources, which implies they are Mesurable 
		 * and Clonable.
		 *
		 * In the future, Surface instances may use smart pointers or 
		 * reference counting (AddRef/Release) from Ceylan.
		 *
		 * Surfaces can contain widgets, which they own.
		 *
		 * Surfaces maintain a 'need for redraw' state, which determines 
		 * whether a given surface has to be redrawn. 
		 * As soon as a graphical element (be it Surface, Widget or other) 
		 * needs to be redrawn, the whole container tree has to be itself
		 * rescanned, because of possibly overlapping elements. 
		 *
		 * @see OSDL-usersguide.html for more detailed informations on 
		 * widget management.
		 *
		 * Surface will automatically disable locking/unlocking if possible, 
		 * one does not therefore need to test it with Surface::mustBeLocked
		 * before locking or unlocking it.
		 *
		 * @note In case the surface is double-buffered, and if everything 
		 * is not to be redrawn at each frame, the surface should be drawn
		 * twice, so that both buffers are filled.
		 *
		 * Surface is firstly a mere proxy-object of its backend counterpart,
		 * SDL_Surface. 
		 * However advanced services have been added, to provide higher level
		 * abstractions of video buffers.
		 *
		 * Surface class is designed to always have a valid internal SDL
		 * surface.
		 *
		 * A surface being a widget container, it is an event source, since 
		 * it must register its widgets so that they perform pick-correlation 
		 * or get, for example, demand for redraw events. 
		 *
		 * Stacking is handled thanks to a list of widgets, which is this 
		 * event source's listeners list, listeners being sorted from 
		 * bottom to top. 
		 * Therefore, to redraw the whole surface once it has been primarily
		 * rendered, its widgets are in turn redraw, thanks to a simple
		 * iteration on listener ordered list : they should be then drawn 
		 * from bottom to top, so that widget overlapping is properly managed.
		 * 
		 * Obviously no widget should be registered more than once.
		 *
		 * Images can be loaded in Surfaces, supported formats are :
		 *		- JPG (recommended for snapshots)
		 *		- PNG (recommended for everything else) [Portable Network
		 * Graphics, lossless compression]
		 * 		- BMP
		 *		- GIF
		 *		- LBM
		 *		- PCX
		 *		- PNM
		 *		- TGA
		 *		- XPM
		 * 
		 * Surfaces can be stretched, zoomed, rotated, etc.
		 *
		 * @see UprightRectangle, TwoDimensional::Image, Ceylan::Lockable
		 *
		 * @see testOSDLVideo.cc
		 *
		 * @see http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fDisplayFormat
		 *
		 * Screen surfaces have also methods to control the mouse position.
		 *
		 */
		class OSDL_DLL Surface : 
			public TwoDimensional::UprightRectangle, 
			public Ceylan::EventSource,
			public Ceylan::Lockable, 
			public Ceylan::SmartResource
		{
	
	
			public:
			
			
				/**
				 * Describes the display type of a surface :
				 *
				 *   - BackBuffer for non display surfaces
				 *   - ClassicalScreenSurface for usual screen surfaces
				 *   - OpenGLScreenSurface for OpenGL-based screen surfaces 
				 *
				 */
				enum DisplayType { 
					BackBuffer, 
					ClassicalScreenSurface, 
					OpenGLScreenSurface 
				} ;
			
			
			
				/*
				 * These flags can apply to all Surfaces.
				 *
				 */


				/// Indicates that a surface is to be stored in system memory.
				static const Ceylan::Flags Software ;			   


				/// Indicates that a surface is to be stored in video memory.
				static const Ceylan::Flags Hardware ;			   
				
				
				/**
				 * Indicates that a surface should use asynchronous blits 
				 * if possible.
				 *
				 */
				static const Ceylan::Flags AsynchronousBlit ;
					
									
				/// Indicates that a surface should have an exclusive palette.
				static const Ceylan::Flags ExclusivePalette ;	
				
				
				/**
				 * Indicates that a surface is to use hardware-accelerated
				 * blits.				 
				 *
				 */  
				static const Ceylan::Flags HardwareAcceleratedBlit ;
				
				
				/// Indicates that a surface is to use colorkey blits.
				static const Ceylan::Flags ColorkeyBlit ;		   
				
				
				/**
				 * Indicates that a surface is to use RLE-accelerated 
				 * colorkey blits.
				 *
				 */
				static const Ceylan::Flags RLEColorkeyBlit  ;	   
				
				
				/// Indicates that a surface is to use alpha blending blits.	
				static const Ceylan::Flags AlphaBlendingBlit ;	   

				/// Indicates that a surface is to use preallocated memory.
				static const Ceylan::Flags Preallocated ;	   


				/**
				 * Indicates that RLE colorkey blit is actually accelerated
				 * (read-only).
				 *
				 * @note The meaning of this private internal flag is an
				 * hypothesis, this flag should not be used externally anyway.
				 *
				 */
				static const Ceylan::Flags RLEColorkeyBlitAvailable ;	   



				/*
	 			 * These flags can only apply to <b>display</b> Surfaces.
				 *
				 */


				/**
				 * Indicates that a <b>display</b> surface may use any pixel
				 * format.
				 *
				 */
				static const Ceylan::Flags AnyPixelFormat ;
				
				
				/**
				 * Indicates that a <b>display</b> surface is to be double
				 * buffered.
				 *
				 */
				static const Ceylan::Flags DoubleBuffered ;
				
				
				/**
				 * Indicates that a <b>display</b> surface is to be full 
				 * screen, not windowed.
				 *
				 */
				static const Ceylan::Flags FullScreen ;
				
				
				/**
				 * Indicates that a <b>display</b> surface should have 
				 * an OpenGL context.
				 *
				 */
				static const Ceylan::Flags OpenGL ;
				
				/**
				 * Indicates that a <b>display</b> surface is to be 
				 * resizable.
				 *
				 */
				static const Ceylan::Flags Resizable ;
	

				/**
				 * Constructs an OSDL Surface from a SDL surface.
				 *
				 * @param surface the SDL surface to manage, whose 
				 * ownership is taken by this OSDL Surface.
				 *
				 * @param displayType tells what is the display type of 
				 * the provided surface, i.e. whether it is a backbuffer 
				 * or a screen surface, usual or OpenGL-based.
				 *
				 */
				explicit Surface( SDL_Surface & surface, 
					DisplayType displayType = BackBuffer ) throw() ;
				
				
				
				/**
				 * Creates an empty surface, whose display type is 
				 * back-buffer.
				 *
				 * On creation this Surface will contain only black pixels :
				 *  - for RGB surfaces, RGB = [0;0;0]
				 *  - for RGBA surfaces, RGBA = [0;0;0;Pixels::AlphaTransparent]
				 *
				 * Actually all created surfaces have all bytes set to zero 
				 * for efficiency reasons, therefore all pixel colors are 
				 * zero too, which explains the difference between the RGB 
				 * and the RGBA cases. 
				 * In the latter case, use 'fill(Pixels::Black)' to have 
				 * all pixels set to a non-fully transparent color such as
				 * RGBA = [0;0;0;Pixels::AlphaOpaque].
				 *
				 * A surface is RGBA iff its alpha mask is non null. 
				 * It in turn implies that the AlphaBlending flag is set.
				 *
				 * @param flags specify the type of Surface that should be
				 * created. This can be any combination of :
				 *		- Surface::Software
				 *		- Surface::Hardware
				 *		- Surface::ColorkeyBlit
				 *		- Surface::AlphaBlendingBlit
				 *
				 * If color masks are all zeros (for instance, if their 
				 * value is not specified) then they will be set as RGBA
				 * coordinates convenient for OpenGL, according to the
				 * endianness of the system.
				 *
				 * @param width the width of this new surface.
				 *
				 * @param height the height of this new surface.
				 *
				 * @param depth if its value is 8 (bits), an empty palette 
				 * is allocated for the surface, otherwise a 'packed-pixel'
				 * pixel format is created, using the [RGBA] mask's provided, 
				 * if any, or the default masks.
				 *
				 * @param redMask the mask to be used to isolate the red
				 * component.
				 *
				 * @param greenMask the mask to be used to isolate the 
				 * green component.
				 *
				 * @param blueMask the mask to be used to isolate the 
				 * blue component.
				 *
				 * @param alphaMask the mask to be used to isolate the 
				 * alpha component. If non null, the 
				 * Surface::AlphaBlendingBlit flag is automatically set.
				 *
				 * @throw VideoException if an error occurred.
				 *
				 * @note If all four color masks are left undefined or are 
				 * set to zero, then the masks will be set automatically 
				 * to the ones expected by OpenGL for textures.
				 *
				 * @note If an alpha-channel is specified (that is, if 
				 * alphaMask is nonzero), then the Surface::AlphaBlendingBlit
				 * flag is automatically set. 
				 * You may remove this flag by calling setAlpha after 
				 * surface creation.
				 *
				 */
				Surface( Ceylan::Flags flags, Length width, Length height, 
					BitsPerPixel depth = 32,
					Pixels::ColorMask redMask   = 0, 
					Pixels::ColorMask greenMask = 0,
					Pixels::ColorMask blueMask  = 0, 
					Pixels::ColorMask alphaMask = 0 ) 
						throw( VideoException ) ;
					
							 
				 
				/**
				 * Surface virtual destructor, frees internal buffer 
				 * if needed.
				 *
				 */
				virtual ~Surface() throw() ;
	
				
				/**
				 * Clones this Surface : creates a new Surface, exact copy 
				 * of this Surface, but with no data shared with it at all. 
				 * The display type of the clone will be back-buffer.
				 *
				 * @throw VideoException on failure, such as the 
				 * impossibility of creating the new surface, or to blit 
				 * the former one.
				 *
				 * @note Returns 'Clonable' instead of 'Surface' since 
				 * adjusting pointers for covariant returns is not 
				 * implemented yet with gcc (3.3.5).
				 *
				 */
				virtual Clonable & clone() const 
					throw( Ceylan::ClonableException ) ;
				

				/**
				 * Returns the SDL surface corresponding to that surface.
				 *
				 * @note The returned surface can be the null pointer.
				 *
				 * @note This surface keeps ownership of the returned SDL
				 * surface.
				 *
				 * @return the SDL surface corresponding to that surface.
				 *
				 */
				virtual SDL_Surface & getSDLSurface() const throw() ;
				
				
				/**
				 * Sets this surface so that it corresponds to the 
				 * supplied SDL surface.
				 *
				 * @param newSurface the new surface to be managed
				 *
				 * @param displayType tells whether this new surface 
				 * is a backbuffer or a screen surface.
				 *
				 * @note Deallocates any previously held surface.
				 *
				 */
				virtual void setSDLSurface( SDL_Surface & newSurface, 
					DisplayType displayType = BackBuffer ) throw() ;
	
	
	
				/**
				 * Returns the display type (back-buffer, screen, etc.) 
				 * of this Surface.
				 *
				 */
				virtual DisplayType getDisplayType() const throw() ;
	
	
				/**
				 * Sets the new display type (back-buffer, screen, etc.) 
				 * of this surface.
				 *
				 */
				virtual void setDisplayType( DisplayType newDisplayType )
					throw() ;
				
							
				/**
				 * Returns the flags caracterizing this surface.
				 *
				 * @see the static Flags defined in this class.
				 *
				 */
				virtual Ceylan::Flags getFlags() const throw() ;

				
				/**
				 * Sets the flags caracterizing this surface.
				 *
				 * @note Does not change anything to the surface, though. 
				 * 
				 * @see the static Flags defined in this class.
				 *
				 */				
				virtual void setFlags( Ceylan::Flags newFlags ) throw() ;
	
	
				/**
				 * Converts the internal surface to the pixel format of 
				 * the display, so that any blit of this surface to the 
				 * screen is quicker.
				 *
				 * The backend must already be initialized.
				 *
				 * @param alphaChannelWanted tells whether this converted
				 * surface should have an alpha channel after this call. 
				 *
				 * This method can be used to convert a colorkey to an 
				 * alpha channel, if the ColorkeyBlit flag is already set 
				 * on this surface and if alphaChannelWanted is true.
				 * The generated surface will then be transparent
				 * (alpha=AlphaTransparent) where the pixels match the 
				 * colorkey, and opaque (alpha=AlphaOpaque) elsewhere.
				 *
				 * @throw VideoException if no internal surface is 
				 * available, if the conversion failed or if there is not 
				 * enough memory.
				 *
				 */
				virtual void convertToDisplay( bool alphaChannelWanted = true ) 
					throw( VideoException ) ;
	
	
				/**
				 * Adjusts the alpha properties of the surface : sets the
				 * per-surface alpha value and/or enables or disables 
				 * alpha blending.
				 *
				 * @param flags can be an OR'd combination of the following 
				 * two options, one of these options, or 0 (none) :
				 *  - Surface::AlphaBlendingBlit
				 *  - Surface::RLEColorkeyBlit
				 *
				 * @param newAlpha is the per-surface alpha value. 
				 * A surface needs not to have an alpha channel to use
				 * per-surface alpha, and blitting can still be accelerated
				 * with Surface::RLEColorkeyBlit.
				 *
				 * @note 0 (AlphaTransparent) is considered fully transparent
				 * and 255 (AlphaOpaque) is considered fully opaque.
				 *
				 * @note The per-surface alpha value of 128 is considered 
				 * a special case and is optimised, so it is much faster 
				 * than other per-surface values.
				 *
				 */
				virtual void setAlpha( Ceylan::Flags flags, 
					Pixels::ColorElement newAlpha ) throw( VideoException ) ;
				
				
				/**
				 * Sets the color key (transparent pixel) in a blittable
				 * surface, and enables or disables RLE blit acceleration.
				 *
				 * RLE acceleration can substantially speed up blitting of
				 * images with large horizontal runs of transparent pixels
				 * (i.e. pixels that match the key value).
				 * The key must be defined accoring to the same pixel format 
				 * as the surface.
				 *
				 * @param flag can be a OR'ed combination :
				 *		- if Surface::ColorkeyBlit is set, then the key is 
				 * the transparent pixel value in the source image of a blit.
				 *		- if Surface::RLEColorkeyBlit is set, RLE acceleration
				 * will be used when blitting this surface. 
				 * The RLE encoding will be performed the first time the
				 * surface is blitted or when it will be converted to display
				 * format.
				 *
				 * If flag is 0, this function clears any current color key.
				 *
				 * @param keyPixelColor, the pixel color of the key. 
				 * Pixels::convertColorDefinitionToPixelColor is often useful
				 * for obtaining a correct value for the color key.
				 *
				 */
				virtual void setColorKey( Ceylan::Flags flags, 
					Pixels::PixelColor keyPixelColor ) throw( VideoException ) ;
				
				
				/**
				 * Sets the color key (transparent pixel) in a blittable
				 * surface, and enables or disables RLE blit acceleration.
				 *
				 * RLE acceleration can substantially speed up blitting of
				 * images with large horizontal runs of transparent pixels
				 * (i.e. pixels that match the key value).
				 * The key must be defined accoring to the same pixel format 
				 * as the surface.
				 *
				 * @param flag can be a OR'ed combination :
				 *		- if Surface::ColorkeyBlit is set, then the key is 
				 * the transparent pixel value in the source image of a blit.
				 *		- if Surface::RLEColorkeyBlit is set, RLE 
				 * acceleration will be used when blitting this surface. 
				 * The RLE encoding will be performed the first time the
				 * surface is blitted or when it will be converted to 
				 * display format.
				 *
				 * If flag is 0, this function clears any current color key.
				 *
				 * @param keyColorDef the color definition of the color key.
				 */
				virtual void setColorKey( Ceylan::Flags flags, 
						Pixels::ColorDefinition keyColorDef ) 
					throw( VideoException ) ;
				
				
				/**
				 * Converts this surface, which must use a color key, so 
				 * that it uses an alpha channel instead.
				 *
				 * This method uses 'convertToDisplay'.
				 *
				 * @throw VideoException if no internal surface is available, 
				 * if it does not use color key, if the conversion failed 
				 * or if there is not enough memory.
				 *
				 */
				virtual void convertFromColorKeyToAlphaChannel() 
					throw( VideoException ) ; 
				
				
				/**
				 * Sets this surface's palette thanks to specified one.
				 *
				 * @param newPalette the palette from which new color
				 * definitions should be taken.
				 *
				 * @param startingColorIndex the color-definition index in
				 * newPalette from which color definitions will be taken.
				 *
				 * @param numberOfColors the number of colors that should 
				 * be copied, starting from <b>startingColorIndex</b>. 
				 * If zero, all remaining color definitions from newPalette 
				 * will be taken.
				 *
				 * @param targettedPalettes a flag which tells whether 
				 * logical palette, physical one or both (OR'ed) should 
				 * be updated.
				 *
				 * @return true if all colours were set exactly as requested,
				 * false otherwise.
				 *
				 */
				virtual bool setPalette( Palette & newPalette, 
					ColorCount startingColorIndex = 0,
					ColorCount numberOfColors = 0, 
					Ceylan::Flags targettedPalettes = 
						Palette::Logical | Palette::Physical ) 
					throw() ;
				
				
				
				/// Returns this surface's pixel format.
				virtual PixelFormat & getPixelFormat() const throw() ;
				
				/// Sets this surface's pixel format.
				virtual void setPixelFormat( PixelFormat & newFormat ) throw() ;


	
				/// Returns this surface's pitch.
				virtual Pitch getPitch() const throw() ;
				
				/// Sets this surface's pitch.
				virtual void setPitch( Pitch newPitch ) throw() ;


				/// Returns this Surface's width.
				virtual Length getWidth() const throw() ;
				
				
				/**
				 * Sets this Surface's width.
				 *
				 * If the new width is different from the current one, 
				 * triggers a resize that will blank (in pure black) this
				 * surface and set its 'needs redraw' state to true.
				 *
				 */
				virtual void setWidth( Length newWidth ) throw() ;
	
	
	
				/// Returns this Surface's height.
				virtual Length getHeight() const throw() ;
				
				
				/**
				 * Sets this Surface's height.
				 *
				 * If the new height is different from the current one, 
				 * triggers a resize that will blank (in pure black) this
				 * surface and set its 'needs redraw' state to true.
				 *
				 */
				virtual void setHeight( Length newHeight ) throw() ;
				
				
				/**
				 * Resizes this surface so that its new dimensions are the
				 * specified ones.
				 *
				 * If the new dimensions are the same as the current ones, 
				 * then nothing special will be done, surface stays as is. 
				 * Otherwise, the surface will be resized and, depending of 
				 * the 'scaleContent' flag, its content will be scaled 
				 * (if true) or lost and replaced by a pure black
				 * surface, with the 'needs redraw' state set to true (if
				 * false).
				 *
				 * @param newWidth the new surface width.
				 *
				 * @param newHeight the new surface height;
				 *
				 * @param scaleContent tells whether content should be 
				 * scaled, or lost. 
				 * If true, current content will be scaled, and antialiased 
				 * if the Video module state machine says so.
				 *
				 */
				virtual void resize( Length newWidth, Length newHeight, 
					bool scaleContent = false ) throw() ;


				/**
				 * Returns this surface's color depth, <b>bits</b> per pixel.
				 *
				 * @note Do not mix up with <b>bytes</b> per pixels.
				 *
				 * @see getBytesPerPixel
				 *
				 */
				virtual BitsPerPixel getBitsPerPixel() const throw() ;
				
				
				/**
				 * Sets this surface's color depth, bits per pixel.
				 *
				 * @note This method does not update the BytesPerPixel 
				 * settings.
				 *
				 */
				virtual void setBitsPerPixel( BitsPerPixel newBitsPerPixel )
					throw() ;
				
				
				/**
				 * Returns this surface's overall color depth, <b>bytes</b> 
				 * per pixel.
				 *
				 * @note Do not mix up with <b>bits</b> per pixels.
				 *
				 * @see getBitsPerPixel
				 *
				 */
				virtual BytesPerPixel getBytesPerPixel() const throw() ;
				
				
				/**
				 * Sets this surface's color depth, <b>bytes</b> per pixel.
				 *
				 * @note This method does not update the BytesPerPixel 
				 * settings.
				 *
				 */
				virtual void setBytesPerPixel( BytesPerPixel newBytesPerPixel )
					throw() ;
				
				
												
				/**
				 * Returns this surface's video buffer, i.e. its whole set 
				 * of pixels.			
				 *
				 */
				virtual void * getPixels() const throw() ;				
				
				
				/**
				 * Sets this surface's video buffer, i.e. its whole set of
				 * pixels.	
				 *
				 */			
				virtual void setPixels( void * newPixels ) throw()	;
						
														
									 
				/**
				 * Fills this surface with specified color. 
				 *
				 * Can be used for blanking a surface, if the background 
				 * color is chosen.
				 *
				 * @param colorDef the color definition of the fill color.
				 *
				 * @return true iff the operation succeeded.
				 *
				 * @note Locks surface if needed.
				 *
				 */
				virtual bool fill( 
						Pixels::ColorDefinition colorDef = Pixels::Black ) 
					throw() ; 

				
				/**
				 * Clears the screen.
				 * 
				 * This method performs a mere 'fill' with pure black.
				 *
				 * @return true iff the operation succeeded.
				 *
				 * @note Locks surface if needed.
				 *
				 */
				virtual bool clear() throw() ; 


				/**
				 * Returns the result of a vertical flip of this source 
				 * surface : the result will contain the original image 
				 * mirrored according to a vertical line splitting this 
				 * surface into a left and right area of equal size.
				 *
				 * Something looking as '>' would be flipped into '<'.
				 *
				 * @return a new surface whose pixels are the ones of this
				 * left-untouched surface, the pixels being vertically 
				 * flipped.
				 *
				 * @note The returned surface will have to be deallocated 
				 * by the caller.
				 *
				 */
				virtual Surface & flipVertical() const throw() ; 


				/**
				 * Returns the result of an horizontal flip of this source
				 * surface : the result will contain the original image 
				 * mirrored according to an horizontal line splitting 
				 * this surface into a top and bottom area of equal size.
				 *
				 * Something looking as '^' would be flipped into 'v'.
				 *
				 * @return a new surface whose pixels are the ones of this
				 * left-untouched surface, the pixels being horizontally
				 * flipped.
				 *
				 * @note The returned surface will have to be deallocated
				 * by the caller.
				 *
				 */
				virtual Surface & flipHorizontal() const throw() ; 



				/**
				 * Returns a textual description of the pixel of this 
				 * surface located at [x;y].
				 *
				 * @param x the abscissa of the pixel.
				 *
				 * @param y the ordinate of the pixel.
				 *
				 * @return a string containing a description (mainly the 
				 * RGBA coordinates) of this pixel.
				 *
				 */
				virtual std::string describePixelAt( 
					Coordinate x, Coordinate y ) throw() ;
				
				

				/**
				 * Returns the pixel color of one particular pixel of this
				 * surface, located at [x;y], encoded according to the 
				 * pixel format of this surface.
				 *
				 * @return the encoded pixel color of the pixel at [x;y].
				 *
				 * No clipping is performed, the surface should have been
				 * previously locked if necessary.
				 *
				 * @see OSDL::Video::Pixels::get
				 *
				 */	
				virtual Pixels::PixelColor getPixelColorAt( 
						Coordinate x, Coordinate y ) const
					throw ( VideoException ) ;


				/**
				 * Returns the color definition of one particular pixel 
				 * of this surface, located at [x;y].
				 *
				 * If this surface has no alpha coordinate, then the fourth
				 * color coordinate of the returned definition will be
				 * AlphaOpaque.
				 *
				 * No clipping is performed, the surface should have been
				 * previously locked if necessary.
				 *
				 * @see OSDL::Video::Pixels::get
				 *
				 */	
				virtual Pixels::ColorDefinition getColorDefinitionAt( 
						Coordinate x, Coordinate y ) 
					const throw ( VideoException ) ;
				 
				 
				/**
				 * Sets the pixel at [x;y] of this surface to the given 
				 * color, specified as separate RGBA coordinates.
				 *
			 	 * @param x the abscissa of the point to change.
			 	 *
			 	 * @param y the ordinate of the point to change.
			 	 *
				 * @param red the red color coordinate.
			 	 *
			 	 * @param green the green color coordinate.
			 	 *
			 	 * @param blue the blue color coordinate.
			 	 *
			 	 * @param alpha the alpha color coordinate.
			 	 *			 
			 	 * @param blending tells whether the alpha channel must be 
				 * taken into account, resulting to alpha blending with 
				 * the destination pixel. 
				 * If false, the exact specified color will be put in 
				 * target pixel, instead of being blended with it.
			 	 *
			 	 * @param clipping tells whether point location is checked
				 * against surface bounds.
			 	 * If clipping is activated and the pixel is outside, nothing 
				 * is done.
			 	 *
			 	 * @param locking tells whether this primitive should take 
				 $ care of locking / unlocking the surface (not recommended 
				 * on a per pixel basis, because of lock overhead)
				 *
			 	 * @throw VideoException if a problem occurs with a lock
				 * operation. 
				 *
			 	 * @note The four RGBA coordinates will be automatically 
				 * mapped according to the target surface's pixel format.
		 	 	 *
			 	 * @see getPixel.
				 *
				 * @see OSDL::Video::Pixels::put.
				 *
				 */
				virtual void putRGBAPixelAt( 
					Coordinate x, Coordinate y, 
					ColorElement red, ColorElement green, ColorElement blue, 
					ColorElement alpha = AlphaOpaque, 
					bool blending = true, bool clipping = true, 
					bool locking = false ) throw( VideoException ) ;


				/**
				 * Sets the pixel at [x;y] of this surface to the given 
				 * color, specified as a color definition (set of RGBA
				 * coordinates).
				 *
			 	 * @param x the abscissa of the point to change.
			 	 *
			 	 * @param y the ordinate of the point to change.
			 	 *
				 * @param colorDef the RGBA color definition of the put pixel.
			 	 *
			 	 * @param blending tells whether the alpha channel must be 
				 * taken into account, resulting to alpha blending with 
				 * the destination pixel. 
				 * If false, the exact specified color will be put in target
				 * pixel, instead of being blended with it.
			 	 *
			 	 * @param clipping tells whether point location is checked
				 * against surface bounds.
			 	 * If clipping is activated and the pixel is outside, 
				 * nothing is done.
			 	 *
			 	 * @param locking tells whether this primitive should take 
				 * care of locking / unlocking the surface (not recommended 
				 * on a per pixel basis because of lock overhead)
				 *
			 	 * @throw VideoException if a problem occurs with a lock
				 * operation. 
				 *
			 	 * @note The four RGBA coordinates will be automatically 
				 * mapped according to the target surface's pixel format.
		 	 	 *
			 	 * @see getPixel
				 *
				 * @see OSDL::Video::Pixels::put
				 *
				 */
				virtual void putColorDefinitionAt( Coordinate x, Coordinate y, 
					ColorDefinition colorDef, 
					bool blending = true, bool clipping = true, 
					bool locking = false ) throw( VideoException ) ;


				/**
				 * Sets the pixel at [x;y] of this surface to the given color,
				 * specified as an already converted RGBA color.
				 *
				 * @param alpha the alpha coordinate of the pixel to be put 
				 * with full precision : the alpha encoded in the converted
				 * color may not be reliable.
				 *
			 	 * @param blending tells whether the alpha channel must be 
				 * taken into account, resulting to alpha blending with 
				 * the destination pixel. If false, the exact specified 
				 * color will be put in target pixel, instead of being 
				 * blended with it.
				 *
			 	 * @param clipping tells whether point location is checked
				 *  against surface bounds.
			 	 * If clipping is activated and the pixel is outside, nothing 
				 * is done.
			 	 *
			 	 * @param locking tells whether this primitive should take 
				 * care of locking / unlocking the surface (not recommended 
				 * on a per pixel basis, because of lock overhead).
				 *
				 */
				virtual void putPixelColorAt( Coordinate x, Coordinate y, 
					PixelColor convertedColor, ColorElement alpha,
					bool blending = true, bool clipping = true, 
					bool locking = false ) throw( VideoException ) ;
				
				
				/**
				 * Sets the alpha coordinate of all pixel matching RGB color
				 * described by <b>colorDef</b> to <b>newAplha</b>, by 
				 * scanning all the pixels of this RGBA surface.
				 *
				 * @example if colorDef is [ x; y; z; t ] (t is ignored), 
				 * then all the pixels of this surface whose RBG values are
				 * equal to [ x; y ; z; * ] will be set to
				 * [ x; y ; z; newAlpha ], regardless of their previous alpha
				 * coordinate.
				 *
				 * @param colorDef the color definition describing the 
				 * RGB value whose alpha coordinate has to be replaced.
				 *
				 * @param newAlpha the alpha coordinate that must be 
				 * assigned to matching pixels.
				 *
				 * @return false iff something went wrong (ex : this is not 
				 * a RGBA surface).
				 *
				 * @note This method is rather expensive (slow) but useful 
				 * since blits cannot achieve that effect.
				 *
				 */
				virtual bool setAlphaForColor( Pixels::ColorDefinition colorDef,
					Pixels::ColorElement newAlpha ) throw() ;
					
					
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), included if and only if
				 * endpoint drawing mode is set, with specified RGBA color, 
				 * in this surface.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawHorizontalLine( 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque )
						throw() ;
			
					
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), included if and only if
				 * endpoint drawing mode is set, with specified actual RGBA
				 * pixel color, in this surface.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawHorizontalLine( 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::PixelColor actualColor ) throw() ;
			
					
				/**
				 * Draws an horizontal line ranging from point (xStart;y),
				 * included, to point (xStop;y), included if and only if
				 * endpoint drawing mode is set, with specified RGBA color, 
				 * in this surface.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawVertical, draw
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawHorizontalLine( 
					Coordinate xStart, Coordinate xStop, Coordinate y, 
					Pixels::ColorDefinition colorDef = Pixels::White )
						throw() ;
					
					
				/**
				 * Draws a vertical line ranging from point (x;yStart),
				 * included, to point (x;yStop), included if and only if
				 * endpoint drawing mode is set, with specified RGBA color, 
				 * in this surface.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawHorizontal, draw
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawVerticalLine(
					Coordinate x, Coordinate yStart, Coordinate yStop, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque )
						throw() ;								
					
					
				/**
				 * Draws a vertical line ranging from point (x;yStart),
				 * included, to point (x;yStop), included if and only if
				 * endpoint drawing mode is set, with specified RGBA color, 
				 * in this surface.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawHorizontal, draw
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState.
				 *
				 */
				virtual bool drawVerticalLine(
						Coordinate x, Coordinate yStart, Coordinate yStop, 
						Pixels::ColorDefinition colorDef = Pixels::White )
					throw() ;								


				/**
				 * Draws a line in this surface, ranging from point
				 * (xStart;yStart), included, to point (xStop;yStop), 
				 * included if and only if endpoint drawing mode is set, 
				 * with specified RGBA color.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawLine(
					Coordinate xStart, Coordinate yStart, 
					Coordinate xStop, Coordinate yStop, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque )
						throw() ;
		
					
				/**
				 * Draws a line in this surface, ranging from point
				 * (xStart;yStart), included, to point (xStop;yStop), 
				 * included if and only if endpoint drawing mode is set, 
				 * with specified RGBA color.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This line will be antialiased if antialias mode 
				 * is set.
				 *
				 * @see drawHorizontal, drawVertical
				 *
				 * @see VideoModule::GetEndPointDrawState,
				 * VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawLine(
					Coordinate xStart, Coordinate yStart, 
					Coordinate xStop, Coordinate yStop, 
					Pixels::ColorDefinition = Pixels::White )
						throw() ;
				
					
				/**
				 * Draws a cross centered in <b>center</b>, included in a 
				 * square whose length of edge is <b>squareEdge</b>.
				 *
				 * @note Useful to mark a point.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This cross will be antialiased if antialias mode 
				 * is set.
				 *
				 */
				virtual bool drawCross( const TwoDimensional::Point2D & center, 
						Pixels::ColorDefinition colorDef = Pixels::White, 
						Length squareEdge = 5 )
					throw() ;
					
					
				/**
				 * Draws a cross centered in <b>center</b>, included in a 
				 * square whose length of edge is <b>squareEdge</b>.
				 *
				 * @note Useful to mark a point.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note This cross will be antialiased if antialias mode 
				 * is set.
				 *
				 */
				virtual bool drawCross( Coordinate xCenter, Coordinate yCenter, 
						Pixels::ColorDefinition colorDef = Pixels::White, 
						Length squareEdge = 5 )
					throw() ;
					
					
				/**
				 * Draws the edges of this surface, with specified color 
				 * and edge width.
				 *
				 * @param edgeColor the color of edges.
				 *
				 * @param edgeWidth the width of edges.
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 */
				virtual bool drawEdges( 
					Pixels::ColorDefinition edgeColor = Pixels::White, 
					Length edgeWidth = 1 ) throw() ;
					
						
				/**
				 * Draws a box with specified RGBA color.
				 * 
				 * @param filled tells whether the drawn rectangle should 
				 * be filled with specified color, or if only its sides 
				 * should be drawn.
				 *
				 * @note Locks surface if needed.
				 *
				 */
				virtual bool drawBox( const UprightRectangle & rectangle, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true ) throw() ;
				
				
				/**
				 * Draws a box with specified RGBA color.
				 * 
				 * @param rectangle the box to fill
				 *
				 * @param colorDef the fill color
				 *
				 * @param filled tells whether the drawn rectangle should 
				 * be filled with specified color, or if only its sides 
				 * should be drawn.
				 *
				 * @note Locks surface if needed.
				 *
				 */
				virtual bool drawBox( const UprightRectangle & rectangle, 
						Pixels::ColorDefinition colorDef = Pixels::White, 
						bool filled = true ) 
					throw() ;
				

				/**
			 	 * Draws a circle whose center is (xCenter,yCenter) of 
				 * specified radius, with specified RGBA color, on this 
				 * surface.
			 	 *
			 	 * @param filled tells whether the circle should be filled
				 * (disc).
			 	 *
			 	 * @param blended tells whether, for each pixel of disc, 
				 * the specified color should be blended with the one of the
				 * target pixel (if true), or if the specified color
			 	 * should replace the former one, regardless of any blending 
				 *(if false). 
				 * Note that only discs may be drawn without being blended :
				 * circles are always blended.
				 *
			 	 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
			 	 * 
			 	 * @note Locks surface if needed.
			 	 *
				 * @note Clipping is performed.
			 	 *
			 	 * @note A circle will be antialiased if antialias mode is set.
				 *
			 	 * @see VideoModule::GetAntiAliasingState
			 	 *
			 	 */
				virtual bool drawCircle( 
					Coordinate xCenter, Coordinate yCenter, 
					Length radius, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true, bool blended = true ) throw() ;
				
				
				
				/**
			 	 * Draws a circle whose center is (xCenter,yCenter) of 
				 * specified radius, with specified RGBA color, on this 
				 * surface.
			 	 *
			 	 * @param filled tells whether the circle should be filled
				 * (disc).
			 	 *
			 	 * @param blended tells whether, for each pixel of disc, 
				 * the specified color should be blended with the one of 
				 * the target pixel (if true), or if the specified color
			 	 * should replace the former one, regardless of any blending 
				 * (if false). 
				 * Note that only discs may be drawn without being blended :
				 * circles are always blended.
				 *
			 	 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
			 	 * 
			 	 * @note Locks surface if needed.
			 	 *
				 * @note Clipping is performed.
			 	 *
			 	 * @note A circle will be antialiased if antialias mode is 
				 * set.
				 *
			 	 * @see VideoModule::GetAntiAliasingState
			 	 *
			 	 */
				virtual bool drawCircle( 
					Coordinate xCenter, Coordinate yCenter, 
					Length radius, 
					Pixels::ColorDefinition colorDef = Pixels::White, 
					bool filled = true, bool blended = true ) throw() ;
				
				
				/**
			 	 * Draws a disc whose center is (xCenter,yCenter), of 
				 * specified radius (totalRadius), filled with specified 
				 * color discColorDef, with a ring in it, on its border, 
				 * i.e. starting from innerRadius to totalRadius, colored with 
				 * ringColorDef, on this surface. 
			 	 *
			 	 * @param xCenter the abscissa of the center of the disc.
			 	 *
			 	 * @param yCenter the ordinate of the center of the disc.
			 	 *
			 	 * @param outerRadius the overall disc radius, including 
				 * the border ring.
			 	 *
			 	 * @param innerRadius, the radius from which the ring is 
				 * drawn, until totalRadius is reached.
			 	 *
			 	 * @param ringColorDef the ring color, the color of the 
				 * border of this disk.
			 	 *
			 	 * @param discColorDef the inner color of the disc.
			 	 *
				 * @param blended tells whether, for each pixel of the 
				 * edged disc, the specified color should be blended with 
				 * the one of the target pixel (if true), or if the
				 * specified color should replace the former one, regardless 
				 * of any blending (if false).
				 *
			 	 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
			 	 * 
				 * @note This method is rather expensive (slow), especially 
				 * if 'discColorDef' is not fully opaque and if 'blended' 
				 * is false.
				 *
			 	 * @note Locks surface if needed, cipping is performed, 
				 * disc will be antialiased if antialias mode is set.
				 *
			 	 * @see VideoModule::GetAntiAliasingState
			 	 *
			 	 */
				virtual bool drawDiscWithEdge( 
					Coordinate xCenter, Coordinate yCenter, 
					Length outerRadius, Length innerRadius, 
					Pixels::ColorDefinition ringColorDef = Pixels::Blue, 
					Pixels::ColorDefinition discColorDef = Pixels::White, 
					bool blended = true )
						throw() ;
				


				/**
				 * Draws an ellipse whose center is (xCenter,yCenter), with
				 * specified horizontal and vertical radii, with specified 
				 * RGBA color, on this surface.
				 *
				 * @param filled tells whether the ellipse should be filled.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note An unfilled ellipse will be antialiased if 
				 * antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawEllipse( 
					Coordinate xCenter, Coordinate yCenter,
					Length horizontalRadius, Length verticalRadius,				
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true ) throw() ;



				/**
				 * Draws an ellipse whose center is (xCenter,yCenter), 
				 * with specified horizontal and vertical radii, with 
				 * specified RGBA color, on this surface.
				 *
				 * @param filled tells whether the ellipse should be filled.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note An unfilled ellipse will be antialiased if 
				 * antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawEllipse( 
					Coordinate xCenter, Coordinate yCenter, 
					Length horizontalRadius, Length verticalRadius,
					Pixels::ColorDefinition colorDef, bool filled = true )
						throw() ;

							
														
			 	/**
			 	 * Draws a polygon-based filled pie whose center is
				 * (xCenter,yCenter) of specified radius, with specified 
				 * RGBA color, on this surface, starting from angleStart,
				 * stopping to angleStop, both expressed in degrees.
				 *
				 * @param xCenter the abscissa of the center of the pie.
				 *
				 * @param yCenter the ordinate of the center of the pie.
				 *
				 * @param radius the radius of the pie.
				 *
				 * @param angleStart the starting angle of the pie, in degrees.
				 *
				 * @param angleStop the stopping angle of the pie, in degrees.
				 *
				 * @param red the red color coordinate of fill color.
				 *
				 * @param green the green color coordinate of fill color.
				 *
				 * @param blue the blue color coordinate of fill color.
				 *
				 * @param alpha the alpha color coordinate of fill color.
				 *
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawPie( 
					Coordinate xCenter, Coordinate yCenter, 
					Length radius, 
					Ceylan::Maths::AngleInDegrees angleStart, 
					Ceylan::Maths::AngleInDegrees angleStop,
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) 
						throw() ;
			
			
					
			 	/**
				 * Draws a polygon-based filled pie whose center is
				 * (xCenter,yCenter) of specified radius, with specified 
				 * RGBA color, on this surface, starting from angleStart,
				 * stopping to angleStop.
				 *
				 * @param xCenter the abscissa of the center of the pie.
				 *
				 * @param yCenter the ordinate of the center of the pie.
				 *
				 * @param radius the radius of the pie.
				 *
				 * @param angleStart the starting angle of the pie, in degrees.
				 *
				 * @param angleStop the stopping angle of the pie, in degrees.
				 *
				 * @param colorDef the color definition of fill color.
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 *
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawPie( 
					Coordinate xCenter, Coordinate yCenter, 
					Length radius,
					Ceylan::Maths::AngleInDegrees angleStart, 
					Ceylan::Maths::AngleInDegrees angleStop, 
					Pixels::ColorDefinition colorDef ) throw() ;



				/**
				 * Draws a triangle defined by specified three points, 
				 * with specified RGBA color, on this surface.
				 *
				 * @param filled tells whether the triangle should be filled
				 * (disc).
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawTriangle( 
					Coordinate x1, Coordinate y1, 
					Coordinate x2, Coordinate y2, 
					Coordinate x3, Coordinate y3, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true ) throw() ;



				/**
				 * Draws a triangle defined by specified three points, 
				 * with specified RGBA color, on this surface.
				 *
				 * @param filled tells whether the triangle should be 
				 * filled (disc).
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawTriangle( 
					Coordinate x1, Coordinate y1, 
					Coordinate x2, Coordinate y2, 
					Coordinate x3, Coordinate y3, 
					Pixels::ColorDefinition colorDef, 
					bool filled = true ) throw() ;
					


				/**
				 * Draws a triangle defined by specified three points, 
				 * with specified RGBA color, on this surface.
				 *
				 * @param filled tells whether the triangle should be 
				 * filled (disc).
				 *
				 * @return false if and only if something went wrong
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawTriangle( 
					const TwoDimensional::Point2D & p1, 
					const TwoDimensional::Point2D & p2, 
					const TwoDimensional::Point2D & p3,
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true ) throw() ;



				/**
				 * Draws a triangle defined by specified three points, 
				 * with specified RGBA color, on this surface.
				 *
				 * @param filled tells whether the triangle should be 
				 * filled (disc).
				 *
				 * @return false if and only if something went wrong 
				 * (ex : surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */
				virtual bool drawTriangle( 
					const TwoDimensional::Point2D & p1, 
					const TwoDimensional::Point2D & p2, 
					const TwoDimensional::Point2D & p3, 
					Pixels::ColorDefinition colorDef, bool filled = true )
						throw() ;
					
					
					
				/**
				 * Draws a polygon defined by specified list of points, 
				 * with specified RGBA color, on specified surface.
				 *
				 * @param summits could be a 'const' list of 'const' Point2D
				 * pointers, but the STL would not be able to match it with
				 * mostly used argument 'const list of Point2D non-const
				 * pointers'.
				 *
				 * @param x the abscissa the polygon should be translated 
				 * of, on screen.
				 *
				 * @param y the ordinate the polygon should be translated 
				 * of, on screen.
				 *
				 * @param filled tells whether the polygon should be filled.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */	
				virtual bool drawPolygon( 
					const std::list<TwoDimensional::Point2D *> summits,
					Coordinate x, Coordinate y,
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque,
					bool filled = true ) throw() ;				



				/**
				 * Draws a polygon defined by specified list of points, 
				 * with specified RGBA color, on specified surface.
				 *
				 * @param summits could be a 'const' list of 'const' Point2D
				 * pointers, but the STL would not be able to match it 
				 * with mostly used argument 'const list of Point2D
				 * non-const pointers'.
				 *
				 * @param x the abscissa the polygon should be translated 
				 * of, on screen.
				 *
				 * @param y the ordinate the polygon should be translated 
				 * of, on screen.
				 *
				 * @param filled tells whether the polygon should be filled.
				 *
				 * @return false if and only if something went wrong (ex :
				 * surface lock failed).
				 * 
				 * @note Locks surface if needed.
				 *
				 * @note Clipping is performed.
				 *
				 * @note Will be antialiased if antialias mode is set.
				 *
				 * @see VideoModule::GetAntiAliasingState
				 *
				 */	
				virtual bool drawPolygon( 
					const std::list<TwoDimensional::Point2D *> summits,	
					Coordinate x, Coordinate y,
					Pixels::ColorDefinition colorDef, bool filled = true )
						throw() ;	
					
						
				/**
				 * Draws a grid, with specified space between columns 
				 * and rows and specified colors.
				 *
				 * @param columnStride the number of pixels between two 
				 * columns.
				 *
				 * @param rowStride the number of pixels between two rows.
				 *
				 * @param lineColor the color of the grid lines.
				 *
				 * @param fillBackground tells whether a background 
				 * should be filled.
				 *
				 * @param backgroundColor the color of the background, 
				 * if activated.
				 *
				 * @note Useful to check transparency blits for example.
				 *
				 * @note Locks surface if needed.
				 *
				 */
				 virtual bool drawGrid( 
				 	Length columnStride = 20, 
				 	Length rowStride = 20, 
					Pixels::ColorDefinition lineColor = Pixels::White,
					bool fillBackground = false,  
					Pixels::ColorDefinition backgroundColor = Pixels::Black )
						throw() ; 
				 		
						
						
				/**
				 * Prints specified string on this surface, with specified
				 * color, taking as upper left corner specified coordinates.
				 * Basic font will be used.
				 *
				 * @see Font class for far better text rendering, including
				 * fixed and TrueType fonts.
				 *
				 */				
				virtual bool printText( const std::string & text,
					Coordinate x, Coordinate y, 
					Pixels::ColorElement red, Pixels::ColorElement green, 
					Pixels::ColorElement blue, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque )
						throw() ; 
					
					
				/**
				 * Prints specified string on this surface, with specified
				 * color, taking as upper left corner specified coordinates.
				 * Basic font will be used.
				 *
				 * @see Font class for far better text rendering, including
				 * fixed and TrueType fonts.
				 *
				 */				
				virtual bool printText( const std::string & text,
						Coordinate x, Coordinate y, 
						ColorDefinition colorDef = Pixels::White ) 
					throw() ;
					
				
				
					
				// Blit section.
				
					
				/**
				 * Blits this surface onto specified surface.
				 * 
				 * Blits from a RGBA Surface with AlphaBlendingBlit set 
				 * to another RGBA surface will keep the alpha of the
				 * destination surface. 
				 * This means that you cannot compose two arbitrary RGBA
				 * surfaces this way and get the result you would expect 
				 * from "overlaying" them : the destination alpha will 
				 * work as a mask. 
				 * Hence one might set the alpha of the destination surface 
				 * to targeted value before the blit.
				 *
				 * @param targetSurface the destination surface this 
				 * surface will be blitted to, starting at [0;0] in the
				 * destination surface : both upper left corners will
				 * correspond.
				 *
				 * @throw VideoMemoryLostException if either of the surfaces
				 * was in video memory, and if the video memory was lost :
				 * it then should be reloaded with content and re-blitted.
				 * Otherwise, if blit went wrong differently, throw a commented
				 * VideoException.
				 *				 
				 * @note Should not be called on a locked surface.
				 *
				 */	
				 virtual bool blitTo( Surface & targetSurface ) 
				 	const throw( VideoException ) ;
					
					
				/**
				 * Blits this surface onto specified surface.
				 * 
				 * Blits from a RGBA Surface with AlphaBlendingBlit set 
				 * to another RGBA surface will keep the alpha of the
				 * destination surface. 
				 * This means that you cannot compose two arbitrary RGBA
				 * surfaces this way and get the result you would expect from
				 * "overlaying" them : the destination alpha will work as a
				 * mask. 
				 * Hence one might set the alpha of the destination surface
				 * to targeted value before the blit.
				 *
				 * @param targetSurface the destination surface this 
				 * surface will be blitted to.
				 *
				 * @param x abscissa of the destination surface where 
				 * this surface will be blitted.
				 *
				 * @param y ordinate of the destination surface where this
				 * surface will be blitted
				 *
				 * @throw VideoMemoryLostException if either of the surfaces
				 * were in video memory, and if the video memory was lost : 
				 * it then should be reloaded with content and re-blitted.
				 * Otherwise, if blit went wrong differently, throw a 
				 * commented VideoException.
				 *				 
				 * @note Should not be called on a locked surface.
				 *
				 */	
				 virtual bool blitTo( Surface & targetSurface, 
					Coordinate x, Coordinate y ) const throw( VideoException ) ;
					
					
				/**
				 * Blits this surface onto specified surface.
				 *
				 * @param targetSurface the destination surface this 
				 * surface will be blitted to.
				 *
				 * @param location the point of the destination surface 
				 * where this surface will be blitted.
				 *
				 * @throw VideoMemoryLostException if either of the 
				 * surfaces was in video memory, and if the video memory was
				 * lost : it then should be reloaded with content and
				 * re-blitted. 
				 * Otherwise, if blit went wrong differently, throw a 
				 * commented VideoException.
				 *				 
				 * @note Should not be called on a locked surface.
				 *
				 */	
				 virtual bool blitTo( Surface & targetSurface, 
					const TwoDimensional::Point2D & location ) const 
						throw( VideoException ) ;
		
				 
				/**
				 * Blits this surface onto specified surface.
				 *
				 * @param targetSurface the destination surface this 
				 * surface will be blitted to.
				 *
				 * @param sourceRectangle a clipping rectangle defining 
				 * which part of this surface is to be blitted.
				 *
				 * @param destinationLocation the point of the destination
				 * surface where this surface will be blitted.
				 *
				 * @throw VideoMemoryLostException if either of the 
				 * surfaces were in video memory, and if the video memory 
				 * was lost : it then should be reloaded with content and
				 * re-blitted.
				 *
				 * @note Should not be called on a locked surface.
				 *
				 */	
				 virtual bool blitTo( 
				 		Surface & targetSurface, 
					 	const TwoDimensional::UprightRectangle 
							& sourceRectangle, 
				 		const TwoDimensional::Point2D & destinationLocation ) 
					const throw( VideoException ) ;
				 
				
				
				/*
				 * Rotozoom section.
				 *
				 * With antialiasing not enabled, rotozoom primitives are fast
				 * enough even for some realtime effects, if the CPU is fast 
				 * or bitmaps are small. 
				 *
				 * With antialiasing, the routines are typically used for
				 * pre-rendering content in higher quality (i.e. smoothing),
				 * hence a new render surface is created each time rotozoom 
				 * is called.
				 *
				 */
				 
				
				
				/**
				 * Returns a newly created surface, obtained from this 
				 * surface after a zoom of given factors. 
				 *
				 * If, for this surface, pixel color is not 8 bit or 32 bit,
				 * then a 32-bit surface will be internally created and 
				 * used for the zoom.
				 *
				 * Ownership of the returned surface is transferred to the
				 * caller, who will have to deallocate it when having finished
				 * with it.
				 *
				 * The final rendering speed depends first on antialiasing 
				 * being requested or not, then on resulting surface size.
				 *
				 * A surface is returned instead of zooming directly this
				 * surface to avoid accumulating rounding errors when 
				 * zooming multiple times the same image : better use a 
				 * constant source surface from which zoomed ones are 
				 * created with various zoom factors than changing the 
				 * same surface again and again.
				 *
				 * @param abscissaZoomFactor the zoom factor to be applied 
				 * on the abscissa axis. 
				 * If negative, the surface is flipped against this axis. 
				 * No antialiasing will be performed in this case.
				 *
				 * @param ordinateZoomFactor the zoom factor to be applied 
				 * on the ordinate axis. 
				 * If negative, the surface is flipped against this axis. 
				 * No antialiasing will be performed in this case.
				 *
				 * @param antialiasing if true and if zoom factors are 
				 * positive, requests antialiasing (fine and expensive
				 * interpolation). 
				 * 8-bit surfaces will never be antialiased.
				 *
				 */
				virtual Surface & zoom( 
						Ceylan::Maths::Real abscissaZoomFactor, 
						Ceylan::Maths::Real ordinateZoomFactor,	
						bool antialiasing = true ) 
					const throw( VideoException ) ;	
				
				
				
				/**
				 * Returns a newly created surface, obtained from this 
				 * surface after a rotation of given angle and a zoom of 
				 * given factor. 
				 *
				 * If, for this surface, pixel color is not 8 bit or 32 bit,
				 * then a 32-bit surface will be internally created and 
				 * used for the rotozoom.
				 *
				 * Ownership of the returned surface is transferred to the
				 * caller, who will have to deallocate it when having finished
				 * with it.
				 *
				 * The final rendering speed depends first on antialiasing 
				 * being requested or not, then on resulting surface size.
				 *
				 * A surface is returned instead of rotating this surface to
				 * avoid accumulating rounding errors when rotozooming 
				 * multiple times the same image : better use a constant 
				 * source surface from which rotozoomed ones are created 
				 * with various angles and zoom factors than changing the 
				 * same surface again and again.
				 * 
				 * @param angle the angle of rotation, in degrees.
				 *
				 * @param zoomFactor the zoom factor to be applied on both
				 * surface dimensions.
				 * If negative, the surface is rotated of Pi before the 
				 * angle is applied. 
				 * No antialiasing will be performed in this case.
				 *
				 * @param antialiasing if true and if zoom factor is 
				 * positive, requests antialiasing (fine and expensive
				 * interpolation). 
				 * 8-bit surfaces will never be antialiased.
				 *
				 */
				virtual Surface & rotoZoom( 
						Ceylan::Maths::AngleInDegrees angle, 
						Ceylan::Maths::Real zoomFactor, 
						bool antialiasing = true ) 
					const throw( VideoException ) ;	
				
				
				
				/**
				 * Returns a newly created surface, obtained from this 
				 * surface after a rotation of given angle and zooms of 
				 * given factors. 
				 *
				 * If, for this surface, pixel color is not 8 bit or 32 bit,
				 * then a 32-bit surface will be internally created and 
				 * used for the rotozoom.
				 *
				 * Ownership of the returned surface is transferred to 
				 * the caller, who will have to deallocate it when finish 
				 * with it.
				 *
				 * The final rendering speed depends first on antialiasing 
				 * being requested or not, then on resulting surface size.
				 *
				 * A surface is returned instead of rotating this surface 
				 * to avoid accumulating rounding errors when rotozooming
				 * multiple times the same image : better use a constant 
				 * source surface from which rotozoomed ones are created 
				 * with various angles and zoom factors than changing the 
				 * same surface again and again.
				 * 
				 * @param angle the angle of rotation, in degrees.
				 *
				 * @param abscissaZoomFactor the zoom factor to be applied 
				 * on the abscissa axis. 
				 * If negative, the surface is flipped against this axis.
				 * No antialiasing will be performed in this case.
				 *
				 * @param ordinateZoomFactor the zoom factor to be 
				 * applied on the ordinate axis. 
				 * If negative, the surface is flipped against this axis. 
				 * No antialiasing will be performed in this case.
				 *
				 * @param antialiasing if true and if zoom factors are 
				 * positive, requests antialiasing (fine and expensive
				 * interpolation). 
				 * 8-bit surfaces will never be antialiased.
				 *
				 */
				virtual Surface & rotoZoom( 
						Ceylan::Maths::AngleInDegrees angle, 
						Ceylan::Maths::Real abscissaZoomFactor, 
						Ceylan::Maths::Real ordinateZoomFactor,
						bool antialiasing = true ) 
					const throw( VideoException ) ;	
				
				
				
					
				// Image section.
					
					
				/**
				 * Returns this surface's clipping area, expressed thanks 
				 * to an UprightRectangle. 
				 *
				 * Should not be confused with the actual area corresponding 
				 * to this surface, which is another UprightRectangle.
				 *
				 * @return A new UprightRectangle, whose ownership is 
				 * transferred to the caller, who therefore has to 
				 * deallocate it when having finished with it.
				 *
				 */
				virtual UprightRectangle & getClippingArea() const throw() ;
				
	
				/**
				 * Sets this surface's clipping area, expressed thanks to an
				 * UprightRectangle, whose ownership is left to the caller.
				 *
				 * Should not be confused with the actual area corresponding to
				 * this surface, which is another UprightRectangle.
				 *
				 */
				virtual void setClippingArea( 
					UprightRectangle & newClippingArea ) throw() ;
					
					
					
				/**
				 * Loads an image, whose format will be auto-detected, 
				 * from specified file, into this surface. 
				 * This is a Surface factory.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the image file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::load
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadImage( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				 
				 
				/**
				 * Loads a JPEG image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the JPEG file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadJPG
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadJPG( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
					
								
				/**
				 * Loads a PNG image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the PNG file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadPNG
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadPNG( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
								
				/**
				 * Loads a BMP image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the BMP file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadBMP
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadBMP( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
								
				/**
				 * Loads a GIF image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the GIF file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadGIF
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadGIF( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
								
				/**
				 * Loads a LBM image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the LBM file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadLBM
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadLBM( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
								
				/**
				 * Loads a PCX image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the PCX file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadPCX
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadPCX( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
						
								
				/**
				 * Loads a PNM image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the PNM file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadPNM
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadPNM( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
								
				/**
				 * Loads a TGA image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the TGA file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadTGA
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadTGA( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
				
								
								
				/**
				 * Loads a XPM image from specified file into this surface.
				 *
				 * @param targetSurface the surface that should correspond to
				 * the loaded image.
				 *
				 * @param filename the name of the XPM file.
				 *
				 * @param blitOnly tells whether the loaded image surface
				 * should only be blitted into the supposed already existing
				 * internal surface and then be deallocated (if flag is true), 
				 * or if this loaded surface should simply replace the former 
				 * one (if flag is false).  
				 * 
				 * @param convertToDisplayFormat tells whether this loaded 
				 * image should have its format converted to the screen's
				 * format, in order to offer faster blits if ever that 
				 * surface was to be blitted multiple times to the screen
				 * (one-time-for-all conversion).
				 *
				 * @param convertWithAlpha if the conversion to screen 
				 * format is selected (convertToDisplayFormat is true), 
				 * tells whether the converted surface should also have an 
				 * alpha channel.  				 
				 *
				 * @note Specifying a blitOnly option set to true is especially
				 * convenient when selecting the screen's surface as target
				 * surface, since this surface is special and should not be 
				 * deallocated as the others may be.
				 * 
				 * @see Image::loadXPM
				 *
				 * @fixme Should be inlined.			 
				 *
				 */					 
				virtual void loadXPM( const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
			


				/**
				 * Saves the current content of this surface into a PNG file.
				 *
				 * @param filename the name of the PNG file to be created 
				 * (ex : 'screenshot.png')
				 * 
				 * @param overwrite tells whether an existing file
				 * <b>filename</b> should be overwritten, or if an 
				 * exception should be raised.
				 *
				 * @note This method is especially useful for screenshots.
				 *
				 * @see loadPNG.
				 *
				 * This method cannot be 'const' since this surface may 
				 * have to be locked in order to access the pixels that 
				 * are to be saved. 
				 *
				 */
				virtual void savePNG( const std::string & filename, 
						bool overwrite = true ) 
					throw( TwoDimensional::ImageException ) ;
								
				
				
				/**
				 *Saves the current content of this surface into a BMP file.
				 *
				 * @param filename the name of the BMP file to create 
				 * (ex : 'screenshot.bmp')
				 * 
				 * @param overwrite tells whether an existing file
				 * <b>filename</b> should be overwritten, or if an exception
				 * should be raised.
				 *
				 * @note The savePNG method should be preferred to this one.
				 *
				 * @see savePNG, loadBMP
				 *
				 * This method cannot be 'const' since this surface may 
				 * have to be locked in order to access the pixels that 
				 * are to be saved. 
				 *
				 */
				virtual void saveBMP( const std::string & filename, 
						bool overwrite = true ) 
					throw( TwoDimensional::ImageException ) ;
				
				
				
				/**
				 * Updates the display with this surface, which will be
				 * automatically set in an up-to-date state if necessary, 
				 * so that it is fully redrawn, including its widgets.
				 *
				 * If it is a screen surface (either OpenGL or not), it 
				 * will be updated in order to have the display match the
				 * surface buffer. 
				 * Does nothing more if this surface is not a screen surface.
				 *
				 * @note When this method is called, the surface is supposed 
				 * not to be locked.
				 *
				 *  - on hardware that supports double-buffering, this method
				 * sets up a flip and returns. 
				 * The hardware will wait for vertical retrace, and then 
				 * swap video buffers before the next video surface blit 
				 * or lock will return. 
				 * - on hardware that does not support double-buffering, 
				 * this is equivalent to calling the updateRectangle method
				 * with all arguments to zero.
				 *
				 * @note The Surface::DoubleBuffered flag must have been 
				 * passed to setMode, when setting the video mode, 
				 * for this function to have a chance to perform hardware
				 * flipping.
				 *
				 * @see SDL_Flip
				 * 
				 * @throw VideoException if the operation failed, including
				 * if this surface is not a screen surface.
				 *
				 */
				virtual void update() throw( VideoException ) ;
			
			
			
				/**
				 * Updates the part of this surface corresponding to the
				 * specified list of rectangles.
				 *
				 * @note The rectangles must be confined within the screen
				 * boundaries (no clipping is done).
				 *
				 * @note The surface must not be locked when this method is
				 * called.
				 *
				 * @note It is advised to call this method only once per 
				 * frame, since each call has some processing overhead. 
				 * This is no restriction, since you can pass any number of
				 * rectangles each time.
				 *
				 * The rectangles are not automatically merged or checked 
				 * for overlap. 
				 * In general, the programmer can use his knowledge about 
				 * his particular rectangles to merge them in an efficient 
				 * way, to avoid overdraw.
				 *
				 * @note STL too clumsy to understand 
				 * list<const UprightRectangle const *> etc.
				 * (would require conversion by copy)
				 * 
				 * @throw VideoException if the operation failed, including
				 * if this surface is not a screen surface.
				 *
				 */
				 virtual void updateRectangles( 
				 		const std::list<UprightRectangle *> & listRects )
				 	throw( VideoException ) ;
					
					
				/**
				 * Updates the part of this surface corresponding to 
				 * specified rectangle.
				 *
				 * @note The rectangle must be confined within the screen
				 * boundaries (no clipping is done).
				 *
				 * @note The surface must not be locked when this method 
				 * is called.
				 *
				 * @throw VideoException if the operation failed, including
				 * if this surface is not a screen surface.
				 * 
				 */
				 virtual void updateRectangle( const UprightRectangle & rect )	
				 	throw( VideoException ) ;
				
					
				/**
				 * Updates the part of this surface corresponding to 
				 * specified rectangle.
				 *
				 * @note The rectangle must be confined within the screen
				 * boundaries (no clipping is done).
				 *
				 * @note If all arguments are equal to zero, the entire 
				 * surface will be updated.
				 *
				 * @note The surface must not be locked when this method is
				 * called.
				 * 
				 * @throw VideoException if the operation failed, including
				 * if this surface is not a screen surface.
				 *
				 */
				 virtual void updateRectangle( 
				 	Coordinate x, Coordinate y, Length width, Length height )
						throw( VideoException ) ;
					


				/**
				 * Sets the need for redraw state.
				 *
				 * @note Meant to be overriden by widget class. 
				 * Public since a widget has to be able to call it on 
				 * its container.
				 *
				 */
				virtual void setRedrawState( bool needsToBeRedrawn ) throw() ;

				
				/// Returns the current need for redraw state.
				virtual bool getRedrawState() const throw() ;
				
						
								
				/**
				 * Redraws this surface if needed, and handles 
				 * automatically its tree of widgets so that it gets 
				 * fully up-to-date and ready to be blitted.
				 *
				 * @return true if and only if the whole redraw was 
				 * performed without any problem.
				 *
				 */
				virtual void redraw() throw() ;
				
				
				/**
				 * Basic do-nothing redraw method for internal rendering.
				 *
				 * @note This method is meant to be overridden, so that this
				 * surface is able to paint itself, i.e. only its background,
				 * its widgets excluded.
				 *
				 */
				virtual void redrawInternal() throw() ;
				 
			
				/**
				 * Tells whether the internal SDL surface is void or not.
				 *
				 * @note By design, this method should always return true.
				 *
				 * @note May be inline.
				 *
				 */
				virtual bool isInternalSurfaceAvailable() const throw() ;
				 
				 
				/**
				 * Registers specified widget to this surface, and put it 
				 * at top level.
				 *
				 * @note The specified widget cannot be 'const' since, 
				 * for example, a redraw event might result in a change 
				 * in the widget.
				 *
				 */
				virtual void addWidget( TwoDimensional::Widget & widget ) 
					throw( VideoException ) ;
		
		
				/**
				 * Returns the surface that should be targeted by widgets 
				 * having to blit their renderings to their container. 
				 *
				 * Usually (but not always) the render target is the 
				 * instance itself.
				 *
				 */
				Surface & getWidgetRenderTarget() throw() ; 
		
						 
				/**
				 * Changes stacking inside this container surface so 
				 * that specified widget is put to front.
				 *
				 * @note Specified widget must already be registered to 
				 * this surface.
				 *
				 * @throw VideoException if the widget is not a registered 
				 * one.
				 *
				 */
				virtual void putWidgetToFront( TwoDimensional::Widget & widget )
					throw( VideoException ) ;
	

				/**
				 * Changes stacking inside this container surface so that
				 * specified widget is put to back.
				 *
				 * @note Specified widget must already be registered to 
				 * this surface.
				 *
				 * @throw VideoException if the widget is not a registered one.
				 *
				 */
				virtual void putWidgetToBack( TwoDimensional::Widget & widget ) 
				   throw( VideoException ) ;
	
	
				/**
				 * Centers in the middle of the screen the position of 
				 * the mouse cursor, by generating a mouse motion event.
				 *
				 * @note Must be called on a screen surface, with mouse 
				 * enabled of course.
				 *
				 */
				virtual void centerMousePosition() throw() ;
				
				
				/**
				 * Sets the position of the mouse cursor, by generating 
				 * a mouse motion event.
				 *
				 * @note Must be called on a screen surface, 
				 * with mouse enabled of course.
				 *
				 */
				virtual void setMousePosition( 
					Coordinate newX = 0, Coordinate newY = 0 ) throw() ;


	
				// Lockable section.
	
	
				/**
				 * Tells whether this surface has to be locked before 
				 * modifying it. 
				 *
				 * When using a surface, in general there is no need to use 
				 * this method because the lock/unlock methods take care 
				 * of that.
				 *
				 * @see Ceylan::Lockable.
				 *
				 * This method cannot be inlined since it is inherited.
				 *
				 */				
				 virtual bool mustBeLocked() const throw() ;
		
		
	
				// Measurable section.


				/**
				 * Returns the approximate size in memory, in bytes, 
				 * currently taken by this object.
				 *
				 * @see Ceylan::Measurable interface.
				 *
			 	 */	
				virtual Ceylan::System::Size getSizeInMemory() const throw() ;
			
			
				
				/**
				 * Displays specified set of data, defined by dataArray, 
				 * an array of dataCount IntegerData elements, as a 
				 * curve drawn with pencilColor, on a background
				 * whose color is backgroundColor. 
				 * The whole display will be scaled vertically to fit 
				 * into inBox. 
				 * On the contrary, the abscissa will be used as is.
				 *
				 * @note If inBox is a null pointer, then the curve will 
				 * be drawn in the whole surface.
				 * 
				 * @note When choosing the width of the container (inBox), 
				 * to avoid truncating the graph, add 2*graphAbscissaOffset 
				 * to the number of samples that describes the curve,
				 * and it will fit at best : both left and right boundaries 
				 * will be taken into account.
				 *
				 */
				virtual bool displayData( 
					const Ceylan::Maths::IntegerData * dataArray, 
					Ceylan::Uint32 dataCount,
					Pixels::ColorDefinition pencilColor, 
					Pixels::ColorDefinition captionColor,
					Pixels::ColorDefinition backgroundColor,
					const std::string & caption = "", 
					const UprightRectangle * inBox = 0 ) 
						throw() ;
					
				
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;



				// Static section.

				
				/**
				 * Surface factory : creates a new Surface instance from
				 * specified image file.
				 *
				 * @note The caller takes ownership of the created Surface, 
				 * and must deallocate it when necessary.
				 *
				 * @note Image format, if supported, is auto-detected.
				 *
				 */
				static Surface & LoadImage( const std::string & filename,
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) 
					throw( TwoDimensional::ImageException ) ;
	
	
		
				/**
				 * Outputs a textual description of the given surface flags.
				 * These flags are interpreted as surface flags only.
				 *
				 * @param flags the surface video settings to interpret.
				 *
				 * @note One shall not call this method on flags used with
				 * VideoModule::setMode.
				 * Use VideoModule::interpretFlags instead, since their 
				 * meaning is different.
				 *
				 */
				static std::string InterpretFlags( Ceylan::Flags flags ) 
					throw() ;
	
	
				/// Offset used when drawing curve in a container.
				static const Length graphAbscissaOffset ;
				
				/// Offset used when drawing curve in a container.
				static const Length graphOrdinateOffset ;

				
				/// Offset used when printing caption in a container.
				static const Length captionAbscissaOffset ;
				
				/// Offset used when printing caption in a container.
				static const Length captionOrdinateOffset ;
				
				
				
					
			protected:



				/**
				 * Effective unlocking of the surface.
				 *
				 * Nevertheless, only lock/unlock pair should be used by 
				 * user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void preUnlock() throw() ;
				
				
				/**
				 * Effective locking of the surface.
				 * Nevertheless, only lock/unlock pair should be used 
				 * by user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void postLock() throw() ;
								 
	
	
				/// Offset of a Surface.
				typedef Ceylan::Sint16 Offset ;

					
				/**
				 * Creates a completely void surface, whose display type is
				 * back-buffer. 
				 *
				 * Useful in order to have it filled with a loaded image, 
				 * with Surface::load* and blitOnly set to false.
				 *
				 * @note Use with care, the created surface is mainly
				 * uninitialized.
				 *
				 * @see Surface::loadPNG
				 *
				 */	
				explicit Surface() throw() ;
		
		
				 
				/// Returns this surface's offset.
				inline Offset getOffset() const throw() ;
				
				/// Sets this surface's offset.
				inline void setOffset( Offset offset ) throw() ;
				
								
				/// Releases internal SDL surface.
				virtual void flush() throw() ;
							
				
				/**
				 * Triggered when an abnormal inconsistency is detected 
				 * between OSDL's surface state and its back-end counterpart.
				 *
				 * @note Those kind of errors, which are detected at runtime,
				 * mean that the code misbehaved badly, therefore an 
				 * emergency stop might help. 
				 *
				 */
				virtual void inconsistencyDetected( 
					const std::string & message ) const throw() ;
				
				
				/**
				 * This internal structure also contains private fields 
				 * not used here.
				 *
				 */
				SDL_Surface * _surface ;
	
	
				/**
				 * Tells what is the display type of this surface.
				 *
				 * The display type has to be recorded since :
				 *  - screen surfaces are not managed like backbuffers, 
				 * since the formers have to update the screen
				 *  - classical screen surfaces and OpenGL ones must be
				 * distinguished, since they update the screen differently 
				 * (flip in double-buffering)
				 *
				 */
				DisplayType _displayType ;
								
	
				/**
				 * Tells whether this surface must be locked before 
				 * modifying it.
				 *
				 */
				bool _mustBeLocked ;
				
			
		
			private:
				
				
				/**
				 * Tells whether the surface would need to be redrawn.	
				 *
				 * Private to force all child classes to use a get/set method
				 * for safe overloading (no unexpected shortcut due to faulty
				 * assignment).
				 *
				 */
				bool _needsRedraw ;


									
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				Surface( const Surface & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Surface & operator = ( const Surface & source ) throw() ;
				
				
		} ;
					
	}
	
}



/// Used to serialize this surface into a stream.
OSDL_DLL std::ostream & operator << ( std::ostream & os, 
	OSDL::Video::Surface & s ) throw() ;


#endif // OSDL_SURFACE_H_

