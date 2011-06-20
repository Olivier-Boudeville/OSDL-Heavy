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


#ifndef OSDL_VIDEO_H_
#define OSDL_VIDEO_H_


#include "OSDLSurface.h"     // for Surface
#include "OSDLPixel.h"       // for PixelFormat
#include "OSDLVideoTypes.h"  // for Length, Coordinate, etc.
#include "OSDLOpenGL.h"      // for Flavour, etc.

#include "Ceylan.h"          // for Uint8, Flags, etc.

#include <string>
#include <list>
#include <utility>           // for std::pair





namespace OSDL
{



	// Video module is created by common module.
	class CommonModule ;


	// Video module can refer to GUI module.
	class GUIModule ;


	namespace Rendering
	{

		// Video module can make use of a video renderer.
		class VideoRenderer ;

	}



	namespace Video
	{



		// Video module manages the screen surface.
		class Surface ;


		namespace OpenGL
		{

			// The Video module may have to manage OpenGL contexts.
			class OpenGLContext ;

		}



		/// Icon masks are use for window manager's icons.
		typedef Ceylan::Uint8 IconMask ;



		/// A screen definition is defined as a ( width, height ) pair.
		typedef std::pair<Length, Length> Definition ;



		/// Useful to specify standard screen sizes for OSDL windows;
		enum StandardScreenSize
		{

			Size_FullScreen = 0,
			Size_640x480    = 1,
			Size_800x600    = 2,
			Size_1024x780   = 3,
			Size_1280x1024  = 4,
			Size_1680x1050  = 5

		} ;



		/**
		 * Root module for all video needs.
		 *
		 * Notably manages the screen surface.
		 *
		 * @note if the video subsystem has already been initialized, calling
		 * the static methods returning video informations will apply to the
		 * current video mode.
		 *
		 * If the video subsystem has not already been initialized, those static
		 * methods will return informations about the best available video mode.
		 *
		 */
		class OSDL_DLL VideoModule : public Ceylan::Module
		{



			// The common module has to create the video module.
			friend class OSDL::CommonModule ;



			public:



				/**
				 * Returns whether a screen surface is available.
				 *
				 */
				bool hasScreenSurface() const ;



				/**
				 * Returns the current screen surface, if any.
				 *
				 * The video module keeps ownership of the returned surface: the
				 * caller should not deallocate it.
				 *
				 * @throw VideoException if no screen surface is available.
				 *
				 */
				virtual Surface & getScreenSurface() const ;



				/**
				 * Assigns a new surface to the screen surface.
				 *
				 * @param newScreenSurface the new screen surface, which must
				 * already have a display type corresponding to a screen surface
				 * (i.e. not BackBuffer).
				 *
				 * @see Surface::setDisplayType
				 *
				 * @throw VideoException if the display type of the specified
				 * surface does not correspond to a screen surface.
				 *
				 */
				virtual void setScreenSurface( Surface & newScreenSurface ) ;



				/**
				 * Tells whether this video module uses a (video) renderer.
				 *
				 */
				virtual bool hasRenderer() const ;



				/**
				 * Returns the video renderer currently used.
				 *
				 * @return the current video renderer.
				 *
				 * @throw VideoException if no renderer was being used.
				 *
				 */
				virtual Rendering::VideoRenderer & getRenderer() const ;



				/**
				 * Sets a new video renderer.
				 *
				 * This renderer will be in charge of all the subsequently view
				 * graphical rendering.
				 *
				 * If a previous renderer was used, it will be deleted.
				 *
				 * @note The renderer will be owned by this Video module and
				 * deallocated as such.
				 *
				 */
				virtual void setRenderer(
					Rendering::VideoRenderer & newRenderer ) ;



				/**
				 * Tells whether this video module has an OpenGL context.
				 *
				 */
				virtual bool hasOpenGLContext() const ;



				/**
				 * Sets a new OpenGL context.
				 *
				 * If a previous OpenGL context was used, it will be deleted
				 * first.
				 *
				 * @note The OpenGL context will be owned by this Video module
				 * and deallocated as such.
				 *
				 */
				virtual void setOpenGLContext(
					OpenGL::OpenGLContext & newOpenGLContext ) ;



