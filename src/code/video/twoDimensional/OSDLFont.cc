#include "OSDLFont.h"

#include "OSDLSurface.h"         // for Surface
#include "OSDLPixel.h"           // for ColorDefinition

#include "SDL_gfxPrimitives.h"   // for stringColor

#include "SDL_ttf.h"             // for TTF_STYLE_NORMAL and others

#include <list>


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>          // for OSDL_DEBUG_FONT and al 
#endif // OSDL_USES_CONFIG_H


using std::list ;
using std::pair ;

using std::string ;


using namespace Ceylan ;
using namespace Ceylan::Log ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;



string Text::Font::FontPathEnvironmentVariable = "FONT_PATH" ;


Ceylan::System::FileLocator Text::Font::FontFileLocator(
	FontPathEnvironmentVariable ) ;



TextException::TextException( const string & reason ) throw():
	VideoException( "TextException: " + reason ) 
{

}
	
	
TextException::~TextException() throw()
{

}




using namespace OSDL::Video::TwoDimensional::Text ;




// Font section.


const RenderingStyle Font::Normal    = TTF_STYLE_NORMAL    ;
const RenderingStyle Font::Bold      = TTF_STYLE_BOLD      ;
const RenderingStyle Font::Italic    = TTF_STYLE_ITALIC    ;
const RenderingStyle Font::Underline = TTF_STYLE_UNDERLINE ;


const Ceylan::System::Size Font::DefaultGlyphCachedQuota = 4 * 1024 * 1024 ;
const Ceylan::System::Size Font::DefaultWordCachedQuota  = 6 * 1024 * 1024 ;
const Ceylan::System::Size Font::DefaultTextCachedQuota  = 8 * 1024 * 1024 ;

const Ceylan::Uint8 Font::DefaultSpaceBasedAlineaWidth = 6 ;



Font::Font( 
		bool convertToDisplay, 
		RenderCache cacheSettings, 
		AllowedCachePolicy cachePolicy, 
		Ceylan::System::Size quota 
			) throw():			
	_renderingStyle( Normal ),
	_convertToDisplay( convertToDisplay ),
	_cacheSettings( cacheSettings ),
	_glyphCache( 0 ),
	_textCache( 0 ),
	_backgroundColor( Pixels::Black ),
	_spaceWidth( 0 ),
	_alineaWidth( 0 )
{
	
	/*
	 * _spaceWidth and _alineaWidth cannot be initialized in this Font
	 *  mother class since getWidth is abstract for it (pure virtual). 
	 * Therefore each child class should initialize it.
	 *
	 */
	
	switch( _cacheSettings )
	{
	
	
		/*
		 * The several sub-switches allow to get rid of following parameter 
		 * in signature:
		 * 'Ceylan::SmartResourceManager<CharColorQualityKey>::CachePolicy
		 * cachePolicy = Ceylan::SmartResourceManager<CharColorQualityKey>::
		 * DropLessRequestedFirst' which would not be compliant with a
		 * SmartResourceManager<StringColorQualityKey>, which is also needed. 
		 *
		 * The abstract AllowedCachePolicy is therefore to be converted 
		 * to its templated version.
		 *
		 */
		 		 
		case None:
		
#if OSDL_DEBUG_FONT
			LogPlug::debug( "Font created with no rendering cache." ) ;
#endif // OSDL_DEBUG_FONT

			// Nothing to do.
			break ;
			
			
		case GlyphCached:

#if OSDL_DEBUG_FONT
			LogPlug::debug( "Font created with glyph rendering cache." ) ;
#endif // OSDL_DEBUG_FONT

			if ( quota == 0 )
				quota = DefaultGlyphCachedQuota ;
			SmartResourceManager<CharColorQualityKey>::CachePolicy
				actualGlyphPolicy ;
				
			/*
			 * Let's convert our allowed cache policy into a 
			 * templated-ready cache policy:
			 *
			 */
			switch( cachePolicy )
			{	
				case NeverDrop:
					actualGlyphPolicy =
						SmartResourceManager<CharColorQualityKey>::NeverDrop ;
					break ;
				
				case DropLessRequestedFirst:
					actualGlyphPolicy =
			SmartResourceManager<CharColorQualityKey>::DropLessRequestedFirst ;
					break ;
				
				default:
					Ceylan::emergencyShutdown(
						"OSDL::Video::TwoDimensional::Font constructor "
						"with glych cache: forbidden cache settings" ) ;		
						
					break ;	
			}		
			
			_glyphCache = new SmartResourceManager<CharColorQualityKey>( quota, 
				actualGlyphPolicy ) ;
			break ;
		
		
		case WordCached:

#if OSDL_DEBUG_FONT
			LogPlug::debug( "Font created with word rendering cache." ) ;
#endif // OSDL_DEBUG_FONT

			if ( quota == 0 )
				quota = DefaultWordCachedQuota ;
				
			SmartResourceManager<StringColorQualityKey>::CachePolicy
				actualWordPolicy ;
				
			/*
			 * Let's convert our allowed cache policy into a 
			 * templated-ready cache policy:
			 *
			 */
			switch( cachePolicy )
			{	
			
				case NeverDrop:
					actualWordPolicy =
						SmartResourceManager<StringColorQualityKey>::NeverDrop ;
					break ;
				
				case DropLessRequestedFirst:
					actualWordPolicy =
		SmartResourceManager<StringColorQualityKey>::DropLessRequestedFirst ;
					break ;
				
				default:
					Ceylan::emergencyShutdown(
						"OSDL::Video::TwoDimensional::Font constructor "
						"with word cache: forbidden cache settings" ) ;		
						
					break ;	
			}		
			
			_textCache = new SmartResourceManager<StringColorQualityKey>( 
				quota, actualWordPolicy ) ;
			break ;
			
			
		case TextCached:

#if OSDL_DEBUG_FONT
			LogPlug::debug( "Font created with text rendering cache." ) ;
#endif // OSDL_DEBUG_FONT

			if ( quota == 0 )
				quota = DefaultTextCachedQuota ;
				
			SmartResourceManager<StringColorQualityKey>::CachePolicy
				actualTextPolicy ;
				
			/*
			 * Let's convert our allowed cache policy into a 
			 * templated-ready cache policy:
			 *
			 */
			switch( cachePolicy )
			{	
			
				case NeverDrop:
					actualTextPolicy =
						SmartResourceManager<StringColorQualityKey>::NeverDrop ;
					break ;
				
				case DropLessRequestedFirst:
					actualTextPolicy =
		SmartResourceManager<StringColorQualityKey>::DropLessRequestedFirst ;
					break ;
				
				default:
					Ceylan::emergencyShutdown(
						"OSDL::Video::TwoDimensional::Font constructor "
						"with text cache: forbidden cache settings" ) ;		
					break ;	
					
			}	
				
			_textCache = new SmartResourceManager<StringColorQualityKey>( 
				quota, actualTextPolicy ) ;		
			break ;
			
			
		default:
			Ceylan::emergencyShutdown( 
				"OSDL::Video::TwoDimensional::Font constructor: "
				"unexpected cache settings" ) ;
			break ;
					
	}
	
	// _spaceWidth and _alineaWidth to be initialized in child classes.
	
}



