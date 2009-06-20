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


#ifndef OSDL_IMAGE_H_
#define OSDL_IMAGE_H_


#include "OSDLPixel.h"           // for Pixels::ColorElement
#include "OSDLVideoTypes.h"      // for VideoException

#include "Ceylan.h"


#include <string>



// To protect LoadIcon:
#include "OSDLIncludeCorrecter.h"




namespace OSDL
{


	namespace Video
	{



		// Images are loaded into surfaces.
		class Surface ;



		namespace TwoDimensional
		{
			
			
			
			/// Mother class for all image exceptions. 		
			class OSDL_DLL ImageException : public OSDL::Video::VideoException 
			{ 
				public: 
				
					ImageException( const std::string & reason ) ; 
					
					virtual ~ImageException() throw() ; 
					
			} ;
				
				
				
			// Forward-declaration for next counted pointer:
			class Image ;
		
		
			/// Image counted pointer.
			typedef Ceylan::CountedPointer<Image> ImageCountedPtr ;
			
			
			
				
			/**
			 * Provides an encapsulation for any picture instance that can 
			 * be loaded as resource.
			 *
			 * The Image interface provides also a set of related functions 
			 * (static methods), including factories. 
			 * 
			 * @see Surface::LoadImage
			 *
			 * Supported images formats are:
			 *	- JPG (recommended)
			 *	- PNG (recommended)
			 * 	- BMP
			 *	- GIF
			 *	- LBM
			 *	- PCX
			 *	- PNM
			 *	- TGA
			 *	- XPM
			 *
			 * Our belief is however that the fewer formats you use, the 
			 * simpler it will be for everyone. 
			 *
			 * We therefore recommend using two of them, which moreover 
			 * are patent-less standards and should cover most if not all 
			 * needs:
			 * - JPG: to compress, with little loss, full-blown images
			 * such as photographies, with high color depth
			 * - PNG: to compress without loss computer-generated 
			 * images, with or without alpha channel
			 *
			 */
			class OSDL_DLL Image : public Ceylan::TextDisplayable,
				public Ceylan::LoadableWithContent<Surface>
			{
				
				
				
				public:
					
					
					
					/**
					 * This Image constructor uses given filename to load 
					 * the picture from the corresponding file.
					 *
					 * Actual picture format is guessed, based on the file
					 * content (and not on the filename).
					 *
					 * @param imageFilename the name of the file containing
					 * the targeted image.
					 * On all PC-like platforms (including Windows and most
					 * UNIX), the supported formats are BMP, PNM (PPM/PGM/PBM),
					 * XPM, LBM, PCX, GIF, JPEG, PNG, and TIFF formats.
					 * JPEG and PNG are the two strongly recommended formats. 
					 *
					 * @param preload the image will be loaded directly by this
					 * constructor iff true, otherwise only its path will be
					 * stored to allow for later loading.
					 *
					 * @param convertToDisplayFormat tells whether this image,
					 * when loaded (now or later) should have its format
					 * converted to the screen's format, in order to offer
					 * faster blits if ever its surface was to be blitted
					 * multiple times to the screen (one-time-for-all
					 * conversion).
					 *
					 * @param convertWithAlpha if the conversion to screen 
					 * format is selected (convertToDisplayFormat is true), 
					 * tells whether the converted surface should also have an 
					 * alpha channel.  				 
					 * 
					 * @throw ImageException if the operation failed or is not
					 * supported.
					 *
					 */
					explicit Image( 
						const std::string & imageFilename,
						bool preload = true, 
						bool convertToDisplayFormat = true,
						bool convertWithAlpha = true ) ;
					
					
										
					/**
					 * Basic overridable destructor, so that it remains 
					 * virtual.
					 *
					 */
					virtual ~Image() throw() ;
					
					
					
					
					// LoadableWithContent template instanciation.
		
				
				
					/**
					 * Loads the image from file.
					 *
					 * @return true iff the image had to be actually loaded
					 * (otherwise it was already loaded and nothing was done).
					 *
					 * @throw Ceylan::LoadableException whenever the loading
					 * fails.
					 *
					 */
					virtual bool load() ;
		
		
		
					/**
					 * Unloads the image that may be contained by this instance.
					 *
					 * @return true iff the image had to be actually unloaded
					 * (otherwise it was not already available and nothing was
					 * done).
					 *
					 * @throw Ceylan::LoadableException whenever the unloading
					 * fails.
					 *
					 */
					virtual bool unload() ;
					
					
					
					/**
					 * Returns an user-friendly description of the state of 
					 * this object.
					 *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from overall 
					 * settings.
					 *
					 * @see Ceylan::TextDisplayable
					 *
					 */
					virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
	
					
					
					
					// Static section.
					
					
					
					/**
					 * Loads specified icon, and returns the corresponding
					 * surface, while updating specified mask so that it 
					 * selects only icon pixels that should remain visible.
					 * 
					 * @note Ownership of the surface and the mask is
					 * transferred to the caller, which has to deallocate 
					 * them when necessary.
					 *
					 * @note The icon must have a palette, and its size 
					 * should be 32 * 32 pixels.
					 *
					 * @note Icon format, if supported, will be auto-detected.
					 *
					 * @see VideoModule::setWindowIcon
					 *
					 */
					static Surface & LoadIcon( const std::string & filename, 
						Pixels::ColorElement ** mask ) ;
				
								
							
							
										
					// Load image section.			
					
											
					
					/**
					 * Loads an image, whose format will be auto-detected, 
					 * from specified file into target surface.
					 *
					 * @param targetSurface the surface that should contain
					 * eventually the loaded image.
					 *
					 * @param filename the name of the image file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 */						
					static void Load( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	
					
						
						
					/**
					 * Loads a JPEG image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the JPEG file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadJPG			 
					 *
					 */	
					static void LoadJPG( 
						Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a PNG image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the PNG file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadPNG			 
					 *
					 */	
					static void LoadPNG( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a BMP image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the BMP file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 *
					 * @see Surface::loadBMP			 
					 *
					 */	
					static void LoadBMP( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a GIF image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the GIF file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadGIF			 
					 *
					 */	
					static void LoadGIF( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a LBM image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the LBM file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadLBM			 
					 *
					 */	
					static void LoadLBM( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a PCX image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the PCX file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadPCX			 
					 *
					 */	
					static void LoadPCX( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a PNM image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the PNM file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadPNM			 
					 *
					 */	
					static void LoadPNM( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a TGA image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the TGA file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadTGA			 
					 *
					 */	
					static void LoadTGA( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  	



					/**
					 * Loads a XPM image from specified file into target
					 * surface.
					 *
					 * @param targetSurface the surface that should contain
					 * the loaded image.
					 *
					 * @param filename the name of the XPM file.
					 *
					 * @param blitOnly tells whether the loaded image 
					 * surface should only be blitted into the supposed 
					 * already existing internal surface and then be
					 * deallocated (if flag is true, notably useful to 
					 * load an image into the screen surface), or if this 
					 * loaded surface should simply replace the former
					 * one (if flag is false, usual case for shadow surfaces).  
					 * 
					 * @param convertToDisplay tells whether this loaded 
					 * image should have its pixel format converted to the
					 * screen's format, in order to allow for faster blits 
					 * if ever that surface was to be blitted 
					 * multiple times to the screen (one-time-for-all
					 * conversion). 
					 * <b>Beware</b>: setMode must have been called before
					 * convertToDisplay is chosen.
					 *
					 * @param convertWithAlpha if the conversion to screen
					 * format is selected (convertToDisplay is true), tells
					 * whether the converted surface should also have an 
					 * alpha channel. 				 
					 *
					 * @note The auto-detected format must be of course a
					 * supported image format.
					 *
					 * @note Specifying a blitOnly option set to true is
					 * especially convenient when specifying the screen's
					 * surface as target surface, since this surface 
					 * is special and should not be deallocated as the 
					 * others may be.
					 * 
					 * @see Surface::loadXPM			 
					 *
					 */	
					static void LoadXPM( Surface & targetSurface, 
						const std::string & filename,
						bool blitOnly = false,
						bool convertToDisplay = true,
						bool convertWithAlpha = true ) ;  
					
					
					
					
					
					// Save image section.			
					
					
											
					/**
					 * Saves specified surface on file, with specified 
					 * filename, in PNG format.
					 *
					 * @param targetSurface the surface that should be 
					 * saved on file. 
					 * This parameter cannot be 'const' since the surface 
					 * may have to be locked to read its pixels, so that 
					 * they can be stored in the image file.
					 *
					 * @param filename the name of the PNG file.
					 *
					 * @param overwrite tells whether any already existing 
					 * file should be overwritten. 
					 * The default is true, if false an exception will be 
					 * raised should a corresponding file be found.
					 *
					 * @note This method is especially convenient for
					 * screenshots. 
					 *
					 * Transparency is managed.				 
					 *
					 * @note The PNG format is to be preferred to the BMP one.
					 * 
					 * @see Surface::savePNG	
					 *		 
					 */	
					static void SavePNG( Surface & targetSurface, 
						const std::string & filename, 
						bool overwrite = true ) ;  	
						
										
											
					/**
					 * Saves specified surface on file, with specified 
					 * filename, in BMP format.
					 *
					 * @param targetSurface the surface that should be 
					 * saved on file. 
					 * This parameter cannot be 'const' since the surface 
					 * may have to be locked to read its pixels, so that 
					 * they can be stored in the image file.
					 *
					 * @param filename the name of the BMP file.
					 *
					 * @param overwrite tells whether an already existing 
					 * file should be overwritten. 
					 * The default is true, if false an exception will be 
					 * raised should a corresponding file be found.
					 *
					 * @note This method is especially convenient for
					 * screenshots.				 
					 *
					 * @note The PNG format is to be preferred to the BMP one.
					 * 
					 * @see Surface::saveBMP	
					 *		 
					 */	
					static void SaveBMP( Surface & targetSurface, 
						const std::string & filename,
						bool overwrite = true ) ;  	



						
				protected:
				
						
						
					/**
					 * Tells whether when loaded this image should be 
					 * converted to screen format.
					 *
					 */	
					bool _convertToDisplayFormat ;
					
					
					
					/**
					 * Tells whether when loaded this image should have an 
					 * alpha channel when converted to screen format.
					 *
					 * @note Ignored if _convertToDisplayFormat is false.
					 *
					 */	
					bool _convertWithAlpha ;
					
						
					
						
				private:
			
			
			
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit Image( const Image & source ) ;
			
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					Image & operator = ( const Image & source ) ;
				
				
				
				
				
					/**
					 * Tag for JPG images, one of the recommended formats.
					 *
					 */
					static const std::string JPGTag ;
					
					
					
					/**
					 * Tag for PNG images, one of the recommended formats.
					 *
					 */
					static const std::string PNGTag ;
		
		
		
					/**
					 * Tag for BMP images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string BMPTag ;
					
					
					
					/**
					 * Tag for GIF images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string GIFTag ;
		
		
		
					/**
					 * Tag for LBM images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string LBMTag ;
		
		
		
					/**
					 * Tag for PCX images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string PCXTag ;
		
		
					/**
					 * Tag for PNM images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string PNMTag ;
		
		
		
					/**
					 * Tag for TGA images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string TGATag ;
		
		
		
					/**
					 * Tag for XPM images, a supported but not recommended
					 * format.
					 *
					 */
					static const std::string XPMTag ;
		
		
				
			} ;




			/// Describes a BMP header.
			typedef struct 
			{
			
			
				/// Magic identifier.
				Ceylan::Uint16 type ;
				
				
				/// File size, in bytes.
				Ceylan::Uint32 size ;
				
				
				/// Reserved for later use.
				Ceylan::Uint16 firstReserved ;


				/// Reserved for later use.
				Ceylan::Uint16 secondReserved ;
					
								
				/// Offset to image data, in bytes.
				Ceylan::Uint32 offset ;


			} 
			
#ifndef CEYLAN_RUNS_ON_WINDOWS
				/*
				 * g++ (gcc) needs this __attribute__, but Visual C++ does 
				 * not understand it.
				 *
				 * As we are here in a public header file, only the
				 * CEYLAN_RUNS_ON_WINDOWS configuration-specific preprocessor
				 * symbol is available here.
				 *
				 */
		__attribute__ ((packed))   

#endif // CEYLAN_RUNS_ON_WINDOWS
			
			BMPHeader ;
			
			
			
			/// Describes a BMP Info header.
			typedef struct 
			{
			
			
				/// Header size, in bytes.
				Ceylan::Uint32 size ;
					
					
				/// Width of image.
				Ceylan::Uint32 width ;
				
				
				/// Height of image.
				Ceylan::Uint32 height ;
				
				
				/// Number of colour planes.
				Ceylan::Uint16 planes ;
				
				
				/// Bits per pixel.
				Ceylan::Uint16 bits ;
					
						
				/// Compression type.
				Ceylan::Uint32 compression ;


				/// Image size, in bytes.
				Ceylan::Uint32 imagesize ;


				/// Pixels per meter, for abscissa. 
				Ceylan::Uint32 xresolution ;


				/// Pixels per meter, for ordinate. 
				Ceylan::Uint32 yresolution ;


				/// Number of colours.
				Ceylan::Uint32 ncolours ;


				/// Important colours.
				Ceylan::Uint32 importantcolours ;


			}
			
#ifndef CEYLAN_RUNS_ON_WINDOWS
				/*
				 * g++ (gcc) needs this __attribute__, but Visual C++ does 
				 * not understand it.
				 *
				 * As we are here in a public header file, only the
				 * CEYLAN_RUNS_ON_WINDOWS configuration-specific preprocessor
				 * symbol is available here.
				 *
				 */
		__attribute__ ((packed))  

#endif // CEYLAN_RUNS_ON_WINDOWS
			
			BMPInfoHeader ;
			

		}
				
	}
	
}		



#endif // OSDL_IMAGE_H_

