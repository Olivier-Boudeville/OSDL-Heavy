#ifndef OSDL_OVERLAY_H_
#define OSDL_OVERLAY_H_

#include "OSDLVideoTypes.h"      // for VideoException
#include "Ceylan.h"              // for inheritance

#include <string>


// Forward declaration.
struct SDL_Overlay ;


namespace OSDL
{


	namespace Video
	{
	
	
	
		/// Thrown when an error regarding overlays occured.
		class OverlayException : public VideoException
		{
		
			public:
			
				OverlayException( const std::string & message ) throw() ;				
				virtual ~OverlayException() throw() ;
			
		} ;
	
	
	
		/**
		 * An overlay is an image, encoded in a specific format, that can be blitted onto a Surface.
		 * 
		 * The encoding is based on YUV, which stands for : Y=Luminance, U=Normalised BY,
		 * V=Normalised RY. This is a colour model that describes colour information in terms of
		 * luminance (Y) and chrominance (U, V).
		 *
		 * YUV can be seen as transformed RGB values such that there is low correlation between 
		 * the three coordinates.
		 *
		 * This is the colour space representation used in PAL video and television transmission.
		 *
		 * @note The term 'overlay' is a misnomer since, unless the overlay is created in hardware,
		 * the contents for the display surface underneath the area where the overlay is shown will
		 * be overwritten when the overlay is displayed.
		 *
		 *
		 */
		class Overlay : public Ceylan::Lockable
		{
		
		
			public:
			
			
				/**
				 * Describes the encoding format used by an overlay.
				 *
				 * Planar modes :
				 * - YV12 : Y + V + U
				 * - IYUV : Y + U + V
				 *
				 * Packed modes :
				 * - YUY2 : Y0 + U0 + Y1 + V0
				 * - UYVY : U0 + Y0 + V0 + Y1
				 * - YVYU : Y0 + V0 + Y1 + U0
				 *
				 */
				enum EncodingFormat { YV12, IYUV, YUY2,UYVY, YVYU } ;
			
			
				/**
				 * Creates a YUV overlay of the specified width, height and format, linked with the
				 * current screen surface.
				 *
				 * @param width the width of this new overlay.
				 *
				 * @param height the height of this new overlay.
				 *
				 * @param format the encoding format for the pixels of this overlay.
				 *
				 * @throw OverlayException if video module could not be retrieved or if the overlay
				 * creation failed.
				 *
				 */
				Overlay( Length width, Length height, EncodingFormat format ) 
					throw( OverlayException ) ;
				
			
				/// Virtual destructor.
				virtual ~Overlay() throw() ;
			

				/**
				 * Blit the overlay to the display.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual bool blit() const throw( OverlayException ) ;

			
				/**
				 * Blits the overlay to the display, at the specified location, at its original
				 * size.
				 *
				 * @param x abscissa of the destination surface where this surface will be
				 * blitted.
				 *
				 * @param y ordinate of the destination surface where this surface will be
				 * blitted.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual bool blit( Coordinate x, Coordinate y ) const throw( OverlayException ) ;
			
			
			
			
				/**
				 * Blit the overlay to the display.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual bool blit() const throw( OverlayException ) ;
			
			
				/**
				 * Blit the overlay to the display.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual bool blit() const throw( OverlayException ) ;
			
			
			
				// Lockable section.
	
	
				/**
				 * Tells whether this overlay has to be locked before modifying
				 * it. When using a overlay, in general there is no need to use this 
				 * method because the lock/unlock methods take care of that.
				 *
				 * @see Ceylan::Lockable.
				 *
				 * This method cannot be inlined since it is inherited.
				 *
				 */				
				 virtual bool mustBeLocked() const throw() ;
		
	
				/**
				 * Effective unlocking of the overlay.
				 * Nevertheless, only lock/unlock pair should be used by user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void preUnlock() throw() ;
				
				
				/**
				 * Effective locking of the overlay.
				 * Nevertheless, only lock/unlock pair should be used by user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void postLock() throw() ;
			
			
	            /**
	             * Returns an user-friendly description of the state of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( Ceylan::VerbosityLevels level = Ceylan::high ) 
					const throw() ;


			
			protected:
			
			
				/// The inner back-end overlay, if any.
				SDL_Overlay * _overlay ;
			
			
				/// The original width of the overlay.
				Length _width ;

				
				/// The original height of the overlay.
				Length _height ;
				
				
				
			
			private:
		
		
				/**
				 * Copy constructor made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined constructor is called, 
				 * implicitly or not.
				 * 
				 */			 
				Overlay( const Overlay & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it will be never called.
				 * The compiler should complain whenever this undefined operator is called, 
				 * implicitly or not.
				 * 
				 */			 
				Overlay & operator = ( const Overlay & source ) throw() ;
				
		
		
		
		} ;	
	
	
	}


}



#endif // OSDL_OVERLAY_H_
