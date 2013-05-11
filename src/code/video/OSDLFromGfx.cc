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


#include "OSDLFromGfx.h"

#include "OSDLPixel.h"  // for convertRGBAToColorDefinition, etc.

#include "Ceylan.h"      // for Ceylan::Sint16, Ceylan::Uint32, etc.





#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>     // for OSDL_DEBUG_PIXEL and al
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS



#if OSDL_USES_SDL

#include "SDL.h"                     // for SDL_Surface, etc.

using namespace OSDL::Video ;

using namespace Ceylan ;
using namespace Ceylan::Log ;



/*
 * Taken litterally (verbatim) from SDL_gfx sources (SDL_gfxPrimitives.c)
 * after this comment, from clipping macros to pixelColorNolock (included),
 * except that everything has been reformatted, and the function
 * int pixelColor( SDL_Surface * dst, Ceylan::Ceylan::Sint16 x,
 * Ceylan::Ceylan::Sint16 y, Ceylan::Uint32 color )
 * which was already exported, and Ceylan::Sint16, Ceylan::Uint8,
 * Ceylan::Uint16 and Ceylan::Uint32 which have been prefixed by 'Ceylan::'
 *
 */



// Forward declarations.

int filledCircleColorNotBlended( SDL_Surface * dst,
  Ceylan::Sint16 x, Ceylan::Sint16 y,
  Ceylan::Sint16 r, OSDL::Video::Pixels::ColorDefinition ) ;


static int clipLine(SDL_Surface * dst, Ceylan::Sint16 * x1, Ceylan::Sint16 * y1,
  Ceylan::Sint16 * x2, Ceylan::Sint16 * y2) ;



int VLineAlpha(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y1,
  Ceylan::Sint16 y2, Ceylan::Uint32 color) ;


int filledRectAlpha(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color) ;


int lineColor(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color) ;

int hlineColor(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 x2,
  Ceylan::Sint16 y, Ceylan::Uint32 color) ;


int vlineColor(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y1,
  Ceylan::Sint16 y2, Ceylan::Uint32 color) ;


int pixelColorWeightNolock(SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Uint32 color, Ceylan::Uint32 weight) ;

int _putPixelAlpha(SDL_Surface * surface, Ceylan::Sint16 x, Ceylan::Sint16 y,
  Ceylan::Uint32 color, Ceylan::Uint8 alpha) ;



/*
 * - putPixelAlpha left 'almost as was in SDL_gfx', but exported now
 * - filledCircleRGBANotBlended, aacircleRGBANotBlended and
 * circleRGBANotBlended adapted from their SDL_gfx original counterparts
 * (filledCircleRGBA, aacircleRGBA, circleRGBA) except that the
 * circle pixels are not blended with the ones of the target surface:
 * they simply replace them.
 *
 */

/* ----- Defines for pixel clipping tests */

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1

/* --------- Clipping routines for line */

/* Clipping based heavily on code from                       */
/* http://www.ncsa.uiuc.edu/Vis/Graphics/src/clipCohSuth.c   */

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

static int clipEncode(Ceylan::Sint16 x, Ceylan::Sint16 y, Ceylan::Sint16 left,
  Ceylan::Sint16 top, Ceylan::Sint16 right, Ceylan::Sint16 bottom)
{
  int code = 0;

  if (x < left) {
	code |= CLIP_LEFT_EDGE;
  } else if (x > right) {
	code |= CLIP_RIGHT_EDGE;
  }
  if (y < top) {
	code |= CLIP_TOP_EDGE;
  } else if (y > bottom) {
	code |= CLIP_BOTTOM_EDGE;
  }
  return code;
}




/**
 * OSDL's version of putPixelAlpha.
 *
 * @param color this color is not a color definition, it is a pixel color
 * already converted to the pixel format of the target surface.
 *
 * @param the full precision alpha coordinate of the pixel to put.
 *
 * The greater the 'alpha' parameter is, the closer to the alpha of put
 * pixel the alpha of the final pixel will be (for a null 'alpha',
 * final pixel alpha will not be modified, for a maximum 'alpha' the alpha
 * of the resulting pixel will be the alpha of put pixel.
 *
 * @note the alpha color element has to be specified, since it cannot
 * reliably be deduced from the 'color' parameter, which is already
 * encoded to a specific pixel format.
 *
 */
