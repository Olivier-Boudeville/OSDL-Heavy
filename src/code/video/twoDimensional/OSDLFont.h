#ifndef OSDL_FONT_H_
#define OSDL_FONT_H_



#include "OSDLVideo.h"        // for VideoException
#include "OSDLVideoTypes.h"   // for Length, SignedLength, etc.
#include "OSDLPixel.h"        // for ColorElement, ColorDefinition
#include "OSDLSurface.h"      // for Surface

#include "Ceylan.h"           // for Uint32, inheritance

#include <string>

	


namespace OSDL
{


	namespace Video
	{


		// Fonts are rendered to surfaces.
		class Surface ;


		namespace TwoDimensional
		{
			
			
			
			/// Mother class for all text exceptions. 		
			class OSDL_DLL TextException: public VideoException 
			{ 
			
				public: 
				
					explicit TextException( const std::string & reason ) 
						throw() ; 
					virtual ~TextException() throw() ; 
					
			} ;
				
			
			
			namespace Text
			{	
			

				/// Point size, in dots per inch.
				typedef Length PointSize ;
				
				
				/**
				 * Describes a font index, when multiple fonts are stored 
				 * in the same location.
				 *
				 */
				typedef Ceylan::Uint16 FontIndex ;
				
				
				
				/// Describes a font pixel unsigned height.
				typedef Ceylan::Uint16 Height ;
				
				/// Describes a font pixel signed height.
				typedef Ceylan::Sint16 SignedHeight ;
				
				
				/// Describes a font pixel unsigned width.
				typedef Ceylan::Uint16 Width ;
				
				/// Describes a font pixel signed width.
				typedef Ceylan::Sint16 SignedWidth ;
				
				
				/// Describes the rendering style of a font.
				typedef int RenderingStyle ;
				
								
				/**
				 * Index designating a position in a text, starting at 
				 * zero with the first character.
				 *
				 */
				typedef Ceylan::Uint16 TextIndex ;
					
									
				/**
				 * Describes a line number is a multiline text.
				 * 
				 * @note The first line has for number '1' (not '0').
				 *
				 */
				typedef Ceylan::Uint16 LineNumber ;
									
									
				
				/// Defines the various possible horizontal alignments.
				enum HorizontalAlignment { Left, WidthCentered, Right } ;
				
				/// Defines the various possible vertical alignments.
				enum VerticalAlignment { Top, HeightCentered, Bottom } ;
				
				
				
				/**
				 * Models an abstract font, including its rendering style 
				 * (bold, italic, etc.).
				 *
				 * For each font, a glyph (a character) or a line of text 
				 * (a series of glyphs) can be rendered, according to various
				 * encodings (all fonts should support at least the Latin-1
				 * encoding), qualities (some types of fonts allows for 
				 * several ones, others will have the selected quality mapped 
				 * to available choices) and text color. 
				 *
				 * Various cache settings are supported to speed up the
				 * rendering when the same glyph or text is requested more 
				 * than once. 
				 * Most of the time, especially if multiline text rendering 
				 * will be requested, the 'WordCached' policy should be
				 * selected.
				 * 
				 * With a Font instance, independently of its inner working,
				 * everything from the simple glyph to the full multiline
				 * justified text with paragraphs and alinea, can be rendered.
				 *
				 * The principle of these rendering methods is to return a
				 * surface with the chosen glyph(s) drawn with the specified
				 * color, with no visible background : thanks to color key or
				 * alpha-blending, only the text can be seen, so that this
				 * returned surface can be directly blitted onto any already
				 * existing background that will be hidden only by the 
				 * pixels corresponding to the text.
				 * 
				 * Words and texts are rendered so that all glyphs lie on a
				 * common baseline, but glyphs are rendered in individual
				 * surfaces of different height. 
				 * Hence drawing a text is more difficult than just blitting 
				 * all glyphs from the same height, and usually applications 
				 * do not use direct glyph rendering : most of the time
				 * words or texts are preferably rendered as a whole.
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
				 * The signatures detail rendering parameters in the names 
				 * (ex : 'renderUTF8Text' instead of simply 'render'), 
				 * since otherwise there would be methods whose names 
				 * would be hidden in child classes.
				 * 
				 * Each child class should initialize itself the '_spaceWidth'
				 * and '_alineaWidth' members, it cannot be done at the 
				 * level of this Font mother class.
				 *
				 */
				class OSDL_DLL Font : public Ceylan::TextDisplayable
				{
				
				
					public:
					


