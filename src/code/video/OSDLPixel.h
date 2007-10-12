#ifndef OSDL_PIXEL_H_
#define OSDL_PIXEL_H_


#include "OSDLVideoTypes.h"  // for Coordinate, BitsPerPixel, VideoException

#include "Ceylan.h"          // for Ceylan::Uint8, etc.



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_Color ;
struct SDL_PixelFormat ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 



#include <string>



namespace OSDL
{
	
	
	namespace Video
	{

	
		// Pixels belong to surfaces.
		class Surface ;
	
	
		// Pixel formats may refer to a palette.
		class Palette ;
		
		
		
		/**
		 * Allows to handle the various pixel colors and formats.
		 *
		 * Various conversion functions are provided, when no canonic
		 * operator can be used.
		 * 
		 * For example, comparing two PixelColor instances can be done 
		 * directly thanks to the '==' operator, provided they respect the
		 * same pixel format.
		 *
		 */
		namespace Pixels
		{



			/**
			 * Corresponds to an actual pixel color, i.e. a color definition
			 * which is encoded according to a pixel format.
			 *
			 */
			typedef Ceylan::Uint32 PixelColor ;
	
			
						
			/**
			 * Corresponds to a bit mask which allows to define how to 
			 * get color coordinates from a PixelColor, for example 
			 * depending on the endianness of the system.
			 *
			 */
			typedef Ceylan::Uint32 ColorMask ;
			
			
			
			/**
			 * Describes a coordinate used in color space, including alpha.
			 *
			 */
			typedef Ceylan::Uint8 ColorElement ;
					
									 
			
			/**
			 * The alpha coordinate which corresponds to transparent pixels.
			 *
			 * @note This is the reverse for OpenGL.
			 *
			 */
			const ColorElement AlphaTransparent = 0 ; 
			
			
			
			/**
			 * The alpha coordinate which corresponds to opaque (solid) 
			 * pixels.		
			 *
			 * @note This is the reverse for OpenGL.
			 *
			 */
			const ColorElement AlphaOpaque = 255 ; 
			

	
			/**
			 * Format-independent color description.
			 *
			 * Corresponds to the RBGA information describing a color 
			 * defined in 32-bit color space, in this order: the latest
			 * coordinate is the pixel's alpha channel.
			 *
			 * @note The alpha coordinate is not always taken into account
			 * and reliable.
			 *
			 * @see The list of color names (ex: Pixels::RoyalBlue).
			 *
			 */		
#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

			/**
			 * Describes a color definition, not converted to a specific
			 * format.
			 *
			 */
			typedef ::SDL_Color ColorDefinition ;


			/**
			 * Describes a pixel format.
			 *
			 */
			typedef ::SDL_PixelFormat PixelFormat ;
			
#else // OSDL_USES_SDL


			/**
			 * Describes a color definition, not converted to a specific
			 * format.
			 *
			 */
			struct ColorDefinition
			{
	
				ColorElement r ;
				ColorElement g ;
				ColorElement b ;
				
				// Useful for alpha though:
				ColorElement unused ;
				
			} ;


			/**
			 * Describes a pixel format.
			 *
			 */
			typedef struct PixelFormat 
			{
			
				Video::Palette * palette ;
				
				Video::BitsPerPixel BitsPerPixel ;
				Video::BytesPerPixel BytesPerPixel ;
				
				Ceylan::Uint8 Rloss ;
				Ceylan::Uint8 Gloss ;
				Ceylan::Uint8 Bloss ;
				Ceylan::Uint8 Aloss ;
				
				Ceylan::Uint8 Rshift ;
				Ceylan::Uint8 Gshift ;
				Ceylan::Uint8 Bshift ;
				Ceylan::Uint8 Ashift ;
				
				ColorMask Rmask ;
				ColorMask Gmask ;
				ColorMask Bmask ;
				ColorMask Amask ;

				// RGB color key information.
				PixelColor colorkey ;
				
				// Alpha value information (per-surface alpha):
				ColorElement alpha ;
			
			} ;
	
	
#endif // OSDL_USES_SDL
		
					
			
