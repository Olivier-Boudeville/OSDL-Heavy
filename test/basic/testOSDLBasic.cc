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

using namespace Ceylan::Log ;



/**
 * Real test for the basic module.
 *
 * Retrieves OSDL root module.
 *
 */
int main( int argc, char * argv[] )
{


	/*
	 * Verifies that the versions of the OSDL header files used to compile this
	 * test and the one of the currently linked OSDL library are compatible:
	 *
	 */
	CHECK_OSDL_VERSIONS() ;


	LogHolder myLog( argc, argv ) ;


	try
	{

		LogPlug::info( "Testing OSDL basic services." ) ;


		OSDL::CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseVideo | CommonModule::UseKeyboard ) ;

		LogPlug::info( myOSDL.toString() ) ;

		LogPlug::info( "This test has been compiled against the "
			+ Ceylan::LibtoolVersion(
				OSDL::actualOSDLHeaderLibtoolVersion ).toString()
			+ " version of the OSDL header files." ) ;

		LogPlug::debug( "One can check that all these constants, "
			"(except CommonModule::UseEverything), have no bit in common"
			"have no two same bit set to one:" ) ;


		LogPlug::debug( "CommonModule::UseTimer is equal to: "
			+ Ceylan::toString( CommonModule::UseTimer,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "CommonModule::UseAudio is equal to      : "
			+ Ceylan::toString( CommonModule::UseAudio,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "CommonModule::UseVideo is equal to      : "
			+ Ceylan::toString( CommonModule::UseVideo,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "CommonModule::UseCDROM is equal to      : "
			+ Ceylan::toString( CommonModule::UseCDROM,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "UseJoystick is equal to   : "
			+ Ceylan::toString( CommonModule::UseJoystick,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "UseKeyboard is equal to                : "
			+ Ceylan::toString( CommonModule::UseKeyboard,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "UseMouse is equal to                   : "
			+ Ceylan::toString( CommonModule::UseMouse,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "UseEverything is equal to : "
			+ Ceylan::toString( CommonModule::UseEverything,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "NoParachute is equal to: "
			+ Ceylan::toString( CommonModule::NoParachute,
				/* bitfield */ true ) ) ;

		LogPlug::debug( "UseEventThread is equal to: "
			+ Ceylan::toString( CommonModule::UseEventThread,
				/* bitfield */ true ) ) ;


		OSDL::stop() ;

		LogPlug::info( "End of OSDL basic test." ) ;


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

	OSDL::shutdown() ;

	return Ceylan::ExitSuccess ;

}