						/**
						 * Describes the various rendering qualities 
						 * available for texts.
						 * Note that not all qualities are available for 
						 * all fonts. 
						 * This applies mostly for TrueType fonts, since 
						 * other systems seldom have different qualities.
						 *
						 * The actual rendering quality will be the closest
						 * available one, compared to the requested one.
						 *
						 * These informations on the internal format of the
						 * rendered surfaces are valid only as long as no
						 * conversion to display occurs :
						 *
						 * - Solid : Quick and Dirty
						 *
						 * Creates an 8-bit palettized surface and renders 
						 * the given text at fast quality with the given 
						 * font and color.
						 *
						 * The 0 pixel value is the colorkey, giving a
						 * transparent background, and the 1 pixel value 
						 * is set to the text color.
						 *
						 * The colormap is set to have the desired foreground
						 * color at index 1, this allows you to change the 
						 * color without having to render the text again.
						 *
						 * Colormap index 0 is of course not drawn, since 
						 * it is the colorkey, and thus transparent, though 
						 * its actual color is 255 minus each RGB component 
						 * of the foreground.
						 *
						 * This is the fastest rendering speed of all the
						 * rendering modes.
						 *
						 * This results in no box around the text, but the 
						 * text is not as smooth as obtained with other
						 * qualities : the 'Solid' one is quite poor. 
						 * By using a render cache, at the expense of more
						 * memory used, other qualities could demand low CPU
						 * resources as well.
						 *
						 * The resulting surface should blit faster than the
						 * Blended one.
						 * Use this mode for frame per second counters and 
						 * other fast changing text displays.
						 *
						 * - Shaded : Slow and Nice (and Recommended quality 
						 * for most outputs)
						 *
						 * Creates a 32-bit RGB RLE-colorkeyed surface 
						 * (with no alpha) and renders the given text at 
						 * high quality with the given font and colors.
						 * Apart the background, the other pixels have 
						 * varying degrees of the foreground color from the
						 * background color. 
						 * This background color should be managed as a state
						 * variable of the corresponding font object. 
						 * Its default value is pure black.
						 *
						 * The text is antialiased. 
						 * This will render slower than Solid, but in about 
						 * the same time as Blended mode.
						 *
						 * The resulting surface should blit quite as fast 
						 * as Solid, once it is made. 
						 * Use this when you need nice text.
						 *
						 * - Blended : Slow Slow Slow, but Ultra Nice over
						 * another image.
						 *
						 * Creates a 32-bit ARGB surface and render the 
						 * given text at high quality, using alpha blending 
						 * to dither the font with the given color.
						 *
						 * This results in a surface with alpha transparency, 
						 * so you do not have a solid colored box around the
						 * text.
						 *
						 * The text is antialiased.
						 * This will render slower than Solid, but in about 
						 * the same time as Shaded mode. 
						 *
						 * The resulting surface will blit slower than if 
						 * you had used Solid or Shaded.
						 *
						 * Use this when you want high quality, and the 
						 * text is not changing too fast.
						 *
						 */
						enum RenderQuality { Solid, Shaded, Blended } ;
				


						/*
						 * The Ceylan cache manager allows to reference
						 * resources based on their key. 
						 * 
						 * Here, the resource is a text rendering (a surface)
						 * and the key is made from the corresponding text 
						 * (be it Latin-1 char or std::string) <em>and</em> 
						 * from the text color <em>and</em> from the desired
						 * rendering quality. 
						 *
						 * I.e we request a red 'Solid' 'c', or a blue 'Shaded'
						 * "Some text", and we expect the cache to return a
						 * corresponding already computed rendering, i.e. a
						 * surface, if available.
						 *
						 * The key therefore is a composite type (a text, a
						 * color and a quality), which could be modeled at 
						 * least thanks to two patterns :
						 *   - a triple made from STL : when quality was not
						 * specifically managed, a
						 * std::pair<Latin1Char,ColorDefinition> could be 
						 * used, since the STL pairs have the '<' (less)
						 * operators defined (necessary for the ordering
						 * of the std::map ordering). 
						 * However ColorDefinition is a struct, with no
						 * specific '<' operator defined, which would be 
						 * needed here. 
						 * Moreover, for three parameters, the STL is 
						 * probably less useful
						 *   - a dedicated key class, which defines that same
						 * operator '<' 
						 *
						 * We thus chose the latter solution here.
						 *
						 */

						
						/**
						 * Cache key to manage colored characters with 
						 * various qualities.
						 *
						 * @note Must be as lightweight as possible, it is 
						 * a search key. Virtual table is avoided.
						 *
						 */
						class OSDL_DLL CharColorQualityKey
						{
						
						
							public:
								
								
								CharColorQualityKey( 
										Ceylan::Latin1Char character, 
										Pixels::ColorDefinition color,
										RenderQuality quality ) throw() :
									_character( character ),
									_color( color ),
									_quality( quality )
								{
								
								}
								
								
								/// Offers an arbitrary order for font keys.
								inline bool operator < ( 
										const CharColorQualityKey & other ) 
									const throw() 
								{

									if ( _character < other._character )
										return true ;
		
									if ( _character > other._character )
										return false ;
		
									if ( Pixels::isLess( _color, 
											other._color ) )
										return true ;
									
									if ( Pixels::isLess( other._color, 
											_color ) )
										return false ;
									
									return _quality < other._quality ;
																			
								}	
								
								
							private:								
							
