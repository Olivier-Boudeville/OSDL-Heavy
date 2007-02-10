#include "OSDLTrueTypeFont.h"

#include "OSDLSurface.h"         // for Surface
#include "OSDLPixel.h"           // for ColorDefinition

#include "Ceylan.h"              // for Uint32, inheritance

#include "SDL_gfxPrimitives.h"   // for stringColor
#include "SDL.h"                 // for SDL_Surface

#include <list>
 
 
#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>          // for OSDL_DEBUG_FONT and al 
#endif // OSDL_USES_CONFIG_H



using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;

using namespace Ceylan ;
using namespace Ceylan::Log ;

using std::string ;


Ceylan::System::FileLocator Text::TrueTypeFont::TrueTypeFontFileLocator ;

string Text::TrueTypeFont::TrueTypeFontFileExtension = ".ttf" ;


const Ceylan::Float32 Text::TrueTypeFont::SpaceWidthFactor = 1 ;


// Used to know when the SDL_TTF module can be stopped.
Ceylan::Uint32 TrueTypeFont::FontCounter = 0 ;



TrueTypeFont::TrueTypeFont( 
	const std::string & fontFilename, 
	PointSize pointSize, 
	FontIndex index, 
	bool convertToDisplay, 
	RenderCache cacheSettings ) 
			throw( TextException ) :
	Font( convertToDisplay, cacheSettings ),	
	_pointSize( pointSize ),
	_actualFont( 0 )
{
	
	string fontFullPath = fontFilename ;
	
	// Search directly in current working directory :
	if ( ! System::File::ExistsAsFileOrSymbolicLink( fontFilename ) )
	{
		
		// On failure use the dedicated TrueType font locator :
		try
		{
		
			fontFullPath = TrueTypeFont::TrueTypeFontFileLocator.find(
				fontFilename ) ;
				
		}
		catch( const System::FileLocatorException & e )
		{
				
			// Last hope is general font locator :
			try
			{
				fontFullPath = Font::FontFileLocator.find( fontFilename ) ;
			}
			catch( const System::FileLocatorException & ex )
			{
				
				// Not found !
				
				string currentDir ;
				
				try
				{
					currentDir =
						System::Directory::GetCurrentWorkingDirectoryName() ;
				}
				catch( const System::Directory::DirectoryException & exc )
				{
					throw TextException( 
						"TrueTypeFont constructor : unable to load '" 
						+ fontFilename 
						+ "', exception generation triggered another failure : "
						+ exc.toString() + "." ) ;
				}
				
				throw TextException( "TrueTypeFont constructor : '" 
					+ fontFilename 
					+ "' is not a regular file or a symbolic link "
					"relative to the current directory (" + currentDir
					+ ") and cannot be found through TrueType font locator ("
					+ TrueTypeFont::TrueTypeFontFileLocator.toString() 
					+ ") nor through general font locator based on "
					"font path environment variable ("
					+ Font::FontPathEnvironmentVariable + ") : " 
					+ Font::FontFileLocator.toString() + "." ) ;
					
			}		
		}		
	}
	
	
	if ( TTF_WasInit() == 0 )
	{
		if ( TTF_Init()== -1 )
			throw TextException( 
				"TrueTypeFont constructor : unable to init font library : "
				+ GetLastError() ) ;
	}	
	
	_actualFont = TTF_OpenFont( fontFullPath.c_str(), pointSize ) ;
	
	if ( _actualFont == 0 )
		throw TextException( "TrueTypeFont constructor : unable to open '" 
			+ fontFullPath 
			+ "' with a point size of " 
			+ Ceylan::toString( pointSize ) + " dots per inch : "
			+ GetLastError() ) ;
	
	_spaceWidth = static_cast<Width>( SpaceWidthFactor * getWidth( ' ' ) ) ;

	// By default, the width of an alinea is a multiple of a space width :
	_alineaWidth = DefaultSpaceBasedAlineaWidth * _spaceWidth ;	
	
	FontCounter++ ;
	
}


TrueTypeFont::~TrueTypeFont() throw()
{

	if ( _actualFont != 0 )
		TTF_CloseFont( _actualFont ) ;
		
	FontCounter-- ;
	
	if ( FontCounter == 0 && TTF_WasInit() != 0 )
		TTF_Quit() ;	
		
}



PointSize TrueTypeFont::getPointSize() const throw()
{

	return _pointSize ;
	
}