Font::~Font() throw()
{

	if ( _glyphCache != 0 )
		delete _glyphCache ;
		
	if ( _textCache != 0 )
		delete _textCache ;
		
}



RenderingStyle Font::getRenderingStyle() const throw()
{

	return _renderingStyle ;
	
}



void Font::setRenderingStyle( RenderingStyle newStyle ) throw( TextException )
{

	_renderingStyle = newStyle ;
	
}




void Font::setBackgroundColor( Pixels::ColorDefinition newBackgroundColor )
	throw()
{

	_backgroundColor = newBackgroundColor ;
	
}



OSDL::Video::Pixels::ColorDefinition Font::getBackgroundColor() const throw()
{

	return _backgroundColor ;
	
}



Width Font::getAlineaWidth() const throw()

{
	return _alineaWidth ;
	
}


void Font::setAlineaWidth( Width newAlineaWidth ) throw()
{

	_alineaWidth = newAlineaWidth ;
	
}



std::string Font::describeGlyphFor( Ceylan::Latin1Char character ) const throw()
{

	list<string> res ;

	res.push_back( "Advance is " 
		+ Ceylan::toString( getAdvance( character ) ) ) ;
	
	res.push_back( "Width is " + Ceylan::toString( getWidth( character ) ) ) ;
	
	res.push_back( "Width offset is " 
		+ Ceylan::toString( getWidthOffset( character ) ) ) ;
	
	res.push_back( "Height above baseline is " 
		+ Ceylan::toString( getHeightAboveBaseline( character ) ) ) ;
		
	return "Informations about the glyph corresponding to the character '" 
		+ Ceylan::toString( character ) + "': " 
		+ Ceylan::formatStringList( res ) ;
		
}


	
Width Font::getInterGlyphWidth() const throw()
{

	// Scales with the letter width, minimum is one:
	static Width inter = static_cast<Width>( 
		Ceylan::Maths::Max<float>( 1, 0.1f * getWidth( 'a' ) ) ) ;

	return inter ;

}



OSDL::Video::Surface & Font::renderLatin1Text( const std::string & text,
		RenderQuality quality, Pixels::ColorDefinition textColor ) 
	throw( TextException )
{

	
	// Different cases, depending on cache settings:

	switch( _cacheSettings )
	{
	
		case None:
		case GlyphCached:
		
			/*
			 * Nothing else to do than full rendering (fire and forget),
			 * glyph that are cached or not are managed directly at the 
			 * glyph level.
			 *
			 */
			return basicRenderLatin1Text( text, quality, textColor ) ;
			break ;
		
		
		case WordCached:
			return renderLatin1TextWithWordCached( text, quality, textColor ) ;
			break ;
			
			
		case TextCached:
			return renderLatin1TextWithTextCached( text, quality, textColor ) ;
			break ;
			
			
		default:
			Ceylan::emergencyShutdown(
				"OSDL::Video::TwoDimensional::Font::renderLatin1Text: "
				"unexpected cache settings" ) ;
			break ;
					
	}
	
	// Avoid a warning:
	throw TextException( "Font::renderLatin1Text: unexpected end of method" ) ;
		
}



