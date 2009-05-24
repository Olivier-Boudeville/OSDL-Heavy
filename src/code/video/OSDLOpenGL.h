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


#ifndef OSDL_OPENGL_H_
#define OSDL_OPENGL_H_


#include "OSDLVideoTypes.h"  // for VideoException
#include "OSDLPixel.h"       // for ColorDefinition, ColorMask, etc.
#include "OSDLPoint2D.h"     // for Point2D::Origin

#include "Ceylan.h"          // for Uint8, Uint32, inheritance




#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

	#include "SDL_opengl.h"      // for GLfloat
	#include "SDL.h"             // for ColorDefinition (SDL_Color)



namespace OSDL
{

	
	namespace Video
	{
		
			
		namespace OpenGL
		{
		
		
			/**
			 * OpenGL attribute.
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 */
			typedef SDL_GLattr GLAttribute ;	

		}
		
	}
	
	
}

	
#else // OSDL_USES_SDL



namespace OSDL
{

	
	namespace Video
	{
		
			
		namespace OpenGL
		{
		
		
			/**
			 * OpenGL attribute.
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 */
			typedef int GLAttribute ;	

		}
		
	}
	
}


	/*
	 * libnds used to define it already for the ARM9 in videoGL.h: 
	 * '#define GLfloat float'. Not true anymore.
	 *
	 */
	
	 
	#if defined(OSDL_ARCH_NINTENDO_DS) && OSDL_ARCH_NINTENDO_DS 

		#include "OSDLConfigForNintendoDS.h"
		
		/*
		 * Currently not using int instead of Ceylan::Float32 to (possibly)
		 * avoid adding a floating-point emulation library: it would require
		 * to eliminate many warnings (converting to 'GLLength' or
		 * 'GLCoordinate' from 'float').
		 *
		 */
		typedef Ceylan::Float32 GLfloat ;

	#endif // on OSDL_ARCH_NINTENDO_DS


#endif // OSDL_USES_SDL not available



#include <string>





namespace OSDL
{

	
	
	namespace Video
	{
	
	
	
		
		/**
		 * All OpenGL-specific operations and conventions are encapsulated 
		 * here, be they related to 2D or 3D.
		 *
		 * The OpenGL viewport is set so that it covers all of the 
		 * setMode-created surface, which must have been set with the OpenGL
		 * flag (directly and/or through the selection of an OpenGL flavour).
		 *
		 * @see http://osdl.sf.net/main/documentation/rendering/SDL-openGL.html
		 * for further implementation details.
		 *
		 */
		namespace OpenGL
		{


		
		
			/// Exception to be raised when OpenGL operations fail.
			class OSDL_DLL OpenGLException: public VideoException
			{
			
				public:
				
				
					/// Constructs a new OpenGLException.
					OpenGLException( const std::string & reason ) ;
					
					
					/// Virtual destructor.
					virtual ~OpenGLException() throw() ;
								
			} ;
			
			
			
		
			/**
			 * Mask used for OpenGL RGBA color specification for red, 
			 * depending on the endianness of the local machine.
			 *
			 */
			extern OSDL_DLL Pixels::ColorMask RedMask ;
			
			
			
			/**
			 * Mask used for OpenGL RGBA color specification for green,
			 * depending on the endianness of the local machine.
			 *
			 */
			extern OSDL_DLL Pixels::ColorMask GreenMask ;
			
			
			
			/**
			 * Mask used for OpenGL RGBA color specification for blue, 
			 * depending on the endianness of the local machine.
			 *
			 */
			extern OSDL_DLL Pixels::ColorMask BlueMask ;
			
			
			
			/**
			 * Mask used for OpenGL RGBA color specification for alpha,
			 * depending on the endianness of the local machine.
			 *
			 */
			extern OSDL_DLL Pixels::ColorMask AlphaMask ;
		
		
		
			/**
			 * Length, OpenGL unit of measure.
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 */
			typedef GLfloat GLLength ;
	
	
	
			/**
			 * Coordinate, OpenGL unit of measure. 
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 */
			typedef GLfloat GLCoordinate ;	
			
			
			
