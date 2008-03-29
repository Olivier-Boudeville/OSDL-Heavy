#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;



#include <iostream>  // for cout


const std::string Usage = " [ -o abscissa ordinate ] [ -p palette_identifier ] X.png\nConverts a PNG file (X.png) into a OSDL frame file (X.osdl.frame), containing a header, then a tile map, then the corresponding set of tiles. The header references the palette being used, the type of the tile map, and the offset for frame coordinates."
	"\n\t -o: specifies the offset of the local referential of this frame to the global referential of the animation it is a part of (default: (0,0) offset)."
	"\n\t -p: specifies the palette identifier to be used (default: palette #1, i.e. the master palette)."
	"\nThe source PNG is expected to have been transformed, directly or not, by our 'process-reiner-individual-archive.sh' script: converted, scaled, sharpened, renamed (ex: 18-16-3-1-0.png), gamma-corrected, color-reduced, etc."
	;



std::string getUsage( const std::string & execName ) throw()
{

	return "Usage: " + execName + Usage ;

}

	

int main( int argc, char * argv[] ) 
{

	
	//bool displayImage = false ;
	bool displayImage = true ;
	
	bool isBatch = false ;
	
	Coordinate x_offset = 0 ;
	Coordinate y_offset = 0 ;
	
	PaletteIdentifier paletteId = 1 ;
	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

		
		LogPlug::info( "Converting a PNG file into an OSDL frame." ) ;

		
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
					
						
			if ( token == "-o" )
			{
			
				if ( options.size() < 2 )
				{
				
					cerr << "Error, parameters lacking for offset.\n" 
						+ getUsage( argv[0] ) << endl ;
					
					exit( 9 ) ;	
					
				}	
			
				x_offset = static_cast<Coordinate>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
					
				options.pop_front() ;

				y_offset = static_cast<Coordinate>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
					
				options.pop_front() ;
				
				LogPlug::info( "User offset set to ("
					+ Ceylan::toString( x_offset ) + ","
					+ Ceylan::toString( y_offset ) + " )." ) ;
					
				tokenEaten = true ;
				
			}
			
			
			if ( token == "-p" )
			{
			
				paletteId = static_cast<PaletteIdentifier>( 
					Ceylan::stringToUnsignedLong( options.front() ) ) ;
					
				options.pop_front() ;
				
				LogPlug::info( "Palette identifier set to "
					+ Ceylan::toString( paletteId ) + "." ) ;
					
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
		
		Ceylan::substituteInString( outputFilename, ".png", 
			".osdl.frame" ) ;
		
		if ( inputFilename == outputFilename )
		{
		
			cerr << "Error, input file '" << inputFilename 
				<< "' does not have an appropriate name (*.png)." << endl ;
			exit( 6 ) ;	
		
		}
		
		cout << "Converting '" << inputFilename << "' into '" << outputFilename 
			<< "', using frame offset (" << x_offset << "," << y_offset 
			<< "), and palette identifier #" << paletteId << "." 
			<< endl << endl ;
			
		File & outputFile = File::Create( outputFilename ) ;
		
		// First write the relevant tag:
		outputFile.writeUint16( OSDL::FrameTag ) ;
		
		outputFile.writeUint16( x_offset ) ;
		outputFile.writeUint16( y_offset ) ;
		outputFile.writeUint16( paletteId ) ;
				
		Surface & sourceSurface = Surface::LoadImage( inputFilename,
			/* convertToDisplayFormat */ false ) ;
		
		LogPlug::info( "Image '" + inputFilename + "' loaded in: "
			+ sourceSurface.toString() ) ;
			
		Surface * screen = 0 ;
		
		OSDL::CommonModule * myOSDL = 0 ;
		
		if ( displayImage )
		{
		
			myOSDL = & OSDL::getCommonModule( 
				CommonModule::UseVideo | CommonModule::UseEvents ) ;	
				
			VideoModule & myVideo = myOSDL->getVideoModule() ; 
		
			Length screenWidth  = 640 ;
			Length screenHeight = 480 ; 

			myVideo.setMode( screenWidth, screenHeight,
				VideoModule::UseCurrentColorDepth, 
				VideoModule::SoftwareSurface ) ;
		
			screen = & myVideo.getScreenSurface() ;
			
			sourceSurface.blitTo( *screen ) ;
			
			screen->update() ;
		
			if ( ! isBatch )				
				myOSDL->getEventsModule().waitForAnyKey() ;
			
		}
		
		LogPlug::info( "Source image is " 
			+ Ceylan::toString( sourceSurface.getWidth() ) + "x" 
			+ Ceylan::toString( sourceSurface.getHeight() ) 
			+ " pixels, and pixel at (0,0) is " 
			+ Ceylan::toString( sourceSurface.getPixelColorAt( 0, 0 ) ) ) ;
			
		Palette	& sourcePalette = sourceSurface.getPalette() ;
		
		LogPlug::info( "Palette of source image is: " 
			+ sourcePalette.toString() ) ;
		
		string paletteRawFilename = Ceylan::substituteIn( 
			inputFilename, ".png", ".rgb" ) ;
		sourcePalette.save( paletteRawFilename, /* encoded */ false ) ;
		
		string paletteEncodedFilename =	Ceylan::substituteIn(
			inputFilename, ".png", ".pal" ) ;
		sourcePalette.save( paletteEncodedFilename, /* encoded */ true ) ;
		
		sourcePalette.correctGamma( 2.3 ) ;

		string paletteRawGammaCorrectedFilename = Ceylan::substituteIn( 
			inputFilename, ".png", "-gamma.rgb" ) ;
		sourcePalette.save( paletteRawGammaCorrectedFilename, 
			/* encoded */ false ) ;
		
		if ( displayImage )
		{
		
			sourceSurface.blitTo( *screen ) ;
			
			screen->update() ;
		
			if ( ! isBatch )				
				myOSDL->getEventsModule().waitForAnyKey() ;
			
		}
		
		delete & sourcePalette ;
		
		delete & outputFile ;
		
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

