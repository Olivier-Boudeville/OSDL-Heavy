#ifndef OSDL_VIDEO_H_
#define OSDL_VIDEO_H_


#include "OSDLSurface.h"     // for Surface
#include "OSDLPixel.h"       // for PixelFormat
#include "OSDLVideoTypes.h"  // for Length, Coordinate, etc.
#include "OSDLTypes.h"       // for Flags
#include "OSDLOpenGL.h"      // for Flavour, etc.

#include "Ceylan.h"          // for Uint8, etc.

#include <string>
#include <list>
#include <utility>           // for std::pair




namespace OSDL
{


	// Video module is created by common module.
	class CommonModule ;
	
		
	namespace Rendering 
	{
		// Video module can make use of a video renderer.
		class VideoRenderer ;
	}
	
	
	namespace Video 
	{
	
	
		// Video module manages the screen surface.			
		class Surface ;
			
		
		/// Icon masks are use for window manager's icons.
		typedef Ceylan::Uint8 IconMask ;

				
		/// A screen definition is defined as a ( width, height ) pair.
		typedef std::pair<Length, Length> Definition ;


								
		/**
		 * Root module for all video needs.
		 *
		 * Notably manages the screen surface.
		 *
		 * @note if the video subsystem has already been initialized, calling the static methods
		 * returning video informations will apply to the current video mode. 
		 * If the video subsystem has not already been initialized, those static methods will 
		 * return informations about the best available video mode. 
		 *
		 */
		class VideoModule : public Ceylan::Module
		{
		
		
			public:
										
										
						
				/**
				 * Assigns a new surface to the screen surface.								
				 *
				 * @param newScreenSurface the new screen surface, which must already have a 
				 * display type corresponding to a screen surface (i.e. not BackBuffer).
				 *
				 * @see Surface::setDisplayType
				 *
				 * @throw VideoException if the display type of the specified surface does not 
				 * correspond to a screen surface.
				 *
				 */
				virtual void setScreenSurface( Surface & newScreenSurface ) 
					throw( VideoException ) ;
					
					
				/**
				 * Returns the current screen surface.
				 *
				 * The video module keeps ownership of the returned surface : the caller should
				 * not deallocate it.
				 *
				 */
				virtual Surface & getScreenSurface() const throw ( VideoException ) ;		
					
									
				