			/**
			 * Bit field for OpenGL selection. 
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 * The real GLbitfield is (usually) unsigned int.
			 *
			 */
			typedef unsigned int GLBitField ;	
			
			
			
			/**
			 * Enumeration for OpenGL selection. 
			 * GL prefix is kept to avoid namespace misuse.
			 *
			 * The real GLenum is (usually) unsigned int.
			 *
			 */
			typedef unsigned int GLEnumeration ;	
			


			/**
			 * Various flavours (presets) are available for OpenGL.
			 *
			 * The flavour must be selected before or during the call to
			 * setMode, and will not take effect until that call.
			 *
			 * Following flavours are available:
			 *   - None: no further action taken after basic OpenGL
			 * initialization
			 *   - OpenGLFor2D: dedicated for accelerated 2D with OpenGL
			 *   - OpenGLFor3D: classical full-blown 3D 
			 *   - Reload: designates the same settings as the ones that 
			 * were lastly used, just set them back in the state machine. 
			 * This is useful for example when a resize occurs: the 
			 * actual OpenGL context of the state machine is lost on some
			 * platforms, so the OSDL OpenGLContext object must set them 
			 * again during the setMode call.
			 *
			 * @note It results in loading the appropriate OpenGL settings 
			 * for the selected mode, including projection, model view 
			 * matrix, etc.
			 *
			 */
			enum Flavour { None, OpenGLFor2D, OpenGLFor3D, Reload } ;




			/**
			 * An OpenGL context corresponds to the state of a running 
			 * OpenGL screen. 
			 *
			 * It can be managed on a per-application basis, or a 
			 * per-window basis.
			 *
			 * OpenGL context objects have to store settings, so that 
			 * they can be reloaded when required, see OpenGL::Reload flavour.
			 *
			 * @note For the moment, direct access is used to the only one
			 * OpenGL context provided by the SDL 1.2.x back-end.
			 *
			 */
			class OSDL_DLL OpenGLContext: public Ceylan::TextDisplayable
			{

			
			
				public:
				
					
					
					/**
					 * Describes a projection mode:
					 *    - Orthographic: 
					 * http://en.wikipedia.org/wiki/Orthographic_projection
					 *    - Perspective:
					 * http://en.wikipedia.org/wiki/Perspective_(graphical)
					 *
					 */
					enum ProjectionMode { Orthographic, Perspective } ;
					
					
					
					/**
					 * Describes a shading model.
					 *
					 * Flat shading selects the computed color of just one
					 * vertex and assigns it to all the pixel fragments
					 * generated by rasterizing a single primitive. 
					 *
					 * Smooth shading, the default, causes the computed 
					 * colors of vertices to be interpolated as the 
					 * primitive is rasterized, typically assigning different
					 * colors to each resulting pixel fragment. 
					 * Also known as Gouraud shading.
					 *
					 * In either case, the computed color of a vertex is 
					 * the result of lighting, if lighting is enabled, or 
					 * it is the current color at the time the vertex
					 * was specified, if lighting is disabled.
					 *
					 */
					enum ShadingModel { Flat, Smooth } ;
					
					
					
					/**
					 * Describes which facets should be culled, i.e. 
					 * specifies whether front-facing or back-facing facets
					 * are culled, when facet culling is enabled.
					 *
					 * Facets include triangles, quadrilaterals, polygons, 
					 * and rectangles.
					 *
					 * @note If FrontAndBack is selected, then no facets 
					 * are drawn, but other primitives such as points and 
					 * lines are.
					 *
					 * Front and back are defined according to the normal
					 * vector of a facet, which can be determined according
					 * to the dot product of ordered listed vertices.
					 *
					 * @see FrontOrientation
					 *
					 */
					enum CulledFacet { Front, Back, FrontAndBack } ;
					 
					
					
					/**
					 * Determines how front-facing polygons are defined,
					 * depending on their winding.
					 *
					 * The projection of a polygon to window coordinates 
					 * is said to have clockwise winding if an imaginary 
					 * object following the path from its first vertex,
					 * its second vertex, and so on, to its last vertex, 
					 * and finally back to its first vertex, moves in a
					 * clockwise direction about the interior of the
					 * polygon.  
					 *
					 * On the contrary, the polygon's winding is said to be
					 * counterclockwise if the imaginary object following 
					 * the same path moves in a counterclockwise
					 * direction about the interior of the polygon.	
					 *
					 */
					enum FrontOrientation { Clockwise, CounterClockwise } ;
					
					
					  
