#include "OSDLBasic.h"

#include "OSDLEvents.h"              // for event module
#include "OSDLVideo.h"               // for video module
#include "OSDLCDROMDriveHandler.h"   // for CD-ROM drive handler

#include "OSDLAudio.h"               // for audio module

#include "OSDLUtils.h"               // for getBackendLastError


#include "Ceylan.h"                  // for FIFOException


#include <list>


#ifdef OSDL_USES_CONFIG_H
#include "OSDLConfig.h"              // for configure-time settings (SDL)
#endif // OSDL_USES_CONFIG_H


#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for CEYLAN_ARCH_NINTENDO_DS and al
#endif // OSDL_ARCH_NINTENDO_DS


#if OSDL_USES_SDL
#include "SDL.h"                     // for SDL_Init, etc.
#endif // OSDL_USES_SDL


#if OSDL_USES_AGAR
#include <agar/core.h>               // for AG_InitCore	
#endif // OSDL_USES_AGAR	



using std::string ;

using namespace Ceylan ;
using namespace Ceylan::Log ;

using namespace OSDL ;


// Replicating these defines allows to enable them on a per-class basis:
#if OSDL_VERBOSE_BASIC_MODULE

#define LOG_DEBUG_BASIC(message)   LogPlug::debug(message)
#define LOG_TRACE_BASIC(message)   LogPlug::trace(message)
#define LOG_WARNING_BASIC(message) LogPlug::warning(message)

#else // OSDL_VERBOSE_BASIC_MODULE

#define LOG_DEBUG_BASIC(message)
#define LOG_TRACE_BASIC(message)
#define LOG_WARNING_BASIC(message)

#endif // OSDL_VERBOSE_BASIC_MODULE



// Backend section.
	
const CommonModule::BackendReturnCode CommonModule::BackendSuccess =  0 ;
const CommonModule::BackendReturnCode CommonModule::BackendError   = -1 ;


// Tells whether the SDL backend is initialized (SDL_Init called).
bool CommonModule::_BackendInitialized = false ;



// Allows to debug OSDL version management:
#define OSDL_DEBUG_VERSION 0


const Ceylan::LibtoolVersion & OSDL::GetVersion() throw()
{


#if	OSDL_DEBUG_VERSION

	// Intentional memory leak:
	
	Ceylan::LibtoolVersion * ceylanVersion ;
	
	try
	{
		osdlVersion = new Ceylan::LibtoolVersion( OSDL_LIBTOOL_VERSION ) ;
	}
	catch( const Ceylan::Exception & e )
	{
		Ceylan::emergencyShutdown( "OSDL::GetVersion failed: "
			+ e.toString() ) ;
	}	
		
	return *osdlVersion ;


#else // OSDL_DEBUG_VERSION

	static Ceylan::LibtoolVersion osdlVersion( OSDL_LIBTOOL_VERSION ) ;
	return osdlVersion ;

#endif // OSDL_DEBUG_VERSION	
	
}




/*
 * These flags are to be used with getCommonModule to specify which submodules 
 * and settings should be used.
 *
 * @note They are defined relatively as SDL back-end.
 *
 */

#if OSDL_USES_SDL

const Ceylan::Flags CommonModule::UseTimer       = SDL_INIT_TIMER       ;
const Ceylan::Flags CommonModule::UseAudio       = SDL_INIT_AUDIO       ;
const Ceylan::Flags CommonModule::UseVideo       = SDL_INIT_VIDEO       ;
const Ceylan::Flags CommonModule::UseCDROM       = SDL_INIT_CDROM       ;
const Ceylan::Flags CommonModule::UseJoystick    = SDL_INIT_JOYSTICK    ;
const Ceylan::Flags CommonModule::UseKeyboard    = 0x4000               ;
const Ceylan::Flags CommonModule::UseMouse       = 0x8000               ;
const Ceylan::Flags CommonModule::UseEverything  = SDL_INIT_EVERYTHING  ;
const Ceylan::Flags CommonModule::NoParachute    = SDL_INIT_NOPARACHUTE ;
const Ceylan::Flags CommonModule::UseEventThread = SDL_INIT_EVENTTHREAD ;

#else // OSDL_USES_SDL

