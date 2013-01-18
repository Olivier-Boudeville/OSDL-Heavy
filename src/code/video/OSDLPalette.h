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


#ifndef OSDL_PALETTE_H_
#define OSDL_PALETTE_H_


#include "OSDLPixel.h"       // for ColorDefinition, GammaFactor

#include "Ceylan.h"          // for TextDisplayable, Uint32, etc.




#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL

// No need to include SDL header here:
struct SDL_Palette ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL




#include <string>




namespace OSDL
{



	namespace Video
	{



		/**
		 * Number of colors.
		 *
		 * @note Change palette constructor from file and the save method is the
		 * size of this type changes.
		 *
		 */
		typedef Ceylan::Uint16 ColorCount ;



		/**
		 * Number of colors or color index, in 256-color palettes.
		 *
		 */
		typedef Ceylan::Uint8 ColorIndex ;



		/// Distance between colors.
		typedef Ceylan::SignedLongInteger ColorDistance ;



		/**
		 * Allows to designate a palette.
		 *
		 * Identifier 1 is reserved for the main master palette, as defined in
		 * master-palette.pal on the Nintendo DS.
		 *
		 */
		typedef Ceylan::Uint16 PaletteIdentifier ;




#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL

		/// SDL low-level palette being used.
		typedef ::SDL_Palette LowLevelPalette ;

#else // OSDL_USES_SDL

		/// Low-level palette being used.
		struct LowLevelPalette
		{

			// Number of color defined.
			ColorCount ncolors ;

			// Array definitions.
			Pixels::ColorDefinition *colors ;

		} ;

#endif // OSDL_USES_SDL





		/// Exception raised when a palette operation failed.
		class OSDL_DLL PaletteException : public VideoException
		{

			public:
				explicit PaletteException( const std::string & message ) ;

				virtual ~PaletteException() throw() ;

		} ;




		/**
		 * Describes a palette, for example for color-indexed modes.
		 *
		 * In these modes, the color of a pixel is defined by an index which
		 * points to a specific entry of the palette.
		 *
		 * Palettes can be used too for special effects, even in true color
		 * modes.
		 *
		 * A palette is set thanks to color definitions, which describe a color
		 * 'as is', without taking into account a particular screen format.
		 *
		 * To be used on a particular surface, color definitions need to be
		 * converted into PixelColor (actual colors), which are defined
		 * according to the surface pixel format.
		 *
		 * Therefore a Palette actually uses two palettes, a logical one (for
		 * color definitions) and a physical one (for pixel colors).
		 *
		 * A palette has getNumberOfColors() colors, ranging from 0 to
		 * getNumberOfColors()-1.
		 *
		 * A color index can be set as being the colorkey.
		 *
		 */
		class OSDL_DLL Palette : public Ceylan::TextDisplayable
		{

			public:



				/// The flag used to designate logical palette.
				static const Ceylan::Flags Logical ;


				/// The flag used to designate physical palette.
				static const Ceylan::Flags Physical ;



				/**
				 * Constructs a palette from a specified array of color
				 * definitions.
				 *
				 * No colorkey is set.
				 *
				 * @param numberOfColors the number of color for this palette.
				 *
				 * @param colors pointer to an array of numberOfColors color
				 * definitions. This palette takes ownership of the array. If a
				 * null pointer is specified, then the palette will create its
				 * own array of numberOfColors and set all colors to full black,
				 * i.e. (0,0,0,255) in the RGBA colorspace.
				 *
				 * @param format if specified (non-null), pixel colors are
				 * computed thanks to it, otherwise they are still to be
				 * converted.
				 *
				 * @note Takes ownership of the array of color definitions.
				 *
				 */
				explicit Palette( ColorCount numberOfColors,
					Pixels::ColorDefinition * colors = 0,
					Pixels::PixelFormat * format = 0 ) ;



				/**
				 * Creates a new palette from specified file, whose format is
				 * expected to be the .osdl.palette, which implies an OSDL
				 * palette header followed by the palette itself, with 24 bits
				 * per color (8 bits per color component), in RGB order.
				 *
				 * A colorkey index may be specified in the file.
				 *
				 * @param paletteFilename the name of the file containing the
				 * palette in '.osdl.palette' format.
				 *
				 * @see the Palette::save for the corresponding '.osdl.palette'
				 * file format.
				 *
				 * @note No alpha information is stored on file, and loaded
				 * colors will all be set to fully opaque (AlphaOpaque).
				 *
				 * @throw PaletteException if the operation failed.
				 *
				 */
				explicit Palette( const std::string & paletteFilename ) ;



