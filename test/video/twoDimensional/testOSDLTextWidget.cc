#include "OSDL.h"
using namespace OSDL ;
using namespace OSDL::Video ;
using namespace OSDL::Video::Pixels ;
using namespace OSDL::Video::TwoDimensional ;
using namespace OSDL::Video::TwoDimensional::Text ;


using namespace Ceylan::Log ;


#include <string>
using std::string ;




#ifdef OSDL_COUNT_INSTANCES

#define CHECKPOINT(message) CHECKPOINT( message )

#else // OSDL_COUNT_INSTANCES

#define CHECKPOINT(message) 

#endif // OSDL_COUNT_INSTANCES



/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string trueTypeFontDirFromExec = 
	"../../../src/doc/web/common/fonts" ;


/*
 * TrueType font directory is defined relatively to OSDL documentation tree,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from OSDL/OSDL-${OSDL_VERSION}/src/code)
 *
 */
const std::string trueTypeFontDirForPlayTests = "../src/doc/web/common/fonts" ;


const std::string firstTrueTypeFontName  = "stilltim.ttf" ;
const std::string secondTrueTypeFontName = "cretino.ttf" ;
const std::string thirdTrueTypeFontName  = "earwigfa.ttf" ;
//const std::string thirdTrueTypeFontName  = "neurochr.ttf" ;




/*
 * Wanting to use at first '9x18B.fnt'.
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string fixedFontDirFromExec = "../../../../OSDL-data/fonts/fixed" ;


/*
 * Wanting to use at first '9x18B.fnt'.
 * Fixed font directory is defined in LOANI as
 * ${alternate_prefix}/OSDL-data/fonts/fixed,
 * usually this pathname relative to the install directory where this test
 * executable should lie is :
 * (to be reached from executable directory)
 *
 */
const std::string fixedFontDirForPlayTests = "../../../OSDL-data/fonts/fixed" ;




/**
 * Small usage tests for text widgets.
 *
 */