// Values from SDL reused here:
const Ceylan::Flags CommonModule::UseTimer       = 0x00000001 ;
const Ceylan::Flags CommonModule::UseAudio       = 0x00000010 ;
const Ceylan::Flags CommonModule::UseVideo       = 0x00000020 ;
const Ceylan::Flags CommonModule::UseCDROM       = 0x00000100 ;
const Ceylan::Flags CommonModule::UseJoystick    = 0x00000200 ;
const Ceylan::Flags CommonModule::UseKeyboard    = 0x4000     ;
const Ceylan::Flags CommonModule::UseMouse       = 0x8000     ;
const Ceylan::Flags CommonModule::UseEverything  = 0x0000FFFF ;
const Ceylan::Flags CommonModule::NoParachute    = 0x00100000 ;
const Ceylan::Flags CommonModule::UseEventThread = 0x01000000 ;

#endif // OSDL_USES_SDL


// To centralize this definition once for all:
const Ceylan::Flags CommonModule::UseEvents =
	UseJoystick | UseKeyboard | UseMouse ;



/*
 * Warning: OSDL added flags (if SDL adds flags, they might collide and 
 * create awkward bugs): see 'testOSDLBasic' to check their value.
 *
 * SDL_INIT_TIMER		: 0x00000001 = 0b00000000000000000000000000000001
 * SDL_INIT_AUDIO		: 0x00000010 = 0b00000000000000000000000000010000
 * SDL_INIT_VIDEO		: 0x00000020 = 0b00000000000000000000000000100000
 * SDL_INIT_CDROM		: 0x00000100 = 0b00000000000000000000000100000000
 * SDL_INIT_JOYSTICK	: 0x00000200 = 0b00000000000000000000001000000000
 * UseKeyboard          : 0x4000     = 0b00000000000000000100000000000000
 * UseMouse             : 0x8000     = 0b00000000000000001000000000000000
 * SDL_INIT_EVERYTHING	: 0x0000FFFF = 0b00000000000000001111111111111111
 * SDL_INIT_NOPARACHUTE	: 0x00100000 = 0b00000000000100000000000000000000
 * SDL_INIT_EVENTTHREAD	: 0x01000000 = 0b00000001000000000000000000000000
 *
 */



CommonModule * CommonModule::_CurrentCommonModule = 0 ;

#if OSDL_USES_SDL

/// See http://sdldoc.csn.ul.ie/sdlenvvars.php
const string CommonModule::_SDLEnvironmentVariables[] = 
{
		"SDL_DEBUG",
		"SDL_CDROM"
} ;

#else // OSDL_USES_SDL

const string CommonModule::_SDLEnvironmentVariables[] = {} ;

#endif // OSDL_USES_SDL




