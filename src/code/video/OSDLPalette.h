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
	

		/// Number of colors.
		typedef Ceylan::Uint32 ColorCount ;
		
		
		/**
		 * Allows to designate a palette.
		 *
		 * Identifier 1 is reserved for the main master palette, as defined
		 * in master-palette.pal on the Nintendo DS.
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
		
		
		
		
		/// Exception raised when palette operation failed.
		class OSDL_DLL PaletteException: public VideoException
		{
		
			public:
				explicit PaletteException( const std::string & message ) 
					throw() ;
				virtual ~PaletteException() throw() ;
							
		} ;
		
	
	
		/**
		 * Describes a palette, for example for color-indexed modes.
		 *
		 * In these modes, the color of a pixel is defined by an index 
		 * which points to a specific entry of the palette.
		 *
		 * Palettes can be used too for special effects, even in true 
		 * color modes.
		 *
		 * A palette is set thanks to color definitions, which describe a 
		 * color 'as is', without taking into account a particular screen
		 * format. 
		 *
		 * To be used on a particular surface, color definitions need to be
		 * converted into PixelColor (actual colors), which are defined
		 * according to the surface pixel format. 
		 *
		 * Therefore a Palette actually uses two palettes, a logical one 
		 * (for color definitions) and a physical one (for pixel colors).
		 *
		 * A palette has getNumberOfColors() colors, ranging from 0 
		 * to getNumberOfColors()-1.
		 *
		 */
		class OSDL_DLL Palette: public Ceylan::TextDisplayable
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
				 * @param numberOfColors the number of color for this palette.
				 *
				 * @param colors pointer to an array of numberOfColors color
				 * definitions. This palette takes ownership of the array. If
				 * a null pointer is specified, then the palette will create
				 * its own array of numberOfColors and set all colors to 
				 * full black, i.e. (0,0,0,255) in the RGBA colorspace.
				 *
				 * @param format if specified (non-null), pixel colors are
				 * computed thanks to it, otherwise they are still to be
				 * converted.
				 * 
				 * @note Takes ownership of the array of color definitions.
				 *
				 */
				Palette( ColorCount numberOfColors, 
						Pixels::ColorDefinition * colors = 0,
						Pixels::PixelFormat * format = 0 ) 
					throw( PaletteException ) ;

				
				/**
				 * Constructs a palette from an already existing SDL palette.
				 *
				 * @note Does not take ownership of the LowLevelPalette's color
				 * buffer.
				 *
				 */				
				explicit Palette( LowLevelPalette & palette ) 
					throw( PaletteException ) ;
				
				
				/// Basic virtual destructor.
				virtual ~Palette() throw() ;
			
			
				/**
				 * Loads from memory a new palette.
				 *
				 * @param colors the color array, which should have been
				 * allocated like: 
				 * 'Pixels::ColorDefinition * colors 
				 *    = new Pixels::ColorDefinition[ numberOfColors ] ;'
				 *
				 * @note If a palette was already registered, it is 
				 * deallocated first.
				 *
				 * @note The palette takes ownership of the specified array 
				 * of colors, and will deallocate them when necessary.
				 *
				 */
				virtual void load( ColorCount numberOfColors, 
						Pixels::ColorDefinition * colors ) 
					throw( PaletteException ) ;
			
			
				/// Returns the number of colors defined in the palette.
				virtual ColorCount getNumberOfColors() const throw() ;
				
				
				/**
				 * Returns the pixel color of the palette which is located 
				 * at specified index.
				 *
				 * @note The pixel color is returned as a 'const' reference.
				 *
				 * @throw PaletteException if index is out of bounds 
				 * (superior or equal to getNumberOfColors).
				 *
				 *
				 */
				virtual const Pixels::PixelColor & getPixelColorAt( 
						ColorCount index ) 
					const throw( PaletteException ) ;
					
					
				/**
				 * Returns the palette's pointer to pixel colors.
				 *
				 * @note This method should generally not be used.
				 *
				 */
				virtual Pixels::PixelColor * getPixelColors() const throw() ;
				
				
				/**
				 * Returns the color definition of the palette which is 
				 * located at specified index.
				 *
				 * @note The color definition is returned as a 'const'
				 * reference.
				 *
				 * @throw PaletteException if index is out of bounds 
				 * (superior or equal to getNumberOfColors).
				 *
				 *
				 */
				virtual const Pixels::ColorDefinition & getColorDefinitionAt(
						ColorCount index ) 
					const throw( PaletteException ) ;
					
					
				/**
				 * Sets the specified color definition at specified index of 
				 * the palette (overwrites prior definition).
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
						Pixels::ColorDefinition & newColorDefinition ) 
					throw( PaletteException ) ;
					
					
				/**
				 * Sets the specified color definition at specified index of 
				 * the palette (overwrites prior definition).
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
						Pixels::ColorElement alpha = Pixels::AlphaOpaque ) 
					throw( PaletteException ) ;
					
					
				/**
				 * Returns the palette's color definitions.
				 *
				 * @note This method should generally not be used.
				 *
				 */
				virtual Pixels::ColorDefinition * getColorDefinitions() 
					const throw() ;

					
				/**
				 * Updates (recomputes) internal pixel colors from internal
				 * color definitions and specified pixel format.
				 *
				 */
				virtual void updatePixelColorsFrom( 
					const Pixels::PixelFormat & format ) throw() ;


				/**
				 * Quantizes the color definitions of this palette.
				 *
				 * @param quantizeMaxCoordinate the quantification will map 
				 * color components (originally in [0;255]) to
				 * ([0,quantizeMaxCoordinate]). 
				 *
				 * @example for the 8-bit palettes of the Nintendo DS,
				 * quantization is made on 5 bits, thus color components have
				 * to be quantized to [0;2^5-1=31], and 
				 * quantizeMaxCoordinate = 31.				 
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
					bool scaleUp = false ) throw() ;


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
				 */
				virtual void correctGamma( Pixels::GammaFactor gamma ) throw() ;


				/**
				 * Draws in specified surface a series of horizontal lines
				 * taking its full width, each line being drawn with the 
				 * color of the palette whose index corresponds to
				 * the line ordinate, with specified background underneath
				 * (useful to see that surface height and color number do 
				 * not match).
				 *
				 * @return false if and only if surface height was too small 
				 * to draw lines with all the palette colors.
				 *
				 * @note Cannot be 'const' since pixel color physical color 
				 * may have to be recomputed.
				 *
				 */
				virtual bool draw( Surface & targetSurface, 
					Pixels::ColorDefinition backgroundColor = Pixels::White )
						throw() ;
				
				
				/**
				 * Saves current palette to specified file.
				 *
				 * All color definitions will be saved in palette order.
				 *
				 * Alpha coordinates will be ignored (not taken into account,
				 * not saved).
				 *
				 * @param filename the name of the palette file to create
				 * (prefer a '.pal' extension if encoded, a '.rgb' one 
				 * otherwise).
				 *
				 * @param encoded tells whether the raw colors will be written
				 * (if false), i.e. in RGB order with each color component 
				 * on 8 bits (thus 24 bits per color), or, if true, if 
				 * encoded colors will be written, i.e. each color definition 
				 * being packed in 16 bits (first bit to 1, then 5 bits per
				 * quantized color component, in BGR order). 
				 *
				 * @note The encoded format is suitable for the Nintendo GBA
				 * and DS consoles. '.pal' preferred to '.osdl.palette', as no
				 * OSDL header thus no tag.
				 * 
				 * @throw PaletteException if the operation failed.
				 *
				 */
				virtual void save( const std::string & filename,
					bool encoded = false ) const throw( PaletteException ) ;
				 	
					
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
						Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;


				
				// Static section.
						
						
				/**
				 * Palette factory, creating palettes with 
				 * <b>numberOfColors</b> greyscale values, ranging uniformly
				 * from pure black to pure white.
				 *
				 */
				static Palette & CreateGreyScalePalette( 
					ColorCount numberOfColors = 256 ) throw() ; 
			
			
				/**
				 * Palette factory, creating palettes with 
				 * <b>numberOfColors</b> values, with continous tone
				 * interpolation from <b>colorStart</b> to <b>colorEnd</b>.
				 *
				 * @note Alpha coordinates are interpolated as well.
				 *
				 */
				static Palette & CreateGradationPalette( 
					Pixels::ColorDefinition colorStart,
					Pixels::ColorDefinition colorEnd, 
					ColorCount numberOfColors = 256 ) throw() ; 
			
			
				/**
				 * Palette factory, creating a 255-color master palette
				 * suitable for best-fit color-reduction of all kinds of images,
				 * notably animation frames.
				 *
				 * @note Only 255 colors (not 256) are defined here, to leave
				 * one index for a future colorkey, not specified here to avoid
				 * that color-reduction algorithms use the corresponding color
				 * to match colors of target image.
				 *
				 * @see http://en.wikipedia.org/wiki/:
				 * List_of_software_palettes#6-8-5_levels_RGB for the
				 * base colors of this palette.
				 *
				 */
				static Palette & CreateMasterPalette() throw() ; 
			
			
			
			protected:
				
				
				/**
				 * Invalidates any already computed pixel colors.
				 *
				 */
				virtual void invalidatePixelColors() throw() ;
				
				
				
				// Static section.
				
				
				/**
				 * Quantizes an individual color component.
				 *
				 * @param component the component to quantize.
				 *
				 * @param newMaxCoordinate the new upper-bound of color
				 * coordinate. Default value is 31, see example.
				 *
				 * @example On the Nintendo DS, color components are encoded
				 * in 5 bits, thus they range in [0;31] and this method would
				 * be called with newMaxCoordinate = 31 to have the specified
				 * component scaled according to this range.
				 *
				 */
				static Pixels::ColorElement QuantizeComponent( 
					Pixels::ColorElement component,	
					Pixels::ColorElement newMaxCoordinate = 31 ) throw() ;
					
					
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
					Pixels::GammaFactor gamma )	throw() ;
					
			
			
				/// The number of colors defined in this palette.
				ColorCount _numberOfColors ;
				
				
				/**
				 * The logical palette, pointing to an array of 
				 * _numberOfColors color definitions.
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
				 * Tells whether the physical colors (pixel colors) have 
				 * already been computed from the stored pixel definitions.
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
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit Palette( const Palette & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Palette & operator = ( const Palette & source ) throw() ;
		
		
		} ;
	
	}

}


#endif // OSDL_PALETTE_H_