				/**
				 * Tells whether this video module uses a (video) renderer.
				 *
				 */
				virtual bool hasRenderer() const throw() ;
				
				
				/**
				 * Sets a new video renderer.
				 *
				 * This renderer will be in charge of all the subsequently view graphical 
				 * rendering. 
				 * If a previous renderer was used, it will be deleted. 
				 *
				 * @note The renderer will be owned by this Video module and deallocated as
				 * such.
				 *
				 */		
				virtual void setRenderer( Rendering::VideoRenderer & newRenderer ) throw() ;
				
				
				/**
				 * Returns the video renderer currently used.
				 *
				 * @return the current video renderer.
				 *
				 * @throw VideoException if no renderer was being used.
				 *
				 */
				virtual Rendering::VideoRenderer & getRenderer() const throw( VideoException ) ;	
				
								
								
				 				  
				/**
				 * Tells whether this video module has an OpenGL context.
				 *
				 *
				 */
				virtual bool hasOpenGLContext() const throw() ;
				
				
				/**
				 * Sets a new OpenGL context.
				 *
				 * If a previous OpenGL context was used, it will be deleted. 
				 *
				 * @note The OpenGL context will be owned by this Video module and deallocated as
				 * such.
				 *
				 */		
				virtual void setOpenGLContext( OpenGL::OpenGLContext & newOpenGLContext ) throw() ;
				
				
				/**
				 * Returns the OpenGL context currently used.
				 *
				 * @return the current OpenGL context.
				 *
				 * @throw VideoException if no OpenGL context was being used.
				 *
				 */
				virtual OpenGL::OpenGLContext & getOpenGLContext() const throw( VideoException ) ;	
				
				 				  
				 
				 
				/**
				 * Returns what is the closest recommended bits per pixel choice for specified
				 * video mode, according to specified bits per pixel target.
				 *
				 * @param They are the same, and have the same meaning, as setMode's ones. 
				 *
				 * @return 0 if the requested mode is not supported under any bit depth, or 
				 * returns the bits-per-pixel of the closest available mode with the given width,
				 * height and requested surface flags 
				 *
				 */
				 virtual BitsPerPixel getBestColorDepthForMode( Length width, Length height, 
					BitsPerPixel askedBpp, Flags flags ) throw() ;
				
				
				/**
				 * Tells whether the display is currently running, i.e. if it has already been
				 * initialized and not been shut at the moment.
				 *
				 */		
				virtual bool isDisplayInitialized() const throw() ;
					
						
				/**
				 * Tries to sets up a video mode with the specified width, height 
				 * and bits-per-pixel.
				 *
				 * Requested flags will be respected on a best effort basis, actually obtained
				 * flags will be returned.
				 *
				 * @param width the client width (not counting window manager decorations) of the
				 * application window.
				 *
				 * @param height the client height (not counting window manager decorations) of the
				 * application window.
				 *
				 * @param askedBpp the requested color depth, in bits per pixel. If a null value
				 * (UseCurrentDepth) is specified, then the color depth of the display will be used.
				 *
				 * @param flags describes the desired features, such as SoftwareSurface, Resizable,
				 * etc. One particularly useful feature is DoubleBuffered, since it allows to
				 * suppress most of the tearing.
				 *
				 * @param flavour the selected OpenGL flavour, if any, and if OpenGL is being used
				 * (see VideoModule::OpenGL setMode flag). Flavours have to be selected no later
				 * than this call.
				 *
				 * @return the flags that were actually obtained
				 *
				 * @throw VideoException if the new setting of the new mode failed.
				 *
				 * @see SDL counter-part, SDL_SetVideoMode
				 * (defined in http://sdldoc.csn.ul.ie/sdlsetvideomode.php)
				 *
				 * @note If OpenGL is used, then asking for double-buffering (flag DoubleBuffered)
				 * is strongly recommended since it prevents nasty visual tearing.
				 *
				 */			
				virtual Flags setMode( Length width, Length height, 
						BitsPerPixel askedBpp, Flags flags, 
						OpenGL::Flavour flavour = OpenGL::None ) 
					throw( VideoException ) ;


				/**
				 * Tells setMode that the color depth of the current display should be used.
				 * Convenient for applications running into a window.
				 * This value is equal to zero.
				 *
				 */
				static const BitsPerPixel UseCurrentColorDepth ;
				
				
				/**
				 * Does everything needed when the application is resized, including calling 
				 * setMode and updating OpenGL state if necessary.
				 *
				 * @param newWidth the new width of the application window.
				 *
				 * @param newHeight the new height of the application window.
				 *
				 */
				 virtual void resize( Length newWidth, Length newHeight ) throw( VideoException ) ;

				
				/**
				 * This method should be called whenever the screen needs to be redrawn, for 
				 * example if the window manager asks for it.
				 *
				 * Depending on a renderer being used or not, the screen will be managed.
				 *
				 */
				virtual void redraw() throw( VideoException ) ;
				
				
				/**
				 * Toggles the application between windowed and fullscreen mode, if supported.
				 *
				 * @note X11 is the only target currently supported, BeOS support is experimental.
				 *
				 */
				virtual void toggleFullscreen() throw( VideoException ) ;


				
				/**
				 * Tells whether line endpoint (final pixel) should be drawn.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @return true if draw-endpoint mode is set, false otherwise.
				 *
				 */
				virtual bool getEndPointDrawState() const throw() ;
				
				
				/**
				 * Sets the draw-endpoint mode.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @param newState the new draw-endpoint mode, activated if and only if true. 
				 *
				 */
				virtual void setEndPointDrawState( bool newState ) throw() ;
				

				

				/**
				 * Tells whether graphics should be anti-aliased, if possible.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @return true if anti-aliasing mode is set, false otherwise.
				 *
				 */
				virtual bool getAntiAliasingState() const throw() ;
				
				
				/**
				 * Sets the anti-aliasing mode.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @param newState the new anti-aliasing mode, activated if and only if true. 
				 *
				 */
				virtual void setAntiAliasingState( bool newState ) throw() ;



				/**
				 * Returns the name of the video driver being currently used (example : x11).
				 *
				 */
				virtual const std::string getDriverName() const throw() ; 
		
		
					
