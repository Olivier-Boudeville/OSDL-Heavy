#ifndef OSDL_PIXEL_H_
#define OSDL_PIXEL_H_


#include "OSDLVideoTypes.h"  // for Coordinate, BitsPerPixel, VideoException

#include "Ceylan.h"          // for Ceylan::Uint8, etc.


#include "SDL.h"             // for SDL_PixelFormat

#include <string>



namespace OSDL
{
	
	
	namespace Video
	{

	
		// Pixels belong to surfaces.
		class Surface ;
	
	
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
			 * Describes a pixel format.
			 *
			 */
			typedef SDL_PixelFormat PixelFormat ;
		
	
	
			/**
			 * Format-independent color description.
			 *
			 * Corresponds to the RBGA information describing a color 
			 * defined in 32-bit color space, in this order : the latest
			 * coordinate is the pixel's alpha channel.
			 *
			 * @note The alpha coordinate is not always taken into account
			 * and reliable.
			 *
			 * @see The list of color names (ex : Pixels::RoyalBlue).
			 *
			 */		
			typedef SDL_Color ColorDefinition ;

		
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
			 * be that gamma adjustment is not supported : not all display
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
			bool setGammaRamp( GammaRampElement * redRamp, 
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
			bool getGammaRamp( GammaRampElement * redRamp, 
					GammaRampElement * greenRamp, GammaRampElement * blueRamp )
				throw() ;




			// Color mask section.
			

			/**
			 * Returns the RBGA masks which are recommended on this platform :
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
			void getRecommendedColorMasks( ColorMask & redMask, 
				ColorMask & greenMask, ColorMask & blueMask, 
				ColorMask & alphaMask ) throw() ;


			/**
			 * Returns the RBG masks which are recommended on this platform :
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
			void getRecommendedColorMasks( ColorMask & redMask, 
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
			void getCurrentColorMasks( const PixelFormat & format, 
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
			ColorDefinition convertRGBAToColorDefinition( 
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
			void convertColorDefinitionToRGBA( ColorDefinition color,
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
			PixelColor convertRGBAToPixelColor( const PixelFormat & format,
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
			 */
			ColorDefinition convertPixelColorToColorDefinition( 
				const PixelFormat & format,	PixelColor pixel ) throw() ;
	
					
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
			PixelColor convertColorDefinitionToPixelColor( 
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
			PixelColor convertColorDefinitionToRawPixelColor( 
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
			PixelColor convertRGBAToRawPixelColor( 
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
			bool areEqual( ColorDefinition first, ColorDefinition second, 
				bool useAlpha = true ) throw() ;
		
		
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
			bool isLess( ColorDefinition value, ColorDefinition comparison )
				throw() ;
		
		
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
			bool areEqual( PixelColor first, PixelColor second ) throw() ;


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
			ColorDefinition selectColorDifferentFrom( ColorDefinition first, 
				ColorDefinition second ) throw() ;
		
		
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
			ColorDefinition selectColorDifferentFrom( ColorDefinition first, 
				ColorDefinition second, ColorDefinition third ) throw() ;
				
				
				
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
			PixelColor getPixelColor( const Surface & fromSurface, 
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
			ColorDefinition getColorDefinition( const Surface & fromSurface, 
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
			void putRGBAPixel( Surface & targetSurface, 
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
			void putColorDefinition( Surface & targetSurface, 
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
			void putPixelColor( Surface & targetSurface, 
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
			void alternativePutPixelColor( Surface & targetSurface, 
				Coordinate x, Coordinate y, 
				PixelColor color, bool mapToSurfaceFormat = false ) throw()	;
				
								
				
			/**
			 * Returns a textual representation of the specified pixel format.
			 *
			 * @param format the pixel format to describe.
			 *
			 */
			std::string toString( const PixelFormat & format ) throw() ;


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
			std::string toString( PixelColor pixel, 
				const PixelFormat & format ) throw() ;

			
			/**
			 * Returns a textual representation of the specified color
			 * definition.
			 *
			 * @note The alpha coordinate has not always a meaning, 
			 * depending of the context.
			 *
			 */
			std::string toString( ColorDefinition color ) throw() ;
			
			
			/**
			 * Main color definitions : 115 different colors described by name.
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
			extern const ColorDefinition Transparent ;          
			
			 
			/// Shades of Grey :

			extern const ColorDefinition Black ;          
			extern const ColorDefinition Grey ;           
			extern const ColorDefinition DimGrey ;        
			extern const ColorDefinition LightGrey ;      
			extern const ColorDefinition SlateGrey ;      
			extern const ColorDefinition Silver ;         



			/// Shades of Blue :
			
			extern const ColorDefinition AliceBlue ;      
			extern const ColorDefinition BlueViolet ;     
			extern const ColorDefinition CadetBlue ;      
			extern const ColorDefinition DarkSlateBlue ;  
			extern const ColorDefinition DarkTurquoise ;  
			extern const ColorDefinition DeepSkyBlue ;    
			extern const ColorDefinition DodgerBlue ;     
			extern const ColorDefinition LightBlue ;      
			extern const ColorDefinition LightCyan ;      
			extern const ColorDefinition MediumBlue ;     
			extern const ColorDefinition NavyBlue ;       
			extern const ColorDefinition RoyalBlue ;      
			extern const ColorDefinition SkyBlue ;        
			extern const ColorDefinition SlateBlue ;      
			extern const ColorDefinition SteelBlue ;      
			extern const ColorDefinition Aquamarine ;     
			extern const ColorDefinition Azure ;          
			extern const ColorDefinition Blue ;           
			extern const ColorDefinition Cyan ; 	      
			extern const ColorDefinition Turquoise ;      
			extern const ColorDefinition MidnightBlue ;      



			/// Shades of Brown :

			extern const ColorDefinition Brown ;      
			extern const ColorDefinition RosyBrown ;      
			extern const ColorDefinition SaddleBrown ;    
			extern const ColorDefinition Beige ;          
			extern const ColorDefinition Burlywood ;      
			extern const ColorDefinition Chocolate ;      
			extern const ColorDefinition Peru ;           
			extern const ColorDefinition Tan ;            
			extern const ColorDefinition Copper ;         



			/// Shades of Green :

			extern const ColorDefinition DarkGreen ;      
			extern const ColorDefinition DarkOliveGreen ; 
			extern const ColorDefinition ForestGreen ;    
			extern const ColorDefinition GreenYellow  ;   
			extern const ColorDefinition LawnGreen ;      
			extern const ColorDefinition LimeGreen ;      
			extern const ColorDefinition MintCream ;     
			extern const ColorDefinition OliveDrab ;      
			extern const ColorDefinition PaleGreen ;      
			extern const ColorDefinition SeaGreen ;       
			extern const ColorDefinition SpringGreen ;    
			extern const ColorDefinition YellowGreen ;    
			extern const ColorDefinition Chartreuse ;     
			extern const ColorDefinition Green ;          
			extern const ColorDefinition Khaki ;          



			/// Shades of Orange :

			extern const ColorDefinition DarkOrange ;     
			extern const ColorDefinition DarkSalmon ;     
			extern const ColorDefinition LightCoral ;     
			extern const ColorDefinition LightSalmon ;    
			extern const ColorDefinition PeachPuff ;      
			extern const ColorDefinition Bisque ;         
			extern const ColorDefinition Coral ;          
			extern const ColorDefinition Honeydew ;       
			extern const ColorDefinition Orange ;         
			extern const ColorDefinition Salmon ;         
			extern const ColorDefinition Sienna ;         



			/// Shades of Red :

			extern const ColorDefinition DeepPink ;       
			extern const ColorDefinition HotPink ;       
			extern const ColorDefinition IndianRed ;      
			extern const ColorDefinition LightPink ;      
			extern const ColorDefinition MediumVioletRed ;
			extern const ColorDefinition MistyRose ;      
			extern const ColorDefinition OrangeRed ;      
			extern const ColorDefinition VioletRed ;      
			extern const ColorDefinition Firebrick ;      
			extern const ColorDefinition Pink ;           
			extern const ColorDefinition Red ;            
			extern const ColorDefinition Tomato ;         



			/// Shades of Violet :

			extern const ColorDefinition DarkOrchid ;     
			extern const ColorDefinition DarkViolet ;     
			extern const ColorDefinition LavenderBlush ;  
			extern const ColorDefinition MediumOrchid ;   
			extern const ColorDefinition MediumPurple ;   
			extern const ColorDefinition Lavender ;       
			extern const ColorDefinition Magenta ;        
			extern const ColorDefinition Maroon ;         
			extern const ColorDefinition Orchid ;         
			extern const ColorDefinition Plum ;           
			extern const ColorDefinition Purple ;         
			extern const ColorDefinition Thistle ;        
			extern const ColorDefinition Violet ;         



			/// Shades of White :

			extern const ColorDefinition AntiqueWhite ;   
			extern const ColorDefinition FloralWhite ;    
			extern const ColorDefinition GhostWhite ;     
			extern const ColorDefinition NavajoWhite ;    
			extern const ColorDefinition OldLace ;        
			extern const ColorDefinition WhiteSmoke ;     
			extern const ColorDefinition Gainsboro ;      
			extern const ColorDefinition Ivory ;          
			extern const ColorDefinition Linen ;          
			extern const ColorDefinition Seashell ;       
			extern const ColorDefinition Snow ;          
			extern const ColorDefinition Wheat ;          
			extern const ColorDefinition White ;          



			/// Shades of Yellow :

			extern const ColorDefinition BlanchedAlmond ; 
			extern const ColorDefinition DarkGoldenrod ;  
			extern const ColorDefinition LemonChiffon ;   
			extern const ColorDefinition LightGoldenrod ; 
			extern const ColorDefinition LightYellow ;    
			extern const ColorDefinition PaleGoldenrod ;  
			extern const ColorDefinition PapayaWhip ;   
			extern const ColorDefinition Cornsilk ;       
			extern const ColorDefinition Gold ;           
			extern const ColorDefinition Goldenrod ;      
			extern const ColorDefinition Moccasin ;       
			extern const ColorDefinition Yellow ;         
		
		
		
		}
	
	}

}


#endif // OSDL_PIXEL_H_

