#ifndef OSDL_PALETTE_H_
#define OSDL_PALETTE_H_

		
#include "OSDLPixel.h"       // for ColorDefinition

#include "Ceylan.h"          // for TextDisplayable, Uint32, etc.

#include <string>



namespace OSDL
{
	
	namespace Video
	{
	

		/// Number of colors.
		typedef Ceylan::Uint32 ColorCount ;
		
		
		/// Exception raised when palette operation failed.
		class OSDL_DLL PaletteException : public VideoException
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
		 * Therefore a Palette actually owns two palettes, a logical one 
		 * (for color definitions) and a physical one (for pixel colors).
		 *
		 * A palette has getNumberOfColors() colors, ranging from 0 
		 * to getNumberOfColors()-1.
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
				 * @param format if specified (non-null), pixel colors are
				 * computed thanks to it, otherwise they are still to be
				 * converted.
				 * 
				 * @note Takes ownership of the array of color definitions.
				 *
				 */
				Palette( ColorCount numberOfColors, 
						Pixels::ColorDefinition * colors,
						Pixels::PixelFormat * format = 0 ) 
					throw( PaletteException ) ;

				
				/**
				 * Constructs a palette from an already existing SDL palette.
				 *
				 * @note Takes ownership of the SDL_Palette's color buffer.
				 * The SDL_Palette object itself is still to be deallocated
				 * by the caller, as if it had no color buffer.
				 *
				 */				
				explicit Palette( SDL_Palette & palette ) 
					throw( PaletteException ) ;
				
				
				/// Basic virtual destructor.
				virtual ~Palette() throw() ;
			
			
				/**
				 * Loads from memory a new palette.
				 *
				 * @param colors the color array, which should have been
				 * allocated like : 
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
				 * @note The pixel format cannot be 'const' because of SDL
				 * back-end.
				 *
				 */
				virtual void updatePixelColorsFrom( 
					Pixels::PixelFormat & format ) throw() ;


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
				 * Palette factory, creating palettes with 
				 * <b>numberOfColors</b> corresponding to landscape colors.
				 *
				 * @note Not implemented yet.
				 *
				 */
				static Palette & CreateLandscapePalette( 
					ColorCount numberOfColors = 256 ) throw() ; 
			
			
			
			protected:
					
			
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

