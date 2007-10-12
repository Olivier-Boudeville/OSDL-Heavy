#ifndef OSDL_OVERLAY_H_
#define OSDL_OVERLAY_H_


#include "OSDLVideoTypes.h"      // for VideoException

#include "Ceylan.h"              // for inheritance

#include <string>



#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

// No need to include SDL header here:
struct SDL_Overlay ;

#endif //  ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 


namespace OSDL
{


	namespace Video
	{
	
		
		
		/// Low-level overlay being used.
#if ! defined(OSDL_USES_SDL) || OSDL_USES_SDL 

		typedef ::SDL_Overlay LowLevelOverlay ;
		
#else // OSDL_USES_SDL	

		struct LowLevelOverlay {} ;

#endif // OSDL_USES_SDL


	
		/// Thrown when an error regarding overlays occured.
		class OSDL_DLL OverlayException: public VideoException
		{
		
			public:
			
				OverlayException( const std::string & message ) throw() ;		
				virtual ~OverlayException() throw() ;
			
		} ;
	
	
	
	
		/**
		 * An overlay is an image, encoded in a specific format, that can 
		 * be blitted onto a Surface.
		 * 
		 * The encoding is based on YUV, which stands for: 
		 *  - Y=Luminance
		 *  - U=Normalised BY
		 *  - V=Normalised RY. 
		 * This is a colour model that describes colour information in 
		 * terms of luminance (Y) and chrominance (U, V).
		 *
		 * YUV can be seen as transformed RGB values such that there 
		 * is low correlation between the three coordinates.
		 *
		 * This is the colour space representation used in PAL video 
		 * and television transmission.
		 *
		 * @note The term 'overlay' is a misnomer since, unless the 
		 * overlay is created in hardware, the contents for the display 
		 * surface underneath the area where the overlay is shown will
		 * be overwritten when the overlay is displayed.
		 *
		 */
		class OSDL_DLL Overlay: public Ceylan::Lockable
		{
		
		
			public:
			
			
				/**
				 * Describes the encoding format used by an overlay.
				 *
				 * Planar modes:
				 * - YV12: Y + V + U
				 * - IYUV: Y + U + V
				 *
				 * Packed modes:
				 * - YUY2: Y0 + U0 + Y1 + V0
				 * - UYVY: U0 + Y0 + V0 + Y1
				 * - YVYU: Y0 + V0 + Y1 + U0
				 *
				 */
				enum EncodingFormat { YV12, IYUV, YUY2,UYVY, YVYU } ;
			
			
				/**
				 * Creates a YUV overlay of the specified width, height 
				 * and format, linked with the current screen surface.
				 *
				 * @param width the width of this new overlay.
				 *
				 * @param height the height of this new overlay.
				 *
				 * @param format the encoding format for the pixels 
				 * of this overlay.
				 *
				 * @throw OverlayException if video module could not 
				 * be retrieved or if the overlay creation failed.
				 *
				 * @see EncodingFormat
				 *
				 */
				Overlay( Length width, Length height, EncodingFormat format ) 
					throw( OverlayException ) ;
				
			
				/// Virtual destructor.
				virtual ~Overlay() throw() ;
			

				/**
				 * Blits the overlay to the display, at the specified 
				 * location, at its original size.
				 *
				 * @param x abscissa of the destination surface where 
				 * this overlay will be blitted.
				 *
				 * @param y ordinate of the destination surface where 
				 * this overlay will be blitted.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual void blit( Coordinate x, Coordinate y ) const 
					throw( OverlayException ) ;			
			
			
				/**
				 * Blits the overlay to the display.
				 *
				 * @throw OverlayException if the operation failed.
				 *
				 */
				virtual void blit() const throw( OverlayException ) ;

					
				/**
				 * Tells whether this overlay has to be locked before 
				 * modifying it. 
				 * 
				 * When using a overlay, in general there is no need to 
				 * use this method because the lock/unlock methods take 
				 * care of that.
				 *
				 * @see Ceylan::Lockable.
				 *
				 * This method cannot be inlined since it is inherited.
				 *
				 */				
				 virtual bool mustBeLocked() const throw() ;
		
	
			
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


			
			protected:
			
			
				/**
				 * Effective unlocking of the overlay.
				 *
				 * Nevertheless only lock/unlock pair should be called 
				 * by user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void preUnlock() throw() ;
				
				
				/**
				 * Effective locking of the overlay.
				 *
				 * Nevertheless only lock/unlock pair should be called by 
				 * user programs.
				 *
				 * @see Ceylan::Lockable
				 *
				 */
				virtual void postLock() throw() ;
			
			
				/// The inner back-end overlay, if any.
				LowLevelOverlay * _overlay ;
			
			
				/// The original width of the overlay.
				Length _width ;

				
				/// The original height of the overlay.
				Length _height ;
				
				
				
			
			private:
		
		
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				Overlay( const Overlay & source ) throw() ;
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				Overlay & operator = ( const Overlay & source ) throw() ;
				
		
		
		
		} ;	
	
	
	}


}



#endif // OSDL_OVERLAY_H_