				/**
				 * Returns the OpenGL context currently used.
				 *
				 * @return the current OpenGL context.
				 *
				 * @throw VideoException if no OpenGL context was being used.
				 *
				 */
				virtual OpenGL::OpenGLContext & getOpenGLContext() const ;



				/**
				 * Returns what is the closest recommended bits per pixel choice
				 * for specified video mode, according to specified bits per
				 * pixel target.
				 *
				 * Parameters are the same, and have the same meaning, as
				 * setMode's ones.
				 *
				 * @return 0 if the requested mode is not supported under any
				 * bit depth, or returns the bits-per-pixel of the closest
				 * available mode with the given width, height and requested
				 * surface flags.
				 *
				 */
				 virtual BitsPerPixel getBestColorDepthForMode(
					Length width, Length height,
					BitsPerPixel askedBpp, Ceylan::Flags flags ) ;



				/**
				 * Tells whether the display is currently running, i.e.  if it
				 * has already been initialized and not been shut down at the
				 * moment.
				 *
				 */
				virtual bool isDisplayInitialized() const ;


				/**
				 * Tells whether a GUI (Graphical User Interface) is being
				 * managed by this video module.
				 *
				 */
				virtual bool isGUIEnabled() const ;



				/**
				 * Registers a GUI (Graphical User Interface) module for further
				 * use.
				 *
				 * @param guiModule the GUI module to register.
				 *
				 */
				virtual void setGUIModule( GUIModule & guiModule ) ;


				/**
				 * Requests this video module to forget any previously known GUI
				 * (Graphical User Interface) module.
				 *
				 */
				virtual void forgetGUIModule() ;


				/**
				 * Tries to sets up a video mode with the specified width,
				 * height and bits-per-pixel.
				 *
				 * Requested flags will be respected on a best effort basis,
				 * actually obtained flags will be returned.
				 *
				 * @param width the client width (not counting window manager
				 * decorations) of the application window.
				 *
				 * @param height the client height (not counting window manager
				 * decorations) of the application window.
				 *
				 * @param askedBpp the requested color depth, in bits per
				 * pixel. If UseCurrentColorDepth (null) is specified, then the
				 * color depth of the display will be used.
				 *
				 * @param flags describes the desired features, such as
				 * SoftwareSurface, Resizable, etc.
				 *
				 * One particularly useful feature is DoubleBuffered, since it
				 * allows to suppress most of the tearing on most platforms.
				 *
				 * Flags must be OR'd together (ex: VideoModule::SoftwareSurface
				 * || VideoModule::Resizable), specify 0 if not wanting to
				 * activate any of these features.
				 *
				 * @param flavour the selected OpenGL flavour, if any, and if
				 * OpenGL is being used (see VideoModule::OpenGL setMode flag).
				 * Flavours have to be selected no later than this call.
				 *
				 * @return the flags that were actually obtained
				 *
				 * @throw VideoException if the setting of the new mode failed.
				 *
				 * @see SDL counter-part, SDL_SetVideoMode (defined in
				 * http://sdldoc.csn.ul.ie/sdlsetvideomode.php)
				 *
				 * @note If OpenGL is used, then asking for double-buffering
				 * (flag DoubleBuffered) is strongly recommended since it
				 * prevents nasty visual tearing.
				 *
				 */
				virtual Ceylan::Flags setMode( Length width, Length height,
					BitsPerPixel askedBpp, Ceylan::Flags flags,
					OpenGL::Flavour flavour = OpenGL::None ) ;



				/**
				 * Tells setMode that the color depth of the current display
				 * should be used.
				 *
				 * Convenient for applications running into a window.
				 * This value is equal to zero.
				 *
				 */
				static const BitsPerPixel UseCurrentColorDepth ;



				/**
				 * Does everything needed when the application is resized,
				 * including calling setMode and updating OpenGL state if
				 * necessary.
				 *
				 * @param newWidth the new width of the application window.
				 *
				 * @param newHeight the new height of the application window.
				 *
				 */
				virtual void resize( Length newWidth, Length newHeight ) ;



				/**
				 * This method should be called whenever the screen needs to be
				 * redrawn, for example if the window manager asks for it.
				 *
				 * Depending on a renderer being used or not, the screen will be
				 * managed.
				 *
				 */
				virtual void redraw() ;