RenderingStyle TrueTypeFont::getRenderingStyle() const throw()
{

	return TTF_GetFontStyle( _actualFont ) ;
	
}


void TrueTypeFont::setRenderingStyle( RenderingStyle newStyle ) 
	throw( TextException )
{


	// Cannot guess whether the specified style is supported.

	/* 
	 * It would flush the internal cache of previously rendered glyphs, 
	 * even if there is no change in style otherwise :
	 *
	 */
	 
	if ( newStyle != getRenderingStyle() )
		TTF_SetFontStyle( _actualFont, newStyle ) ;
		
}	

	 
Width TrueTypeFont::getWidth( Ceylan::Latin1Char character ) 
	const throw( TextException )
{

	/*
	 * Not using the advance parameter except for space, whose width 
	 * would be zero otherwise.
	 *
	 */
 
 	if ( character == ' ' )
		return getAdvance( ' ' ) ;
		
 	int minX, maxX ;
	
	if ( TTF_GlyphMetrics( _actualFont,
			Ceylan::UnicodeString::ConvertFromLatin1( character), 
			& minX, & maxX, 0, 0, 0 )  != 0 )
		throw TextException( "TrueTypeFont::getWidth : " + GetLastError() ) ;

	return static_cast<Width>( maxX - minX ) ;	
	
}


SignedWidth TrueTypeFont::getWidthOffset( Ceylan::Latin1Char character ) 
	const throw( TextException )
{

 	int minX ;
	
	if ( TTF_GlyphMetrics( _actualFont,
			Ceylan::UnicodeString::ConvertFromLatin1( character), 
			& minX, 0, 0, 0, 0 )  != 0 )
		throw TextException( "TrueTypeFont::getWidthOffset : " 
			+ GetLastError() ) ;
			
	return static_cast<SignedWidth>( minX ) ;	

}

							
SignedHeight TrueTypeFont::getHeightAboveBaseline( 
	Ceylan::Latin1Char character ) const throw( TextException )
{

	int maxY ;
	
	if ( TTF_GlyphMetrics( _actualFont,
			Ceylan::UnicodeString::ConvertFromLatin1( character), 
			0, 0, 0, & maxY, 0 )  != 0 )
		throw TextException( "TrueTypeFont::getHeightAboveBaseline : " 
			+ GetLastError() ) ;
	
	return static_cast<SignedHeight>( maxY ) ;
	
}


OSDL::Video::SignedLength TrueTypeFont::getAdvance( 
	Ceylan::Latin1Char character ) const throw( TextException )
{

	int advance ;
	
	if ( TTF_GlyphMetrics( _actualFont,
		Ceylan::UnicodeString::ConvertFromLatin1( character), 
			0, 0, 0, 0, & advance ) != 0 )
		throw TextException( "TrueTypeFont::getAdvance : " + GetLastError() ) ;
	
	return static_cast<SignedLength>( advance ) ;
	
}


Text::Height TrueTypeFont::getHeight() const throw()
{

	return TTF_FontHeight( _actualFont ) ;
	
}


Text::SignedHeight TrueTypeFont::getAscent() const throw()
{

	return TTF_FontAscent( _actualFont ) ;
	
}


Text::SignedHeight TrueTypeFont::getDescent() const throw()
{

	return TTF_FontDescent( _actualFont ) ;
	
}


Text::Height TrueTypeFont::getLineSkip() const throw()
{

	return TTF_FontLineSkip( _actualFont ) ;
	
}


Ceylan::Uint16 TrueTypeFont::getFacesCount() const throw()
{

	return TTF_FontFaces( _actualFont ) ;
	
}


bool TrueTypeFont::isFixedWidth() const throw()
{

	return ( TTF_FontFaceIsFixedWidth( _actualFont ) > 0 ) ;
	
}


string TrueTypeFont::getFaceFamilyName() const throw()
{

	return TTF_FontFaceFamilyName( _actualFont ) ;
	
}


string TrueTypeFont::getFaceStyleName() const throw()
{

	return TTF_FontFaceStyleName( _actualFont ) ;
	
}



// Bounding boxes section.