					/**
					 * Creates a new blank OpenGL context, according to 
					 * the specified flavour and color depth.
					 *
					 * The default projection mode is orthographic.
					 *
					 * @param flavour the OpenGL flavour to use.
					 * 
					 * @param plannedBpp the color depth to use, in bits per
					 * pixel.
					 *
					 * @param viewportWidth the width of the viewport, in
					 * pixels.
					 * 
					 * @param viewportHeight the height of the viewport, in
					 * pixels.
					 * 
					 * @throw OpenGLException if the OpenGL state machine
					 * reports an error.
					 *
					 */
					OpenGLContext( OpenGL::Flavour flavour, 
						BitsPerPixel plannedBpp,
						Length viewportWidth,
						Length viewportHeight ) ;
		
		
						
					/// Virtual destructor.
					virtual ~OpenGLContext() throw() ;
				
				
				
					/**
					 * If OpenGL is to be used, allows to select a flavour,
					 * which provides convenient presets.
					 *
					 * @param flavour the selected flavour for OpenGL.
					 *
					 * @note OpenGL support cannot fully be encapsulated into 
					 * Video renderers, since they may not be used, and 
					 * OpenGL must be specified during the setMode phase.
					 *
					 * @see OpenGLFlavour
					 *
					 * @throw OpenGLException if the OpenGL state machine
					 * reports an error.
					 *
					 */
					virtual void selectFlavour( OpenGL::Flavour flavour ) ;
	
	
	
					/**
					 * Sets the OpenGL 2D flavour, i.e. the settings deemed
					 * the most appropriate for 2D rendering.
					 *
					 * Once called, all primitives can be rendered at integer
					 * positions.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 * @note Calling selectFlavour should be preferred, as it
					 * performs additionally a viewport update.
					 *
					 * @note Leaves the context in ModelView mode.
					 *
					 */
					virtual void set2DFlavour() ;
				
				
				
					/**
					 * Sets the OpenGL 3D flavour, i.e. the settings deemed
					 * the most appropriate for 3D rendering.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 * @note Calling selectFlavour should be preferred, as it
					 * performs additionally a viewport update.
					 *
					 * @note Leaves the context in ModelView mode.
					 *
					 */
					virtual void set3DFlavour() ;
				
				
					
					/**
					 * Blanks the current settings in the actual OpenGL 
					 * state machine. 
					 *
					 * @note It is useful to reset the OpenGL state before 
					 * applying a flavour.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					virtual void blank() ;
					 
				
				
					/**
					 * Requests the OpenGL context to reload the current
					 * settings in the actual OpenGL state machine. 
					 *
					 * It is notably useful for platforms that loose their
					 * OpenGL context under various circumstances, 
					 * including resizing.
					 *
					 * @see OpenGL::Reload
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					virtual void reload() ;
					
					
				
					/**
					 * Sets the OpenGL blending function.
					 *
					 * @param sourceFactor specifies which of the nine 
					 * methods is used to scale the source color components.
					 * 
					 * @param destinationFactor specifies which of the eight 
					 * methods is used to scale the destination color
					 * components.
					 * 
					 * @throw OpenGLException should an error occur.
					 *
					 */
					virtual void setBlendingFunction( 
						GLEnumeration sourceFactor,
						GLEnumeration destinationFactor ) ;

				
				
					/**
					 * Sets the shading technique to be used.
					 *
					 * @param newShadingModel the new shading technique 
					 * that shall be used.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					 virtual void setShadingModel( 
					 	ShadingModel newShadingModel = Smooth ) ;
	
	
					 
					/**
					 * Sets the cullings status, i.e. enable or disable 
					 * culling.
					 *
					 * @param newStatus culling is activated iff true.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */	
					virtual void setCullingStatus( bool newStatus ) ;
	
	
				