CommonModule::CommonModule( Flags flags ) throw ( OSDL::Exception ): 
	Ceylan::Module( 
		"OSDL common module",
		"This is the root module of OSDL",
		"http://osdl.sourceforge.net",
		"Olivier Boudeville",
		"olivier.boudeville@online.fr",
		OSDL::GetVersion(),
		"disjunctive LGPL/GPL" ),		
	_video(  0 ), 
	_events( 0 ), 
	_audio(  0 ), 
	_flags( flags ),
	_cdromHandler( 0 ) 
{

	bool useAgar = false ;
	
	LOG_TRACE_BASIC( "CommonModule constructor" ) ;
		
	// For the sake of safety:
	flags = AutoCorrectFlags( flags ) ;
	 
	send( "Starting OSDL version " + OSDL::GetVersion().toString() + ". " 
		+ InterpretFlags( flags ) ) ; 

#if OSDL_USES_SDL
	
	SDL_version compileTimeSDLVersion ;
	SDL_VERSION( & compileTimeSDLVersion ) ;

	SDL_version linkTimeSDLVersion = *SDL_Linked_Version() ;
	
	send( "Using SDL backend, compiled against the " 
		+ Ceylan::toNumericalString( compileTimeSDLVersion.major) + "."
		+ Ceylan::toNumericalString( compileTimeSDLVersion.minor) + "."
		+ Ceylan::toNumericalString( compileTimeSDLVersion.patch) 
		+ " version, linked against the "
		+ Ceylan::toNumericalString( linkTimeSDLVersion.major) + "."
		+ Ceylan::toNumericalString( linkTimeSDLVersion.minor) + "."
		+ Ceylan::toNumericalString( linkTimeSDLVersion.patch) + " version." ) ;
	
#endif // OSDL_USES_SDL



#if OSDL_ARCH_NINTENDO_DS
		
#ifdef OSDL_RUNS_ON_ARM7

	throw OSDL::Exception( "CommonModule constructor failed: "
		"not supported on the ARM7." ) ;
		
#elif defined(OSDL_RUNS_ON_ARM9)

	try
	{
	
		// Initializes and activates the IPC system:
		CommandManager::GetCommandManager().activate() ;
	
	}
	catch( const Ceylan::System::FIFO::FIFOException & e )
	{
	
		throw OSDL::Exception( "CommonModule constructor failed: "
			+ e.toString() ) ;
	
	}
			
#endif // OSDL_RUNS_ON_ARM7

#endif // OSDL_ARCH_NINTENDO_DS
	

#if OSDL_USES_SDL

#if OSDL_USES_AGAR

	useAgar = true ;
	
	AG_AgarVersion linkTimeAgarVersion ;
	
	AG_GetVersion( &linkTimeAgarVersion ) ;
	 
	send( "Using Agar backend, linked against the "
		+ Ceylan::toNumericalString( linkTimeAgarVersion.major ) + "."
		+ Ceylan::toNumericalString( linkTimeAgarVersion.minor ) + "."
		+ Ceylan::toNumericalString( linkTimeAgarVersion.patch ) 
		+ ", version "
		+ Ceylan::toString( *linkTimeAgarVersion.release ) + "." ) ;
	
	// Implies a call to SDL_Init with SDL_INIT_TIMER and SDL_INIT_NOPARACHUTE:
	if ( AG_InitCore( /* progname */ "OSDL-application", 
			/* SDL flags */ 0 ) == -1 )
		throw OSDL::Exception( "CommonModule constructor failed: "
			"Agar initialization failed: " + string( AG_GetError() ) ) ;
				
#else // OSDL_USES_AGAR

	// Starts with a minimal init:
	if ( SDL_Init( 0 ) != BackendSuccess )
		throw OSDL::Exception( "CommonModule constructor failed: "
			"SDL first initialization failed: "
			+ Utils::getBackendLastError() ) ;
			
#endif //OSDL_USES_AGAR
			 
#endif // OSDL_USES_SDL
		


	/*
	 * UseEverything flag is 0x0000FFFF, therefore not to be specifically
	 * managed.
	 *
	 * Moreover, the flags should have been already fixed thanks to the
	 * AutoCorrectFlags method, so no more interpretation is needed.
	 *
	 */
	
	if ( flags & UseTimer ) 
	{
	
		send( "Initializing timer subsystem" ) ;
		
#if OSDL_USES_SDL

		if ( useAgar )
		{
			send( "Agar being used, no additional timer initialization is "
				" needed." ) ;
		}		
		else
		{
		
			if ( SDL_InitSubSystem( UseTimer ) != BackendSuccess ) 
				throw OSDL::Exception( "CommonModule constructor: "
					"unable to initialize timer subsystem: " 
					+ Utils::getBackendLastError() ) ;
		}
		
				
	// OSDL_ARCH_NINTENDO_DS: No need to initialize timers beforehand.	
	
#endif // OSDL_USES_SDL
				
		send( "Timer subsystem initialized" ) ;
						
	}
	
	
#if ! OSDL_ARCH_NINTENDO_DS
	
	if ( flags & UseCDROM ) 
	{
	
		_cdromHandler = new OSDL::CDROMDriveHandler() ;
		
	}

#if OSDL_USES_SDL
	
	if ( flags & NoParachute ) 
	{

		send( "Disabling SDL parachute" ) ;
		
		if ( useAgar )
		{
			send( "Agar being used, no SDL parachute request is needed." ) ;
		}		
		else
		{
		
			if ( SDL_InitSubSystem( NoParachute ) != BackendSuccess )
				throw OSDL::Exception( "CommonModule constructor: "
					"unable to disable SDL parachute: " 
					+ Utils::getBackendLastError() ) ;
					
			send( "SDL parachute initialized" ) ;				
		}
			
	}

#endif // OSDL_USES_SDL
	
#endif // OSDL_ARCH_NINTENDO_DS
			
	/*
	 * @fixme Events must imply video. There seems to exist no way 
	 * of requesting specifically events. Currently video and event 
	 * supports are synonym features.
	 *
	 * @fixme On some platforms (ex: Windows), audio may not work if 
	 * no video mode is initialized.
	 * Hence audio would imply video (to be checked).
	 *
	 */
		 			
	if ( flags & UseVideo ) 
	{
	
		_video = new Video::VideoModule() ;	
		
		 		
	}
	
	// Relies on the job of AutoCorrectFlags (video already added if necessary):
	if ( flags & UseEvents ) 
	{
	
		_events = new Events::EventsModule( flags ) ; 
		
	}
	
	
	/*
	 * Video must be initialized *before* audio (to rely on a window handle) 
	 * on some platforms:
	 *
	 */	
	if ( flags & UseAudio )
	{
	
		_audio = new Audio::AudioModule() ;
		
	}	
	
	
	_BackendInitialized = true ;
	
    send( "OSDL successfully initialized" )	;
	
	dropIdentifier() ;
	
}