			/**
			 * Gamma controls the brightness/contrast of colors displayed 
			 * on the screen. 
			 *
			 */
			typedef Ceylan::Float32 GammaFactor ;
			
			
			/**
			 * Sets the "gamma function" for the display of each color
			 * component. 
			 *
			 * Gamma controls the brightness/contrast of colors displayed 
			 * on the screen. 
			 *
			 * A gamma value of 1.0 is identity (i.e., no adjustment is made).
			 *
			 * @note Not all display hardware are able to change gamma.
			 *
			 * @return true iff the operation is a success. An error could 
			 * be that gamma adjustment is not supported: not all display
			 * hardware support it.
			 *
			 */
			bool setGamma( GammaFactor red, GammaFactor green, 
				GammaFactor blue ) throw() ;
			
			
			/// A color element of a gamma ramp.
			typedef Ceylan::Uint16 GammaRampElement ;
						
			
			/**
			 * Sets the gamma lookup tables for the display for each color
			 * component. 
			 *
			 * Each ramp (table) should have 256 entries.
			 *
			 * Each gamma ramp represents a mapping between the input and 
			 * output for that color coordinate.
			 *
			 * The input is the index into the array, and the output is the
			 * 16-bit gamma value at that index, scaled to the output color
			 * precision. 
			 * You may pass a null pointer (O) to any of the channels to 
			 * leave them unchanged.
			 *
			 * @note This function adjusts the gamma based on lookup tables, 
			 * you can also have the gamma calculated based on a 
			 * "gamma function", parametered with setGamma.
			 *
			 * @return true iff the operation is a success. An error could be
			 * that gamma adjustment is not supported, since not all display
			 * hardware support it.
			 *
			 * @fixme Ownership of specified buffers is not clear. 
			 * Who should deallocate them ?
			 *
			 */
			OSDL_DLL bool setGammaRamp( GammaRampElement * redRamp, 
					GammaRampElement * greenRamp, GammaRampElement * blueRamp )
				throw() ;
				
				
			/**
			 * Gets the color gamma lookup tables for the display.
			 *
			 * @return true iff the operation is a success. An error could be
			 * that gamma adjustment is not supported, since not all display
			 * hardware support it.
			 *
			 * @fixme Ownership of specified buffers is not clear. 
			 * Who should allocate them ? We suppose the caller must allocate
			 * them.
			 *
			 */	
			OSDL_DLL bool getGammaRamp( GammaRampElement * redRamp, 
					GammaRampElement * greenRamp, GammaRampElement * blueRamp )
				throw() ;




			// Color mask section.
			

			/**
			 * Returns the RBGA masks which are recommended on this platform:
			 * the endianness is taken into account, regardless of any 
			 * specific pixel format. 
			 *
			 * This method just chooses, among the various 32 bit 
			 * combinations of masks, one that would be suitable for most 
			 * uses, including for OpenGL textures.
			 *
			 * @param format the pixel format from which masks are to be read.
			 *
			 * @param redMask the variable in which this method will put 
			 * the red mask.
			 *
			 * @param greenMask the variable in which this method will
			 * put the green mask.
			 *
			 * @param blueMask the variable in which this method will 
			 * put the blue mask.
			 *
			 * @param alphaMask the variable in which this method will 
			 * put the alpha mask.
			 *
			 * @see getColorMasks
			 *
			 */
			OSDL_DLL void getRecommendedColorMasks( ColorMask & redMask, 
				ColorMask & greenMask, ColorMask & blueMask, 
				ColorMask & alphaMask ) throw() ;


			/**
			 * Returns the RBG masks which are recommended on this platform:
			 * the endianness is taken into account, regardless of any 
			 * specific pixel format. 
			 *
			 * This method just chooses, among the various 32 bit 
			 * combinations of masks, one that would be suitable for most 
			 * uses, including for OpenGL textures.
			 *
			 * @param format the pixel format from which masks are to be read.
			 *
			 * @param redMask the variable in which this method will put 
			 * the red mask.
			 *
			 * @param greenMask the variable in which this method will 
			 * put the green mask.
			 *
			 * @param blueMask the variable in which this method will 
			 * put the blue mask.
			 *
			 * @see getColorMasks
			 *
			 */
			OSDL_DLL void getRecommendedColorMasks( ColorMask & redMask, 
				ColorMask & greenMask, ColorMask & blueMask ) throw() ;


