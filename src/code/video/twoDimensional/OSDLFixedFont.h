#ifndef OSDL_FIXED_FONT_H_
#define OSDL_FIXED_FONT_H_



#include "OSDLFont.h"         // for inheritance


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
				extern const Length BasicFontCharacterWidth ;
				
				/// The character height of fixed basic font.
				extern const Length BasicFontCharacterHeight ;
				
				
				/**
				 * Prints specified string in target surface, with specified color, taking
				 * as upper left corner specified coordinates. Basic 8x8 bitmap font will be used.
				 *
				 * Not all accentuated characters are supported by the built-in font. For example, 
				 * 'à', 'â', etc. are replaced by greek characters.
				 *
				 * @note The printBasic methods are very lightweight since they rely onto built-in
				 * basic fonts : no need to instanciate a FixedFont class to use them.
				 *
				 */
				bool printBasic( const std::string & text, Surface & targetSurface,
					Coordinate x, Coordinate y, Pixels::ColorElement red, Pixels::ColorElement blue,
					Pixels::ColorElement green, Pixels::ColorElement alpha = Pixels::AlphaOpaque )
						throw() ; 
				
				
				/**
				 * Prints specified string in target surface, with specified color, taking
				 * as upper left corner specified coordinates. Basic 8x8 bitmap font will be used.
				 *
				 * Not all accentuated characters are supported by the built-in font. For example, 
				 * 'à', 'â', etc. are replaced by greek characters.
				 *
				 * @note The printBasic methods are very lightweight since they rely onto built-in
				 * basic fonts : no need to instanciate a FixedFont class to use them.
				 *
				 */
				bool printBasic( const std::string & text, Surface & targetSurface,
					Coordinate x, Coordinate y, Pixels::ColorDefinition colorDef ) throw() ; 
					


				/**
				 * Models fixed fonts, a category which gather all fonts whose rendering demands
				 * little resources, but cannot be scaled, as opposed to Truetype fonts.
				 *
				 * They usually have a character set including at least all alphanumerical 
				 * characters, including capital letters, plus most special characters. They should
				 * provide at least a full support for characters encoded according to Latin-1.
				 *
				 * For these fonts, the baseline is chosen to be at the very bottom of the glyph :
				 * no glyph goes below the baseline.
				 *
				 * These fonts are loaded from font files, whose extension is usually '.fnt'.
				 * The font files are searched first 'as is', using directly the filename
				 * (which may be absolute or relative to the current directory), then using the 
				 * fixed font locator, then the overall font locator.
				 * 
				 * The principle of these rendering methods is to return a surface with the chosen
				 * glyph(s) drawn with the specified color, with no visible background : thanks to
				 * color key or alpha-blending, only the text can be seen so that this returned
				 * surface can be directly blitted onto any already existing background that will
				 * by hidden only by the pixels corresponding to the text.
				 * 
				 * As a returned surface is encoded with a RLE-color key and/or can be hardware,
				 * it is especially important to lock this surface before reading and/or writing
				 * its pixels, and to unlock it afterwards.
				 *
				 * This returned surface is to be owned by the caller of the render method. This 
				 * caller will have to deallocate it when the surface will not be of use anymore.
				 *
				 * Rendering should only occur after the video has been initialized (setMode).
				 *
				 * @see http://jcatki.no-ip.org/SDL_ttf/metrics.png for informations about
				 * glyph metrics.
				 *
				 * @note Most lenghts are given alongside a directed axis, and therefore they can 
				 * be negative.
				 *
				 */	
				class FixedFont : public Font
				{
				
				
					public:
											
						
						/**
						 * Creates, if possible, a Fixed font having specified features, by looking
						 * up a corresponding font file.
						 *
						 * These fonts are deemed lightweight compared to Truetype fonts.
						 *
						 * @param characterWidth the desired width of a character.
						 *
						 * @param characterHeight the desired height of a character.
						 *
						 * @param renderingStyle the desired rendering style, which can be only one
						 * of the listed rendering styles (no combination of attributes allowed).
						 *
						 * @param convertToDisplay tells whether a returned or cached rendering 
						 * should have already been converted to display. Useful for classical 2D
						 * rendering, useless for OpenGL since surfaces will have to be converted
						 * nonetheless.
						 *
						 * @param cacheSettings determines which renderings should be cached. The
						 * recommended - and default - setting is 'GlyphCached', since with 
						 * fixed fonts words can be easily made for prerendered glyphs.
						 *
						 * @param cachePolicy determines how the cache should behave regarding 
						 * renderings being put in cache (store everything, enforce a maximum size
						 * taken in memory, etc.)
						 *
						 * @param quota Should an upper-bound to the memory size of cached
						 * renderings apply (depending on the chosen cache policy), determines 
						 * its actual value. If a null (0) quota is passed, then the default value
						 * of the quota for the selected cache settings will be chosen. For example,
						 * if cacheSettings is 'WordCached' and cachePolicy is
						 * 'DropLessRequestedFirst' (this policy uses a quota), and if a null
						 * quota is specified, then the actual quota being used will be
						 * 'DefaultWordCachedQuota'.
						 *
						 * Most common fixed fonts are (width x height : available styles, which
						 * cannot be combined), there are 21 of them :
						 *	- 5x7   : Normal
						 *	- 5x8   : Normal
						 *	- 6x9   : Normal
						 *	- 6x10  : Normal
						 *	- 6x12  : Normal
						 *	- 6x13  : Normal, or Bold, or Italic
						 *	- 7x13  : Normal, or Bold, or Italic
						 *	- 7x14  : Normal, or Bold
						 *	- 8x13  : Normal, or Bold, or Italic
						 *	- 9x15  : Normal, or Bold
						 *	- 9x18  : Normal, or Bold
						 *  - 10x20 : Normal
						 *
						 * Rendering styles encoding is :
						 *   - 'B' for Bold
						 *   - 'O' for Italic (sorry for the back-end poor choice)
						 *	 - 'U' for Underline
						 *
						 * The filename of a font file describes the font, its form is :
						 * 'width'x'height'[rendering style]'.fnt'. 
						 *
						 * For example, '10x20.fnt' is a normal 10x20 font, whereas
						 * '8x13B.fnt' is a 8x13 bold font and '8x13O.fnt' is without bold but with
						 * italic. Rendering style mixing (ex : 'BO') is not allowed, a font have 
						 * at most one rendering style.
						 *
						 * @throw TextException if no corresponding font file could be found, or if 
						 * cache setting is not known, etc.
						 *
						 * The font file will be searched from the fixed font locator first and, 
						 * on failure, from the overall font locator. 
						 *
						 *						 
						 */
						FixedFont( 
							Length characterWidth, 
							Length characterHeight, 
							RenderingStyle renderingStyle = Normal,
							bool convertToDisplay = true,
							RenderCache cacheSettings = GlyphCached,
							AllowedCachePolicy cachePolicy = DropLessRequestedFirst,
							Ceylan::System::Size quota = 0 ) 
								throw( TextException ) ;
						 
						 
						 
						/**
						 * Creates a new Fixed font from specified font file.
						 *
						 * @param fontFilename the filename of the file where the font is stored,
						 * usually a .fnt file. The filename will be interpreted
						 * first 'as is', i.e. as an absolute path or a relative path to the 
						 * current directory. If it does not succeed, the font file will be 
						 * searched through directories listed in the FONT_PATH environment 
						 * variable, if any.
						 *
						 * @param renderingStyle the desired rendering style, which can be only one
						 * of the listed rendering styles (no combination of attributes allowed).
						 *
						 * @param convertToDisplay tells whether a returned or cached rendering 
						 * should be converted to display beforehand.
						 *
						 * @param cacheSettings determines which renderings should be cached. The
						 * recommended - and default - setting is 'GlyphCached', since with 
						 * fixed fonts words can be easily made for prerendered glyphs.
						 *
						 * @param cachePolicy determines how the cache should behave regarding 
						 * renderings being put (store everything, enforce a maximum size taken in 
						 * memory, etc.)
						 *
						 * @param quota Should an upper-bound to the memory size of cached
						 * renderings apply (depending on the chosen cache policy), determines 
						 * its actual value. If a null (0) quota is passed, then the default value
						 * of the quota for the selected cache settings will be chosen. For example,
						 * if cacheSettings is 'WordCached' and cachePolicy is
						 * 'DropLessRequestedFirst' (this policy uses a quota), and if a null
						 * quota is specified, then the actual quota being used will be
						 * 'DefaultWordCachedQuota'.
						 *
						 * @throw TextException if no corresponding font file could be found, or if 
						 * cache setting is not known, etc.
						 *
						 * @note The rendering style is deduced from the font file name.
						 *
						 * Only the blit and render methods for glyphs are declared and defined in
						 * this class, since their counterparts for words and texts are built 
						 * upon them and already available through inheritance (Font mother class).
						 *
						 */
						explicit FixedFont( const std::string & fontFilename, 
							bool convertToDisplay = true,
							RenderCache cacheSettings = GlyphCached,
							AllowedCachePolicy cachePolicy = DropLessRequestedFirst,
							Ceylan::System::Size quota = 0 ) throw( TextException ) ;
						
						
						/**
						 * Fixed font virtual destructor.
						 *
						 */
						virtual ~FixedFont() throw() ;
								
												
									 
						/**
						 * Returns the maximum pixel width of all glyphs of this font. 
						 *
						 * You may use this width for rendering text as close together horizontally
						 * as possible, though adding at least one pixel height to it will space it,
						 * so they cannot touch. 
						 *
						 * @note Width is constant in this fixed font special case, and null here
						 * since already taken into account in character width.
						 *
						 */
						virtual Width getWidth() const throw() ;


						/**
						 * Returns the width of the specified glyph, rendered with this font. 
						 *
						 * @param character the character whose width will be returned.
						 *
						 * Remember that multiline printing is not enabled, so the user is
						 * responsible for line breaking.
						 *
						 * @note For fixed font, width does not depend on the specific character
						 * chosen.
						 *
						 * @see getAdvance
						 *
						 */
						virtual Width getWidth( Ceylan::Latin1Char character ) const throw() ;


						/**
						 * Returns the width of the abscissa offset for the specified glyph,
						 * rendered with this font. 
						 *
						 * This offset corresponds to the first abscissa from which the glyph 
						 * should be rendered from a given location.
						 *
						 * @param character the character whose abscissa offset will be returned.
						 *
						 * Remember that multiline printing is not enabled, so the user is
						 * responsible for line breaking.
						 *
						 * @throw TextException if the glyph metrics could not be retrieved.
						 *
						 * @note For fixed font, offset width does not depend on the specific
						 * character chosen, and is null.
						 */
						virtual SignedWidth getWidthOffset( Ceylan::Latin1Char character ) 
							const throw( TextException ) ;


						/**
						 * Returns the height above baseline of the specified glyph, 
						 * rendered with this font. 
						 *
						 * @param character the character whose height above baseline will be
						 * returned.
						 *
						 * @throw TextException if the glyph metrics could not be retrieved.
						 *
						 * @note For fixed font, height above baseline does not depend on the
						 * specific character chosen, and is equal to the height of this fixed
						 * font.
						 *
						 */
						virtual SignedHeight getHeightAboveBaseline( Ceylan::Latin1Char character ) 
							const throw( TextException ) ;


						/**
						 * Returns the advance (recommended width containing a glyph, so that they
						 * can be chained in a word), which with fixed fonts does not depend on 
						 * the specific character being rendered.
						 *
						 * This method is not inherited from Font.
						 *
						 * @throw TextException if the glyph metrics could not be retrieved.
						 *
						 */
						virtual SignedLength getAdvance() const throw( TextException ) ;
						
						
						/**
						 * Returns the advance (recommended width containing a glyph, so that they
						 * can be chained in a word), which with fixed fonts does not depend on 
						 * the specific character being rendered.
						 *
						 * @param character any character has the same advance.
						 *
						 * @throw TextException if the glyph metrics could not be retrieved.
						 *
						 * @note This method exists only because the interface demands it.
						 *
						 */
						virtual SignedLength getAdvance( Ceylan::Latin1Char character ) 
							const throw( TextException ) ;


						/**
						 * Returns the width of the recommended space between two glyphs.
						 *
						 * Simple text rendering can rely on constant space between glyphs, but 
						 * for more complex cases such as multi-line printing, more complex
						 * rules should be used.
						 *
						 * The default value is one pixel.
						 *
						 */
						virtual Width getInterGlyphWidth() const throw() ;
				
						 
						 
						/**
						 * Returns the maximum pixel height of all glyphs of this font. 
						 *
						 * You may use this height for rendering text as close together vertically
						 * as possible, though adding at least one pixel height to it will space it,
						 * so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, so the user is
						 * responsible for line spacing, see getLineSkip as well.
						 *
						 */
						virtual Height getHeight() const throw() ;
						 						 
						 
						/**
						 * Returns the maximum pixel ascent (height above baseline) of all glyphs 
						 * of this font.
						 *
						 * You may use this height for rendering text as close together vertically
						 * as possible, though adding at least one pixel height to it will space it,
						 * so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, so the user is
						 * responsible for line spacing, see getLineSkip as well.
						 *
						 * @note At the moment, no baseline information is available for fixed
						 * fonts. The baseline is chosen to be at the very bottom of the glyph,
						 * hence the ascent is equal to the height returned by getHeight.
						 *
						 */
						virtual SignedHeight getAscent() const throw() ;
						 
						 
						/**
						 * Returns the maximum pixel descent (height below baseline) of all glyphs 
						 * of this font. If at least a glyph has parts below the abscissa axis, the
						 * descent is negative.
						 *
						 * It could be used when drawing an individual glyph relative to a bottom
						 * point, by combining it with the glyph's maxy metric to resolve the top 
						 * of the rectangle used when blitting the glyph on the screen.
						 *
						 * Remember that multiline printing is not enabled, so the user is
						 * responsible for line spacing, see getLineSkip as well.
						 *
						 * @note At the moment, no baseline information is available for fixed
						 * fonts. The baseline is supposed to be at the very bottom of the glyph,
						 * hence the descent is equal to zero.
						 *
						 */
						virtual SignedHeight getDescent() const throw() ;
						 
						 
						/**
						 * Returns the recommended pixel height of a rendered line of text of this 
						 * font. This is usually larger than the value returned by getHeight.
						 *
						 */
						virtual Height getLineSkip() const throw() ;
						
						
						
						
						// Bounding boxes section.
						
						
						/**
						 * Returns a rectangular bounding box corresponding to the rendering of
						 * a character (with fixed fonts all characters are of the same size). 
						 *
						 * The returned height is the same as you can get using the getHeight
						 * method.
						 *
						 * The upright rectangle has its lower left corner set to the origin. 
						 *
						 *
						 * @return an upright rectangle enclosing the character (bounding box),
						 * whose lower left corner is located at the origin.
						 *
						 * @throw TextException if an error occured.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBox() const throw( TextException ) ;
							
						 
						/**
						 * Returns a rectangular bounding box corresponding to the rendering of
						 * specified word, encoded in ASCII. 
						 *
						 * The returned height is the same as you can get using the getHeight
						 * method.
						 *
						 * The upright rectangle has its lower left corner set to the origin. 
						 *
						 * @param word the ASCII encoded string to size up. This must be a word,
						 * therefore including no whitespace, since they have to be handled 
						 * separately, depending on the container room.
						 *
						 * @return an upright rectangle enclosing the word (bounding box), whose
						 * lower left corner is located at the origin.
						 *
						 * @note After the last character not other character should be added, 
						 * therefore no advance is taken into account for this last character.
						 *
						 * @throw TextException if an error occured, for example if the specified
						 * word is empty.
						 *
						 */ 
						virtual UprightRectangle & getBoundingBoxFor( const std::string & word )
							const throw( TextException ) ;
							 
							 
							 
						
						// Render section.
							 
							 
						/**
						 * Renders specified glyph (Latin-1 character) in specified color, in a
						 * returned surface.
						 * Depending on the settings, the returned surface can be a clone of a 
						 * pre-rendered (cached) character or, if not available, be put in cache
						 * itself so that it is rendered once. 
						 * Otherwise, if no cache feature is allowed, each character will be
						 * rendered as many times as requested.
						 *
						 * The caller is responsible for deleting any returned surface.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. For fixed font, only the 
						 * 'Solid' quality is available.
						 *
						 * @param glyphColor the color definition for the glyph.
						 *
						 * @return a newly allocated Surface, whose ownership is transferred to 
						 * the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual Surface & renderLatin1Glyph( Ceylan::Latin1Char character, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition glyphColor = Pixels::White ) 
							throw( TextException ) ;
					
						
						/**
						 * Blits specified glyph (Latin-1 character) in specified color, on
						 * specified location of given surface.
						 *
						 * @param targetSurface the surface the glyph will be blitted to.
						 *
						 * @param x the abscissa in target surface of the top-left corner of the 
						 * glyph blit.
						 *
						 * @param y the ordinate in target surface of the top-left corner of the 
						 * glyph blit.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. For fixed font, only the 
						 * 'Solid' quality is available.
						 *
						 * @param glyphColor the color definition for the glyph. 
						 *
						 * @return a newly allocated Surface, whose ownership is transferred to 
						 * the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1Glyph( Surface & targetSurface,
								Coordinate x, Coordinate y, 
								Ceylan::Latin1Char character, RenderQuality quality = Solid, 
								Pixels::ColorDefinition glyphColor = Pixels::White ) 
							throw( TextException ) ;
						
						
						/**
						 * @note This method should not be used.
						 * 
						 * This method was kept so that one day this alpha-blending based 
						 * version of 'renderLatin1Glyph' method could work. 
						 * We never managed to have it working despite numerous attempts. 
						 * This is not too disappointing since colorkey is probably a better
						 * solution than alpha for characters.
						 *
						 */
						virtual Surface & renderLatin1GlyphAlpha( Ceylan::Latin1Char character, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition glyphColor = Pixels::White ) 
							throw( TextException ) ;
						
						
							
						// Render and blit of whole texts is inherited from Font mother class.
						
							
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
							Ceylan::VerbosityLevels level = Ceylan::high ) const throw() ;


						
						// Static section.
	
	
						/**
						 * Returns which quality would be used if specified rendering quality was
						 * requested.
						 *
						 */
						static RenderQuality GetObtainedQualityFor( RenderQuality targetedQuality )
							throw() ;
						 
	
						/**
						 * Sets, if necessary, the parameters of the font used by the text 
						 * back-end.
						 *
						 * @param fontData the data describing the glyphs of the font, probably 
						 * read from a font file. Use a null (0) pointer to specify that the 
						 * SDL_gfx internal font should be used.
						 *
						 * @param characterWidth the width of characters of this font.
						 *
						 * @param characterHeight the height of characters of this font.
						 *
						 * @note This method should be called instead of 'gfxPrimitivesSetFont',
						 * i.e. whenever wanting to change font. 
						 * This method remembers last fixed font settings and allows to reset
						 * SDL_gfx parameters (including its glyph cache) only when necessary, 
						 * i.e. only when the specified settings do not match the current ones.
						 * Otherwise at each glyph rendering, even if the same font is used, we 
						 * would have to reset SDl_gfx state (cache) since we would not be able to
						 * know whether it had been reset between the rendering of the two glyphs.
						 *
						 */
						static void SetFontSettings( const char * fontData, Length characterWidth, 
							Length characterHeight ) throw() ;

						 
						/**
						 * Allows to keep track of fixed font directories.
						 *
						 */
						static Ceylan::System::FileLocator FixedFontFileLocator ;


						/// Extension of fixed font files.
						static std::string FontFileExtension ;
						
						
						/**
						 * The multiplying factor to apply to the font space width to compute the
						 * actual space width.
						 *
						 */
						static const Ceylan::Float32 SpaceWidthFactor ;
						
						
						
					protected:
												
						
						/**
						 * As no specific quality is managed, a default quality is passed in cache
						 * entries. 
						 *
						 */
						static const RenderQuality DefaultQuality = Solid ;
						
							
						/**
						 * Loads the fixed font from specified file.
						 *
						 * @param fontFilename the font filename, whose extension is generally
						 * '.fnt';
						 *
						 * @throw TextException if the font coud not be loaded.
						 *
						 */						
						virtual void loadFontFrom( const std::string & fontFilename ) 
							throw( TextException ) ;
							
						
						/**
						 * Renders specified glyph, puts it in cache, and returns a 'const'
						 * reference whose ownership is kept by the cache (hence the caller should
						 * not deallocate the returned surface).
						 *
						 * The specified character should not be already in cache ; its key should
						 * not already be associated.
						 *
						 * This helper method is especially useful for blits, since otherwise they
						 * would have to call a render method which would create a copy for the
						 * cache, whereas blits can use the cached entry directly. It saves an
						 * useless copy/deleting. 
						 *
						 * It does not check the cache for a previous entry since its purpose is to
						 * feed the glyph cache when the specified key is still to be associated. 
						 * It does that independently of the cache settings (glyph, word, text,
						 * etc.).
						 * 
						 * One should ensure the corresponding cache key is not already in use,
						 * notably if the 'NeverDrop' cache policy is chosen.
						 *
						 * @param character the Latin-1 character to have rendered.
						 *
						 * @param glyphColor the color definition for the glyph. 
						 *
						 * @return a 'const' referencec to a glyph in cache.
						 *
						 * @throw TextException on error, including 'NeverDrop' policy being used
						 * and an entry already existing.
						 *
						 */
						virtual const Surface & submitLatin1GlyphToCache( 
								Ceylan::Latin1Char character, Pixels::ColorDefinition glyphColor )
							throw( TextException ) ;
							
						
						/**
						 * Renders specified glyph (Latin-1 character) in specified color, on a 
						 * new surface, directly thanks to the font backend : no quality nor cache
						 * are taken into account. 
						 *
						 * Colorkeys and conversion to display are however enforced.
						 *
						 * This method is meant to be used as an helper function so that
						 * user-exposed methods can be easily implemented.
						 *
						 */
						virtual Surface & basicRenderLatin1Glyph( Ceylan::Latin1Char character,
							Pixels::ColorDefinition glyphColor ) throw( TextException ) ;
							
							
						/**
						 * Blits specified glyph (Latin-1 character) in specified color, on
						 * specified location of given surface, directly thanks to the font 
						 * backend : no quality nor cache are taken into account.
						 *
						 * This method is meant to be used as an helper function so that
						 * user-exposed methods can be easily implemented.
						 *
						 * @param targetSurface the surface the glyph will be blitted to.
						 *
						 * @param x the abscissa in target surface of the top-left corner of the 
						 * glyph blit.
						 *
						 * @param y the ordinate in target surface of the top-left corner of the 
						 * glyph blit.
						 *
						 * @param character the Latin-1 character to render.
						 *
						 * @param quality the chosen rendering quality. For fixed font, only the 
						 * 'Solid' quality is available.
						 *
						 * @param glyphColor the color definition for the glyph. 
						 *
						 * @return a newly allocated Surface, whose ownership is transferred to 
						 * the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void basicBlitLatin1Glyph( Surface & targetSurface, 
							Coordinate x, Coordinate y,	Ceylan::Latin1Char character,
							Pixels::ColorDefinition glyphColor ) throw( TextException ) ;
						
						
														
						/// The character width for this font.
						Length _width ;
						
						/// The character height for this font.
						Length _height ;
												
							
						/// The data describing the characters of the font. 
						char * _fontData ;
						
																		
						/**
						 * Returns the name of the font file which would correspond to the specified
						 * settings.
						 *
						 * @param characterWidth the character width.
						 *
						 * @param characterHeight the character height.
						 *
						 * @param renderingStyle the rendering style. For fixed font, attributes
						 * (bold, italic, etc.) cannot be mixed.
						 *
						 * @throw TextException if the settings could not be satisfied.
						 *
						 */
						static std::string BuildFontFilenameFor( Length characterWidth, 
								Length characterHeight, RenderingStyle renderingStyle ) 
							throw( TextException ) ;
							
							
						
						// Static section.
						
						
						/**
						 * Returns in specified variables the font attributes guessed from the
						 * filename.
						 *
						 * @param the input filename, without any path
						 * (ex : '10x20.fnt', '8x13B.fnt', etc.)
						 *
						 * @param characterWidth where the guessed width is stored.
						 *
						 * @param characterHeight where the guessed height is stored.
						 *
						 * @param renderingStyle where the guessed style is stored.
						 *
						 * @throw TextException if the font settings could not be guessed from the
						 * filename.
						 *
						 */
						static void GetFontAttributesFrom( const std::string & filename, 
							Length & characterWidth, Length & characterHeight, 
							RenderingStyle & renderingStyle ) throw( TextException ) ;
					
					
						/// Defines the number of potential different characters in a fixed font.
						static Ceylan::Uint16 FontCharacterCount ;
					
					
					
					private:
					
									
									
						/**
						 * Copy constructor made private to ensure that it will be never called.
						 * The compiler should complain whenever this undefined constructor is
						 * called, implicitly or not.
						 * 
						 * @note Use the clone method instead.
						 *
						 */			 
						FixedFont( const FixedFont & source ) throw() ;
			
			
						/**
						 * Assignment operator made private to ensure that it will be never called.
						 * The compiler should complain whenever this undefined operator is called, 
						 * implicitly or not.
						 * 
						 *
						 */			 
						FixedFont & operator = ( const FixedFont & source ) throw() ;
				
									
				
				} ;
				
			} 
			
		}	
			
	}
	
}		


#endif // OSDL_FIXED_FONT_H_