				/**
				 * Sets the window title and icon name.
				 *
				 * @param newTitle the new window title.
				 *
				 * @param iconName the new icon name.
				 *
				 */	
				virtual void setWindowCaption( const std::string & newTitle,
					const std::string & iconName ) throw() ;
	
					
				/**
				 * Gets the window title and icon name.
				 *
				 * @param title the current window title.
				 *
				 * @param iconName the icon name
				 *
				 * @note Uses the specified strings to return title and icon.
				 *
				 */					
				virtual void getWindowCaption( std::string & title, 
					std::string & iconName ) throw() ;	
				
				
				
				/**
				 * Sets the icon for the display window. 
				 *
				 * @param filename the name of the file containing the icon. Its format should be
				 * auto-detected.
				 *
				 * The mask is a bitmask that describes the shape of the icon. 
				 * - if mask is null (0), then the shape is determined by the colorkey of icon,
				 * if any, or makes the icon rectangular (no transparency) otherwise.
				 * - if mask is non-null (non 0), it has to point to a bitmap with bits set where
				 * the corresponding pixel should be visible. The format of the bitmap is as
				 * follows. Scanlines come in the usual top-down order. Each scanline consists 
				 * of (width / 8) bytes, rounded up. The most significant bit of each byte
				 * represents the leftmost pixel.
				 *
				 * @note Win32 icons must be 32x32.
				 *
				 * @note This function must be called before the first call to setMode.
				 *
				 * @note The specified surface will have to be deallocated on exit by the user code.
				 *
				 * @see LoadImage
				 *
				 */
				virtual void setWindowIcon( const std::string & filename ) throw( VideoException ) ;
				
				
				/**
				 * Iconifies (minimizes) the window.
				 *
				 * @note If the application is running in a window managed environment, this 
				 * method attempts to iconify (minimise) it. If this call is successful,
				 * the application will receive an ApplicationFocus loss event.
				 *
				 * @return true on success, false if iconification is not supported or was 
				 * refused by the window manager.
				 *
				 */
				virtual bool iconifyWindow() throw() ;
	
	
							
				/**
				 * Tells whether frames shoud be monitored to know the frame-per-second (FPS) 
				 * indicator.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @return true if frame accounting mode is set, false otherwise.
				 *
				 */
				virtual bool getFrameAccountingState() throw() ;
				
				
				/**
				 * Sets the frame-accounting mode.
				 *
				 * If activated, the frame rate will be computed.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @param newState the new frame-counting mode, activated if and only if true. 
				 *
				 */
				virtual void setFrameAccountingState( bool newState ) throw() ;				

								
								
	            /**
	             * Returns an user-friendly description of the state of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;

				
				
				
				// Static section.
								
							
				/**
				 * Tells whether display has already been initialized (VideoModule::setMode called).
				 *
				 * @note This method is static so that calling it is convenient : no need to 
				 * explicitly retrieve the common module, then video module before knowing the
				 * result. 
				 *
				 * The need to retrieve the right module from scratch at each call is 
				 * rather inefficient though.
				 *
				 */
				static bool IsDisplayInitialized() throw() ;
				
				
				/**
				 * Tells whether the last pixel of a segment of line should be drawn.
				 *
				 * @note This method is static so that calling it is convenient : no need to 
				 * explicitly retrieve the common module, then video module before knowing the
				 * result.
				 *
				 * The need to retrieve the right module from scratch at each call is 
				 * rather inefficient though.
				 *
				 */
				static bool GetEndPointDrawState() throw() ;
				
				
				/**
				 * Tells whether antialiasing is wanted.
				 *
				 * @note This method is static so that calling it is convenient : no need to 
				 * explicitly retrieve the common module, then video module before knowing the
				 * result.
				 *
				 * The need to retrieve the right module from scratch at each call is 
				 * rather inefficient though.
				 *
				 */
				static bool GetAntiAliasingState() throw() ;
				



				/**
				 * Returns the name of the video driver being currently used (example : x11).
				 *
				 */
				static const std::string GetDriverName() throw() ;
				
				
				/**
				 * Describes specified video flags according to the semantics used in 
				 * setMode, and outputs them in specified format.
				 *
				 * @note One shall not call this method on flags used with a surface.
				 * Use Surface::interpretFlags instead, since their meaning is different.
				 * 
				 * @see setMode
				 *
				 */	
				static std::string InterpretFlags( Flags flags ) throw() ;
						
				
						