void Font::blitLatin1Text( Surface & targetSurface, Coordinate x, Coordinate y, 
		const std::string & text, RenderQuality quality, 
		Pixels::ColorDefinition textColor ) throw( TextException ) 			
{
	 	
	/*
	 * Shortcut:
	 * Tests if one can retrieve and blit its target from the text cache 
	 * instead of creating, blitting and destroying a Surface just for
	 * this blit (if the text cache is used; word cache has far
	 * too few chances of having 'text', it is not interrogated here as a
	 * shortcut):
	 *
	 */ 
	if ( _cacheSettings == TextCached )
	{
	
		StringColorQualityKey renderKey( text, textColor, quality ) ;
			
		const Resource * res = _textCache->get( renderKey ) ;

		
		if ( res != 0 )
		{
		
			const Surface * toReturn = dynamic_cast<const Surface *>( res ) ;
			
#if OSDL_DEBUG_FONT

			LogPlug::debug( "Font::blitLatin1Text: cache hit, "
				"returning clone of prerendered text." ) ;
			
			if ( toReturn == 0 )
				Ceylan::emergencyShutdown( "Font::blitLatin1Text: "
					"clone is not a Surface." ) ;
					
#endif // OSDL_DEBUG_FONT
			
			toReturn->blitTo( targetSurface, x, y ) ;
			
			return ;

		}
	
	}
	
	
	/*
	 * Here we have to ask a full one-shot rendered surface 
	 * (possibly with cache help):
	 *
	 */
	Surface & res = renderLatin1Text( text, quality, textColor ) ;
	
	// Useful for testing: 
	// res.savePNG( Ceylan::toString( y ) + ".png" ) ;
	
	try
	{
		res.blitTo( targetSurface, x, y ) ;
	}
	catch( const VideoException & e )
	{
		// Would not be safe: delete & res ;
		throw TextException( "Font::blitLatin1Text: blit failed: " 
			+ e.toString() ) ;
	}	
	
	delete & res ;
	
}


		
OSDL::Video::Surface & Font::renderLatin1MultiLineText( 
	Length width, Length height, const std::string & text, 
	TextIndex & renderIndex, Coordinate & lastOrdinateUsed,
	RenderQuality quality, Pixels::ColorDefinition textColor, bool justified ) 
		throw( TextException )
{

	/*
	 * @see also Ceylan/trunk/src/code/generic/CeylanTextBuffer.cc
	 *
	 */
	 
	ColorMask redMask, greenMask, blueMask ;
	
	Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
	
	Surface & res = * new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
		width, height, /* bpp */ 32, redMask, greenMask, blueMask, 
		/* no alpha wanted */ 0 ) ;

	/*
	 * Avoid messing text color with color key.
	 * 'Pixels::selectColorDifferentFrom' could be used but here a full 
	 * surface fill can be saved in most cases (as long as the text color 
	 * is not pure black).
	 *
	 */
	Pixels::ColorDefinition colorKey ;
	
	if ( Pixels::areEqual( textColor, Pixels::Black, /* use alpha */ false ) )
	{
		colorKey = Pixels::White ;
		res.fill( colorKey ) ;
	}	
	else
	{
	
		colorKey = Pixels::Black ;
		/*
		 * No need to fill 'res' with black, since new RGB surfaces come 
		 * all black already.
		 *
		 */
		 
	}	
		
	Length lineSkip = getLineSkip() ;
	
	/*
	 * This integer division returns the maximum usable number of lines 
	 * in this box:
	 *
	 */
	LineNumber maxLines = height / lineSkip ;
	
	if ( maxLines == 0 )
		throw TextException( "Font::renderLatin1MultiLineText: box height ("
			+ Ceylan::toString( height ) 
			+ ") is not enough even for one line of text, whose height is "
			+ Ceylan::toString( lineSkip )  + "." ) ;

	renderIndex = 0 ;
	
#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::renderLatin1MultiLineText: " 
		+ Ceylan::toString( maxLines )
		+ " line(s) available to render following text: '" + text + "'." ) ;
		
