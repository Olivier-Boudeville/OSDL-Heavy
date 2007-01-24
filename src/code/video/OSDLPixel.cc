#include "OSDLPixel.h"

#include "OSDLUtils.h"           // for getBackendLastError
#include "OSDLSurface.h"         // for Surface
#include "OSDLOpenGL.h"          // for OpenGL color masks

#include "OSDLFromGfx.h"         // taken from SDL_gfx

#include "ceylan.h"              // for CEYLAN_DETECTED_LITTLE_ENDIAN
#include "SDL_gfxPrimitives.h"   // for all graphics primitives

#include <list>




using std::string ;


using namespace Ceylan::Log ;
using namespace OSDL::Video ;



// Different colors described by name.



/// Fully Transparent special color :

extern const ColorDefinition OSDL::Video::Pixels::Transparent     = 
	{   0,   0,   0,  0  } ; 




/// Shades of Grey :

extern const ColorDefinition OSDL::Video::Pixels::Black           = 
	{   0,   0,   0, 255 } ; 
	
extern const ColorDefinition OSDL::Video::Pixels::Grey            = 
	{ 190, 190, 190, 255 } ; 
	
extern const ColorDefinition OSDL::Video::Pixels::DimGrey         = 
	{ 105, 105, 105, 255 } ; 
	
extern const ColorDefinition OSDL::Video::Pixels::LightGrey       = 
	{ 211, 211, 211, 255 } ; 
	