				/**
				 * Toggles the application between windowed and fullscreen mode,
				 * if supported.
				 *
				 * @note X11 is the only target currently supported, BeOS
				 * support is experimental.
				 *
				 */
				virtual void toggleFullscreen() ;



				/**
				 * Performs a screenshot of onscreen content, and saves it in a
				 * BMP file whose name is specified.
				 *
				 * @param screenshotFilename the target screenshot filename
				 *
				 * @throw VideoException if the operation failed.
				 *
				 * @note This method exists mainly for platforms lacking Surface
				 * support (ex: the Nintendo DS, whose screenshots represent
				 * only the content of the main core), even if it will work on
				 * Surface-enabled platforms (in this case it will operate on
				 * the screen surface, if any).
				 *
				 * @see Surface::savePNG, Surface::saveBMP.
				 *
				 */
				virtual void makeBMPScreenshot(
					const std::string & screenshotFilename ) ;



				/**
				 * Tells whether line endpoint (final pixel) should be drawn.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @return true if draw-endpoint mode is set, false otherwise.
				 *
				 */
				virtual bool getEndPointDrawState() const ;



				/**
				 * Sets the draw-endpoint mode.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @param newState the new draw-endpoint mode, activated if
				 * and only if true.
				 *
				 */
				virtual void setEndPointDrawState( bool newState ) ;




				/**
				 * Tells whether graphics should be anti-aliased, if possible.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @return true if anti-aliasing mode is set, false otherwise.
				 *
				 */
				virtual bool getAntiAliasingState() const ;



				/**
				 * Sets the anti-aliasing mode.
				 *
				 * @note Belongs to the state machine settings.
				 *
				 * @param newState the new anti-aliasing mode, activated if and
				 * only if true.
				 *
				 */
				virtual void setAntiAliasingState( bool newState ) ;




				/**
				 * Returns the name of the video driver being currently used
				 * (example: 'x11').
				 *
				 * @see GetDriverName
				 *
				 * @throw VideoException if the operation failed.
				 *
				 */
				virtual std::string getDriverName() const ;




				/**
				 * Sets the window title and icon name.
				 *
				 * @param newTitle the new window title.
				 *
				 * @param iconName the new icon name.
				 *
				 */
				virtual void setWindowCaption( const std::string & newTitle,
					const std::string & iconName ) ;



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
					std::string & iconName ) ;




				/**
				 * Sets the icon for the display window.
				 *
				 * @param filename the name of the file containing the icon.
				 * Its format (ex: PNG) should be auto-detected.
				 *
				 * @note Win32 icons must be 32x32.
				 *
				 * @note This function must be called before the first call to
				 * setMode.
				 *
				 * @note The specified surface will have to be deallocated on
				 * exit by the user code.
				 *
				 * @see LoadImage
				 *
				 */
				virtual void setWindowIcon( const std::string & filename ) ;



				/**
				 * Iconifies (minimizes) the window.
				 *
				 * @note If the application is running in a window managed
				 * environment, this method attempts to iconify (minimise) it.
				 * If this call is successful, the application will receive an
				 * ApplicationFocus loss event.
				 *
				 * @return true on success, false if iconification is not
				 * supported or was refused by the window manager.
				 *
				 */
				virtual bool iconifyWindow() ;



				/**
				 * Returns the current average refresh rate (in frames per
				 * second) of the screen surface, since its creation.
				 *
				 * @throw VideoException if the operation failed, including if
				 * no screen surface is actually available.
				 *
				 */
				virtual Ceylan::Float32 getAverageRefreshRate() ;



				/**
				 * Tells whether OpenGL is being used currently by this video
				 * module.
				 *
				 */
				virtual bool isUsingOpenGL() const ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;





				// Static section.



				/**
				 * Tells whether display has already been initialized
				 * (VideoModule::setMode called).
				 *
				 * @note This method is static so that calling it is convenient:
				 * no need to explicitly retrieve the common module, then the
				 * video module, before knowing the result.
				 *
				 * The need to retrieve the right module from scratch at each
				 * call is rather inefficient though.
				 *
				 */
				static bool IsDisplayInitialized() ;