#endif // OSDL_DEBUG_FONT
	
	
	/*
	 * Auto-centering vertically the usable area for text in its surface 
	 * should not be enforced here since it would hinder placing the 
	 * returned surface exactly where appropriate in its container.
	 *
	 * Hence disabled: 
	 * 'Height lineHeight = ( height - maxLines * lineSkip ) / 2 ;'
	 *
	 */
	Height lineHeight = 0 ; 

	// Each paragraph in turn will be split into a list of words:
	list<string> words ;
	
	string currentWord ;
	
	Length currentWidth, storedWidth ;
	
	Width wordWidth ;
	const Surface * wordSurface ;
		
	
	/*
	 * Having a justified text implies that all words in a line of text 
	 * must be rendered before starting the blits for this line. 
	 *
	 * If word or text caches are used, then word renderings are directly
	 * taken from them: it prevents from creating, rendering and 
	 * destroying uselessly as many surfaces as words.
	 *
	 * If no word nor text cache is used, these renderings have to be kept
	 * nonetheless, whether only a glyph cache is used or not. 
	 * Therefore a word cache is temporarily used to speed up this 
	 * two-pass line rendering. 
	 * Glyph cache, if any, is used as well nonetheless and will be fed,
	 * whereas word cache will be deallocated after this rendering. 
	 *
	 */
	
	bool createTemporaryWordCache = 
		( _cacheSettings == GlyphCached || _cacheSettings == None ) ;
	
	if ( createTemporaryWordCache )
	{
	
#if OSDL_DEBUG_FONT

		if ( _textCache != 0 )
			throw TextException( "Font::renderLatin1MultiLineText: "
				"unable to create temporary word cache 
				"since already existing." ) ;

		LogPlug::debug( "Font::renderLatin1MultiLineText: "
			"creating a temporary word cache" ) ;

#endif // OSDL_DEBUG_FONT

		_textCache = new SmartResourceManager<StringColorQualityKey>(
			DefaultTextCachedQuota, 
	SmartResourceManager<StringColorQualityKey>::DropLessRequestedFirst ) ;

	}

	/*
	 * Hence in all cases we can rely on having a word cache 
	 * (even though it starts empty).
	 *
	 */

	list<string> paragraphs = Ceylan::splitIntoParagraphs( text ) ;

	words = Ceylan::splitIntoWords( paragraphs.front() ) ;
	paragraphs.pop_front() ;
	
	currentWidth = _alineaWidth ;

	list<string> wordsOnTheLine ;

	Width totalWordWidth ;

	bool lineFull ;

	// Here we use a (word) cache, and we draw the lines one by one:
	for ( TextIndex currentLine = 0; currentLine < maxLines; currentLine++ )
	{

		// Save current width for later restore:
		storedWidth = currentWidth ;
	
		/*
		 * Start from the left edge, and select as many words as possible 
		 * within this line:
		 *
		 */
		totalWordWidth = 0 ;

		lineFull = false ;
		wordsOnTheLine.clear() ;
    	   
		while ( ! words.empty() && ! lineFull )
		{
       
			currentWord = words.front() ;
    	   
			// Multiple whitespaces in a row can lead to empty words:
			if ( currentWord.empty() )
			{
				wordSurface = 0 ;
				wordWidth = 0 ;
			}
			else
			{   
				wordSurface = & getConstLatin1WordFromCache( currentWord,
					quality, textColor ) ;
				wordWidth   = wordSurface->getWidth() ;
			}
    	   
			if ( currentWidth + wordWidth <= width ) 
			{
    	   
				// Word accepted for this line:
				renderIndex += 
					static_cast<Text::TextIndex>( currentWord.size() )
					+ /* trailing space */ 1 ;

				totalWordWidth += wordWidth ;
    		   
				if ( justified )
					wordsOnTheLine.push_back( currentWord ) ;
				else 
					if ( wordSurface != 0 )
						wordSurface->blitTo( res, currentWidth, lineHeight ) ;
    				   
				currentWidth += wordWidth + _spaceWidth ;
				words.pop_front() ;
			}
			else
			{
				// With this last word, the line would be too long:
				lineFull = true ;
			}
    							   
		}
       
		// Words are selected for the current line, now time to render them.

		System::Size wordCount = wordsOnTheLine.size() ;
		currentWidth = storedWidth ;
       
		/*
		 * Last part of a paragraph should not be justified: it would 
		 * result in huge inter-word spaces, instead the text can stop 
		 * anywhere before the line's end.
		 * Hence we check that 'words' is not empty.
		 *
		 * Zero word or only one word ? Do nothing special to justify text.
		 * 
		 */
		if ( justified && ! words.empty() && wordCount > 1 )
		{
    										   
			for ( list<string>::const_iterator it = wordsOnTheLine.begin();
				it != wordsOnTheLine.end(); it++ ) 
			{

				if ( (*it ).empty() )
				{
					wordWidth = 0 ;
				}
				else
				{
				
					wordSurface = & getConstLatin1WordFromCache( (*it), 
						quality, textColor ) ;
					   
					wordSurface->blitTo( res, currentWidth, lineHeight ) ;
    			   
					wordWidth = wordSurface->getWidth() ;
					
				}   
    			   
				/*
				 * For justified text, space width is computed with 
				 * pixel-perfect accuracy.
				 * Knowing exactly what words fit with normal space width, 
				 * a new space width is computed so that these words are
				 * dispatched regularly on the line, and begin and end with 
				 * it, provided it is not a paragraph end.
				 *
				 * As this space width has to be an integer, round off errors
				 * would accumulate if a constant corrected space width 
				 * was used, and the last word of the line would not end
				 * perfectly at the end of it, which would lead to a rather
				 * unpleasant visual effect: the right edge of the text 
				 * would not be vertically aligned.
				 *
				 * To correct that, after each word the perfect space width 
				 * for this step is computed, considering only what remains 
				 * to be written. 
				 * Hence the space width is adapted and the text fit 
				 * perfectly on the line.
				 * 
				 * Better round to lowest integer (ceil or static_cast) than 
				 * to nearest, since if space width is rounded up (floor) 
				 * the text might be clipped by the line edge.
				 *
				 * Number of spaces is equal to number of remaining words 
				 * minus one, the width of the current justified space is 
				 * the one that would be chosen if it was divided equally 
				 * for all remaining spaces.
				 *
				 */
				wordCount-- ;
    		   
				currentWidth += wordWidth + /* justified space */ 
					static_cast<Width>( Maths::Round( 
						static_cast<Ceylan::Float32>( 
					   		width - currentWidth - totalWordWidth ) 
								/ wordCount ) ) ;
    			   
				totalWordWidth -= wordWidth ;
    							   
			}
       
		}   
		else 
		{   
       
			// We do not justify text here:
    	   
			for ( list<string>::const_iterator it = wordsOnTheLine.begin();
				it != wordsOnTheLine.end(); it++ ) 
			{
    		   
				if ( ! (*it).empty() )
				{
				
					wordSurface = & getConstLatin1WordFromCache( (*it), 
						quality, textColor ) ;
					wordSurface->blitTo( res, currentWidth, lineHeight ) ;
					currentWidth += wordSurface->getWidth() + _spaceWidth ;
					
				}   
    							   
			}						   

		}

		lineHeight += lineSkip ;

       
		if ( words.empty() )
		{
			
			if ( paragraphs.empty() )
				break ;
			words = Ceylan::splitIntoWords( paragraphs.front() ) ;
			paragraphs.pop_front() ;
    	   
			// One empty line between paragraphs:
			currentLine++ ;
			lineHeight += lineSkip ;
			currentWidth = _alineaWidth ;
    	   
		}   
		else
		{
			currentWidth = 0 ;
		}   
    			   
	}


	if ( createTemporaryWordCache )
	{
		

#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::renderLatin1MultiLineText: "
			"deleting temporary word cache: " + _textCache->toString() ) ;
			