int putPixelAlpha( SDL_Surface * surface, Ceylan::Sint16 x, Ceylan::Sint16 y,
  OSDL::Video::Pixels::PixelColor color,
  OSDL::Video::Pixels::ColorElement alpha )
{


#if OSDL_DEBUG_PIXEL

  if ( x % 20 == 0 && y % 20 == 0 )
  {
	LogPlug::trace( "putPixelAlpha (OSDLFromGfx.cc): putting at ["
	  + Ceylan::toString( x )	+ ";" + Ceylan::toString( y )
	  + "] pixel color " + Pixels::toString(
		Pixels::convertPixelColorToColorDefinition(
		  * surface->format, color ) )
	  + " with alpha coordinate = "
	  + Ceylan::toNumericalString( alpha ) ) ;
  }

#endif // OSDL_DEBUG_PIXEL

  Ceylan::Uint32 Rmask = surface->format->Rmask ;
  Ceylan::Uint32 Gmask = surface->format->Gmask ;
  Ceylan::Uint32 Bmask = surface->format->Bmask ;
  Ceylan::Uint32 Amask = surface->format->Amask ;

  Ceylan::Uint32 R, G, B, A = 0 ;


  if ( x >= clip_xmin(surface) && x <= clip_xmax(surface)
	&& y >= clip_ymin(surface) && y <= clip_ymax(surface) )
  {

	switch( surface->format->BytesPerPixel )
	{

	case 1:
	  {

		/* Assuming 8-bpp */
		if (alpha == 255)
		{
		  *((Ceylan::Uint8 *) surface->pixels + y * surface->pitch + x)
			= color ;
		}
		else
		{
		  Ceylan::Uint8 *pixel =
			(Ceylan::Uint8 *) surface->pixels + y * surface->pitch + x ;

		  Ceylan::Uint8 dR = surface->format->palette->colors[*pixel].r;
		  Ceylan::Uint8 dG = surface->format->palette->colors[*pixel].g;
		  Ceylan::Uint8 dB = surface->format->palette->colors[*pixel].b;
		  Ceylan::Uint8 sR = surface->format->palette->colors[color].r;
		  Ceylan::Uint8 sG = surface->format->palette->colors[color].g;
		  Ceylan::Uint8 sB = surface->format->palette->colors[color].b;

		  dR = dR + ((sR - dR) * alpha >> 8);
		  dG = dG + ((sG - dG) * alpha >> 8);
		  dB = dB + ((sB - dB) * alpha >> 8);

		  *pixel = SDL_MapRGB(surface->format, dR, dG, dB);
		}
	  }
	  break ;

	case 2:
	  {

		/* Probably 15-bpp or 16-bpp */
		if (alpha == 255)
		{

		  *((Ceylan::Uint16 *) surface->pixels + y * surface->pitch / 2
			+ x) = color;

		}
		else
		{

		  Ceylan::Uint16 *pixel = (Ceylan::Uint16 *) surface->pixels
			+ y * surface->pitch / 2 + x;
		  Ceylan::Uint32 dc = *pixel;

		  R = ((dc & Rmask)
			+ (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;

		  G = ((dc & Gmask)
			+ (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;
		  B = ((dc & Bmask)
			+ (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;

		  if (Amask)
			A = ((dc & Amask) + (((color & Amask) - (dc & Amask))
				* alpha >> 8)) & Amask;

		  *pixel = R | G | B | A;

		}
	  }
	  break;

	case 3:
	  {
		/* Slow 24-bpp mode, usually not used */
		Ceylan::Uint8 *pix = (Ceylan::Uint8 *) surface->pixels
		  + y * surface->pitch + x * 3;
		Ceylan::Uint8 rshift8 = surface->format->Rshift / 8;
		Ceylan::Uint8 gshift8 = surface->format->Gshift / 8;
		Ceylan::Uint8 bshift8 = surface->format->Bshift / 8;
		Ceylan::Uint8 ashift8 = surface->format->Ashift / 8;


		if (alpha == 255)
		{
		  *(pix + rshift8) = color >> surface->format->Rshift;
		  *(pix + gshift8) = color >> surface->format->Gshift;
		  *(pix + bshift8) = color >> surface->format->Bshift;
		  *(pix + ashift8) = color >> surface->format->Ashift;
		}
		else
		{
		  Ceylan::Uint8 dR, dG, dB, dA = 0;
		  Ceylan::Uint8 sR, sG, sB, sA = 0;

		  pix = (Ceylan::Uint8 *) surface->pixels
			+ y * surface->pitch + x * 3;

		  dR = *((pix) + rshift8);
		  dG = *((pix) + gshift8);
		  dB = *((pix) + bshift8);
		  dA = *((pix) + ashift8);

		  sR = (color >> surface->format->Rshift) & 0xff;
		  sG = (color >> surface->format->Gshift) & 0xff;
		  sB = (color >> surface->format->Bshift) & 0xff;
		  sA = (color >> surface->format->Ashift) & 0xff;

		  dR = dR + ((sR - dR) * alpha >> 8);
		  dG = dG + ((sG - dG) * alpha >> 8);
		  dB = dB + ((sB - dB) * alpha >> 8);
		  dA = dA + ((sA - dA) * alpha >> 8);

		  *((pix) + rshift8) = dR;
		  *((pix) + gshift8) = dG;
		  *((pix) + bshift8) = dB;
		  *((pix) + ashift8) = dA;
		}
	  }
	  break;


	case 4:
	  {
		/* Probably 32-bpp */
		if (alpha == 255)
		{
		  *((Ceylan::Uint32 *) surface->pixels
			+ y * surface->pitch / 4 + x) = color;
		}
		else
		{

		  Ceylan::Uint32 *pixel = (Ceylan::Uint32 *) surface->pixels
			+ y * surface->pitch / 4 + x;

		  Ceylan::Uint32 dc = *pixel;

		  R = ((dc & Rmask)
			+ (((color & Rmask) - (dc & Rmask)) * alpha >> 8)) & Rmask;

		  G = ((dc & Gmask)
			+ (((color & Gmask) - (dc & Gmask)) * alpha >> 8)) & Gmask;

		  B = ((dc & Bmask)
			+ (((color & Bmask) - (dc & Bmask)) * alpha >> 8)) & Bmask;

		  if (Amask)
			A = ((dc & Amask) + (((color & Amask) - (dc & Amask))
				* alpha >> 8)) & Amask;

		  *pixel = R | G | B | A;

		}
	  }
	  break;

	default:
	  LogPlug::error( "putPixelAlpha: unexpected bpp ("
		+ Ceylan::toString( surface->format->BytesPerPixel ) + ")" ) ;
	  break ;


	}


  }
  else
  {

	// Clipped-out pixel:
#if OSDL_DEBUG_PIXEL
	LogPlug::trace( "putPixelAlpha (OSDLFromGfx.cc): "
	  "pixel clipped out since location ["
	  + Ceylan::toString( x ) + ";" + Ceylan::toString( y )
	  + "] is out of surface bounds" ) ;
#endif // OSDL_DEBUG_PIXEL

  }

  return 0 ;

}



int filledCircleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r, Ceylan::Uint8 g,
  Ceylan::Uint8 b, Ceylan::Uint8 a )
{

  /*
   * Draw:
   */
  return::filledCircleColorNotBlended( dst, x, y, rad,
	Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;

}


int pixelColorNolock(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y,
  Ceylan::Uint32 color)
{
  Ceylan::Uint8 alpha;
  Ceylan::Uint32 mcolor;
  int result = 0;

  /*
   * Setup color
   */
  alpha = color & 0x000000ff;
  mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
	  (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

  /*
   * Draw
   */
  result = _putPixelAlpha(dst, x, y, mcolor, alpha);

  return (result);
}



/*
 * Not implemented yet (mostly useless since only non blended discs
 * interest us here):
 *
 */


#ifdef OSDL_CIRCLE_NOT_BLENDED_IMPLEMENTED

int aacircleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r, Ceylan::Uint8 g,
  Ceylan::Uint8 b, Ceylan::Uint8 a )
{

  /*
   * Draw:
   */
  return::aaellipseColorNotBlended( dst, x, y, rad, rad,
	Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;

}



int::circleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r,
  Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a)
{

  /*
   * Draw:
   */
  return::circleColor( dst, x, y, rad,
	Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;

}


#endif // OSDL_CIRCLE_NOT_BLENDED_IMPLEMENTED



int filledCircleColorNotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Sint16 r, Pixels::ColorDefinition color )
{

#if OSDL_USES_SDL_GFX

#if OSDL_DEBUG_PIXEL

  LogPlug::trace( "filledCircleColorNotBlended: "
	"drawing a non-blended disc at ["
	+ Ceylan::toString( x ) + ";" + Ceylan::toString( y )
	+ "], with radius = " + Ceylan::toString( r )
	+ "	and color definition " + Pixels::toString( color ) ) ;

#endif // OSDL_DEBUG_PIXEL

  Ceylan::Sint16 left, right, top, bottom;
  int result;
  Ceylan::Sint16 x1, y1, x2, y2;
  Ceylan::Sint16 cx = 0;
  Ceylan::Sint16 cy = r;
  Ceylan::Sint16 ocx = ( Ceylan::Sint16 ) 0xffff;
  Ceylan::Sint16 ocy = ( Ceylan::Sint16 ) 0xffff;
  Ceylan::Sint16 df = 1 - r;
  Ceylan::Sint16 d_e = 3;
  Ceylan::Sint16 d_se = -2 * r + 5;
  Ceylan::Sint16 xpcx, xmcx, xpcy, xmcy;
  Ceylan::Sint16 ypcy, ymcy, ypcx, ymcx;

  Pixels::PixelColor convertedColor = SDL_MapRGBA( dst->format,
	color.r, color.g, color.b, color.unused ) ;

  /*
   * Sanity check radius
   */
  if (r < 0) {
	return (-1);
  }

  /*
   * Special case for r=0 - draw a point
   */
  if (r == 0) {

	/*
	 * was: 'return (pixelColor(dst, x, y, color));': locks, clips and blends
	 *
	 * Locks, clips and does not blend instead:
	 *
	 */
	fastPixelColor( dst, x, y, convertedColor ) ;
  }

  /*
   * Get clipping boundary
   */
  left   = dst->clip_rect.x;
  right  = dst->clip_rect.x + dst->clip_rect.w - 1;
  top    = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  /*
   * Test if bounding box of circle is visible
   */
  x1 = x - r;
  x2 = x + r;
  y1 = y - r;
  y2 = y + r;
  if ((x1<left) && (x2<left)) {
	return(0);
  }
  if ((x1>right) && (x2>right)) {
	return(0);
  }
  if ((y1<top) && (y2<top)) {
	return(0);
  }
  if ((y1>bottom) && (y2>bottom)) {
	return(0);
  }

  /*
   * Draw
   */
  result = 0;
  do {
	xpcx = x + cx;
	xmcx = x - cx;
	xpcy = x + cy;
	xmcy = x - cy;
	if (ocy != cy) {
	  if (cy > 0) {
		ypcy = y + cy;
		ymcy = y - cy;

		/*
		 * hlineColorNotBlended is already available and called
		 * 'hlineColorStore', it replaces here
		 * 'hlineColor':
		 */

		result |= hlineColorStore(dst, xmcx, xpcx, ypcy, convertedColor);
		result |= hlineColorStore(dst, xmcx, xpcx, ymcy, convertedColor);
	  } else {
		result |= hlineColorStore(dst, xmcx, xpcx, y, convertedColor);
	  }
	  ocy = cy;
	}
	if (ocx != cx) {
	  if (cx != cy) {
		if (cx > 0) {
		  ypcx = y + cx;
		  ymcx = y - cx;
		  result |= hlineColorStore(dst, xmcy, xpcy, ymcx, convertedColor);
		  result |= hlineColorStore(dst, xmcy, xpcy, ypcx, convertedColor);
		} else {
		  result |= hlineColorStore(dst, xmcy, xpcy, y, convertedColor);
		}
	  }
	  ocx = cx;
	}
	/*
	 * Update
	 */
	if (df < 0) {
	  df += d_e;
	  d_e += 2;
	  d_se += 2;
	} else {
	  df += d_se;
	  d_e += 2;
	  d_se += 4;
	  cy--;
	}
	cx++;
  } while (cx <= cy);

  return (result);
}



/*
 * Just store color including alpha, no blending.
 *
 * Taken 'as was in SDL_gfx' (except basic numerical types prefixed by
 * 'Ceylan::'), since was not exported.
 *
 * Used by filledCircleColorNotBlended.
 *
 * Could be named as well 'hlineColorNotBlended'.
 */

int hlineColorStore( SDL_Surface * dst, Ceylan::Sint16 x1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y, Pixels::PixelColor color )
{
  Ceylan::Sint16 left, right, top, bottom;
  Ceylan::Uint8 *pixel, *pixellast;
  int dx;
  int pixx, pixy;
  Ceylan::Sint16 w;
  Ceylan::Sint16 xtmp;
  int result = -1;

  /*
   * Get clipping boundary
   */
  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  /*
   * Check visibility of hline
   */
  if ((x1<left) && (x2<left)) {
	return(0);
  }
  if ((x1>right) && (x2>right)) {
	return(0);
  }
  if ((y<top) || (y>bottom)) {
	return (0);
  }

  /*
   * Clip x
   */
  if (x1 < left) {
	x1 = left;
  }
  if (x2 > right) {
	x2 = right;
  }

  /*
   * Swap x1, x2 if required
   */
  if (x1 > x2) {
	xtmp = x1;
	x1 = x2;
	x2 = xtmp;
  }

  /*
   * Calculate width
   */
  w = x2 - x1;

  /*
   * Sanity check on width
   */
  if (w < 0) {
	return (0);
  }

  /*
   * Lock surface
   */
  SDL_LockSurface(dst);

  /*
   * More variable setup
   */
  dx = w;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Ceylan::Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

  /*
   * Draw
   */
  switch (dst->format->BytesPerPixel) {
  case 1:
	memset(pixel, color, dx);
	break;
  case 2:
	pixellast = pixel + dx + dx;
	for (; pixel <= pixellast; pixel += pixx) {
	  *( Ceylan::Uint16 * ) pixel = color;
	}
	break;
  case 3:
	pixellast = pixel + dx + dx + dx;
	for (; pixel <= pixellast; pixel += pixx) {
	  if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		pixel[0] = (color >> 16) & 0xff;
		pixel[1] = (color >> 8) & 0xff;
		pixel[2] = color & 0xff;
	  } else {
		pixel[0] = color & 0xff;
		pixel[1] = (color >> 8) & 0xff;
		pixel[2] = (color >> 16) & 0xff;
	  }
	}
	break;
  default:		/* case 4 */
	dx = dx + dx;
	pixellast = pixel + dx + dx;
	for (; pixel <= pixellast; pixel += pixx)
	{
	  *( Ceylan::Uint32 * ) pixel = color;
	}
	break;
  }

  /*
   * Unlock surface
   */
  SDL_UnlockSurface(dst);

  /*
   * Set result code
   */
  result = 0;

  return (result);

#else // OSDL_USES_SDL_GFX

  throw VideoException( "OSDLFromGfx: filledCircleColorNotBlended "
	"not available: no SDL_gfx support available." ) ;

#endif // OSDL_USES_SDL_GFX

}



/* ----- Pixel - fast, no blending, locking, clipping
   Taken 'as was in SDL_gfx', since was not exported.
   Used by fastPixelColor
*/

int fastPixelColor(SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Uint32 color)
{

#if OSDL_USES_SDL_GFX

  int result;

  /*
   * Lock the surface
   */
  if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	  return (-1);
	}
  }

  result = fastPixelColorNolock(dst, x, y, color);

  /*
   * Unlock surface
   */
  if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
  }

  return (result);

#else // OSDL_USES_SDL_GFX

  throw VideoException( "OSDLFromGfx: fastPixelColor "
	"not available: no SDL_gfx support available." ) ;

#endif // OSDL_USES_SDL_GFX

}



/* ----- Pixel - fast, no blending, no locking, clipping
 * Taken 'as was in SDL_gfx', since was not exported.
 *
 * Used by filledCircleColorNotBlended.
 *
 */

int fastPixelColorNolock(SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Pixels::PixelColor color)
{

  int bpp;
  Ceylan::Uint8 *p;

  /*
   * Honor clipping setup at pixel level
   */
  if ((x >= clip_xmin(dst)) && (x <= clip_xmax(dst)) && (y >= clip_ymin(dst))
	&& (y <= clip_ymax(dst))) {

	/*
	 * Get destination format
	 */
	bpp = dst->format->BytesPerPixel;
	p = (Ceylan::Uint8 *) dst->pixels + y * dst->pitch + x * bpp;
	switch (bpp) {
	case 1:
	  *p = color;
	  break;
	case 2:
	  *( Ceylan::Uint16 * ) p = color;
	  break;
	case 3:
	  if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		p[0] = (color >> 16) & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = color & 0xff;
	  } else {
		p[0] = color & 0xff;
		p[1] = (color >> 8) & 0xff;
		p[2] = (color >> 16) & 0xff;
	  }
	  break;
	case 4:
	  *( Ceylan::Uint32 * ) p = color;
	  break;
	}			/* switch */


  }

  return (0);

}

/* ----- Pixel - fast, no blending, no locking, no clipping */

/* (faster but dangerous, make sure we stay in surface bounds) */

int fastPixelColorNolockNoclip(SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Uint32 color)
{
  int bpp;
  Ceylan::Uint8 *p;

  /*
   * Get destination format
   */
  bpp = dst->format->BytesPerPixel;
  p = (Ceylan::Uint8 *) dst->pixels + y * dst->pitch + x * bpp;
  switch (bpp) {
  case 1:
	*p = color;
	break;
  case 2:
	*(Ceylan::Uint16 *) p = color;
	break;
  case 3:
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	  p[0] = (color >> 16) & 0xff;
	  p[1] = (color >> 8) & 0xff;
	  p[2] = color & 0xff;
	} else {
	  p[0] = color & 0xff;
	  p[1] = (color >> 8) & 0xff;
	  p[2] = (color >> 16) & 0xff;
	}
	break;
  case 4:
	*(Ceylan::Uint32 *) p = color;
	break;
  }				/* switch */

  return (0);
}


