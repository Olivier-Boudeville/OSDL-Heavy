#include "OSDLFromGfx.h"  

#include "Ceylan.h"    // for Ceylan::Sint16, Ceylan::Uint32, etc.

#include "SDL.h"       // for SDL_SUrface, etc.




using namespace Ceylan ;
using namespace Ceylan::Log ;

using namespace OSDL::Video ;


/*
 * - putPixelAlpha left 'almost as was in SDL_gfx', but exported now
 * - filledCircleRGBANotBlended, aacircleRGBANotBlended and 
 * circleRGBANotBlended adapted from their SDL_gfx original counterparts
 * (filledCircleRGBA, aacircleRGBA, circleRGBA) except that the
 * circle pixels are not blended with the ones of the target surface : 
 * they simply replace them.
 *
 */

/* ----- Defines for pixel clipping tests */

#define clip_xmin(surface) surface->clip_rect.x
#define clip_xmax(surface) surface->clip_rect.x+surface->clip_rect.w-1
#define clip_ymin(surface) surface->clip_rect.y
#define clip_ymax(surface) surface->clip_rect.y+surface->clip_rect.h-1


	

/* 
 * Taken litterally (verbatim) from SDL_gfx sources (SDL_gfxPrimitives.c) 
 * after this comment, from clipping macros to pixelColorNolock (included),
 * except that everything has been reformatted, and the function  
 * int pixelColor( SDL_Surface * dst, Ceylan::Ceylan::Sint16 x,
 * Ceylan::Ceylan::Sint16 y, Ceylan::Ceylan::Uint32 color ) 
 * which was already exported, and Sint16, Uint8, Uint16 and Uint32 which 
 * have been prefixed by 'Ceylan::' 
 *
 */


// Forward declaration.
int filledCircleColorNotBlended( SDL_Surface * dst, 
	Ceylan::Sint16 x, Ceylan::Sint16 y,
	Ceylan::Sint16 r, OSDL::Video::Pixels::ColorDefinition ) ;



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
int ::putPixelAlpha( SDL_Surface * surface, Ceylan::Sint16 x, Ceylan::Sint16 y, 
	OSDL::Video::Pixels::PixelColor color, 
	OSDL::Video::Pixels::ColorElement alpha )
{


#if OSDL_DEBUG_PIXEL

	if ( x % 20 == 0 && y % 20 == 0 )
	{
		LogPlug::trace( "putPixelAlpha (OSDLFromGfx.cc) : putting at [" 
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
	  	LogPlug::error( "putPixelAlpha : unexpected bpp (" 
			+ Ceylan::toString( surface->format->BytesPerPixel ) + ")" ) ;
	  	break ;
		
		
	}
				

    }
	else
	{
	
		// Clipped-out pixel :
#if OSDL_DEBUG_PIXEL
		LogPlug::trace( "putPixelAlpha (OSDLFromGfx.cc) : "
			"pixel clipped out since location [" 
			+ Ceylan::toString( x ) + ";" + Ceylan::toString( y ) 
			+ "] is out of surface bounds" ) ;
#endif // OSDL_DEBUG_PIXEL
		
	}	

    return 0 ;
	
}



int ::filledCircleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r, Ceylan::Uint8 g,
	Ceylan::Uint8 b, Ceylan::Uint8 a )
{

    /*
     * Draw :
     */
    return ::filledCircleColorNotBlended( dst, x, y, rad, 
		Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;
		
}




/*
 * Not implemented yet (mostly useless since only non blended discs 
 * interest us here) :
 *
 */


#ifdef OSDL_CIRCLE_NOT_BLENDED_IMPLEMENTED

int ::aacircleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r, Ceylan::Uint8 g,
	Ceylan::Uint8 b, Ceylan::Uint8 a )
{

    /*
     * Draw :
     */
    return ::aaellipseColorNotBlended( dst, x, y, rad, rad, 
		Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;
		
}



int ::circleRGBANotBlended( SDL_Surface * dst, Ceylan::Sint16 x, 
	Ceylan::Sint16 y, Ceylan::Sint16 rad, Ceylan::Uint8 r, 
	Ceylan::Uint8 g, Ceylan::Uint8 b, Ceylan::Uint8 a)
{

    /*
     * Draw :
     */
    return ::circleColor( dst, x, y, rad, 
		Pixels::convertRGBAToColorDefinition( r, g, b, a ) ) ;
	
}


#endif // OSDL_CIRCLE_NOT_BLENDED_IMPLEMENTED



int ::filledCircleColorNotBlended( SDL_Surface * dst, Ceylan::Sint16 x,
	Ceylan::Sint16 y, Ceylan::Sint16 r, Pixels::ColorDefinition color )
{

#if OSDL_DEBUG_PIXEL

	LogPlug::trace( "filledCircleColorNotBlended : "
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
	 * was : 'return (pixelColor(dst, x, y, color));' : locks, clips and blends
	 *
	 * Locks, clips and does not blend instead :
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
		 * 'hlineColor' :
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
}



/* ----- Pixel - fast, no blending, locking, clipping 
   Taken 'as was in SDL_gfx', since was not exported.	
   Used by fastPixelColor
*/

int fastPixelColor(SDL_Surface * dst, Ceylan::Sint16 x, 
	Ceylan::Sint16 y, Ceylan::Uint32 color)
{
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