#endif // OSDL_DEBUG_FONT
		
		delete _textCache ;
		_textCache = 0 ;

	}

	
	// No ordinate beyond container height should be retured:
	lastOrdinateUsed = lineHeight ;
	
	// To inspect justified text:
	//res.drawEdges() ;
	
#if OSDL_DEBUG_FONT

	if ( renderIndex == text.size() )
		LogPlug::debug( 
			"Font::renderLatin1MultiLineText: full text fit in box." ) ;
	else	
		LogPlug::debug( "Font::renderLatin1MultiLineText: only " 
			+ Ceylan::toString( renderIndex ) + " characters out of "
			+ Ceylan::toString( text.size() ) 
			+ " characters of the full text could be rendered in the box." ) ;
			
#endif // OSDL_DEBUG_FONT


	/*
	 * Comment out following two lines to see blit blocks 
	 * (as black rectangles):
	 *
	 */
	res.setColorKey( Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
		convertColorDefinitionToPixelColor( res.getPixelFormat(), 
		colorKey ) ) ;


	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey, so we do not choose to add alpha.
		 * Surface will be RLE encoded here:
		 *	
		 */
		res.convertToDisplay( /* alphaChannelWanted */ false ) ;
	}	
	
	return res ;
	
}



void Font::blitLatin1MultiLineText( Surface & targetSurface, 
		const UprightRectangle & clientArea, const std::string & text,
		TextIndex & renderIndex, RenderQuality quality, 
		Pixels::ColorDefinition textColor, bool justified ) 
	throw( TextException )
{

#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::blitLatin1MultiLineText: rendering multiline text '"
		+ text + "' on location " + clientArea.toString() + " of target " 
		+ targetSurface.toString( Ceylan::low ) + "." ) ;
		
#endif // OSDL_DEBUG_FONT
	
	blitLatin1MultiLineText( targetSurface, clientArea.getUpperLeftAbscissa(),
		clientArea.getUpperLeftOrdinate(), clientArea.getWidth(),
		clientArea.getHeight(),	text, renderIndex, quality, 
		textColor, justified ) ;

}



void Font::blitLatin1MultiLineText( Surface & targetSurface, 
		Coordinate x, Coordinate y, Length width, Length height, 
		const std::string & text, TextIndex & renderIndex,
		RenderQuality quality, Pixels::ColorDefinition textColor, 
		bool justified ) 
	throw( TextException )
{
	
	// Not used here:
	Coordinate lastOrdinateUsed ;
	
	/*
	 * Nothing to optimize at this level, user ought cache multiline 
	 * renderings if appropriate.
	 *
	 */
	Surface * res = & renderLatin1MultiLineText( width, height, 
		text, renderIndex, lastOrdinateUsed, quality, textColor, justified ) ;
	
	res->blitTo( targetSurface, x, y ) ;
	
	delete res ;

}	
							
		
																		
const string Font::toString( Ceylan::VerbosityLevels level ) const throw()
{ 

	string res = "Rendering style: " ;
	
	if ( _renderingStyle == Normal )
		res += "normal" ;
	else
	{	
		std::list<string> listRes ;
	
		if ( _renderingStyle & Bold )
			listRes.push_back( "bold" ) ;
		
		if ( _renderingStyle & Italic )
			listRes.push_back( "italic" ) ;
		
		if ( _renderingStyle & Underline )
			listRes.push_back( "underline" ) ;
			
		res += Ceylan::join( listRes, ", " ) ;
		
	}
	
	res += ". Renderings (and caches if activated) are " ;
	
	if ( ! _convertToDisplay )
		res += "not " ; 
	
	res += "automatically converted to display. " ;
	
	switch( _cacheSettings )
	{
		
		case None:
			res += "No render cache used" ;
			break ;
			
		case GlyphCached:
			res += "Glyph renderings are cached" ;
			break ;
	
		case WordCached:
			res += "Word renderings are cached" ;
			break ;
	
		case TextCached:
			res += "Text renderings are cached" ;
			break ;
	
		default:
			res += "Unknown policy for render cache (abnormal)" ;
			break ;
		
	}


	if ( level == Ceylan::low )
		return res ;	
		
		
	// Cache pointers should be ok, hence are not tested beforehand:
	switch( _cacheSettings )
	{
		
		case None:
			break ;
			
		case GlyphCached:
			res += ". Glyph cache state is: " 
				+ _glyphCache->toString( level ) ;
			break ;
	
		case WordCached:
			res += ". Word cache state is: " + _textCache->toString( level )  ;
			break ;
	
		case TextCached:
			res += ". Text cache state is: " + _textCache->toString( level )  ;
			break ;
	
		default:
			break ;
		
	}

	return res ;
	
}



string Font::InterpretRenderingStyle( RenderingStyle style ) throw()
{

	if ( style == Normal )
		return "normal" ;

	std::list<string> res ;
	
	if ( style & Bold )
		res.push_back( "bold" ) ;
				
	if ( style & Italic )
		res.push_back( "italic" ) ;
				
	if ( style & Underline )
		res.push_back( "underline" ) ;
	
	return Ceylan::join( res, ", " ) ;
					
}