CommonModule::~CommonModule() throw ()
{	

    send( "Stopping OSDL." ) ;

	if ( _cdromHandler != 0 )
	{
		delete _cdromHandler ;
		_cdromHandler = 0 ;	
	}
		
	if ( _audio != 0 )
	{
		delete _audio ;
		_audio = 0 ;
	}	
	
	if ( _events != 0 )
	{
		delete _events ;
		_events = 0 ;
	}	
	
	if ( _video != 0 )
	{
		delete _video ;
		_video = 0 ;
	}
	
				
#if OSDL_USES_SDL

#if OSDL_USES_AGAR

    send( "OSDL successfully stopped (Agar branch)." ) ;
    
	AG_Destroy() ;

	// When Agar did not initialize SDL, we have to shut SDL down by ourself:
    SDL_Quit() ;

#else // OSDL_USES_AGAR

    SDL_Quit() ;

#endif // OSDL_USES_AGAR

#endif // OSDL_USES_SDL
	
    send( "OSDL successfully stopped." ) ;		
	
}



string CommonModule::InterpretFlags( Flags flags ) throw() 
{

	std::list<string> res ;
		
	if ( flags & UseTimer )
		res.push_back( "Timer requested (UseTimer is set)." ) ;
	else
		res.push_back( "No timer requested (UseTimer is not set)." ) ;
		
	if ( flags & UseAudio)  
		res.push_back( "Audio requested (UseAudio is set)." ) ;
	else
		res.push_back( "No audio requested (UseAudio is not set)." ) ;
		
	if ( flags & UseVideo )
		res.push_back( "Video requested (UseVideo is set)." ) ;
	else
		res.push_back( "No video requested (UseVideo is not set)." ) ;
		
	if ( flags & UseCDROM )
		res.push_back( "CD-ROM support requested (UseCDROM is set)." ) ;
	else
		res.push_back( "No CD-ROM support requested "
			"(UseCDROM is not set)." ) ;
		
		
	if ( flags & UseJoystick )
		res.push_back( "Joystick support requested (UseJoystick is set)." ) ;
	else
		res.push_back( "No joystick support requested "
			"(UseJoystick is not set)." ) ;
		
	if ( flags & UseKeyboard )
		res.push_back( "Keyboard support requested (UseKeyboard is set)." ) ;
	else
		res.push_back( "No keyboard support requested "
			"(UseKeyboard is not set)." ) ;
		
	if ( flags & UseMouse )
		res.push_back( "Mouse support requested (UseMouse is set)." ) ;
	else
		res.push_back( "No mouse support requested (UseMouse is not set)." ) ;
		
		
	if ( ( flags & UseEverything ) == UseEverything )
		res.push_back( "Every subsystem is requested "
			"(UseEverything is set)." ) ;
	else
		res.push_back( "Not all subsystems are requested "
			"(UseEverything is not set)." ) ;
		
	if ( flags & NoParachute )
		res.push_back( "No catching of fatal signals requested "
			"(NoParachute is set)." ) ;
	else
		res.push_back( "Fatal signal will be caught "
			"(NoParachute is not set)." ) ;
		
	if ( flags & UseEventThread )
		res.push_back( "Event thread requested (UseEventThread is set)." ) ;
	else
		res.push_back( "No event thread requested "
			"(UseEventThread is not set)." ) ;
		

	return "The specified flags for Common module, whose value is " 
		+ Ceylan::toString( flags, /* bit field */ true ) 
		+ ", mean: " + Ceylan::formatStringList( res ) ;

}