UprightRectangle & TrueTypeFont::getBoundingBoxFor( 
		Ceylan::Unicode glyph, SignedLength & advance ) 
	const throw( TextException )
{

	int minX, maxX, minY, maxY, intAdvance ;

	if ( TTF_GlyphMetrics( _actualFont, glyph, & minX, & maxX, 
			& minY, & maxY, & intAdvance ) != 0 )
		throw TextException( "TrueTypeFont::getClippingBoxFor (glyph) : " 
			+ GetLastError() ) ;
	
	advance = static_cast<SignedLength>( intAdvance ) ;
	
	return * new UprightRectangle( 
		static_cast<Coordinate>( minX ), 
		static_cast<Coordinate>( maxY ),
		static_cast<Length>( maxX - minX ), 
		static_cast<Length>( maxY - minY ) ) ;
		 
}


UprightRectangle & TrueTypeFont::getBoundingBoxFor( const std::string & text )
	const throw( TextException )
{

	int width, height ;
	
	if ( TTF_SizeText( _actualFont, text.c_str(), & width, & height ) != 0 )
		throw TextException( 
			"TrueTypeFont::getBoundingBoxFor (Latin-1 string) : " 
			+ GetLastError() ) ;
	
	return * new UprightRectangle( 0, 0, static_cast<Length>( width ), 
		static_cast<Length>( height ) ) ;	
		
}


UprightRectangle & TrueTypeFont::getBoundingBoxForUTF8( 
	const std::string & text ) const throw( TextException )
{

	int width, height ;
	
	if ( TTF_SizeUTF8( _actualFont, text.c_str(), & width, & height ) != 0 )
		throw TextException( 
			"TrueTypeFont::getBoundingBoxFor (UTF-8 string) : " 
			+ GetLastError() ) ;
	
	return * new UprightRectangle( 0, 0, static_cast<Length>( width ), 
		static_cast<Length>( height ) ) ;	
		
}


UprightRectangle & TrueTypeFont::getBoundingBoxForUnicode( 
	const Ceylan::Unicode * text ) const throw( TextException )
{

	if ( text == 0 )
		throw TextException( "TrueTypeFont::getBoundingBoxForUnicode : "
			"null pointer for Unicode string." ) ;

	int width, height ;
	
	if ( TTF_SizeUNICODE( _actualFont, text, & width, & height ) != 0 )
		throw TextException( 
			"TrueTypeFont::getBoundingBoxFor (Unicode string) : " 
			+ GetLastError() ) ;
	
	return * new UprightRectangle( 0, 0, static_cast<Length>( width ), 
		static_cast<Length>( height ) ) ;	
		
}



// Render section.


OSDL::Video::Surface & TrueTypeFont::renderLatin1Glyph( 
		Ceylan::Latin1Char character, 
		RenderQuality quality, 
		Pixels::ColorDefinition glyphColor ) 
	throw( TextException )
{
	
	return renderUnicodeGlyph( 
		Ceylan::UnicodeString::ConvertFromLatin1( character ),
		quality, glyphColor ) ;
		
}


void TrueTypeFont::blitLatin1Glyph( 
	Surface & targetSurface, Coordinate x, Coordinate y, 
	Ceylan::Latin1Char character, RenderQuality quality, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	// Could be easily optimized (even though Freetype has a cache feature) :
	
	Surface & res = renderUnicodeGlyph(
		Ceylan::UnicodeString::ConvertFromLatin1( character ),
		quality, glyphColor ) ;
		
	res.blitTo( targetSurface, x, y ) ;
	
	delete & res ;
		
}


OSDL::Video::Surface & TrueTypeFont::renderUnicodeGlyph( 
	Ceylan::Unicode character, 
	RenderQuality quality, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	// Two different cases, depending on a glyph cache being used or not :
	
	if ( _cacheSettings == GlyphCached )
	{
		
		/*
		 * First check that the character-quality-color combination is 
		 * not already available in cache :
		 *
		 */
		
		CharColorQualityKey renderKey( character, glyphColor, quality ) ;
			
		SmartResource * res = _glyphCache->getClone( renderKey ) ;
		
		if ( res != 0 )
		{
		
			Surface * returned = dynamic_cast<Surface *>( res ) ;
			
#if OSDL_DEBUG_FONT

			LogPlug::debug( "TrueTypeFont::renderUnicodeGlyph : cache hit, "
				"returning clone of prerendered glyph." ) ;
			
			if ( returned == 0 )
				Ceylan::emergencyShutdown( "TrueTypeFont::renderUnicodeGlyph : "
					"clone is not a Surface." ) ;
					
#endif // OSDL_DEBUG_FONT
			
			return * returned ;
			
		}	
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "TrueTypeFont::renderUnicodeGlyph : "
			"cache miss, creating new glyph rendering." ) ;
			
#endif // OSDL_DEBUG_FONT

		// Here it its a cache miss, we therefore have to generate the glyph :
		Surface & newSurface = basicRenderUnicodeGlyph( character, quality,
			glyphColor ) ;
		
		// Give the cache a chance of being fed :		
		_glyphCache->scanForAddition( renderKey, newSurface ) ;
		
		
		return newSurface ;
					
	}

	/*
	 * Here we are not using a glyph cache, we have simply to generate 
	 * the glyph :	
	 *
	 */
	return basicRenderUnicodeGlyph( character, quality, glyphColor ) ;	

}




