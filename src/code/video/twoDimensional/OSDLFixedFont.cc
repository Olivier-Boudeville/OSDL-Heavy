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


#include "OSDLFixedFont.h"

#include "OSDLSurface.h"         // for Surface
#include "OSDLPixel.h"           // for ColorDefinition
#include "OSDLVideoTypes.h"      // for Length, SignedLength, etc.
#include "OSDLUtils.h"           // for getBackendLastError.


#include <list>
 
#include <ctype.h>                   // for isdigit


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_DEBUG_FONT and al 
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL_GFX

#include "SDL_gfxPrimitives.h"       // for stringColor

#endif // OSDL_USES_SDL_GFX


#if OSDL_USES_SDL

#include "SDL.h"                     // for SDL_CreateRGBSurface

#endif // OSDL_USES_SDL



using std::string ;

using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Video ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;


Ceylan::System::FileLocator Text::FixedFont::FixedFontFileLocator ;


const OSDL::Video::Length
	OSDL::Video::TwoDimensional::Text::BasicFontCharacterWidth  = 8 ;
	
const OSDL::Video::Length
	OSDL::Video::TwoDimensional::Text::BasicFontCharacterHeight = 8 ;



string Text::FixedFont::FontFileExtension = ".fnt" ;


const Ceylan::Float32 Text::FixedFont::SpaceWidthFactor = 0.7f ;


const Ceylan::Uint16 Text::FixedFont::FontCharacterCount = 256 ;




// First, the two basic fixed font primitives.



