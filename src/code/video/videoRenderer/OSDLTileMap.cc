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


#include "OSDLTileMap.h"

#include "Ceylan.h"       // for File, etc.

#include <list>


using namespace Ceylan::Log ;
using namespace Ceylan::System ;

using namespace OSDL::Rendering ;
using namespace OSDL::Video ;

using std::string ;


#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>     // for OSDL_DEBUG_SPRITE and al 
#endif // OSDL_USES_CONFIG_H

#if OSDL_ARCH_NINTENDO_DS
#include "OSDLConfigForNintendoDS.h" // for OSDL_USES_SDL and al
#endif // OSDL_ARCH_NINTENDO_DS





TileMapException::TileMapException( const string & message ) : 
	VideoRenderingException( "TileMap exception: " + message ) 
{

}
	
	
			
TileMapException::~TileMapException() throw()
{
		
}




TileMap::TileMap( TileCount width, TileCount height,
		Ceylan::System::File & mapFile ) :
	_width( width ),
	_height( height ),
	_size(_width*_height)
{

	// No array with more than one non-constant dimension possible in C/C++:
	_map = new TileIndex[_size] ;
	
	try
	{
	
		// TileNumber, not too small TileCount: 
		for ( TileNumber i = 0; i < _size; i++ )
		{
	
			_map[i] = mapFile.readUint8() ;
			
		}
	
	}
	catch( const Ceylan::Exception & e )
	{
	
		throw TileMapException( 
			"TileMap constructor: reading of the map failed: " 
			+ e.toString() ) ;
	}	
}



TileMap::~TileMap() throw()
{

	if ( _map != 0 )
		delete [] _map ;
		
}



const string TileMap::toString( Ceylan::VerbosityLevels level ) const 
{

	string res = "TileMap whose dimensions are " + Ceylan::toString( _width )
		+ "x" + Ceylan::toString( _height ) ;
	
	if ( level == Ceylan::low )
		return res ;
	
	res += ". Map content is: " ;
	
	std::list<string> l ;
		
	for ( TileCount j = 0; j < _height; j++ )
	{
		for ( TileCount i = 0; i < _width; i++ )
		{
	
			// TO-DO!	
		
		}
		
		
	}
	
	return res + Ceylan::formatStringList( l ) ;
	
}

