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


#ifndef OSDL_TRUETYPE_FONT_H_
#define OSDL_TRUETYPE_FONT_H_



#include "OSDLFont.h"         // for inheritance



#include <string>

#if ! defined(OSDL_USES_SDL_TTF) || OSDL_USES_SDL_TTF 

/*
 * No need to include SDL_ttf header here.
 *
 * 'struct TTF_Font' could not be forward-declared, as SDL_ttf.h defines 
 * it as 'typedef struct _TTF_Font TTF_Font;' which leads to the error
 * "'struct TTF_Font' has a previous declaration as 'struct TTF_Font'",
 * as soon as an implementation file includes SDL_ttf.h.
 * So we use _TTF_Font instead of TTF_Font.
 *
 */
struct _TTF_Font ;

#endif // OSDL_USES_SDL_TTF





namespace OSDL
{



	namespace Video
	{



		// Fonts are rendered to surfaces.
		class Surface ;




		namespace TwoDimensional
		{
			
						
			
			namespace Text
			{	
			


				// The internal actual font object.
				#if ! defined(OSDL_USES_SDL_TTF) || OSDL_USES_SDL_TTF 

				typedef ::_TTF_Font LowLevelTTFFont ;
				
				#else // OSDL_USES_SDL_TTF

				struct LowLevelTTFFont {} ;
				
				#endif // OSDL_USES_SDL_TTF
				


				/**
				 * Truetype font.
				 *
				 * A font can contain faces, which are specific sub-fonts, 
				 * with particular size and style.
				 *
				 * These fonts can be drawn with different qualities.
				 *
				 * Latin-1 is an extension of ASCII, where ASCII only defines
				 * characters 0 through 127. 
				 * Latin-1 continues and adds more common international 
				 * symbols to define through character 255.
				 *
				 * Plain ASCII is UTF-8 compatible.
				 *
				 * The principle of these rendering methods is to return a
				 * surface with the chosen glyph, word or full text drawn 
				 * with the specified color, with no visible background: 
				 * thanks to color key or alpha-blending, only the text can 
				 * be seen so that this returned surface can be directly 
				 * blitted onto any already existing background that will 
				 * be hidden only by the pixels corresponding to the text.
				 * 
				 * Words and texts are rendered so that all glyphs lie on a
				 * common baseline, but glyphs are rendered in individual
				 * surfaces of different height. 
				 * Hence drawing a text is more difficult than just blitting 
				 * all glyphs at the same height, and usually applications 
				 * do not use direct glyph rendering: most of the time
				 * words or texts are rendered as a whole.
				 *
				 * As a returned surface is encoded with a RLE-color key 
				 * and/or can be hardware, it is especially important to 
				 * lock this surface before reading and/or writing
				 * its pixels, and to unlock it afterwards.
				 *
				 * This returned surface is to be owned by the caller of 
				 * the render method. 
				 * This caller will have to deallocate it when the surface 
				 * will not be of use anymore.
				 *
				 * Rendering should only occur after the video has been
				 * initialized (setMode).
				 *
				 * @see http://jcatki.no-ip.org/SDL_ttf/metrics.png for
				 * informations about glyph metrics.
				 *
				 * @note Most lenghts are given alongside a directed axis, 
				 * and therefore they can be negative.
				 *
				 * @note There exist some Truetype fonts that are actually
				 * 'fixed width'. 
				 * They are supported by this class, even if the special 
				 * case of fixed font could be managed thanks to the 
				 * FixedFont class too. 
				 *
				 */	
				class OSDL_DLL TrueTypeFont: public Font
				{
				
				
					public:
					
					
					
						
						/**
						 * Creates a new Truetype font.
						 *
						 * @param fontFilename the filename of the file where
						 * the font is stored, usually a .ttf, .TTF or a .FON
						 * file. 
						 * The filename will be interpreted first 'as is', 
						 * i.e. as an absolute path or a relative path to the 
						 * current directory. 
						 * If it does not succeed, the font file will be 
						 * searched through directories listed in the
						 * Truetype file locator.
						 * If it fails again, the list of directories 
						 * specified in the FONT_PATH environment variable will
						 * be scanned for that filename, if any.
						 *
						 * @param pointSize the size of a point, based on 72
						 * dots per inch. 
						 * This basically translates to pixel height.
						 *
						 * @param index the index of this font in specified
						 * file.
						 *
						 * @param convertToDisplay tells whether a returned 
						 * or cached rendering should have already been
						 * converted to display.
						 *
						 * @param cacheSettings determines which renderings
						 * should be cached. 
						 * The recommended - and default - setting is
						 * 'GlyphCached'.
						 *
						 * @throw TextException if the file could not be 
						 * found or if the font library did not initialize
						 * properly.
						 *
						 * @note This constructor can operate on all kinds of
						 * files transparently, including archive-embedded ones.
						 *
						 */
						TrueTypeFont( 
							const std::string & fontFilename, 
							PointSize pointSize,
							FontIndex index = 0, 
							bool convertToDisplay = true,
							RenderCache cacheSettings = GlyphCached ) ;
						
						
						