OSDL::Video::Surface & Font::renderLatin1TextWithWordCached( 
	const string & text, RenderQuality quality, 
	Pixels::ColorDefinition textColor ) throw( TextException )
{

	/*
	 * The difficulty here is that we cannot know the total width a priori.
	 *
	 * Two passes are therefore needed: one to know the size and create 
	 * the overall surface, the other to blit words onto it.
	 *
	 */
	
	// Splits text into a list of words:
	list<string> words = Ceylan::split( text, ' ' ) ;
	
	
	Length currentWidth = 0 ;
	
#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::renderLatin1TextWithWordCached: will render '"
		+ text + "', space width is " + Ceylan::toString( _spaceWidth ) ) ;
		
#endif // OSDL_DEBUG_FONT
	
	const Surface * wordRendered ;
		
	/*
	 * First iteration: feeds the cache with word renderings and 
	 * computes the total width:
	 *
	 */
	for ( list<string>::const_iterator it = words.begin(); 
		it != words.end(); it++ )
	{
	
		if ( (*it).empty() )
		{

#if OSDL_DEBUG_FONT

			LogPlug::debug( 
				"Font::renderLatin1TextWithWordCached: jumping a space." ) ;
				
#endif // OSDL_DEBUG_FONT

			currentWidth += _spaceWidth ;
			continue ;
		}	
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::renderLatin1TextWithWordCached: examining '" 
			+ (*it) + "'." ) ;
			
#endif // OSDL_DEBUG_FONT
		
		/*
		 * Note that this method is used in the 'word cached' case.
		 * In all cases the returned 'const Surface' is still owned 
		 * by the cache.
		 *
		 */
		wordRendered = & getConstLatin1WordFromCache( (*it), quality, 
			textColor ) ;
		currentWidth += wordRendered->getWidth() + _spaceWidth ;
				
	}
	
	
	// We have the final width, let's create the overall surface:
	
	ColorMask redMask, greenMask, blueMask ;
	Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
	
	Surface & res = * new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
		currentWidth, getLineSkip() - getDescent(), 32, 
		redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;

	// Avoid messing text color with color key:
	Pixels::ColorDefinition colorKey ;
	
	if ( Pixels::areEqual( textColor, Pixels::Black, /* use alpha */ false ) )
	{
	
		colorKey = Pixels::White ;
		res.fill( colorKey ) ;
		
	}	
	else
	{
	
		colorKey = Pixels::Black ;
		/*
		 * No need to fill 'res' with black, since new RGB surfaces 
		 * come all black already.
		 *
		 */
		 
	}	
			
	currentWidth = 0 ;
		
	/*
	 * Second iteration: blit the word renderings. 
	 *
	 * Surfaces from first iteration could have been stored to save 
	 * the efforts needed to find them in cache, but depending on the 
	 * cache quota and policy, it cannot be assumed they are 
	 * are all still available (the last could make the first go out). 
	 *
	 */
	for ( list<string>::const_iterator it = words.begin(); 
		it != words.end(); it++ )
	{
	
		if ( (*it).empty() )
		{
			currentWidth += _spaceWidth ;
			continue ;
		}	
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::renderLatin1TextWithWordCached: blitting '" 
			+ (*it) + "'." ) ;
			
#endif // OSDL_DEBUG_FONT
		
		wordRendered = & getConstLatin1WordFromCache( (*it), 
			quality, textColor ) ;
		wordRendered->blitTo( res, currentWidth, 0 ) ;
		currentWidth += wordRendered->getWidth() + _spaceWidth ;
				
	}
	

	// Comment out following two lines to see blit blocks (as black rectangles):
	res.setColorKey( Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
		convertColorDefinitionToPixelColor( res.getPixelFormat(), colorKey ) ) ;
	
	/*
	 * Uncomment next line to debug computation of bounding boxes for 
	 * renderings:
	 *
	 */
	//res.drawEdges() ;
	
	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey, so we do not choose to add alpha.
		 * Surface will be RLE encoded here:
		 *	
		 */
		res.convertToDisplay( /* alphaChannelWanted */ false ) ;
	}	
	
	return res ;
	
}


	
OSDL::Video::Surface & Font::renderLatin1TextWithTextCached( 
	const string & text, RenderQuality quality, 
	Pixels::ColorDefinition textColor ) throw( TextException )
{

#if OSDL_DEBUG_FONT

	LogPlug::trace( "Font::renderLatin1TextWithTextCached" ) ;
	
#endif // OSDL_DEBUG_FONT
	
	/*
	 * First check that the text-quality-color combination is not already
	 * available in cache:
	 *
	 */
		
	StringColorQualityKey renderKey( text, textColor, quality ) ;
			
	SmartResource * res = _textCache->getClone( renderKey ) ;

		
	if ( res != 0 )
	{
		
		 Surface * returned = dynamic_cast<Surface *>( res ) ;
		 
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::renderLatin1TextWithTextCached: cache hit, "
			"returning clone of prerendered text." ) ;
			
		if ( returned == 0 )
			Ceylan::emergencyShutdown( "Font::renderLatin1TextWithTextCached: "
				"clone is not a Surface." ) ;
				
#endif // OSDL_DEBUG_FONT
			
		return * returned ;
			
	}	
	
