/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDLSprite.h"

#include "OSDLBoundingBox2D.h"       // for BoundingBox2D
#include "OSDLGLTexture.h"           // for GLTexture


using namespace Ceylan::Log ;

using namespace OSDL::Rendering ;
using namespace OSDL::Video ;

using std::string ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>              // for OSDL_DEBUG_SPRITE and al 
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL 
#include "SDL.h"					 // for SDL_Cursor, etc.
#endif // OSDL_USES_SDL



#if OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message) LogPlug::debug( message ) ;

#else // OSDL_DEBUG_SPRITE

#define OSDL_RENDER_LOG(message)

#endif // OSDL_DEBUG_SPRITE




const Sprite::Shape Sprite::EightTimesEight         =  1 ;
const Sprite::Shape Sprite::SixteenTimesEight       =  2 ;
const Sprite::Shape Sprite::ThirtyTwoTimesEight     =  3 ;

const Sprite::Shape Sprite::EightTimesSixteen 	    =  4 ;
const Sprite::Shape Sprite::SixteenTimesSixteen	    =  5 ;
const Sprite::Shape Sprite::ThirtyTwoTimesSixteen   =  6 ;

const Sprite::Shape Sprite::EightTimesThirtyTwo     =  7 ;
const Sprite::Shape Sprite::SixteenTimesThirtyTwo   =  8 ;
const Sprite::Shape Sprite::ThirtyTwoTimesThirtyTwo =  9 ;
const Sprite::Shape Sprite::SixtyFourTimesThirtyTwo = 10 ;

const Sprite::Shape Sprite::ThirtyTwoTimesSixtyFour = 11 ;
const Sprite::Shape Sprite::SixtyFourTimesSixtyFour = 12 ;

const Sprite::Shape Sprite::PowersOfTwo             = 13 ;




SpriteException::SpriteException( const string & message ) : 
	VideoRenderingException( "Sprite exception: " + message ) 
{

}
	
	
			
SpriteException::~SpriteException() throw()
{
		
}




Sprite::Sprite( const string & frameFilename )
{

	// TO-DO.
}



/*

Sprite::Sprite( Shape spriteShape, bool ownBoundingBox ):
	View(),
	_ownBoundingBox( ownBoundingBox ),
	_box( 0 ),
	_shape( spriteShape )
#if OSDL_USES_OPENGL
	, _texture( 0 )
#endif // OSDL_USES_OPENGL	
{

}

*/



Sprite::~Sprite() throw()
{

	if ( _ownBoundingBox && ( _box != 0 ) )
		delete _box ;

#if OSDL_USES_OPENGL

	if ( _texture != 0 )
		delete _texture ;

#endif // OSDL_USES_OPENGL	
		
}



const string Sprite::toString( Ceylan::VerbosityLevels level ) const 
{

	string res = "Sprite which " ;
	
	if ( _ownBoundingBox )
		res += "owns" ;
	else
		res += " does not own" ;
	
	res += " its bounding box. " + View::toString( level ) ;		
	
	res += ". Its shape is: " + DescribeShape( _shape ) ;
		
#if OSDL_USES_OPENGL

	if ( _texture == 0 )
		res += ". No texture is associated with this sprite" ;
	else
		res += ". A texture is associated with this sprite; " 
			+ _texture->toString() ;
			
#endif // OSDL_USES_OPENGL	

	return res ;
	
}





// Static section.



string Sprite::DescribeShape( Shape shape )
{


	switch( shape )
	{
	
		case EightTimesEight:
			return "8x8" ;
			break ;
			
		case SixteenTimesEight:
			return "16x8" ;
			break ;
			
		case ThirtyTwoTimesEight:
			return "32x8" ;
			break ;
			
			
		case EightTimesSixteen:
			return "8x16" ;
			break ;
			
		case SixteenTimesSixteen:
			return "16x16" ;
			break ;
			
		case ThirtyTwoTimesSixteen:
			return "32x16" ;
			break ;
			
			
		case EightTimesThirtyTwo:
			return "8x32" ;
			break ;
			
		case SixteenTimesThirtyTwo:
			return "16x32" ;
			break ;
			
		case ThirtyTwoTimesThirtyTwo:
			return "32x32" ;
			break ;
			
		case SixtyFourTimesThirtyTwo:
			return "64x32" ;
			break ;
			
			
		case ThirtyTwoTimesSixtyFour:
			return "32x64" ;
			break ;
			
		case SixtyFourTimesSixtyFour:
			return "64x64" ;
			break ;
			
		case PowersOfTwo:
			return "both dimensions are powers of two" ;
			break ;
			
		default:
			throw SpriteException( "Sprite::DescribeShape failed: "
				"unknown shape (" + Ceylan::toNumericalString( shape ) + ")" ) ;
			break ;
			
	
	}
	
}
				
	
				
