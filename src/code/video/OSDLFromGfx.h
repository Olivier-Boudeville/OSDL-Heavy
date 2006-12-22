#ifndef OSDL_FROM_GFX_H_
#define OSDL_FROM_GFX_H_


/*
 * This file is meant to be only included internally by OSDLFromGfx.cc.
 *
 * @note The DLLINTERFACE may not be enough for Windows, since SDL_gfx will be compiled "as is",
 * thus the following internal functions will not be exported. One clever fix for Windows would be,
 * if these functions are indeed not accessible, to copy their code litteraly in OSDLFromGfx.cc
 * (only in Windows case).
 *
 */
 
#include "SDL.h"         // for Ceylan::Sint16, etc.

#include "OSDLPixel.h"   // for OSDL::Video::Pixels::PixelColor, OSDL::Video::Pixels::ColorElement


struct SDL_Surface ;

/*
 * If this file, all per pixel operations are made thanks to the atomic putpixel functions
 * of SDL_gfx (http://www.ferzkopp.net/~aschiffler/Software/SDL_gfx-2.0/).
 *
 * Sadly enough, they are not exported by their header file (SDL_gfxPrimitives.h), so to use 
 * them one has to duplicate their code.
 *
 * Many thanks nevertheless to A. Schiffler for his work !
 *
 * (LGPL license too)
 *
 */


/* 
 * List of putpixel functions and of their dependencies ; 
 *
 * 1. No blending : 
 *
 *   fastPixelColorNolockNoclip (never used)
 *
 *   fastPixelColorNolock (no depends) [clips]
 *
 *   fastPixelColor = lock + fastPixelColorNolock 
 *   fastPixelRGBANolock = map + fastPixelColorNolock
 *
 *   fastPixelRGBA = map + fastPixelColor = map + lock + fastPixelColorNolock
 *
 *
 * 2. Blending :
 *
 *   _putPixelAlpha (no depends) [clips]
 *
 *   pixelColorNolock = map + _putPixelAlpha
 *   pixelColor = lock + map + _putPixelAlpha
 *
 *   pixelRGBA = ( map + fastPixelColor ) | pixelColor
 *
 *
 */
 
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


/* ----- W32 DLL interface */

#ifdef WIN32
#ifdef BUILD_DLL
#define DLLINTERFACE __declspec(dllexport)
#else
#define DLLINTERFACE __declspec(dllimport)
#endif
#else
#define DLLINTERFACE
#endif


// Supplementary pixel-level primitives to declare (were internal) :
 
DLLINTERFACE int fastPixelColorNolock(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, 
	Ceylan::Uint32 color) ;
	
DLLINTERFACE int fastPixelColorNolockNoclip(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, 
	Ceylan::Uint32 color) ;
	
DLLINTERFACE int fastPixelColor(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, Ceylan::Uint32 color) ;


// This one is not used :
DLLINTERFACE int fastPixelRGBA(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, 
 Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a) ;
 
// This one is not used : 
DLLINTERFACE int fastPixelRGBANolock(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, 
	Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a) ;


/**
 * OSDL's version of putPixelAlpha.
 *
 * @param color this color is not a color definition, it is a pixel color already converted 
 * to the pixel format of the target surface.
 *
 * @param the alpha coefficient to use for this pixel to be blended.
 *
 * @note the alpha color element has to be specified since it cannot reliably be deduced from
 * the 'color' parameter, which is already encoded to a specific pixel format.
 *
 */
DLLINTERFACE int putPixelAlpha(SDL_Surface * surface, Ceylan::Sint16 x, Ceylan::Sint16 y, 
	OSDL::Video::Pixels::PixelColor color, OSDL::Video::Pixels::ColorElement alpha ) ;
	
	
/* 
 * This one is already exported by SDL_gfx :
 * int pixelColor(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, Ceylan::Uint32 color) ;
 *
 */
 
DLLINTERFACE int pixelColorNolock(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y, Ceylan::Uint32 color) ;

DLLINTERFACE int aalineColorInt(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1, Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color, int draw_endpoint) ;

DLLINTERFACE int hlineColorStore(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 x2, Ceylan::Sint16 y, Ceylan::Uint32 color) ;

DLLINTERFACE int filledCircleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y,
	Ceylan::Sint16 rad, Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a ) ;


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
};
#endif

# endif // OSDL_FROM_GFX_H_
