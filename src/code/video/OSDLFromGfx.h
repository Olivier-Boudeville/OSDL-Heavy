#ifndef OSDL_FROM_GFX_H_
#define OSDL_FROM_GFX_H_


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>             // for OSDL_VERBOSE_JOYSTICK_HANDLER and al
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL_GFX

/*
 * All these functions come from SDL_gfx (2.0.16): there were not
 * exported, whereas they are useful here, for example to manage
 * the endpoint drawing.
 *
 * @see http://www.ferzkopp.net/Software/SDL_gfx-2.0/
 *
 * Many thanks to Andreas Schiffler.
 *
 * This file is meant to be only included internally by OSDLFromGfx.cc
 * and the (internal too) OSDL implementation files calling these 
 * functions.
 *
 */
 
 
// for OSDL::Video::Pixels::PixelColor, OSDL::Video::Pixels::ColorElement:
#include "OSDLPixel.h"   
 

// These functions operate directly on SDL surfaces:
struct SDL_Surface ;


/*
 * In this file, all per pixel operations are made thanks to the 
 * atomic putpixel functions of SDL_gfx.
 *
 * @see http://www.ferzkopp.net/~aschiffler/Software/SDL_gfx-2.0/
 *
 * Sadly enough, they are not exported by their header file
 * (SDL_gfxPrimitives.h), so to use them one has to duplicate their code.
 *
 * Many thanks nevertheless to A. Schiffler for his work !
 *
 * (LGPL license too)
 *
 */


/* 
 * List of putpixel functions and of their dependencies ; 
 *
 * 1. No blending: 
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
 * 2. Blending:
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
#endif // __cplusplus



// Supplementary pixel-level primitives to declare (were internal):
 
int fastPixelColorNolock( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Uint32 color ) ;
	
int fastPixelColorNolockNoclip( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Uint32 color ) ;
	
int fastPixelColor( SDL_Surface * dst, Ceylan::Sint16 x, 
	Ceylan::Sint16 y, Ceylan::Uint32 color ) ;


// This one is not used:
int fastPixelRGBA( SDL_Surface * dst, Ceylan::Sint16 x, 
	Ceylan::Sint16 y, Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b,
	 Ceylan::Uint8 a ) ;
 
// This one is not used: 
int fastPixelRGBANolock( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b,
	Ceylan::Uint8 a ) ;


/**
 * OSDL's version of putPixelAlpha.
 *
 * @param color this color is not a color definition, it is a pixel color
 * already converted to the pixel format of the target surface.
 *
 * @param the alpha coefficient to use for this pixel to be blended.
 *
 * @note the alpha color element has to be specified since it cannot 
 * reliably be deduced from the 'color' parameter, which is already 
 * encoded to a specific pixel format.
 *
 */
int putPixelAlpha( SDL_Surface * surface, Ceylan::Sint16 x,
	Ceylan::Sint16 y, OSDL::Video::Pixels::PixelColor color,
	OSDL::Video::Pixels::ColorElement alpha ) ;
	
	
/* 
 * This one is already exported by SDL_gfx:
 * int pixelColor( SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y,
 * Ceylan::Uint32 color) ;
 *
 */
 
int pixelColorNolock( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Uint32 color ) ;

int aalineColorInt( SDL_Surface * dst, Ceylan::Sint16 x1,
	Ceylan::Sint16 y1, Ceylan::Sint16 x2, Ceylan::Sint16 y2, 
	Ceylan::Uint32 color, int draw_endpoint ) ;

int hlineColorStore( SDL_Surface * dst, Ceylan::Sint16 x1,
	Ceylan::Sint16 x2, Ceylan::Sint16 y, Ceylan::Uint32 color ) ;

int filledCircleRGBANotBlended( SDL_Surface * dst, 
	Ceylan::Sint16 x, Ceylan::Sint16 y, Ceylan::Sint16 rad, 
	Ceylan::Uint8 r, Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a ) ;


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // OSDL_USES_SDL_GFX


# endif // OSDL_FROM_GFX_H_

