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


#include "OSDLResourceManager.h"

#include "OSDLMusic.h"                // for Music constructor
#include "OSDLSound.h"                // for Sound constructor


using namespace Ceylan::Log ;         // for LogPlug


using namespace OSDL ;
using namespace OSDL::Data ;

using std::string ;
using std::list ;
using std::map ;



#ifdef OSDL_USES_CONFIG_H
#include <OSDLConfig.h>               // for OSDL_DEBUG_RESOURCE_MANAGER and al
#endif // OSDL_USES_CONFIG_H



#if OSDL_DEBUG_RESOURCE_MANAGER
 
#define OSDL_DATA_LOG(message) send( message )

#else // OSDL_DEBUG_RESOURCE_MANAGER

#define OSDL_DATA_LOG(message)

#endif // OSDL_DEBUG_RESOURCE_MANAGER




/*
 * Implementation notes:
 *
 * The management of the resource life-cycle is not obvious; their reference
 * count is maintained and they are deallocated when not referenced any more.
 *
 * The resource manager owns a reference on each resource, therefore when
 * their refcount is 1 then no third-party references them, and the manager
 * may or may not (if wanting to cache resources) unload them, if they are
 * loaded.
 *
 * The reference counts have been successfully checked 
 * (see testOSDLResourceManager.cc)
 *
 * Note also that counted pointers have always to be passed by value, not  
 * by address, otherwise the reference count will be wrong.
 *
 * Splitting resources into a set of typed maps allows faster look-ups and
 * removes the need of casts.
 *
 */


ResourceManagerException::ResourceManagerException( const string & reason ) :
	DataException( reason )
{

}


ResourceManagerException::~ResourceManagerException() throw()
{

}






// ResourceManager section.



// For the sake of clarity:
typedef std::list<Ceylan::XML::XMLParser::XMLTree * > XMLSubtreeList ;



ResourceManager::ResourceManager( const string & resourceMapFilename )
{

	send( "Creating a ResourceManager based on resource map in '"
		+ resourceMapFilename + "'." ) ;
	
	Ceylan::XML::XMLParser * resourceParser = 
		new Ceylan::XML::XMLParser( resourceMapFilename ) ;
	
	send( resourceParser->toString() ) ;
	
	resourceParser->loadFromFile() ;
	
	send( resourceParser->toString() ) ;
	
	Ceylan::XML::XMLParser::XMLTree & root = resourceParser->getXMLTree() ;

	send( "Inserting resource defined in: " + root.toString() ) ;
	
	// Sons are directly the resource entries:
	const XMLSubtreeList & sons = root.getSons() ;
	
	for ( XMLSubtreeList::const_iterator it = sons.begin(); 
			it != sons.end(); it++ )
		registerResource( *(*it) ) ;
	
	send( "All resources inserted." ) ;
		
	delete resourceParser ;
	
	dropIdentifier() ;

}



ResourceManager::~ResourceManager() throw()
{

	/*
	 * When the manager is deallocated, its map are deallocated, leading to
	 * refcount for resources dropping to zero (if not referenced by any
	 * third party). Resources are then deallocated.
	 *
	 */
}



