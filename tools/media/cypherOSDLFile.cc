#include "OSDL.h"
using namespace OSDL ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " <an OSDL file to cypher>\nReplaces the current content of the specified file by a cyphered content, so that this updated file, once included in an OSDL archive and read from it with cyphering on, will show a content identical to the original one. " ;



std::string getUsage( const std::string & execName ) throw()
{

	return "Usage: " + execName + Usage ;

}



int main( int argc, char * argv[] ) 
{
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Cyphering an OSDL file." ) ;

		
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
			
		/*
         * We preferred reading the full file to memory, convert its content,
         * and write it back rather than processing it byte per byte.
         *
         */
         			
		// We read a normal file:
		File & inputfile = StandardFile::Open( inputFilename ) ;
		
        Size fileSize = inputfile.size() ;
        
        Ceylan::Byte * buffer = new Ceylan::Byte[fileSize] ;
        
        inputfile.read( buffer, fileSize ) ;
        
		delete & inputfile ;

        // We use cyphering, despite this is not an embedded file:
        EmbeddedFile::CypherBuffer( buffer, fileSize ) ;
        
        // We open/create the (same) standard file:
		File & outputfile = StandardFile::Create( inputFilename ) ;
        
        outputfile.write( buffer, fileSize ) ;
        
		delete & outputfile ;

        delete [] buffer ;
        
		cout << "Successfully updated '" << inputFilename << "', "
        	<< fileSize << " bytes cyphered and written." << endl ;
		

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

