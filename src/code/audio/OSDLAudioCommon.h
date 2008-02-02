#ifndef OSDL_AUDIO_COMMON_H_
#define OSDL_AUDIO_COMMON_H_


#include "OSDLException.h"   // for OSDL::Exception 



namespace OSDL
{


	namespace Audio 
	{
	
	

		/// Mother class for all audio exceptions. 		
		class OSDL_DLL AudioException: public OSDL::Exception 
		{ 
		
			public: 
			
			
				AudioException( const std::string & reason ) throw():
					OSDL::Exception( reason )
				{
				
				} ; 
				
				
				virtual ~AudioException() throw()
				{
				
				}
				
		} ;

			
		/// For buffer sizes, in bytes (prefer full 32-bit words, faster).
		typedef Ceylan::Uint32 BufferSize ;


		/**
		 * Describes the encoding (format) of a given sample.
		 * A sample has a size (8-bit or 16-bit generally), is signed or not,
		 * and, if using more than one byte, an endianness (little or big).
		 *
		 * @example: Uint8SampleFormat, LittleSint16SampleFormat, etc.
		 *
		 */
		typedef Ceylan::Uint16 SampleFormat ;
	
		
		/**
		 * Specifies the channel organization of an audio output.
		 * 
		 * It includes the number of channels (ex: mono, stereo) and the role
		 * of each channel.
		 *
		 * @example: Mono, Stereo.
		 *
		 * @note This has nothing to do with mixing (input) channels.
		 *
		 */
		typedef Ceylan::Uint16 ChannelFormat ;

		
		/**
		 * Describe a number of channel, either for input (mixing channels) or
		 * for output (playback channels of a given channel format).
		 *
		 * @example: there can be 16 input mixing channels. Regarding output,
		 * for mono sound, there is 1 playback channel, and for stereo, 2.
		 *
		 */
		typedef Ceylan::Uint16 ChannelNumber ;
	
	
		/**
		 * Describes the size, in bytes, of a sample chunk.
		 *
		 */
		typedef Ceylan::Uint32 ChunkSize ;
		
		
		/**
		 * Describes the distance between an audio source and the listener.
		 *
		 * Ranges from 0 (closest possible) to 255 (far).
		 *
		 */
		typedef Ceylan::Uint8 ListenerDistance ;
		
		
		/**
		 * Describes the direction, the angle between the listener and an
		 * audio source, from 0 to 360 degrees.
		 *
		 * It goes clockwise, starting at: directly in front. 
		 *
		 * So an angle of 0 means that the source is directly in front of the
		 * listener, 90 means the source is directly to his right, 180 directly
		 * behind him, 270 directly to its left.
		 *
		 */
		typedef Ceylan::Sint16 ListenerAngle ;
		

		/**
		 * Describes the volume for the playback of an audible.
		 *
		 * Ranges from MinVolume (0) to MaxVolume (128).
		 *
		 */
		typedef Ceylan::Uint8 Volume ;


		/// Describes a fading status.	
		enum FadingStatus
		{
		
			In,
			Out,
			None
			
		} ;
		
				
	}	
	
}	


#endif // OSDL_AUDIO_COMMON_H_

