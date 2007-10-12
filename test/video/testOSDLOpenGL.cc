#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::OpenGL ;

using namespace Ceylan::Log ;



const std::string textureFilename = "OSDL-icon.png" ;



/**
 * Testing the basic services of the OSDL video using OpenGL.
 *
 * @note Only used here for 2D.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;
	
	
    try 
	{
	
	
		LogPlug::info( "Testing OSDL video OpenGL basic services." ) ;
			

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
		

		LogPlug::info( "Starting OSDL with OpenGL enabled." )	;
			
        CommonModule & myOSDL = getCommonModule( 
			CommonModule::UseVideo | CommonModule::UseEvents ) ;		
			
		myOSDL.logState() ;
					
		LogPlug::info( "Testing real video (displayable)." ) ;
		
		LogPlug::info( "Getting video module." ) ;
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		myVideo.logState() ;
				
		LogPlug::info( "Displaying available video definitions: " 
			+ VideoModule::DescribeAvailableDefinitions( 
					Surface::FullScreen | Surface::Hardware ) ) ;     
		
		LogPlug::info( "Displaying configuration informations, "
			"including best available pixel format: "
			+ VideoModule::DescribeVideoCapabilities() ) ;
						
		LogPlug::info( "Displaying video driver name: "
			+ VideoModule::GetDriverName() + "." ) ;
		

		// Going from potential to real:
					
		LogPlug::info( "Entering visual tests: "
			"initializing the screen with OpenGL." ) ;


		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
		
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::OpenGL /* | VideoModule::Fullscreen */, OpenGLFor2D ) ;

		LogPlug::info( "Displaying now new current video informations. "
			+ VideoModule::DescribeVideoCapabilities() ) ;

		//myVideo.setOpenGLFor( 2D ) ;		
	
		Surface & screen = myVideo.getScreenSurface() ;
		
		GLTexture::SetTextureMode( GLTexture::TwoDim ) ;

		Ceylan::System::FileLocator textureFinder ;

		// When run from playTests.sh build directory:
		textureFinder.addPath( "../src/doc/web/images" ) ;
		
		// When run from executable build directory:
		textureFinder.addPath( "../../src/doc/web/images" ) ;

		// When run from executable install directory:
		textureFinder.addPath( "../OSDL/doc/web/images" ) ;
				
		GLTexture & texture = * new GLTexture( 
			textureFinder.find( textureFilename ) ) ;
		
		LogPlug::info( VideoModule::DescribeEnvironmentVariables() ) ;
		
		OSDL::Events::EventsModule & myEvents = myOSDL.getEventsModule() ;
		
		myEvents.logState() ;
		
		screen.update() ;
		
		if ( ! isBatch )
			myEvents.waitForAnyKey() ;
		
		delete & texture ;
		
		LogPlug::info( "Stopping OSDL." ) ;		
        OSDL::stop() ;

		LogPlug::info( "End of OSDL OpenGL test." ) ;
		
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