extern const ColorDefinition OSDL::Video::Pixels::SlateGrey       = 
	{ 112, 128, 144, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Silver          = 
	{ 230, 232, 250, 255 } ;




/// Shades of Blue :

extern const ColorDefinition OSDL::Video::Pixels::AliceBlue       = 
	{ 240, 248, 255, 255 } ;

extern const ColorDefinition OSDL::Video::Pixels::BlueViolet      = 
	{ 138,  43, 226, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::CadetBlue       = 
	{  95, 158, 160, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DarkSlateBlue   = 
	{  72,  61, 139, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DarkTurquoise   = 
	{   0, 206, 209, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DeepSkyBlue     = 
	{   0, 191, 255, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DodgerBlue      = 
	{  30, 144, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightBlue       = 
	{ 173, 216, 230, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::LightCyan       = 
	{ 224, 255, 255, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::MediumBlue      = 
	{ 123, 104, 238, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::NavyBlue        = 
	{   0,   0, 128, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::RoyalBlue       = 
	{  65, 105, 225, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::SkyBlue         = 
	{ 135, 206, 235, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::SlateBlue       = 
	{ 106,  90, 205, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::SteelBlue       = 
	{  70, 130, 180, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Aquamarine      = 
	{ 127, 255, 212, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Azure           = 
	{ 240, 255, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Blue            = 
	{   0,   0, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Cyan            = 
	{   0, 255, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Turquoise       = 
	{  64, 224, 208, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MidnightBlue    = 
	{  25,  25, 112, 255 } ; 




/// Shades of Brown :

extern const ColorDefinition OSDL::Video::Pixels::Brown           = 
	{ 165,  42,  42, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::RosyBrown       = 
	{ 188, 143, 143, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::SaddleBrown     = 
	{ 139,  69,  19, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Beige           = 
	{ 245,  42,  42, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Burlywood       = 
	{ 222, 184, 135, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Chocolate       = 
	{ 210, 105,  30, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Peru            = 
	{ 205, 133,  63, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Tan             = 
	{ 210, 180, 140, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Copper          = 
	{ 184, 115,  51, 255 } ; 




/// Shades of Green :

extern const ColorDefinition OSDL::Video::Pixels::DarkGreen       = 
	{	0, 100,   0, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DarkOliveGreen  = 
	{  85, 107,  47, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::ForestGreen     = 
	{  34, 139,  34, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::GreenYellow     = 
	{ 173, 255,  47, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::LawnGreen       = 
	{ 124, 252,   0, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::LimeGreen       = 
	{  50, 205,  50, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MintCream       = 
	{ 245, 255, 250, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::OliveDrab       = 
	{ 107, 142,  35, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::PaleGreen       = 
	{ 152, 251, 152, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::SeaGreen        = 
	{  46, 139,  87, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::SpringGreen     = 
	{	0, 255, 127, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::YellowGreen     = 
	{ 154, 205,  50, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Chartreuse      = 
	{ 127, 255,   0, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Green           = 
	{	0, 255,   0, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Khaki           = 
	{ 240, 230, 140, 255 } ; 




/// Shades of Orange :

extern const ColorDefinition OSDL::Video::Pixels::DarkOrange      = 
	{ 255, 140,   0, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::DarkSalmon      = 
	{ 233, 150, 122, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightCoral      = 
	{ 240, 128, 128, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightSalmon     = 
	{ 255, 160, 122, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::PeachPuff       = 
	{ 255, 218, 185, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Bisque          = 
	{ 255, 228, 196, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Coral           = 
	{ 255, 127,  80, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Honeydew        = 
	{ 240, 255, 240, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Orange          = 
	{ 255, 165,   0, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Salmon          = 
	{ 250, 128, 114, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Sienna          = 
	{ 160,  82,  45, 255 } ; 




/// Shades of Red :

extern const ColorDefinition OSDL::Video::Pixels::DeepPink        = 
	{ 255,  20, 147, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::HotPink         = 
	{ 255, 105, 180, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::IndianRed       = 
	{ 205,  92,  92, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightPink       = 
	{ 255, 182, 193, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MediumVioletRed = 
	{ 199,  21, 133, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MistyRose       = 
	{ 255, 228, 225, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::OrangeRed       = 
	{ 255,  69,   0, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::VioletRed       = 
	{ 208,  32, 144, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Firebrick       = 
	{ 178,  34, 34 , 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Pink            = 
	{ 255, 192, 203, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Red             = 
	{ 255,   0,   0, 255 } ; 

extern const ColorDefinition OSDL::Video::Pixels::Tomato          = 
	{ 255,  99,  71, 255 } ;
 



/// Shades of Violet :

extern const ColorDefinition OSDL::Video::Pixels::DarkOrchid      = 
	{ 153,  50, 204, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::DarkViolet      = 
	{ 148,   0, 211, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LavenderBlush   = 
	{ 255, 240, 245, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MediumOrchid    = 
	{ 186,  85, 211, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::MediumPurple    = 
	{ 147, 112, 219, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Lavender        = 
	{ 230, 230, 250, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Magenta         = 
	{ 255,   0, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Maroon          = 
	{ 176,  48,  96, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Orchid          = 
	{ 218, 112, 214, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Plum            = 
	{ 221, 160, 221, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Purple          = 
	{ 160,  32, 240, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Thistle         = 
	{ 216, 191, 216, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Violet          = 
	{ 238, 130, 238, 255 } ;
 



/// Shades of White :

extern const ColorDefinition OSDL::Video::Pixels::AntiqueWhite    = 
	{ 250, 235, 215, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::FloralWhite     = 
	{ 255, 250, 240, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::GhostWhite      = 
	{ 248, 248, 255, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::NavajoWhite     = 
	{ 255, 222, 173, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::OldLace         = 
	{ 253, 245, 230, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::WhiteSmoke      = 
	{ 245, 245, 245, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Gainsboro       = 
	{ 220, 220, 220, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Ivory           = 
	{ 255, 255, 240, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Linen           = 
	{ 250, 240, 230, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Seashell        = 
	{ 255, 245, 238, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Snow            = 
	{ 255, 250, 250, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Wheat           = 
	{ 245, 222, 179, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::White           = 
	{ 255, 255, 255, 255 } ; 




/// Shades of Yellow :

extern const ColorDefinition OSDL::Video::Pixels::BlanchedAlmond  = 
	{ 255, 235, 205, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::DarkGoldenrod   = 
	{ 184, 134,  11, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LemonChiffon    = 
	{ 255, 250, 205, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightGoldenrod  = 
	{ 238, 221, 130, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::LightYellow     = 
	{ 255, 255, 224, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::PaleGoldenrod   = 
	{ 238, 232, 170, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::PapayaWhip      = 
	{ 255, 239, 213, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Cornsilk        = 
	{ 255, 248, 220, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Gold            = 
	{ 255, 215,   0, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Goldenrod       = 
	{ 218, 165,  32, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Moccasin        = 
	{ 255, 228, 181, 255 } ;
 
extern const ColorDefinition OSDL::Video::Pixels::Yellow          = 
	{ 255, 255,   0, 255 } ; 




bool Pixels::setGamma( GammaFactor red, GammaFactor green, GammaFactor blue )
	throw()
{

	if ( SDL_SetGamma( red, green, blue ) == -1 )
	{
		LogPlug::error( "Pixels::setGamma : " 
			+ Utils::getBackendLastError() ) ;
		return false ;
	}
	
	return true ;
	
}


bool Pixels::setGammaRamp( GammaRampElement * redRamp, 
	GammaRampElement * greenRamp, GammaRampElement * blueRamp ) throw()
{

	if ( SDL_SetGammaRamp( redRamp, greenRamp, blueRamp ) == -1 )
	{
		LogPlug::error( "Pixels::setGammaRamp : " 
			+ Utils::getBackendLastError() ) ;
		return false ;
	}
	
	return true ;

}	


bool Pixels::getGammaRamp( GammaRampElement * redRamp, 
	GammaRampElement * greenRamp, GammaRampElement * blueRamp ) throw()
{

	if ( SDL_GetGammaRamp( redRamp, greenRamp, blueRamp ) == -1 )
	{
		LogPlug::error( "Pixels::getGammaRamp : " 
			+ Utils::getBackendLastError() ) ;
		return false ;
	}
	
	return true ;

}	



// Color masks.


void Pixels::getRecommendedColorMasks( ColorMask & redMask, 
	ColorMask & greenMask, ColorMask & blueMask, 
	ColorMask & alphaMask ) throw()
{


#if OSDL_DEBUG

	// Check endianness here just to output a log message :
	
#if CEYLAN_DETECTED_LITTLE_ENDIAN

	LogPlug::debug( "Pixels::getRecommendedColorMasks (with alpha): "
		"using little endian convention." ) ;
		
#else // CEYLAN_DETECTED_LITTLE_ENDIAN

	LogPlug::debug( "Pixels::getRecommendedColorMasks (with alpha): "
		"using big endian convention." ) ;
		
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
	
	/*
	 * Ensure color masks are only defined once (in OpenGL module) to 
	 * avoid disaster :
	 *
	 */
	
	redMask   = OpenGL::RedMask ;
	greenMask = OpenGL::GreenMask ;
	blueMask  = OpenGL::BlueMask ;
	alphaMask = OpenGL::AlphaMask ;
		
}	
								
				
void Pixels::getRecommendedColorMasks( ColorMask & redMask, 
	ColorMask & greenMask, ColorMask & blueMask ) throw()
{



#if OSDL_DEBUG

	// Check endianness here just to output a log message :
	
#if CEYLAN_DETECTED_LITTLE_ENDIAN

	LogPlug::debug( "Pixels::getRecommendedColorMasks (with alpha): "
		"using little endian convention." ) ;
		
#else // CEYLAN_DETECTED_LITTLE_ENDIAN

	LogPlug::debug( "Pixels::getRecommendedColorMasks (with alpha): "
		"using big endian convention." ) ;
		
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN

	
	redMask   = OpenGL::RedMask ;
	greenMask = OpenGL::GreenMask ;
	blueMask  = OpenGL::BlueMask ;

}	
				

void Pixels::getCurrentColorMasks( const Pixels::PixelFormat & format, 
	Pixels::ColorMask & redMask,  Pixels::ColorMask & greenMask, 
	Pixels::ColorMask & blueMask, Pixels::ColorMask & alphaMask ) throw()
{

	redMask   = format.Rmask ;
	greenMask = format.Gmask ;
	blueMask  = format.Bmask ;
	alphaMask = format.Amask ;
		
}



// Color conversion section.


ColorDefinition Pixels::convertRGBAToColorDefinition( 
	ColorElement red,  ColorElement green, 
	ColorElement blue, ColorElement alpha ) throw()
{

	ColorDefinition result ;
	
	result.r      = red ;
	result.g      = green ;	
	result.b      = blue ;	
	result.unused = alpha ;	
	
	return result ;
}
	
				
void Pixels::convertColorDefinitionToRGBA( ColorDefinition color,
	ColorElement & red, ColorElement & green, ColorElement & blue, 
	ColorElement & alpha ) throw() 
{

	red   = color.r ;
	green = color.g ;
	blue  = color.b ;
	alpha = color.unused ;
	
}	
							
				
PixelColor Pixels::convertRGBAToPixelColor( const Pixels::PixelFormat & format,
	ColorElement red, ColorElement green, ColorElement blue, 
	ColorElement alpha ) throw() 
{	

	/*
	
	LogPlug::debug( "convertRGBAToPixelColor : pixel format is " 
		+ Pixels::toString( format ) + ", pixel is [ R, G, B, A ] = [ "
				+ Ceylan::toString( static_cast<Uint16>( red ) ) )   + " ; "
				+ Ceylan::toString( static_cast<Uint16>( green ) ) + " ; "
				+ Ceylan::toString( static_cast<Uint16>( blue  ) )  + " ; "
				+ Ceylan::toString( static_cast<Uint16>( alpha ) )  + " ] " ) ;
	*/
				
	return SDL_MapRGBA( const_cast<Pixels::PixelFormat *>( & format ), 
		red, green, blue, alpha ) ;
	
}


ColorDefinition Pixels::convertPixelColorToColorDefinition(
	 const PixelFormat & format, PixelColor pixel )	throw()
{

	ColorDefinition pixDef ;

	SDL_GetRGBA( pixel, const_cast<Pixels::PixelFormat *>( & format ), 
		& pixDef.r, & pixDef.g, & pixDef.b, & pixDef.unused ) ;
	
	return pixDef ;
	
}


PixelColor Pixels::convertColorDefinitionToPixelColor( 
	const PixelFormat & format, ColorDefinition colorDef ) throw()
{

	return SDL_MapRGBA( const_cast<Pixels::PixelFormat *>( & format ),
		colorDef.r, colorDef.g, colorDef.b, colorDef.unused ) ;
	
}


PixelColor Pixels::convertColorDefinitionToRawPixelColor( 
	ColorDefinition colorDef ) throw()
{

	/*
	 * This is a very artificial transformation, requested by the compiler :
	 * from a set of four bytes (struct SDL_Color) to a Uint32...
	 *
	 */
	return ((Ceylan::Uint32) colorDef.r << 24) 
		 | ((Ceylan::Uint32) colorDef.g << 16) 
		 | ((Ceylan::Uint32) colorDef.b << 8 ) 
		 | ((Ceylan::Uint32) colorDef.unused ) ;
		 
}


PixelColor Pixels::convertRGBAToRawPixelColor( 
	ColorElement red, ColorElement green, ColorElement blue, 
	ColorElement alpha ) throw()
{
	return ((Uint32) red << 24) | ((Uint32) green << 16) 
		 | ((Uint32) blue << 8) | ((Uint32) alpha ) ;
	
}




// Color comparisons section.


bool Pixels::areEqual( ColorDefinition first, ColorDefinition second, 
	bool useAlpha ) throw()
{
	

#if OSDL_DEBUG_COLOR
	LogPlug::trace( "Pixels::areEqual : comparing " 
		+ Pixels::toString( first ) + " with "
		+ Pixels::toString( second ) 
		+ ( useAlpha ? " (alpha taken into account)" : " (alpha ignored)" ) ) ;
#endif // OSDL_DEBUG_COLOR
	
	if ( first.r != second.r )
		return false ;

	if ( first.g != second.g )
		return false ;
		
	if ( first.b != second.b )
		return false ;

	if ( useAlpha )
		return ( first.unused == second.unused ) ;
	else
		return true ;	
		
}


bool Pixels::isLess( ColorDefinition value, ColorDefinition comparison ) throw()
{

	if ( value.r < comparison.r )
		return true ;
		
	if ( value.r > comparison.r )
		return false ;
		
		
	if ( value.g < comparison.g )
		return true ;
		
	if ( value.g > comparison.g )
		return false ;
		
		
	if ( value.b < comparison.b )
		return true ;
		
	if ( value.b > comparison.b )
		return false ;
		
		
	if ( value.unused < comparison.unused )
		return true ;
	
	if ( value.unused > comparison.unused )
		return false ;
	
	return false ;	
		
}					



bool Pixels::areEqual( PixelColor first, PixelColor second ) throw()
{

	return first == second ;
	
}


		
ColorDefinition Pixels::selectColorDifferentFrom( ColorDefinition first, 
	ColorDefinition second ) throw() 	
{

	// Three different choices are always enough :
	ColorDefinition res = Pixels::Red ;
	
	if ( ( ! areEqual( res, first, /* useAlpha */ false ) ) 
			&& ( ! areEqual( res, second, /* useAlpha */ false ) ) )
		return res ;
	
	res = Pixels::Green ;
		
	if ( ( ! areEqual( res, first, /* useAlpha */ false ) ) 
			&& ( ! areEqual( res, second, /* useAlpha */ false ) ) )
		return res ;
	
	return Pixels::Blue ;
			
}
		
		
ColorDefinition Pixels::selectColorDifferentFrom( ColorDefinition first, 
	ColorDefinition second, ColorDefinition third ) throw() 	
{

	// Four different choices are always enough :
	ColorDefinition res = Pixels::Red ;
	
	if ( ( ! areEqual( res, first, /* useAlpha */ false ) ) 
		&& ( ! areEqual( res, second, /* useAlpha */ false  ) 
			&& ( ! areEqual( res, third, /* useAlpha */ false ) ) ) )
		return res ;
	
	res = Pixels::Green ;
		
	if ( ( ! areEqual( res, first, /* useAlpha */ false ) ) 
		&& ( ! areEqual( res, second, /* useAlpha */ false  ) 
			&& ( ! areEqual( res, third, /* useAlpha */ false ) ) ) )
		return res ;
	
	res = Pixels::Blue ;
		
	if ( ( ! areEqual( res, first, /* useAlpha */ false ) ) 
		&& ( ! areEqual( res, second, /* useAlpha */ false  ) 
			&& ( ! areEqual( res, third, /* useAlpha */ false ) ) ) )
		return res ;
	
	return Pixels::Black ;
	
}
		
		

// get/put pixel operations.


Pixels::PixelColor Pixels::getPixelColor( const Surface & fromSurface,
	Coordinate x, Coordinate y ) throw ( VideoException )
{

    BytesPerPixel bpp = fromSurface.getBytesPerPixel() ;
	
    /*
	 * Here p is the address to the pixel whose color we want to 
	 * retrieve :
	 *
	 */
    Uint8 * p = reinterpret_cast<Uint8 *>( fromSurface.getPixels() )
		+ y * fromSurface.getPitch() + x * bpp ;

    switch( bpp ) 
	{
	
    	case 1:
    	    return * p ;

    	case 2:
    	    return *( Uint16 * ) p ;

   		 case 3:
#if CEYLAN_DETECTED_LITTLE_ENDIAN		 
   	        return p[0] | ( p[1] << 8 ) | p[2] << 16 ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
			return ( p[0] << 16 ) | ( p[1] << 8 ) | p[2] ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN
 
    	case 4:
   	  		return *( Uint32 * ) p ;
 	
    	default:
        	throw VideoException( "Abnormal bit per pixel detected "
				"in Pixels::getPixelColor" ) ;
    }
	
} 


Pixels::ColorDefinition Pixels::getColorDefinition( 
		const Surface & fromSurface, Coordinate x, Coordinate y ) 
	throw ( VideoException )
{

	return convertPixelColorToColorDefinition( fromSurface.getPixelFormat(),
		getPixelColor( fromSurface, x, y ) ) ;
		
}



		
void Pixels::putRGBAPixel( Surface & targetSurface, 
		Coordinate x, Coordinate y, 
		ColorElement red, ColorElement green, ColorElement blue, 
		ColorElement alpha, bool blending, bool clipping, bool locking ) 
	throw( VideoException )
{

	putPixelColor( targetSurface, x, y, 
		convertRGBAToPixelColor( targetSurface.getPixelFormat(), 
			red, green, blue, alpha ), alpha, blending, clipping, locking ) ;

}


void Pixels::putColorDefinition( Surface & targetSurface, 
		Coordinate x, Coordinate y, ColorDefinition colorDef, 
		bool blending, bool clipping, bool locking )
	throw( VideoException )
{

	putPixelColor( targetSurface, x, y, 
		convertRGBAToPixelColor( targetSurface.getPixelFormat(), 
			colorDef.r, colorDef.g, colorDef.b, colorDef.unused ), 
		colorDef.unused, blending, clipping, locking ) ;
		
}	


void Pixels::putPixelColor( Surface & targetSurface, 
		Coordinate x, Coordinate y, PixelColor convertedColor, 
		ColorElement alpha, bool blending, bool clipping, bool locking ) 
	throw( VideoException )
{
	
	/*
	 * The lock method is conditional, mustBeLocked() tests could not be used.
	 *
	 */

	/* 
	 * Selects the right SDL_gfx primitive to call :
	 * [ blending, clipping, locking ] = [ B, C, L ]
	 *
	 */
	
	if ( locking )
	{
		
		if ( clipping )
		{
		
			if ( blending )
			{
			
				// [ B, C, L ] = [ 1, 1, 1 ]
				
				if ( targetSurface.mustBeLocked() )
				{	
					targetSurface.lock() ;
					
					::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
						convertedColor, alpha ) ;
					
					targetSurface.unlock() ;								
				}
				else
				{
					::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
						convertedColor, alpha ) ;	
				}
				
			}
			else // blending is false :
			{
			
				// [ B, C, L ] = [ 0, 1, 1 ]

				if ( targetSurface.mustBeLocked() )
				{	
					targetSurface.lock() ;
					
					::fastPixelColorNolock( & targetSurface.getSDLSurface(), 
						x, y, convertedColor ) ;
								
					targetSurface.unlock() ;								
				}
				else
				{
					::fastPixelColorNolock( & targetSurface.getSDLSurface(), 
						x, y, convertedColor ) ;			
				}
					
			} // end blending
		
		} 
		else // clipping is false (beware !) :
		{
		
			if ( blending )
			{
			
				// [ B, C, L ] = [ 1, 0, 1 ]
			
				/*
				 * Clipping is done nevertheless (cannot be disabled, not 
				 * that bad).
				 *
				 */
				
				if ( targetSurface.mustBeLocked() )
				{	
					targetSurface.lock() ;
					::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
						convertedColor, alpha ) ;
					targetSurface.unlock() ;								
				}
				else
				{
					::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
						convertedColor, alpha ) ;	
				}
			
			
			}
			else // blending is false :
			{
			
				// [ B, C, L ] = [ 0, 0, 1 ]
			
				// This time, clipping is *not* performed, as wanted...
				
				if ( targetSurface.mustBeLocked() )
				{	
					targetSurface.lock() ;
					::fastPixelColorNolockNoclip( 
						& targetSurface.getSDLSurface(), 
						x, y, convertedColor ) ;
					targetSurface.unlock() ;								
				}
				else
				{
					::fastPixelColorNolockNoclip( 
						& targetSurface.getSDLSurface(), 
						x, y, convertedColor ) ;		
				}
			
								
			} // end blending
		
		
		} // end clipping
		
		
	} 
	else // locking is false :
	{

		if ( clipping )
		{

	
			if ( blending )
			{
			
				// [ B, C, L ] = [ 1, 1, 0 ]
				
				// This is the most commonly used case !
				
				//alternativePut( targetSurface, x, y, convertedColor ) ;	
				::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
					convertedColor, alpha ) ;			
					
			}
			else // blending is false :
			{		
			
				// [ B, C, L ] = [ 0, 1, 0 ]
				
				::fastPixelColorNolock( & targetSurface.getSDLSurface(), 
					x, y, convertedColor ) ;
				
			} // end blending
	
		} 
		else // clipping is false (beware !) :
		{
		
			if ( blending )
			{

				// [ B, C, L ] = [ 1, 0, 0 ]

				// Actually the clipping is made in this case too.
				::putPixelAlpha( & targetSurface.getSDLSurface(), x, y, 
					convertedColor, alpha ) ;	
				
			}
			else // blending is false :
			{
			
				// [ B, C, L ] = [ 0, 0, 0 ]
				
				::fastPixelColorNolockNoclip( & targetSurface.getSDLSurface(), 
					x, y, convertedColor ) ;
			
			} // end blending
		
		
		} // end clipping


	} // end locking


}


void Pixels::alternativePutPixelColor( Surface & targetSurface, 
	Coordinate x, Coordinate y, PixelColor color, bool mapToSurfaceFormat )
		throw()
{

	/*
	
	LogPlug::debug( "Pixels::alternativePut called for point in [ "
		+ Ceylan::toString( x ) + " ; " 
		+ Ceylan::toString( y ) + " ] with pixel color "
		+ Ceylan::toString( color ) 
		+ " (" + Ceylan::toString( color, true ) + " )." ) ;
	
	*/

	ColorMask redMask, greenMask, blueMask, alphaMask ;
	Pixels::getRecommendedColorMasks( redMask, greenMask, blueMask, 
		alphaMask ) ;

	if ( mapToSurfaceFormat )
	{
		color = SDL_MapRGBA( & targetSurface.getPixelFormat(), 
			( color & redMask   ) >> 24,
		    ( color & greenMask ) >> 16, 
			( color & blueMask  ) >>  8,
			  color & alphaMask ) ;
	}
					
	BytesPerPixel bytes = targetSurface.getBytesPerPixel() ;
	
    // Here p is the address of the pixel whose color is to be set :
    ColorElement * p = reinterpret_cast<ColorElement *>(
			targetSurface.getPixels() )	
			+ y * targetSurface.getPitch() + x * bytes ;

    switch( bytes ) 
	{
	
    	case 1:
    	    *p = color ;
    	    break ;

    	case 2:
    	    *( Uint16 * ) p = color ;
    	    break ;

   	 	case 3:
#if CEYLAN_DETECTED_LITTLE_ENDIAN == SDL_BIG_ENDIAN ) 
   	        	p[0] =   color         & 0xff ;
            	p[1] = ( color >> 8  ) & 0xff ;
            	p[2] = ( color >> 16 ) & 0xff ;
#else // CEYLAN_DETECTED_LITTLE_ENDIAN
          		p[0] = ( color >> 16 ) & 0xff ;
				p[1] = ( color >>  8 ) & 0xff ;
        	    p[2] =   color         & 0xff ;
#endif // CEYLAN_DETECTED_LITTLE_ENDIAN		
        	break ;

		case 4:
        	*( Uint32 * ) p = color ;
        	break ;
			
		default:
			LogPlug::error( "Pixels::alternativePut : "
				"unexpected bytes per pixel specified ("
				+ Ceylan::toString( 
					static_cast<Ceylan::Uint16>( bytes ) ) 
				+ "), nothing done." ) ;
			break ;
			
    }
}


	
string Pixels::toString( const Pixels::PixelFormat & format ) throw()
{

	string result = "Pixel format description "
		"(masks are binary masks used to retrieve individual color values, "
		"loss is the precision loss of each color component, "
		"shift correspond to binary left shifts of each "
		"color component in the pixel value) : " ;

	std::list<string> l ;

	l.push_back( Ceylan::toNumericalString( format.BitsPerPixel ) 
		+ " bits per pixel." ) ;
		
	l.push_back( Ceylan::toNumericalString( format.BytesPerPixel ) 
		+ " bytes per pixel." ) ;

	l.push_back( "Colorkey (pixel value of transparent pixels) is " 
		+ Ceylan::toNumericalString( format.colorkey ) ) + "." ) ;
		
	l.push_back( "Overall alpha is " 
		+ Ceylan::toNumericalString( format.alpha ) ) + "." ) ;
	
	if ( format.palette )
		l.push_back( "Palette available." ) ;
	else
		l.push_back( "No palette used." ) ;
		
	
	l.push_back( "Rmask is " + Ceylan::toString( format.Rmask, true ) + "." ) ;
	l.push_back( "Gmask is " + Ceylan::toString( format.Gmask, true ) + "." ) ;
	l.push_back( "Bmask is " + Ceylan::toString( format.Bmask, true ) + "." ) ;
	l.push_back( "Amask is " + Ceylan::toString( format.Amask, true ) + "." ) ;

	l.push_back( "Rshift is " 
		+ Ceylan::toNumericalString( format.Rshift ) ) + "." ) ;
		
	l.push_back( "Gshift is " 
		+ Ceylan::toNumericalString( format.Gshift ) ) + "." ) ;
		
	l.push_back( "Bshift is " 
		+ Ceylan::toNumericalString( format.Bshift ) ) + "." ) ;
		
	l.push_back( "Ashift is " 
		+ Ceylan::toNumericalString( format.Ashift ) ) + "." ) ;
		
	l.push_back( "Rloss is " 
		+ Ceylan::toNumericalString( format.Rloss ) ) + "." ) ;
		
	l.push_back( "Gloss is " 
		+ Ceylan::toNumericalString( format.Gloss ) ) + "." ) ;
		
	l.push_back( "Bloss is " 
		+ Ceylan::toNumericalString( format.Bloss ) ) + "." ) ;
		
	l.push_back( "Aloss is " 
		+ Ceylan::toNumericalString( format.Aloss ) ) + "." ) ;
	
		
	return result + Ceylan::formatStringList( l ) ;
	
}


string Pixels::toString( PixelColor pixel, const PixelFormat & format ) throw()
{

	return "Pixel whose color definition is " 
		+ toString( convertPixelColorToColorDefinition( format, pixel ) ) ; 
	
}
			
				
string Pixels::toString( ColorDefinition color ) throw() 
{
	
	string result = "[R;G;B;A] = [ " 
			+ Ceylan::toNumericalString( color.r ) + " ; "
			+ Ceylan::toNumericalString( color.g ) + " ; "
			+ Ceylan::toNumericalString( color.b ) + " ; "
			+ Ceylan::toNumericalString( color.unused ) + " ] " ;
			
	if ( Ceylan::TextDisplayable::GetOutputFormat() 
		== Ceylan::TextDisplayable::html )
	{
	
		string hexcolor = Ceylan::toHexString( color.r, 
				/* prefix */ false, /* minDigits */ 2 ) 
			+ Ceylan::toHexString( color.g ,
				/* prefix */ false, /* minDigits */ 2 ) 
			+ Ceylan::toHexString( color.b , 
				/* prefix */ false, /* minDigits */ 2 ) ;
		
		/*
		 * Some characters have to be displayed so that the HTML table 
		 * cell is drawn :
		 *
		 */
		result += "<table><tr><td style=\"background : #" 
			+ hexcolor + "; color : #" 
			+ hexcolor + "\">OSDL rocks !</td></tr></table>" ;

	}
	
	return result ;
	
}

