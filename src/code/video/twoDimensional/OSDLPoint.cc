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


#include "OSDLPoint.h"


#include <iostream>      // for ostream


using std::string ;
using std::list ;


using namespace OSDL::Video::TwoDimensional ;



Point::Point()
{

}


	
Point::~Point() throw()
{

}



const string Point::toString( Ceylan::VerbosityLevels level ) const
{

    return "Abstract point" ;
	
}



std::ostream & operator << ( std::ostream & os, const Point & p )
{

    return os << p.toString() ;
	
}

