#ifndef OSDL_IMAGE_H_
#define OSDL_IMAGE_H_


#include "OSDLPixel.h"           // for Pixels::ColorElement
#include "OSDLVideoTypes.h"      // for VideoException

#include "Ceylan.h"


#include <string>



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
				
					ImageException( const std::string & reason ) throw() ; 
					virtual ~ImageException() throw() ; 
			} ;
				
				
				
			/**
			 * Provides an encapsulation for any picture instance.
			 *
			 * The Image interface is mostly a set of related functions 
			 * (static methods), including factories. 
			 * 
			 * @see Surface::LoadImage
			 *
			 * Supported images formats are :
			 *		- JPG (recommended)
			 *		- PNG (recommended)
			 * 		- BMP
			 *		- GIF
			 *		- LBM
			 *		- PCX
			 *		- PNM
			 *		- TGA
			 *		- XPM
			 *
			 * Our belief is however that the fewer formats you use, the 
			 * simpler it will be for everyone. 
			 *
			 * We therefore recommend using two of them, which moreover 
			 * are patent-less standards and should cover most if not all 
			 * needs :
			 *		- JPG : to compress, with little loss, full-blown images
			 * such as photographies, with high color depth
			 *		- PNG : to compress without loss computer-generated 
			 * images, with or without alpha channel
			 *
			 */
			class OSDL_DLL Image : public Ceylan::TextDisplayable
			{
				
				
				public:
					
					
					/**
					 * This Image constructor uses given filename to load 
					 * the picture from the corresponding file .
					 *
					 * Actual picture format is guessed.
					 *
					 */
					explicit Image( const std::string & filename ) 
						throw( ImageException ) ;
					
										
					/**
					 * Basic overridable destructor, so that it remains 
					 * virtual.
					 *
					 */
					virtual ~Image() throw() ;
					
					
					
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
						Pixels::ColorElement ** mask ) throw( ImageException ) ;
				
								
							
										
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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	
					
						
						
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
					 * <b>Beware</b> : setMode must have been called before
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
					static void LoadJPG( Surface & targetSurface, 
							const std::string & filename,
							bool blitOnly = false,
							bool convertToDisplay = true,
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
						bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  	



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
					 * <b>Beware</b> : setMode must have been called before
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
							bool convertWithAlpha = true  ) 
						throw( ImageException ) ;  
					
					
					
					
					
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
							bool overwrite = true ) 
						throw( ImageException ) ;  	
						
										
											
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
							bool overwrite = true ) 
						throw( ImageException ) ;  	


						
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
				
				
					/// The filename corresponding to the internal image.		
					std::string _filename ;
						
						
						
				private:
			
			
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit Image( const Image & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					Image & operator = ( const Image & source ) throw() ;
				
				
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
		}		
	}
}		


#endif // OSDL_IMAGE_H_