				/**
				 * Returns whether hardware surfaces can be created.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool HardwareSurfacesCanBeCreated() throw( VideoException ) ;				
				

				/**
				 *  Returns whether there is a window manager available.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool WindowManagerAvailable() throw( VideoException ) ;
				
				
				/**
				 * Returns whether hardware to hardware blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool HardwareToHardwareBlitsAccelerated() throw( VideoException ) ;
				

				/**
				 * Returns whether hardware to hardware colorkey blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool HardwareToHardwareColorkeyBlitsAccelerated() throw( VideoException ) ;	
				

				/**
				 * Returns whether hardware to hardware alpha blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool HardwareToHardwareAlphaBlitsAccelerated() throw( VideoException ) ;
				
				
				/**
				 * Returns whether software to hardware blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool SoftwareToHardwareBlitsAccelerated() throw( VideoException ) ;
				

				/**
				 * Returns whether hardware to hardware colorkey blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool SoftwareToHardwareColorkeyBlitsAccelerated() throw( VideoException ) ;	
				

				/**
				 * Returns whether hardware to hardware alpha blits are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool SoftwareToHardwareAlphaBlitsAccelerated() throw( VideoException ) ;
					
					
				/**
				 * Returns whether color fills are accelerated.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static bool ColorFillsAccelerated() throw( VideoException ) ;
					

				/**
				 * Returns the total amount of video memory, in kilobytes.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static unsigned int GetVideoMemorySize() throw( VideoException ) ;	
					
					
				/**
				 * Returns the native format of the video device.
				 * 
				 * @return If the graphics system has already been initialized (using setMode),
				 * returns the pixel format of the current video mode, otherwise returns the 
				 * pixel format of the "best" available video mode.
				 *
				 * @throw VideoException if the information could not be obtained.
				 *
				 */
				static Pixels::PixelFormat GetVideoDevicePixelFormat() throw( VideoException ) ;
				
				
				/**
				 * Returns a summary of video chain capabilities.
				 *
				 * @throw VideoException if video informations could not be retrieved.
				 *
				 */
				static std::string DescribeVideoCapabilities() throw( VideoException ) ;


				/**
				 * Tells whether definitions are restricted for the specified flags and pixel
				 * format.
				 *
				 * @return false if all definitions are possible for the given pixel
				 * format, true if dimensions are restricted, and in the provided list
				 * <b>definitions</b>, available dimension pairs are stored, if any.
				 *
				 * @param definitions an empty list in which dimension pairs will be stored,
				 * if not all dimensions are allowed for the given pixel format. The list may
				 * remain empty, if nothing is available for specified choice.
				 *
				 * @param flags describes the desired screen surface, with the same meaning as
				 * the setMode flags.
				 *
				 * @param format : desired pixel format. If format is null (0), the definition list
				 * will correspond to the "best" mode available.
				 *
				 *
				 */
				static bool AreDefinitionsRestricted( std::list<Definition> & definitions,
					Flags flags, Pixels::PixelFormat * pixelFormat = 0 ) throw() ;		
			
					
				/**
				 * Returns a summary of available screen definitions for specified flags and 
				 * pixel format.
				 *
				 * @param flags describes a target screen surface, with the same meaning as
				 * the setMode flags.
				 *
				 * @param format : desired pixel format. If format is null (0), the definition list
				 * will correspond to the "best" mode available.
				 *
				 */
				static std::string DescribeAvailableDefinitions( Flags flags, 
					Pixels::PixelFormat * pixelFormat = 0 ) throw() ;
					
					
				/**
				 * Returns a summary about the possible use of video-related environment
				 * variables, for the selected back-end, expressed in specified format.
				 *
				 * @note The SDL back-end can be partly driven by a set of environment variables.
				 *
				 */	
				static std::string DescribeEnvironmentVariables() throw() ;	
					
				 
				/*
				 * These flags can apply to Surfaces <b>created by setMode</b>, i.e. all 
				 * the display Surfaces.
				 * 
				 * The legal flags for setMode form a subset of the Surface flags, with one
				 * flag being added, NoFrame. 
				 *
				 * @note These flags are not to be mixed up with the Surface flags' ones.
				 *
				 * @see OSDL::Video::Surface
				 *
				 */