/* AA Line */

#define AAlevels 256
#define AAbits 8

/*

  This implementation of the Wu antialiasing code is based on Mike Abrash's
  DDJ article which was reprinted as Chapter 42 of his Graphics Programming
  Black Book, but has been optimized to work with SDL and utilizes 32-bit
  fixed-point arithmetic. (A. Schiffler).

*/

int aalineColorInt(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color, int draw_endpoint)
{
  Ceylan::Sint32 xx0, yy0, xx1, yy1;
  int result;
  Ceylan::Uint32 intshift, erracc, erradj;
  Ceylan::Uint32 erracctmp, wgt;
  int dx, dy, tmp, xdir, y0p1, x0pxdir;

  /*
   * Clip line and test if we have to draw
   */
  if (!(clipLine(dst, &x1, &y1, &x2, &y2))) {
	return (0);
  }

  /*
   * Keep on working with 32bit numbers
   */
  xx0 = x1;
  yy0 = y1;
  xx1 = x2;
  yy1 = y2;

  /*
   * Reorder points if required
   */
  if (yy0 > yy1) {
	tmp = yy0;
	yy0 = yy1;
	yy1 = tmp;
	tmp = xx0;
	xx0 = xx1;
	xx1 = tmp;
  }

  /*
   * Calculate distance
   */
  dx = xx1 - xx0;
  dy = yy1 - yy0;

  /*
   * Adjust for negative dx and set xdir
   */
  if (dx >= 0) {
	xdir = 1;
  } else {
	xdir = -1;
	dx = (-dx);
  }

  /*
   * Check for special cases
   */
  if (dx == 0) {
	/*
	 * Vertical line
	 */
	return (vlineColor(dst, x1, y1, y2, color));
  } else if (dy == 0) {
	/*
	 * Horizontal line
	 */
	return (hlineColor(dst, x1, x2, y1, color));
  } else if (dx == dy) {
	/*
	 * Diagonal line
	 */
	return (lineColor(dst, x1, y1, x2, y2, color));
  }

  /*
   * Line is not horizontal, vertical or diagonal
   */
  result = 0;

  /*
   * Zero accumulator
   */
  erracc = 0;

  /*
   * # of bits by which to shift erracc to get intensity level
   */
  intshift = 32 - AAbits;

  /* Lock surface */
  if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	  return (-1);
	}
  }

  /*
   * Draw the initial pixel in the foreground color
   */
  result |= pixelColorNolock(dst, x1, y1, color);

  /*
   * x-major or y-major?
   */
  if (dy > dx) {

	/*
	 * y-major.  Calculate 16-bit fixed point fractional part of a pixel that
	 * X advances every time Y advances 1 pixel, truncating the result so that
	 * we won't overrun the endpoint along the X axis
	 */
	/*
	 * Not-so-portable version: erradj =
	 * ((Ceylan::Uint64)dx << 32) / (Ceylan::Uint64)dy;
	 */
	erradj = ((dx << 16) / dy) << 16;

	/*
	 * draw all pixels other than the first and last
	 */
	x0pxdir = xx0 + xdir;
	while (--dy) {
	  erracctmp = erracc;
	  erracc += erradj;
	  if (erracc <= erracctmp) {
		/*
		 * rollover in error accumulator, x coord advances
		 */
		xx0 = x0pxdir;
		x0pxdir += xdir;
	  }
	  yy0++;		/* y-major so always advance Y */

	  /*
	   * the AAbits most significant bits of erracc give us the intensity
	   * weighting for this pixel, and the complement of the weighting for
	   * the paired pixel.
	   */
	  wgt = (erracc >> intshift) & 255;
	  result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
	  result |= pixelColorWeightNolock (dst, x0pxdir, yy0, color, wgt);
	}

  }
  else
  {

	/*
	 * x-major line.  Calculate 16-bit fixed-point fractional part of a pixel
	 * that Y advances each time X advances 1 pixel, truncating the result so
	 * that we won't overrun the endpoint along the X axis.
	 */
	/*
	 * Not-so-portable version: erradj =
	 * ((Ceylan::Uint64)dy << 32) / (Ceylan::Uint64)dx;
	 */
	erradj = ((dy << 16) / dx) << 16;

	/*
	 * draw all pixels other than the first and last
	 */
	y0p1 = yy0 + 1;
	while (--dx) {

	  erracctmp = erracc;
	  erracc += erradj;
	  if (erracc <= erracctmp) {
		/*
		 * Accumulator turned over, advance y
		 */
		yy0 = y0p1;
		y0p1++;
	  }
	  xx0 += xdir;	/* x-major so always advance X */
	  /*
	   * the AAbits most significant bits of erracc give us the intensity
	   * weighting for this pixel, and the complement of the weighting for
	   * the paired pixel.
	   */
	  wgt = (erracc >> intshift) & 255;
	  result |= pixelColorWeightNolock (dst, xx0, yy0, color, 255 - wgt);
	  result |= pixelColorWeightNolock (dst, xx0, y0p1, color, wgt);
	}
  }

  /*
   * Do we have to draw the endpoint
   */
  if (draw_endpoint) {
	/*
	 * Draw final pixel, always exactly intersected by the line and doesn't
	 * need to be weighted.
	 */
	result |= pixelColorNolock (dst, x2, y2, color);
  }

  /* Unlock surface */
  if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
  }

  return (result);
}



