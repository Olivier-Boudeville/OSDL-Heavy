#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " [ -f frequency ] [ -m mode ] [ -t {cbr|vbr} ] -u BYTE_COUNT X.mp3\nConverts a .mp3 file into a .osdl.music file by appending an header filled with informations specified from the command-line."
	"\n\t -f: specifies the output sampling frequency, in Hz, ex: -f 22050 (the default)" 
	"\n\t -m: specifies the output mode, mono or stereo, ex: -m mono (the default)" 
	"\n\t -t: specifies the bitrate type, either constant bit rate (CBR) or variable bitrate (VBR) (the default)" 
	"\n\t -u: specifies the upper bound of the size of an encoded mp3 frame in this music, in bytes." 
	"\nOne may use the wavToMP3ForDS.sh script, or directly the lame command-line tool, to convert beforehand a .wav into a .mp3."
	"\n\tEx: 'lame YourLongMusic.wav --verbose -m m --vbr-new -V 5 -q 0 -B 112 -t --resample 22.05 YourLongMusic.mp3' converts the music." 
	"\nThen 'mp3ToOSDLMusic.exe -f 22050 -m mono -t vbr -u 314 YourLongMusic.mp3' results in the creation of 'YourLongMusic.osdl.music'" 
	"\nThe upper bound of the size of an encoded mp3 frame can be determined thanks to the getMP3Settings OSDL media tool. It runs on the DS, plays the mp3 and, once done, displays that upper bound."
	;



std::string getUsage( const std::string & execName ) throw()
{

	return "Usage: " + execName + Usage ;

}

	

int main( int argc, char * argv[] ) 
{

	// Default frequency, in Hz:
	Ceylan::Maths::Hertz frequency = 22050 ;
	
	string modeName = "mono" ;
	ChannelFormat mode = AudioModule::Mono ;
	
	BitrateType bitrateType = VBR ;
	
	// The upperbound 
	Ceylan::Sint32 upperBound = -1 ;
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Testing OSDL events basic services." ) ;

		
		std::string executableName ;
		std::list<std::string> options ;
		
		Ceylan::parseCommandLineOptions( executableName, options, argc, argv ) ;
		
		std::string token ;
		bool tokenEaten ;
		
		string inputFilename ;
		
		while ( ! options.empty() )
		{
		
			token = options.front() ;
			options.pop_front() ;

			tokenEaten = false ;
					
						
			if ( token == "-f" )
			{
			
				frequency = static_cast<Ceylan::Maths::Hertz>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
					
				options.pop_front() ;
				
				LogPlug::info( "Frequency set to "
					+ Ceylan::toString( frequency ) + " Hz." ) ;
					
				tokenEaten = true ;
				
			}
			
			
			if ( token == "-m" )
			{
			
				modeName = options.front() ;
				options.pop_front() ;
				
				if ( modeName == "mono" )
					mode = AudioModule::Mono ;
				else if ( modeName == "stereo" )
					mode = AudioModule::Stereo ;
				else
				{
					
					cerr << "Error, unexpected mode specified: '" + token 
						+ "'.\n" + getUsage( argv[0] ) << endl ;
					
					exit( 2 ) ;	
				
				}						
					
				LogPlug::info( "Mode set to " + Ceylan::toString( mode ) ) ;
					
				tokenEaten = true ;
				
			}
			
			
			if ( token == "-t" )
			{
			
				string type = options.front() ;
				options.pop_front() ;
				
				if ( type == "vbr" )
					bitrateType = VBR ;
				else if ( type == "cbr" )
					bitrateType = CBR ;
				else
				{
					
					cerr << "Error, unexpected bitrate type specified: '" 
						+ token 
						+ "'.\n" + getUsage( argv[0] ) << endl ;
					
					exit( 3 ) ;	
				
				}	
					
				LogPlug::info( "Bitrate type is " 
					+ Ceylan::toNumericalString( bitrateType ) + "(" 
					+ type + ")." ) ;
					
				tokenEaten = true ;
				
			}
			
			
			if ( token == "-u" )
			{
			
				upperBound = static_cast<Ceylan::Sint32>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
					
				options.pop_front() ;
				
				LogPlug::info( "Upper bound set to "
					+ Ceylan::toString( upperBound ) + " bytes." ) ;
					
				tokenEaten = true ;

			}
			
			
			if ( LogHolder::IsAKnownPlugOption( token ) )
			{
				// Ignores log-related (argument-less) options.
				tokenEaten = true ;
			}
			
			
			if ( ! tokenEaten )
			{
			
				if ( options.empty() )
				{	

					inputFilename = token ;

				}
				else
				{	
					
					cerr << "Unexpected command line argument: '" + token 
						+ "'.\n" + getUsage( argv[0] ) << endl ;
					exit( 1 ) ;	
				
				}
					
			}
		
		
		} // while
		
		
		if ( inputFilename.empty() )
		{
			
			cerr << "Error, no input file specified.\n" 
				+ getUsage( argv[0] ) << endl ;
			exit( 4 ) ;	
			
		}	
		
		if ( ! File::ExistsAsFileOrSymbolicLink( inputFilename ) )
		{
			
			cerr << "Error, input file '" << inputFilename << "' not found.\n" 
				+ getUsage( argv[0] ) << endl ;
			exit( 5 ) ;	
			
		}	

		if ( upperBound < 80 )
		{
			
			cerr << "Error, no upper bound in mp3 frame size specified, "
				"or unrealistic one specified.\n" + getUsage( argv[0] ) 
				<< endl ;
			exit( 6 ) ;	
			
		}	
			
		string outputFilename = inputFilename ;
		
		Ceylan::substituteInString( outputFilename, ".mp3", ".osdl.music" ) ;
		
		cout << "Converting '" << inputFilename << "' into '" << outputFilename 
			<< "', using frequency " << frequency << " Hz, mode " 
			<< modeName << " (" << mode << "), bitrate " 
			<< Ceylan::toNumericalString( bitrateType )
			<< ", upper bound " << upperBound << " bytes." << endl << endl ;
			
		File & outputFile = File::Create( outputFilename ) ;
		
		// First write the relevant tag:
		outputFile.writeUint16( OSDL::MusicTag ) ;
		
		outputFile.writeUint16( frequency ) ;
		outputFile.writeUint16( mode ) ;
		outputFile.writeUint8( bitrateType ) ;
		outputFile.writeUint16( static_cast<Ceylan::Uint16>( upperBound ) ) ;
		
		// Clumsy, slow, but working:
		File & inputFile = File::Open( inputFilename ) ;
		
		Ceylan::System::Size inputSize = inputFile.size() ;
		
		for ( Ceylan::System::Size i = 0; i < inputSize; i++ )
			outputFile.writeUint8( inputFile.readUint8() ) ;
					
		delete & outputFile ;
		delete & inputFile ;
		

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

