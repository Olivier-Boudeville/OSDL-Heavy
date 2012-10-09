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
using namespace OSDL::Audio ;
using namespace OSDL::Video ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " <an OSDL file>\nIdentifies the type of "
  "the OSDL file, and gives some informations about it." ;



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



void interpretMusicFile( File & inputFile )
{

	cout << "  + Sampling frequency: " << inputFile.readUint16() << " Hz."
		<< endl ;

	cout << "  + Channel format: "
		<< channelFormatToString( inputFile.readUint16() ) << "." << endl ;

	cout << "  + Bitrate type: "
		<< Music::DescribeBitrateType( inputFile.readUint8() ) << "." << endl ;

	cout << "  + Upper bound of encoded frame size: "
		<< inputFile.readUint16() << " bytes." << endl ;

	cout << "  + Size of actual mp3 content: "
		<< inputFile.size() - 4*sizeof(Ceylan::Uint16)
			- sizeof(Ceylan::Uint8)<< " bytes." << endl ;

}



void interpretPaletteFile( File & inputFile )
{

	bool hasColorkey = ( inputFile.readUint8() != 0 ) ;

	Size colorDefinitionSizeInFile = inputFile.size() - sizeof( PaletteTag )
		- 1 /* colorkey indicator */ ;


	if ( hasColorkey )
	{

		cout << "  + Color key defined, to index #" << inputFile.readUint16()
			<< "." << endl ;

		colorDefinitionSizeInFile -= 2 ;

	}
	else
	{

		cout << "  + No color key defined." << endl ;

	}

	if ( colorDefinitionSizeInFile % 3 != 0 )
	{
		cerr << "Abnormal size for color definitions." << endl ;
		exit( 8 ) ;

	}

	cout << "  + " + Ceylan::toString( colorDefinitionSizeInFile / 3 )
		+ " color definitions stored." << endl ;


}



void interpretFrameFile( File & inputFile )
{

	cout << "  + Frame offset: [" << inputFile.readSint16() << ","
		 << inputFile.readSint16() <<"]." << endl ;

	cout << "  + Palette identifer: " << inputFile.readUint16() << "." << endl ;

	cout << "  + Sprite shape: "
		 << Rendering::Sprite::DescribeShape( inputFile.readUint8() )
		 << " pixels." << endl ;

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

		FileTag tag = inputFile.readUint16() ;

		if ( ! OSDL::IsAValidOSDLFileTag( tag ) )
		{

			cerr << "File '" << inputFilename
				<< "' does not seem to be an OSDL file (unknown "
				<< tag << " tag)." << endl ;

			exit( 6 ) ;

		}

		cout << "File '" << inputFilename << "', according to its tag, is a(n) "
			<< OSDL::DescribeFileTag( tag ) << "." << endl ;

		/*
		 * No switch( tag ) as:
		 * «OSDL::SoundTag" cannot appear in a constant-expression
		 *
		 */
		if ( tag == OSDL::SoundTag )
			interpretSoundFile( inputFile ) ;
		else if ( tag == OSDL::MusicTag )
			interpretMusicFile( inputFile ) ;
		else if ( tag == OSDL::PaletteTag )
			interpretPaletteFile( inputFile ) ;
		else if ( tag == OSDL::FrameTag )
			interpretFrameFile( inputFile ) ;

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