/* ----- Pixel - pixel draw with blending enabled if a<255 */

int pixelColor(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y,
  Ceylan::Uint32 color)
{
  Ceylan::Uint8 alpha;
  Ceylan::Uint32 mcolor;
  int result = 0;

  /*
   * Lock the surface
   */
  if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	  return (-1);
	}
  }

  /*
   * Setup color
   */
  alpha = color & 0x000000ff;
  mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
	  (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

  /*
   * Draw
   */
  result = _putPixelAlpha(dst, x, y, mcolor, alpha);

  /*
   * Unlock the surface
   */
  if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
  }

  return (result);
}


/* Pixel - using alpha weight on color for AA-drawing - no locking */

int pixelColorWeightNolock(SDL_Surface * dst, Ceylan::Sint16 x,
  Ceylan::Sint16 y, Ceylan::Uint32 color, Ceylan::Uint32 weight)
{
  Ceylan::Uint32 a;

  /*
   * Get alpha
   */
  a = (color & (Ceylan::Uint32) 0x000000ff);

  /*
   * Modify Alpha by weight
   */
  a = ((a * weight) >> 8);

  return (pixelColorNolock(dst, x, y,
	  (color & (Ceylan::Uint32) 0xffffff00) | (Ceylan::Uint32) a));
}