OSDL::Video::Surface & TrueTypeFont::renderLatin1Text( 
		const std::string & text, 
		RenderQuality quality, Pixels::ColorDefinition textColor )
	throw( TextException )
{


	/*
	 * Test to compare both implementations 
	 * (OSDL glyph-based versus SDL_ttf whole string) : 
	 *  - next line uncommented : OSDL glyph-based 
	 *  - next line commented : SDL_ttf whole string
	 *
	 * Whole string (this actual method) may be preferred since rendering 
	 * is maybe a bit better (maybe kerning better supported, alpha 
	 * specifically managed for each quality, etc.).
	 *
	 * However both methods look most of the time the same, pixel-perfect 
	 * wise.
	 *
	 * Default selected implementation (when next line not commented) is 
	 * the OSDL's one :
	 *
	 */	 
	return Font::renderLatin1Text( text, quality, textColor ) ;
	
	
	/*
	 * Following SDL_ttf whole string rendering deactivated if previous 
	 * line not commented :
	 *
	 */
	
	SDL_Surface * textSurface ;
	Surface * res ;
	
	switch( quality )
	{	
	
		case Solid:
			textSurface = TTF_RenderText_Solid( _actualFont, 
				text.c_str(), textColor ) ;
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderLatin1Text (solid) : " 
					+ GetLastError() ) ; 
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	
			// No colorkey added, since already there. 
			break ;
	
	
		case Shaded:
			textSurface = TTF_RenderText_Shaded( _actualFont, 
				text.c_str(), textColor, _backgroundColor ) ;
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderLatin1Text (shaded) : " 
					+ GetLastError() ) ;
					
			/*
			 * We have to create a new surface so that the surface returned by
			 * 'TTF_RenderText_Shaded' will no more be palettized : we need a
			 * color key to have transparent blits.
			 *
			 */	 
			ColorMask redMask, greenMask, blueMask ;
			Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
	
			res = new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
				textSurface->w, textSurface->h, /* bpp */ 32, 
				redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;

			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
			}	
			else
			{
				colorKey = Pixels::Black ;
				/*
				 * No need to fill 'res' with black, since new RGB 
				 * surfaces come all black already.
				 *
				 */
			}	
				
			try
			{
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
			}
			catch( const Video::VideoException & e )
			{
				throw TextException( 
					"TrueTypeFont::renderLatin1Text (shaded) : "
					"color keying failed : " + e.toString() ) ; 
			}
			SDL_BlitSurface( textSurface, 0, & res->getSDLSurface(), 0 ) ;
			break ;


		case Blended:
			textSurface = TTF_RenderText_Blended( _actualFont, 
				text.c_str(), textColor ) ;
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderLatin1Text (blended) : " 
					+ GetLastError() ) ; 
					
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	

			// Not defined by default :
#ifdef OSDL_ADDS_COLOR_KEY

			/*
			 * Alpha should be preserved since the blending is the 
			 * point of this quality, but adding a color key should not harm :
			 *
			 */
			 
			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
			}	
			else
			{
				colorKey = Pixels::Black ;
				
				/*
				 * No need to fill 'res' with black, since new RGB 
				 * surfaces come all black already.
				 *
				 */
			}	
			
			try
			{
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
			}
			catch( const Video::VideoException & e )
			{
				throw TextException( 
					"TrueTypeFont::renderLatin1Text (blended) : "
					"color keying failed : " + e.toString() ) ; 
			}
			