								Ceylan::Latin1Char _character ;
								
								Pixels::ColorDefinition _color ;
									
								RenderQuality _quality ;
						
						
						} ;
						
								

						/**
						 * Cache key to manage colored strings, both for 
						 * word only and full text.
						 *
						 */
						class OSDL_DLL StringColorQualityKey
						{
						

							public:
								
								
								StringColorQualityKey( 
									const std::string & text, 
									Pixels::ColorDefinition color, 
									RenderQuality quality )
										throw() :
									_text( text ),
									_color( color ),
									_quality( quality )
								{
								
								}

								
								inline bool operator < ( 
										const StringColorQualityKey & other ) 
									const throw() 
								{	
									
									/*
									 * Comparisons between strings are done
									 * lexicographically.
									 *
									 */
									
									if ( _text < other._text )
										return true ;
		
									if ( _text > other._text )
										return false ;
		
									if ( Pixels::isLess( _color, 
											other._color ) )
										return true ;
									
									if ( Pixels::isLess( other._color,
											 _color ) )
										return false ;
									
									return _quality < other._quality ;
		
								}	
								
								
							private:								
							
								std::string _text ;
								Pixels::ColorDefinition _color ;	
								RenderQuality _quality ;
								
						
						} ;
						


					
						/**
						 * As rendering glyphs can demand a lot of resources,
						 * they can be put in cache so that the same request 
						 * is not rendered twice.
						 *
						 * The various settings for the render cache are :
						 *
						 * - None : when no caching is wanted (prefer memory 
						 * to CPU)
						 * - GlyphCached : put in cache individual glyphs
						 * - WordCached : put in cache full words. This can be
						 * useful for example with Truetype fonts, since a 
						 * word cannot be easily rendered from the set of
						 * its glyphs, due, for instance, to kerning.
						 * - TextCached : allows for full lines, including
						 * whitespaces, to be cached as blocks. Useful when 
						 * the same sequence of words has to be rendered
						 * multiple times.
						 *
						 */
						enum RenderCache { 
							None, GlyphCached, WordCached, TextCached } ;
						
					
						/**
						 * Describes which of the cache policies made 
						 * available by the Smart Resource manager are 
						 * allowed for fonts.
						 *
						 * @see Ceylan::SmartResourceManager for the 
						 * description of the various allowed policies. 
						 *
						 * This enum is a convenient way too of overcoming 
						 * the impossibility of specifying a cache policy
						 * independently of the key chosen to instantiate
						 * the manager : this is an additional level of
						 * indirection, so that in the font constructor the
						 * cache policy can be specified directly, and
						 * not as :
						 * Ceylan::SmartResourceManager
						 * <StringColorQualityKey>::CachePolicy
						 * for example, which would prevent to be able to
						 * specify a Ceylan::SmartResourceManager
						 * <CharColorQualityKey>::CachePolicy needed 
						 * for the glyph cache.
						 *
						 */
						enum AllowedCachePolicy { 
							NeverDrop, DropLessRequestedFirst } ;
						
						
						/**
						 * Creates an abstract font.
						 *
						 * @param convertToDisplay tells whether a returned
						 * rendering should have already been converted to
						 * display. 
						 * Any cached rendering follows this choice too. 
						 * Pre-converting is useful for classical 2D rendering,
						 * useless for OpenGL since surfaces will have to be
						 * expressed according to OpenGL convention 
						 * nonetheless.
						 *
						 * If the conversion to display is chosen, then at 
						 * each change of the display format, the renderings
						 * will have to be converted again.
						 *
						 * @param cacheSettings determines what will be put 
						 * in cache in terms of rendering (glyphs, or words, 
						 * or texts, etc.)
						 *
						 * @param cachePolicy determines how the cache should
						 * behave regarding renderings being put in cache 
						 * (store everything, enforce a maximum size in 
						 * memory, etc.)
						 *
						 * @param quota Should an upper-bound to the memory 
						 * size of cached renderings apply (depending on the
						 * chosen cache policy), determines its actual value. 
						 * If a null (0) quota is passed, then the default 
						 * value of the quota for the selected cache 
						 * settings will be chosen. 
						 * For example, if cacheSettings is 'WordCached' 
						 * and cachePolicy is 'DropLessRequestedFirst' (this
						 * policy uses a quota), and if a null quota is
						 * specified, then the actual quota being used will be
						 * 'DefaultWordCachedQuota'.
						 *
						 */
						explicit Font( 
							bool convertToDisplay = true, 
							RenderCache cacheSettings = GlyphCached, 
							AllowedCachePolicy cachePolicy 
								= DropLessRequestedFirst,
							Ceylan::System::Size quota = 0 ) throw() ;
						
						
						/// Virtual destructor.
						virtual ~Font() throw() ;

						
						/**
						 * Returns the current rendering style.
						 *
						 */
						virtual RenderingStyle getRenderingStyle() const
							throw() ;
						
						
						/**
						 * Sets the current rendering style for this font.
						 *
						 * @throw TextException if the specified style is 
						 * not supported.
						 *
						 * @note Some fonts (ex : some fixed fonts) support 
						 * only the rendering style they had at their creation.
						 * In this case, this method throws systematically 
						 * an exception if the rendering style is changed.
						 * The solution would be to create a new Font subclass
						 * with the desired rendering style, instead of
						 * changing an already existing font instance.
						 *
						 */
						virtual void setRenderingStyle( 
							RenderingStyle newStyle ) throw( TextException ) ;
						