int main( int argc, char * argv[] ) 
{


	LogHolder myLog( argc, argv ) ;

	Font::RenderQuality chosenQuality = Font::Blended ;
	

    try 
	{
			

    	LogPlug::info( "Testing OSDL Text widget" ) ;	
		
    	LogPlug::info( "Pre requesite : initializing the display" ) ;	
	         		 
		CommonModule & myOSDL = OSDL::getCommonModule( CommonModule::UseVideo 
			| CommonModule::UseKeyboard ) ;				
		
		VideoModule & myVideo = myOSDL.getVideoModule() ; 
		
		Length screenWidth  = 640 ;
		Length screenHeight = 480 ; 
	
		CHECKPOINT( "Before setMode." ) ;	
		myVideo.setMode( screenWidth, screenHeight,
			VideoModule::UseCurrentColorDepth,
			VideoModule::SoftwareSurface ) ;
			
		CHECKPOINT( "Retrieving screen surface." ) ;	
		Surface & screen = myVideo.getScreenSurface() ;
				
    	LogPlug::info( "Drawing text widget." ) ;		
 		
		
    	LogPlug::info( "Preparing TrueType font." ) ;				

		Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirFromExec ) ;
			
		Text::TrueTypeFont::TrueTypeFontFileLocator.addPath(
			trueTypeFontDirForPlayTests ) ;		
	
		Text::TrueTypeFont myFirstTrueTypeFont( firstTrueTypeFontName,
			/* point size */ 26, 
			/* index */ 0, 
			/* convertToDisplay */ true, 
			/* render cache */ Text::Font::WordCached ) ;
		
		Text::TrueTypeFont mySecondTrueTypeFont( secondTrueTypeFontName,
			/* point size */ 30, 
			/* index */ 0, 
			/* convertToDisplay */ true, 
			/* render cache */ Text::Font::WordCached ) ;
		
		Text::TrueTypeFont myThirdTrueTypeFont( thirdTrueTypeFontName,
			/* point size */ 30, 
			/* index */ 0, 
			/* convertToDisplay */ true, 
			/* render cache */ Text::Font::WordCached ) ;
		
		
    	LogPlug::info( "Preparing Fixed font." ) ;	
					
		Text::FixedFont::FixedFontFileLocator.addPath( fixedFontDirFromExec ) ;
		Text::FixedFont::FixedFontFileLocator.addPath( 
			fixedFontDirForPlayTests ) ;
				
		Text::FixedFont myFixedFont( 9, 18, 
			/* renderingStyle */ Text::Font::Bold, 
			/* convertToDisplay */ true, /* render cache */
			Text::Font::WordCached ) ;
		
		
		const string myFirstText = 
			"This is an example of a text widget with square corners. "
			"Dare to be naive. Zombie processes detected, machine is haunted. "
			"End of paragraph.\n"
			"Happiness is twin floppies. Intel CPUs are not defective, "
			"they just act that way. "
			"Have you reconsidered a computer career ? "
			"People who take cold baths never have rheumatism, "
			"but they have cold baths. Another paragraph hits the dust.\n"
			"I recognize terror as the finest emotion and so I will try to "
			"terrorize the reader. "
			"But if I find that I cannot terrify, I will try to horrify, "
			"and if I find that I cannot horrify, I'll go for the gross-out "
			"(Stephen King)." ;


		const string mySecondText = 
			"This is an example of a text widget with round corners. "
			"Never trust an operating system. "
			"You know you're using the computer too much when "
			"you emerge packages just to watch the compile messages fly "
			"by on the screen...\n"
			"You will visit the Dung Pits of Glive soon. "
			"If people could put rainbows in zoos, they'd do it." ;

		LogPlug::info( "Before adding text widget, screen surface is : " 
			+ screen.toString() ) ;
		
		CHECKPOINT( "Creating first widget." ) ;	
		
		
		// Will be deallocated by container (screen) on explicit removal :
		new TextWidget( screen, 
			Point2D( (Coordinate) 10, 20 ),
			/* width */ 400, 
			/* height */ 450, 
			/* shape */ TextWidget::SquareCorners, 
			/* text color */ Pixels::Green, 
			/* edge color */ Pixels::CadetBlue, 
			/* back color */ Pixels::Purple,
			myFirstText, 
			myFirstTrueTypeFont,		
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "TrueType font test" ) ;
					

		CHECKPOINT( "Creating second widget." ) ;	
		
		
		// Made to overlap :
				
		/*
		 * Automatic registering to container (screen), which would remove 
		 * it automatically, but, for example purpose here, the pointer is 
		 * kept so that the deallocation is triggered explicitly in user code.
		 * As this instance count should show, both types of deletion should
		 * work.
		 *
		 */
		new TextWidget( screen, 
			Point2D( (Coordinate) 330, 220 ),
			/* width */ 300, 
			/* height  */ 240, 
			/* shape */ TextWidget::SquareCorners, 
			/* text color */ Pixels::Brown, 
			/* edge color */ Pixels::Sienna, 
			/* back color */ Pixels::ForestGreen,
			mySecondText, 
			myFixedFont, 
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "Fixed font test" ) ;
		
		CHECKPOINT( "Creating third widget (non text)." ) ;	


		// Test a basic non-decorated widget :
		Widget * theWidgetIChoseToDeleteMyself = new Widget( screen, 
			Point2D( (Coordinate) 220, 250 ),
			/* width */ 50, 
			/* height */ 100, 
			/* baseColorMode */ Widget::BackgroundColor,
			/* background color */ Pixels::Blue, 
			/* title */ "",
			/* minMaximizable */ false, /* draggable */ false, 
			/* wrappable */ false, 	/* closable */ false ) ;
		
		LogPlug::info( "After adding text widget, screen surface is : " 
			+ screen.toString() ) ;

		screen.lock() ;				
		screen.redraw() ;		
		screen.unlock() ;
				
		screen.update() ;	
		
		screen.savePNG( argv[0] + std::string( "-simple.png" ) ) ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
		
		/*
		 * If you do not want this widget to show up in second screen just
		 * delete it :
		 *
		 */
		delete theWidgetIChoseToDeleteMyself ;
		
		screen.clear() ;

		std::string currentText = "A short sentence. "
		
		// Uncomment this to check that too long word are correctly managed :
		/*
		 * currentText += 
		 	" Atoolongwordtofitinthetextcontaineryesmylorditisverylong" ;
		 *
		 */
		
		CHECKPOINT( "Creating fourth widget." ) ;	


		new TextWidget( screen, 
			Point2D( (Coordinate) 50, 100 ),
			/* width */ 400, 
			/* height */ 300, 
			/* shape */ TextWidget::SquareCorners, 
			/* text color */ Pixels::Yellow, 
			/* edge color */ Pixels::Pink, 
			/* back color */ Pixels::Brown,
			currentText, 
			mySecondTrueTypeFont,		
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "I want a caption" ) ;
			
		currentText += "Longer one. More, so that there are two lines." ;

		CHECKPOINT( "Creating fifth widget." ) ;	


		new TextWidget( screen, 
			Point2D( (Coordinate) 50, 170 ),
			/* width */ 400, 
			/* height */ 300, 
			/* shape */ TextWidget::SquareCorners, 
			/* text color */ Pixels::Yellow, 
			/* edge color */ Pixels::Pink, 
			/* back color */ Pixels::Brown,
			currentText, 
			mySecondTrueTypeFont,		
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "" ) ;
		
		currentText += 
			" Still a longer one, hoping a total of at least three lines." ;

		CHECKPOINT( "Creating sixth widget." ) ;	


		new TextWidget( screen, 
			Point2D( (Coordinate) 50, 280 ),
			/* width */ 400, 
			/* height */ 300, 
			/* shape */ TextWidget::SquareCorners, 
			/* text color */ Pixels::Yellow, 
			/* edge color */ Pixels::Pink, 
			/* back color */ Pixels::Brown,
			currentText, 
			mySecondTrueTypeFont,		
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "" ) ;
			
			
		new TextWidget( screen, 
			Point2D( (Coordinate) 300, 30 ),
			/* width */ 335, 
			/* height */ 300, 
			/* shape */ TextWidget::RoundCorners, 
			/* text color */ Pixels::MintCream, 
			/* edge color */ Pixels::MintCream, 
			/* back color */ Pixels::DarkSlateBlue,
			"This is a hello world message, from a beautiful text widget "
				"with splendid round corners, as anyone can see !", 
			myThirdTrueTypeFont,		
			/* minimum height */ true,	
			/* vertically aligned */ true,
			/* justified */ true,
			/* quality */ chosenQuality,
			/* title */ "" ) ;
			
			
		screen.lock() ;				
		screen.redraw() ;		
		screen.unlock() ;
				
		screen.update() ;	
		
		screen.savePNG( argv[0] + std::string( ".png" ) ) ;
		
		myOSDL.getEventsModule().waitForAnyKey() ;
								
		LogPlug::info( "End of OSDL Text widget test" ) ;
		
		CHECKPOINT( "Before stopping OSDL " + Ceylan::Countable::ToString() ) ;	
				
		OSDL::stop() ;

		CHECKPOINT( "After having stopped OSDL " 
			+ Ceylan::Countable::ToString() ) ;	
		
		// Fonts are deallocated automatically.
		
    }
	
    catch ( const OSDL::Exception & e )
    {
	
        LogPlug::error( "OSDL exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const Ceylan::Exception & e )
    {
	
        LogPlug::error( "Ceylan exception caught : "
        	 + e.toString( Ceylan::high ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( const std::exception & e )
    {
	
        LogPlug::error( "Standard exception caught : " 
			 + std::string( e.what() ) ) ;
       	return Ceylan::ExitFailure ;

    }

    catch ( ... )
    {
	
        LogPlug::error( "Unknown exception caught" ) ;
       	return Ceylan::ExitFailure ;

    }

	CHECKPOINT( "Just before exiting : " + Ceylan::Countable::ToString() ) ;	

    return Ceylan::ExitSuccess ;

}