bool CommonModule::hasVideoModule() const throw()
{

	return ( _video != 0 ) ;
	
}



Video::VideoModule & CommonModule::getVideoModule() const 
	throw( OSDL::Exception )
{

	if ( _video == 0 )
		throw OSDL::Exception( 
			"CommonModule::getVideoModule: no video module available." ) ;
	
	return * _video ;
	
}



bool CommonModule::hasEventsModule() const throw()
{

	return ( _events != 0 ) ;
	
}



Events::EventsModule & CommonModule::getEventsModule() const 
	throw( OSDL::Exception )
{

	if ( _events == 0 )
		throw OSDL::Exception( 
			"CommonModule::getEventsModule: no events module available." ) ;
	
	return * _events ;
	
}



bool CommonModule::hasAudioModule() const throw()
{

	return ( _audio != 0 ) ;
	
}
 
 
 
Audio::AudioModule & CommonModule::getAudioModule() const 
	throw( OSDL::Exception )
{

	if ( _audio == 0 )
		throw OSDL::Exception( 
			"CommonModule::getAudioModule: no audio module available." ) ;
	
	return * _audio ;
	
}



Flags CommonModule::getFlags() const throw()
{

	return _flags ;
	
} 



bool CommonModule::hasCDROMDriveHandler() const throw()
{

	return ( _cdromHandler != 0 ) ;
	
}

		
CDROMDriveHandler & CommonModule::getCDROMDriveHandler() 
	const throw( OSDL::Exception )
{

	if ( _cdromHandler == 0 )
		throw OSDL::Exception( "CommonModule::getCDROMDriveHandler: "
			"no CD-ROM handler available." ) ;
	
	return * _cdromHandler ;

}



const string CommonModule::toString( Ceylan::VerbosityLevels level ) 
	const throw()
{
	
	string res = "Common root module, with currently video module " ;
		
	if ( _video != 0 )
		res += "enabled" ;
	else
		res += "disabled" ;
		
	res += ", with event module " ;
	
	if ( _events != 0 )
		res += "enabled" ;
	else
		res += "disabled" ;
		
	res += ", with audio module " ;
	
	if ( _audio != 0 )
		res += "enabled" ;
	else
		res += "disabled" ;
	
	res += ", using " ;
	
	if ( _cdromHandler != 0 )
		res += "a" ;
	else	
		res += "no" ;
		
	res += " CD-ROM handler" ;	
	
	if ( level == Ceylan::low )
		return res ;
			
			
	res += ". " + InterpretFlags( _flags ) ;
		
	if ( level == Ceylan::medium )
		return res ;
	
	std::list<string> completeMessage ;
	
	completeMessage.push_back( res ) ;
	
	completeMessage.push_back( Ceylan::Module::toString()  ) ;
	
	completeMessage.push_back( 
		"The version of the Ceylan library currently linked to is " 
		+ Ceylan::GetVersion().toString() + "." ) ;
		
	completeMessage.push_back( 
		"The version of the OSDL library currently linked to is " 
		+ OSDL::GetVersion().toString() + "." ) ;
	
	return Ceylan::formatStringList( completeMessage ) ;
	
}



string CommonModule::DescribeEnvironmentVariables() throw()
{

#if OSDL_USES_SDL

	Ceylan::Uint16 varCount = sizeof( _SDLEnvironmentVariables ) 
		/ sizeof (string) ;
	
	string result = "Examining the " + Ceylan::toString( varCount )
		+ " general-purpose environment variables for SDL backend:" ;
	
	std::list<string> variables ;
		
	string var, value ;
	
	TextOutputFormat htmlFormat = Ceylan::TextDisplayable::GetOutputFormat() ;
	
	for ( Ceylan::Uint16 i = 0; i < varCount; i++ ) 
	{
	
		var = _SDLEnvironmentVariables[ i ] ;
		value = Ceylan::System::getEnvironmentVariable( var ) ;
		
		if ( value.empty() )
		{
			if ( htmlFormat == Ceylan::TextDisplayable::html )
			{
				variables.push_back( "<em>" + var + "</em> is not set." ) ;
			}
			else
			{
				variables.push_back( var + " is not set." ) ;			
			}	
		}
		else
		{			
			if ( htmlFormat == Ceylan::TextDisplayable::html )
			{
				variables.push_back( "<b>" + var + "</b> set to [" 
					+ value + "]." ) ;
			}
			else
			{
				variables.push_back( var + " set to [" + value + "]." ) ;
			}	
		}	
	
	}
	
	return result + Ceylan::formatStringList( variables ) ;
	
#else // OSDL_USES_SDL

	return "(not using SDL)" ;
	
#endif // OSDL_USES_SDL
	
}



