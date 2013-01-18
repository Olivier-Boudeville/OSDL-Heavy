/*
 * Copyright (C) 2003-2013 Olivier Boudeville
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
using namespace OSDL::Video::TwoDimensional ;

using namespace Ceylan::Log ;

#include <string>



/**
 * Small usage tests for Point2D.
 *
 */
int main( int argc, char * argv[] )
{

  {

	LogHolder myLog( argc, argv ) ;


	try
	{

	  LogPlug::info( "Testing OSDL Point2D." ) ;

	  Point2D p1( static_cast<OSDL::Video::Coordinate>( 14 ), 34 ) ;

	  LogPlug::info( "Point located at ( 14 ; 34 ) displays as "
		+ p1.toString() ) ;

	  LogPlug::info( "Getting first coordinate: "
		+ Ceylan::toString( p1.getX() ) ) ;

	  LogPlug::info( "Getting second coordinate: "
		+ Ceylan::toString( p1.getY() ) ) ;

	  LogPlug::info( "Setting first coordinate: 11 " ) ;
	  p1.setX( 11 ) ;

	  LogPlug::info( "Setting second coordinate: 108 " ) ;
	  p1.setY( 108 ) ;

	  LogPlug::info( "Getting first coordinate: "
		+ Ceylan::toString( p1.getX() ) ) ;
	  LogPlug::info( "Getting second coordinate: "
		+ Ceylan::toString( p1.getY() ) ) ;

	  LogPlug::info( "Point now displays as "
		+ p1.toString() ) ;

	  // OSDL not launched hence not stopped.

	  LogPlug::info( "End of OSDL Point2D test." ) ;

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

  }

  OSDL::shutdown() ;

  return Ceylan::ExitSuccess ;

}