static int clipLine(SDL_Surface * dst, Ceylan::Sint16 * x1,
  Ceylan::Sint16 * y1, Ceylan::Sint16 * x2, Ceylan::Sint16 * y2)
{
  Ceylan::Sint16 left, right, top, bottom;
  int code1, code2;
  int draw = 0;
  Ceylan::Sint16 swaptmp;
  float m;

  /*
   * Get clipping boundary
   */
  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  while (1) {
	code1 = clipEncode(*x1, *y1, left, top, right, bottom);
	code2 = clipEncode(*x2, *y2, left, top, right, bottom);
	if (CLIP_ACCEPT(code1, code2)) {
	  draw = 1;
	  break;
	} else if (CLIP_REJECT(code1, code2))
	  break;
	else {
	  if (CLIP_INSIDE(code1)) {
		swaptmp = *x2;
		*x2 = *x1;
		*x1 = swaptmp;
		swaptmp = *y2;
		*y2 = *y1;
		*y1 = swaptmp;
		swaptmp = code2;
		code2 = code1;
		code1 = swaptmp;
	  }
	  if (*x2 != *x1) {
		m = (*y2 - *y1) / (float) (*x2 - *x1);
	  } else {
		m = 1.0f;
	  }
	  if (code1 & CLIP_LEFT_EDGE) {
		*y1 += (Ceylan::Sint16) ((left - *x1) * m);
		*x1 = left;
	  } else if (code1 & CLIP_RIGHT_EDGE) {
		*y1 += (Ceylan::Sint16) ((right - *x1) * m);
		*x1 = right;
	  } else if (code1 & CLIP_BOTTOM_EDGE) {
		if (*x2 != *x1) {
		  *x1 += (Ceylan::Sint16) ((bottom - *y1) / m);
		}
		*y1 = bottom;
	  } else if (code1 & CLIP_TOP_EDGE) {
		if (*x2 != *x1) {
		  *x1 += (Ceylan::Sint16) ((top - *y1) / m);
		}
		*y1 = top;
	  }
	}
  }

  return draw;
}


/* ----- Vertical line */

int vlineColor(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y1,
  Ceylan::Sint16 y2, Ceylan::Uint32 color)
{
  Ceylan::Sint16 left, right, top, bottom;
  Ceylan::Uint8 *pixel, *pixellast;
  int dy;
  int pixx, pixy;
  Ceylan::Sint16 h;
  Ceylan::Sint16 ytmp;
  int result = -1;
  Ceylan::Uint8 *colorptr;

  /*
   * Get clipping boundary
   */
  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  /*
   * Check visibility of vline
   */
  if ((x<left) || (x>right)) {
	return (0);
  }
  if ((y1<top) && (y2<top)) {
	return(0);
  }
  if ((y1>bottom) && (y2>bottom)) {
	return(0);
  }

  /*
   * Clip y
   */
  if (y1 < top) {
	y1 = top;
  }
  if (y2 > bottom) {
	y2 = bottom;
  }

  /*
   * Swap y1, y2 if required
   */
  if (y1 > y2) {
	ytmp = y1;
	y1 = y2;
	y2 = ytmp;
  }

  /*
   * Calculate height
   */
  h = y2 - y1;

  /*
   * Sanity check on height
   */
  if (h < 0) {
	return (0);
  }

  /*
   * Alpha check
   */
  if ((color & 255) == 255) {

	/*
	 * No alpha-blending required
	 */

	/*
	 * Setup color
	 */
	colorptr = (Ceylan::Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
	  color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1],
		colorptr[2], colorptr[3]);
	}
	else
	{
	  color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2],
		colorptr[1], colorptr[0]);
	}

	/*
	 * Lock surface
	 */
	SDL_LockSurface(dst);

	/*
	 * More variable setup
	 */
	dy = h;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Ceylan::Uint8 *) dst->pixels) + pixx * (int) x + pixy * (int) y1;
	pixellast = pixel + pixy * dy;

	/*
	 * Draw
	 */
	switch (dst->format->BytesPerPixel) {
	case 1:
	  for (; pixel <= pixellast; pixel += pixy) {
		*(Ceylan::Uint8 *) pixel = color;
	  }
	  break;
	case 2:
	  for (; pixel <= pixellast; pixel += pixy) {
		*(Ceylan::Uint16 *) pixel = color;
	  }
	  break;
	case 3:
	  for (; pixel <= pixellast; pixel += pixy) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		  pixel[0] = (color >> 16) & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = color & 0xff;
		} else {
		  pixel[0] = color & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = (color >> 16) & 0xff;
		}
	  }
	  break;
	default:		/* case 4 */
	  for (; pixel <= pixellast; pixel += pixy) {
		*(Ceylan::Uint32 *) pixel = color;
	  }
	  break;
	}

	/*
	 * Unlock surface
	 */
	SDL_UnlockSurface(dst);

	/*
	 * Set result code
	 */
	result = 0;

  } else {

	/*
	 * Alpha blending blit
	 */

	result = VLineAlpha(dst, x, y1, y1 + h, color);

  }

  return (result);
}


/* Draw vertical line with alpha enabled from RGBA color */

int VLineAlpha(SDL_Surface * dst, Ceylan::Sint16 x, Ceylan::Sint16 y1,
  Ceylan::Sint16 y2, Ceylan::Uint32 color)
{
  return (filledRectAlpha(dst, x, y1, x, y2, color));
}



/* Filled rectangle with alpha blending, color in destination format */

