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


#ifndef OSDL_TILE_MAP_H_
#define OSDL_TILE_MAP_H_



#include "OSDLVideoRenderer.h"        // for VideoRenderingException
#include "OSDLVideoTypes.h"           // for Length
#include "OSDLPalette.h"              // for ColorIndex

#include "Ceylan.h"                   // for inheritance


#include <string>




namespace OSDL
{



	namespace Rendering 
	{
	
	

		/// Exception raised when a sprite operation failed.
		class OSDL_DLL TileMapException: public VideoRenderingException
		{
		
			public:
				explicit TileMapException( const std::string & message )  ;
				
				virtual ~TileMapException() throw() ;
							
		} ;




		/**
		 * Describes a number of tiles in a tilemap.
		 *
		 * @note This variable is in [0;255].
		 *
		 */
		typedef Ceylan::Uint8 TileCount ;



		/**
		 * Describes a number of tiles in a tile block, like a tile cache.
		 *
		 * @note This variable is in [0;65535].
		 *
		 */
		typedef Ceylan::Uint16 TileNumber ;




		/// Describes a tile index, in a stored tile map.
		typedef Ceylan::Uint8 TileIndex ;




		// Tile section.
		
		
		/// Describes a length (width or height) of a tile, in pixels.
		typedef Ceylan::Uint8 TileLength ;



		/// The width of a tile.
		const TileLength TileWidth = 8 ;      
			
				
		/// The height of a tile.
		const TileLength TileHeight = 8 ;      



		/**
		 * Describes a tile definition, a TileWidth x TileHeight matrix of 
		 * color index to a palette.
		 *
		 */
		typedef OSDL::Video::ColorIndex TileDefinition[TileWidth][TileHeight] ;


		
		
		/**
		 * A tilemap is a (rectangular) array of index to tile definitions.
		 *
		 */	
		class OSDL_DLL TileMap: public Ceylan::TextDisplayable
		{
		
		
			public:
			
				
			
				/**
				 * Constructs a new tilemap with specified dimensions, the map
				 * itself being read from specified file.
				 *
				 * @param width the width, in tiles, of the map.
				 *
				 * @param height the height, in tiles, of the map.
				 *
				 * @param mapFile a file whose read pointer is set at the 
				 * beginning of a serialized tile map of the specified
				 * dimensions. Ownership is not taken, after this constructor
				 * the file read pointer will just be advanced just after the
				 * read tilemap.
				 *
				 * @throw TileMapException if the operation failed, including
				 * if the appropriate data could not be read from file.
				 *
				 */
				TileMap( TileCount width, TileCount height,
					Ceylan::System::File & mapFile ) ;
				
				
				
				/**
				 * Virtual destructor.
				 *
				 */
				virtual ~TileMap() throw() ;
			


	            /**
	             * Returns an user-friendly description of the state 
				 * of this object.
	             *
				 * @param level the requested verbosity level.
				 *
				 * @note Text output format is determined from overall settings.
				 *
				 * @see Ceylan::TextDisplayable
	             *
	             */
		 		virtual const std::string toString( 
					Ceylan::VerbosityLevels level = Ceylan::high ) const ;

				
				
				
				
				
			protected:
			
			
			
				/// The width, in tiles, of this rectangular tile map.
				TileCount _width ;
				
				
				/// The height, in tiles, of this rectangular tile map.
				TileCount _height ;
				
				
				/**
				 * Number of tile index for this map.
				 * 
				 * @note Cached value for better performance.
				 *
				 */
				TileNumber _size ;
				
				
				/// The internal tile map (1D array of tile index).
				TileIndex * _map ;
						
			
				
						
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				TileMap( const TileMap & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				TileMap & operator = ( const TileMap & source ) ;
				
				
		} ;
		

	}

}



#endif // OSDL_TILE_MAP_H_

