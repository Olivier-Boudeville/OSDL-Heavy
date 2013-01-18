/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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


#include "OSDLPalette.h"

#include "OSDLSurface.h"  // for Surface
#include "OSDLFileTags.h" // for PaletteTag

#include "Ceylan.h"       // for Ceil, File, etc.



#include <list>


using std::string ;


using namespace Ceylan::Log ;
using namespace Ceylan::System ;


using namespace OSDL::Video ;
using namespace OSDL::Video::Pixels ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>   // for OSDL_DEBUG_PALETTE and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS




#if OSDL_USES_SDL

#include "SDL.h"          // for SDL_PixelFormat, SDL_LOGPAL, etc.

const Ceylan::Flags Palette::Logical  = SDL_LOGPAL ;
const Ceylan::Flags Palette::Physical = SDL_PHYSPAL ;

#else // OSDL_USES_SDL

// Same as SDL:
const Ceylan::Flags Palette::Logical  = 0x01 ;
const Ceylan::Flags Palette::Physical = 0x02 ;

#endif // OSDL_USES_SDL




PaletteException::PaletteException( const string & message ) :
	VideoException( "Palette exception: " + message )
{

}



PaletteException::~PaletteException() throw()
{

}




Palette::Palette( ColorCount numberOfColors, ColorDefinition * colors,
		PixelFormat * format ) :
	_numberOfColors( 0 ),
	_colorDefs( 0 ),
	_pixelColors( 0 ),
	_converted( false ),
	_ownsColorDefinition( true ),
	_hasColorkey( false ),
	_colorKeyIndex( 0 )
{

#if OSDL_USES_SDL


	// 'colors' may be null, will be managed:
	load( numberOfColors, colors ) ;

	if ( format != 0 )
		updatePixelColorsFrom( * format ) ;

#else // OSDL_USES_SDL

	throw PaletteException( "Palette constructor failed:"
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



Palette::Palette( const LowLevelPalette & palette ) :
	_numberOfColors( 0 ),
	_colorDefs( 0 ),
	_pixelColors( 0 ),
	_converted( false ),
	_ownsColorDefinition( false ),
	_hasColorkey( false ),
	_colorKeyIndex( 0 )
{

#if OSDL_USES_SDL

	//LogPlug::debug( "Constructing a Palette from a low-level palette." ) ;

	load( palette.ncolors, palette.colors ) ;

#else // OSDL_USES_SDL

	throw PaletteException( "Palette constructor failed:"
		"no SDL support available" ) ;

#endif // OSDL_USES_SDL

}



Palette::Palette( const string & paletteFilename ) :
	_numberOfColors( 0 ),
	_colorDefs( 0 ),
	_pixelColors( 0 ),
	_converted( false ),
	_ownsColorDefinition( true ),
	_hasColorkey( false ),
	_colorKeyIndex( 0 )
{

	try
	{

		Size fileSize = File::GetSize( paletteFilename ) ;

		Ceylan::Holder<File> paletteHolder( File::Open( paletteFilename ) ) ;


		// First check the OSDL palette tag:
		FileTag readTag = paletteHolder->readUint16() ;
		fileSize -= sizeof( PaletteTag ) ;

		if ( readTag != PaletteTag )
			throw PaletteException(
				"Palette constructor: expected palette tag not found ("
				+ Ceylan::toString( PaletteTag ) + "), read instead "
				+ Ceylan::toString( readTag ) + ", which corresponds to: "
				+ DescribeFileTag( readTag ) ) ;


		bool hasColorkey = ( paletteHolder->readUint8() != 0 ) ;
		fileSize-- ;

		if ( hasColorkey )
		{
			setColorKeyIndex( paletteHolder->readUint16() ) ;
			fileSize -= 2 ;
		}

		// Header read, now reading colors:

		if ( fileSize % 3 != 0 )
			throw PaletteException( "Palette constructor from file failed: "
				"size of palette file '" + paletteFilename + "' is "
				+ Ceylan::toString( fileSize )
				+ " bytes, which is not a multiple of 3 bytes "
				"(24 bits per color)" ) ;


		// Exact integer division:
		ColorCount colorCount = fileSize / 3 ;


		/*
		LogPlug::debug( "Palette loaded from '" + paletteFilename
			+ "' will have " + Ceylan::toString( colorCount ) + " colors." ) ;
		 */

		// This instance will take ownership of it:
		ColorDefinition * colorDefs = new ColorDefinition[ colorCount ] ;


		for ( ColorCount i = 0; i < colorCount; i++ )
		{

			colorDefs[i].r = static_cast<ColorElement>(
				paletteHolder->readUint8() ) ;

			colorDefs[i].g = static_cast<ColorElement>(
				paletteHolder->readUint8() ) ;

			colorDefs[i].b = static_cast<ColorElement>(
				paletteHolder->readUint8() ) ;

			colorDefs[i].unused = Pixels::AlphaOpaque ;

		}

		load( colorCount, colorDefs ) ;


	}
	catch( const Ceylan::Exception & e )
	{

		throw PaletteException( "Palette constructor from file failed: "
			+ e.toString() ) ;

	}

}



Palette::~Palette() throw()
{

	if ( _ownsColorDefinition && _colorDefs != 0 )
	{
		delete [] _colorDefs ;
	}

	invalidatePixelColors() ;

}



void Palette::load( ColorCount numberOfColors, ColorDefinition * colors )
{

	//LogPlug::debug( "Loading palette." ) ;


	_converted = false ;

	if ( _colorDefs != 0 )
		delete [] _colorDefs ;


	if ( colors == 0 )
	{

		// Creates its own palette and set it to full black for more safety:
		_colorDefs = new ColorDefinition[ numberOfColors ] ;

		for ( ColorCount i=0; i < numberOfColors; i++ )
		{

			// Cannot use setColorDefinitionAt, as _numberOfColors not set yet:
			_colorDefs[i].r = 0 ;
			_colorDefs[i].g = 0 ;
			_colorDefs[i].b = 0 ;
			_colorDefs[i].unused = AlphaOpaque ;

		}

	}
	else
	{

		_colorDefs = colors ;

	}


	if ( numberOfColors != _numberOfColors )
	{

		_numberOfColors = numberOfColors ;

		invalidatePixelColors() ;

	}

}



ColorCount Palette::getNumberOfColors() const
{

	return _numberOfColors ;

}



const Pixels::PixelColor & Palette::getPixelColorAt( ColorCount index ) const
{

	if ( index >= _numberOfColors )
		 throw PaletteException(
			"Palette::getPixelColorAt: out of bounds: specified index ("
			+ Ceylan::toString( index ) + ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;

	return _pixelColors[ index ] ;

}



Pixels::PixelColor * Palette::getPixelColors() const
{

	return _pixelColors ;

}



const Pixels::ColorDefinition & Palette::getColorDefinitionAt(
	ColorCount index ) const
{

	if ( index >= _numberOfColors )
		 throw PaletteException(
			"Palette::getColorDefinitionAt: out of bounds: specified index ("
			+ Ceylan::toString( index ) + ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;

	return _colorDefs[ index ] ;

}



void Palette::setColorDefinitionAt(	ColorCount targetIndex,
	const ColorDefinition & newColorDefinition )
{

	if ( targetIndex >= _numberOfColors )
		 throw PaletteException(
			"Palette::setColorDefinitionAt: out of bounds: specified index ("
			+ Ceylan::toString( targetIndex )
			+ ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;

	_colorDefs[ targetIndex ] = newColorDefinition ;

}



void Palette::setColorDefinitionAt(	ColorCount targetIndex,
	ColorElement red, ColorElement green, ColorElement blue,
	ColorElement alpha )
{

	if ( targetIndex >= _numberOfColors )
		 throw PaletteException(
			"Palette::setColorDefinitionAt: out of bounds: "
			"specified index ("
			+ Ceylan::toString( targetIndex )
			+ ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;

	_colorDefs[ targetIndex ].r      = red ;
	_colorDefs[ targetIndex ].g      = green ;
	_colorDefs[ targetIndex ].b      = blue ;
	_colorDefs[ targetIndex ].unused = alpha ;

}



Pixels::ColorDefinition * Palette::getColorDefinitions() const
{

	return _colorDefs ;

}



bool Palette::hasColorKey() const
{

	return _hasColorkey ;

}



ColorCount Palette::getColorKeyIndex() const
{

	if ( ! _hasColorkey )
		throw PaletteException( "Palette::getColorKeyIndex failed: "
			"no available colorkey" ) ;

	return _colorKeyIndex ;

}



void Palette::setColorKeyIndex( ColorCount colorkeyIndex )
{

	// SDL_SetColorKey is relative to a surface.

	_hasColorkey = true ;

	_colorKeyIndex = colorkeyIndex ;

}



void Palette::updatePixelColorsFrom( const PixelFormat & format )
{

	if ( _numberOfColors == 0 )
	{

		LogPlug::warning(
			 "Palette::updatePixelColorsFrom: no color available." ) ;

		_converted = true ;
		return ;

	}

	if ( _pixelColors == 0 )
	{
		_pixelColors = new PixelColor[ _numberOfColors ] ;
	}


	for ( ColorCount c = 0; c < _numberOfColors; c++ )
	{
		_pixelColors[c] = convertColorDefinitionToPixelColor(
			* const_cast<PixelFormat*>( &format ), _colorDefs[c] ) ;
	}

	_converted = true ;

}



void Palette::quantize( Pixels::ColorElement quantizeMaxCoordinate,
	bool scaleUp )
{

	for ( ColorCount index = 0; index < _numberOfColors; index++ )
	{

		_colorDefs[index].r = QuantizeComponent( _colorDefs[index].r,
			quantizeMaxCoordinate ) ;

		_colorDefs[index].g = QuantizeComponent( _colorDefs[index].g,
			quantizeMaxCoordinate ) ;

		_colorDefs[index].b = QuantizeComponent( _colorDefs[index].b,
			quantizeMaxCoordinate ) ;

		// Alpha left as is.


		if ( scaleUp )
		{

			_colorDefs[index].r = static_cast<ColorElement>(
				Ceylan::Maths::Round(
					_colorDefs[index].r * 255.0f / quantizeMaxCoordinate ) ) ;

			_colorDefs[index].g = static_cast<ColorElement>(
				Ceylan::Maths::Round(
					_colorDefs[index].g * 255.0f / quantizeMaxCoordinate ) ) ;

			_colorDefs[index].b = static_cast<ColorElement>(
				Ceylan::Maths::Round(
					_colorDefs[index].b * 255.0f / quantizeMaxCoordinate ) ) ;

			// Alpha left as is.

		}

	}

	invalidatePixelColors() ;

}



void Palette::correctGamma( GammaFactor gamma )
{

	for ( ColorCount index = 0; index < _numberOfColors; index++ )
	{

		if ( _hasColorkey && ( index == _colorKeyIndex ) )
			continue ;

		_colorDefs[index].r = CorrectGammaComponent( _colorDefs[index].r,
			gamma ) ;

		_colorDefs[index].g = CorrectGammaComponent( _colorDefs[index].g,
			gamma ) ;

		_colorDefs[index].b = CorrectGammaComponent( _colorDefs[index].b,
			gamma ) ;

		// Alpha left as is.

	}

	invalidatePixelColors() ;

}



ColorCount Palette::getClosestColorIndexTo(	const ColorDefinition & color )
	const
{

	if ( _numberOfColors == 0 )
		throw PaletteException( "Palette::getClosestColorIndexTo failed: "
			"no color in palette." ) ;

	bool initialized = false ;

	ColorCount bestIndex ;
	ColorDistance smallestDistance, currentDistance ;

	for ( ColorCount i=0; i < _numberOfColors; i++ )
	{

		if ( _hasColorkey && ( i == _colorKeyIndex ) )
			continue ;

		currentDistance = GetDistance( color, _colorDefs[i] ) ;

		if ( initialized )
		{

			if ( currentDistance < smallestDistance )
			{

				smallestDistance = currentDistance ;
				bestIndex = i ;

			}

		}
		else
		{

			smallestDistance = currentDistance ;
			bestIndex = i ;
			initialized = true ;

		}


	}

	return bestIndex ;

}



bool Palette::draw( Surface & targetSurface,
	const ColorDefinition & backgroundColor )
{


	/*
	 * Unconditionnaly reconverts actual pixel colors, since it is not known a
	 * priori whether an already computed pixel color array would correspond to
	 * the same pixel format.
	 *
	 */

	updatePixelColorsFrom( targetSurface.getPixelFormat() ) ;

	targetSurface.lock() ;

	targetSurface.fill( backgroundColor ) ;

	for ( Length i = 0;
		i < targetSurface.getHeight() && i < _numberOfColors; i++ )
	{
		targetSurface.drawHorizontalLine( 0, targetSurface.getWidth(),
			i, getPixelColorAt( i ) ) ;
	}

	targetSurface.unlock() ;

	return ( targetSurface.getHeight() >= _numberOfColors ) ;

}



bool Palette::hasDuplicates( bool useAlpha ) const
{

	if ( _numberOfColors == 0 )
		return false ;

	for ( ColorCount index = 0; index < _numberOfColors-1; index++ )
	{

		if ( _hasColorkey && ( index == _colorKeyIndex ) )
			continue ;

		for ( ColorCount checkIndex = index+1; checkIndex < _numberOfColors;
				checkIndex++ )
			if ( Pixels::areEqual( _colorDefs[index], _colorDefs[checkIndex],
				 useAlpha ) )
			{

				if ( _hasColorkey && ( checkIndex == _colorKeyIndex ) )
					continue ;

				LogPlug::debug( "Palette::hasDuplicates: "
					"duplicate detected at index #" + Ceylan::toString( index )
					+ " and index #" + Ceylan::toString( checkIndex )
					+ " for color definition "
					+ Pixels::toString( _colorDefs[index] ) + "." ) ;

				return true ;

			}

	}

	return false ;

}



void Palette::save( const std::string & filename, bool encoded ) const
{

	// Alpha coordinates are ignored.

	try
	{

		Ceylan::Holder<File> paletteHolder( File::Create( filename ) ) ;

		if ( ! encoded )
		{

			// Write .osdl.palette header:
			paletteHolder->writeUint16( PaletteTag ) ;

			if ( hasColorKey() )
			{

				paletteHolder->writeUint8( 1 ) ;
				paletteHolder->writeUint16( getColorKeyIndex() ) ;

			}
			else
			{

				paletteHolder->writeUint8( 0 ) ;

			}

			// End of header.

		}


		for ( ColorCount index = 0; index < _numberOfColors; index++ )
		{

			if ( encoded )
			{

				/*
				 * Quantized 5-bit components in BGR order:
				 * (base is 0xb1000000000000000 = 32768)
				 *
				 */
				Ceylan::Uint16 colorDefinition = 32768
					|     QuantizeComponent( _colorDefs[index].r )
					| ( ( QuantizeComponent( _colorDefs[index].g ) ) << 5  )
					| ( ( QuantizeComponent( _colorDefs[index].b ) ) << 10 ) ;

				// Beware to endianess:
				paletteHolder->writeUint8( ( colorDefinition & 0xff00 ) >> 8 ) ;
				paletteHolder->writeUint8(   colorDefinition & 0x00ff ) ;

			}
			else
			{

				// Raw components in RGB order:
				paletteHolder->writeUint8( _colorDefs[index].r ) ;
				paletteHolder->writeUint8( _colorDefs[index].g ) ;
				paletteHolder->writeUint8( _colorDefs[index].b ) ;


			}
		}

		// Auto-close on holder deletion.

	}
	catch( const Ceylan::Exception & e )
	{

		throw PaletteException( "Palette::save failed: " + e.toString() ) ;

	}

}



const string Palette::toString( Ceylan::VerbosityLevels level ) const
{

	if ( _numberOfColors == 0 )
		return "Palette has no color defined" ;

	if ( _colorDefs == 0 )
	{

		LogPlug::error( "Palette::toString: palette should have "
			+ Ceylan::toString( _numberOfColors ) + " color definitions, "
			"but no definition is registered (null pointer)." ) ;

		return "Palette has no color defined and is in inconsistent state" ;

	}

	string result = "Palette has " + Ceylan::toString( _numberOfColors )
		+ " color definitions, " ;

	if ( _pixelColors == 0 )
		result += "they are not converted to actual pixel colors" ;
	else
		result += "they are already converted to actual pixel colors" ;

	if ( _hasColorkey )
		result += ". Palette has a colorkey, corresponding to color index #"
			+ Ceylan::toString( _colorKeyIndex ) ;
	else
		result += ". Palette has no colorkey" ;

	if ( level == Ceylan::low )
		return result ;


	result += ". Listing registered color definitions:" ;

	std::list<string> l ;

	for ( ColorCount index = 0; index < _numberOfColors; index++ )
		l.push_back( "Color #" + Ceylan::toString( index ) + ": "
			+ Pixels::toString( _colorDefs[index] ) ) ;


	return result + Ceylan::formatStringList( l ) ;

}






// Static section.



Palette & Palette::CreateGreyScalePalette( ColorCount numberOfColors )
{

	return CreateGradationPalette( Pixels::Black, Pixels::White,
		numberOfColors ) ;

}



Palette & Palette::CreateGradationPalette( const ColorDefinition & colorStart,
	const ColorDefinition & colorEnd, ColorCount numberOfColors )
{

	ColorDefinition * colorBuffer = new ColorDefinition[ numberOfColors ] ;

	if ( colorBuffer == 0 )
		Ceylan::emergencyShutdown(
			"Palette::CreateGradationPalette: not enough memory." ) ;

#if OSDL_DEBUG_PALETTE
	LogPlug::debug( "Gradation ranging from "
		+ Pixels::toString( colorStart ) + " to "
		+ Pixels::toString( colorEnd ) + "." ) ;
#endif // OSDL_DEBUG_PALETTE

	Ceylan::Float32 redIncrement   = (
		static_cast<Ceylan::Float32>( colorEnd.r ) - colorStart.r )
			/ numberOfColors ;

	Ceylan::Float32 greenIncrement = (
		static_cast<Ceylan::Float32>( colorEnd.g ) - colorStart.g )
			/ numberOfColors ;

	Ceylan::Float32 blueIncrement  = (
		static_cast<Ceylan::Float32>( colorEnd.b ) - colorStart.b )
			/ numberOfColors ;

	Ceylan::Float32 alphaIncrement = (
		static_cast<Ceylan::Float32>( colorEnd.unused ) - colorStart.unused )
			/ numberOfColors ;

	for ( ColorCount c = 0; c < numberOfColors; c++ )
	{

		/*
		 * Without Ceil, there would be an offset, for example with 256 colors
		 * since 255/256 < 1.
		 *
		 */

		colorBuffer[ c ].r      = static_cast<ColorElement>(
			Ceylan::Maths::Ceil( colorStart.r + redIncrement * c ) ) ;

		colorBuffer[ c ].g      = static_cast<ColorElement>(
			Ceylan::Maths::Ceil( colorStart.g + greenIncrement * c ) ) ;

		colorBuffer[ c ].b      = static_cast<ColorElement>(
			Ceylan::Maths::Ceil( colorStart.b + blueIncrement * c ) ) ;

		colorBuffer[ c ].unused = static_cast<ColorElement>(
			Ceylan::Maths::Ceil( colorStart.unused + alphaIncrement * c ) ) ;

	}

	return * new Palette( numberOfColors, colorBuffer ) ;

}



Palette & Palette::CreateMasterPalette( bool addColorkey )
{

	ColorCount colorCount = 255 ;

	if ( addColorkey )
		colorCount++ ;

	Palette & palette = * new Palette( colorCount ) ;


	colorCount = 0 ;

	if ( addColorkey )
	{

		// Same convention as for the DS:
		const ColorCount colorKeyIndex = 0 ;

		palette.setColorDefinitionAt( colorKeyIndex, Pixels::DefaultColorkey ) ;
		palette.setColorKeyIndex( colorKeyIndex ) ;
		colorCount++ ;

	}


	/*
	 * First, specify the 240 base colors, sampling the RGB cube uniformly in
	 * each dimension (but with a number of samples per dimension which depends
	 * on that dimension, chosen according to the sensibility of human eye):
	 *
	 */
	const Ceylan::Uint8 redRange   = 6 ;
	const Ceylan::Uint8 greenRange = 8 ;
	const Ceylan::Uint8 blueRange  = 5 ;


	ColorDefinition currentDef ;

	currentDef.unused = Pixels::AlphaOpaque ;

	for ( ColorCount redIndex = 0; redIndex < redRange; redIndex++ )
		for ( ColorCount greenIndex = 0; greenIndex < greenRange;
				greenIndex++ )
			for ( ColorCount blueIndex = 0; blueIndex < blueRange;
				blueIndex++ )
			{

				currentDef.r = static_cast<ColorElement>(
					Ceylan::Maths::Round(
						255.0f * redIndex / (redRange-1) ) ) ;

				currentDef.g = static_cast<ColorElement>(
					Ceylan::Maths::Round(
						255.0f * greenIndex / (greenRange-1) ) ) ;

				currentDef.b = static_cast<ColorElement>(
					Ceylan::Maths::Round(
						255.0f * blueIndex / (blueRange-1) ) ) ;

				palette.setColorDefinitionAt( colorCount, currentDef ) ;

				colorCount++ ;

			}


	// 255-240 = 15 index left, adding greys:
	const ColorCount greyRange = 8 ;

	for ( ColorCount greyIndex = 0; greyIndex < greyRange; greyIndex++ )
	{

		// Avoid pure white and pure black, already available:
		ColorElement greyComponent = static_cast<ColorElement>(
			Ceylan::Maths::Round( 255.0f *(greyIndex+1) / (greyRange+1) ) ) ;

		currentDef.r = greyComponent ;
		currentDef.g = greyComponent ;
		currentDef.b = greyComponent ;

		palette.setColorDefinitionAt( colorCount, currentDef ) ;

		colorCount++ ;

	}

	/*
	 * 7 remaining index, add three 'blue':
	 * (actually coming from the Stan character of Reiner's Tileset)
	 *
	 */
	palette.setColorDefinitionAt( colorCount++, 6,  177, 220 ) ;
	palette.setColorDefinitionAt( colorCount++, 4,  147, 183 ) ;
	palette.setColorDefinitionAt( colorCount++, 28,  95, 139 ) ;

	// One brown/black, for the hair:
	palette.setColorDefinitionAt( colorCount++, 70, 59, 42 ) ;

	// Two orange/pink, for the flesh:
	palette.setColorDefinitionAt( colorCount++, 238, 190, 141 ) ;
	palette.setColorDefinitionAt( colorCount++, 250, 205, 155 ) ;

	// One yellow for blonde hair:
	palette.setColorDefinitionAt( colorCount++, 255, 255, 111 ) ;

	// That's it!

	return palette ;

}





// Protected section.



void Palette::invalidatePixelColors()
{

	if ( _pixelColors != 0 )
	{

		delete [] _pixelColors ;
		_pixelColors = 0 ;

	}

	// Will have to be recomputed when needed.

}





// Static section.



Pixels::ColorElement Palette::QuantizeComponent( ColorElement component,
	Pixels::ColorElement newMaxCoordinate )
{

	return static_cast<ColorElement>( Ceylan::Maths::Round(
		component / 255.0f * newMaxCoordinate ) ) ;

}



Pixels::ColorElement Palette::CorrectGammaComponent(
	ColorElement component, GammaFactor gamma )
{

	// Normalize, apply gamma, denormalize:
	return static_cast<ColorElement>( Ceylan::Maths::Round(
		Ceylan::Maths::Pow( component/255.0f, 1/gamma ) * 255.0f ) ) ;

}



ColorDistance Palette::GetDistance(
	const Pixels::ColorDefinition & firstColor,
	const Pixels::ColorDefinition & secondColor )
{

	// Directly inspired from http://www.compuphase.com/cmetric.htm:

	// Integer division:
	ColorDistance rmean = ( firstColor.r + secondColor.r ) / 2 ;

	ColorDistance rs = ( firstColor.r - secondColor.r ) *
		( firstColor.r - secondColor.r ) ;

	ColorDistance gs = ( firstColor.g - secondColor.g ) *
		( firstColor.g - secondColor.g ) ;

	ColorDistance bs = ( firstColor.b - secondColor.b ) *
		( firstColor.b - secondColor.b ) ;

	return ( ((512+rmean)*rs)>>8 ) + 4*gs + ( ((767-rmean)*bs)>>8 ) ;

}