				/**
				 * Constructs a palette from an already existing SDL palette.
				 *
				 * No colorkey is set.
				 *
				 * @note Does not take ownership of the LowLevelPalette's color
				 * buffer.
				 *
				 */
				explicit Palette( const LowLevelPalette & palette ) ;



				/// Basic virtual destructor.
				virtual ~Palette() throw() ;



				/**
				 * Loads new color definitions from memory .
				 *
				 * @param colors the color array, which should have been
				 * allocated like: 'Pixels::ColorDefinition * colors = new
				 * Pixels::ColorDefinition[ numberOfColors ] ;'
				 *
				 * @note If a palette was already registered, it is deallocated
				 * first.
				 *
				 * @note This palette takes ownership of the specified array of
				 * colors, and will deallocate them when necessary.
				 *
				 */
				virtual void load( ColorCount numberOfColors,
					Pixels::ColorDefinition * colors ) ;



				/**
				 * Returns the number of colors defined in the palette
				 * (including any colorkey).
				 *
				 */
				virtual ColorCount getNumberOfColors() const ;



				/**
				 * Returns the pixel color of the palette which is located at
				 * specified index.
				 *
				 * @note The pixel color is returned as a 'const' reference.
				 *
				 * @throw PaletteException if index is out of bounds (superior
				 * or equal to getNumberOfColors).
				 *
				 */
				virtual const Pixels::PixelColor & getPixelColorAt(
					ColorCount index ) const ;



				/**
				 * Returns the palette's pointer to pixel colors.
				 *
				 * @note This method should generally not be used, and is
				 * "pseudo-const".
				 *
				 */
				virtual Pixels::PixelColor * getPixelColors() const ;



				/**
				 * Returns the color definition of the palette which is located
				 * at specified index.
				 *
				 * @note The color definition is returned as a 'const'
				 * reference.
				 *
				 * @throw PaletteException if index is out of bounds (superior
				 * or equal to getNumberOfColors).
				 *
				 */
				virtual const Pixels::ColorDefinition & getColorDefinitionAt(
					ColorCount index ) const ;



				/**
				 * Sets the specified color definition at specified index of the
				 * palette (overwrites prior definition).
				 *
				 * @param targetIndex the index of the color definition to
				 * update.
				 *
				 * @param newColorDefinition the color definition to be set.
				 *
				 * @throw PaletteException if targetIndex is out of bounds
				 * (superior or equal to getNumberOfColors).
				 *
				 */
				virtual void setColorDefinitionAt(
					ColorCount targetIndex,
					const Pixels::ColorDefinition & newColorDefinition ) ;



				/**
				 * Sets the specified color definition at specified index of the
				 * palette (overwrites prior definition).
				 *
				 * @param targetIndex the index of the color definition to
				 * update.
				 *
				 * @throw PaletteException if targetIndex is out of bounds
				 * (superior or equal to getNumberOfColors).
				 *
				 */
				virtual void setColorDefinitionAt(
					ColorCount targetIndex,
					Pixels::ColorElement red,
					Pixels::ColorElement green,
					Pixels::ColorElement blue,
					Pixels::ColorElement alpha = Pixels::AlphaOpaque ) ;



				/**
				 * Returns the palette's color definitions.
				 *
				 * @note This method should generally not be used, and is
				 * "pseudo-const".
				 *
				 */
				virtual Pixels::ColorDefinition * getColorDefinitions() const ;



				/**
				 * Returns whether this palette has a colorkey registered.
				 *
				 */
				virtual bool hasColorKey() const ;



				/**
				 * Returns the index of the colorkey.
				 *
				 * @throw PaletteException if no colorkey is registered.
				 *
				 * @see hasColorKey
				 *
				 */
				virtual ColorCount getColorKeyIndex() const ;



				/**
				 * Sets the specified index as a colorkey.
				 *
				 */
				virtual void setColorKeyIndex( ColorCount colorkeyIndex ) ;



				/**
				 * Updates (recomputes) internal pixel colors from internal
				 * color definitions and specified pixel format.
				 *
				 */
				virtual void updatePixelColorsFrom(
					const Pixels::PixelFormat & format ) ;