					/**
					 * Sets the culling settings, and enables it if requested.
					 *
					 * @param culledFacet tells whether front, or back, or 
					 * front and back facets should be culled.
					 *
					 * @param frontOrientation defines what are front 
					 * facing polygons, depending on their winding.
					 *
					 * @param autoEnable will specifically enable the use 
					 * of culling iff true (it will not be only set).
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */					 
					virtual void setCulling( CulledFacet culledFacet = Back, 
						FrontOrientation frontOrientation = CounterClockwise, 
						bool autoEnable = true ) ;
						
				
				
					/**
					 * Enables or disables the use of the depth buffer.
					 *
					 * @param newStatus if true, will enable the depth 
					 * buffer, if false will disable it.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					virtual void setDepthBufferStatus( bool newStatus ) ;
					




					// Viewport section.
					
					
					
					/**
					 * Sets the OpenGL viewport, i.e.determines the
					 * transformation of x and y from normalized device
					 * coordinates to window coordinates. 
					 *
					 * The recomputation of the projection is automatically
					 * triggered accordingly, to avoid distorted graphics: 
					 * it ensures that the viewport aspect ratio is the same
					 * as the one of the viewing volume.
					 *
					 * @param width the width of the viewport.
					 * 
					 * @param height the height of the viewport.
					 * 
					 * @param lowerLeftCorner the coordinates of the lower 
					 * left corner of the viewport rectangle in pixels. 
					 * The default is the origin, (0,0).
					 *
					 * @throw OpenGLException if either width or height 
					 * is negative, or if this method is called between 
					 * the execution of glBegin and the corresponding 
					 * execution of glEnd.
					 *
					 */					 
					virtual void setViewPort( Length width, Length height, 
						const TwoDimensional::Point2D & lowerLeftCorner = 
							TwoDimensional::Point2D::Origin ) ;
						
						
						
					/**
					 * Sets an orthographic projection, so that the viewing
					 * volume is a box, whose sides are parallel to the 
					 * main axes.
					 *
					 * The viewer is located at the origin, looking towards 
					 * the negative z values, his up vector being (0;1;0).
					 *
					 * The box will be set so that:
					 *   - the viewport ratio is preserved, to avoid distorted
					 * graphics
					 *   - the box is cut in two equal parts by the axes x=0 
					 * and y=0
					 *   - the box will lie between the two planes z=-near 
					 * and z=-far
					 *
					 * Therefore, for a box width of w, the box will range
					 * between -w/2 and w/2 on the x axis, and between 
					 * -w*r/2 and w*r/2 on the y axis, where r is the 
					 * viewport aspect ratio: 
					 * r = (viewport height) / (viewport width).
					 *
					 * @param width the length of the box edge which is 
					 * parallel to the x axis.
					 *
					 * @param near the z coordinate of the near clipping plane.
					 *
					 * @param far the z coordinate of the far clipping plane.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */	
					virtual void setOrthographicProjection( GLLength width, 
						GLCoordinate near = -DefaultNearClippingPlaneFor3D, 
						GLCoordinate far  = -DefaultFarClippingPlaneFor3D ) ;
						
						
						
					/**
					 * Sets an orthographic projection for 2D rendering, chosen
					 * to match the current OpenGL viewport size.
					 *
					 * @param width the viewport width.
					 *
					 * @param height the viewport height.
					 *
					 * @note Near and far clipping planes are set to default
					 * values for 2D.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */	
					virtual void setOrthographicProjectionFor2D( 
						GLLength width, GLLength height ) ;
						
			
			
					/**
					 * Sets the current clear color to the specified one.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					virtual void setClearColor( 
						const Video::Pixels::ColorDefinition & color ) ;
					
					
					
					/**
					 * Clears the viewport with the current clear color.
					 * 
					 * @note The depth buffer will not be modified here.
					 *
					 * @throw OpenGLException if this method is called 
					 * between the execution of glBegin and the 
					 * corresponding execution of glEnd.
					 *
					 */	
					virtual void clearViewport() ;
					
					
					
