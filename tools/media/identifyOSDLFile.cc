#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " AN_OSDL_FILE\nIdentifies the type of the OSDL file, and gives some informations about it." ;



std::string getUsage( const std::string & execName ) throw()
{

	return "Usage: " + execName + Usage ;

}

	
	
void interpretSoundFile( File & inputFile ) 
{

	cout << "  + Sampling frequency: " << inputFile.readUint16() << " Hz." 
		<< endl ;
	
	cout << "  + Sample format: " 
		<< sampleFormatToString( inputFile.readUint16() ) << "." << endl ;

	cout << "  + Channel format: " 
		<< channelFormatToString( inputFile.readUint16() ) << "." << endl ;
	
	cout << "  + Size of all samples: " 
		<< inputFile.size() -  4*sizeof(Ceylan::Uint16) << " bytes." << endl ;
	
}



int main( int argc, char * argv[] ) 
{
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Identifying an OSDL file." ) ;

		
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
			
					
		// Clumsy, slow, but working:
		File & inputFile = File::Open( inputFilename ) ;
		
		//Ceylan::System::Size inputSize = inputFile.size() ;

		FileTag tag = inputFile.readUint16() ;
		
		if ( ! OSDL::IsAValidOSDLFileTag( tag ) )
		{
		
			cerr << "File '" << inputFilename 
				<< "' does not seem to be an OSDL file (unknown "
				<< tag << " tag)." << endl ;
			
			exit( 6 ) ;
				
		}
		
		cout << "File '" << inputFilename << "', according to its tag, is a "
			<< OSDL::DescribeFileTag( tag ) << "." << endl ;
		
		/*
		 * No switch( tag ) as:
		 * «OSDL::SoundTag" cannot appear in a constant-expression
		 *
		 */
		if ( tag == OSDL::SoundTag )
			interpretSoundFile( inputFile ) ;
			
				
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