						/**
						 * Font virtual destructor.
						 *
						 */
						virtual ~TrueTypeFont() throw() ;
						
						
						
						/**
						 * Returns the point size of this font, expressed 
						 * in dots per inch.
						 *
						 */
						virtual PointSize getPointSize() const ;
						


						/**
						 * Returns the current rendering style.
						 *
						 */
						virtual RenderingStyle getRenderingStyle() const ;
						
						
						
						/**
						 * Sets the current rendering style for this font.
						 *
						 * @note Combining Font::Underline with anything can
						 * cause a segfault, other combinations may also 
						 * do this (due to the SDL_ttf/Freetype backend).
						 *
						 * @note Changing the font style flushes the internal
						 * cache of previously rendered glyphs.
						 *
						 * @param newStyle a bitmask of the desired 
						 * rendering styles. 
						 *
						 * @example myFont.setRenderingStyle( Font::Italic 
						 * | Font:Bold ) ;
						 *
						 * @throw TextException if the specified style 
						 * is not supported.
						 *
						 */
						virtual void setRenderingStyle( 
							RenderingStyle newStyle ) ;




						// Glyph-dependent methods.		
						
						
						
						/**
						 * Returns the width of the specified glyph, 
						 * rendered with this font. 
						 *
						 * @param character the character whose width will 
						 * be returned.
						 *
						 * @note There is no argument-less getWidth since 
						 * for most fonts it depends too heavily on the 
						 * selected glyph.
						 *
						 * @throw TextException if the glyph metrics could
						 * not be retrieved.
						 *
						 */
						virtual Width getWidth( Ceylan::Latin1Char character ) 
							const ;
								
							
								
						/**
						 * Returns the width of the abscissa offset for 
						 * the specified glyph, rendered with this font. 
						 *
						 * This offset corresponds to the first abscissa 
						 * from which the glyph should be rendered from 
						 * a given location.
						 *
						 * @param character the character whose abscissa 
						 * offset will be returned.
						 *
						 * @throw TextException if the glyph metrics could 
						 * not be retrieved.
						 *
						 */
						virtual SignedWidth getWidthOffset( 
							Ceylan::Latin1Char character ) const ;
							
								
								
						/**
						 * Returns the height above baseline of the 
						 * specified glyph, rendered with this font. 
						 *
						 * @param character the character whose height 
						 * above baseline will be returned.
						 *
						 * @throw TextException if the glyph metrics 
						 * could not be retrieved.
						 *
						 */
						virtual SignedHeight getHeightAboveBaseline(
							Ceylan::Latin1Char character ) const ;
								
						 
						 
						/**
						 * Returns the advance of the specified glyph, 
						 * rendered with this font, which is the space 
						 * between the leftmost part of the glyph bounding 
						 * box (actually, the origin in the glyph referential)
						 * and the same part of the next glyph. 
						 * It includes therefore the width of this glyph 
						 * and the space between the next glyph that could 
						 * be rendered afterwards. 
						 *
						 * @param character the character whose advance 
						 * will be returned.
						 *
						 * @throw TextException if the glyph metrics could 
						 * not be retrieved.
						 *
						 */
						virtual SignedLength getAdvance( 
							Ceylan::Latin1Char character ) const ;
						
						
						
						
						// Glyph-independent methods.		
							
												
												
						/**
						 * Returns the maximum pixel height of all glyphs 
						 * of this font. 
						 *
						 * You may use this height for rendering text as 
						 * close together vertically as possible, though 
						 * adding at least one pixel height to it will space 
						 * it, so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing, 
						 * see getLineSkip as well.
						 *
						 * @see getLineSkip
						 *
						 */
						virtual Height getHeight() const ;
						 	
												
						