			/**
			 * Reads from specified pixel format the RBGA masks, and 
			 * returns them by updating specified color mask references.
			 *
			 * @param format the pixel format from which masks are to be read.
			 *
			 * @param redMask the variable in which this method will put 
			 * the red mask.
			 *
			 * @param greenMask the variable in which this method will 
			 * put the green mask.
			 *
			 * @param blueMask the variable in which this method will put 
			 * the blue mask.
			 *
			 * @param alphaMask the variable in which this method will 
			 * put the alpha mask.
			 *
			 * @see getRecommendedColorMasks
			 *
			 */
			OSDL_DLL void getCurrentColorMasks( const PixelFormat & format, 
				ColorMask & redMask, ColorMask & greenMask, 
				ColorMask & blueMask, ColorMask & alphaMask ) throw() ;



			// Color conversion section.


			/**
			 * Converts a set of four coordinates in RGBA color space to 
			 * the equivalent ColorDefinition.
			 *
			 * @note If alpha coordinate is not specified (just plain RGB), 
			 * the pixel will be considered as fully opaque.
			 *
			 * @note No mapping to any pixel format is performed, it is 
			 * just two different ways of gathering the exact same data 
			 * about color.
			 *
			 * @see convertColorDefinitionToRGBA, convertRGBAToPixelColor
			 *
			 */
			OSDL_DLL ColorDefinition convertRGBAToColorDefinition( 
				ColorElement red, ColorElement green, ColorElement blue, 
				ColorElement alpha = AlphaOpaque ) throw() ;
				
			
			/**
			 * Updates provided RGBA quadruplet from the specified color
			 * definition.
			 *
			 * @note No mapping to any pixel format is performed, it is 
			 * just two different ways of gathering the exact same data 
			 * about color.
			 *
			 * @see convertRGBAToColorDefinition, convertRGBAToPixelColor
			 *
			 */
			OSDL_DLL void convertColorDefinitionToRGBA( ColorDefinition color,
				ColorElement & red, ColorElement & green, ColorElement & blue, 
				ColorElement & alpha ) throw() ;
							
			
			/**
			 * Converts a set of four coordinates in RGBA color space to a
			 * PixelColor, using specified format.
			 *
			 * @note If alpha coordinate is not specified (just plain RGB), 
			 * the pixel will be considered as fully opaque.
			 *
			 */
			OSDL_DLL PixelColor convertRGBAToPixelColor( 
				const PixelFormat & format,
				ColorElement red, ColorElement green, ColorElement blue, 
				ColorElement alpha = AlphaOpaque ) throw() ;
				
			
			/**
			 * Returns RGBA quadruplet corresponding to specified pixel, 
			 * encoded according to specified pixel format.
			 *
			 * @note If the surface has no alpha component, the alpha 
			 * will be returned as AlphaOpaque (100% opaque).
			 *
			 * @note This methods uses the entire 8-bit [0..255] range when
			 * converting color components from pixel formats with less than
			 * 8-bits per RGB component (e.g., a completely white pixel in
			 * 16-bit RGB565 format would return [0xff, 0xff, 0xff] not
			 * [0xf8, 0xfc, 0xf8]).
			 *
			 * @return a ColorDefinition, from which RGBA components can be
			 * directly read (myPixelDefinition.r, myPixelDefinition.g,
			 * myPixelDefinition.b and myPixelDefinition.unused for alpha)
			 *
			 * @throw VideoException if the operation failed or is not 
			 * supported.
			 *
			 */
			OSDL_DLL ColorDefinition convertPixelColorToColorDefinition( 
					const PixelFormat & format,	PixelColor pixel ) 
				throw( VideoException ) ;
	
					
			/**
			 * Converts a set of four coordinates in RGBA color space, 
			 * expressed as a color definition, to a PixelColor, using 
			 * specified surface format.
			 *
			 * If the specified pixel format has no alpha component, or if 
			 * a palette is used, then the alpha value will be ignored 
			 * since it can be in no way be taken into account.
			 *
			 */
			OSDL_DLL PixelColor convertColorDefinitionToPixelColor( 
				const PixelFormat & format,	ColorDefinition colorDef ) throw() ;
	