				/**
				 * Quantizes the color definitions of this palette.
				 *
				 * @param quantizeMaxCoordinate the quantification will map
				 * color components (originally in [0;255]) to
				 * ([0,quantizeMaxCoordinate]).
				 *
				 * @example for the 8-bit palettes of the Nintendo DS,
				 * quantization is made on 5 bits, thus color components have to
				 * be quantized to [0;2^5-1=31], and quantizeMaxCoordinate = 31.
				 *
				 * @param scaleUp if false (the default), all color components
				 * will be left in [0,quantizeMaxCoordinate]. If true, then they
				 * will be mapped again to [0;255]. This allows to display the
				 * effects of quantification. For example, an original component
				 * equal to 120 will be quantized with quantizeMaxCoordinate=31
				 * to 15. With scaleUp set to true, it will be scaled back,
				 * which leads, due to the roundings, to 123.
				 *
				 */
				virtual void quantize(
					Pixels::ColorElement quantizeMaxCoordinate,
					bool scaleUp = false ) ;



				/**
				 * Applies a gamma correction to the color definitions of this
				 * palette.
				 *
				 * @param gamma the factor that will be used on each normalized
				 * color component Cn of each color definition:
				 * Cn_new = Cn_old^(1/gamma), then the component is
				 * denormalized.
				 *
				 * @example gamma is roughly equal to 2.3 for the Nintendo DS.
				 *
				 * @note If a colorkey is defined, it will be not be
				 * gamma-corrected on purpose.
				 *
				 */
				virtual void correctGamma( Pixels::GammaFactor gamma ) ;



				/**
				 * Returns the index of the color definition in the palette that
				 * matches the most closely to specified color.
				 *
				 * @throw PaletteException if the palette does not have at least
				 * one color.
				 *
				 * @note Uses GetDistance as a metric.
				 *
				 * @note If a colorkey is defined, it will not be taken into
				 * account as a possible color match.
				 *
				 */
				virtual ColorCount getClosestColorIndexTo(
					const Pixels::ColorDefinition & color ) const ;



				/**
				 * Draws in specified surface a series of horizontal lines
				 * taking its full width, each line being drawn with the color
				 * of the palette whose index corresponds to the line ordinate,
				 * with specified background underneath (useful to see that
				 * surface height and color number do not match).
				 *
				 * @return false if and only if surface height was too small to
				 * draw lines with all the palette colors.
				 *
				 * @note Cannot be 'const' since pixel color physical color may
				 * have to be recomputed.
				 *
				 * @note The colorkey will be drawn as other colors.
				 *
				 */
				virtual bool draw( Surface & targetSurface,
					const Pixels::ColorDefinition & backgroundColor
						= Pixels::White ) ;



				/**
				 * Tells whether the current palette has strictly identical
				 * color definitions.
				 *
				 * @param useAlpha tells whether the alpha coordinate is tested
				 * too for equality.
				 *
				 * @return true iff there is at least a duplicated color.
				 *
				 * @note If a colorkey is defined, it will not be taken into
				 * account for a possible color collision.
				 *
				 */
				virtual bool hasDuplicates( bool useAlpha = false ) const ;



				/**
				 * Saves current palette to specified file.
				 *
				 * All color definitions will be saved in palette order.
				 *
				 * Alpha coordinates will be ignored (not taken into account,
				 * not saved).
				 *
				 * A palette can be saved in the osdl.palette format (a.k.a
				 * unencoded format), or in .pal format (a.k.a. encoded format.
				 *
				 * With the osdl.palette format, a palette is made of a header,
				 * storing:
				 *  - the OSDL tag for palette
				 *  - in next byte: 1 if a colorkey is defined, 0 otherwise
				 *  - if there is a color key, in next sizeof(ColorCount) bytes
				 * the index of the color key is specified (otherwise the
				 * header contains only the first byte).
				 *
				 * After the header, the content of the palette itself is
				 * stored, with 8 bits per color component, in RGB order (thus
				 * 24 bits per color, and no alpha coordinate stored).  The
				 * extension of files respecting this format is preferably
				 * '.osdl.palette'.
				 *
				 * With the encoded format, quantized colors will be written,
				 * i.e. each color definition will be packed into 16 bits only
				 * (first bit set to 1, then 5 bits per quantized color
				 * component, in BGR order). The fact that a given index
				 * corresponds to a colorkey will not be stored in this encoded
				 * format. The encoded format is suitable for the Nintendo GBA
				 * and DS consoles.
				 *
				 * The extension of files respecting this format is preferably
				 * '.pal'.
				 *
				 * @param filename the name of the palette file to create
				 * (respecting recommended extensions is encouraged).
				 *
				 * @param encoded tells whether the palette will be written in
				 * encoded format (if true), or in .osdl.palette format (if
				 * false).
				 *
				 * @throw PaletteException if the operation failed.
				 *
				 */
				virtual void save( const std::string & filename,
					bool encoded = false ) const ;



