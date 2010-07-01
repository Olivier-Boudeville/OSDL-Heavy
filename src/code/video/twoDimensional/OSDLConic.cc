/*
 * Copyright (C) 2003-2010 Olivier Boudeville
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


#include "OSDLConic.h"

#include "OSDLFromGfx.h"        // for filledCircleRGBANotBlended
#include "OSDLSurface.h"        // for Surface
#include "OSDLPoint2D.h"        // for Point2D
#include "OSDLVideo.h"          // for VideoModule

#include "Ceylan.h"             // for Ceylan::LogPlug



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>         // for OSDL_DEBUG_CONICS and al
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS


using namespace OSDL::Video ;
using namespace OSDL::Video::Pixels ;

using namespace Ceylan::Log ;




#if OSDL_USES_SDL_GFX

#include "SDL_gfxPrimitives.h"  // for graphic primitives exported by SDL_gfx

#endif // OSDL_USES_SDL_GFX





// A return code of 0 for SDL_gfx functions means no error, contrary to -1.



bool TwoDimensional::drawCircle( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter, Length radius,
	Pixels::ColorElement red, Pixels::ColorElement green,
	Pixels::ColorElement blue, Pixels::ColorElement alpha,
	bool filled, bool blended )
{

#if OSDL_USES_SDL_GFX

	/*
	 * To avoid code duplication, this RGBA version of drawCircle may
	 * call the color definition-based one.
	 * However back-end functions take RGBA arguments, so a useless double
	 * conversion is spared here.
	 *
	 */
#if OSDL_DEBUG_CONICS

	LogPlug::trace( "TwoDimensional::drawCircle (RGBA): center = ["
		+ Ceylan::toString( xCenter ) + ";" + Ceylan::toString( yCenter )
		+ "], radius = " + Ceylan::toString( radius )
		+ ", color: "   + Pixels::toString(
			Pixels::convertRGBAToColorDefinition( red, green, blue, alpha ) )
		+ ", filled = "  + Ceylan::toString( filled )
		+ ", blended = " + Ceylan::toString( blended ) ) ;

