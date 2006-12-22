#ifndef OSDL_VIDEO_TYPES_H_
#define OSDL_VIDEO_TYPES_H_


#include "OSDLException.h"   // for OSDL::Exception 

#include "Ceylan.h"          // for the various basic types


/**
 * Definition of the main OSDL video types.
 *
 * As much as possible, consider using strong typing.
 *
 * @note These units are not chosen so that they are accurate. Their purpose is to efficiently
 * fulfill most multimedia needs.
 *
 *
 */
namespace OSDL
{


	namespace Video
	{
	
	
	
		/// Length, unit of measure, unsigned.
		typedef Ceylan::Uint16 Length ;
	
		/// Length, unit of measure, signed.
		typedef Ceylan::Sint16 SignedLength ;
	
	
		/// Pixel pitch, unit of measure.
		typedef Ceylan::Uint16 Pitch ;


		/**
		 * Coordinate, unit of measure, integer, signed.
		 *
		 * @note Not having 'Coordinate' set to 'Ceylan::Sint16' would break functions 
		 * using SDL_gfx (example : vertex array elements are expected to be 'Ceylan::Sint16').
		 *
		 */
		typedef Ceylan::Sint16 Coordinate ;
		
		
		/// Offset, difference between coordinates.
		typedef Coordinate Offset ;

		/// Coordinate unit of measure, floating-point.
		typedef float FloatingPointCoordinate ;


		/// Color depth, bits per pixel.
		typedef Ceylan::Uint8 BitsPerPixel ;
	
		/// Color depth, bytes per pixel.
		typedef Ceylan::Uint8 BytesPerPixel ;
		
		/// Angle unit of measure, in degrees, should be in range ]-180;180] (signed).
		// Disabled not to mix with Ceylan angle : typedef Ceylan::Sint16 Angle ;
		
		/// Ratio (percentage) unit, should store floating point values between 0 and 1.
		typedef float Ratio ;


		/// Mother class for all video exceptions. 		
		class VideoException: public OSDL::Exception 
		{ 
			public:
				
				explicit VideoException( const std::string & reason ) throw() : 
					OSDL::Exception( reason )
				{

				}


				virtual ~VideoException() throw()
				{
				
				}
				
		} ;		
	
	}
	
}

#endif // OSDL_VIDEO_TYPES_H_
