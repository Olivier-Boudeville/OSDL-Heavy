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

#include <iostream>

#include <string>
using std::string ;

#include <list>
using std::list ;

#include <exception>


using namespace Ceylan ;
using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL ;
using namespace OSDL::Audio ;




/**
 * Test for the support of video offered by the OSDL library on the Nintendo DS.
 *
 */
int main( int argc, char * argv[] )
{

	// To avoid using a screen for the logs:
	LogHolder myLog( argc, argv, /* immediateWrite */ false ) ;


	try
	{


		// For the test:
		bool interactive = true ;
		//bool interactive = false ;

		bool testFailed = false ;

		LogPlug::info( "Test of OSDL support for video." ) ;


		CommonModule & myOSDL = OSDL::getCommonModule(
			CommonModule::UseVideo ) ;

		LogPlug::info( "OSDL state: " + myOSDL.toString() ) ;

		//LogPlug::info( "Loading




		if ( testFailed )
			throw OSDL::TestException( "Test failed because of error(s) "
				"previously displayed." ) ;

		if ( interactive )
		{

			LogPlug::info( "Press any key to end the test" ) ;
			waitForKey() ;

		}

		// LogHolder out of scope: log browser triggered.

	}

	catch ( const Ceylan::Exception & e )
	{

		LogPlug::error( "Ceylan exception caught: "
			+ e.toString( Ceylan::high ) ) ;

		return Ceylan::ExitFailure ;

	}

	catch ( const std::exception & e )
	{

		LogPlug::error( string( "Standard exception caught: " ) + e.what() ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( ... )
	{

		LogPlug::error( "Unknown exception caught" ) ;
		return Ceylan::ExitFailure ;

	}

	LogPlug::info( "Exit on success (no error)" ) ;

	OSDL::shutdown() ;

	return Ceylan::ExitSuccess ;

}