			/**
			 * Converts a color definition to a raw PixelColor, regardless 
			 * of any pixel format.
			 *
			 * @note No pixel format mapping of any kind is performed, 
			 * it is a transformation between two exactly similar formats, as
			 * if the pixel format was exactly 32-bit RGBA.
			 *
			 */
			OSDL_DLL PixelColor convertColorDefinitionToRawPixelColor( 
				ColorDefinition colorDef ) throw() ;
		
		
			/**
			 * Converts a RGBA raw color into a raw PixelColor, with no 
			 * pixel format mapping.
			 *
			 * @note No pixel format mapping of any kind is performed, 
			 * it is a transformation between two exactly similar formats, as
			 * if the pixel format was exactly 32-bit RGBA.
			 *
			 */
			OSDL_DLL PixelColor convertRGBAToRawPixelColor( 
				ColorElement red, ColorElement green, ColorElement blue, 
				ColorElement alpha = AlphaOpaque ) throw() ;
		
		
		
		
			// Color comparison section.
			
			
			/**
			 * Returns whether the two color definitions describe the 
			 * exact same color.
			 *
			 * @param first the first color definition for the comparison.
			 *
			 * @param second the second color definition for the comparison.
			 *
			 * @param useAlpha tells whether the alpha coordinate is tested 
			 * too for equality.
			 *
			 */
			OSDL_DLL bool areEqual( ColorDefinition first, 
				ColorDefinition second, bool useAlpha = true ) throw() ;
		
		
			/**
			 * Returns whether <b>value</b> is strictly less than
			 * <b>comparison</b>, by defining an arbitrary order in color
			 * coordinates.
			 *
			 * @param value the value of reference.
			 *
			 * @param comparison the value to compare with.
			 *
			 * @note This function is useful to provide a '<' operator for 
			 * some containers (see Text::Font class).
			 *
			 */
			OSDL_DLL bool isLess( ColorDefinition value, 
				ColorDefinition comparison ) throw() ;
		
		
			/**
			 * Returns whether the two pixel colors describe the exact 
			 * same color, supposing that the two pixel colors are 
			 * defined according to the same pixel format.
			 *
			 * @param first the first pixel color for the comparison.
			 *
			 * @param second the second pixel definition for the comparison.
			 *
			 * @note The alpha coordinate cannot be disabled or enabled, 
			 * this function just compares the two values and could be 
			 * directly replaced by the '==' operator.
			 *
			 */
			OSDL_DLL bool areEqual( PixelColor first, PixelColor second )
				throw() ;


			/**
			 * Returns a color definition which is chosen different from the 
			 * specified ones. 
			 *
			 * The alpha coordinate is not taken into account in comparisons.
			 *
			 * It is useful for example when having to find predictably a 
			 * color key which does not collide with two used colors.
			 *
			 * @param first the first color to avoid.
			 *
			 * @param second the second color to avoid.
			 *
			 * @return a colorkey different from all provided colors.
			 *
			 */
			OSDL_DLL ColorDefinition selectColorDifferentFrom( 
				ColorDefinition first, ColorDefinition second ) throw() ;
		
		
			/**
			 * Returns a color definition which is different from the 
			 * specified ones. 
			 *
			 * The alpha coordinate is not taken into account in comparisons.
			 *
			 * It is useful for example when having to find predictably a 
			 * color key which does not collide with a set of used colors.
			 *
			 * @param first the first color to avoid.
			 *
			 * @param second the second color to avoid.
			 *
			 * @param third the third color to avoid.
			 *
			 * @return a colorkey different from all provided colors.
			 *
			 */
			OSDL_DLL ColorDefinition selectColorDifferentFrom( 
				ColorDefinition first, ColorDefinition second, 
					ColorDefinition third ) throw() ;
				
				
				
			// get/put pixel operations.
			