#if OSDL_DEBUG_FONT
	LogPlug::debug( "Font::renderLatin1TextWithTextCached: "
		"cache miss, creating new text rendering." ) ;
#endif // OSDL_DEBUG_FONT
		
	// Here it its a cache miss, we therefore have to generate the text:
	Surface & newSurface = basicRenderLatin1Text( text, quality, textColor ) ;
		
	// Give the cache a chance of being fed:		
	_textCache->scanForAddition( renderKey, newSurface ) ;
		
	return newSurface  ; 
	
}	



void Font::blitLatin1Word( Surface & targetSurface, Coordinate x, Coordinate y, 
		const std::string & word, RenderQuality quality, 
		Pixels::ColorDefinition wordColor ) throw( TextException )
{


	/*
	 * We do not expect a given word to be already in cache if text-cached: 
	 * OSDL_WORD_LOOKUP_IN_TEXT_CACHE is not defined by default.
	 *
	 * Hence there are two different cases: either we are word-cached, 
	 * and we may have a rendering in cache, or not.
	 *
	 */
	 
	/*
	 * Uncomment to search in text cache too 
	 * (not recommended since not more efficient): 
	 *
	 */
	//#define OSDL_WORD_LOOKUP_IN_TEXT_CACHE
	
#ifdef OSDL_WORD_LOOKUP_IN_TEXT_CACHE
	if ( _cacheSettings == WordCached || _cacheSettings == TextCached )
#else // OSDL_WORD_LOOKUP_IN_TEXT_CACHE
	if ( _cacheSettings == WordCached )
#endif // OSDL_WORD_LOOKUP_IN_TEXT_CACHE
	{
		
		getConstLatin1WordFromCache( word, quality, wordColor ).blitTo(
			targetSurface, x, y ) ;
										
	}
	else
	{
		// Here we cannot cache words, we blit it as directly as possible:
		basicRenderLatin1Text( word, quality, wordColor ).blitTo( 
			targetSurface, x, y ) ;
	}
	
}


/*
 * This method cannot exist since the caller should never have to deallocate 
 * the returned surface:
 *
const OSDL::Video::Surface & Font::getConstRenderingForLatin1Word( 
	const std::string & word,
	RenderQuality quality, Pixels::ColorDefinition wordColor ) 
		throw( TextException )
{
	
	// 
	
	if ( _cacheSettings == WordCached || _cacheSettings == TextCached )
	{
		return getConstLatin1WordFromCache( word, quality, wordColor ) ;	
	}
	else
	{
		
	}
	
}
*/
		
							
const OSDL::Video::Surface & Font::getConstLatin1WordFromCache( 
	const std::string & word, RenderQuality quality, 
	Pixels::ColorDefinition wordColor ) throw( TextException )
{
	
	// If not in cache, render it and put it in.

	StringColorQualityKey renderKey( word, wordColor, quality ) ;
		
	const Resource * inCache = _textCache->get( renderKey ) ;	
	
	if ( inCache != 0 )
	{
	
		// Found in cache !
		const Surface * wordSurface = dynamic_cast<const Surface *>( inCache ) ;
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::getConstLatin1WordFromCache: "
			"cache hit for '" + word 
			+ "', returning 'const' prerendered word." ) ;
			
		if ( wordSurface == 0 )
			Ceylan::emergencyShutdown( "Font::getConstLatin1WordFromCache: "
				"cache did not return a Surface." ) ;
				
#endif // OSDL_DEBUG_FONT
	
		return * wordSurface ;
		
	}

#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::getConstLatin1WordFromCache: "
		"cache miss for '" + word 
		+ "', rendering it and submitting it to cache." ) ;
		
#endif // OSDL_DEBUG_FONT
	
	// Here we know the word rendering is not in cache, we need to put it in:	
	Surface & wordSurface = basicRenderLatin1Text( word, quality, wordColor ) ;
		
	try
	{ 
		_textCache->takeOwnershipOf( renderKey, wordSurface ) ;
	}
	catch( const ResourceManagerException & e )
	{
	
		/*
		 * This really should never happen: we know this word rendering 
		 * is not in cache.
		 *
		 */
		throw TextException( "Font::getConstLatin1WordFromCache: "
			"cache submitting failed (abnormal): " + e.toString() ) ;

	}
	
	// Cache still owns that (const) surface:		
	return wordSurface ;

}	
							
							

OSDL::Video::Surface & Font::basicRenderLatin1Text( const std::string & text,
		RenderQuality quality, Pixels::ColorDefinition textColor ) 
	throw( TextException )
{

	Length lineSkip = getLineSkip() ;
	Length ascent   = getAscent() ;
	
	System::Size textSize = text.size() ;

#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::basicRenderLatin1Text: rendering '" 
		+ text + "'." ) ;
		
	LogPlug::debug( "Font::basicRenderLatin1Text: line skip is " 
		+ Ceylan::toString( lineSkip ) ) ;
		
	LogPlug::debug( "Font::basicRenderLatin1Text: ascent is " 
		+ Ceylan::toString( ascent ) ) ;
		
