/* 
 * Copyright (C) 2003-2009 Olivier Boudeville
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


#ifndef OSDL_RESOURCE_MANAGER_H_
#define OSDL_RESOURCE_MANAGER_H_


#include "OSDLDataCommon.h"     // for DataException, ResourceID


#include "OSDLMusic.h"          // for MusicCountedPtr
#include "OSDLSound.h"          // for SoundCountedPtr
#include "OSDLImage.h"          // for ImageCountedPtr


#include "Ceylan.h"             // for ResourceID


#include <string>
#include <list>
#include <map>




namespace OSDL
{


	// Forward declaration of all known resource types.
	
	
	namespace Data 
	{
	
		
		
		
		// The ResourceManager manages resources.
		class Resource ;


		/// Exception to be thrown when engine abnormal behaviour occurs.
		class OSDL_DLL ResourceManagerException : public DataException
		{
			public:
		
				explicit ResourceManagerException( 
					const std::string & reason ) ;
				
				virtual ~ResourceManagerException() throw() ;
			
		} ;



		/**
		 * Describes the various known content type for resources.
		 *
		 * This type does not reflect the encoding (ex: no '.wav' or '.ogg'),
		 * as a given encoding can be used in different contexts (ex: as a 
		 * sound or as a music), and as the actual encoding is generally 
		 * automatically determined by the back-end (ex: SDL_mixer) based 
		 * on the file content rather than based on its extension.
		 * Therefore we have usage-based types (ex: '.music', '.sound') rather
		 * than encoding-based extensions.
		 *
		 */
		enum ContentType
		{
		
			text,
			
			data,
			
			sound,
			
			music,
			
			image,
			
			ttf_font,
			
			unknown
			
		} ;
		

				
		/**
		 * This ResourceManager manages resources on behalf of the developer.
		 *
		 * The manager is able to keep track of resources, to load them and
		 * to make them available to any number of clients.
		 *
		 * To do so, the manager reads a resource map, usually from an XML
		 * file in an OSDL archive, and based on it maintains a set of 
		 * dictionaries, each of them corresponding to a kind of resource, like
		 * images, sounds, etc.
		 *
		 * A key of such a dictionary is a resource identifier, which is
		 * expected to correspond to a resource this dictionary manages.
		 *
		 * Each associated value is a Ceylan::LoadableWithContent instance,
		 * which records the path of the resource file and, if loaded, the
		 * resource itself.
		 *
		 * @see create-OSDL-archive.sh to understand how resources are indexed
		 * in an archive.
		 *
		 */	
		class OSDL_DLL ResourceManager: public Ceylan::Object
		{
		
		
			public:
			
			
				/** 
				 * Creates a resource manager, which will manage resources 
				 * listed in the resource map, whose filename is specified.
				 *
				 * @param resourceMapFilename the filename of the XML resource
				 * map to use; typically "resource-map.xml" if using the
				 * create-OSDL-archive.sh script.
				 *
				 */
				explicit ResourceManager( 
					const std::string & resourceMapFilename = 
						"resource-map.xml" ) ;
				
				
				
				/// Virtual destructor.
				virtual ~ResourceManager() throw() ;				 
			 
			 
			 
			 	/**
				 * Returns a counter pointer referencing the specified music.
				 *
				 * @param id the resource identifier for that music.
				 *
				 * @throw ResourceManagerException if the music could not be
				 * found.
				 *
				 */
			 	Audio::MusicCountedPtr getMusic( Ceylan::ResourceID id ) ; 
				 	
					
			 
			 	/**
				 * Returns a counter pointer referencing the specified sound.
				 *
				 * @param id the resource identifier for that sound.
				 *
				 * @throw ResourceManagerException if the sound could not be
				 * found.
				 *
				 */
			 	Audio::SoundCountedPtr getSound( Ceylan::ResourceID id ) ; 
				 	
					
					
			 	/**
				 * Returns a counter pointer referencing the specified image.
				 *
				 * @param id the resource identifier for that image.
				 *
				 * @throw ResourceManagerException if the image could not be
				 * found.
				 *
				 */
			 	Video::TwoDimensional::ImageCountedPtr 
					getImage( Ceylan::ResourceID id ) ; 
				 	
					
										
	            /**
	             * Returns an user-friendly description of the state of 
				 * this object.
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





				// Static section.


				/**
				 * Returns the content type corresponding to the specified
				 * stringified version.
				 *
				 * @param stringifiedType the string to interpret.
				 *
				 * @param throwIfNotMatched if true, if the context type could
				 * not be determined, throws a ResourceManagerException rather
				 * than returning the 'unknown' content type.
				 *
				 */
				static ContentType GetContentType( 
					const std::string & stringifiedType, 
					bool throwIfNotMatched = true ) ;

					
					
			protected:
			


				/** 
				 * Registers a new resource, as specified by the XML resource
				 * entry.
				 *
				 */
				void registerResource( 
					const Ceylan::XML::XMLParser::XMLTree & resourceXMLEntry ) ;
				


				// Variable section.
				
				

/* 
 * Takes care of the awful issue of Windows DLL with templates.
 *
 * @see Ceylan's developer guide and README-build-for-windows.txt 
 * to understand it, and to be aware of the associated risks. 
 * 
 */
#pragma warning( push )
#pragma warning( disable: 4251 )
						
				
				/**
				 * Dictionary for music resources. 
				 *
				 * Each key of the map is a resource identifier, which is
				 * expected to correspond to a music resource.
				 *
				 * Each associated value is a counted pointer on a
				 * Ceylan::LoadableWithContent instance (here, a music), 
				 * which records the path of the resource file and, if loaded,
				 * the resource itself.
				 *
				 */
				std::map<Ceylan::ResourceID, Audio::MusicCountedPtr> _musicMap ;
				
				
				
				/**
				 * Dictionary for sound resources. 
				 *
				 * Each key of the map is a resource identifier, which is
				 * expected to correspond to a sound resource.
				 *
				 * Each associated value is a counted pointer on a
				 * Ceylan::LoadableWithContent instance (here, a sound), 
				 * which records the path of the resource file and, if loaded,
				 * the resource itself.
				 *
				 */
				std::map<Ceylan::ResourceID, Audio::SoundCountedPtr> _soundMap ;
				
				
				
				/**
				 * Dictionary for image resources. 
				 *
				 * Each key of the map is a resource identifier, which is
				 * expected to correspond to a image resource.
				 *
				 * Each associated value is a counted pointer on a
				 * Ceylan::LoadableWithContent instance (here, a image), 
				 * which records the path of the resource file and, if loaded,
				 * the resource itself.
				 *
				 */
				std::map<Ceylan::ResourceID,
					Video::TwoDimensional::ImageCountedPtr> _imageMap ;
				
				
#pragma warning( pop ) 

				
				 
			private:
			
			
			
				/**
				 * Copy constructor made private to ensure that it will 
				 * never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * constructor is called, implicitly or not.
				 * 
				 */			 
				explicit ResourceManager( const ResourceManager & source ) ;
			
			
			
				/**
				 * Assignment operator made private to ensure that it 
				 * will never be called.
				 *
				 * The compiler should complain whenever this undefined 
				 * operator is called, implicitly or not.
				 * 
				 */			 
				ResourceManager & operator = ( 
					const ResourceManager & source ) ;
					
							
				
		} ;
		

	}

}



#endif // OSDL_RESOURCE_MANAGER_H_