						/**
						 * Sets the current background color for this font.
						 *
						 * @param newBackgroundColor the new background color.
						 *
						 * @note The concept of background color only applies
						 * when the 'Shaded' quality is chosen, since with 
						 * other rendering qualities the background is
						 * transparent.
						 *
						 */
						virtual void setBackgroundColor( 
								Pixels::ColorDefinition newBackgroundColor ) 
							throw() ;	
							
					
						/**
						 * Returns the current background color.
						 *
						 * @note The concept of background color only applies
						 * when the 'Shaded' quality is chosen, since with 
						 * other rendering qualities the background is
						 * transparent.
						 *
						 */		
						Pixels::ColorDefinition getBackgroundColor() 
							const throw() ;
												
						
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
						 * @throw TextException if the glyph metrics 
						 * could not be retrieved.
						 *
						 */
						virtual Width getWidth( Ceylan::Latin1Char character ) 
							const throw( TextException) = 0 ;
						 
						 
						/**
						 * Returns the width of the abscissa offset for the
						 * specified glyph, rendered with this font. 
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
								Ceylan::Latin1Char character ) 
							const throw( TextException ) = 0 ;


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
								Ceylan::Latin1Char character )
							const throw( TextException ) = 0 ;

						 
						/**
						 * Returns the advance of the specified glyph, 
						 * rendered with this font, which is the space 
						 * between the leftmost part of the glyph bounding 
						 * box and the same part of the next glyph. 
						 *
						 * It includes therefore the width of this glyph 
						 * and the space between the next glyph that could be 
						 * rendered afterwards. 
						 *
						 * @param character the character whose advance will 
						 * be returned.
						 *
						 * @throw TextException if the glyph metrics could 
						 * not be retrieved.
						 *
						 */
						virtual SignedLength getAdvance( 
								Ceylan::Latin1Char character ) 
							const throw( TextException ) = 0 ;
						 
						 
						/**
						 * Returns the width of the recommended space 
						 * between two glyphs.
						 *
						 * Simple text rendering can rely on constant space
						 * between glyphs, but for more complex cases such 
						 * as multi-line printing, more complex rules should 
						 * be used.
						 *
						 * @return a value computed that is an evaluation 
						 * which may not be consistent with the 
						 * character-based advances that can be requested.
						 *
						 */
						virtual Width getInterGlyphWidth() const throw() ;
						 
												
						/**
						 * Returns the maximum pixel height of all glyphs 
						 * of this font. 
						 *
						 * You may use this height for rendering text as c
						 * lose together vertically as possible, though 
						 * adding at least one pixel height to it will space
						 * it, so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing.
						 *
						 * @see getLineSkip
						 *
						 */
						virtual Height getHeight() const throw() = 0 ;
						
						
						 
						/**
						 * Returns the maximum pixel ascent (height above
						 * baseline) of all glyphs of this font.
						 *
						 * You may use this height for rendering text as c
						 * lose together vertically as possible, though 
						 * adding at least one pixel height to it will space
						 * it, so they cannot touch. 
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing.
						 *
						 * @see getLineSkip
						 *
						 */
						virtual SignedHeight getAscent() const throw() = 0 ;
						 
						 
						/**
						 * Returns the maximum pixel descent (height below
						 * baseline) of all glyphs of this font. 
						 * 
						 * If at least a glyph has parts below the abscissa
						 * axis, the descent is negative.
						 *
						 * It could be used when drawing an individual glyph
						 * relative to a bottom point, by combining it 
						 * with the glyph's vertical metrics to resolve the top 
						 * of the rectangle used when blitting the glyph 
						 * on the screen.
						 *
						 * Remember that multiline printing is not enabled, 
						 * so the user is responsible for line spacing.
						 *
						 * @see getLineSkip
						 *
						 */
						virtual SignedHeight getDescent() const throw() = 0 ;
						 
						 
						/**
						 * Returns the recommended pixel height of a 
						 * rendered line of text of this font. 
						 *
						 * It is usually larger than the value returned by
						 * getHeight.
						 *
						 */
						virtual Height getLineSkip() const throw() = 0 ;
						
						
						