						/**
						 * Returns the maximum pixel ascent (height above
						 * baseline) of all glyphs of this font.
						 *
						 * You may use this height for rendering text as 
						 * close together vertically as possible, though 
						 * adding at least one pixel height to it will 
						 * space it, so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing, 
						 * see getLineSkip as well.
						 *
						 */
						virtual SignedHeight getAscent() const ;
						 
						 
						 
						/**
						 * Returns the maximum pixel descent (height below
						 * baseline) of all glyphs of this font. 
						 *
						 * If at least a glyph has parts below the abscissa
						 * axis, the descent is negative.
						 *
						 * It could be used when drawing an individual glyph
						 * relative to a bottom point, by combining it with 
						 * the glyph's maxy metric to resolve the top of the
						 * rectangle used when blitting the glyph on the 
						 * screen.
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing, 
						 * see getLineSkip as well.
						 *
						 */
						virtual SignedHeight getDescent() const ;
						 
						 
						 
						/**
						 * Returns the recommended pixel height of a 
						 * rendered line of text of this font. 
						 *
						 * This is usually larger than the value returned 
						 * by getHeight.
						 *
						 */
						virtual Height getLineSkip() const ;
						
						
						
						/**
						 * Returns the number of faces ("sub-fonts") 
						 * available within this font.
						 *
						 * This is a count of the number of specific fonts
						 * contained in this font.
						 *
						 * @note For the moment, no specific use of this
						 * information can be done through the library.
						 *
						 */
						virtual Ceylan::Uint16 getFacesCount() const ; 
						
						
						
						/**
						 * Returns whether the current font face of this 
						 * font is a fixed width font.
						 *
						 * Fixed width fonts are monospace, meaning all
						 * characters that exist in the font have the same 
						 * width, thus you can assume that a rendered 
						 * string's width is going to be the result of a 
						 * simple calculation:
						 * <code>stringWidth = glyphWidth * stringLength</code>.
						 *
						 */
						virtual bool isFixedWidth() const ;
						
						
						  
						/**
						 * Returns the family name of the current face.
						 *
						 */
						virtual std::string getFaceFamilyName() const ;
						
						
				 		
						/**
						 * Returns the style name of the current face.
						 *
						 */
						virtual std::string getFaceStyleName() const ;
						
						
						
						
						
						// Bounding boxes section.
						
						
						
						/**
						 * Returns the metrics of a glyph specified by its
						 * Unicode character.
						 * 
						 * @param glyph the glyph whose metrics are to be
						 * returned.
						 *
						 * @param advance will be set to the advance of 
						 * this glyph.
						 *
						 * The metrics are described as an upright 
						 * rectangle which contains the glyph, defined from
						 * its upper left corner (xMin, yMax) to its lower
						 * right corner, (xMax, yMin) as shown in:
						 * http://jcatki.no-ip.org/SDL_ttf/metrics.png
						 *
						 * @return an upright rectangle enclosing the glyph
						 * (bounding box).
						 *
						 * The caller gets ownership of this returned 
						 * rectangle, it has therefore to delete it when 
						 * the rectange is not to be used any more.
						 *
						 * @throw TextException on error, for example if 
						 * the specified character does not exist in this font.
						 *
						 * Width is xMax - xMin.
						 * Height is yMax - yMin.
						 * BearingX is xMin
						 * BearingY is yMax.
						 *
						 */
						virtual UprightRectangle & getBoundingBoxFor(
								Ceylan::Unicode glyph, SignedLength & advance )
							const ;
							
							
						 