					/**
					 * Clears the depth buffer within the viewport.
					 * 
					 * @note The color buffer will not be modified here.
					 *
					 * @throw OpenGLException if this method is called 
					 * between the execution of glBegin and the 
					 * corresponding execution of glEnd.
					 *
					 */
					virtual void clearDepthBuffer() ;
					
					
					
					/**
					 * Pushes specified attribute field onto OpenGL attribute
					 * stack.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					virtual void pushAttribute( GLBitField attributeField ) ;
					
					
					
					/**
					 * Pops latest pushed attribute field from OpenGL attribute
					 * stack.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					virtual void popAttribute() ;
					
					 	
						
					/**
					 * Uploads specified surface as an OpenGL texture in 
					 * OpenGL context.
					 *
					 * @param source the surface that must be converted into
					 * texture. Its width and height must be a power of 2.
					 *
					 * It is no longer needed once the texture is loaded, 
					 * hence it can be deallocated by the caller.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 *
					Texture & uploadTextureFrom( const Video::Surface & source )
					*/
					
					
					
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
										
				
					
					/**
					 * Prepares the OpenGL attributes appropriate for the
					 * flavour that will be used in a VideoModule::setMode call.
					 *
					 * @param flavour the OpenGL flavour to aim at.
					 *
					 * @param safest tells whether we should use best settings
					 * (if true) or safest ones (if false), which are useful
					 * if the best just failed to be set properly. 
					 *
					 * @note The corresponding attributes will not be taken 
					 * into account until the next call to 
					 * VideoModule::setMode.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static void SetUpForFlavour( OpenGL::Flavour flavour,
						bool safest = false ) ;
						
						
						
					/**
					 * Enables specified feature in the OpenGL state machine.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static void EnableFeature( GLEnumeration feature ) ;
					
					
					
					/**
					 * Disables specified feature in the OpenGL state machine.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static void DisableFeature( GLEnumeration feature ) ;




					/*
					 * Following methods are static, as they are to be issued
					 * before a VideoModule::setMode call (otherwise they 
					 * will not be taken into account until the next setMode
					 * call), and no OpenGL context may already exist.
					 *
					 */
					 
					 
				
					/**
					 * Returns the current OpenGL double buffering status.
					 *
					 * @return true iff double buffering is used.
					 *
					 * @throw OpenGLException should an error occur.
					 *
					 */
					static bool GetDoubleBufferStatus() ;
							
									
				
					/**
					 * Sets the OpenGL double buffering status (enabled or
					 * disabled).
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @param newStatus, double buffering will be enabled
					 * iff true (and available).
					 * 
					 * @throw OpenGLException should an error occur.
					 *
					 */
					static void SetDoubleBufferStatus( bool newStatus ) ;
				
					 
					 
					/**
					 * Returns the size, in bits, of the OpenGL depth buffer.
					 *
					 * @throw OpenGLException should an error occur.
					 *
					 */
					static Ceylan::Uint8 GetDepthBufferSize() ;
									
				
					