#endif // OSDL_DEBUG_FONT
		
	Length * horizSteps = new Length[ textSize ] ;


	/*
	 * When adding a letter y after a letter x, y has to be drawn at: 
	 * abscissa of x plus x's advance, so that if x leaves room in the 
	 * baseline, y can start as left as possible, even if x spreads on the 
	 * right of the position where y starts (ex: if x is a capital 'L' whose
	 * bottom line goes under the baseline).
	 *
	 * However when a letter is the last in a surface, one should use its 
	 $ width instead of its advance, so that the letter is not truncated. 
	 * Moreover, the (n-1) letter might spread more to the right than the 
	 * n one (ex: 'y.', the dot can be placed at the left of the rightmost
	 * branch of the 'y'), so the best solution is to record max width 
	 * at each step (instead of replacing the advance by the width for the 
	 * last glyph).
	 *
	 */
	Length width = 0 ;
	Length maxWidth = 0 ;
	System::Size charCount = 0 ;
			
	bool firstLetter = true ;
	
	Ceylan::Latin1Char currentChar ;		
	SignedLength       currentOffset ;
	Length             currentAdvance ;
	
	/*
	 * First iteration: guess width and height of the resulting surface:
	 * (widths are stored for later use)
	 *
	 */
	for ( string::const_iterator it = text.begin(); it != text.end(); it++ )
	{
	
		currentChar    = static_cast<Ceylan::Latin1Char>(*it) ;
		currentOffset  = getWidthOffset( currentChar ) ;
		currentAdvance = getAdvance( currentChar ) ;
		
		/*
		 * First letters may have a negative offset that should be corrected:
		 * (otherwise leftmost part of first letter could be truncated)
		 *
		 */
		
		if ( firstLetter )
		{
			
			firstLetter = false ;
			
			// Uses 0 instead of currentOffset:			
			horizSteps[charCount] = 0 ;
			
		}
		else
		{	
		
			/*
			 * The offset corresponds to the abscissa of the leftmost 
			 * part of the glyph in its local referential.
			 *
			 */			
			horizSteps[charCount] = width + currentOffset ;
		
		}
		
		charCount++ ;
				
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::basicRenderLatin1Text: adding " 
			+ Ceylan::toString( currentAdvance ) 
			+ " width for char '" +  Ceylan::toString( currentChar ) + "'" ) ;
			
#endif // OSDL_DEBUG_FONT
				
		// The new real abscissa than can be written is: 
		width += currentAdvance ;
					
		/*
		 * Previously using next addition, which leads to too much space 
		 * between letters and not to compact uppercase letters (ex: 'OSDL')
		 * whereas they should be 
		 * (see with cursive fonts with really wide uppercase glyphs, such 
		 * as 'cretino.ttf').
		 *
		 */
		//width += getWidth( currentChar ) + currentOffset ;

	}
	
	/*
	 * Retrieves the rightmost abscissa that could be drawn:  
	 * (character width should be used instead of advance so that its 
	 * rightmost part is not truncated).
	 *
	 */
 	maxWidth = horizSteps[ textSize - 1 ] + currentOffset 
		+ getWidth( currentChar ) ;
			
#if OSDL_DEBUG_FONT

	LogPlug::debug( "Font::basicRenderLatin1Text: text width will be " 
		+ Ceylan::toString( maxWidth ) 
		+ ", height will be " + Ceylan::toString( lineSkip ) + "." ) ;
		 
#endif // OSDL_DEBUG_FONT
	
	/*
	 * Must be filled with glyphs now 
	 * (lineskip is used to avoid complex blits of lines):
	 *
	 */

	ColorMask redMask, greenMask, blueMask ;
	Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
	
	Surface * res ;
	
	try
	{
		res = new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
			maxWidth, lineSkip - getDescent(), 32, 
			redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;
	}
	catch( const VideoException & e )
	{
		delete [] horizSteps ;
		throw TextException( 
			"Font::basicRenderLatin1Text: surface creation failed: "
			+ e.toString() ) ;
	}
	
	// Avoid messing text color with color key:
	Pixels::ColorDefinition colorKey ;
	
	if ( Pixels::areEqual( textColor, Pixels::Black, /* use alpha */ false ) )
	{
		colorKey = Pixels::White ;
		res->fill( colorKey ) ;
	}	
	else
	{
		colorKey = Pixels::Black ;
		/*
		 * No need to fill 'res' with black, since new RGB surfaces come 
		 * all black already.
		 *
		 */
	}	

	
	// Second pass: actual rendering:
	charCount = 0 ;
	
	for ( string::const_iterator it = text.begin(); it != text.end(); it++ )
	{
		currentChar = static_cast<Ceylan::Latin1Char>(*it) ;
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "Font::basicRenderLatin1Text: rendering '" 
			+ Ceylan::toString( currentChar )
			+ "' at width "  + Ceylan::toString( horizSteps[charCount] )
			+ ", at height " 
			+ Ceylan::toString( ascent - getHeightAboveBaseline( currentChar ) )
		) ;
		
#endif // OSDL_DEBUG_FONT
			
		blitLatin1Glyph( *res, horizSteps[charCount], 
			ascent - getHeightAboveBaseline( currentChar ), 
			currentChar, quality, textColor ) ;
			
		charCount++ ;
	}
	
	// Comment out following two lines to see blit blocks (as black rectangles):
	res->setColorKey( Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
		convertColorDefinitionToPixelColor( res->getPixelFormat(), colorKey ) 
	) ;
	
	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey, so we do not choose to add alpha.
		 * Surface will be RLE encoded here:
		 *	
		 */
		res->convertToDisplay( /* alphaChannelWanted */ false ) ;
	}	
	
	/*
	 * Uncomment next line to debug computation of bounding boxes for 
	 * renderings:
	 *
	 */
	//res->drawEdges() ;
	
	delete [] horizSteps ;
	
	return *res ;
	
}

