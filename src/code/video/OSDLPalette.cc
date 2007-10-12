#include "OSDLPalette.h"

#include "OSDLSurface.h"  // for Surface

#include "Ceylan.h"       // for Ceil



#include <list>


using std::string ;


using namespace Ceylan::Log ;

using namespace OSDL::Video ;


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



PaletteException::PaletteException( const string & message ) throw(): 
	VideoException( "Palette exception: " + message ) 
{

}
	
			
PaletteException::~PaletteException() throw()
{
		
}
		
		
			
			
Palette::Palette( ColorCount numberOfColors, Pixels::ColorDefinition * colors, 
		Pixels::PixelFormat * format ) throw( PaletteException ):
	_numberOfColors( 0 ), 
	_colorDefs( 0 ),
	_pixelColors( 0 ),
	_converted( false )
{

#if OSDL_USES_SDL

	load( numberOfColors, colors ) ;
	
	if ( format != 0 )
		updatePixelColorsFrom( * format ) ;
		
#else // OSDL_USES_SDL

	throw PaletteException( "Palette constructor failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL

}



Palette::Palette( LowLevelPalette & palette ) throw( PaletteException )
{

#if OSDL_USES_SDL

	load( palette.ncolors, palette.colors ) ;
		
#else // OSDL_USES_SDL

	throw PaletteException( "Palette constructor failed :"
		"no SDL support available" ) ;
		
#endif // OSDL_USES_SDL
	
}



Palette::~Palette() throw()
{

	if ( _colorDefs != 0 )
	{
		delete [] _colorDefs ;
	}
	
	if ( _pixelColors != 0 )
	{
		delete [] _pixelColors ;
	}
	
}



void Palette::load( ColorCount numberOfColors, 
	Pixels::ColorDefinition * colors ) throw( PaletteException )
{

	if ( colors == 0 )
		throw PaletteException( "Palette::load: "
			"specified pointer to color definitions is null." ) ;
			
	_converted = false ;

	if ( _colorDefs != 0 )
		delete [] _colorDefs ;

	_colorDefs = colors ;
	
	
	if ( numberOfColors != _numberOfColors )
	{

		_numberOfColors = numberOfColors ;

	
		if ( _pixelColors != 0 )
		{
			delete [] _pixelColors ;
			_pixelColors = 0 ;
		}

		// Will have to be recomputed when needed.
		
	}
		
}



ColorCount Palette::getNumberOfColors() const throw()
{

	return _numberOfColors ;
	
}



const Pixels::PixelColor & Palette::getPixelColorAt( ColorCount index ) 
	const throw( PaletteException )
{

	if ( index >= _numberOfColors )
		 throw PaletteException( 
		 	"Palette::getPixelColorAt: out of bounds: specified index ("
		 	+ Ceylan::toString( index ) + ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;
			
	return _pixelColors[ index ] ;	
	
}



Pixels::PixelColor * Palette::getPixelColors() const throw()
{

	return _pixelColors ;
	
}
 
 
 
const Pixels::ColorDefinition & Palette::getColorDefinitionAt(
	ColorCount index ) const throw( PaletteException )
{

	if ( index >= _numberOfColors )
		 throw PaletteException( 
		 	"Palette::getColorDefinitionAt: out of bounds: "
		 	"specified index ("
		 	+ Ceylan::toString( index ) + ") greater or equal to upper index ("
			+ Ceylan::toString( _numberOfColors ) + ")." ) ;
			
	return _colorDefs[ index ] ;
		
}



Pixels::ColorDefinition * Palette::getColorDefinitions() const throw()
{

	return _colorDefs ;
	
}



void Palette::updatePixelColorsFrom( Pixels::PixelFormat & format ) throw()
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
		_pixelColors = new Pixels::PixelColor[ _numberOfColors ] ;
	}
	
	
	for ( ColorCount c = 0; c < _numberOfColors; c++ )
	{
		_pixelColors[c] = Pixels::convertColorDefinitionToPixelColor( 
			format, _colorDefs[c] ) ;
	}
	
	_converted = true ;
	
}



bool Palette::draw( Surface & targetSurface, 
	Pixels::ColorDefinition backgroundColor ) throw()
{


	/*
	 * Unconditionnaly reconverts actual pixel colors, since it is not
	 * known a priori whether an already computed pixel color array would
	 * correspond to the same pixel format.
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



const string Palette::toString( Ceylan::VerbosityLevels level ) const throw()	
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
	
	if ( _pixelColors != 0 )
		result += "they are not converted to actual pixel colors" ;
	else
		result += "they are already converted to actual pixel colors" ;
		
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


Palette & Palette::CreateGreyScalePalette( ColorCount numberOfColors ) throw()
{

	return CreateGradationPalette( Pixels::Black, Pixels::White, 
		numberOfColors ) ;	
		
}



Palette & Palette::CreateGradationPalette( Pixels::ColorDefinition colorStart,
	Pixels::ColorDefinition colorEnd, ColorCount numberOfColors ) throw()
{


	Pixels::ColorDefinition * colorBuffer = 
		new Pixels::ColorDefinition[ numberOfColors ] ;
	
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
		 * Without Ceil, there would be an offset, for example with 
		 * 256 colors since 255/256 < 1.
		 *
		 */
		
		colorBuffer[ c ].r      = static_cast<Pixels::ColorElement>( 
			Ceylan::Maths::Ceil( colorStart.r + redIncrement * c ) ) ;
			
		colorBuffer[ c ].g      = static_cast<Pixels::ColorElement>( 
			Ceylan::Maths::Ceil( colorStart.g + greenIncrement * c ) ) ;
			
		colorBuffer[ c ].b      = static_cast<Pixels::ColorElement>( 
			Ceylan::Maths::Ceil( colorStart.b + blueIncrement * c ) ) ;
			
		colorBuffer[ c ].unused = static_cast<Pixels::ColorElement>( 
			Ceylan::Maths::Ceil( colorStart.unused + alphaIncrement * c ) ) ;
			
	}

	return * new Palette( numberOfColors, colorBuffer ) ;
		
}


					
Palette & Palette::CreateLandscapePalette( ColorCount numberOfColors ) throw()
{

	// @fixme
	return * new Palette( numberOfColors, 0 ) ;

}