					/**
					 * Sets the size, in bits, of the OpenGL depth buffer.
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @note Does not enable the depth buffer, 
					 * see setDepthBufferStatus.
					 *
					 * @param bitsNumber the new depth buffer size.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static void SetDepthBufferSize( 
						Ceylan::Uint8 bitsNumber = 16 ) ;
					 
					 
					 
					/**
					 * Returns whether the OpenGL fullscreen antialiasing
					 * (FSAA) is available.
					 *
					 * @note Requires the GL_ARB_MULTISAMPLE extension.
					 *
					 * @return 0 if the feature is not available, otherwise
					 * the non-null number of samples used 
					 * (ex: 4 means 4x antialiasing).
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static Ceylan::Uint8 GetFullScreenAntialiasingStatus() ;
						
						
						
					/**
					 * Sets the OpenGL fullscreen antialiasing (FSAA) 
					 * status (enabled or disabled).
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @note Requires the GL_ARB_MULTISAMPLE extension.
					 *
					 * @param newStatus, FSAA will be enabled iff true.
					 *
					 * @param samplesPerPixelNumber number of samples 
					 * per pixel when multisampling (FSAA) is enabled.
					 * Is usually 2, 4, 16, etc. This parameter is
					 * ignored if newStatus is false.
					 * 
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static void SetFullScreenAntialiasingStatus( 
						bool newStatus,
						Ceylan::Uint8 samplesPerPixelNumber = 4 ) ;
					
					 
					 
					/**
					 * Returns whether the OpenGL hardware-acceleration is
					 * available.
					 *
					 * @return true iff the hardware acceleration is 
					 * available.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static bool GetHardwareAccelerationStatus() ;
						
						
						
					/**
					 * Sets the OpenGL hardware-acceleration status 
					 * (enabled or disabled).
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @note Requires the GL_ARB_MULTISAMPLE extension.
					 *
					 * @param newStatus, FSAA will be enabled iff true.
					 *
					 * @param samplesPerPixelNumber number of samples 
					 * per pixel when multisampling (FSAA) is enabled.
					 * 
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static void SetHardwareAccelerationStatus( 
						bool newStatus ) ;
					
					
					
					/**
					 * Returns whether the OpenGL synchronization with
					 * vertical blank retrace (VSYNC) is activated.
					 *
					 * @return true iff OpenGL synchronizes with VSYNC.
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static bool GetVerticalBlankSynchronizationStatus() ;
						
						
						
					/**
					 * Sets the OpenGL synchronization with vertical blank
					 * retrace (VSYNC).
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @param newStatus vsync synchronization will be 
					 * enabled iff true (and available).
					 *
				 	 * @throw OpenGLException should an error occur.
					 *
					 */
					static void SetVerticalBlankSynchronizationStatus( 
						bool newStatus ) ;
					
					
					
					/**
					 * Tries to set the OpenGL synchronization with vertical
					 * blank retrace (VSYNC).
					 *
					 * @note Any set attribute does not take effect until
					 * VideoModule::setMode is called.
					 *
					 * @param attribute the attribute to set.
					 *
					 * @param value the value to assign.
					 *
					 * @return true iff this feature could be set on this
					 * platform; if it failed, no exception is thrown, only  
					 * false is returned.
					 *
					 */				 	
					static bool TrySettingVerticalBlankSynchronizationStatus(
						bool newStatus ) ;
						
						
											
					/**
					 * Gets the OpenGL color depth for each color component.
					 *
					 * @param redSize the variable where the size in bits 
					 * of the red component will be written.
					 * 
					 * @param greenSize the variable where the size in bits
					 * of the green component will be written.
					 *
					 * @param blueSize the variable where the size in bits
					 * of the blue component will be written.
					 *
					 * @param alphaSize the variable where the size in bits
					 * of the alpha component will be written.
					 *
					 * @return the actual overall bits per pixel value.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					 static OSDL::Video::BitsPerPixel GetColorDepth( 
					 	OSDL::Video::BitsPerPixel & redSize, 
					 	OSDL::Video::BitsPerPixel & greenSize, 
						OSDL::Video::BitsPerPixel & blueSize, 
						OSDL::Video::BitsPerPixel & alphaSize ) ;
				 
				 
				 
					/**
					 * Sets the OpenGL color depth.
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @param plannedBpp the planned color depth, in bits
					 * per pixel.
					 * 
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static void SetColorDepth( 
						OSDL::Video::BitsPerPixel plannedBpp ) ;
				
				
				 
					/**
					 * Sets the OpenGL color depth.
					 *
					 * @note Will not take effect until the next call
					 * to VideoModule::setMode.
					 *
					 * @param redSize the size of the red component, in 
					 * bits per pixel.
					 * 
					 * @param greenSize the size of the green component, 
					 * in bits per pixel.
					 * 
					 * @param blueSize the size of the blue component, 
					 * in bits per pixel.
					 * 
					 * @note Maybe alpha should be added.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static void SetColorDepth( 
						OSDL::Video::BitsPerPixel redSize, 
						OSDL::Video::BitsPerPixel greenSize, 
						OSDL::Video::BitsPerPixel blueSize ) ;
				
				
				
					/**
					 * Returns a textual description of the availability of 
					 * the OpenGL features.
					 *
					 * @note The description is only useful after a 
					 * VideoModule::setMode call.
					 *  
					 * @throw OpenGLException if the operation failed.
					 *
					 */
					static std::string InterpretFeatureAvailability() ;
				
				
				
