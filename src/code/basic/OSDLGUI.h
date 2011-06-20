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


#ifndef OSDL_GUI_H_
#define OSDL_GUI_H_



#include "OSDLException.h"   // for inheritance.
#include "OSDLFont.h"        // for PointSize

#include "Ceylan.h"          // for inheritance.

#include <string>


/*
 * Module for the management of a GUI (Graphical User Interface).
 *
 * As a GUI involves rendering, events, sound, input devices, etc. it is in none
 * of these namespaces, but only in the OSDL root one.
 *
 */


#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL

// No need to include SDL header here:
struct SDL_Surface ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL



#if ! defined(OSDL_USES_AGAR) || OSDL_USES_AGAR

// No need to include Agar header here:

struct AG_Surface ;


/*
 * We would have liked to forward-declare AG_Font, but the struct is declared
 * as: 'typdef struct ag_font {...} AG_Font;' in Agar's text.h so we have to
 * mention the internal ag_font instead.
 *
 */
//struct AG_Font ;
struct ag_font ;

#endif // ! defined(OSDL_USES_AGAR) || OSDL_USES_AGAR



namespace OSDL
{


#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL

  /*
   * No way of forward declaring LowLevelSurface apparently: we would have liked
   * to specify 'struct LowLevelThing ;' here and in the implementation file
   * (.cc): 'typedef BackendThing LowLevelThing' but then the compiler finds
   * they are conflicting declarations.
   *
   */

  /// The internal actual surface.
  typedef ::SDL_Surface LowLevelSurface ;

#else // OSDL_USES_SDL

  struct LowLevelSurface {} ;

#endif // OSDL_USES_SDL


  /*
   * A GUILevelSurface is the lower-level surface that is directly managed by
   * the GUI back-end (ex: Agar).
   *
   * This allows to provide higher-level constructs and to load images from any
   * data source (ex: virtual filesystems).
   *
   */

#if ! defined(OSDL_USES_AGAR) || OSDL_USES_AGAR

  /// Agar surface being used.
  typedef ::AG_Surface GUILevelSurface ;

  /// Agar font being used.
  typedef ::ag_font GUILevelFont ;

#else // ! defined(OSDL_USES_AGAR) || OSDL_USES_AGAR

  /// GUI-level surface being used.
  struct GUILevelSurface {} ;

  /// GUI-level font being used.
  struct GUILevelFont {} ;


#endif // ! defined(OSDL_USES_AGAR) || OSDL_USES_AGAR



  // GUI module is created by common module.
  class CommonModule ;


  namespace Video
  {

	class VideoModule ;

  }


  namespace Audio
  {

	class AudioModule ;

  }


  namespace Events
  {

	class EventsModule ;

  }



  /**
   * To be raised when dealing with graphical user interface issues.
   *
   */
  class OSDL_DLL GUIException : public OSDL::Exception
  {

  public:

	explicit GUIException( const std::string & reason ) ;

	virtual ~GUIException() throw() ;

  } ;



  /**
   * Root module for all GUI needs.
   *
   * @note The GUI module is a singleton (up to one GUI running at any time).
   *
   */
  class OSDL_DLL GUIModule : public Ceylan::Module
  {


	// The common module has to create the GUI module.
	friend class OSDL::CommonModule ;



  public:


	/**
	 * Called at the end of VideoModule::setMode, so that the GUI can finish its
	 * initialization, once a proper screen is available.
	 *
	 */
	void postSetModeInit( LowLevelSurface & screen ) ;


	/**
	 * Requests the GUI to redraw itself.
	 *
	 */
	void redraw() ;


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
	virtual const std::string toString(
	  Ceylan::VerbosityLevels level = Ceylan::high ) const ;





	// Static section.



	/**
	 * Loads specified PNG image, and returns it directly as a GUI-level
	 * surface.
	 *
	 * @param pngFilename the filename corresponding to the image to load.
	 *
	 * @throw GUIException if the operation failed.
	 *
	 * @note The graphic subsystem must have been initialised first.
	 *
	 */
	static GUILevelSurface & LoadPNG( const std::string & pngFilename ) ;



	/**
	 * Loads specified JPEG image, and returns it directly as a GUI-level
	 * surface.
	 *
	 * @param jpegFilename the filename corresponding to the image to load.
	 *
	 * @throw GUIException if the operation failed.
	 *
	 * @note The graphic subsystem must have been initialised first.
	 *
	 */
	static GUILevelSurface & LoadJPEG( const std::string & jpegFilename ) ;



	/**
	 * Loads specified (Truetype) font, and returns it directly as a GUI-level
	 * font.
	 *
	 * @param fontFilename the filename corresponding to the font to load.
	 *
	 * @param pointSize the point size, in dots per inch.
	 *
	 * @throw GUIException if the operation failed.
	 *
	 * @note The graphic subsystem must have been initialised first.
	 *
	 */
	static GUILevelFont & LoadFont( const std::string & fontFilename,
	  OSDL::Video::TwoDimensional::Text::PointSize pointSize ) ;


	/**
	 * Returns a string describing the last error (if any) that occurred with
	 * the GUI backend.
	 *
	 */
	static std::string GetBackendLastError() ;



  protected:


	// Variables section.

	/// The name of the current application.
	std::string _applicationName ;


	/// Video module, needed by the GUI subsystem:
	Video::VideoModule * _video ;

	/// Audio module, needed by the GUI subsystem:
	Audio::AudioModule * _audio ;

	/// Events module, needed by the GUI subsystem:
	Events::EventsModule  * _events ;



  private:



	/**
	 * Private constructor to be sure it will not be implicitly called.
	 *
	 * @param applicationName the application name, useful for example for the
	 * caption of the main GUI window.
	 *
	 * @throw GUIException if the GUI subsystem initialization failed.
	 *
	 */
	GUIModule( const std::string & applicationName,
	  Video::VideoModule & videoModule, Audio::AudioModule & audioModule,
	  Events::EventsModule & eventsModule ) ;



	/// Basic virtual private destructor.
	virtual ~GUIModule() throw() ;




	/**
	 * Copy constructor made private to ensure that it will never be called.
	 *
	 * The compiler should complain whenever this undefined constructor is
	 * called, implicitly or not.
	 *
	 */
	explicit GUIModule( const GUIModule & source ) ;



	/**
	 * Assignment operator made private to ensure that it will never be called.
	 *
	 * The compiler should complain whenever this undefined operator is called,
	 * implicitly or not.
	 *
	 */
	GUIModule & operator = ( const GUIModule & source ) ;



  } ;


}


#endif // OSDL_GUI_H_