bool CommonModule::IsBackendInitialized() throw()
{

	return _BackendInitialized ;
	
}



Flags CommonModule::AutoCorrectFlags( Flags inputFlags ) throw()
{

#if OSDL_USES_SDL

	/*
	 * Event source implies event propagation which implies, with SDL, 
	 * video being activated:
	 *
	 */
	
	if ( ! ( inputFlags & UseVideo ) )
	{
	
		// Video not selected, are events used ?
		
		if ( inputFlags & UseEvents ) 
		{
		
			/*
			 * Yes, it is abnormal since events implies video. 
			 * Let's correct that:
			 *
			 */
			
			LOG_WARNING_BASIC( "CommonModule::AutoCorrectFlags: "
				"at least one input device was selected, "
				"hence event support was requested, "
				"whereas video was not specifically set. " 
				"Since the event loop needs video, "
				"the video subsystem has been automatically enabled." ) ; 
				
			inputFlags |= UseVideo ;

		}
	
	}
	
#endif // OSDL_USES_SDL

	return inputFlags ;
		
}



// Friend section:


CommonModule & OSDL::getCommonModule( Flags flags ) throw()
{

	// First, auto-correct flags with implied sub-systems, if necessary:
	flags = CommonModule::AutoCorrectFlags( flags ) ;
			
	LogPlug::info( "Retrieving basic common module for OSDL" ) ;
	
	if ( CommonModule::_CurrentCommonModule == 0 ) 
	{
	
		// if not running, launch OSDL unconditionally and store it.	
		LogPlug::info( 
			"OSDL was not running yet, launching basic OSDL with flags "
			+ Ceylan::toString( flags, /* bitfield */ true ) ) ;
		CommonModule::_CurrentCommonModule = new CommonModule( flags ) ;
		return * CommonModule::_CurrentCommonModule ;
		
	} 	
	else 
	{
	
		// OSDL was already running.
		LogPlug::info( "OSDL is already running, comparing demanded flags "
			"with flags of the running version." ) ;
		
		if ( flags == CommonModule::_CurrentCommonModule->getFlags() ) 
		{
		
			/* 
			 * if requested flags are the same that the ones of the 
			 * currently running OSDL instance, return currently running OSDL
			 * instance.
			 */
			 
			LogPlug::info( "Flags are matching, "
				"returning already launched basic OSDL module" ) ;
				
			return * CommonModule::_CurrentCommonModule ;
			
		} 
		else 
		{
		
			/* 
			 * Running flags and requested flags do not match.
			 * Stop current OSDL instance, and re-run OSDL with requested flags.
			 *
			 */
			LogPlug::info( "Flags do not match, "
				"stopping already launched OSDL root module, "
				"restarting with new flags, returning this new instance" ) ;
				
			delete CommonModule::_CurrentCommonModule ;
			
			CommonModule::_CurrentCommonModule = new CommonModule( flags ) ;
			
			return * CommonModule::_CurrentCommonModule ;
			
		}
	}	
				
}



bool OSDL::hasExistingCommonModule() throw()
{

	return ( CommonModule::_CurrentCommonModule != 0 ) ;
	
}



CommonModule & OSDL::getExistingCommonModule() throw()
{

	if ( CommonModule::_CurrentCommonModule == 0 ) 
		Ceylan::emergencyShutdown( 
			"OSDL::getExistingCommonModule: no common module available." ) ;
	
	return * CommonModule::_CurrentCommonModule ;
	
}



void OSDL::stop() throw()
{

	if ( CommonModule::_CurrentCommonModule == 0 )
	{
		LogPlug::error( 
			"OSDL::stop has been called whereas OSDL was not running." ) ;
	}	
	else
	{
		LogPlug::info( "Stopping launched OSDL module." ) ;
        
        // Will delete in turn all direct child modules (events, video, etc.):
		delete CommonModule::_CurrentCommonModule ;
		CommonModule::_CurrentCommonModule = 0 ;
	}
	
}

