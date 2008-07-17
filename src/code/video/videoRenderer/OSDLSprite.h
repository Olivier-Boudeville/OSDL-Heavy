#ifndef OSDL_SPRITE_H_
#define OSDL_SPRITE_H_





#include "OSDLVideoRenderer.h"        // for VideoRenderingException
#include "OSDLVideoTypes.h"           // for Length


/*


#include "OSDLEvents.h"               // for RenderingTick

#include "OSDLException.h"            // for Exception

#include <list>

*/

#include "Ceylan.h"                   // for inheritance


#include <string>



namespace OSDL
{


	namespace Engine 
	{

		class BoundingBox2D ;
		
	}
	
	
		
	namespace Video 
	{

		namespace OpenGL
		{
		
			// Sprites may use a texture with OpenGL:
			class GLTexture ;
			
		}
			
	}
	
	
		
	namespace Rendering 
	{
	

		/// Exception raised when a sprite operation failed.
		class OSDL_DLL SpriteException: public VideoRenderingException
		{
		
			public:
				explicit SpriteException( const std::string & message ) 
					throw() ;
				virtual ~SpriteException() throw() ;
							
		} ;



		/**
		 * A sprite is a specialized view corresponding to a 2D 
		 * bitmap-based graphical element.
		 *
		 */	
		class OSDL_DLL Sprite: public Ceylan::View
		{
		
		
			public:
			
			
				/// Describes a possible shape for a tiled sprite. 
				typedef Ceylan::Uint8 Shape ;
			
			
				/**
				 * Constructs a new sprite.
				 *
				 * @param frameFilename the filename of the OSDL frame to be
				 * used.
				 *
				 */
				explicit Sprite( const std::string & frameFilename ) throw() ;
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~Sprite() throw() ;
			


	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
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

				
				// Sprite shape section.
				

				/*
				 * Convention is to name basic shapes 'ATimesB' for AxB,
				 * A being the sprite width, B being the sprite height, in
				 * pixels.
				 * 
				 * Typing shapes this way is convenient for tile-based platforms
				 * like the Nintendo DS.
				 *
				 * For other platforms, like the PC, differents shapes are
				 * usually used, like 'PowersOfTwo'.
				 *
				 * @note Not enum (int), as one byte is largely enough.
				 *
				 */
				
				
				// Section with an height of 8 pixels.
				
				/// Describes a sprite whose shape is 8x8 pixels.
				static const Shape EightTimesEight ;
				
				/// Describes a sprite whose shape is 16x8 pixels.
				static const Shape SixteenTimesEight ;
				
				/// Describes a sprite whose shape is 32x8 pixels.
				static const Shape ThirtyTwoTimesEight ;
				
		
				
				// Section with an height of 16 pixels.
				
				/// Describes a sprite whose shape is 8x16 pixels.
				static const Shape EightTimesSixteen ;
				
				/// Describes a sprite whose shape is 16x16 pixels.
				static const Shape SixteenTimesSixteen ;
				
				/// Describes a sprite whose shape is 32x16 pixels.
				static const Shape ThirtyTwoTimesSixteen ;
				
				
				
				// Section with an height of 32 pixels.
				
				/// Describes a sprite whose shape is 8x32 pixels.
				static const Shape EightTimesThirtyTwo ;
				
				/// Describes a sprite whose shape is 16x32 pixels.
				static const Shape SixteenTimesThirtyTwo ;
				
				/// Describes a sprite whose shape is 32x32 pixels.
				static const Shape ThirtyTwoTimesThirtyTwo ;
				
				/// Describes a sprite whose shape is 32x32 pixels.
				static const Shape SixtyFourTimesThirtyTwo ;
				
				
				
				// Section with an height of 64 pixels.
								
				/// Describes a sprite whose shape is 32x64 pixels.
				static const Shape ThirtyTwoTimesSixtyFour ;
				
				/// Describes a sprite whose shape is 64x64 pixels.
				static const Shape SixtyFourTimesSixtyFour ;

				/**
				 * Describes a sprite whose width and height are powers of 
				 * two (not necessarily the samoe one).
				 *
				 */
				static const Shape PowersOfTwo ;

				
				/**
				 * Returns a textual description of specified sprite shape.
				 *
				 * @throw SpriteException if the specified shape is not known.
				 *
				 */
				static std::string DescribeShape( Shape shape ) 
					throw( SpriteException ) ;
				
				
				/**
				 * Returns the smallest registered standard sprite shape 
				 * enclosing a rectangle of specified dimensions.
				 *
				 * @param width the width of the rectangle to enclose.
				 *
				 * @param height the height of the rectangle to enclose.
				 *
				 * @throw SpriteException if no known shape is large enough
				 * to contain the specified size.
				 *
				 */
				static Shape GetSmallestEnclosingShape( 
					OSDL::Video::Length width,
					OSDL::Video::Length height ) throw( SpriteException ) ;
					
					
				
				/**
				 * Returns the width, in pixels, of the sprite shape associated
				 * to specified shape.
				 *
				 * @param shape the shape whose width is needed.
				 *
				 * @return the width of specified shape, in pixels.
				 *
				 * @throw SpriteException if the operation failed, including 
				 * if the shape is not known.
				 *
				 */	
				static OSDL::Video::Length GetShapeWidthFor( Shape shape )
					throw( SpriteException ) ;
				
				
				/**
				 * Returns the height, in pixels, of the sprite shape associated
				 * to specified shape.
				 *
				 * @param shape the shape whose height is needed.
				 *
				 * @return the height of specified shape, in pixels.
				 *
				 * @throw SpriteException if the operation failed, including 
				 * if the shape is not known.
				 *
				 */	
				static OSDL::Video::Length GetShapeHeightFor( Shape shape )
					throw( SpriteException ) ;
				
				
				
				
			protected:
			

				/**
				 * Tells whether this sprite owns the bounding box(es) it 
				 * refers to.
				 *
				 * On a local scheme, the bounding box is shared between 
				 * the view(s) and the model, since it may use both for
				 * rendering and collision detection.
				 *
				 * On a distributed context (typically, client/server), 
				 * multiple bounding boxes exist: one on each client, 
				 *shared between the views, and one on the server, for 
				 * the model.
				 *
				 */
				bool _ownBoundingBox ;
				
				
				/**
				 * Each sprite is linked with a bounding box, which 
				 * graphically contains it.
				 *
				 * @note For the moment, each sprite has only one (2D) 
				 * bounding box.
				 *
				 */
				Engine::BoundingBox2D * _box ;
		
		
				/**
				 * The shape of this sprite.
				 *
				 */
				Shape _shape ;
				
#if defined(OSDL_ARCH_NINTENDO_DS) && OSDL_ARCH_NINTENDO_DS 

				//TileMap + Frame ?
				
#else // OSDL_ARCH_NINTENDO_DS

				/// The texture owned by this sprite, if OpenGL is used.
				Video::OpenGL::GLTexture * _texture ;
				
#endif // OSDL_ARCH_NINTENDO_DS
				 
				 
						
			private:
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				Sprite( const Sprite & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Sprite & operator = ( const Sprite & source ) throw() ;
				
		} ;

	}

}


#endif // OSDL_SPRITE_H_