				/**
				 * Tells whether the last pixel of a segment of line should be
				 * drawn.
				 *
				 * @note This method is static so that calling it is convenient:
				 * no need to explicitly retrieve the common module, then the
				 * video module, before knowing the result.
				 *
				 * The need to retrieve the right module from scratch at each
				 * call is rather inefficient though.
				 *
				 */
				static bool GetEndPointDrawState() ;



				/**
				 * Tells whether antialiasing is wanted.
				 *
				 * @note This method is static so that calling it is convenient:
				 * no need to explicitly retrieve the common module, then the
				 * video module, before knowing the result.
				 *
				 * The need to retrieve the right module from scratch at each
				 * call is rather inefficient though.
				 *
				 */
				static bool GetAntiAliasingState() ;



				/**
				 * Returns the name of the video driver being currently used
				 * (example: 'x11').
				 *
				 * @see getDriverName.
				 *
				 * @throw VideoException if the operation failed.
				 *
				 */
				static std::string GetDriverName() ;



				/**
				 * Describes specified video flags according to the semantics
				 * used in setMode.
				 *
				 * @note One shall not call this method on flags used with a
				 * surface.
				 *
				 * Use Surface::interpretFlags instead, since their meaning
				 * is different.
				 *
				 * @see setMode.
				 *
				 */
				static std::string InterpretFlags( Ceylan::Flags flags ) ;



				/**
				 * Returns whether hardware surfaces can be created.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool HardwareSurfacesCanBeCreated() ;



				/**
				 * Returns whether there is a window manager available.
				 *
				 * @throw VideoException if the information could not
				 * be obtained.
				 *
				 */
				static bool WindowManagerAvailable() ;




				// Hardware to hardware section.



				/**
				 * Returns whether hardware to hardware blits are accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool HardwareToHardwareBlitsAccelerated() ;



				/**
				 * Returns whether hardware to hardware colorkey blits are
				 * accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool HardwareToHardwareColorkeyBlitsAccelerated() ;



				/**
				 * Returns whether hardware to hardware alpha blits are
				 * accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool HardwareToHardwareAlphaBlitsAccelerated() ;




				// Software to hardware section.



				/**
				 * Returns whether software to hardware blits are accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool SoftwareToHardwareBlitsAccelerated() ;



				/**
				 * Returns whether software to hardware colorkey blits are
				 * accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool SoftwareToHardwareColorkeyBlitsAccelerated() ;



				/**
				 * Returns whether software to hardware alpha blits are
				 * accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool SoftwareToHardwareAlphaBlitsAccelerated() ;



				/**
				 * Returns whether color fills are accelerated.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool ColorFillsAccelerated() ;



				/**
				 * Returns the total amount of video memory, in kilobytes.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static Ceylan::Uint32 GetVideoMemorySize() ;



				/**
				 * Returns the native format of the video device.
				 *
				 * @return If the graphics system has already been initialized
				 * (using setMode), returns the pixel format of the current
				 * video mode, otherwise returns the pixel format of the "best"
				 * available video mode.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static Pixels::PixelFormat GetVideoDevicePixelFormat() ;



				/**
				 * Returns a summary of video chain capabilities.
				 *
				 * @throw VideoException if video informations could not be
				 * retrieved.
				 *
				 */
				static std::string DescribeVideoCapabilities() ;



				/**
				 * Tells whether definitions are restricted for the specified
				 * flags and pixel format.
				 *
				 * @return false if all definitions are possible for the given
				 * pixel format, true if dimensions are restricted, and in this
				 * case in the specified list (<b>definitions</b>) available
				 * dimension pairs are stored, if any.
				 *
				 * @param definitions an empty list in which dimension pairs
				 * will be stored, if not all dimensions are allowed for the
				 * given pixel format.
				 *
				 * The list may remain empty, if nothing is available for
				 * specified choice.
				 *
				 * @param flags describes the desired screen surface, with the
				 * same meaning as the setMode flags.
				 *
				 * @param format: desired pixel format. If format is null (0),
				 * the definition list will correspond to the "best" mode
				 * available.
				 *
				 * @throw VideoException if the operation failed or is not
				 * supported.
				 *
				 */
				static bool AreDefinitionsRestricted(
					std::list<Definition> & definitions,
					Ceylan::Flags flags,
					Pixels::PixelFormat * pixelFormat = 0 ) ;



