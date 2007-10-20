#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan::Log ;



/**
 * Testing the basic services of the OSDL audio module.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL audio basic services." ) ;


		bool isBatch = false ;
		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
						
			if ( token == "--batch" )
			{
			
				LogPlug::info( "Batch mode selected" ) ;
				isBatch = true ;
				tokenEaten = true ;
			}
			
			if ( token == "--interactive" )
			{
				LogPlug::info( "Interactive mode selected" ) ;
				isBatch = false ;
				tokenEaten = true ;
			}
			
			if ( token == "--online" )
			{
				// Ignored:
				tokenEaten = true ;
			}
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
				throw Ceylan::CommandLineParseException( 
					"Unexpected command line argument: " + token ) ;
			}
		
		}

			
		LogPlug::info( "Starting OSDL with audio enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( CommonModule::UseAudio ) ;		
			
		myOSDL.logState() ;
					
		
		LogPlug::info( "Getting audio module." ) ;
		AudioModule & myAudio = myOSDL.getAudioModule() ; 
		
		myAudio.logState() ;
			
						
		LogPlug::info( "Displaying audio driver name: "
			+ myAudio.getDriverName() + "." ) ;
		
		Ceylan::Maths::Hertz outputFrequency = 22050 ;
		
		SampleFormat outputSampleFormat = 
			AudioModule::NativeSint16SampleFormat ;
			
		ChannelFormat outputChannel = AudioModule::Stereo ;
		
		ChunkSize chunkSize = 4096 ;
		
		LogPlug::info( "Requesting a mixing mode at " 
			+ Ceylan::toString( outputFrequency ) + " Hz, with sample format " 
			+ sampleFormatToString( outputSampleFormat ) 
			+ ", channel format " 
			+ channelFormatToString( outputChannel ) 
			+ " and a chunk size of " + Ceylan::toString( chunkSize )
			+ " bytes." ) ;
			
			
		myAudio.setMode( outputFrequency, 
			/* sample format */ AudioModule::NativeSint16SampleFormat,
			outputChannel, chunkSize ) ;
		
		myAudio.logState() ;

		ChannelNumber count ;
		
		Ceylan::System::Millisecond latency = myAudio.getObtainedMode(
			outputFrequency, outputSampleFormat, count ) ;
			
		LogPlug::info( "Obtained a mixing mode at " 
			+ Ceylan::toString( outputFrequency ) + " Hz, with sample format " 
			+ sampleFormatToString( outputSampleFormat ) + " and " 
			+ Ceylan::toString( count ) 
			+ " channel(s), which results in a mean theoritical latency of " 
			+ Ceylan::toString( latency ) + " milliseconds." ) ;
					
		
		/*
		if ( ! isBatch )
			myEvents.waitForAnyKey() ;
		*/
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL audio test." ) ;
		
	}
		
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught: "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught: " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

    return Ceylan::ExitSuccess ;

}