			/**
	 		 * Returns the pixel color at [x;y].
			 * 
			 * This method is generic (i.e. not optimized for any display
			 * format).
			 *
			 * @note No clipping is performed, and the surface should have 
			 * been previously locked if necessary.			 
	 		 *
			 * @see put methods
	 		 * @see http://sdldoc.csn.ul.ie/guidevideo.php 
			 *
	 		 */
			OSDL_DLL PixelColor getPixelColor( const Surface & fromSurface, 
				Coordinate x, Coordinate y ) throw ( VideoException ) ;
			
			
			/**
	 		 * Returns the color definition of pixel at [x;y].
			 * 
			 * This method is generic (i.e. not optimized for any display
			 * format).
			 *
			 * @param fromSurface the surface to read the pixel from. 
			 * If this surface has no alpha coordinate, then the returned 
			 * color definition will have its fourth color coordinate
			 * equal to AlphaOpaque.
			 *
			 * @note No clipping is performed, and the surface should have 
			 * been previously locked if necessary.			 
	 		 *
			 * @see put methods
	 		 * @see http://sdldoc.csn.ul.ie/guidevideo.php 
			 *
	 		 */
			OSDL_DLL ColorDefinition getColorDefinition( 
				const Surface & fromSurface, 
				Coordinate x, Coordinate y ) throw ( VideoException ) ;

			
			/**
		 	 * Puts specified pixel at [x;y] with the given color, 
			 * specified as separate RGBA coordinates. 
			 *
			 * If the alpha coordinate is not AlphaOpaque, then the specified
			 * pixel will be alphablended with the pixel already present 
			 * at specified location.
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
			 * @param blending tells whether the alpha channel must be taken
			 * into account, resulting to alpha blending with the destination
			 * pixel. 
			 * If false, the exact specified color will be put in target 
			 * pixel, instead of being blended with it.
			 *
			 * @param clipping tells whether point location is checked 
			 * against surface bounds.
			 * If clipping is activated and the pixel is outside, nothing 
			 * is done.
			 *
			 * @param locking tells whether this primitive should take care 
			 * of locking / unlocking the surface (not recommended on a per
			 * pixel basis, because of lock overhead).
			 *
			 * @throw VideoException if a problem occurs with a lock operation. 
			 *
			 * @note The four RGBA coordinates will be automatically mapped
			 * according to the target surface's pixel format.
		 	 *
			 * @see get methods
			 *
			 * @see http://sdldoc.csn.ul.ie/guidevideo.php
			 *
			 */
			OSDL_DLL void putRGBAPixel( Surface & targetSurface, 
					Coordinate x, Coordinate y, 
					ColorElement red, ColorElement green, ColorElement blue, 
					ColorElement alpha = AlphaOpaque, 
					bool blending = true, bool clipping = true, 
					bool locking = false )
				throw( VideoException ) ;
	


			/**
		 	 * Puts the specified pixel at [x;y] with the given color, 
			 * specified as a RGBA color definition, not as a pixel color
			 * already encoded according to the pixel format of target surface.
			 *
			 * If the alpha coordinate is not AlphaOpaque, then the specified
			 * pixel will be alphablended with the pixel already present at
			 * specified location.
			 * 
			 * @param x the abscissa of the point to change.
			 *
			 * @param y the ordinate of the point to change.
			 *
			 * @param colorDef the color definition of the pixel to be put.
			 * These color coordinates will be mapped according to the 
			 * target surface's pixel format.
			 *
			 * @param blending tells whether the alpha channel must be 
			 * taken into account, resulting to alpha blending with the
			 * destination pixel. 
			 * If false, the exact specified color will be put in target 
			 * pixel, instead of being blended with it.
			 *
			 * @param clipping tells whether point location is checked 
			 * against surface bounds.
			 * If clipping is activated and the pixel is outside, nothing 
			 * is done.
			 *
			 * @param locking tells whether this primitive should take care
			 * of locking / unlocking the surface (not recommended on a 
			 * per pixel basis, because of lock overhead).
			 *
			 * @note This method is relatively expensive.
			 *
			 * @see get methods
			 *
			 * @see http://sdldoc.csn.ul.ie/guidevideo.php
			 *
			 */
			OSDL_DLL void putColorDefinition( Surface & targetSurface, 
					Coordinate x, Coordinate y, 
					ColorDefinition colorDef,
					bool blending = true, bool clipping = true, 
					bool locking = false )
			 	throw( VideoException ) ;
	
	
	