				/**
				 * Returns a summary of available screen definitions for
				 * specified flags and pixel format.
				 *
				 * @param flags describes a target screen surface, with the same
				 * meaning as the setMode flags.
				 *
				 * @param format: desired pixel format.
				 *
				 * If format is null (0), the definition list will correspond to
				 * the "best" mode available.
				 *
				 * @throw VideoException if the operation failed.
				 *
				 */
				static std::string DescribeAvailableDefinitions(
					Ceylan::Flags flags,
					Pixels::PixelFormat * pixelFormat = 0 ) ;




				/**
				 * Returns the standard screen size corresponding to the
				 * specified text.
				 *
				 * @param standardScreenSizeInText the textual description of
				 * the standard screen size
				 *
				 * @throw VideoException if the operation failed, i.e. if the
				 * specified text could not be converted into a known screen
				 * size.
				 *
				 */
				static StandardScreenSize ToStandardScreenSize(
					const std::string & standardScreenSizeInText ) ;


				/**
				 * Returns (in specified variables) the screen dimensions
				 * corresponding to the specified physical standard screen size.
				 *
				 * @param screenSize the screen size whose dimensions are to be
				 * determined (ex: Size_640x480).
				 *
				 * @param screenWidth the variable that will be set to the width
				 * corresponding to the specified size
				 *
				 * @param screenHeight the variable that will be set to the
				 * height corresponding to the specified size
				 *
				 * @throw VideoException if the operation failed, i.e. if the
				 * specified screen size is not known, or if it corresponds to a
				 * logical size (ex: Size_FullScreen) rather instead of a
				 * physical one (ex: Size_640x480).
				 *
				 */
				static void ToScreenDimensions( StandardScreenSize screenSize,
					Length & screenWidth, Length & screenHeight ) ;



				/**
				 * Returns a summary about the possible use of video-related
				 * environment variables, for the selected back-end, expressed
				 * in specified format.
				 *
				 * @note The SDL back-end can be partly driven by a set of
				 * environment variables.
				 *
				 */
				static std::string DescribeEnvironmentVariables() ;



				/**
				 * Returns whether OpenGL is being used, at least by one video
				 * module.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool IsUsingOpenGL() ;


				/**
				 * Returns whether drawing primitives can be rendered.
				 *
				 * @note This basically boils down to returning whether SDL_gfx
				 * is available. This is useful for example from a test which
				 * should not fail if the use of SDL_gfx was disabled.
				 *
				 * @throw VideoException if the information could not be
				 * obtained.
				 *
				 */
				static bool IsUsingDrawingPrimitives() ;




				/*
				 * These flags can apply to Surfaces <b>created by setMode</b>,
				 * i.e. all the Display Surfaces, and only them.
				 *
				 * The legal flags for setMode form a subset of the Surface
				 * flags, with one flag being added, NoFrame.
				 *
				 * @note These flags are not to be mixed up with the Surface
				 * flags' ones.
				 *
				 * @see OSDL::Video::Surface
				 *
				 */



				/**
				 * Indicates that the screen surface is to be stored in
				 * system memory.
				 *
				 */
				static const Ceylan::Flags SoftwareSurface ;



				/**
				 * Indicates that the screen surface is to be stored in video
				 * memory.
				 *
				 */
				static const Ceylan::Flags HardwareSurface ;



				/**
				 * Enables the use of asynchronous updates of the display
				 * surface.
				 *
				 */
				static const Ceylan::Flags AsynchronousBlit ;



				/// Indicates that the screen surface may use any pixel format.
				static const Ceylan::Flags AnyPixelFormat ;



				/// Indicates that a surface should have an exclusive palette.
				static const Ceylan::Flags ExclusivePalette ;



				/**
				 * Indicates that the screen surface is to be double buffered.
				 *
				 * Works also if OpenGL is used.
				 *
				 * @note The vast majority of applications should request that
				 * feature.
				 *
				 */
				static const Ceylan::Flags DoubleBuffered ;