#endif // OSDL_DEBUG_CONICS

	if ( blended )
	{

		if ( filled )
		{

			// Disc:
			return ( ::filledCircleRGBA( & targetSurface.getSDLSurface(),
				xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;
		}
		else
		{

			// Circle:

			if ( VideoModule::GetAntiAliasingState() )
			{
				return ( ::aacircleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;

			}
			else
			{
				return ( ::circleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;

			}

		}

	}
	else
	{

		/*
		 * Here the disc should not be blended.
		 * As for circles, this is not implemented yet so they will still be
		 * blended:
		 *
		 */


		if ( filled )
		{

			// Disc:
			return ( ::filledCircleRGBANotBlended(
				& targetSurface.getSDLSurface(),
				xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;
		}
		else
		{


			// Circle:

			if ( VideoModule::GetAntiAliasingState() )
			{

				/* Not implemented yet:

				return ( ::aacircleRGBANotBlended(
					& targetSurface.getSDLSurface(),
					xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;

				 */

				return ( ::aacircleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;
			}
			else
			{

				/* Not implemented yet:

				return ( ::circleRGBANotBlended(
					& targetSurface.getSDLSurface(), xCenter, yCenter,
					radius, red, green, blue, alpha ) == 0 ) ;
				 */

				return ( ::circleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius, red, green, blue, alpha ) == 0 ) ;
			}

		}

	}

#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawCircle: "
		"no SDL_gfx support available" ) ;

#endif // OSDL_USES_SDL_GFX

}



bool TwoDimensional::drawCircle( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter,
	Length radius, Pixels::ColorDefinition colorDef, bool filled, bool blended )
{

#if OSDL_USES_SDL_GFX

#if OSDL_DEBUG_CONICS

	LogPlug::trace( "TwoDimensional::drawCircle (color definition): center = ["
		+ Ceylan::toString( xCenter ) + ";" + Ceylan::toString( yCenter )
		+ "], radius = " + Ceylan::toString( radius )
		+ ", color: "   + Pixels::toString( colorDef )
		+ ", filled = "  + Ceylan::toString( filled )
		+ ", blended = " + Ceylan::toString( blended ) ) ;

#endif // OSDL_DEBUG_CONICS


	if ( blended )
	{

		if ( filled )
		{

			// Disc:
			return ( ::filledCircleColor( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					Pixels::convertColorDefinitionToRawPixelColor( colorDef ) )
						== 0 ) ;
		}
		else
		{

			// Circle:

			if ( VideoModule::GetAntiAliasingState() )
			{

				return ( ::aacircleColor( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					Pixels::convertColorDefinitionToRawPixelColor( colorDef ) )
						== 0 ) ;

			}
			else
			{

				return ( ::circleColor( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					Pixels::convertColorDefinitionToRawPixelColor( colorDef ) )
						== 0 ) ;

			}

		}

	}
	else // Not blended here:
	{

		/*
		 * Here the disc should not be blended. As for circles, this is not
		 * implemented yet so they will still be blended:
		 *
		 */

		if ( filled )
		{

			// Disc:

#if OSDL_DEBUG_CONICS
			LogPlug::trace( "TwoDimensional::drawCircle: "
				"before filledCircleColorNotBlended, "
				+ targetSurface.describePixelAt( xCenter, yCenter ) ) ;

			bool result = ( ::filledCircleRGBANotBlended(
					& targetSurface.getSDLSurface(), xCenter, yCenter, radius,
				colorDef.r, colorDef.g, colorDef.b, colorDef.unused ) == 0 ) ;

			LogPlug::trace( "TwoDimensional::drawCircle: "
				"after filledCircleColorNotBlended, "
				+ targetSurface.describePixelAt( xCenter, yCenter ) ) ;

			return result ;

#endif // OSDL_DEBUG_CONICS

			return ( ::filledCircleRGBANotBlended(
				& targetSurface.getSDLSurface(), xCenter, yCenter, radius,
				colorDef.r, colorDef.g, colorDef.b, colorDef.unused ) == 0 ) ;

		}
		else
		{


			// Circle:

			if ( VideoModule::GetAntiAliasingState() )
			{

				/* Not implemented yet:

				return ( ::aacircleRGBANotBlended(
					& targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					colorDef.r, colorDef.g, colorDef.b, colorDef.unused )
						== 0 ) ;

				 */

				return ( ::aacircleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					colorDef.r, colorDef.g, colorDef.b, colorDef.unused )
						== 0 ) ;
			}
			else
			{

				/* Not implemented yet:

				return ( ::circleRGBANotBlended(
					& targetSurface.getSDLSurface(), xCenter, yCenter, radius,
					 colorDef.r, colorDef.g, colorDef.b, colorDef.unused )
					 	== 0 ) ;

				 */

				return ( ::circleRGBA( & targetSurface.getSDLSurface(),
					xCenter, yCenter, radius,
					colorDef.r, colorDef.g, colorDef.b, colorDef.unused )
						== 0 ) ;

			}

		}

	}

#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawCircle: "
		"no SDL_gfx support available" ) ;

#endif // OSDL_USES_SDL_GFX

}



bool TwoDimensional::drawDiscWithEdge( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter,
	Length outerRadius, Length innerRadius,
	Pixels::ColorDefinition ringColorDef,
	Pixels::ColorDefinition discColorDef, bool blended )
{

#if OSDL_USES_SDL_GFX

	if ( innerRadius >= outerRadius )
		return false ;


	/*
	 * Special case if inner color is not totally opaque <b>and</b>
	 * blending is requested:
	 *
	 */
	if ( discColorDef.unused != AlphaOpaque && blended )
	{


		/*
		 * Drawing directly the whole edged disc cannot be achieved unless
		 * writing a dedicated primitive.
		 * The work is done here in two steps, each of them drawing a
		 * basic disc, so that the final result is the expected edged disc.
		 *
		 */

		/*
		 * Previous solution was OSDL_MUST_RECTIFY_DISC_ALPHA:
		 *
		 * In this case, we have to create a temporary surface where both
		 * discs (first the outer one for the ring, then the inner one) are
		 * blitted: as the latter must replace the former instead of being
		 * blended with it, the inner disc is blitted as if it was totally
		 * opaque (so the ring color is not taken into account), then its
		 * alpha is corrected.
		 *
		 * Current solution is: do the same but draw the inner circle with
		 * a putPixel involving no blending to avoid having to fix alpha.
		 *
		 */


		/*
		 * Uncomment to switch back to former (more expensive) deprecated
		 * solution (useless with newer circle primitive):
		 *
		 */
		//#define OSDL_MUST_RECTIFY_DISC_ALPHA

#ifdef OSDL_MUST_RECTIFY_DISC_ALPHA

		// 'blended' not taken into account here (always considered as true).

		// First create the overall surface which will contain both discs:
		Surface * fullEdgedDiscSurface = new Surface(
			Surface::Software | Surface::AlphaBlendingBlit,
			/* width */ 2 * outerRadius, /* height */ 2 * outerRadius ) ;

		// From here 'fullEdgedDiscSurface' is a totally transparent surface.

		// Draw an opaque ring:
		ColorElement ringAlpha = ringColorDef.unused ;
		ringColorDef.unused = Pixels::AlphaOpaque ;

		// Draw the outer disc in it, for the ring:
		if ( ! fullEdgedDiscSurface->drawCircle( outerRadius,
			outerRadius, outerRadius, ringColorDef, /* filled */ true ) )
		{
			delete fullEdgedDiscSurface ;
			return false ;
		}

		// Draw the inner disc as if it were opaque:

		ColorElement discAlpha = discColorDef.unused ;
		discColorDef.unused = Pixels::AlphaOpaque ;

		fullEdgedDiscSurface->drawCircle( outerRadius,
			outerRadius, innerRadius, discColorDef, /* filled */ true ) ;

		/*
		 * Refreshes the alpha of the ring (ought to apply to a smaller
		 * area than the whole surface):
		 *
		 */
		fullEdgedDiscSurface->setAlphaForColor( ringColorDef, ringAlpha ) ;


		/*
		 * Refreshes the alpha of the disc (ought to apply to a smaller area
		 * than the whole surface):
		 *
		 */
		fullEdgedDiscSurface->setAlphaForColor( discColorDef, discAlpha ) ;

		LogPlug::debug( "fullEdgedDiscSurface: "
			+ fullEdgedDiscSurface->describePixelAt(
				outerRadius, outerRadius ) ) ;

		fullEdgedDiscSurface->blitTo( targetSurface,
			xCenter - outerRadius, yCenter - outerRadius ) ;

		delete fullEdgedDiscSurface ;


#else // OSDL_MUST_RECTIFY_DISC_ALPHA

		/*
		 * Not using previous trick, preferring introducting a new circle
		 * primitive ignoring blending so that the aimed alpha is already
		 * there, hence does not have to be fixed afterwards.
		 *
		 */

		// First, create the overall surface which will contain both discs:
		Surface * fullEdgedDiscSurface = new Surface(
			Surface::Software | Surface::AlphaBlendingBlit,
			/* width */ 2 * outerRadius, /* height */ 2 * outerRadius ) ;

		/*
		 * RGBA surfaces are created with all pixels set to [0;0;0;0]
		 * (totally transparent black), which is convenient since it
		 * allows that nothing of the outside of the ring will appear
		 * with the final blit (apparently color key and pixel-level
		 * alpha blending cannot be used together).
		 *
		 * Hence 'fullEdgedDiscSurface->fill( Pixels::Transparent ) ;'
		 * would be useless.
		 *
		 */

		/*
		 * Draws the outer ring (including the inside disc, which be
		 * replaced later):
		 *
		 */
		if ( ! fullEdgedDiscSurface->drawCircle(
			outerRadius, outerRadius, outerRadius,
			ringColorDef, /* filled */ true, /* blended */ false ) )
		{
			delete fullEdgedDiscSurface ;
			return false ;
		}

		// Draws inside the inner disc:

		if ( ! fullEdgedDiscSurface->drawCircle(
			outerRadius, outerRadius, innerRadius,
			discColorDef, /* filled */ true, /* blended */ false ) )
		{
			delete fullEdgedDiscSurface ;
			return false ;
		}

		fullEdgedDiscSurface->blitTo( targetSurface,
			xCenter - outerRadius, yCenter - outerRadius ) ;

		delete fullEdgedDiscSurface ;


#endif // OSDL_MUST_RECTIFY_DISC_ALPHA


		return true ;


	}


	/*
	 * Here the inner color is totally opaque and/or no blending is
	 * requested (the easy case!):
	 *
	 */

	// Draws the outer ring (and the inside which be replaced):
	if ( ! drawCircle( targetSurface, xCenter, yCenter,
		outerRadius, ringColorDef, /* filled */ true, blended ) )
	{
		return false ;
	}


	// Draws inside the inner disc:
	if ( ! drawCircle( targetSurface, xCenter, yCenter,
		innerRadius, discColorDef,	/* filled */ true, blended ) )
	{
		return false ;
	}

	return true ;

#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawDiscWithEdge: "
		"no SDL_gfx support available" ) ;

#endif // OSDL_USES_SDL_GFX

}



bool TwoDimensional::drawEllipse( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter,
	Length horizontalRadius, Length verticalRadius,
	Pixels::ColorElement red, Pixels::ColorElement green,
	Pixels::ColorElement blue, Pixels::ColorElement alpha, bool filled )
{

#if OSDL_USES_SDL_GFX

	if ( filled )
	{

		return ( ::filledEllipseRGBA( & targetSurface.getSDLSurface(),
			xCenter, yCenter, horizontalRadius, verticalRadius,
			red, green, blue, alpha ) == 0 ) ;
	}
	else
	{

		if ( VideoModule::GetAntiAliasingState() )
		{

			return ( ::aaellipseRGBA( & targetSurface.getSDLSurface(),
				xCenter, yCenter, horizontalRadius, verticalRadius,
				red, green, blue, alpha ) == 0 ) ;


		}
		else
		{

			return ( ::ellipseRGBA( & targetSurface.getSDLSurface(),
				xCenter, yCenter, horizontalRadius, verticalRadius,
				red, green, blue, alpha ) == 0 ) ;

		}

	}

#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawEllipse: "
		"no SDL_gfx support available" ) ;

#endif // OSDL_USES_SDL_GFX

}