						/**
						 * Returns the current alinea width, expressed in
						 * pixels.
						 *
						 */
						virtual Width getAlineaWidth() const throw() ;
						
						
						/**
						 * Sets the current alinea width, in pixels.
						 *
						 * Setting a null (zero) value disables the alinea
						 * feature.
						 *
						 * @param newAlineaWidth the new value, in pixel, 
						 * for the alinea.
						 *
						 */
						virtual void setAlineaWidth( Width newAlineaWidth )
							throw() ;	
						 
						 
						 
						/**
						 * Returns a text describing the metrics of the 
						 * glyph corresponding to the specified character.
						 *
						 * @param character the character whose glyph is to
						 * be described.
						 *
						 * @return a description of the corresponding glyph.
						 *
						 */
						virtual std::string describeGlyphFor( 
								Ceylan::Latin1Char character )
							const throw() ;
						
						
						
						// Render section.
							 							 

						/**
						 * Renders specified glyph (Latin-1 character) in
						 * specified color, in a returned new surface.
						 *
						 * Depending on the settings, the returned surface 
						 * can be a clone of a pre-rendered (cached) 
						 * character or, if not available, be put in cache
						 * itself so that it is rendered once. 
						 *
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
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller. 
						 * This surface is encoded with RLE colorkey for 
						 * maximum efficiency.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual Surface & renderLatin1Glyph( 
								Ceylan::Latin1Char character, 
								RenderQuality quality = Solid, 
								Pixels::ColorDefinition glyphColor 
									= Pixels::White ) 
							throw( TextException ) = 0 ;


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
						 *
						 * @param glyphColor the color definition for the 
						 * glyph. 
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1Glyph( 
								Surface & targetSurface,
								Coordinate x, 
								Coordinate y, 
								Ceylan::Latin1Char character, 
								RenderQuality quality = Solid, 
								Pixels::ColorDefinition glyphColor 
									= Pixels::White )  
							throw( TextException )= 0 ;
							
								
								
						/**
						 * Renders specified text, encoded in Latin-1, in
						 * specified color, in a returned surface, in one 
						 * line that will not be broken (no multi-line).
						 *
						 * The caller is responsible for deleting the 
						 * returned surface.
						 *
						 * @param text the text, encoded in Latin-1, to render.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller.
						 *
						 * @throw TextException on error, for example if a
						 * specified glyph was not found.
						 *
						 */
						virtual Surface & renderLatin1Text( 
								const std::string & text, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White ) 
							throw( TextException ) ;
							
							
							
