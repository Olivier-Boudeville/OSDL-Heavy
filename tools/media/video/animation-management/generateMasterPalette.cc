#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace std ;


/*
 * See http://en.wikipedia.org/wiki/List_of_software_palettes#6-8-5_levels_RGB
 * See also: trunk/src/code/scripts/python/generate_master_palette.py
 *
 */


#include <iostream>  // for cout


const string paletteFilePrefix = "master-palette" ;

const string originalFileSuffix  = "-original" ;
const string quantizedFileSuffix = "-quantized" ;
const string gammaFileSuffix     = "-gamma-corrected" ;


const string originalPaletteFilename = paletteFilePrefix + originalFileSuffix 
	+ ".rgb" ;
	
const string quantizedPaletteFilename = paletteFilePrefix + quantizedFileSuffix
	+ ".rgb" ;

const string gammaPaletteFilename = paletteFilePrefix + quantizedFileSuffix 
	+ gammaFileSuffix + ".rgb" ;


Pixels::GammaFactor gamma = 2.3 ;


const std::string Usage = " [ -h ]: generates a 255-color quantized non-gamma corrected main master palette and saves it under a file named '"
	+ quantizedPaletteFilename + "'. For documentary purpose, saves under a file named '" + originalFileSuffix + "' the original (non-quantized) palette and under a file named '" + gammaPaletteFilename + "' a quantized then gamma-corrected version of the original palette as well."
"\n\t-h: displays this help"
"\nThe main palette ('" + quantizedPaletteFilename + "') is dedicated to color-reduction of any frame image. It stores all the color definitions of the palette in an unencoded form (8 bits per color component, but quantized as it would be in 5 bits, and not gamma-corrected), in RGB order. No colorkey is specified to avoid that a color-matching algorithm uses it (thus 255 colors instead of 256 are defined in this master palette)." 
"\nPalette files can be converted into palette images thanks to:"
"\n'convert -depth 8 -size 17x15 " + originalPaletteFilename + " -scale 800% master-palette-original.png'. Use also: 'make master-palette-original.png'";




std::string getUsage( const std::string & execName ) throw()
{


	return "Usage: " + execName + Usage ;

}

	

int main( int argc, char * argv[] ) 
{

	
	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{

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
			
			
			if ( token == "-h" )
			{
			
				cout << endl << getUsage( argv[0] ) << endl ;
				exit( 0 ) ;	
					
				tokenEaten = true ;
				
			}
			
			
			if ( ! tokenEaten )
			{
					
				cerr << "Unexpected command line argument: '" + token 
					+ "'.\n" + getUsage( argv[0] ) << endl ;
				exit( 1 ) ;	
					
			}
		
		
		} // while
		
		
		
		
		cout << endl << "Generating the '" << quantizedPaletteFilename << 
			"' quantized master palette. "
			"The original (non-quantized) palette will be stored in '" 
			+ originalPaletteFilename 
			+ "', and a quantized gamma-corrected version (with gamma factor: " 
			<< gamma << ") will be stored in '"
			<< gammaPaletteFilename + "'." << endl << endl ;


		Palette & palette = Palette::CreateMasterPalette() ;
		palette.save( originalPaletteFilename ) ;
		
		palette.quantize( /* quantizeMaxCoordinate */ 31,
			/* scaleUp */ true ) ;
		palette.save( quantizedPaletteFilename ) ;
				
		palette.correctGamma( gamma ) ;
		palette.save( gammaPaletteFilename ) ;
		
		delete & palette ;
		
		cout << "Generation of master palette succeeded !" << endl ;

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