bool TwoDimensional::drawEllipse( Surface & targetSurface,
	Coordinate xCenter, Coordinate yCenter,
	Length horizontalRadius, Length verticalRadius,
	Pixels::ColorDefinition colorDef, bool filled )
{

#if OSDL_USES_SDL_GFX

	if ( filled )
	{

		return ( ::filledEllipseColor( & targetSurface.getSDLSurface(),
			xCenter, yCenter, horizontalRadius, verticalRadius,
			Pixels::convertColorDefinitionToRawPixelColor( colorDef ) ) == 0 ) ;

	}
	else
	{

		if ( VideoModule::GetAntiAliasingState() )
		{

			return ( ::aaellipseColor( & targetSurface.getSDLSurface(),
				xCenter, yCenter, horizontalRadius, verticalRadius,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) )
					== 0 ) ;

		}
		else
		{

			return ( ::ellipseColor( & targetSurface.getSDLSurface(),
				xCenter, yCenter, horizontalRadius, verticalRadius,
				Pixels::convertColorDefinitionToRawPixelColor( colorDef ) )
					== 0 ) ;

		}

	}

#else // OSDL_USES_SDL_GFX

	throw VideoException( "TwoDimensional::drawEllipse: "
		"no SDL_gfx support available" ) ;

#endif // OSDL_USES_SDL_GFX

}
