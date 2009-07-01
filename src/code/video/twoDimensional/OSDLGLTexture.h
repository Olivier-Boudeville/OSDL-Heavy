/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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
			class OSDL_DLL GLTextureException: public OpenGLException
			{
			
				public:
				
				
					/// Constructs a new TextureException.
					GLTextureException( const std::string & reason ) ;
					
					
					/// Virtual destructor.
					virtual ~GLTextureException() throw() ;
								
			} ;



			/**
			 * Encapsulates an OpenGL texture, constructed directly or not 
			 * from a surface, that can be loaded as resource.
			 *
			 * The ownership of the source surface is not taken by GLtexture
			 * instances, since they have to convert it anyway into their own
			 * specific format for OpenGL.
			 * 
			 * This converted surface can be kept by the GLtexture for future
			 * use.
			 *
			 * It is recommended to do so, insofar as, on some platforms, some
			 * operations, such as resizing, destroy the OpenGL context,
			 * including bound textures. 
			 *
			 * In this case, they have to be reloaded, which might be easier 
			 * if they have kept their converted surface.
			 *
			 * @note OpenGL support must have been selected and be available
			 * at configure time so that these OpenGL services can be used.
			 * Moreover as soon as a texture is uploaded, a proper OpenGL
			 * context must be available.
			 *
		 	 * @see 'http://osdl.sourceforge.net', then 'Documentation',
			 * 'Rendering', 'OpenGL + SDL' for further implementation details.
			 *
			 */
			class OSDL_DLL GLTexture : public Ceylan::TextDisplayable,
				public Ceylan::LoadableWithContent<Surface>
			{

			
				public:

			
			
					/**
					 * Texture support can be disabled, or enabled for 
					 * 1D or 2D textures.
					 *
					 * Default is 'TwoDim', enabled for 2D textures.
					 *
					 */
					enum TextureDimensionality { Disabled, OneDim, TwoDim } ;
					
					
					
					/**
					 * Texture flavours are presets that can be chosen as 
					 * a whole.
					 *
					 * 'None' does not change state of texture settings, 
					 * whereas 'Basic' sets classical settings for filters 
					 * and texture coordinate management, and 'For2D' selects
					 * settings appropriate for 2D rendering.
					 *
					 */
					enum TextureFlavour { None, Basic, For2D, For3D } ;

				
				
					/**
					 * Constructs a texture out of an image stored in a file.
					 *
					 * @param textureFilename the filename of the texture image,
					 * whose format (PNG, JPEG, etc.) will be auto-detected 
					 * from content.
					 *
					 * @param preload the image will be loaded directly by this
					 * constructor iff true, otherwise only its path will be
					 * stored to allow for later loading.
					 *
					 * @param flavour the texture flavour that should be used.
					 *
					 * The internal surface, corresponding to the image, will
					 * be, if needed, automatically kept back and managed by
					 * this texture, so that it can be reloaded in an OpenGL
					 * context if necessary, should the context be lost. 
					 *
					 * @note A constructed texture is not uploaded to the
					 * video card yet.
					 *
					 * @throw GLTextureException if the operation failed.
					 *
					 */
					explicit GLTexture( const std::string textureFilename,
						TextureFlavour flavour, bool preload = true ) ;
				
				
				
					/**
					 * Constructs a texture out of an existing surface, whose
					 * ownership is not taken.
					 *
					 * @param sourceSurface the surface from which the 
					 * texture will be defined. 
					 * It cannot be 'const' since a temporary change to 
					 * the surface has to be performed. 
					 * Nevertheless the source surface is, after a successful
					 * call, actually unchanged, as its state is restored. 
					 *
					 * @param flavour the texture flavour that should be used.
					 *
					 * The internal surface, corresponding to the image, will
					 * be, if needed, automatically kept back and managed by
					 * this texture, so that it can be reloaded in an OpenGL
					 * context if necessary, should the context be lost. 
					 *
					 */
					//explicit GLTexture( Surface & sourceSurface,
					//	TextureFlavour flavour ) ;
				
				
				
					/**
					 * Virtual destructor, deallocates the owned internal
					 * surface, if any.
					 *
					 */
					virtual ~GLTexture() throw() ;
				


					/**
					 * Returns the width of this texture.
					 *
					 * @throw GLTextureException if the texture is not already
					 * loaded.
					 *
					 */
					virtual Length getWidth() const ;


					/**
					 * Returns the height of this texture.
					 *
					 * @throw GLTextureException if the texture is not already
					 * loaded.
					 *
					 */
					virtual Length getHeight() const ;



					
					/*
					 * Section dealing with the transfer and removal of this
					 * texture from the system memory to the video card.
					 *
					 */
					
			
					/**
					 * Returns true iff the texture has already been uploaded
					 * to the video card.
					 *
					 */
					virtual bool wasUploaded() const ;
					
					 
					/**
					 * Uploads the internal texture to the OpenGL context,
					 * i.e. copies the internal surface to the video card.
					 *
					 * @note The corresponding texture surface will be loaded 
					 * is necessary. Thus in all cases after an upload, an 
					 * unload can be performed.
					 *
					 * @note A proper OpenGL context must be availabe before
					 * calling this method.
					 *
					 * @throw GLTextureException if the texture could not 
					 * be uploaded.
					 *					
					 */
					virtual void upload() ;
					
					
					
					/**
					 * Removes this texture, supposedly already uploaded,
					 * from the video card.
					 *
					 * @throw GLTextureException if the texture could not 
					 * be removed.
					 *					
					 */
					virtual void remove() ;
					
					
					
					
					
					/**
					 * Sets this texture as the current texture: further
					 * textured renderings will make use of it.
					 *
					 * @throw GLTextureException if the operation failed.
					 *
					 */
					virtual void setAsCurrent() const ;
				
					
					
					/**
					 * Returns true iff this texture is loaded in texture 
					 * memory.
					 *
					 * @throw GLTextureException if the operation failed.
					 *
					 */
					virtual bool isResident() ;
					
					
					
					

					// LoadableWithContent template instanciation.
		
				
				
					/**
					 * Loads the texture image from file.
					 *
					 * @return true iff the image had to be actually loaded
					 * (otherwise it was already loaded and nothing was done).
					 *
					 * @throw Ceylan::LoadableException whenever the loading
					 * fails.
					 *
					 * @note Loading (the internal texture image) does not imply
					 * uploading it to the video card.
					 *
					 */
					virtual bool load() ;
		
		
		
					/**
					 * Unloads the texture image that may be contained by this
					 * instance.
					 *
					 * @return true iff the image had to be actually unloaded
					 * (otherwise it was not already available and nothing was
					 * done).
					 *
					 * @throw Ceylan::LoadableException whenever the unloading
					 * fails.
					 *
					 * @note Unloading (the internal texture image) does 
					 * not imply removing it from the texture memory in the 
					 * video card.
					 *
					 */
					virtual bool unload() ;
					



					
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
						Ceylan::VerbosityLevels level = Ceylan::high ) const ;
				
				
				

				
				
					// Static section.
					
					
					// FIXME Should use the flavour member.
						
										
					/// Returns the current texture dimensionality.
					static TextureDimensionality GetTextureDimensionality() ;
				
					
					
					/// Sets the current texture dimensionality.
					static void SetTextureDimensionality( 
						TextureDimensionality Dimensionality = TwoDim ) ;
				
				
				
					/**
					 * Stores the current texture dimensionality.
					 *
					 * Default is 'TwoDim'.
					 *
					 */
					static TextureDimensionality CurrentTextureDimensionality ;
					
								
				
					/**
					 * Sets the current texture state according to 
					 * specified texture flavour.
					 *
					 * @param textureFlavour the selected texture flavour.
					 *
					 * @throw GLTextureException if the operation failed,
					 * including if no OpenGL support is available.
					 *
					 */
					static void SetTextureFlavour( 
						TextureFlavour textureFlavour ) ;
					
					

					/**
					 * Sets a parameter of a texture environment.
					 *
					 * @param targetEnvironment the texture environment to set.
					 *
					 * @param environmentParameter the symbolic name of a
					 * texture environment parameter.
					 *
					 * @param parameterValue a single symbolic constant, 
					 * a single floating-point number, or an RGBA color.
					 *
				 	 * @throw GLTextureException should an error occur.
					 *
					 */
					static void SetTextureEnvironmentParameter( 
						GLEnumeration targetEnvironment,
						GLEnumeration environmentParameter,
						GLfloat parameterValue ) ;



					/**
					 * Sets the parameters of a texture environment.
					 *
					 * @param targetEnvironment the texture environment to set.
					 *
					 * @param environmentParameter the symbolic name of a
					 * texture environment parameter.
					 *
					 * @param parameterValue pointer to a parameter array 
					 * that contains either a single symbolic constant, 
					 * single floating-point number, or an RGBA color.
					 *
				 	 * @throw GLTextureException should an error occur.
					 *
					 */
					static void SetTextureEnvironmentParameter( 
						GLEnumeration targetEnvironment,
						GLEnumeration environmentParameter,
						const GLfloat * parameterValues ) ;

					
					
					/**
					 * Returns the filename extension which corresponds to
					 * the specified texture flavour.
					 *
					 * For example, for the For2D flavour, ".tex2D" shall be
					 * returned.
					 *
					 * @throw GLTextureException if the flavour is not known.
					 *
					 */
					static std::string GetExtensionForFlavour( 
						TextureFlavour flavour ) ;
					
					
					
					
				protected:
				
				
				
					/**
					 * Uploads an adequately converted surface, copied 
					 * from the specified one, and deallocates it iff no
					 * OpenGL context can be lost, otherwise keeps it in 
					 * texture cache.
					 *
					 * @param sourceSurface the surface to upload. 
					 * In all cases it will be copied and converted, 
					 * therefore it will remain untouched.
					 *
					 * The internal surface, corresponding to the image, will
					 * be, if needed, automatically kept back and managed by
					 * this texture, so that it can be reloaded in an OpenGL
					 * context if necessary, in case it is lost. 
					 *
					 * @note sourceSurface cannot be 'const' since 
					 * per-surface alpha setting has to be modified 
					 * temporarily, even if this method has eventually no 
					 * side-effect on this surface.
					 *
					 */
					void upload( Surface & sourceSurface ) ;
					
					
					
					/*
					 * The internal texture surface itself is in the _content
					 * member, inherited from the loadable template.
					 *
					 */
					 
					 
			
					/**
					 * Identifier given by OpenGL to reference this texture.
					 *
					 */
					GLTextureIdentifier _id ;
			
				
				
					/**
					 * Stores the current texture flavour.	
					 *
					 * @note Needed, as a side-effect of uploading a texture
					 * is to create a new (blank) target texture, whose
					 * settings therefore need to be set specifically set.
					 *
					 */	
					TextureFlavour _flavour ;
			
			

			
				private:
				
				
				
					/**
					 * Copy constructor made private to ensure that it will 
					 * never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * constructor is called, implicitly or not.
					 * 
					 */			 
					explicit GLTexture( const GLTexture & source ) ;
			
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will never be called.
					 *
					 * The compiler should complain whenever this undefined 
					 * operator is called, implicitly or not.
					 * 
					 */			 
					GLTexture & operator = ( const GLTexture & source ) ;
			
			
			} ;
			
			
			
			/// Texture counted pointer.
			typedef Ceylan::CountedPointer<GLTexture> TextureCountedPtr ;
		
		}
		
	}

}



#endif // OSDL_GL_TEXTURE_H_