int _filledRectAlpha(SDL_Surface * surface, Ceylan::Sint16 x1,
  Ceylan::Sint16 y1, Ceylan::Sint16 x2, Ceylan::Sint16 y2,
  Ceylan::Uint32 color, Ceylan::Uint8 alpha)
{
  Ceylan::Uint32 Rmask = surface->format->Rmask, Gmask =
	surface->format->Gmask, Bmask = surface->format->Bmask,
	Amask = surface->format->Amask;
  Ceylan::Uint32 R, G, B, A = 0;
  Ceylan::Sint16 x, y;

  switch (surface->format->BytesPerPixel) {
  case 1:{			/* Assuming 8-bpp */
	Ceylan::Uint8 *row, *pixel;
	Ceylan::Uint8 dR, dG, dB;

	Ceylan::Uint8 sR = surface->format->palette->colors[color].r;
	Ceylan::Uint8 sG = surface->format->palette->colors[color].g;
	Ceylan::Uint8 sB = surface->format->palette->colors[color].b;

	for (y = y1; y <= y2; y++) {
	  row = (Ceylan::Uint8 *) surface->pixels + y * surface->pitch;
	  for (x = x1; x <= x2; x++) {
		pixel = row + x;

		dR = surface->format->palette->colors[*pixel].r;
		dG = surface->format->palette->colors[*pixel].g;
		dB = surface->format->palette->colors[*pixel].b;

		dR = dR + ((sR - dR) * alpha >> 8);
		dG = dG + ((sG - dG) * alpha >> 8);
		dB = dB + ((sB - dB) * alpha >> 8);

		*pixel = SDL_MapRGB(surface->format, dR, dG, dB);
	  }
	}
  }
	break;

  case 2:{			/* Probably 15-bpp or 16-bpp */
	Ceylan::Uint16 *row, *pixel;
	Ceylan::Uint32 dR = (color & Rmask), dG = (color & Gmask),
	  dB = (color & Bmask), dA = (color & Amask);

	for (y = y1; y <= y2; y++) {
	  row = (Ceylan::Uint16 *) surface->pixels + y * surface->pitch / 2;
	  for (x = x1; x <= x2; x++) {
		pixel = row + x;

		R = ((*pixel & Rmask) + ((dR - (*pixel & Rmask)) * alpha >> 8)) & Rmask;
		G = ((*pixel & Gmask) + ((dG - (*pixel & Gmask)) * alpha >> 8)) & Gmask;
		B = ((*pixel & Bmask) + ((dB - (*pixel & Bmask)) * alpha >> 8)) & Bmask;
		if (Amask)
		  A = ((*pixel & Amask) + ((dA - (*pixel & Amask)) * alpha >> 8)) & Amask;

		*pixel = R | G | B | A;
	  }
	}
  }
	break;

  case 3:{			/* Slow 24-bpp mode, usually not used */
	Ceylan::Uint8 *row, *pix;
	Ceylan::Uint8 dR, dG, dB, dA;
	Ceylan::Uint8 rshift8 = surface->format->Rshift / 8;
	Ceylan::Uint8 gshift8 = surface->format->Gshift / 8;
	Ceylan::Uint8 bshift8 = surface->format->Bshift / 8;
	Ceylan::Uint8 ashift8 = surface->format->Ashift / 8;

	Ceylan::Uint8 sR = (color >> surface->format->Rshift) & 0xff;
	Ceylan::Uint8 sG = (color >> surface->format->Gshift) & 0xff;
	Ceylan::Uint8 sB = (color >> surface->format->Bshift) & 0xff;
	Ceylan::Uint8 sA = (color >> surface->format->Ashift) & 0xff;

	for (y = y1; y <= y2; y++) {
	  row = (Ceylan::Uint8 *) surface->pixels + y * surface->pitch;
	  for (x = x1; x <= x2; x++) {
		pix = row + x * 3;

		dR = *((pix) + rshift8);
		dG = *((pix) + gshift8);
		dB = *((pix) + bshift8);
		dA = *((pix) + ashift8);

		dR = dR + ((sR - dR) * alpha >> 8);
		dG = dG + ((sG - dG) * alpha >> 8);
		dB = dB + ((sB - dB) * alpha >> 8);
		dA = dA + ((sA - dA) * alpha >> 8);

		*((pix) + rshift8) = dR;
		*((pix) + gshift8) = dG;
		*((pix) + bshift8) = dB;
		*((pix) + ashift8) = dA;
	  }
	}

  }
	break;

  case 4:{			/* Probably 32-bpp */
	Ceylan::Uint32 Rshift, Gshift, Bshift, Ashift;
	Ceylan::Uint32 *row, *pixel;
	Ceylan::Uint32 dR = (color & Rmask), dG = (color & Gmask), dB = (color & Bmask), dA = (color & Amask);

	Rshift = surface->format->Rshift;
	Gshift = surface->format->Gshift;
	Bshift = surface->format->Bshift;
	Ashift = surface->format->Ashift;

	for (y = y1; y <= y2; y++) {
	  row = (Ceylan::Uint32 *) surface->pixels + y * surface->pitch / 4;
	  for (x = x1; x <= x2; x++) {
		pixel = row + x;

		R = ((*pixel & Rmask)
		  + ((((dR - (*pixel & Rmask)) >> Rshift) * alpha >> 8) << Rshift)) & Rmask;
		G = ((*pixel & Gmask)
		  + ((((dG - (*pixel & Gmask)) >> Gshift) * alpha >> 8) << Gshift)) & Gmask;
		B = ((*pixel & Bmask)
		  + ((((dB - (*pixel & Bmask)) >> Bshift) * alpha >> 8) << Bshift)) & Bmask;
		if (Amask)
		  A = ((*pixel & Amask)
			+ ((((dA - (*pixel & Amask)) >> Ashift) * alpha >> 8) << Ashift)) & Amask;

		*pixel = R | G | B | A;
	  }
	}
  }
	break;
  }

  return (0);
}


/* Draw horizontal line with alpha enabled from RGBA color */

int HLineAlpha(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 x2,
  Ceylan::Sint16 y, Ceylan::Uint32 color)
{
  return (filledRectAlpha(dst, x1, y, x2, y, color));
}


/* Draw rectangle with alpha enabled from RGBA color. */

int filledRectAlpha(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color)
{
  Ceylan::Uint8 alpha;
  Ceylan::Uint32 mcolor;
  int result = 0;

  /*
   * Lock the surface
   */
  if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	  return (-1);
	}
  }

  /*
   * Setup color
   */
  alpha = color & 0x000000ff;
  mcolor =
	SDL_MapRGBA(dst->format, (color & 0xff000000) >> 24,
	  (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, alpha);

  /*
   * Draw
   */
  result = _filledRectAlpha(dst, x1, y1, x2, y2, mcolor, alpha);

  /*
   * Unlock the surface
   */
  if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
  }

  return (result);
}