						/**
						 * Blits specified text, encoded in Latin-1, in
						 * specified color, on specified location of given
						 * surface, in one line that will not be broken 
						 * (no multi-line).
						 *
						 * @param targetSurface the surface the text will 
						 * be blitted to.
						 *
						 * @param x the abscissa in target surface of the
						 * top-left corner of the 
						 * text blit.
						 *
						 * @param y the ordinate in target surface of the
						 * top-left corner of the text blit.
						 *
						 * @param text the text, encoded in Latin-1, to render.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @throw TextException on error, for example if a
						 * specified glyph was not found.
						 *
						 */
						virtual void blitLatin1Text( 
								Surface & targetSurface,
								Coordinate x, 
								Coordinate y, 
								const std::string & text, 
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White ) 
							throw( TextException ) ;
							

								
						/**
						 * Renders specified text, encoded in Latin-1, in
						 * specified color, in a returned surface of 
						 * specified dimensions. 
						 *
						 * The text will be rendered on as many lines as 
						 * needed, and will be automatically justified 
						 * if requested. 
						 *
						 * If the text is too long to fit in the box, then 
						 * all the words that fit in will be rendered. 
						 *
						 * If a word is too wide to be rendered at all in 
						 * the box, then the rendering will stop before it.
						 *
						 * This method will return in 'renderIndex' the 
						 * index of the first character from the specified 
						 * text that could not have been rendered, if any. 
						 *
						 * Words are not broken : only full words are rendered.
						 *						 
						 * The caller is responsible for deleting the returned
						 * surface.
						 *
						 * @note Multiline text rendering is recommended for 
						 * use with one of the two most useful cases 
						 * regarding text caches : the 'WordCached' and
						 * 'TextCached' policies. 
						 * Using 'GlyphCached' or no cache at all works but
						 * is far less efficient. 
						 * Notably, a temporary word cache will be used,
						 * which may use more peak memory space than wanted.
						 * 
						 * @param width the width of the box in which the 
						 * text is to be rendered.
						 *
						 * @param height the height of the box in which the 
						 * text is to be rendered.
						 *
						 * @param text the text, encoded in Latin-1, to 
						 * render.
						 *
						 * @param renderIndex the method will assign this
						 * user-provided variable the index of the first
						 * character that could not be rendered. 
						 * Hence if the full text could be rendered in the
						 * container, then 'renderIndex' will be equal to
						 * the length of 'text'.
						 *
						 * For example, if the text is 'Hello OSDL' and only
						 * 'Hello' could be rendered, then 'renderIndex'
						 * (starting from zero) will be assigned to 6, the 
						 * index of the 'O' of OSDL (leading spaces are
						 * ignored).
						 *
						 * @param lastOrdinateUsed the method will assign 
						 * this user-provided variable the ordinate of 
						 * the last row used in the returned surface : 
						 * depending on the text, the requested surface will 
						 * be filled at different levels. 
						 * This ordinate is useful for example to align
						 * vertically a text in its container. 
						 * No ordinate beyond container height can be returned.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @param justified tells whether the text should be
						 * justified, i.e. should be vertically aligned on 
						 * both edges.
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual Surface & renderLatin1MultiLineText( 
								Length width, 
								Length height,
								const std::string & text,
								TextIndex & renderIndex,
								Coordinate & lastOrdinateUsed,
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White,
								bool justified = true ) 
							throw( TextException ) ;


						/**
						 * Blits specified text, encoded in Latin-1, in
						 * specified color, in specified location of 
						 * specified surface, in a box of specified
						 * dimensions. 
						 *
						 * The text will be rendered on as many lines as 
						 * needed, and will be automatically justified. 
						 * If the text is too long to fit in the box, then
						 * all the words that fit in will be rendered.
						 *
						 * This method will return in 'renderIndex' the index 
						 * of the first character from the specified text 
						 * that could not have been rendered, if any. 
						 *
						 * Words are not broken : only full words are rendered.
						 *						 
						 * @note Multiline text rendering is for the moment 
						 * only implemented for the two most useful cases
						 * regarding text caches : this rendering is 
						 * available for the 'WordCached' and 'TextCached'
						 * policies. 
						 * It is not implemented for 'GlyphCached', or no 
						 * cache at all, yet.
						 *
						 * @param targetSurface the surface the text will be
						 * blitted to.
						 *
						 * @param clientArea a rectangle enclosing the usable
						 * area for text rendering in <b>targetSurface</b>,
						 * expressed in the referential of this surface.
						 *
						 * @param text the text, encoded in Latin-1, to render.
						 *
						 * @param renderIndex the method will assign this
						 * user-provided variable the index of the first
						 * character that could not be rendered. 
						 * Hence if the full text could be rendered, then
						 * 'renderIndex' will be equal to the length of 'text'.
						 *
						 * For example, if the text is 'Hello OSDL' and only
						 * 'Hello' could be rendered, then 'renderIndex'
						 * (starting from zero) will be assigned to 6, the 
						 * index of the 'O' of OSDL (leading spaces are
						 * ignored).
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @param justified tells whether the text should be
						 * justified, i.e. should be vertically aligned on 
						 * both edges.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1MultiLineText( 
								Surface & targetSurface,
								const UprightRectangle & clientArea, 
								const std::string & text,
								TextIndex & renderIndex,
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White,
								bool justified = true ) 
							throw( TextException ) ;


						/**
						 * Blits specified text, encoded in Latin-1, in
						 * specified color, in specified location of specified
						 * surface, in a box of specified dimensions. 
						 *
						 * The text will be rendered on as many lines as 
						 * needed, and will be automatically justified. 
						 * If the text is too long to fit in the box, then
						 * all the words that fit in will be rendered.
						 *
						 * This method will return in 'renderIndex' the index 
						 * of the first character from the specified text 
						 * that could not have been rendered, if any. 
						 *
						 * Words are not broken : only full words are rendered.
						 *						 
						 * @note Multiline text rendering is for the moment 
						 * only implemented for the two most useful cases
						 * regarding text caches : this rendering is 
						 * available for the 'WordCached' and 'TextCached'
						 * policies. 
						 * It is not implemented for 'GlyphCached', or no 
						 * cache at all, yet.
						 *						 
						 * @param targetSurface the surface the text will be
						 * blitted to.
						 *
						 * @param x the abscissa in target surface of the
						 * top-left corner of the text blit.
						 *
						 * @param y the ordinate in target surface of the
						 * top-left corner of the text blit.
						 *
						 * @param width the width of the box in which the
						 * text is to be rendered.
						 *
						 * @param height the height of the box in which the 
						 * text is to be rendered.
						 *
						 * @param text the text, encoded in Latin-1, to render.
						 *
						 * @param renderIndex the method will assign this
						 * user-provided variable the index of the first
						 * character that could not be rendered. 
						 * Hence if the full text could be rendered, then
						 * 'renderIndex' will be equal to the length of 'text'.
						 *
						 * For example, if the text is 'Hello OSDL' and only
						 * 'Hello' could be rendered, then 'renderIndex'
						 * (starting from zero) will be assigned to 6, the 
						 * index of the 'O' of OSDL (leading spaces are
						 * ignored).
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @param justified tells whether the text should be
						 * justified, i.e. should be vertically aligned on 
						 * both edges.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1MultiLineText( 
								Surface & targetSurface,
								Coordinate x, 
								Coordinate y,
								Length width, 
								Length height,
								const std::string & text,
								TextIndex & renderIndex,
								RenderQuality quality = Solid,
								Pixels::ColorDefinition textColor 
									= Pixels::White,
								bool justified = true ) 
							throw( TextException ) ;



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
							const throw() ;

						
						
						
						// Static section.
						
												
						/**
						 * The name of the environment variable that may 
						 * contain directory names that should contain font
						 * files.
						 *
						 */
						static std::string FontPathEnvironmentVariable  ;
						
						
						/**
						 * Allows to keep track of font directories.
						 *
						 * Automatically gathers the list of directories
						 * specified as the value of the environment 
						 * variable named as specified in 
						 * FontPathEnvironmentVariable (FONT_PATH).
						 * 
						 */
						static Ceylan::System::FileLocator FontFileLocator ;
						
						
						/**
						 * Returns a description of the specified 
						 * rendering style.
						 *
						 * @param style the rendering style to describe.
						 *
						 */
						static std::string InterpretRenderingStyle(
							RenderingStyle style ) throw() ;
						
						
						/*
						 * Rendering styles can be combined for some fonts : 
						 * 'Bold | Italic' selects for example a bold italic
						 * rendering style.
						 * 
						 */
						
