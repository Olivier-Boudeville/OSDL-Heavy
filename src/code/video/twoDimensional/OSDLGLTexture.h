#ifndef OSDL_GL_TEXTURE_H_
#define OSDL_GL_TEXTURE_H_


#include "OSDLOpenGL.h"    // for OpenGLException

#include "Ceylan.h"        // for inheritance, Uint32, etc.

#include <string>



namespace OSDL
{

	
	namespace Video
	{
	
	
		// Texture are made from surfaces.
		class Surface ;
		
	
		namespace OpenGL
		{
		
		
		
			/**
			 * Texture identifier.
			 *
			 * @note Made so that it can be converted (i.e. has at least the
			 * same value range) into OpenGL texture identifier, Gluint
			 * (which is 'unsigned int' in general).
			 *
			 * No 'typedef GLUint GLTextureIdentifier' could be kept, as
			 * it would have made this header file depend on OSDLConfig, which
			 * should not be installed.
			 *
			 */
			typedef GLU::Int GLTextureIdentifier ;
		
		
		
			/// Exception to be raised when OpenGL texture operations fail.
			class OSDL_DLL GLTextureException : public OpenGLException
			{
			
				public:
				
				
					/// Constructs a new TextureException.
					GLTextureException( const std::string & reason ) throw() ;
					
					/// Virtual destructor.
					virtual ~GLTextureException() throw() ;
								
			} ;

		

			/**
			 * Encapsulates an OpenGL texture, constructed directly or not 
			 * from a surface.
			 *
			 * The ownership of the source surface is not taken by GLtexture
			 * instances, since they have to convert it into their own
			 * specific format for OpenGL.
			 * 
			 * This converted surface can be kept by the GLtexture for future
			 * use.
			 *
			 * It is recommended to do so, insofar as on some platforms some
			 * operations, such as resizing, destroy the OpenGL context,
			 * including bound textures. 
			 *
			 * In this case, they have to be reloaded, which is possible 
			 * if they have kept their converted surface.
			 *
			 * @note OpenGL support must have been selected and be available
			 * at configure time so that these OpenGL services can be used.
			 *
			 * @note SetTextureMode should be called prior to any texture
			 * operation.
			 *
		 	 * @see 'http://osdl.sourceforge.net', then 'Documentation',
			 * 'Rendering', 'OpenGL + SDL' for further implementation details.
			 *
			 */
			class OSDL_DLL GLTexture : public Ceylan::TextDisplayable
			{

			
				public:

			
			
					/**
					 * Texture support can be disabled, or enabled for 
					 * 1D or 2D textures.
					 *
					 * Default is 'TwoDim', enabled for 2D textures.
					 *
					 */
					enum TextureMode { Disabled, OneDim, TwoDim } ;
					
					
					
					/**
					 * Texture flavours are presets that can be chosen as 
					 * a whole.
					 *
					 * 'None' does not change state of texture settings, 
					 * whereas 'Basic' sets classical settings for filters 
					 * and texture coordinate management.
					 *
					 */
					enum Textureflavour { None, Basic } ;

				
				
					/**
					 * Constructs a texture out of an image stored in a file.
					 *
					 * @param imageFilename the filename of the image, 
					 * whose format (PNG, JPEG, etc.) will be auto-detected.
					 *
					 * @param flavour chooses the texture flavour to apply
					 * before loading this texture. 
					 * Changes the texture settings if not 'None' (side-effect).
					 *
					 * @param keep tells whether the converted surface, 
					 * whose source is the one loaded from specified filename,
					 * should be kept back in this texture, so that
					 * the texture can be reloaded in OpenGL context if
					 * necessary. 
					 * Otherwise (keep is false), after the image has been
					 * loaded, converted and uploaded to OpenGL, it is
					 * deallocated.
					 *
					 */
					explicit GLTexture( const std::string imageFilename, 
							Textureflavour flavour = None, bool keep = true ) 
						throw( GLTextureException ) ;
				
				
				