Sprite::Shape Sprite::GetSmallestEnclosingShape( Length width, Length height )
{

#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw SpriteException( "Sprite::GetSmallestEnclosingShape failed: "
		"not supported on the ARM7." ) ;
		
#elif defined(OSDL_RUNS_ON_ARM9)


	/*
	 * Used for Nintendo assets, but may be used on a PC as well (ex: PC tools 
	 * for the DS).
	 * Thus no OSDL_ARCH_NINTENDO_DS here.
	 *
	 * @see also http://www.palib.info/wiki/doku.php?id=day4#sprite_sizes
	 *
	 */
	
	if ( width > 64 || height > 64 )
		throw SpriteException( "Sprite::GetSmallestEnclosingShape failed: "
			"size " + Ceylan::toString(width) + "x" + Ceylan::toString(height)
			+ " too large for standard shapes" ) ;
	

	if ( width > 32 )
		if ( height > 32 )
			return SixtyFourTimesSixtyFour ;
		else
			return SixtyFourTimesThirtyTwo ;
	
	// Here width <= 32.
	
	if ( height > 32 )
		return ThirtyTwoTimesSixtyFour ;
		
	// Here height <= 32.
	
	
	if ( width > 16 )
		if ( height > 16 )
			return ThirtyTwoTimesThirtyTwo ;
		else if ( height > 8 )
			return ThirtyTwoTimesSixteen ;
		else
			return ThirtyTwoTimesEight ;
	
	// Here width <= 16.
	
	if ( width > 8 )
		if ( height > 16 )
			return SixteenTimesThirtyTwo ;
		else if ( height > 8 )
			return SixteenTimesSixteen ;
		else
			return SixteenTimesEight ;
		
	
	// Here width <= 8.
	if ( height > 16 )
		return EightTimesThirtyTwo ;
	else if ( height > 8 )
		return EightTimesSixteen ;
	else
		return EightTimesEight ;

#endif //  defined(OSDL_RUNS_ON_ARM9)

#else // OSDL_ARCH_NINTENDO_DS

	LogPlug::error( "Sprite::GetSmallestEnclosingShape failed: "
		"not to be used if not on the Nintendo DS." ) ;

	// FIXME:
	return 1 ;

#endif // OSDL_ARCH_NINTENDO_DS
			 
}
	
	
	
Length Sprite::GetShapeWidthFor( Shape shape )
{

	switch( shape )
	{
	
		case EightTimesEight:
		case EightTimesSixteen:
		case EightTimesThirtyTwo:
			return 8 ;
			break ;
	
		case SixteenTimesEight:
		case SixteenTimesSixteen:
		case SixteenTimesThirtyTwo:
			return 16 ;
			break ;
	
		case ThirtyTwoTimesEight:
		case ThirtyTwoTimesSixteen:
		case ThirtyTwoTimesThirtyTwo:
		case ThirtyTwoTimesSixtyFour:
			return 32 ;
			break ;
	
		case SixtyFourTimesThirtyTwo:
		case SixtyFourTimesSixtyFour:
			return 64 ;
			break ;
		
		default:
			throw SpriteException( "Sprite::GetShapeWidthFor failed: "
				"unknown shape (" + Ceylan::toNumericalString( shape ) + ")" ) ;
			break ;
	}

}



Length Sprite::GetShapeHeightFor( Shape shape )
{

	switch( shape )
	{
	
		case EightTimesEight:
		case SixteenTimesEight:
		case ThirtyTwoTimesEight:
			return 8 ;
			break ;
	
		case EightTimesSixteen:
		case SixteenTimesSixteen:
		case ThirtyTwoTimesSixteen:
			return 16 ;
			break ;
	
		case EightTimesThirtyTwo:
		case SixteenTimesThirtyTwo:
		case ThirtyTwoTimesThirtyTwo:
		case SixtyFourTimesThirtyTwo:
			return 32 ;
			break ;
	
		case ThirtyTwoTimesSixtyFour:
		case SixtyFourTimesSixtyFour:
			return 64 ;
			break ;
		
		default:
			throw SpriteException( "Sprite::GetShapeHeightFor failed: "
				"unknown shape (" + Ceylan::toNumericalString( shape ) + ")" ) ;
			break ;
	}

}