#endif // OSDL_ADDS_COLOR_KEY
			
			break ;
	
	
		default:
			throw TextException( "TrueTypeFont::renderLatin1Text : "
				"unknown quality requested : " 
				+ Ceylan::toString( quality ) + "." ) ; 
			break ;	
			
	}
	

	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey (if any), so we choose to add 
		 * alpha only if no colorkey is used.
		 *
		 * Surface will be RLE encoded here :
		 *	
		 */
		if ( quality == Blended )
			res->convertToDisplay( /* alphaChannelWanted */ true ) ;
		else	
			res->convertToDisplay( /* alphaChannelWanted */ false ) ;
			
	}	

	return * res ;	
			
}




OSDL::Video::Surface & TrueTypeFont::renderUTF8Text( 
		const std::string & text, 
		RenderQuality quality, 
		Pixels::ColorDefinition textColor )
	throw( TextException )
{

	SDL_Surface * textSurface ;
	Surface * res ;
	
	switch( quality )
	{
	
	
		case Solid:
			textSurface = TTF_RenderUTF8_Solid( _actualFont, text.c_str(),
				textColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( "TrueTypeFont::renderUTF8Text (solid) : " 
					+ GetLastError() ) ; 
					
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	
			break ;
	
	
		case Shaded:
			textSurface = TTF_RenderUTF8_Shaded( _actualFont, text.c_str(),
				textColor, _backgroundColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( "TrueTypeFont::renderUTF8Text (shaded) : " 
					+ GetLastError() ) ; 
					
			/*
			 * We have to create a new surface so that the surface returned by
			 * 'TTF_RenderUTF8_Shaded' will no more be palettized : 
			 * we need a color key to have transparent blits.
			 *
			 */
			ColorMask redMask, greenMask, blueMask ;
			Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
			 	 
			res = new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
				textSurface->w, textSurface->h, 32, 
				redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;
				
			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
			
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
				
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
				
			try
			{
			
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
						
			}
			catch( const Video::VideoException & e )
			{
				throw TextException( "TrueTypeFont::renderUTF8Text (shaded) : "
					"color keying failed : " + e.toString() ) ; 
			}
			
			SDL_BlitSurface( textSurface, 0, & res->getSDLSurface(), 0 ) ;
			break ;


		case Blended:
			textSurface = TTF_RenderUTF8_Blended( _actualFont, text.c_str(),
				textColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderUTF8Text (blended) : " 
					+ GetLastError() ) ; 
					
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	


			// Not defined by default :
#ifdef OSDL_ADDS_COLOR_KEY

			/*
			 * Alpha should be preserved since the blending is the 
			 * point of this quality, but adding a color key should not harm :
			 *
			 */
			 
			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
			
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
				
			}	
			else
			{
			
				colorKey = Pixels::Black ;
				/*
				 * No need to fill 'res' with black, since new RGB 
				 * surfaces come all black already.
				 *
				 */
			}	
			
			try
			{
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
			}
			catch( const Video::VideoException & e )
			{
			
				throw TextException( "TrueTypeFont::renderUTF8Text (blended) : "
					"color keying failed : " + e.toString() ) ;
					 
			}
			
#endif // OSDL_ADDS_COLOR_KEY

			break ;
	
	
		default:
			throw TextException( "TrueTypeFont::renderUTF8Text : "
				"unknown quality requested : " 
				+ Ceylan::toString( quality ) + "." ) ; 
			break ;	
			
	}
	
	
	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey (if any), so we choose to add 
		 * alpha only if no colorkey is used.
		 *
		 * Surface will be RLE encoded here :
		 *	
		 */
		if ( quality == Blended )
			res->convertToDisplay( /* alphaChannelWanted */ true ) ;
		else	
			res->convertToDisplay( /* alphaChannelWanted */ false ) ;
			
	}	

	return * res ;	
	
}