				/**
				 * Indicates that the screen surface is to be full screen, not
				 * windowed.
				 *
				 */
				static const Ceylan::Flags FullScreen ;



				/**
				 * Indicates that the screen surface should have an OpenGL
				 * context.
				 *
				 */
				static const Ceylan::Flags OpenGL ;



				/**
				 * Indicates that the screen surface is to be resizable.
				 *
				 */
				static const Ceylan::Flags Resizable ;



				/**
				 * Indicates that a window which would correspond to a screen
				 * surface should have no title bar nor frame decoration, if
				 * possible.
				 *
				 */
				static const Ceylan::Flags NoFrame ;





			protected:




				// Variables section.



				/// Internal surface corresponding to the screen.
				Surface * _screen ;



				/**
				 * Keeps tracks of setMode calls, be OpenGL used or not.
				 *
				 */
				bool _displayInitialized ;



				/**
				 * Records any graphical user interface module being used.
				 *
				 * @note This GUI module (if any) is not owned by the video
				 * module.
				 *
				 */
				OSDL::GUIModule * _gui ;



				/// The video renderer being used, if any.
				Rendering::VideoRenderer * _renderer ;



				/**
				 * Caches the current state relative to the current use of
				 * OpenGL.
				 *
				 * @note This cached value can be used to avoid overhead in case
				 * of very frequent reads with only one video module
				 * instanciated (the general case).
				 *
				 */
				static bool _IsUsingOpenGL ;



				/**
				 * Caches the current state relative to final pixel drawing for
				 * lines.
				 *
				 * @note This cached value can be used to avoid overhead in case
				 * of very frequent reads with only one video module
				 * instanciated (the general case).
				 *
				 */
				static bool _DrawEndPoint ;



				/**
				 * Caches the current state relative to antialiasing.
				 *
				 * @note This cached value can be used to avoid overhead in case
				 * of very frequent reads with only one video module
				 * instanciated (the general case).
				 *
				 */
				static bool _AntiAliasing ;



				/// The maximum length for the name of the display driver.
				static const Ceylan::Uint16 DriverNameMaximumLength ;



			private:



				/**
				 * The current OpenGL context, if OpenGL is used.
				 *
				 * @note Private to ensure cache consistency is enforced.
				 *
				 */
				OpenGL::OpenGLContext * _openGLcontext ;



				/**
				 * Records the current state relative to final pixel drawing for
				 * lines.
				 *
				 * @note Default value: false.
				 *
				 * @note Private to ensure cache consistency is enforced.
				 *
				 */
				bool _drawEndPoint ;



				/**
				 * Records the current antialiasing state.
				 *
				 * @note Default value: true.
				 *
				 * @note Private to ensure cache consistency is enforced.
				 *
				 */
				bool _antiAliasing ;



				/*
				 * Not specified directly in the screen surface, as otherwise
				 * the creation of any surface would take useless time, to
				 * retrieve the current time:
				 *
				 */


				/**
				 * Stores the second at which the screen surface (if any) was
				 * created, to compute its average refresh rate (frames per
				 * second).
				 *
				 */
				Ceylan::System::Second _screenStartingSecond ;


				/**
				 * Stores the microsecond at which the screen surface (if any)
				 * was created, to compute its average refresh rate (frames per
				 * second).
				 *
				 */
				Ceylan::System::Microsecond _screenStartingMicrosecond ;




				/**
				 * Private constructor to be sure it will not be implicitly
				 * called.
				 *
				 * @throw VideoException if the video subsystem initialization
				 * failed.
				 *
				 */
				VideoModule() ;



				/// Basic virtual private destructor.
				virtual ~VideoModule() throw() ;




				/**
				 * Copy constructor made private to ensure that it will never be
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit VideoModule( const VideoModule & source ) ;



				/**
				 * Assignment operator made private to ensure that it will never
				 * be called.
				 *
				 * The compiler should complain whenever this undefined operator
				 * is called, implicitly or not.
				 *
				 */
				VideoModule & operator = ( const VideoModule & source ) ;



				/**
				 * Array of all known environment variables related to video,
				 * for SDL back-end.
				 *
				 */
				static const std::string _SDLEnvironmentVariables[] ;



		} ;


	}

}



#endif // OSDL_VIDEO_H_
