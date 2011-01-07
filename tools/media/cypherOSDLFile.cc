/* 
 * Copyright (C) 2003-2011 Olivier Boudeville
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


#include "OSDL.h"
using namespace OSDL ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " [-d|--decypher] <an OSDL file to cypher>\nReplaces the current content of the specified file by a cyphered content, so that this updated file, once included in an OSDL archive and read from it with cyphering on, will show a content identical to the original one. If the -d option is used, the file will be decyphered instead." ;



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
	
    	bool decypher = false ;
		
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
			else if ( token == "-d" || token == "--decypher" )			
            {
            
            	cout << "(will decypher instead of cyphering)" << endl ;
                decypher = true ;
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
        if ( ! decypher )
	        EmbeddedFile::CypherBuffer( buffer, fileSize ) ;
        else
	        EmbeddedFile::DecypherBuffer( buffer, fileSize ) ;

        // We open/create the (same) standard file:
		File & outputfile = StandardFile::Create( inputFilename ) ;
        
        outputfile.write( buffer, fileSize ) ;
        
        outputfile.close() ;
        
		delete & outputfile ;

        delete [] buffer ;
        
        if ( ! decypher )
			cout << "Successfully updated '" << inputFilename << "', "
        		<< fileSize << " bytes cyphered and written." << endl ;
        else        
			cout << "Successfully updated '" << inputFilename << "', "
        		<< fileSize << " bytes decyphered and written." << endl ;
		

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