OSDL::Video::Surface & TrueTypeFont::renderUnicodeText( 
		const Ceylan::Unicode * text, 
		RenderQuality quality, 
		Pixels::ColorDefinition textColor ) 
	throw( TextException )
{

	if ( text == 0 )
		throw TextException( 
			"TrueTypeFont::renderUnicode : null pointer for Unicode string." ) ;
		
	SDL_Surface * textSurface ;
	Surface * res ;
	
	switch( quality )
	{
	
		case Solid:
			textSurface = TTF_RenderUNICODE_Solid( _actualFont, text, 
				textColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderUnicodeText (solid) : " 
					+ GetLastError() ) ; 
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	
			break ;
	
	
		case Shaded:
			textSurface = TTF_RenderUNICODE_Shaded( _actualFont, text,
				textColor, _backgroundColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderUnicodeText (shaded) : " 
					+ GetLastError() ) ; 
					
			/*
			 * We have to create a new surface so that the surface returned by
			 * 'TTF_RenderUNICODE_Shaded' will no more be palettized : 
			 * we need a color key to have transparent blits.
			 *
			 */	 			 
			ColorMask redMask, greenMask, blueMask ;
			Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
			
			res = new Surface( Surface::Hardware | Surface::ColorkeyBlit, 
				textSurface->w, textSurface->h, 32, 
				redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;

			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
			
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
				
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
				
			try
			{
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
			}
			catch( const Video::VideoException & e )
			{
				throw TextException( 
					"TrueTypeFont::renderUnicodeText (shaded) : "
					"color keying failed : " + e.toString() ) ; 
			}
			SDL_BlitSurface( textSurface, 0, & res->getSDLSurface(), 0 ) ;
			break ;


		case Blended:
			textSurface = TTF_RenderUNICODE_Blended( _actualFont, text,
				textColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::renderUnicodeText (blended) : " 
					+ GetLastError() ) ;
					 
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;


			// Not defined by default :
#ifdef OSDL_ADDS_COLOR_KEY

			/*
			 * Alpha should be preserved since the blending is the 
			 * point of this quality, but adding a color key should not harm :
			 *
			 */
			 
			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( textColor, Pixels::Black,
				 /* use alpha */ false ) )
			{
			
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
				
			}	
			else
			{
			
				colorKey = Pixels::Black ;
				
				/*
				 * No need to fill 'res' with black, since new RGB 
				 * surfaces come all black already.
				 *
				 */
			}	
			
				
			/*
			 * Alpha should be preserved since the blending is the point 
			 * of this quality, but adding a color key should not harm :
			 *
			 */
			try
			{
			
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
						
			}
			catch( const Video::VideoException & e )
			{
				throw TextException( 
					"TrueTypeFont::renderUnicodeText (blended) : "
					"color keying failed : " + e.toString() ) ; 
			}
			
#endif // OSDL_ADDS_COLOR_KEY

			break ;
	
	
		default:
			throw TextException( "TrueTypeFont::renderUnicodeText : "
				"unknown quality requested : " 
				+ Ceylan::toString( quality ) + "." ) ; 
			break ;	

	}

	
	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey (if any), so we choose to add
		 *  alpha only if no colorkey is used.
		 *
		 * Surface will be RLE encoded here :
		 *	
		 */
		if ( quality == Blended )
			res->convertToDisplay( /* alphaChannelWanted */ true ) ;
		else	
			res->convertToDisplay( /* alphaChannelWanted */ false ) ;
			
	}	

	return * res ;	
	
}



const string TrueTypeFont::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{

	string res = "Truetype font, whose point size is " 
		+ Ceylan::toString( _pointSize ) + " dots per inch" ;
		
	if ( level == Ceylan::low )
		return res ;
	
	Ceylan::Uint16 faceCount = getFacesCount() ;
	
	res += ". This font has " ;
	
	switch( faceCount )
	{
	
		case 0:
			res += "no available face (abnormal)" ;
			break ;
			
		case 1:
			res += "exactly one face" ;
			break ;
			
		default:
			res += Ceylan::toString( faceCount ) + " faces" ;
			break ;			
	
	}
	
	if ( isFixedWidth() )
		res +=", and is fixed width" ;
	else
		res +=", and is not fixed width" ;
	
	res +=". The family name of the current face is " + getFaceFamilyName() 
		+ ", and its style name is " + getFaceStyleName() ;
	
	if 	( level == Ceylan::medium )
		return res ;	 	
			
	SDL_version compileVersion ;
	
	TTF_VERSION( & compileVersion ) ;	
	
	const SDL_version * linkedVersion = TTF_Linked_Version() ;
	
	res += ". The OSDL font module is compiled with SDL_ttf version " 
		+ Ceylan::toNumericalString( compileVersion.major ) + "."
		+ Ceylan::toNumericalString( compileVersion.minor ) + "."
		+ Ceylan::toNumericalString( compileVersion.patch ) 
		+ ", linked with version "
		+ Ceylan::toNumericalString( linkedVersion->major ) + "."
		+ Ceylan::toNumericalString( linkedVersion->minor ) + "."
		+ Ceylan::toNumericalString( linkedVersion->patch ) ;
		+ ". Rendering style is " 
		+ InterpretRenderingStyle( TTF_GetFontStyle( _actualFont ) ) ;
		
		
	return res ;
		
		
}