bool Text::printBasic( const std::string & text, Surface & targetSurface,
		Coordinate x, Coordinate y, Pixels::ColorDefinition colorDef ) 
	throw( VideoException )
{	

#if OSDL_USES_SDL_GFX

	FixedFont::SetFontSettings( /* no font data: built-in */ 0, /* width */ 8, 
		/* height */ 8 ) ;
			
	bool result = ::stringColor( & targetSurface.getSDLSurface(), 
		x, y, text.c_str(),
		Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ;
		
	return result ;

#else // OSDL_USES_SDL_GFX

	throw VideoException( "Text::printBasic failed: "
		"no SDL_gfx support available" ) ;
	
#endif // OSDL_USES_SDL_GFX

}



bool Text::printBasic( const std::string & text, Surface & targetSurface,
		Coordinate x, Coordinate y, 
		Pixels::ColorElement red, Pixels::ColorElement blue,
		Pixels::ColorElement green, Pixels::ColorElement alpha )
	throw( VideoException )
{	

#if OSDL_USES_SDL_GFX
	
	FixedFont::SetFontSettings( /* no font data: built-in */ 0, /* width */ 8, 
		/* height */ 8 ) ;
		
	/*
	 * Color should not be mapped to target surface, since it will be 
	 * done thanks to the SDL_gfx blit of the result.
	 *
	 */
		
	bool result = ::stringRGBA( & targetSurface.getSDLSurface(), x, y,
		text.c_str(), red, green, blue, alpha ) == 0 ;
		
	return result ;
	
#else // OSDL_USES_SDL_GFX

	throw VideoException( "Text::printBasic failed: "
		"no SDL_gfx support available" ) ;
	
#endif // OSDL_USES_SDL_GFX
	
}




/*
 * The problem of the SDL_gfx fixed fonts is that the glyph caching 
 * feature works only for one font and one color.
 * If multiple fixed fonts are used, as they share the same cache,
 * their glyphs are regularly overwritten by the same character rendered 
 * with another font, and, even worse, switching fonts leads to flushing 
 * all the glyph cache.
 *
 * To avoid that, OSDL uses one cache by fixed font. 
 * Besides, the cached glyph can be directly converted to display, which,
 * together with a RLE encoding based on the color key, should be more
 * efficient.
 *
 */
FixedFont::FixedFont( 
		Length characterWidth, 
		Length characterHeight, 
		RenderingStyle renderingStyle, 
		bool convertToDisplay, 
		RenderCache cacheSettings,
		AllowedCachePolicy cachePolicy, 
		Ceylan::System::Size quota 
			) throw( TextException ):
	Font( convertToDisplay, cacheSettings, cachePolicy, quota ),
	_width( characterWidth ),
	_height( characterHeight ),
	_fontData( 0 )
{

	// Space and alinea widths are set as well by:
	loadFontFrom( BuildFontFilenameFor( characterWidth, characterHeight,
		renderingStyle ) ) ;
	
}


							  
FixedFont::FixedFont( 
		const std::string & fontFilename, 
		bool convertToDisplay,
		RenderCache cacheSettings, 
		AllowedCachePolicy cachePolicy, 
		Ceylan::System::Size quota 
			) throw( TextException ):
	Font( convertToDisplay, cacheSettings, cachePolicy, quota ),
	_width( 0 ),
	_height( 0 ),
	_fontData( 0 )
{

	GetFontAttributesFrom( fontFilename, _width, _height, _renderingStyle ) ;

	// Space and alinea widths are set as well by:
	loadFontFrom( fontFilename ) ;
	
}



FixedFont::~FixedFont() throw()
{		
		
	if ( _fontData != 0 )
		delete [] _fontData ;	
					
}



Width FixedFont::getWidth() const throw()
{

	return _width ;
	
}



Width FixedFont::getWidth( Ceylan::Latin1Char character ) const throw()
{

	return _width ;
	
}



SignedWidth FixedFont::getWidthOffset( Ceylan::Latin1Char character ) 
	const throw( TextException )
{

	return 0 ;
	
}



SignedHeight FixedFont::getHeightAboveBaseline( Ceylan::Latin1Char character ) 
	const throw( TextException )
{

	return _height ;
	
}



SignedLength FixedFont::getAdvance() const throw( TextException )
{

	return _width ;
	
}



SignedLength FixedFont::getAdvance( Ceylan::Latin1Char character ) 
	const throw( TextException )
{

	// As getInterGlyphWidth returns 0:
	return _width ;
	
}



Width FixedFont::getInterGlyphWidth() const throw()
{

	return 0 ;
	
}



Text::Height FixedFont::getHeight() const throw()
{

	return _height ;
	
}



SignedHeight FixedFont::getAscent() const throw()
{
	return _height ;
}



SignedHeight FixedFont::getDescent() const throw()
{

	return 0 ;
	
}



Text::Height FixedFont::getLineSkip() const throw()
{

	return _height + static_cast<Text::Height>( 
		Ceylan::Maths::Max<Ceylan::Float32>( 2, 0.1f * _height ) ) ;
		
}





// Bounding boxes section.


UprightRectangle & FixedFont::getBoundingBox() const throw( TextException )
{

	return * new UprightRectangle( 0, 0, getAdvance(), getLineSkip() ) ;
	
}



UprightRectangle & FixedFont::getBoundingBoxFor( const std::string & word )
	const throw( TextException )
{

	if ( word.empty() )
		throw TextException( 
			"FixedFont::getBoundingBoxFor (word): empty word specified." ) ;
			
	return * new UprightRectangle( 0, 0, 
		static_cast<Length>( getAdvance() * ( word.size() - 1 ) + _width ), 
		static_cast<Length>( _height ) ) ;
		
}




// Render section.



OSDL::Video::Surface & FixedFont::renderLatin1Glyph( 
	Ceylan::Latin1Char character, RenderQuality quality, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	// Two different cases, depending on a glyph cache being used or not:
	
	if ( _cacheSettings == GlyphCached )
	{
		
		/*
		 * First check that the character-quality-color combination is not
		 * already available in cache:
		 *
		 */
		
		CharColorQualityKey renderKey( character, glyphColor, 
			/* quality ignored */ DefaultQuality ) ;
			
		SmartResource * res = _glyphCache->getClone( renderKey ) ;
		
		if ( res != 0 )
		{
		
			Surface * toReturn = dynamic_cast<Surface *>( res ) ;
			
#if OSDL_DEBUG_FONT

			LogPlug::debug( "FixedFont::renderLatin1Glyph: cache hit, "
				"returning clone of prerendered glyph." ) ;
			
			if ( toReturn == 0 )
				Ceylan::emergencyShutdown( "FixedFont::renderLatin1Glyph: "
					"cache did not return a Surface." ) ;
								
#endif // OSDL_DEBUG_FONT
			
			return *toReturn ;
			
		}	
		
		
#if OSDL_DEBUG_FONT

		LogPlug::debug( "FixedFont::renderLatin1Glyph: "
			"cache miss, creating new glyph rendering." ) ;
			
#endif // OSDL_DEBUG_FONT

		// Here it its a cache miss, we therefore have to generate the glyph:
		Surface & newSurface = basicRenderLatin1Glyph( character, glyphColor ) ;
		
		// Give the cache a chance of being fed:		
		_glyphCache->scanForAddition( renderKey, newSurface ) ;
		
		
		return newSurface ;
					
	}

	/*
	 * Here we are not using a glyph cache, we have simply to generate 
	 * the glyph:	
	 *
	 */
	return basicRenderLatin1Glyph( character, glyphColor ) ;	

}



void FixedFont::blitLatin1Glyph( Surface & targetSurface, 
	Coordinate x, Coordinate y, 
	Ceylan::Latin1Char character, RenderQuality quality, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	// Here the 'quality' attribute is ignored.
	
	// Two different cases: either we are glyph-cached, or not.
	
	
	if ( _cacheSettings == GlyphCached )
	{
		
		/*
		 * First check that the character-(quality)-color combination 
		 * is not already available in cache:
		 *
		 */
		 
		CharColorQualityKey renderKey( character, glyphColor, 
			/* quality ignored */ DefaultQuality ) ;
			
		const Resource * cacheEntry = _glyphCache->get( renderKey ) ;
		
		if ( cacheEntry != 0 )
		{
		
			// Already available ? Get it and blit it !
			const Surface * cachedSurface = 
				dynamic_cast<const Surface *>( cacheEntry ) ;
			
#if OSDL_DEBUG_FONT

			LogPlug::debug( "FixedFont::blitLatin1Glyph: "
				"cache hit, blitting prerendered glyph." ) ;
			
			if ( cachedSurface == 0 )
				Ceylan::emergencyShutdown( "FixedFont::blitLatin1Glyph: "
					"cache did not return a Surface." ) ;
					
#endif // OSDL_DEBUG_FONT
			
			try
			{
			
				cachedSurface->blitTo( targetSurface, x, y ) ;
				
			}
			catch( const VideoException & e )
			{
				throw TextException( "FixedFont::blitLatin1Glyph: "
					"blit of cloned glyph failed: " + e.toString() ) ;
			}
			
		}
		else
		{
		
#if OSDL_DEBUG_FONT

			LogPlug::debug( "FixedFont::blitLatin1Glyph: cache miss, "
				"blitting newly rendered glyph." ) ;
				
#endif // OSDL_DEBUG_FONT
		
			/*
			 * Not found in cache, hence ask a new rendering 
			 * (and submit it to the glyph cache):
			 *
			 * (the surface returned by 'submitLatin1GlyphToCache' is 
			 * owned in all cases by the cache, it should not be 
			 * deallocated here)
			 *
			 */ 
			submitLatin1GlyphToCache( character, glyphColor ).blitTo(
				targetSurface, x, y ) ;
							
		}
					
	}
	
	/*
	 * Here we are not glyph-cached, we blit the glyph directly:
	 * (no quality managed):
	 *
	 */
	basicBlitLatin1Glyph( targetSurface, x, y, character, glyphColor ) ;
	
}

			
				
OSDL::Video::Surface & FixedFont::renderLatin1GlyphAlpha( 
	Ceylan::Latin1Char character, RenderQuality quality,
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

#if OSDL_USES_SDL_GFX

	/*
	 * This method should not be used, since it does not do what was expected.
	 *
	 * This is not really annoying since using colorkey is indeed better 
	 * than using alphablending in the case of fonts.
	 *
	 */
	
	/*
	 * With testOSDLFixedFont.cc, one can see that:
	 *   - if the alpha mask for the created surface is the expected
	 * '0x000000FF', then one can see only a grid, no character is to be seen
	 *   - if the alpha mask for the created surface is '0x00000FF0', then 
	 * one can see that the characters are indeed available but that their 
	 * alpha coordinate is null, whereas we would want to have it set to 
	 * the one of the surface generated by SDL_gfx: the blit does not do
	 * what we would want, we tried many settings with no luck for the moment.
	 *
	 */
	 
	Pixels::PixelColor color = Pixels::convertColorDefinitionToRawPixelColor(
		glyphColor ) ;
	
	/*
	if ( _cacheSettings == Font::GlyphCached )
	{

		unsigned char charIndex = static_cast<unsigned char>( character ) ;
		Surface * inCache = _cachedGlyphs[ charIndex ] ;
				
		if ( inCache != 0  && color == _cachedColors[ charIndex ] )
				return inCache->clone() ;
	}
	*/
	
	// Here we have to generate the glyph.

	/*
	 * Some explanations are needed to understand how alpha blending is 
	 * managed:
	 * 1. an empty RGBA surface, with source alpha blending disabled, is
	 * created.
	 * It will hold a copy of the surface put in cache and blit by SDL_gfx
	 * (characterColor).
	 * The first setAlpha disables the source alpha blending on this surface, 
	 * to prepare for the SDL_gfx operated blit. 
	 * Otherwise we would be in the case of (RGBA with SRCALPHA) -> RBGA 
	 * blit which, according to
	 * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetAlpha, would result
	 * to our new surface having unchanged alpha coordinates, hence uniform:
	 * the alpha-coded shape of letters would be lost. 
	 * Instead, this first setAlpha allows to have a
	 * (RGBA with SRCALPHA) -> RBG blit, which preserves the alpha 
	 * coordinates from SDL_gfx 
	 * 2. then the blit is performed thanks to characterColor
	 * 3. the source alpha attribute of the result is restored thanks 
	 * to a second call to setAlpha.
	 * This way, when our surface will be blitted (for example to the 
	 * screen surface), it will be blitted as a surface directly generated 
	 * by SDL_gfx would be blitted, i.e. with respect to the alpha 
	 * coordinates we managed painfully to preserve.
	 *
	 * I found this alpha-blending subject rather dull and awfully explained 
	 * by the SDL doc, the result does not seem to work correctly.
	 *
	 */
	 
	 
	/*
	 * Creates a new back-buffer surface to which the glyph will be blitted:
	 *
	 * (beware to endianness)
	 *
	 */
		
	Surface & res = * new Surface( * SDL_CreateRGBSurface( 
		Surface::Hardware, _width, _height, 32, 
		0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF0
		/* replace 0x000000FF by 0x00000FF0 to see that this could work */
		  ) ) ;
	
	// Clears source alpha bit for our surface to prepare for next blit:
	res.setAlpha( 0 /* Surface::AlphaBlendingBlit disabled */,
		Pixels::AlphaOpaque ) ;		
	
	// Blits the SDL_gfx-generated character to our surface:
	if ( ::characterColor( & res.getSDLSurface(), 0, 0, 
			static_cast<char>( character ), color ) != 0 )
		throw TextException( "FixedFont::renderLatin1Glyph: blit failed, " 
			+ Utils::getBackendLastError() ) ;

	// Restores source alpha attributes for our surface:
	res.setAlpha( Surface::AlphaBlendingBlit, Pixels::AlphaOpaque ) ;		
		
	//if ( _convertToDisplay )
	//	res.convertToDisplay( /* alphaChannelWanted */ true ) ;

	/*
	if ( _cacheSettings == Font::GlyphCached )
		updateCache( res.clone(), character, color ) ;
	*/
	
	return res ;	

#else // OSDL_USES_SDL_GFX

	throw TextException( "FixedFont::renderLatin1GlyphAlpha failed: "
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX

}



const string FixedFont::toString( Ceylan::VerbosityLevels level ) const throw()
{

	string res = "Fixed font, whose dimensions are " 
		+ Ceylan::toString( _width ) 
		+ "x" + Ceylan::toString( _height ) ;
		
	if ( level == Ceylan::low )
		return res ;
	
	res += ". " + Font::toString( level ) ;
	
	return res ;
		
}




// Static public section.



// Protected section.

	
Font::RenderQuality FixedFont::GetObtainedQualityFor( 
	Font::RenderQuality targetedQuality ) throw()	
{

	return Solid ;
	
}



void FixedFont::SetFontSettings( const Ceylan::Byte * fontData, 
	Length characterWidth, Length characterHeight ) throw( TextException )	
{

#if OSDL_USES_SDL_GFX

	static const char * lastFontData  = 0 ;
	static Length lastCharacterWidth  = 0 ;
	static Length lastCharacterHeight = 0 ;
	
	/*
	 * No other code should use directly 'gfxPrimitivesSetFont' since it 
	 * would change SDL_gfx state.
	 *
	 */
	if ( lastFontData != fontData || lastCharacterWidth != characterWidth 
		|| lastCharacterHeight != characterHeight )
	{
	
		// We have indeed to reset SDL_gfx state:
				
#if OSDL_DEBUG_FONT

		LogPlug::debug( 
			"FixedFont::SetFontSettings: having to reset SDL_gfx state" ) ;
			
#endif // OSDL_DEBUG_FONT
		
		lastFontData        = fontData ;
		lastCharacterWidth  = characterWidth ;
		lastCharacterHeight = characterHeight ;

		gfxPrimitivesSetFont( fontData, characterWidth, characterHeight ) ;
	}	
	else
	{
	
#if OSDL_DEBUG_FONT

		LogPlug::debug( "FixedFont::SetFontSettings: saved a SDL_gfx reset" ) ;
		
#endif // OSDL_DEBUG_FONT

	}

#else // OSDL_USES_SDL_GFX

	throw TextException( "FixedFont::SetFontSettings failed: "
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX

}							



							
// Protected section.

							
void FixedFont::loadFontFrom( const std::string & fontFilename ) 
	throw( TextException )
{


#if OSDL_DEBUG_FONT

	LogPlug::trace( "FixedFont::loadFontFrom: trying to load font file '" 
		+ fontFilename + "'." ) ;
	
#endif // OSDL_DEBUG_FONT

	string fontFullPath = fontFilename ;
	
	// Search directly in current working directory:
	if ( ! File::ExistsAsFileOrSymbolicLink( fontFilename ) )
	{
		
		try
		{
			// On failure use the dedicated TrueType font locator:
			fontFullPath = FixedFontFileLocator.find( fontFilename ) ;
		}
		catch( const System::FileLocatorException & e )
		{
		
			// Last hope: general font locator:
			try
			{
				fontFullPath = Font::FontFileLocator.find( fontFilename ) ;
			}	
			catch( const System::FileLocatorException & ex )
			{
				
				// Not found at all, let's raise an exception.
				
				string currentDir ;
				
				try
				{
					currentDir = Directory::GetCurrentWorkingDirectoryPath() ;
				}
				catch( const DirectoryException & exc )
				{
				
					throw TextException( 
						"FixedFont::loadFontFrom: unable to load '" 
						+ fontFilename 
						+ "', exception generation triggered another failure: "
						+ exc.toString() + "." ) ;
						
				}
				
				throw TextException( "FixedFont::loadFontFrom: '" 
					+ fontFilename 
					+ "' is not a regular file or a symbolic link "
					"relative to the current directory (" + currentDir
					+ ") and cannot be found through Fixed font locator ("
					+ FixedFont::FixedFontFileLocator.toString() 
					+ ") nor through general font locator "
					"based on font path environment variable ("
					+ Font::FontPathEnvironmentVariable + "): " 
					+ Font::FontFileLocator.toString() + "." ) ;
					
			}		
		}		
	}		
	
	
	// Here fontFullPath should be OK.

#if OSDL_DEBUG_FONT

	LogPlug::debug( "FixedFont::loadFontFrom: full font filename is '" 
		+ fontFullPath + "'." ) ;
		
#endif // OSDL_DEBUG_FONT
	
	try
	{
	
		Ceylan::Holder<File> fontFile( 
			File::Open( fontFullPath /* OpenToReadBinary */ ) ) ; 
		
		System::Size dataSize = fontFile->size() ;
		
		if ( _fontData != 0 )
			delete [] _fontData ;
			
		_fontData = new char[ dataSize ] ;
		
		if ( _fontData == 0 )
			throw TextException( 
				"FixedFont::loadFontFrom: not enough memory." ) ;

		fontFile->readExactLength( _fontData, dataSize ) ; 
		
		// fontFile deallocated here.
		
	}
	catch( const System::SystemException & e )
	{
	
		throw TextException( "FixedFont::loadFontFrom: "
			"error while loading font data file: "	+ e.toString() ) ;
	}
	
	_spaceWidth = static_cast<Width>( SpaceWidthFactor * 
		getWidth( /* all character have the same width here */ ) ) ;

	// By default, the width of an alinea is a multiple of a space width:
	_alineaWidth = DefaultSpaceBasedAlineaWidth * _spaceWidth ;	

	// Here the font is loaded.	
			
}



const Surface & FixedFont::submitLatin1GlyphToCache( 
		Ceylan::Latin1Char character, Pixels::ColorDefinition glyphColor ) 
	throw( TextException )
{
	
	/*
	 * The method contract tells us this character is not in cache, we 
	 * have to render it:
	 *
	 */
	Surface & glyphSurface = basicRenderLatin1Glyph( character, glyphColor ) ;

	CharColorQualityKey renderKey( character, glyphColor, 
		/* quality not managed */ DefaultQuality ) ;
	
	bool takenByCache ; 
	
	try
	{ 
		takenByCache = _glyphCache->takeOwnershipOf( renderKey, glyphSurface ) ;
	}
	catch( const ResourceManagerException & e )
	{
	
		/*
		 * This should never happen since this method is meant to be 
		 * called only when the cache could not respond: the key should
		 * therefore not be already associated.
		 *
		 */
		 throw TextException( 
		 	"FixedFont::submitLatin1GlyphToCache: cache submitting failed: "
		 	+ e.toString() ) ;

	}
	
	if ( ! takenByCache )
		throw TextException( "FixedFont::submitLatin1GlyphToCache: "
			"cache did not accept rendering for '" 
			+ Ceylan::toString( character ) + "'." ) ;
	
	// Cache still owns that (const) surface:		
	return glyphSurface ;
	
}



Surface & FixedFont::basicRenderLatin1Glyph( Ceylan::Latin1Char character, 
	Pixels::ColorDefinition glyphColor ) throw( TextException )
{

	ColorMask redMask, greenMask, blueMask ;
	
	Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask ) ;
	
	// Creates a back-buffer surface with no source alpha blending requested:
	Surface & res = * new Surface( Surface::Hardware | Surface::ColorkeyBlit,
		_width, _height, /* bpp */ 32, redMask, greenMask, blueMask, 
		/* no alpha wanted */ 0 ) ;

	// Avoid messing text color with color key:
	Pixels::ColorDefinition colorKey ;
	
	if ( Pixels::areEqual( glyphColor, Pixels::Black, /* use alpha */ false ) )
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
		
	basicBlitLatin1Glyph( res, 0, 0, character, glyphColor ) ;

	// Comment out following two lines to see blit blocks (as black rectangles):
	res.setColorKey( Surface::ColorkeyBlit | Surface::RLEColorkeyBlit, 
		convertColorDefinitionToPixelColor( res.getPixelFormat(), colorKey ) ) ;
			
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
		
								
							
void FixedFont::basicBlitLatin1Glyph( Surface & targetSurface,
	 Coordinate x, Coordinate y, Ceylan::Latin1Char character,
	 Pixels::ColorDefinition glyphColor ) throw( TextException )
{

#if OSDL_USES_SDL_GFX

	/*
	 * SDL_gfx internal cache may have to be reset, since another fixed 
	 * font may be the current one.
	 *
	 * This is not that annoying since glyphs already can be cached at 
	 * the OSDL level, and the actual reset is only triggered when necessary.
	 *
	 */
	SetFontSettings( _fontData, _width, _height ) ; 

	Pixels::PixelColor color = Pixels::convertColorDefinitionToRawPixelColor(
		glyphColor ) ;
	
	if ( ::characterColor( & targetSurface.getSDLSurface(), x, y, 
			static_cast<char>( character ), color ) != 0 )
		throw TextException( 
			"FixedFont::basicBlitLatin1Glyph: blit of glyph failed, " 
			+ Utils::getBackendLastError() ) ;	

#else // OSDL_USES_SDL_GFX

	throw TextException( "FixedFont::basicBlitLatin1Glyph failed: "
		"no SDL_gfx support available" ) ;
		
#endif // OSDL_USES_SDL_GFX

}
	
							
							
string FixedFont::BuildFontFilenameFor( 
	Length characterWidth, Length characterHeight,
	RenderingStyle renderingStyle ) throw( TextException )
{

	string attribute ;
	
	if ( renderingStyle & Bold )
	{
	
		if ( renderingStyle & ~Bold )
			throw TextException( "FixedFont::BuildFontFilenameFor: "
				"too many rendering styles selected: " 
				+ Ceylan::toString( renderingStyle ) + "." ) ;
				
		attribute += "B" ;
		
	}
	
	if ( renderingStyle & Italic )
	{
	
		if ( renderingStyle & ~Italic )
			throw TextException( "FixedFont::BuildFontFilenameFor: "
				"too many rendering styles selected: " 
				+ Ceylan::toString( renderingStyle ) + "." ) ;
				
		attribute += "O" ;
		
	}
	
	if ( renderingStyle & Underline )
	{
	
		if ( renderingStyle & ~Underline )
			throw TextException( "FixedFont::BuildFontFilenameFor: "
				"too many rendering styles selected: " 
				+ Ceylan::toString( renderingStyle ) + "." ) ;
				
		attribute += "U" ;
		
	}
	
	
	return Ceylan::toString( characterWidth ) + "x" 
		+ Ceylan::toString( characterHeight ) + attribute + FontFileExtension ;

}



void FixedFont::GetFontAttributesFrom( const string & filename, 
	Length & characterWidth, Length & characterHeight, 
	RenderingStyle & renderingStyle ) throw( TextException )
{

	// Some examples: '10x20.fnt', '8x13B.fnt'
	System::Size size = filename.size() ;

	if ( filename.substr( size - 4 ) != FontFileExtension )
		throw TextException( 
			"FixedFont::GetFontAttributesFrom: expected extension ("
			+ FontFileExtension + "), not found in '" + filename + "'." ) ;
	
	string width, height ;
	
	Ceylan::Uint16 index = 0 ;
	
	while ( filename[index] != 'x' && index < size - 4 )
	{
		width += filename[index] ;
	} 		
	
	try
	{	
		characterWidth = static_cast<Length>( stringToUnsignedLong( width ) ) ;
	}
	catch( const Ceylan::Exception & e )
	{
	
		throw TextException( 
			"FixedFont::GetFontAttributesFrom: unable to guess width from '"
			+ filename + "': " + e.toString() ) ;
	}
	
	index++ ;
	
	while ( ::isdigit( filename[index] ) 
		&& index < size - 3 /* to include the '.' */ )
	{
		height += filename[index] ;
	} 		
	
	
	try
	{	
		characterHeight = 
			static_cast<Length>( stringToUnsignedLong( height ) ) ;
	}
	catch( const Ceylan::Exception & e )
	{
		throw TextException( 
			"FixedFont::GetFontAttributesFrom: unable to guess height from '"
			+ filename + "': " + e.toString() ) ;
	}
	
	
	switch( filename[index] )
	{
	
		case '.':
			// We are already in the extension.
			renderingStyle = Normal ;
			break ;

		case 'B':
			renderingStyle = Bold ;
			break ;
	
		case 'O':
			renderingStyle = Italic ;
			break ;
	
		case 'U':
			renderingStyle = Underline ;
			break ;
					
		default:
			throw TextException( "FixedFont::getFontAttributesFrom: "
				"unable to guess rendering style from '" + filename + "'." ) ;
			break ;
			
	}
	
}