						/// Normal font rendering style.
						static const RenderingStyle Normal ;
				
								
						/// Normal font rendering style.
						static const RenderingStyle Bold ;
				
						
						/// Italic font rendering style.
						static const RenderingStyle Italic ;
				
						
						/// Underline font rendering style.
						static const RenderingStyle Underline ;


						/**
						 * Defines the default quota value (maximum size 
						 * of cached surfaces in memory, in bytes) if 
						 * the cache is glyph-based.
						 *
						 * This default quota is equal to 4 megabytes.
						 *
						 */
						static const Ceylan::System::Size
							DefaultGlyphCachedQuota ;
						
						
						/**
						 * Defines the default quota value (maximum size 
						 * of cached surfaces in memory, in bytes) if the 
						 * cache is word-based.
						 *
						 * This default quota is equal to 6 megabytes.
						 *
						 */
						static const Ceylan::System::Size 
							DefaultWordCachedQuota ;
						
						
						/**
						 * Defines the default quota value (maximum size 
						 * of cached surfaces in memory, in bytes) if 
						 * the cache is text-based.
						 *
						 * This default quota is equal to 8 megabytes.
						 *
						 */
						static const Ceylan::System::Size 
							DefaultTextCachedQuota ;
						
												
						/**
						 * Defines the default width for an alinea as 
						 * a multiple of the width of a space.
						 *
						 */
						static const Ceylan::Uint8 
							DefaultSpaceBasedAlineaWidth ;
						
						
						
						
					protected:


						
						/**
						 * Renders specified Latin-1 text with specified
						 * quality, in specified color, so that the word 
						 * cache is used.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 * @param text the Latin-1 text to render.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						Surface & renderLatin1TextWithWordCached( 
								const std::string & text,
								RenderQuality quality, 
								Pixels::ColorDefinition textColor ) 
							throw( TextException ) ;
							
							
							
						/**
						 * Renders specified Latin-1 text with specified
						 * quality, in specified color, so that the text 
						 * cache is used.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 * @param text the Latin-1 text to render.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @return a newly allocated Surface, whose 
						 * ownership is transferred to the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						Surface & renderLatin1TextWithTextCached( 
								const std::string & text,
								RenderQuality quality, 
								Pixels::ColorDefinition textColor ) 
							throw( TextException ) ;
							

						
						/**
						 * Blits specified word, encoded with Latin-1
						 * characters, in specified color, with specified
						 * quality, on specified location of given surface.
						 *
						 * The cache is interrogated so that a previous
						 * rendering for this word can be used again. 
						 * If not, the rendering is performed and proposed to 
						 * the word cache, if activated.
						 *
						 * @param targetSurface the surface the word will 
						 * be blitted to.
						 *
						 * @param x the abscissa in target surface of 
						 * the top-left corner of the word blit.
						 *
						 * @param y the ordinate in target surface of the
						 * top-left corner of the word blit.
						 *
						 * @param word the Latin-1 encoded word to render.
						 *
						 * @param quality the chosen rendering quality. 
						 * For fixed font, only the 'Solid' quality is
						 * available.
						 *
						 * @param wordColor the color definition for the word. 
						 *
						 * @return the width of the blit surface.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual void blitLatin1Word( 
								Surface & targetSurface,
								Coordinate x, 
								Coordinate y, 
								const std::string & word, 
								RenderQuality quality = Solid, 
								Pixels::ColorDefinition wordColor 
									= Pixels::White ) 
							throw( TextException ) ;
						


						/**
						 * Ensures that specified word rendering is in 
						 * text cache, and returns a 'const' reference to it :
						 * if the rendering is not already in text cache,
						 * it will be added.
						 *
						 * This helper method is especially useful for blits,
						 * since otherwise they would have to call a render
						 * method which would create a copy for the
						 * cache, whereas blits can use the cached entry
						 * directly. 
						 * It saves an useless copy/deleting. 
						 *
						 * It does not check the cache for a previous entry
						 * since its purpose is to feed the text cache. 
						 * It does that independently of the cache settings
						 * (glyph, word, text, etc.). 
						 * If the 'NeverDrop' cache policy is chosen, then 
						 * one should ensure the corresponding cache key 
						 * is not already in use.
						 *
						 * @param word the Latin-1 encoded word to have
						 * rendered.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param wordColor the color definition for the word. 
						 *
						 * @return a 'const' reference to a surface which is 
						 * a rendering of the specified word. 
						 * The surface is still owned by the cache, and should 
						 * therefore not be deallocated by the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						virtual const Surface & getConstLatin1WordFromCache( 
								const std::string & word, 
								RenderQuality quality,
								Pixels::ColorDefinition wordColor )
							throw( TextException ) ;

							
						/**
						 * Renders specified Latin-1 text with specified
						 * quality, in specified color, directly thanks to 
						 * the font backend : no cache is taken into account.
						 *
						 * This method is meant to be used as an helper 
						 * function so that user-exposed methods can be 
						 * easily implemented.
						 *
						 * @param text the Latin-1 text to render.
						 *
						 * @param quality the chosen rendering quality.
						 *
						 * @param textColor the color definition for the text. 
						 *
						 * @return a newly allocated Surface, whose ownership 
						 * is transferred to the caller.
						 *
						 * @throw TextException on error.
						 *
						 */
						Surface & basicRenderLatin1Text( 
								const std::string & text,
								RenderQuality quality, 
								Pixels::ColorDefinition textColor ) 
							throw( TextException ) ;
							
							
						/**
						 * Records the current rendering style (Normal, Bold,
						 * etc.) of this font.
						 *
						 */
						RenderingStyle _renderingStyle ;
					
					
						/**
						 * Tells whether returned and cached renderings 
						 * should be converted to display beforehand.
						 *
						 */
						bool _convertToDisplay ;
						
																		
						/// Describes the settings of the render cache.
						RenderCache _cacheSettings ;

						 
						/**
						 * The smart resource manager that would cache 
						 * rendered glyphs, should the GlyphCached 
						 * render cache be selected. 
						 *
						 * The keys used to specify a glyph are made 
						 * of a Latin-1 character and a color specification.
						 *
						 */
						Ceylan::SmartResourceManager<CharColorQualityKey> *
							_glyphCache ;
						
						
						/**
						 * The smart resource manager that would cache 
						 * rendered words and/or text, should the 
						 * WordCached or TextCached render cache be selected. 
						 *
						 * The keys used to specify a glyph are made of a
						 * Latin-1 encoded string and a color specification.
						 *
						 */
						Ceylan::SmartResourceManager<StringColorQualityKey> *
							_textCache ;

						
						/**
						 * Defines what is the background color, for 
						 * 'Shaded' quality.
						 *
						 */
						Pixels::ColorDefinition _backgroundColor ;
						
						
						/**
						 * Records the width of a space, in pixels, if not 
						 * null.
						 *
						 * Useful for example with word cache, since two
						 * successive words have to be separated by such
						 * interval.
						 *
						 * @note All child classes should initialize it
						 * appropriately, in their constructors.
						 *
						 */
						Width _spaceWidth ;
							
							
						/**
						 * Records the width of an alinea space, in pixels, 
						 * to be put at the beginning of a paragraph.
						 *
						 * @note All child classes should initialize it
						 * appropriately, in their constructors.
						 *
						 */
						Width _alineaWidth ;
							
				
				
					private:
					
				
									
						/**
						 * Copy constructor made private to ensure that 
						 * it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined constructor is called, implicitly or not.
						 *
						 */			 
						Font( const Font & source ) throw() ;
			
			
						/**
						 * Assignment operator made private to ensure 
						 * that it will be never called.
						 *
						 * The compiler should complain whenever this 
						 * undefined operator is called, implicitly or not.
						 *
						 */			 
						Font & operator = ( const Font & source ) throw() ;
						
						
				} ;							
				
			} 
			
		}	
			
	}
	
}		



#endif // OSDL_FONT_H_