// Static section.


Font::RenderQuality TrueTypeFont::GetObtainedQualityFor( 
	Font::RenderQuality targetedQuality ) throw()
{

	return targetedQuality ;
	
}


void TrueTypeFont::SetUnicodeSwapStatus( bool newStatus ) throw()
{

	if ( newStatus )
		TTF_ByteSwappedUNICODE( 1 ) ;
	else	
		TTF_ByteSwappedUNICODE( 0 ) ;
		
}


string TrueTypeFont::GetLastError() throw() 
{

	return TTF_GetError() ;
	
}


OSDL::Video::Surface & TrueTypeFont::basicRenderUnicodeGlyph( 
	Ceylan::Unicode character, RenderQuality quality, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	// Render unconditionnally here :
	
	SDL_Surface * textSurface ;
	Surface * res ;
	
	switch( quality )
	{
	
	
		case Solid:
			textSurface = TTF_RenderGlyph_Solid( _actualFont, character,
				glyphColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::basicRenderUnicodeGlyph (solid) : " 
					+ GetLastError() ) ; 
					
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	
				
			// No colorkey added, since already there. 
			break ;
	
	
		case Shaded:
			textSurface = TTF_RenderGlyph_Shaded( _actualFont, character,
				glyphColor, _backgroundColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::basicRenderUnicodeGlyph (shaded) : " 
					+ GetLastError() ) ;
					 
			/*
			 * We have to create a new surface so that the surface returned by
			 * 'TTF_RenderGlyph_Shaded' will no more be palettized : 
			 * we need a color key to have transparent blits.
			 *
			 */	 
			
			ColorMask redMask, greenMask, blueMask ;
			Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
			
			res = new Surface( Surface::Hardware | Surface::ColorkeyBlit,
				textSurface->w, textSurface->h, /* bpp */ 32, 
				redMask, greenMask, blueMask, /* no alpha wanted */ 0 ) ;

			// Avoid messing text color with color key :
			Pixels::ColorDefinition colorKey ;
	
			if ( Pixels::areEqual( glyphColor, Pixels::Black, 
				/* use alpha */ false ) )
			{
			
				colorKey = Pixels::White ;
				res->fill( colorKey ) ;
				
			}	
			else
			{
			
				colorKey = Pixels::Black ;
				
				/*
				 * No need to fill 'res' with black, since new RGB 
				 * surfaces come all black already.
				 *
				 */
				 
			}	
			
			try
			{
				res->setColorKey( 
					Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
					Pixels::convertColorDefinitionToPixelColor(
						res->getPixelFormat(), colorKey ) ) ;
			}
			catch( const Video::VideoException & e )
			{
			
				throw TextException( 
					"TrueTypeFont::basicRenderUnicodeGlyph (shaded) : "
					"color keying failed : " + e.toString() ) ; 
					
			}
			SDL_BlitSurface( textSurface, 0, & res->getSDLSurface(), 0 ) ;
			break ;


		case Blended:
			textSurface = TTF_RenderGlyph_Blended( _actualFont, character,
				glyphColor ) ;
				
			if ( textSurface == 0 )
				throw TextException( 
					"TrueTypeFont::basicRenderUnicodeGlyph (blended) : " 
					+ GetLastError() ) ; 
			
			res = new Surface( * textSurface, 
				/* display type */ Surface::BackBuffer ) ;	

			// No color key enforced.
			
			break ;
	
	
		default:
			throw TextException( "TrueTypeFont::basicRenderUnicodeGlyph : "
				"unknown quality requested : " 
				+ Ceylan::toString( quality ) + "." ) ; 
			break ;	
			
	}
	
	
	if ( _convertToDisplay )
	{
	
		/*
		 * We want to keep our colorkey (if any), so we choose to add 
		 * alpha only if no colorkey is used.
		 *
		 * Surface will be RLE encoded here :
		 *	
		 */
		if ( quality == Blended )
			res->convertToDisplay( /* alphaChannelWanted */ true ) ;
		else	
			res->convertToDisplay( /* alphaChannelWanted */ false ) ;
			
	}	
	
	// Uncomment next line to debug character bounding-box :
	//res->drawEdges() ;
		
	return * res ;	
	

}