int hlineColor(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 x2,
  Ceylan::Sint16 y, Ceylan::Uint32 color)
{
  Ceylan::Sint16 left, right, top, bottom;
  Ceylan::Uint8 *pixel, *pixellast;
  int dx;
  int pixx, pixy;
  Ceylan::Sint16 w;
  Ceylan::Sint16 xtmp;
  int result = -1;
  Ceylan::Uint8 *colorptr;

  /*
   * Get clipping boundary
   */
  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  /*
   * Check visibility of hline
   */
  if ((x1<left) && (x2<left)) {
	return(0);
  }
  if ((x1>right) && (x2>right)) {
	return(0);
  }
  if ((y<top) || (y>bottom)) {
	return (0);
  }

  /*
   * Clip x
   */
  if (x1 < left) {
	x1 = left;
  }
  if (x2 > right) {
	x2 = right;
  }

  /*
   * Swap x1, x2 if required
   */
  if (x1 > x2) {
	xtmp = x1;
	x1 = x2;
	x2 = xtmp;
  }

  /*
   * Calculate width
   */
  w = x2 - x1;

  /*
   * Sanity check on width
   */
  if (w < 0) {
	return (0);
  }

  /*
   * Alpha check
   */
  if ((color & 255) == 255) {

	/*
	 * No alpha-blending required
	 */

	/*
	 * Setup color
	 */
	colorptr = (Ceylan::Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	  color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1],
		colorptr[2], colorptr[3]);
	}
	else
	{
	  color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2],
		colorptr[1], colorptr[0]);
	}

	/*
	 * Lock surface
	 */
	SDL_LockSurface(dst);

	/*
	 * More variable setup
	 */
	dx = w;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Ceylan::Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y;

	/*
	 * Draw
	 */
	switch (dst->format->BytesPerPixel) {
	case 1:
	  memset(pixel, color, dx);
	  break;
	case 2:
	  pixellast = pixel + dx + dx;
	  for (; pixel <= pixellast; pixel += pixx) {
		*(Ceylan::Uint16 *) pixel = color;
	  }
	  break;
	case 3:
	  pixellast = pixel + dx + dx + dx;
	  for (; pixel <= pixellast; pixel += pixx) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		  pixel[0] = (color >> 16) & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = color & 0xff;
		} else {
		  pixel[0] = color & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = (color >> 16) & 0xff;
		}
	  }
	  break;
	default:		/* case 4 */
	  dx = dx + dx;
	  pixellast = pixel + dx + dx;
	  for (; pixel <= pixellast; pixel += pixx) {
		*(Ceylan::Uint32 *) pixel = color;
	  }
	  break;
	}

	/*
	 * Unlock surface
	 */
	SDL_UnlockSurface(dst);

	/*
	 * Set result code
	 */
	result = 0;

  } else {

	/*
	 * Alpha blending blit
	 */

	result = HLineAlpha(dst, x1, x1 + w, y, color);

  }

  return (result);
}




/* ----- Line */

/* Non-alpha line drawing code adapted from routine          */
/* by Pete Shinners, pete@shinners.org                       */
/* Originally from pygame, http://pygame.seul.org            */

#define ABS(a) (((a)<0) ? -(a): (a))

int lineColor(SDL_Surface * dst, Ceylan::Sint16 x1, Ceylan::Sint16 y1,
  Ceylan::Sint16 x2, Ceylan::Sint16 y2, Ceylan::Uint32 color)
{
  int pixx, pixy;
  int x, y;
  int dx, dy;
  int ax, ay;
  int sx, sy;
  int swaptmp;
  Ceylan::Uint8 *pixel;
  Ceylan::Uint8 *colorptr;

  /*
   * Clip line and test if we have to draw
   */
  if (!(clipLine(dst, &x1, &y1, &x2, &y2))) {
	return (0);
  }

  /*
   * Test for special cases of straight lines or single point
   */
  if (x1 == x2) {
	if (y1 < y2) {
	  return (vlineColor(dst, x1, y1, y2, color));
	} else if (y1 > y2) {
	  return (vlineColor(dst, x1, y2, y1, color));
	} else {
	  return (pixelColor(dst, x1, y1, color));
	}
  }
  if (y1 == y2) {
	if (x1 < x2) {
	  return (hlineColor(dst, x1, x2, y1, color));
	} else if (x1 > x2) {
	  return (hlineColor(dst, x2, x1, y1, color));
	}
  }

  /*
   * Variable setup
   */
  dx = x2 - x1;
  dy = y2 - y1;
  sx = (dx >= 0) ? 1 : -1;
  sy = (dy >= 0) ? 1 : -1;

  /* Lock surface */
  if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	  return (-1);
	}
  }

  /*
   * Check for alpha blending
   */
  if ((color & 255) == 255) {

	/*
	 * No alpha blending - use fast pixel routines
	 */

	/*
	 * Setup color
	 */
	colorptr = (Ceylan::Uint8 *) & color;
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	  color = SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2],
		colorptr[3]);
	}
	else
	{
	  color = SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1],
		colorptr[0]);
	}

	/*
	 * More variable setup
	 */
	dx = sx * dx + 1;
	dy = sy * dy + 1;
	pixx = dst->format->BytesPerPixel;
	pixy = dst->pitch;
	pixel = ((Ceylan::Uint8 *) dst->pixels) + pixx * (int) x1 + pixy * (int) y1;
	pixx *= sx;
	pixy *= sy;
	if (dx < dy) {
	  swaptmp = dx;
	  dx = dy;
	  dy = swaptmp;
	  swaptmp = pixx;
	  pixx = pixy;
	  pixy = swaptmp;
	}

	/*
	 * Draw
	 */
	x = 0;
	y = 0;
	switch (dst->format->BytesPerPixel) {
	case 1:
	  for (; x < dx; x++, pixel += pixx) {
		*pixel = color;
		y += dy;
		if (y >= dx) {
		  y -= dx;
		  pixel += pixy;
		}
	  }
	  break;
	case 2:
	  for (; x < dx; x++, pixel += pixx) {
		*(Ceylan::Uint16 *) pixel = color;
		y += dy;
		if (y >= dx) {
		  y -= dx;
		  pixel += pixy;
		}
	  }
	  break;
	case 3:
	  for (; x < dx; x++, pixel += pixx) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		  pixel[0] = (color >> 16) & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = color & 0xff;
		} else {
		  pixel[0] = color & 0xff;
		  pixel[1] = (color >> 8) & 0xff;
		  pixel[2] = (color >> 16) & 0xff;
		}
		y += dy;
		if (y >= dx) {
		  y -= dx;
		  pixel += pixy;
		}
	  }
	  break;
	default:		/* case 4 */
	  for (; x < dx; x++, pixel += pixx) {
		*(Ceylan::Uint32 *) pixel = color;
		y += dy;
		if (y >= dx) {
		  y -= dx;
		  pixel += pixy;
		}
	  }
	  break;
	}

  } else {

	/*
	 * Alpha blending required - use single-pixel blits
	 */

	ax = ABS(dx) << 1;
	ay = ABS(dy) << 1;
	x = x1;
	y = y1;
	if (ax > ay) {
	  int d = ay - (ax >> 1);

	  while (x != x2) {
		pixelColorNolock (dst, x, y, color);
		if (d > 0 || (d == 0 && sx == 1)) {
		  y += sy;
		  d -= ax;
		}
		x += sx;
		d += ay;
	  }
	} else {
	  int d = ax - (ay >> 1);

	  while (y != y2) {
		pixelColorNolock (dst, x, y, color);
		if (d > 0 || ((d == 0) && (sy == 1))) {
		  x += sx;
		  d -= ay;
		}
		y += sy;
		d += ax;
	  }
	}
	pixelColorNolock (dst, x, y, color);

  }

  /* Unlock surface */
  if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
  }

  return (0);
}