					/**
					 * Constructs a texture out of an existing surface.
					 *
					 * @param sourceSurface the surface from which the 
					 * texture will be defined. 
					 * It cannot be 'const' since a temporary change to 
					 * the surface has to be performed. 
					 * Nevertheless the source surface is, after a successful
					 * call, actually unchanged. 
					 *
					 * @param flavour chooses the texture flavour to apply
					 * before loading this texture. 
					 * Changes the texture settings if not None (side-effect).
					 *
					 * @param keep tells whether the converted surface, 
					 * whose source is the one loaded from specified filename,
					 * should be kept back in this texture, so that
					 * the texture can be reloaded in OpenGL context if
					 * necessary. 
					 * Otherwise (keep is false), after the image has been
					 * loaded, converted and uploaded to OpenGL, it is
					 * deallocated.
					 *
					 */
					explicit GLTexture( Surface & sourceSurface, 
							Textureflavour flavour = None, bool keep = true ) 
						throw( GLTextureException ) ;
				
				
					/**
					 * Virtual destructor, deallocates the owned internal
					 * surface, if any.
					 *
					 */
					virtual ~GLTexture() throw() ;
				
				
					/**
					 * Tells whether this texture can be reloaded to the 
					 * OpenGL context. 
					 *
					 * One necessary condition is that its source surface 
					 * is available.
					 *
					 * @see reload 
					 *
					 */
					virtual bool canBeUploaded() const throw() ;
										
					
					/**
					 * Uploads the internal texture to the OpenGL context.
					 *
					 * @throw GLTextureException if the texture could not 
					 * be reloaded, for example if its source surface is 
					 * not available anymore.
					 *					
					 */
					virtual void upload() throw( GLTextureException ) ;
					
					
	 	            /**
		             * Returns an user-friendly description of the state 
					 * of this object.
		             *
					 * @param level the requested verbosity level.
					 *
					 * @note Text output format is determined from 
					 * overall settings.
					 *
					 * @see Ceylan::TextDisplayable
		             *
		             */
			 		virtual const std::string toString( 
							Ceylan::VerbosityLevels level = Ceylan::high ) 
						const throw() ;
				
				
				
					// Static section.
					
										
					/// Returns the current texture mode.
					static TextureMode GetTextureMode() throw() ;
					
					
					/// Sets the current texture mode.
					static void SetTextureMode( 
						TextureMode newMode = TwoDim ) throw() ;
				
				
					/**
					 * Stores the current texture mode.
					 *
					 * Default is 'TwoDim'.
					 *
					 */
					static TextureMode CurrentTextureMode ;
					
								
				
					/**
					 * Sets the current texture state according to 
					 * specified flavour.
					 *
					 * @throw GLTextureException if the operation failed,
					 * including if no OpenGL support is available.
					 *
					 */
					void SetTextureFlavour( Textureflavour flavour )
						throw( GLTextureException ) ;
					
					
					
					
				protected:
				
				
				
					/**
					 * Uploads an adequately converted surface, copied 
					 * from the specified one, and deallocates it iff keep 
					 * is false, otherwise keeps it in texture cache.
					 *
					 * @param sourceSurface the surface to upload. 
					 * In all cases it will be copied and converted, 
					 * therefore it will remain untouched.
					 *
					 * @param flavour the texture flavour to apply. 
					 * See Textureflavour.
					 *
					 * @param keep tells whether the converted surface 
					 * is to be kept for future reloading.
					 *
					 * @note sourceSurface cannot be 'const' since 
					 * per-surface alpha setting has to be modified 
					 * temporarily, even if this method has eventually no 
					 * side-effect on this surface.
					 *
					 */
					void upload( Surface & sourceSurface, 
							Textureflavour flavour, bool keep )
						throw( GLTextureException ) ;
					
					
				
					/**
					 * The source surface from which this texture is made.
					 *
					 * @note A non-null source pointer implies that the 
					 * texture owns this surface.
					 *
					 */
					Surface * _source ;
			
			
					/**
					 * Identifier given by OpenGL to reference this texture.
					 *
					 */
					GLTextureIdentifier _id ;
			
			
			
			
				private:
				
				
				
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit GLTexture( const GLTexture & source ) throw() ;
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					GLTexture & operator = ( const GLTexture & source ) 
						throw() ;
			
			
			} ;
		
		}
		
	}

}



#endif // OSDL_GL_TEXTURE_H_

