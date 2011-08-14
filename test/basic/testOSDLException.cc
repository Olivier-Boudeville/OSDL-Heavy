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
 * Test of TestException throwing and catching.
 *
 */


int main( int argc, char * argv[] )
{

	LogHolder myLog( argc, argv ) ;

	try
	{

		LogPlug::info( "Testing OSDL exception handling" ) ;

		throw OSDL::TestException(
			"This exception has been explicitly raised." ) ;


		// Never reached :

		LogPlug::fatal( "Failed raising exception." ) ;
		return Ceylan::ExitFailure ;

	}

	catch ( const OSDL::Exception & e )
	{

		LogPlug::info( "End of OSDL exception handling test." ) ;

		OSDL::shutdown() ;

		// This is a success, from this test point of view.
		return Ceylan::ExitSuccess ;

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

	// Exception missed :
	return Ceylan::ExitFailure ;

}