/* PutPixel routine with alpha blending, input color in destination format */

/* New, faster routine - default blending pixel */

int _putPixelAlpha(SDL_Surface * surface, Ceylan::Sint16 x, Ceylan::Sint16 y,
  Ceylan::Uint32 color, Ceylan::Uint8 alpha)
{
  Ceylan::Uint32 Rmask = surface->format->Rmask,
	Gmask =
	surface->format->Gmask,
	Bmask = surface->format->Bmask,
	Amask = surface->format->Amask;
  
  Ceylan::Uint32 R, G, B, A = 0;

  if (x >= clip_xmin(surface) && x <= clip_xmax(surface)
	&& y >= clip_ymin(surface) && y <= clip_ymax(surface)) {

	switch (surface->format->BytesPerPixel) {
	case 1:{		/* Assuming 8-bpp */
	  if (alpha == 255) {
		*((Ceylan::Uint8 *) surface->pixels + y * surface->pitch + x) = color;
	  } else {
		Ceylan::Uint8 *pixel = (Ceylan::Uint8 *) surface->pixels + y * surface->pitch + x;

		Ceylan::Uint8 dR = surface->format->palette->colors[*pixel].r;
		Ceylan::Uint8 dG = surface->format->palette->colors[*pixel].g;
		Ceylan::Uint8 dB = surface->format->palette->colors[*pixel].b;
		Ceylan::Uint8 sR = surface->format->palette->colors[color].r;
		Ceylan::Uint8 sG = surface->format->palette->colors[color].g;
		Ceylan::Uint8 sB = surface->format->palette->colors[color].b;

		dR = dR + ((sR - dR) * alpha >> 8);
		dG = dG + ((sG - dG) * alpha >> 8);
		dB = dB + ((sB - dB) * alpha >> 8);

		*pixel = SDL_MapRGB(surface->format, dR, dG, dB);
	  }
	}
	  break;

	case 2:{		/* Probably 15-bpp or 16-bpp */
	  if (alpha == 255)
	  {
		*((Ceylan::Uint16 *) surface->pixels + y * surface->pitch / 2 + x) =
		  color;
	  } else {
		Ceylan::Uint16 *pixel = (Ceylan::Uint16 *) surface->pixels
		  + y * surface->pitch / 2 + x;
		Ceylan::Uint32 dc = *pixel;

		R = ((dc & Rmask) + (((color & Rmask) - (dc & Rmask)) * alpha >> 8))
		  & Rmask;
		G = ((dc & Gmask) + (((color & Gmask) - (dc & Gmask)) * alpha >> 8))
		  & Gmask;
		B = ((dc & Bmask) + (((color & Bmask) - (dc & Bmask)) * alpha >> 8))
		  & Bmask;
		if (Amask)
		  A = ((dc & Amask) + (((color & Amask) - (dc & Amask)) * alpha >> 8))
			& Amask;

		*pixel = R | G | B | A;
	  }
	}
	  break;

	case 3:{		/* Slow 24-bpp mode, usually not used */
	  Ceylan::Uint8 *pix = (Ceylan::Uint8 *) surface->pixels
		+ y * surface->pitch + x * 3;
	  Ceylan::Uint8 rshift8 = surface->format->Rshift / 8;
	  Ceylan::Uint8 gshift8 = surface->format->Gshift / 8;
	  Ceylan::Uint8 bshift8 = surface->format->Bshift / 8;
	  Ceylan::Uint8 ashift8 = surface->format->Ashift / 8;


	  if (alpha == 255) {
		*(pix + rshift8) = color >> surface->format->Rshift;
		*(pix + gshift8) = color >> surface->format->Gshift;
		*(pix + bshift8) = color >> surface->format->Bshift;
		*(pix + ashift8) = color >> surface->format->Ashift;
	  } else {
		Ceylan::Uint8 dR, dG, dB, dA = 0;
		Ceylan::Uint8 sR, sG, sB, sA = 0;

		pix = (Ceylan::Uint8 *) surface->pixels + y * surface->pitch + x * 3;

		dR = *((pix) + rshift8);
		dG = *((pix) + gshift8);
		dB = *((pix) + bshift8);
		dA = *((pix) + ashift8);

		sR = (color >> surface->format->Rshift) & 0xff;
		sG = (color >> surface->format->Gshift) & 0xff;
		sB = (color >> surface->format->Bshift) & 0xff;
		sA = (color >> surface->format->Ashift) & 0xff;

		dR = dR + ((sR - dR) * alpha >> 8);
		dG = dG + ((sG - dG) * alpha >> 8);
		dB = dB + ((sB - dB) * alpha >> 8);
		dA = dA + ((sA - dA) * alpha >> 8);

		*((pix) + rshift8) = dR;
		*((pix) + gshift8) = dG;
		*((pix) + bshift8) = dB;
		*((pix) + ashift8) = dA;
	  }
	}
	  break;

	case 4:{		/* Probably 32-bpp */
	  if (alpha == 255) {
		*((Ceylan::Uint32 *) surface->pixels + y * surface->pitch / 4 + x) = color;
	  } else {
		Ceylan::Uint32 Rshift, Gshift, Bshift, Ashift;
		Ceylan::Uint32 *pixel = (Ceylan::Uint32 *) surface->pixels
		  + y * surface->pitch / 4 + x;
		Ceylan::Uint32 dc = *pixel;

		Rshift = surface->format->Rshift;
		Gshift = surface->format->Gshift;
		Bshift = surface->format->Bshift;
		Ashift = surface->format->Ashift;

		R = ((dc & Rmask) + (((((color & Rmask) - (dc & Rmask)) >> Rshift) * alpha >> 8) << Rshift)) & Rmask;
		G = ((dc & Gmask) + (((((color & Gmask) - (dc & Gmask)) >> Gshift) * alpha >> 8) << Gshift)) & Gmask;
		B = ((dc & Bmask) + (((((color & Bmask) - (dc & Bmask)) >> Bshift) * alpha >> 8) << Bshift)) & Bmask;
		if (Amask)
		  A = ((dc & Amask) + (((((color & Amask) - (dc & Amask)) >> Ashift) * alpha >> 8) << Ashift)) & Amask;

		*pixel = R | G | B | A;
	  }
	}
	  break;
	}
  }

  return (0);
}



#endif // OSDL_USES_SDL