				/**
				 * Returns an user-friendly description of the state of this
				 * object.
				 *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
				 *
				 */
				virtual const std::string toString(
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;





				// Static section.



				/**
				 * Palette factory, creating palettes with <b>numberOfColors</b>
				 * greyscale values, ranging uniformly from pure black to pure
				 * white.
				 *
				 */
				static Palette & CreateGreyScalePalette(
					ColorCount numberOfColors = 256 ) ;



				/**
				 * Palette factory, creating palettes with <b>numberOfColors</b>
				 * values, with continous tone interpolation from
				 * <b>colorStart</b> to <b>colorEnd</b>.
				 *
				 * @note Alpha coordinates are interpolated as well.
				 *
				 */
				static Palette & CreateGradationPalette(
					const Pixels::ColorDefinition & colorStart,
					const Pixels::ColorDefinition & colorEnd,
					ColorCount numberOfColors = 256 ) ;



				/**
				 * Palette factory, creating a colorkey-less 255 or
				 * colorkey-enabled 256 color master palette suitable for
				 * best-fit color-reduction of all kinds of images, notably
				 * animation frames.
				 *
				 * @param addColorkey if true, the default colorkey will be
				 * inserted at palette index #0, and registered as such. There
				 * will be then 256 colors in the palette. Otherwise, if false,
				 * only 255 colors (not 256) will be defined here, to leave one
				 * index for a future colorkey.
				 *
				 * @see http://en.wikipedia.org/wiki/:
				 * List_of_software_palettes#6-8-5_levels_RGB for the base
				 * colors of this palette.
				 *
				 */
				static Palette & CreateMasterPalette(
					bool addColorkey = true ) ;




			protected:



				/**
				 * Invalidates any already computed pixel colors.
				 *
				 */
				virtual void invalidatePixelColors() ;




				// Static section.



				/**
				 * Quantizes an individual color component.
				 *
				 * @param component the component to quantize.
				 *
				 * @param newMaxCoordinate the new upper-bound of color
				 * coordinate. Default value is 31, see example.
				 *
				 * @example On the Nintendo DS, color components are encoded in
				 * 5 bits, thus they range in [0;31] and this method would be
				 * called with newMaxCoordinate = 31 to have the specified
				 * component scaled according to this range.
				 *
				 */
				static Pixels::ColorElement QuantizeComponent(
					Pixels::ColorElement component,
					Pixels::ColorElement newMaxCoordinate = 31 ) ;



				/**
				 * Corrects gamma-wise an individual color component.
				 *
				 * @param component the component to gamma-correct.
				 *
				 * @param gamma the gamma correction factor to apply.
				 *
				 */
				static Pixels::ColorElement CorrectGammaComponent(
					Pixels::ColorElement component,
					Pixels::GammaFactor gamma )	;



				/**
				 * Returns the perceived distance of the human eye between the
				 * two specified color definitions.
				 *
				 * An advanced weighted Euclidean distance is used, see the
				 * lost-cost approximation in
				 * http://www.compuphase.com/cmetric.htm.
				 *
				 * @note The alpha coordinate is ignored for the distance
				 * computation.
				 *
				 */
				static ColorDistance GetDistance(
					const Pixels::ColorDefinition & firstColor,
					const Pixels::ColorDefinition & secondColor ) ;



				/// The number of colors defined in this palette.
				ColorCount _numberOfColors ;



				/**
				 * The logical palette, pointing to an array of _numberOfColors
				 * color definitions.
				 *
				 */
				Pixels::ColorDefinition * _colorDefs ;



				/**
				 * The physical palette, pointing to an array of
				 * _numberOfColors pixel colors.
				 *
				 */
				Pixels::PixelColor * _pixelColors ;



				/**
				 * Tells whether the physical colors (pixel colors) have already
				 * been computed from the stored pixel definitions.
				 *
				 */
				bool _converted ;



				/**
				 * Tells whether this palette owns any color definitions it
				 * uses.
				 *
				 */
				bool _ownsColorDefinition ;




			private:



				/// Tells whether this palette has a registered color key.
				bool _hasColorkey ;



				/// Index in palette of the used colorkey (if any), usually #0.
				ColorCount _colorKeyIndex ;



				/**
				 * Copy constructor made private to ensure that it will never be
				 * called.
				 *
				 * The compiler should complain whenever this undefined
				 * constructor is called, implicitly or not.
				 *
				 */
				explicit Palette( const Palette & source ) ;



				/**
				 * Assignment operator made private to ensure that it will never
				 * be called.
				 *
				 * The compiler should complain whenever this undefined operator
				 * is called, implicitly or not.
				 *
				 */
				Palette & operator = ( const Palette & source ) ;


		} ;


	}

}



#endif // OSDL_PALETTE_H_