				/// Indicates that the screen surface is to be stored in system memory.
				static const Flags SoftwareSurface ;			   

				/// Indicates that the screen surface is to be stored in video memory.
				static const Flags HardwareSurface ;			   
				
				/// Enables the use of asynchronous updates of the display surface.
				static const Flags AsynchronousBlit ;
									
				/// Indicates that the screen surface may use any pixel format.
				static const Flags AnyPixelFormat ;

				/// Indicates that a surface should have an exclusive palette.
				static const Flags ExclusivePalette ;	
				
				/**
				 * Indicates that the screen surface is to be double buffered.				
				 * Works also if OpenGL is used.
				 *
				 */
				static const Flags DoubleBuffered ;
				
				/// Indicates that the screen surface is to be full screen, not windowed.		
				static const Flags FullScreen ;
				
				/// Indicates that the screen surface should have an OpenGL context.		
				static const Flags OpenGL ;
				
				/// Indicates that the screen surface is to be resizable.		
				static const Flags Resizable ;


				/**
				 * Indicates that a windows which would correspond to a screen surface should
				 * have no title bar nor frame decoration, if possible.
				 *
				 */		
				static const Flags NoFrame ;
				
						
				
			
						
			protected:
	
	
				/** 
				 * Tells whether OpenGL is being used.
				 *
				 */
				//virtual bool useOpenGL() const throw() ;
				
				
				// Variables section.
				
				
				/// Internal surface corresponding to the screen.
				Surface * _screen ;				
		
		
				/**
				 * Keeps tracks of setMode calls, be OpenGL used or not.
				 *
				 */
				bool _displayInitialized ;


				/// The video renderer being used, if any.
				Rendering::VideoRenderer * _renderer ;
							
								
				/// The current OpenGL context, if OpenGL is used.
				OpenGL::OpenGLContext * _openGLcontext ;
				

				/**
				 * Records the current state relative to final pixel drawing for lines.
				 *
				 * @note Default value : false.
				 *
				 */
				bool _drawEndPoint ;

				
				/**
				 * Records the current antialiasing state.
				 *
				 * @note Default value : true.
				 *
				 */
				bool _antiAliasing ;


				/**
				 * Records the current frame-accounting state, which tells if frame rate is to be 
				 * monitored.
				 *
				 * @note Default value : true.
				 *
				 */
				bool _frameAccountingState ;
				
				
				/// The maximum length for the name of the display driver.
				static const unsigned int DriverNameMaximumLength ;

				
				// @fixme Frame rate display should be put elsewhere.

				
				
				/// Delay in milliseconds between two displays of the frame rate.
				static const unsigned int DelayBetweenFrameRateDisplay ;
				

				/// Top-left corner of the frame rate counter.
				// @fixme : be widget static Point2D * FrameRateCounterOrigin ;
		
				
				/**
				 * User-defined color of the frame rate counter display.
				 *
				 *
				 */
				//static PixelDefinition FrameRateCounterSpecifiedColor ;
				
				
				/**
				 * Precomputed color of the frame rate counter display.
				 *
				 * @note This is a color already mapped to relevant display format.  
				 * It should be recomputed if the screen format changes.
				 *
				 */
				//static PixelColor FrameRateCounterActualColor ;
				
				
				
			private:
	
	
				/**
				 * Private constructor to be sure it won't be implicitly called.
				 *
				 * @throw VideoException if the video subsystem initialization failed.
				 *
				 */
				VideoModule() throw( VideoException ) ;
				
				/// Basic virtual private destructor.
				virtual ~VideoModule() throw() ;
		

				/**
				 * Copy constructor made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined constructor is called, 
				 * implicitly or not.
				 * 
				 *
				 */			 
				explicit VideoModule( const VideoModule & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined operator is called, 
				 * implicitly or not.
				 * 
				 */			 
				VideoModule & operator = ( const VideoModule & source ) throw() ;
		
		
				/// Array of all known environment variables related to video, for SDL back-end.
				static std::string _SDLEnvironmentVariables[] ;
								
				friend class CommonModule ;
			
			
		} ;
		
	}	
	
}	




#endif // OSDL_VIDEO_H_
