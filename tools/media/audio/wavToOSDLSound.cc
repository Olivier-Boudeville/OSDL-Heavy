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


#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Audio ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " [ -f frequency ] [ -m mode ] [ -b bitdepth ] X.wav\nConverts a WAVE file (*.wav) into a .osdl.sound file by replacing the WAVE header by an OSDL header filled with informations specified from the command-line."
	"\n\t -f: specifies the output sampling frequency, in Hz, ex: -f 22050 (the default)" 
	"\n\t -m: specifies the output mode, mono or stereo, ex: -m mono (the default)" 
	"\n\t -b: specifies the sample (PCM) bit depth, in bits, ex: -b 16 (the default). A bit depth of 4 corresponds by convention to the IMA ADPCM sample format." 
	"\nOne may use the sox command-line tool to retrieve the relevant audio settings for the source sound."
	"\n\tEx: 'sox -V YourSound.wav -n' converts the sound and outputs its metadata that can be used to fill the next command line." 
	"\nThen 'wavToOSDLSound.exe -f 44100 -m stereo -b 8 YourSound.wav' results in the creation of 'YourSound.osdl.sound'. Alternatively, use the wavToOSDLSound.sh script: 'wavToOSDLSound.sh YourSound.wav' takes care of everything and results in the YourSound.osdl.sound file." 
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
	
	// Actually LittleSint16SampleFormat:
	Ceylan::Uint8 bitDepth = 16 ;
	
	SampleFormat format = AudioModule::LittleSint16SampleFormat ;
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Converting a raw PCM file into an OSDL sound." ) ;

		
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
			
			
			if ( token == "-b" )
			{
			
				bitDepth = static_cast<Ceylan::Uint8>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
				options.pop_front() ;
				
				if ( bitDepth == 16 )
					format = AudioModule::LittleSint16SampleFormat ;
				else if ( bitDepth == 8 )
					format = AudioModule::Sint8SampleFormat ;
				else if ( bitDepth == 4 )
					format = AudioModule::IMAADPCMSampleFormat ;
				else
				{
					
					cerr << "Error, unexpected bit depth specified: '" + token 
						+ "'.\n" + getUsage( argv[0] ) << endl ;
					
					exit( 3 ) ;	
				
				}	
					
				LogPlug::info( "Bit depth implied format "
					+ Ceylan::toString( format ) + "." ) ;
					
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
			
		string outputFilename = inputFilename ;
		
		Ceylan::substituteInString( outputFilename, ".wav", 
			".osdl.sound" ) ;
		
		if ( inputFilename == outputFilename )
		{
		
			cerr << "Error, input file '" << inputFilename 
				<< "' does not have an appropriate name (*.wav)." << endl ;
			exit( 6 ) ;	
		
		}
		
		cout << "Converting '" << inputFilename << "' into '" << outputFilename 
			<< "', using frequency " << frequency << " Hz, mode " 
			<< modeName << " (" << mode << "), bit depth " 
			<< Ceylan::toNumericalString( bitDepth )
			<< " (format " << format << ")." << endl << endl ;
			
		File & outputFile = File::Create( outputFilename ) ;
		
		// First write the relevant tag:
		outputFile.writeUint16( OSDL::SoundTag ) ;
		
		outputFile.writeUint16( frequency ) ;
		outputFile.writeUint16( format ) ;
		outputFile.writeUint16( mode ) ;
		
		// Clumsy, slow, but working:
		File & inputFile = File::Open( inputFilename ) ;
		
		Ceylan::System::Size inputSize = inputFile.size() ;

		if ( inputSize < 44 )
		{
		
			cerr << "Error, input file '" << inputFilename 
				<< "' is even smaller than the WAVE header." << endl ;
			exit( 7 ) ;	
		
		}

		// Skip the first 44 bytes of the wav header:
		Ceylan::System::Size i ;
		 
		for ( i = 0; i < 44; i++ )
			inputFile.readUint8() ;
			
		for ( ; i < inputSize; i++ )
			outputFile.writeUint8( inputFile.readUint8() ) ;
					
		delete & outputFile ;
		delete & inputFile ;
		
		cout << "Generation of '" << outputFilename << "' succeeded !" 
			<< endl ;

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