			/**
		 	 * Puts the pixel at [x;y] with the given color, specified as a
			 * 32-bit RGBA pixel color, already encoded according to 
			 * Surface's pixel format.
			 * 
			 * If the alpha coordinate is not AlphaOpaque, then the 
			 * specified pixel will be alphablended with the pixel already
			 * present at specified location.
			 *
			 * @param x the abscissa of the point to change.
			 *
			 * @param y the ordinate of the point to change.
			 *
			 * @param convertedColor the color of the pixel to be put, 
			 * a 32-bit pixel value already mapped according to the target
			 * surface's pixel format.
			 *
			 * @param alpha the full alpha coordinate to be used. 
			 * The one guessed from the 'convertedColor' parameter would 
			 * not be enough, since it would be already encoded and rounded.
			 *
			 * @param blending tells whether the alpha channel must be taken
			 * into account, resulting to alpha blending with the 
			 * destination pixel. 
			 * If false, the exact specified color will be put in target
			 * pixel, instead of being blended with it.
			 *
			 * @param clipping tells whether point location is checked 
			 * against surface bounds.
			 * If clipping is activated and the pixel is outside, nothing 
			 * is done.
			 *
			 * @param locking tells whether this primitive should take 
			 * care of locking / unlocking the surface (not recommended on 
			 * a per pixel basis, because of lock overhead).
			 *
			 * @see get methods
			 *
			 * @see http://sdldoc.csn.ul.ie/guidevideo.php
			 *
			 */
			OSDL_DLL void putPixelColor( Surface & targetSurface, 
					Coordinate x, Coordinate y, 
					PixelColor convertedColor, ColorElement alpha,
					bool blending = true, bool clipping = true, 
					bool locking = false )
			 	throw( VideoException ) ;
	
		
		
			/**
			 * Alternative method to set the pixel at [x;y] to the given 
			 * pixel color.
			 *
			 * If the alpha coordinate is not AlphaOpaque, then the 
			 * specified pixel will be alphablended with the pixel already
			 * present at specified location.
			 *
			 * @param mapToSurfaceFormat tells whether the specified pixel 
			 * color is to be encoded to target Surface's pixel format (if
			 * true), or if it is not wanted (if false, for example if it 
			 * is already done).
			 *
			 * @note This function should not be used directly, it is merely
			 * a debug method when pixel-level operations must be checked.
			 *
			 */
			OSDL_DLL void alternativePutPixelColor( Surface & targetSurface, 
				Coordinate x, Coordinate y, 
				PixelColor color, bool mapToSurfaceFormat = false ) throw()	;
				
								
				
			/**
			 * Returns a textual representation of the specified pixel format.
			 *
			 * @param format the pixel format to describe.
			 *
			 */
			OSDL_DLL std::string toString( const PixelFormat & format ) 
				throw() ;


			/**
			 * Returns a textual representation of the specified color,
			 * according to the specified pixel format.
			 *
			 * @param pixel the pixel color to describe.
			 *
			 * @param format the pixel format which allows to decode 
			 * the pixel.
			 *
			 */
			OSDL_DLL std::string toString( PixelColor pixel, 
				const PixelFormat & format ) throw() ;

			
			/**
			 * Returns a textual representation of the specified color
			 * definition.
			 *
			 * @note The alpha coordinate has not always a meaning, 
			 * depending of the context.
			 *
			 */
			OSDL_DLL std::string toString( ColorDefinition color ) throw() ;
			
			
			/**
			 * Main color definitions: 115 different colors described by name.
			 *
			 * Come from http://eies.njit.edu/~walsh 
			 * Copyright © 2001 Kevin J. Walsh
			 *
			 */			
			

			/**
			 * Transparent color (alpha set to not opaque at all). 
			 * With an opaque alpha, would be pure black.
			 *
			 * This color is useful to specify that, for example, a 
			 * background should remain empty, i.e. fully transparent.
			 *
			 */
			extern OSDL_DLL const ColorDefinition Transparent ;          
			
			 
			/// Shades of Grey:

			extern OSDL_DLL const ColorDefinition Black ;          
			extern OSDL_DLL const ColorDefinition Grey ;           
			extern OSDL_DLL const ColorDefinition DimGrey ;        
			extern OSDL_DLL const ColorDefinition LightGrey ;      
			extern OSDL_DLL const ColorDefinition SlateGrey ;      
			extern OSDL_DLL const ColorDefinition Silver ;         



			/// Shades of Blue:
			
			extern OSDL_DLL const ColorDefinition AliceBlue ;      
			extern OSDL_DLL const ColorDefinition BlueViolet ;     
			extern OSDL_DLL const ColorDefinition CadetBlue ;      
			extern OSDL_DLL const ColorDefinition DarkSlateBlue ;  
			extern OSDL_DLL const ColorDefinition DarkTurquoise ;  
			extern OSDL_DLL const ColorDefinition DeepSkyBlue ;    
			extern OSDL_DLL const ColorDefinition DodgerBlue ;     
			extern OSDL_DLL const ColorDefinition LightBlue ;      
			extern OSDL_DLL const ColorDefinition LightCyan ;      
			extern OSDL_DLL const ColorDefinition MediumBlue ;     
			extern OSDL_DLL const ColorDefinition NavyBlue ;       
			extern OSDL_DLL const ColorDefinition RoyalBlue ;      
			extern OSDL_DLL const ColorDefinition SkyBlue ;        
			extern OSDL_DLL const ColorDefinition SlateBlue ;      
			extern OSDL_DLL const ColorDefinition SteelBlue ;      
			extern OSDL_DLL const ColorDefinition Aquamarine ;     
			extern OSDL_DLL const ColorDefinition Azure ;          
			extern OSDL_DLL const ColorDefinition Blue ;           
			extern OSDL_DLL const ColorDefinition Cyan ; 	      
			extern OSDL_DLL const ColorDefinition Turquoise ;      
			extern OSDL_DLL const ColorDefinition MidnightBlue ;      



			/// Shades of Brown:

			extern OSDL_DLL const ColorDefinition Brown ;      
			extern OSDL_DLL const ColorDefinition RosyBrown ;      
			extern OSDL_DLL const ColorDefinition SaddleBrown ;    
			extern OSDL_DLL const ColorDefinition Beige ;          
			extern OSDL_DLL const ColorDefinition Burlywood ;      
			extern OSDL_DLL const ColorDefinition Chocolate ;      
			extern OSDL_DLL const ColorDefinition Peru ;           
			extern OSDL_DLL const ColorDefinition Tan ;            
			extern OSDL_DLL const ColorDefinition Copper ;         



			/// Shades of Green:

			extern OSDL_DLL const ColorDefinition DarkGreen ;      
			extern OSDL_DLL const ColorDefinition DarkOliveGreen ; 
			extern OSDL_DLL const ColorDefinition ForestGreen ;    
			extern OSDL_DLL const ColorDefinition GreenYellow  ;   
			extern OSDL_DLL const ColorDefinition LawnGreen ;      
			extern OSDL_DLL const ColorDefinition LimeGreen ;      
			extern OSDL_DLL const ColorDefinition MintCream ;     
			extern OSDL_DLL const ColorDefinition OliveDrab ;      
			extern OSDL_DLL const ColorDefinition PaleGreen ;      
			extern OSDL_DLL const ColorDefinition SeaGreen ;       
			extern OSDL_DLL const ColorDefinition SpringGreen ;    
			extern OSDL_DLL const ColorDefinition YellowGreen ;    
			extern OSDL_DLL const ColorDefinition Chartreuse ;     
			extern OSDL_DLL const ColorDefinition Green ;          
			extern OSDL_DLL const ColorDefinition Khaki ;          



			/// Shades of Orange:

			extern OSDL_DLL const ColorDefinition DarkOrange ;     
			extern OSDL_DLL const ColorDefinition DarkSalmon ;     
			extern OSDL_DLL const ColorDefinition LightCoral ;     
			extern OSDL_DLL const ColorDefinition LightSalmon ;    
			extern OSDL_DLL const ColorDefinition PeachPuff ;      
			extern OSDL_DLL const ColorDefinition Bisque ;         
			extern OSDL_DLL const ColorDefinition Coral ;          
			extern OSDL_DLL const ColorDefinition Honeydew ;       
			extern OSDL_DLL const ColorDefinition Orange ;         
			extern OSDL_DLL const ColorDefinition Salmon ;         
			extern OSDL_DLL const ColorDefinition Sienna ;         



			/// Shades of Red:

			extern OSDL_DLL const ColorDefinition DeepPink ;       
			extern OSDL_DLL const ColorDefinition HotPink ;       
			extern OSDL_DLL const ColorDefinition IndianRed ;      
			extern OSDL_DLL const ColorDefinition LightPink ;      
			extern OSDL_DLL const ColorDefinition MediumVioletRed ;
			extern OSDL_DLL const ColorDefinition MistyRose ;      
			extern OSDL_DLL const ColorDefinition OrangeRed ;      
			extern OSDL_DLL const ColorDefinition VioletRed ;      
			extern OSDL_DLL const ColorDefinition Firebrick ;      
			extern OSDL_DLL const ColorDefinition Pink ;           
			extern OSDL_DLL const ColorDefinition Red ;            
			extern OSDL_DLL const ColorDefinition Tomato ;         



			/// Shades of Violet:

			extern OSDL_DLL const ColorDefinition DarkOrchid ;     
			extern OSDL_DLL const ColorDefinition DarkViolet ;     
			extern OSDL_DLL const ColorDefinition LavenderBlush ;  
			extern OSDL_DLL const ColorDefinition MediumOrchid ;   
			extern OSDL_DLL const ColorDefinition MediumPurple ;   
			extern OSDL_DLL const ColorDefinition Lavender ;       
			extern OSDL_DLL const ColorDefinition Magenta ;        
			extern OSDL_DLL const ColorDefinition Maroon ;         
			extern OSDL_DLL const ColorDefinition Orchid ;         
			extern OSDL_DLL const ColorDefinition Plum ;           
			extern OSDL_DLL const ColorDefinition Purple ;         
			extern OSDL_DLL const ColorDefinition Thistle ;        
			extern OSDL_DLL const ColorDefinition Violet ;         



			/// Shades of White:

			extern OSDL_DLL const ColorDefinition AntiqueWhite ;   
			extern OSDL_DLL const ColorDefinition FloralWhite ;    
			extern OSDL_DLL const ColorDefinition GhostWhite ;     
			extern OSDL_DLL const ColorDefinition NavajoWhite ;    
			extern OSDL_DLL const ColorDefinition OldLace ;        
			extern OSDL_DLL const ColorDefinition WhiteSmoke ;     
			extern OSDL_DLL const ColorDefinition Gainsboro ;      
			extern OSDL_DLL const ColorDefinition Ivory ;          
			extern OSDL_DLL const ColorDefinition Linen ;          
			extern OSDL_DLL const ColorDefinition Seashell ;       
			extern OSDL_DLL const ColorDefinition Snow ;          
			extern OSDL_DLL const ColorDefinition Wheat ;          
			extern OSDL_DLL const ColorDefinition White ;          



			/// Shades of Yellow:

			extern OSDL_DLL const ColorDefinition BlanchedAlmond ; 
			extern OSDL_DLL const ColorDefinition DarkGoldenrod ;  
			extern OSDL_DLL const ColorDefinition LemonChiffon ;   
			extern OSDL_DLL const ColorDefinition LightGoldenrod ; 
			extern OSDL_DLL const ColorDefinition LightYellow ;    
			extern OSDL_DLL const ColorDefinition PaleGoldenrod ;  
			extern OSDL_DLL const ColorDefinition PapayaWhip ;   
			extern OSDL_DLL const ColorDefinition Cornsilk ;       
			extern OSDL_DLL const ColorDefinition Gold ;           
			extern OSDL_DLL const ColorDefinition Goldenrod ;      
			extern OSDL_DLL const ColorDefinition Moccasin ;       
			extern OSDL_DLL const ColorDefinition Yellow ;         
		
		
		
		}
	
	}

}


#endif // OSDL_PIXEL_H_