Audio::MusicCountedPtr ResourceManager::getMusic( Ceylan::ResourceID id )
{

	std::map<Ceylan::ResourceID, Audio::MusicCountedPtr>::iterator it =
		_musicMap.find( id ) ;
		
	if ( it == _musicMap.end() )
		throw ResourceManagerException( "ResourceManager::getMusic: id #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Audio::MusicCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Audio::SoundCountedPtr ResourceManager::getSound( Ceylan::ResourceID id )
{

	std::map<Ceylan::ResourceID, Audio::SoundCountedPtr>::iterator it =
		_soundMap.find( id ) ;
		
	if ( it == _soundMap.end() )
		throw ResourceManagerException( "ResourceManager::getSound: id #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Audio::SoundCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Video::TwoDimensional::ImageCountedPtr ResourceManager::getImage(
	Ceylan::ResourceID id )
{

	std::map<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>::iterator it =
		_imageMap.find( id ) ;
		
	if ( it == _imageMap.end() )
		throw ResourceManagerException( "ResourceManager::getImage: id #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Video::TwoDimensional::ImageCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



const string ResourceManager::toString( Ceylan::VerbosityLevels level ) const 
{	

	list<string> maps ;
	
	Ceylan::Uint32 size = _musicMap.size() ;
	
	string temp ;
	
	if ( size != 0 )
	{
	
		temp = "Number of musics managed: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> musics ;
			
			for ( std::map<Ceylan::ResourceID,
				Audio::MusicCountedPtr>::const_iterator it = _musicMap.begin();
					it != _musicMap.end(); it++ )
			{
			
				musics.push_back( (*it).second->toString(level)
					+ "', and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( musics ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		 
	}
	else
	{

		maps.push_back( "No music is managed" ) ;
	
	}
	
	
	size = _soundMap.size() ;
	
	if ( size != 0 )
	{
	
		temp = "Number of sounds managed: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> sounds ;
			
			for ( std::map<Ceylan::ResourceID,
				Audio::SoundCountedPtr>::const_iterator it = _soundMap.begin();
					it != _soundMap.end(); it++ )
			{
			
				sounds.push_back( (*it).second->toString(level)
					+ "', and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( sounds ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No sound is managed" ) ;
	
	}
	
	
	size = _imageMap.size() ;
	
	if ( size != 0 )
	{
	
		temp = "Number of images managed: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> images ;
			
			for ( std::map<Ceylan::ResourceID,
				Video::TwoDimensional::ImageCountedPtr>::const_iterator it =
					 _imageMap.begin();	it != _imageMap.end(); it++ )
			{
			
				images.push_back( (*it).second->toString(level)
					+ "', and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( images ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No image is managed" ) ;
	
	}
	
	
	return "ResourceManager state: " + Ceylan::formatStringList( maps ) ;
					
}





// Static section.





// Protected members below:


void ResourceManager::registerResource( 
	const Ceylan::XML::XMLParser::XMLTree & resourceXMLEntry )
{
	
	const Ceylan::XML::XMLMarkup * const resourceMarkup =
		dynamic_cast<const Ceylan::XML::XMLMarkup*>(
			& resourceXMLEntry.getContentAsConst() ) ;
	
	if ( resourceMarkup == 0 )
		throw ResourceManagerException( "ResourceManager::registerResource: "
			"expected resource mark-up not found." ) ;
			
	Ceylan::ResourceID id = static_cast<Ceylan::ResourceID>(
		Ceylan::stringToUnsignedLong(
			resourceMarkup->getExistingAttribute( "id" ) ) ) ;
	
	//send( "id = " + Ceylan::toString( id ) ) ;
	
	const XMLSubtreeList & resourceSettingEntries = resourceXMLEntry.getSons() ;

	string resourcePath, resourceStringifiedType ;
	
	const Ceylan::XML::XMLMarkup * currentMarkup ;
	
	for ( XMLSubtreeList::const_iterator it = resourceSettingEntries.begin(); 
		it != resourceSettingEntries.end(); it++ )
	{
	
		currentMarkup = dynamic_cast<const Ceylan::XML::XMLMarkup*>(
			& (*it)->getContentAsConst() ) ;

		if ( currentMarkup == 0 )
			throw ResourceManagerException( 
				"ResourceManager::registerResource: "
				"expected setting mark-up not found." ) ;
				
		if ( currentMarkup->getMarkupName() == "resource_path" )
		{
		
			const Ceylan::XML::XMLText * resourceText =
				dynamic_cast<const Ceylan::XML::XMLText*>(
					& (*it)->getSons().front()->getContent() ) ;
			
			if ( resourceText == 0 ) 	
				throw ResourceManagerException( 
					"ResourceManager::registerResource: "
					"expected resource path not found." ) ;
					
			resourcePath = resourceText->toString() ;
		
		}
		else if ( currentMarkup->getMarkupName() == "content_type" )
		{
		
			const Ceylan::XML::XMLText * resourceContentType =
				dynamic_cast<const Ceylan::XML::XMLText*>(
					& (*it)->getSons().front()->getContent() ) ;
			
			if ( resourceContentType == 0 ) 	
				throw ResourceManagerException( 
					"ResourceManager::registerResource: "
					"expected resource content type not found." ) ;
					
			resourceStringifiedType = resourceContentType->toString() ;
		
		}
		else
		{
		
			LogPlug::warning( "ResourceManager::registerResource: "
				"mark-up setting '" + currentMarkup->getMarkupName() 
				+ "' ignored." ) ;
		}
		
		
	}
	
	if ( resourcePath.empty() )
		throw ResourceManagerException( 
			"ResourceManager::registerResource: "
			"no resource path could be found." ) ;
	
	if ( resourceStringifiedType.empty() )
		throw ResourceManagerException( 
			"ResourceManager::registerResource: "
			"no resource content type could be found." ) ;
	
	ContentType resourceType = GetContentType( resourceStringifiedType ) ;
	
	if ( resourceType == Data::music )
	{
	
		Audio::MusicCountedPtr newMusicPtr = 
			new Audio::Music( resourcePath, /* preload */ false ) ;
	
		// Reference count is 1 here.	
		
		/*
		send( "Inserting into music dictionary: '" + newMusicPtr->toString() 
			+ "', with refcount = " 
			+ Ceylan::toString( newMusicPtr.getReferenceCount() ) + "." ) ;
		 */
		 
		/*
		 * Not kept as apparently induces useless reference counts changes:
		 * (2->1->2 instead of staying at 2 with an insert)
		   
		_musicMap[ id ] = newMusicPtr ;
		
		 */
		 
		_musicMap.insert( 
			std::pair<Ceylan::ResourceID,Audio::MusicCountedPtr>( 
				id, newMusicPtr ) ) ;

		// Reference count is 2 here.	

		/*
		send( "Inserted into music dictionary: '" + newMusicPtr->toString() 
			+ "', with refcount = " 
			+ Ceylan::toString( newMusicPtr.getReferenceCount() ) + "." ) ;
		 */
		 
	}
	else if ( resourceType == Data::sound )
	{
	
		Audio::SoundCountedPtr newSoundPtr = 
			new Audio::Sound( resourcePath, /* preload */ false ) ;
			 
		_soundMap.insert( 
			std::pair<Ceylan::ResourceID,Audio::SoundCountedPtr>( 
				id, newSoundPtr ) ) ;
		 
	}
	else if ( resourceType == Data::image )
	{
	
		// Conversion to display to screen format could be managed here:
		
		Video::TwoDimensional::ImageCountedPtr newImagePtr = 
			new Video::TwoDimensional::Image( resourcePath, 
				/* preload */ false, /* convertToDisplayFormat */ true, 
				/* convertWithAlpha */ true ) ;
			 
		_imageMap.insert( std::pair<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>( id, newImagePtr ) ) ;

		 
	}
	else
	{
	
		send( "Resource in '" + resourcePath + "' has for type '" 
			+ resourceStringifiedType + "', which is not managed." ) ;
	
		return ;
		
	}
	
	
	//send( "Resource inserted." ) ;
	
	// Reference count is 1 here.
		
}



ContentType ResourceManager::GetContentType( 
	const std::string & stringifiedType, bool throwIfNotMatched )
{

	if ( stringifiedType == "text" )
		return Data::text ;
	else if	( stringifiedType == "data" )
		return Data::data ;
	else if	( stringifiedType == "sound" )
		return Data::sound ;
	else if	( stringifiedType == "music" )
		return Data::music ;
	else if	( stringifiedType == "image" )
		return Data::image ;
	else if	( stringifiedType == "ttf_font" )
		return Data::ttf_font ;
	else
	{
	
		// Not found:
		if ( throwIfNotMatched )
			throw ResourceManagerException( "ResourceManager::GetContentType: "
				"the string '" + stringifiedType 
				+ "' could not be interpreted." ) ;
		else
			return Data::unknown ;
					
	}
		
}

	