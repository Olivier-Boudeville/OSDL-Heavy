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


#ifndef OSDL_FIXED_FONT_H_
#define OSDL_FIXED_FONT_H_



#include "OSDLFont.h"         // for inheritance

#include "Ceylan.h"           // for Ceylan::Byte, Height

#include <string>





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
			
			

				/// The character width of fixed basic font.
				extern OSDL_DLL const Length BasicFontCharacterWidth ;
				
				
				/// The character height of fixed basic font.
				extern OSDL_DLL const Length BasicFontCharacterHeight ;
				
				
				
				/**
				 * Prints specified string on target surface, with specified
				 * color, taking as upper left corner specified coordinates.
				 * Basic 8x8 bitmap font will be used.
				 *
				 * Not all accentuated characters are supported by the
				 * built-in font. 
				 * For example, 'à', 'â', etc. are replaced by greek 
				 * characters.
				 *
				 * @note The printBasic methods are very lightweight, since 
				 * they rely onto built-in basic fonts: no need to 
				 * instanciate a FixedFont class to use them.
				 *
				 * @throw VideoException if the operation is not supported.
				 *
				 */
				OSDL_DLL bool printBasic( 
					const std::string & text, 
					Surface & targetSurface, Coordinate x, Coordinate y,
					Pixels::ColorElement red, Pixels::ColorElement blue,
					Pixels::ColorElement green, 
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ; 
				
				
				
				/**
				 * Prints specified string on target surface, with specified
				 * color, taking as upper left corner specified coordinates.
				 * Basic 8x8 bitmap font will be used.
				 *
				 * Not all accentuated characters are supported by the
				 * built-in font. 
				 * For example, 'à', 'â', etc. are replaced by greek 
				 * characters.
				 *
				 * @note The printBasic methods are very lightweight, since 
				 * they rely onto built-in basic fonts: no need to 
				 * instanciate a FixedFont class to use them.
				 *
				 * @throw VideoException if the operation is not supported.
				 *
				 */
				OSDL_DLL bool printBasic( 
					const std::string & text, 
					Surface & targetSurface,
					Coordinate x, Coordinate y, 
					Pixels::ColorDefinition colorDef ) ;
					
					


				/**
				 * Models fixed fonts, a category which gathers all fonts 
				 * whose rendering demands little resources, but cannot 
				 * be scaled, as opposed to Truetype fonts.
				 *
				 * Fixed fonts usually have a character set including at 
				 * least all alphanumerical characters, including capital
				 * letters, plus most special characters. 
				 * They should provide at least a full support for characters
				 * encoded according to Latin-1.
				 *
				 * For these fonts, the baseline is chosen to be at the 
				 * very bottom of the glyph: no glyph goes below the 
				 * baseline.
				 *
				 * These fonts are loaded from font files, whose extension 
				 * is usually '.fnt'.
				 * The font files are searched first 'as specified', using
				 * directly the specified filename (which may be absolute 
				 * or relative to the current directory), then using the 
				 * fixed font locator, then the overall font locator.
				 * 
				 * The principle of these rendering methods is to return a
				 * surface with the chosen glyph(s) drawn with the specified
				 * color, with no visible background: thanks to color key 
				 * or alpha-blending, only the text can be seen, so that 
				 * this returned surface can be directly blitted onto any
				 * already existing background that will by hidden only by
				 * the pixels corresponding to the text.
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
				 * Only the blit and render methods for glyphs are
				 * declared and defined in this class, since their 
				 * counterparts for words and texts are built upon them 
				 * and already available through inheritance (Font mother
				 * class).
				 *
				 * Font rendering should only occur after the video has been
				 * initialized (setMode).
				 *
				 * @see http://jcatki.no-ip.org/SDL_ttf/metrics.png for
				 * informations about glyph metrics.
				 *
				 * @note Most lenghts are given alongside a directed axis, 
				 * and therefore they can be negative.
				 *
				 */	
				class OSDL_DLL FixedFont: public Font
				{
				
				
				
					public:
						
							
											
						
						/**
						 * Creates, if possible, a Fixed font having 
						 * specified features, by looking up a corresponding
						 * font file.
						 *
						 * These fonts are deemed lightweight compared to
						 * Truetype fonts.
						 *
						 * @param characterWidth the desired width of a
						 * character.
						 *
						 * @param characterHeight the desired height of a
						 * character.
						 *
						 * @param renderingStyle the desired rendering style,
						 * which can be only one of the listed rendering 
						 * styles (no combination of attributes allowed).
						 *
						 * @param convertToDisplay tells whether a returned 
						 * or cached rendering should have already been
						 * converted to display. 
						 * Useful for classical 2D rendering, useless for 
						 * OpenGL since surfaces will have to be converted
						 * nonetheless.
						 *
						 * @param cacheSettings determines which renderings
						 * should be cached. 
						 * The recommended - and default - setting is
						 * 'GlyphCached', since with fixed fonts words can 
						 * be easily drawn from prerendered glyphs.
						 *
						 * @param cachePolicy determines how the cache should
						 * behave regarding renderings being put in cache 
						 * (store everything, enforce a maximum size taken 
						 * in memory, etc.)
						 *
						 * @param quota Should an upper-bound to the memory 
						 * size of cached renderings apply (depending on the
						 * chosen cache policy), determines its actual value. 
						 * If a null (0) quota is passed, then the default 
						 * value of the quota for the selected cache settings
						 * will be chosen. 
						 * For example, if cacheSettings is 'WordCached' 
						 * and cachePolicy is 'DropLessRequestedFirst' 
						 * (this policy uses a quota), and if a null quota 
						 * is specified, then the actual quota being used 
						 * will be 'DefaultWordCachedQuota'.
						 *
						 * There are 21 most common fixed fonts:
						 * (descriptions must be read as: 
						 * 'width x height: available styles', styles
						 * cannot be combined):
						 *	- 5x7  : Normal
						 *	- 5x8  : Normal
						 *	- 6x9  : Normal
						 *	- 6x10 : Normal
						 *	- 6x12 : Normal
						 *	- 6x13 : Normal, or Bold, or Italic
						 *	- 7x13 : Normal, or Bold, or Italic
						 *	- 7x14 : Normal, or Bold
						 *	- 8x13 : Normal, or Bold, or Italic
						 *	- 9x15 : Normal, or Bold
						 *	- 9x18 : Normal, or Bold
						 *  - 10x20: Normal
						 *
						 * Rendering styles encoding is:
						 *   - 'B' for Bold
						 *   - 'O' for Italic (sorry for the back-end poor
						 * choice)
						 *	 - 'U' for Underline
						 *
						 * The filename of a font file describes the font, 
						 * its form is:
						 * 'width'x'height'[rendering style]'.fnt'. 
						 *
						 * @example '10x20.fnt' is a normal 10x20 font, whereas
						 * '8x13B.fnt' is a 8x13 bold font and '8x13O.fnt' is
						 * font without bold but with italic. 
						 * Rendering style mixing (ex: 'BO') is not allowed, 
						 * a font has at most one rendering style.
						 *
						 * @throw FontException if no corresponding font 
						 * file could be found, or if cache setting is not
						 * known, etc.
						 *
						 * The font file will be searched from the fixed 
						 * font locator first and, on failure, from the 
						 * overall font locator. 
						 *
						 */
						FixedFont( 
							Length characterWidth, 
							Length characterHeight, 
							RenderingStyle renderingStyle = Normal,
							bool convertToDisplay = true,
							RenderCache cacheSettings = GlyphCached,
							AllowedCachePolicy cachePolicy
								= DropLessRequestedFirst,
							Ceylan::System::Size quota = 0 ) ;
						 
						 
						 
						/**
						 * Creates a new Fixed font from specified font file.
						 *
						 * @param fontFilename the filename of the file 
						 * where the font is stored, usually a .fnt file. 
						 * These fonts are loaded from font files, whose
						 * extension  is usually '.fnt'.
						 * The font files are searched first 'as specified',
						 * using directly the specified filename (which may 
						 * be absolute or relative to the current directory),
						 * then using the fixed font locator, then the overall
						 * font locator.
						 *
						 * @param renderingStyle the desired rendering style,
						 * which can be only one of the listed rendering 
						 * styles (no combination of attributes allowed).
						 *
						 * @param convertToDisplay tells whether a returned 
						 * or cached rendering should have already been
						 * converted to display. 
						 * Useful for classical 2D rendering, useless for 
						 * OpenGL since surfaces will have to be converted
						 * nonetheless.
						 *
						 * @param cacheSettings determines which renderings
						 * should be cached. 
						 * The recommended - and default - setting is
						 * 'GlyphCached', since with fixed fonts words can 
						 * be easily drawn from prerendered glyphs.
						 *
						 * @param cachePolicy determines how the cache should
						 * behave regarding renderings being put in cache 
						 * (store everything, enforce a maximum size taken 
						 * in memory, etc.)
						 *
						 * @param quota Should an upper-bound to the memory 
						 * size of cached renderings apply (depending on the
						 * chosen cache policy), determines its actual value. 
						 * If a null (0) quota is passed, then the default 
						 * value of the quota for the selected cache settings
						 * will be chosen. 
						 * For example, if cacheSettings is 'WordCached' 
						 * and cachePolicy is 'DropLessRequestedFirst' 
						 * (this policy uses a quota), and if a null quota 
						 * is specified, then the actual quota being used 
						 * will be 'DefaultWordCachedQuota'.
						 *
						 * @throw FontException if no corresponding font file
						 * could be found, or if cache setting is not known,
						 * etc.
						 *
						 * @note The rendering style and the dimensions are
						 * deduced from the font file name.
						 *
						 */
						explicit FixedFont( 
							const std::string & fontFilename, 
							bool convertToDisplay = true,
							RenderCache cacheSettings = GlyphCached,
							AllowedCachePolicy cachePolicy 
								= DropLessRequestedFirst,
							Ceylan::System::Size quota = 0 ) ;
						
						
						
						/**
						 * Fixed font virtual destructor.
						 *
						 */
						virtual ~FixedFont() throw() ;
								
												
									 
						/**
						 * Returns the maximum pixel width of all glyphs of 
						 * this font. 
						 *
						 * You may use this width for rendering text as 
						 * close together horizontally as possible.
						 * At least one pixel should be added, so that glyphs
						 * cannot touch. 
						 *
						 * @note Width is constant in this fixed font special
						 * case.
						 *
						 */
						virtual Width getWidth() const ;



						/**
						 * Returns the width of the specified glyph, 
						 * rendered with this font. 
						 *
						 * @param character the character whose width will 
						 * be returned.
						 *
						 * Remember that multiline printing is not enabled
						 * here, so the user is responsible for line breaking.
						 *
						 * @note For fixed font, width does not depend on 
						 * the specific character being chosen.
						 *
						 * @see getAdvance
						 *
						 */
						virtual Width getWidth( Ceylan::Latin1Char character )
							const ;



						/**
						 * Returns the width of the abscissa offset for the
						 * specified glyph, rendered with this font. 
						 *
						 * This offset corresponds to the first abscissa 
						 * from which the glyph should be rendered from a
						 * given location.
						 *
						 * @param character the character whose abscissa 
						 * offset will be returned.
						 *
						 * Remember that multiline printing is not enabled
						 * here, so the user is responsible for line breaking.
						 *
						 * @throw FontException if the glyph metrics could 
						 * not be retrieved.
						 *
						 * @note For fixed font, offset width does not depend 
						 * on the specific character chosen, and is null, as
						 * already taken into account into character width.
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
						 * @throw FontException if the glyph metrics could 
						 * not be retrieved.
						 *
						 * @note For fixed font, height above baseline does 
						 * not depend on the specific character being chosen,
						 * and is equal to the height of this fixed font.
						 *
						 */
						virtual SignedHeight getHeightAboveBaseline(
							Ceylan::Latin1Char character ) const ;



						/**
						 * Returns the advance (recommended width containing
						 * a glyph, so that they can be chained in a word),
						 * which with fixed fonts does not depend on 
						 * the specific character being rendered.
						 *
						 * This method is not inherited from Font.
						 *
						 * @throw FontException if the glyph metrics could 
						 * not be retrieved.
						 *
						 */
						virtual SignedLength getAdvance() const ;
						
						
						
						/**
						 * Returns the advance (recommended width containing
						 * a glyph, so that they can be chained in a word),
						 * which with fixed fonts does not depend on 
						 * the specific character being rendered.
						 *
						 * @param character any character has the same advance.
						 *
						 * @throw FontException if the glyph metrics could 
						 * not be retrieved.
						 *
						 * @note This method exists only because the 
						 * interface demands it.
						 *
						 * @see getAdvance()
						 *
						 */
						virtual SignedLength getAdvance( 
							Ceylan::Latin1Char character ) const ;



						/**
						 * Returns the width of the recommended space 
						 * between two glyphs.
						 *
						 * Simple text rendering can rely on constant space
						 * between glyphs, but for more complex cases such as
						 * multi-line printing, more complex rules should
						 * be used.
						 *
						 * The default value is one pixel.
						 *
						 */
						virtual Width getInterGlyphWidth() const ;
				
						 
						 
						/**
						 * Returns the maximum pixel height of all glyphs 
						 * of this font. 
						 *
						 * You may use this height for rendering text as 
						 * close together vertically as possible, though 
						 * adding at least one pixel height to this value
						 * will space glyphs so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled
						 * here, so the user is responsible for line spacing,
						 * see getLineSkip as well.
						 *
						 */
						virtual Text::Height getHeight() const ;
						 						 
												 
						 
						/**
						 * Returns the maximum pixel ascent (height above
						 * baseline) of all glyphs of this font.
						 *
						 * You may use this height for rendering text as 
						 * close together vertically as possible, though 
						 * adding at least one pixel height to this value
						 * will space glyphs so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled
						 * here, so the user is responsible for line spacing,
						 * see getLineSkip as well.
						 *
						 * @note At the moment, no baseline information is
						 * available in descriptions of fixed fonts. 
						 * The baseline is thus chosen to be at the very 
						 * bottom of the glyphs, hence the ascent is equal 
						 * to the height returned by getHeight.
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
						 * It could be used when drawing an individual 
						 * glyph relative to a bottom point, by combining 
						 * with the glyph's vertical metrics to resolve the
						 * top of the rectangle used when blitting the 
						 * glyph on the screen.
						 *
						 * Remember that multiline printing is not enabled
						 * here, so the user is responsible for line spacing,
						 * see getLineSkip as well.
						 *
						 * @note At the moment, no baseline information is
						 * available in descriptions of fixed fonts. 
						 * The baseline is thus chosen to be at the very 
						 * bottom of the glyphs, hence the descent is equal 
						 * to zero.
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
						virtual Text::Height getLineSkip() const ;
						
						
						
						
						// Bounding boxes section.
						
						
						
						/**
						 * Returns a rectangular bounding box corresponding 
						 * to the rendering of a glyph (with fixed fonts 
						 * all characters are of the same size). 
						 *
						 * The returned height is the same as the one you 
						 * can get using the getHeight method.
						 *
						 * The upright rectangle has its lower left corner 
						 * set to the origin. 
						 *
						 * @return an upright rectangle enclosing the 
						 * character (bounding box), whose lower left 
						 * corner is located at the origin.
						 *
						 * @throw FontException if an error occured.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBox() const ;
							
						 
						 
						/**
						 * Returns a rectangular bounding box corresponding 
						 * to the rendering of specified word, encoded in 
						 * ASCII. 
						 *
						 * The returned height is the same as the one you 
						 * can get using the getHeight method.
						 *
						 * The upright rectangle has its lower left corner 
						 * set to the origin. 
						 *
						 * @param word the nonempty ASCII encoded string to 
						 * size up. 
						 * This must be a word, therefore including no
						 * whitespace, since they have to be handled 
						 * separately, depending on the container room.
						 *
						 * @return an upright rectangle enclosing the word
						 * (bounding box), whose lower left corner is 
						 * located at the origin.
						 *
						 * @note After the last character, no other character
						 * is expected to be added, therefore no advance 
						 * is taken into account for this last character.
						 *
						 * @throw FontException if an error occured, for 
						 * example if the specified word is empty.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBoxFor( 
							const std::string & word ) const ;
							 
							 
							 
						
						// Render section.
							 
							 
							 
						/**
						 * Renders specified glyph (Latin-1 character) in
						 * specified color, in a returned new surface.
						 *
						 * Depending on the settings, the returned surface 
						 * can be a clone of a pre-rendered (cached) 
						 * character or, if not available, be put in cache
						 * itself so that it is rendered only once. 
						 * Otherwise, if no cache feature is allowed, each
						 * character will be rendered as many times as
						 * requested.
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * For fixed fonts, only the 'Solid' quality is
						 * available.
						 *
						 * @param glyphColor the color definition for the 
						 * glyph.
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller.
						 *
						 * @throw FontException on error.
						 *
						 */
						virtual Surface & renderLatin1Glyph( 
							Ceylan::Latin1Char character, 
							RenderQuality quality = Solid,
							Pixels::ColorDefinition glyphColor = Pixels::White
						) ;
					
					
						
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
						 * For fixed fonts, only the 'Solid' quality is
						 * available.
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw FontException on error.
						 *
						 */
						virtual void blitLatin1Glyph( 
							Surface & targetSurface,
							Coordinate x, Coordinate y, 
							Ceylan::Latin1Char character, 
							RenderQuality quality = Solid, 
							Pixels::ColorDefinition glyphColor = Pixels::White 
						) ;
						
						
						
						/**
						 * @note This method should not be used.
						 * 
						 * This method was kept so that one day this
						 * alpha-blending based version of 'renderLatin1Glyph'
						 * method could work. 
						 * We never managed to have it working despite 
						 * numerous attempts. 
						 * This is not too disappointing since colorkey 
						 * is probably a better solution than alpha for
						 * characters.
						 *
						 */
						virtual Surface & renderLatin1GlyphAlpha(
							Ceylan::Latin1Char character, 
							RenderQuality quality = Solid,
							Pixels::ColorDefinition glyphColor = Pixels::White 
						) ;
						
						
							
						/*
						 * Rendering and blitting of whole texts are inherited
						 * from Font mother class.
						 *
						 */
						
							
							
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
						 * Sets, if necessary, the parameters of the font 
						 * used by the text back-end.
						 *
						 * @param fontData the data describing the glyphs 
						 * of the font, probably read from a font file. 
						 * Use a null (0) pointer to specify that the 
						 * SDL_gfx internal font should be used.
						 *
						 * @param characterWidth the width of characters 
						 * of this font.
						 *
						 * @param characterHeight the height of characters 
						 * of this font.
						 *
						 * @note This method should be called instead of
						 * 'gfxPrimitivesSetFont', i.e. whenever wanting 
						 * to change font. 
						 *
						 * This method remembers last fixed font settings 
						 * and allows to reset SDL_gfx parameters (including 
						 * its glyph cache) only when necessary, i.e. only 
						 * when the specified settings do not match the 
						 * current ones.
						 * Otherwise at each glyph rendering, even if the 
						 * same font had just been used, we would have to reset 
						 * SDL_gfx state (cache) since we would not be able 
						 * to know whether it had been reset between the
						 * rendering of the two glyphs.
						 *
						 * @throw FontException if the operation failed or is
						 * not supported.
						 *
						 */
						static void SetFontSettings( 
							const Ceylan::Byte * fontData, 
							Length characterWidth, 
							Length characterHeight ) ;

						 
						 
						/**
						 * Allows to keep track of fixed font directories.
						 *
						 */
						static Ceylan::System::FileLocator 
							FixedFontFileLocator ;



						/// Extension of fixed font files ('.fnt').
						static std::string FontFileExtension ;
						
						
						
						/**
						 * The multiplying factor to apply to the font space
						 * width to compute the actual space width.
						 *
						 */
						static const Ceylan::Float32 SpaceWidthFactor ;
						
						
						
						
					protected:
							
												
						
						/**
						 * As no specific quality is managed, a default 
						 * quality is passed in cache entries. 
						 *
						 */
						static const RenderQuality DefaultQuality = Solid ;
						
							
							
						/**
						 * Loads the fixed font from specified file.
						 *
						 * @param fontFilename the font filename, whose
						 * extension is generally '.fnt';
						 *
						 * @throw FontException if the font coud not be loaded.
						 *
						 */						
						virtual void loadFontFrom( 
							const std::string & fontFilename ) ;
						
							
						
						/**
						 * Renders specified glyph, puts it in cache, and
						 * returns a 'const' reference whose ownership is 
						 * kept by the cache (hence the caller should
						 * not deallocate the returned surface).
						 *
						 * The specified character should not be already 
						 * in cache ; its key should not already be associated.
						 *
						 * This helper method is especially useful for blits,
						 * since otherwise they would have to call a render
						 * method which would create a copy for the
						 * cache, whereas blits can use the cached entry
						 * directly.
						 *
						 * It saves an useless copy/deleting. 
						 *
						 * It does not check the cache for a previous entry
						 * since its purpose is to feed the glyph cache when 
						 * the specified key is still to be associated. 
						 * It does that independently of the cache settings
						 * (glyph, word, text, etc.).
						 * 
						 * One should ensure the corresponding cache key is 
						 * not already in use, notably if the 'NeverDrop' 
						 * cache policy is chosen.
						 *
						 * @param character the Latin-1 character to have
						 * rendered.
						 *
						 * @param glyphColor the color definition for the
						 * glyph. 
						 *
						 * @return a 'const' referencec to a glyph in cache.
						 *
						 * @throw FontException on error, including 
						 * 'NeverDrop' policy being used and an entry already
						 * existing.
						 *
						 */
						virtual const Surface & submitLatin1GlyphToCache( 
							Ceylan::Latin1Char character,
							Pixels::ColorDefinition glyphColor ) ;
							
							
						
						/**
						 * Renders specified glyph (Latin-1 character) in
						 * specified color, on a new surface, directly 
						 * thanks to the font backend: no quality nor cache
						 * are taken into account. 
						 *
						 * Colorkeys and conversion to display are however
						 * enforced.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 */
						virtual Surface & basicRenderLatin1Glyph(
							Ceylan::Latin1Char character,
							Pixels::ColorDefinition glyphColor ) ;
							
							
							
						/**
						 * Blits specified glyph (Latin-1 character) in
						 * specified color, on specified location of given
						 * surface, directly thanks to the font backend: 
						 * no quality nor cache are taken into account.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 * @param targetSurface the surface the glyph will be
						 * blitted to.
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
						 * For fixed font, only the 'Solid' quality is
						 * available.
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw FontException on error.
						 *
						 */
						virtual void basicBlitLatin1Glyph( 
							Surface & targetSurface, 
							Coordinate x, Coordinate y,	
							Ceylan::Latin1Char character,
							Pixels::ColorDefinition glyphColor ) ;
						
						
						
														
						/// The character width for this font.
						Length _width ;
						
						
						
						/// The character height for this font.
						Length _height ;
							
												
							
						/// The data describing the characters of the font. 
						char * _fontData ;
						
						
																		
						/**
						 * Returns the name of the font file which would
						 * correspond to the specified settings.
						 *
						 * @param characterWidth the character width.
						 *
						 * @param characterHeight the character height.
						 *
						 * @param renderingStyle the rendering style. 
						 * For fixed fonts, attributes (bold, italic, etc.)
						 * cannot be mixed.
						 *
						 * @throw FontException if the settings could not 
						 * be satisfied.
						 *
						 */
						static std::string BuildFontFilenameFor( 
							Length characterWidth, 
							Length characterHeight, 
							RenderingStyle renderingStyle ) ;
							
							
							
						
						// Static section.
						
						
						
						/**
						 * Returns in specified variables the font attributes
						 * guessed from the filename.
						 *
						 * @param the input filename, without any path
						 * (ex: '10x20.fnt', '8x13B.fnt', etc.)
						 *
						 * @param characterWidth where the guessed width 
						 * is stored.
						 *
						 * @param characterHeight where the guessed height 
						 * is stored.
						 *
						 * @param renderingStyle where the guessed style 
						 * is stored.
						 *
						 * @throw FontException if the font settings could 
						 * not be guessed from the filename.
						 *
						 */
						static void GetFontAttributesFrom( 
							const std::string & filename, 
							Length & characterWidth, 
							Length & characterHeight, 
							RenderingStyle & renderingStyle ) ;
					
					
					
						/**
						 * Defines the number of potential different 
						 * characters in a fixed font.
						 *
						 */
						static const Ceylan::Uint16 FontCharacterCount ;
					
					
					
					
					private:
								
						
									
						/**
						 * Copy constructor made private to ensure that 
						 * it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined constructor is called, implicitly or not.
						 *
						 */			 
						FixedFont( const FixedFont & source ) ;
			
			
			
						/**
						 * Assignment operator made private to ensure 
						 * that it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined operator is called, implicitly or not.
						 *
						 */			 
						FixedFont & operator = ( const FixedFont & source ) ;
				
									
				
				} ;
				
				
			} 
			
		}	
			
	}
	
}		



#endif // OSDL_FIXED_FONT_H_