		            /**
	    	         * Returns an user-friendly description of the 
					 * specified OpenGL flavour.
	        	     *
					 * @param flavour the flavour to describe.
					 *
					 */
					static std::string ToString( OpenGL::Flavour flavour ) ;	





					// OpenGL context behaviour.



					/**
					 * Tells whether the OpenGL context can be lost (and 
					 * therefore must be reloaded) under certain unexpected
					 * circumstances (ex: window resize, going to fullscreen,
					 * switching to another application, etc.).
					 *
					 * @see OpenGL::Flavour, for Reload.
					 *
					 */
					static const bool ContextCanBeLost ;
			
			
			
					/**
					 * Tells whether the OpenGL context is lost (and 
					 * therefore must be reloaded) on resize.
					 *
					 * @see OpenGL::Flavour, for Reload.
					 *
					 */
					static const bool ContextIsLostOnResize ;
			

			
					/**
					 * Tells whether the OpenGL context is lost (and 
					 * therefore must be reloaded) when switching application.
					 *
					 * @see OpenGL::Flavour, for Reload.
					 *
					 */
					static const bool ContextIsLostOnApplicationSwitch ;
			
			
			
					/**
					 * Tells whether the OpenGL context is lost (and 
					 * therefore must be reloaded) when changing the color
					 * depth.
					 *
					 * @see OpenGL::Flavour, for Reload.
					 *
					 */
					static const bool ContextIsLostOnColorDepthChange ;
			


			
					/**
					 * Default width of the orthographic box is 1000
					 * (1000.0f).
					 *
					 */
					static const GLLength DefaultOrthographicWidth ;
					
					
					
					/**
					 * Coordinate, along the -z axis, of the default near
					 * clipping plane in 2D is -1 (-1.0f).
					 *
					 */
					static const GLCoordinate DefaultNearClippingPlaneFor2D ;
					
					
					
					/**
					 * Coordinate, along the -z axis, of the default far
					 * clipping plane in 2D is 1 (1.0f).
					 *
					 */
					static const GLCoordinate DefaultFarClippingPlaneFor2D ;
					
					
					
					/**
					 * Coordinate, along the -z axis, of the default near
					 * clipping plane in 3D is 1 (1.0f).
					 *
					 */
					static const GLCoordinate DefaultNearClippingPlaneFor3D ;
					
					
					
					/**
					 * Coordinate, along the -z axis, of the default far
					 * clipping plane in 3D is 100000 (100000.0f).
					 *
					 */
					static const GLCoordinate DefaultFarClippingPlaneFor3D ;
					
					
					
					
					
			protected:
			
			
			
					/**
					 * Updates the current projection so that its aspect 
					 * ratio matches the one of the viewport.
					 *
					 * @throw OpenGLException if the current projection 
					 * mode cannot be handled.
					 *
					 */
					virtual void updateProjection() ;
					
					

					/**
					 * Returns whether the specified OpenGL attribute is 
					 * supported.
					 *
					 * If not (i.e. if false is returned), then calling
					 * the GetGLAttribute function would throw an exception.
					 *
					 * @param attribute the attribute whose support is to be
					 * acknowledged.
					 *
					 * @return true iff the specified attribute is supported.
					 *
					 */				 	
					static bool HasGLAttribute( GLAttribute attribute ) ;
					
					
			
					/**
					 * Gets the value of specified OpenGL attribute.
					 *
					 * @param attribute the attribute whose value is wanted.
					 *
					 * @return the attribute value.
					 *
					 * @throw OpenGLException if the operation failed, including
					 * if on this platform the specified attribute is not
					 * supported.
					 * 
					 */				 	
					static int GetGLAttribute( GLAttribute attribute ) ;
					
					
					
					/**
					 * Sets the specified OpenGL attribute.
					 *
					 * @note the set attributes do not take effect until
					 * VideoModule::setMode is called.
					 *
					 * @param attribute the attribute to set.
					 *
					 * @param value the value to assign.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */				 	
					static void SetGLAttribute( GLAttribute attribute,
						int value ) ;
									
					
					