						/**
						 * Returns a rectangular bounding box corresponding 
						 * to the rendering of specified text, encoded in
						 * Latin-1. 
						 *
						 * Correct kerning is done to get the actual width. 
						 * The height returned is the same as you can get 
						 * using the getHeight method.
						 *
						 * The upright rectangle has its lower left corner 
						 * set to the origin. 
						 *
						 * The caller gets ownership of this returned 
						 * rectangle, it has therefore to delete it when 
						 * the rectange is not to be used any more.
						 *
						 * @param text the Latin-1 encoded string to size up.
						 *
						 * @return an upright rectangle enclosing the 
						 * text (bounding box), whose lower left corner is
						 * located at the origin.
						 *
						 * @throw TextException if an error occured, for 
						 * example if a requested glyph does not exist.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBoxFor( 
							const std::string & text ) const ;
							 
							 
							 
						/**
						 * Returns a rectangular bounding box corresponding 
						 * to the rendering of specified text, encoded 
						 * in UTF-8. 
						 *
						 * Correct kerning is done to get the actual width. 
						 * The height returned is the same as you can get 
						 * using the getHeight method.
						 *
						 * The upright rectangle has its lower left corner 
						 * set to the origin. 
						 *
						 * The caller gets ownership of this returned 
						 * rectangle, it has therefore to delete it when 
						 * the rectange is not to be used any more.
						 *
						 * @param text the UTF-8 encoded null terminated 
						 * string to size up.
						 *
						 * @return an upright rectangle enclosing the text
						 * (bounding box), whose lower left corner is 
						 * located at the origin.
						 *
						 * @throw TextException if an error occured, 
						 * for example if a requested glyph does not exist.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBoxForUTF8( 
							const std::string & text ) const ;
							 
							 
							 
						/**
						 * Returns a rectangular bounding box corresponding 
						 * to the rendering of specified text, encoded in
						 * Unicode. 
						 *
						 * Correct kerning is done to get the actual width. 
						 * The height returned is the same as you can get 
						 * using the getHeight method.
						 *
						 * The upright rectangle has its lower left corner 
						 * set to the origin. 
						 *
						 * The caller gets ownership of this returned 
						 * rectangle, it has therefore to delete it when 
						 * the rectange is not to be used any more.
						 *
						 * @param text the Unicode encoded null terminated
						 * string to size up.
						 *
						 * @return an upright rectangle enclosing the text
						 * (bounding box), whose lower left corner is located 
						 * at the origin.
						 *
						 * @throw TextException if an error occured, for 
						 * example if a requested glyph does not exist.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBoxForUnicode( 
							const Ceylan::Unicode * text ) const ;
							 
							 
							 
						
						// Render section.



						/**
						 * Renders specified glyph (Latin-1 character) in
						 * specified color, in a returned surface.
						 *
						 * Depending on the settings, the returned surface 
						 * can be a clone of a pre-rendered (cached) 
						 * character or, if not available, be put in cache
						 * itself so that it is rendered once. 
						 *
						 * Otherwise, if no cache feature is allowed, 
						 * each character will be rendered as many times as
						 * requested.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will 
						 * be used.
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual Surface & renderLatin1Glyph( 
								Ceylan::Latin1Char character, 
								RenderQuality quality = Solid, 
								Pixels::ColorDefinition glyphColor 
									= Pixels::White ) ;
							 
							 

						/**
						 * Blits specified glyph (Latin-1 character) in
						 * specified color, on specified location of given
						 * surface.
						 *
						 * @param targetSurface the surface the glyph will 
						 * be blitted to.
						 *
						 * @param x the abscissa in target surface of the
						 * top-left corner of the glyph blit.
						 *
						 * @param y the ordinate in target surface of the
						 * top-left corner of the glyph blit.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will 
						 * be used.
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1Glyph( 
								Surface & targetSurface,
								Coordinate x, Coordinate y, 
								Ceylan::Latin1Char character, 
								RenderQuality quality = Solid, 
								Pixels::ColorDefinition glyphColor 
									= Pixels::White ) ;
							 
							 
							 
						/**
						 * Renders specified glyph, specified as a
						 * Unicode-encoded character, in specified color, 
						 * in a returned surface. 
						 *
						 * Unicode management is available with Truetype 
						 * fonts, but may be not for other types of fonts.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param character the character, encoded in Unicode,
						 * to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will be
						 * used.
						 *
						 * @param textColor the color definition for the 
						 * glyph. 
						 * This becomes first index of colormap, in Solid 
						 * and Shaded qualities.
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller.
						 *
						 * @throw TextException on error, for example if 
						 * the specified glyph was not found.
						 *
						 */
						virtual Surface & renderUnicodeGlyph( 
								Ceylan::Unicode character, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White ) ;
						
						
						
						/**
						 * Renders specified text, encoded in Latin-1, in
						 * specified color, in a returned surface. 
						 *
						 * The caller is responsible for deleting the returned
						 * surface.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param text the text, encoded in Latin-1, to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will be
						 * used.
						 *
						 * @param textColor the color definition for the text.
						 * This becomes first index of colormap, in Solid 
						 * and Shaded qualities.
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error, for example if 
						 * a specified glyph was not found.
						 *
						 */
						virtual Surface & renderLatin1Text( 
								const std::string & text, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White ) ;
							
							
						
						/**
						 * Renders specified text, encoded in UTF-8, in
						 * specified color, in a returned surface. 
						 *
						 * The caller is responsible for deleting the returned
						 * surface.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param text the text, encoded in UTF-8, to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will be
						 * used.
						 *
						 * @param textColor the color definition for the 
						 * text. 
						 * This becomes first index of colormap, in Solid 
						 * and Shaded qualities.
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error, for example if a
						 * specified glyph was not found.
						 *
						 */
						virtual Surface & renderUTF8Text( 
								const std::string & text, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White ) ;
							
							
						
						/**
						 * Renders specified text, encoded in Unicode, in
						 * specified color, in a returned surface. 
						 *
						 * The caller is responsible for deleting the returned
						 * surface.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param text the text, encoded in Unicode, 
						 * to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * If 'Shaded', the current background color will 
						 * be used.
						 *
						 * @param textColor the color definition for the 
						 * text. 
						 * This becomes first index of colormap, in Solid 
						 * and Shaded qualities.
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error, for example if a
						 * specified glyph was not found.
						 *
						 */
						virtual Surface & renderUnicodeText( 
								const Ceylan::Unicode * text, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::Black ) ;
							
							
														
							
			            /**
	    		         * Returns an user-friendly description of the 
						 * state of this object.
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
							const ;




	
						// Static section.
						
						
						
						/**
						 * Returns which quality would be used if specified
						 * rendering quality was requested.
						 *
						 */
						static RenderQuality GetObtainedQualityFor(
							RenderQuality targetedQuality ) ;

					
					
						/**
						 * Determines whether Unicode data is to be swapped
						 * relative to the CPU endianness.
						 *
						 * @param newStatus if true, then Unicode data is 
						 * byte swapped relative to the CPU's native 
						 * endianness. 
						 * 
						 * If false, does not swap Unicode data, uses the
						 * CPU's native endianness.
						 *
						 * A Unicode_BOM_NATIVE or Unicode_BOM_SWAPPED 
						 * character in a string will temporarily override 
						 * this setting for the remainder of that string,
						 * however this setting will be restored for the 
						 * next one. 
						 *
						 * The default mode is non-swapped, native endianness 
						 * of the CPU.
						 *
						 */
						void SetUnicodeSwapStatus( bool newStatus ) ;
				
				
				
						/**
						 * Returns a textual description of the last error 
						 * that occured relatively to Truetype fonts.
						 *
						 */	
						static std::string DescribeLastError() ;
						
	
	
						/**
						 * Allows to keep track of Truetype font directories.
						 *
						 */
						static Ceylan::System::FileLocator
							TrueTypeFontFileLocator ;



						/// Recommended extension of TrueType font files.
						static std::string TrueTypeFontFileExtension ;
						
						
						
						/**
						 * The multiplying factor to apply to the font space
						 * width to compute the actual space width.
						 *
						 */
						static const Ceylan::Float32 SpaceWidthFactor ;
							
												
							
												
						
					protected:
					


						/**
						 * Renders specified glyph (Unicode character) in
						 * specified color, on a new surface, directly 
						 * thanks to the font backend: no cache is taken 
						 * into account. 
						 *
						 * Colorkeys and conversion to display are however
						 * enforced.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 * @param character the character to render.
						 *
						 * @param quality the specified rendering quality.
						 *
						 * @param glyphColor the requested glyph color.
						 *
						 */
						virtual Surface & basicRenderUnicodeGlyph(
							Ceylan::Unicode character,
							RenderQuality quality, 
							Pixels::ColorDefinition glyphColor ) ;
	
	
	
						
						// Font filename is not kept currently.
						
						
						/// The size of a point, based on 72 dots per inch
						PointSize _pointSize ;
						
						
						/// Stores the actual TTF font.
						LowLevelTTFFont * _actualFont ;
						
						
						/**
						 * Records how many fonts are currently on use, 
						 * to perform back-end clean-up as soon as no 
						 * font is in use anymore.
						 *
						 */
						static Ceylan::Uint32 FontCounter ;
					
					
					
					
					private:
					
				
									
						/**
						 * Copy constructor made private to ensure that 
						 * it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined constructor is called, implicitly or not.
						 *
						 */			 
						TrueTypeFont( const TrueTypeFont & source ) ;
			
			
			
						/**
						 * Assignment operator made private to ensure 
						 * that it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined operator is called, implicitly or not.
						 *
						 */			 
						TrueTypeFont & operator = ( 
							const TrueTypeFont & source ) ;
							
				
				} ;
				
				
			} 
			
		}	
			
	}
	
}		



#endif // OSDL_TRUETYPE_FONT_H_