					/**
					 * Tries to set the specified OpenGL attribute.
					 *
					 * @note Any set attribute does not take effect until
					 * VideoModule::setMode is called.
					 *
					 * @param attribute the attribute to set.
					 *
					 * @param value the value to assign.
					 *
					 * @return true iff this attribute could be set on this
					 * platform; if it failed, no exception is thrown, only  
					 * false is returned.
					 *
					 */				 	
					static bool TrySettingGLAttribute( GLAttribute attribute,
						int value ) ;
									
					
					
					/**
					 * Returns a string describing the specified OpenGL
					 * attribute.
					 *
					 * @param attribute the attribute to describe.
					 *
					 * @throw OpenGLException if the operation failed.
					 *
					 */				 	
					static std::string GLAttributeToString( 
						GLAttribute attribute ) ;
									
					
					
					
					
					// Internal OpenGL context state.
					
					
					
					///	Stores the current OpenGL flavour.		
					OpenGL::Flavour _flavour ;
							
										 	
													
					/// Size in bits of the red component.
					OSDL::Video::BitsPerPixel _redSize ; 
					
					
					
					/// Size in bits of the green component.
					OSDL::Video::BitsPerPixel _greenSize ;
					
					
					
					/// Size in bits of the blue component.
					OSDL::Video::BitsPerPixel _blueSize ;



					/// Size in bits of the alpha component.
					OSDL::Video::BitsPerPixel _alphaSize ;


					
					/**
					 * Width of the viewport, useful to ensure aspect ratio 
					 * is well managed.
					 *
					 */
					Length _viewportWidth ;
					
					
					
					/**
					 * Height of the viewport, useful to ensure aspect ratio 
					 * is well managed.
					 *
					 */
					Length _viewportHeight ;
					
					
					
					/// Stores the current projection mode.
					ProjectionMode _projectionMode ;
					
					
					
					/**
					 * Width of the projection volume, so that projection 
					 * can be automatically updated when the viewport area
					 * changes.
					 *
					 */
					GLLength _projectionWidth ;
					
					
					
					/**
					 * The current z axis coordinate of the near clipping 
					 * plane.
					 *
					 */
					GLCoordinate _nearClippingPlane ;
					
					
					
					/// The current z axis coordinate of the far clipping plane.
					GLCoordinate _farClippingPlane ;
					
					
					
					/**
					 * The current clear color for the viewport.
					 *
					 * @note Default is pure black.
					 *
					 */
					Pixels::ColorDefinition _clearColor ; 
					
					
					
					
			private:
				
						
									
					/**
					 * Copy constructor made private to ensure that it 
					 * will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined constructor is called, implicitly or not.
					 * 
					 */			 
					OpenGLContext( const OpenGLContext & source ) ;
			
			
			
					/**
					 * Assignment operator made private to ensure that it 
					 * will be never called.
					 *
					 * The compiler should complain whenever this 
					 * undefined operator is called, implicitly or not.
					 *
					 */			 
					OpenGLContext & operator = ( const OpenGLContext & source );
			
			
			} ;
		
		
		
		
			/**
			 * Gathers appropriate GLU (OpenGL Utility Library) encapsulations
			 * for better integration into OSDL.
			 *
			 * Some of them are defined here just to use GLU conventions 
			 * (ex: GLUint) without needing to include GLU headers, that may
			 * or may not be available.
			 *
			 * This is a form of code duplication, but GLU conventions are not
			 * expected to change often, and the other solutions are worse:
			 * hiding actual datatypes behind indirection pointers (would need
			 * a lot of code for that and would decrease runtime performance) 
			 * or including GLU headers (would require to include as well
			 * OSDLConfig.h in OSDL headers, and then would force to 
			 * install OSDLConfig.h, which is not wished).
			 *
			 */ 
			namespace GLU
			{
			
				/// GLU::Int (actually GLUint) is unsigned int.
				typedef Ceylan::Uint32 Int ;
			
			}
			
			
						
		}
		
	}
	
}



#endif // OSDL_OPENGL_H_

