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
#include "OSDLGLTexture.h"            // for GLTexture constructor


using namespace Ceylan ;              // for ResourceID
using namespace Ceylan::Log ;         // for LogPlug


using namespace OSDL ;
using namespace OSDL::Data ;
using namespace OSDL::Video::TwoDimensional ;

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


Data::ResourceManagerException::ResourceManagerException( 
		const string & reason ) :
	DataException( reason )
{

}


Data::ResourceManagerException::~ResourceManagerException() throw()
{

}






// ResourceManager section.



// For the sake of clarity:
typedef std::list<Ceylan::XML::XMLParser::XMLTree * > XMLSubtreeList ;



Data::ResourceManager::ResourceManager( const string & resourceMapFilename ):
	_maxID( 0 )
{

	send( "Creating a ResourceManager based on the resource map in file '"
		+ resourceMapFilename + "'." ) ;
	
	Ceylan::XML::XMLParser * resourceParser = 
		new Ceylan::XML::XMLParser( resourceMapFilename ) ;
	
	//send( resourceParser->toString() ) ;
	
	resourceParser->loadFromFile() ;
	
	//send( resourceParser->toString() ) ;
	
	Ceylan::XML::XMLParser::XMLTree & root = resourceParser->getXMLTree() ;

	//send( "Inserting resource defined in: " + root.toString() ) ;
	
	// Sons are directly the resource entries:
	const XMLSubtreeList & sons = root.getSons() ;
	
	for ( XMLSubtreeList::const_iterator it = sons.begin(); 
			it != sons.end(); it++ )
		registerResource( *(*it) ) ;
	
	send( "All resources inserted, initial state is: " + toString() ) ;
		
	delete resourceParser ;
	
	dropIdentifier() ;

}



Data::ResourceManager::~ResourceManager() throw()
{

	/*
	 * When the manager is deallocated, its maps are deallocated, leading to
	 * refcount for resources dropping to zero (if not still referenced by any
	 * third party). Resources are then automatically deallocated.
	 *
	 */
	
	send( "Deleting ResourceManager, whose final state was: "
		+ toString() ) ;
		 
}



Audio::MusicCountedPtr Data::ResourceManager::getMusic( Ceylan::ResourceID id )
{

	map<Ceylan::ResourceID, Audio::MusicCountedPtr>::iterator it =
		_musicMap.find( id ) ;
		
	if ( it == _musicMap.end() )
		throw ResourceManagerException( "ResourceManager::getMusic: ID #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Audio::MusicCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Audio::MusicCountedPtr Data::ResourceManager::getMusic( 
	const string & musicPath )
{

	map<Ceylan::ResourceID, Audio::MusicCountedPtr>::iterator it =
		_musicMap.find( getIDForPath(musicPath) ) ;
		
	if ( it == _musicMap.end() )
		throw ResourceManagerException( 
			"ResourceManager::getMusic: music path '" + musicPath 
			+ "' could not be found." ) ;
			
	Audio::MusicCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}




Audio::SoundCountedPtr Data::ResourceManager::getSound( Ceylan::ResourceID id )
{

	
	map<Ceylan::ResourceID, Audio::SoundCountedPtr>::iterator it =
		_soundMap.find( id ) ;
		
	if ( it == _soundMap.end() )
		throw ResourceManagerException( "ResourceManager::getSound: ID #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Audio::SoundCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Audio::SoundCountedPtr Data::ResourceManager::getSound( 
	const string & soundPath )
{
	
	map<Ceylan::ResourceID, Audio::SoundCountedPtr>::iterator it =
		_soundMap.find( getIDForPath(soundPath) ) ;
		
	if ( it == _soundMap.end() )
		throw ResourceManagerException( 
			"ResourceManager::getSound: sound path '" + soundPath 
			+ "' could not be found." ) ;	
				
	Audio::SoundCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}




Video::TwoDimensional::ImageCountedPtr Data::ResourceManager::getImage(
	Ceylan::ResourceID id )
{

	map<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>::iterator it =
		_imageMap.find( id ) ;
		
	if ( it == _imageMap.end() )
		throw ResourceManagerException( "ResourceManager::getImage: ID #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Video::TwoDimensional::ImageCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Video::TwoDimensional::ImageCountedPtr Data::ResourceManager::getImage( 
	const string & imagePath )
{

	map<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>::iterator it =
		_imageMap.find( getIDForPath(imagePath) ) ;
		
	if ( it == _imageMap.end() )
		throw ResourceManagerException( "ResourceManager::getImage: image '"
			+ imagePath + "' could not be found." ) ;
			
	Video::TwoDimensional::ImageCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Video::OpenGL::TextureCountedPtr Data::ResourceManager::getTexture(
	Ceylan::ResourceID id, bool uploadWanted )
{

	map<Ceylan::ResourceID,Video::OpenGL::TextureCountedPtr>::iterator it =
		_textureMap.find( id ) ;
		
	if ( it == _textureMap.end() )
	{
	
		LogPlug::fatal( "Data::ResourceManager::getTexture for ID failed." ) ;
		
		throw ResourceManagerException( "ResourceManager::getTexture: ID #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
	
	}
	
			
	Video::OpenGL::TextureCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	// Maybe wanting to have it uploaded directly to the video card?
	if ( uploadWanted && ( ! res->wasUploaded() ) )
		res->upload() ;
		
	return res ;
			
}



Video::OpenGL::TextureCountedPtr Data::ResourceManager::getTexture( 
	const string & texturePath, bool uploadWanted )
{

	map<Ceylan::ResourceID,Video::OpenGL::TextureCountedPtr>::iterator it =
		_textureMap.find( getIDForPath(texturePath) ) ;
		
	if ( it == _textureMap.end() )
	{
	
		LogPlug::fatal( "Data::ResourceManager::getTexture for file '"
			+ texturePath + "' failed." ) ;

		throw ResourceManagerException( "ResourceManager::getTexture: texture '"
			+ texturePath + "' could not be found." ) ;
	
	}
	
			
	Video::OpenGL::TextureCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	// Maybe wanting to have it uploaded directly to the video card?
	if ( uploadWanted && ( ! res->wasUploaded() ) )
		res->upload() ;
		
	return res ;
			
}



std::pair<Video::OpenGL::TextureCountedPtr,Ceylan::ResourceID>
	Data::ResourceManager::getTextureFrom( 
		Video::Surface & sourceSurface, 
		Video::OpenGL::GLTexture::TextureFlavour flavour, 
		bool uploadWanted )
{

	Video::OpenGL::TextureCountedPtr resPtr = new Video::OpenGL::GLTexture(
		sourceSurface, flavour ) ;

	// Maybe wanting to have it uploaded directly to the video card?
	if ( uploadWanted && ( ! resPtr->wasUploaded() ) )
		resPtr->upload() ;

	_maxID++ ;

	Ceylan::ResourceID resID = _maxID ;
			 
	_textureMap.insert( std::pair<Ceylan::ResourceID,
			Video::OpenGL::TextureCountedPtr>( resID, resPtr ) ) ;
	
	return std::pair<Video::OpenGL::TextureCountedPtr,Ceylan::ResourceID>( 
		resPtr, resID ) ;
	
}
		
					

Video::TwoDimensional::Text::TrueTypeFontCountedPtr
	Data::ResourceManager::getTrueTypeFont( Ceylan::ResourceID id )
{

	map<ResourceID,Text::TrueTypeFontCountedPtr>::iterator it = 
		_truetypeFontMap.find( id ) ;
		
	if ( it == _truetypeFontMap.end() )
		throw ResourceManagerException( "ResourceManager::getTrueTypeFont: ID #"
			+ Ceylan::toString( id ) + " could not be found." ) ;
			
	Video::TwoDimensional::Text::TrueTypeFontCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
	
	return res ;
			
}



Video::TwoDimensional::Text::TrueTypeFontCountedPtr
	Data::ResourceManager::getTrueTypeFont( const string & fontPath )
{

	map<ResourceID,Text::TrueTypeFontCountedPtr>::iterator it =
		_truetypeFontMap.find( getIDForPath(fontPath) ) ;
		
	if ( it == _truetypeFontMap.end() )
		throw ResourceManagerException( "ResourceManager::getTrueTypeFont: "
			"font '" + fontPath + "' could not be found." ) ;
			
	Text::TrueTypeFontCountedPtr res = (*it).second ;
	
	// Ensures a returned resource is always loaded:
	res->load() ;
			
	return res ;
			
}



void Data::ResourceManager::discardTexture( Ceylan::ResourceID textureId )
{

	map<Ceylan::ResourceID,Video::OpenGL::TextureCountedPtr>::iterator it =
		_textureMap.find( textureId ) ;
		
	if ( it == _textureMap.end() )
		throw ResourceManagerException( "ResourceManager::discardTexture: "
			"no texture with ID #" + Ceylan::toString( textureId ) 
			+ " found." ) ;
	
	// Ensures the texture is removed from video card and deleted:
	_textureMap.erase( it ) ;
	
}



void Data::ResourceManager::purge()
{

	for ( map<Ceylan::ResourceID, Audio::MusicCountedPtr>::iterator it =
		_musicMap.begin(); it != _musicMap.end(); it++ )
	{
	
		if ( ((*it).second).getReferenceCount() == 1 )
			((*it).second)->unload() ;
			
	}
	
		
	for ( map<Ceylan::ResourceID, Audio::SoundCountedPtr>::iterator it =
		_soundMap.begin(); it != _soundMap.end(); it++ )
	{
	
		if ( ((*it).second).getReferenceCount() == 1 )
			((*it).second)->unload() ;
			
	}
		
		
	for ( map<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>::iterator it =
		_imageMap.begin(); it != _imageMap.end(); it++ )
	{
	
		if ( ((*it).second).getReferenceCount() == 1 )
			((*it).second)->unload() ;
			
	}
		
		
	for ( map<Ceylan::ResourceID,
			Video::OpenGL::TextureCountedPtr>::iterator it =
		_textureMap.begin(); it != _textureMap.end(); it++ )
	{
	
		if ( ((*it).second).getReferenceCount() == 1 )
			((*it).second)->unload() ;
			
	}
		
		
	for ( map<Ceylan::ResourceID,
			Video::TwoDimensional::Text::TrueTypeFontCountedPtr>::iterator it =
		_truetypeFontMap.begin(); it != _truetypeFontMap.end(); it++ )
	{
	
		if ( ((*it).second).getReferenceCount() == 1 )
			((*it).second)->unload() ;
			
	}
	
	
	/*
	Ceylan::checkpoint( "ResourceManager is now purged, new state: " ) ;
	displayLoadedResources() ;
	 */
	 	
}



void Data::ResourceManager::displayLoadedResources()
{

	Ceylan::VerbosityLevels level = Ceylan::high ;
	
	list<string> loadedList ;
	
	for ( map<Ceylan::ResourceID,
			Audio::MusicCountedPtr>::const_iterator it = _musicMap.begin();
		it != _musicMap.end(); it++ )
	{
		
		if ( ((*it).second)->hasContent() )
			loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
				+ ": " + (*it).second->toString(level)
				+ ") and whose reference count is " 
				+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
	}	
			
			
	for ( map<Ceylan::ResourceID,
			Audio::SoundCountedPtr>::const_iterator it = _soundMap.begin();
		it != _soundMap.end(); it++ )
	{
		
		if ( ((*it).second)->hasContent() )
			loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
				+ ": " + (*it).second->toString(level)
				+ ") and whose reference count is " 
				+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
	}	
			
			
	for ( map<Ceylan::ResourceID,
		Video::TwoDimensional::ImageCountedPtr>::const_iterator it =
			_imageMap.begin(); it != _imageMap.end(); it++ )
	{
		
		if ( ((*it).second)->hasContent() )
			loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
				+ ": " + (*it).second->toString(level)
				+ ") and whose reference count is " 
				+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
	}	
		
			
	for ( map<Ceylan::ResourceID,
		Video::OpenGL::TextureCountedPtr>::const_iterator it =
			_textureMap.begin(); it != _textureMap.end(); it++ )
	{
		
		if ( ((*it).second)->hasContent() )
			loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
				+ ": " + (*it).second->toString(level)
				+ ") and whose reference count is " 
				+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
	}	
		
			
	for ( map<Ceylan::ResourceID,
		Text::TrueTypeFontCountedPtr>::const_iterator it =
			_truetypeFontMap.begin(); it != _truetypeFontMap.end(); it++ )
	{
		
		if ( ((*it).second)->hasContent() )
			loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
				+ ": " + (*it).second->toString(level)
				+ ") and whose reference count is " 
				+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
	}	
			
	if ( loadedList.empty() )
	{
	
		Ceylan::checkpoint( "Resource manager: no resource loaded." ) ;
	
	}	
	else
	{
	
		Ceylan::checkpoint( "Resource manager: "
			+ Ceylan::formatStringList( loadedList, 
				TextDisplayable::rawText ) ) ;
	
	}		
		
}



const string Data::ResourceManager::toString( 
	Ceylan::VerbosityLevels level ) const 
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
			
			for ( map<Ceylan::ResourceID,
				Audio::MusicCountedPtr>::const_iterator it = _musicMap.begin();
					it != _musicMap.end(); it++ )
			{
			
				musics.push_back( (*it).second->toString(level)
					+ " (ID #" + Ceylan::toString( (*it).first )
					+ ") and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( musics,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
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
			
			for ( map<Ceylan::ResourceID,
				Audio::SoundCountedPtr>::const_iterator it = _soundMap.begin();
					it != _soundMap.end(); it++ )
			{
			
				sounds.push_back( (*it).second->toString(level)
					+ " (ID #" + Ceylan::toString( (*it).first )
					+ ") and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( sounds,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
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
			
			for ( map<Ceylan::ResourceID,
				Video::TwoDimensional::ImageCountedPtr>::const_iterator it =
					 _imageMap.begin(); it != _imageMap.end(); it++ )
			{
			
				images.push_back( (*it).second->toString(level)
					+ " (ID #" + Ceylan::toString( (*it).first )
					+ ") and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( images,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No image is managed" ) ;
	
	}
	
	
	size = _textureMap.size() ;
	
	if ( size != 0 )
	{
	
		temp = "Number of textures managed: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> textures ;
			
			for ( map<Ceylan::ResourceID,
				Video::OpenGL::TextureCountedPtr>::const_iterator it =
					 _textureMap.begin(); it != _textureMap.end(); it++ )
			{
			
				textures.push_back( (*it).second->toString(level)
					+ " (ID #" + Ceylan::toString( (*it).first )
					+ ") and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( textures,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No texture is managed" ) ;
	
	}
	

	size = _truetypeFontMap.size() ;
	
	if ( size != 0 )
	{
	
		temp = "Number of TrueType font managed: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> trueTypeFonts ;
			
			for ( map<Ceylan::ResourceID,
				Text::TrueTypeFontCountedPtr>::const_iterator it =
					 _truetypeFontMap.begin(); it != _truetypeFontMap.end();
					 it++ )
			{
			
				trueTypeFonts.push_back( (*it).second->toString(level)
					+ " (ID #" + Ceylan::toString( (*it).first )
					+ ") and whose reference count is " 
					+ Ceylan::toString( (*it).second.getReferenceCount() ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( trueTypeFonts,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No texture is managed" ) ;
	
	}
	

	size = _reverseMap.size() ;
	
	if ( size != 0 )
	{
	
		temp = "Number of entries in reverse map: " + Ceylan::toString( size ) ;
		
		if ( level != Ceylan::low )
		{
		
			list<string> entries ;
			
			for ( map<std::string,Ceylan::ResourceID>::const_iterator it =
				_reverseMap.begin(); it != _reverseMap.end(); it++ )
			{
			
				entries.push_back( "Resource path '" + (*it).first 
					+ "' corresponds to ID #" 
					+ Ceylan::toString( (*it).second ) ) ;
			
			}	
			
			temp += Ceylan::formatStringList( entries,
				/* surroundByTicks */ false, /* indentationLevel */ 2 ) ;
			
		}
		
		
		maps.push_back( temp ) ;
		
	}
	else
	{

		maps.push_back( "No entry in reverse resource map" ) ;
	
	}
	
	maps.push_back( "Maximum resource ID currently allocated: " 
		+ Ceylan::toString( _maxID ) ) ;
		
	return "ResourceManager state: " + Ceylan::formatStringList( maps ) ;
					
}





// Static section.





// Protected members below:



void Data::ResourceManager::registerResource( 
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
	
	if ( id > _maxID )
		_maxID = id ;
		
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
	
	
	// First register that resource in reverse map:
	_reverseMap.insert( std::pair<std::string,Ceylan::ResourceID>(
		resourcePath, id ) ) ;
	
	
	ContentType resourceType = GetContentType( resourceStringifiedType,
		/* throwIfNotMatched */ false ) ;
	
	
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
	
		// Defaults:
		bool convertToDisplayFormat = false ;
		bool convertWithAlpha = true ;

		// Conversion to display to screen format could be managed here:
		
		Video::TwoDimensional::ImageCountedPtr newImagePtr = 
			new Video::TwoDimensional::Image( resourcePath, 
				/* preload */ false, 
				/* convertToDisplayFormat */ convertToDisplayFormat, 
				/* convertWithAlpha */ convertWithAlpha ) ;
			 
		_imageMap.insert( std::pair<Ceylan::ResourceID,
			Video::TwoDimensional::ImageCountedPtr>( id, newImagePtr ) ) ;

		 
	}
	else if ( resourceType == Data::texture_2D )
	{

		Video::OpenGL::TextureCountedPtr newTexturePtr = 
			new Video::OpenGL::GLTexture( resourcePath, 
				/* flavour */ Video::OpenGL::GLTexture::For2D, 
				/* preload */ false ) ;
			 
		_textureMap.insert( std::pair<Ceylan::ResourceID,
			Video::OpenGL::TextureCountedPtr>( id, newTexturePtr ) ) ;
 
	}
	else if ( resourceType == Data::texture_3D )
	{

		Video::OpenGL::TextureCountedPtr newTexturePtr = 
			new Video::OpenGL::GLTexture( resourcePath, 
				/* flavour */ Video::OpenGL::GLTexture::For3D, 
				/* preload */ false ) ;
			 
		_textureMap.insert( std::pair<Ceylan::ResourceID,
			Video::OpenGL::TextureCountedPtr>( id, newTexturePtr ) ) ;
 
	}
	else if ( resourceType == Data::ttf_font )
	{

		/*
		 * The user code will be able to change the point size later,
		 * preferably at loading time:
		 *
		 */
		Text::TrueTypeFontCountedPtr newTrueTypeFontPtr = 
			new Video::TwoDimensional::Text::TrueTypeFont( 
				/* font filename */ resourcePath, 
				/* index */ 0,
				/* convertToDisplay */ false,
				/* cacheSettings */ Text::Font::GlyphCached,
				/* preload */ false ) ;
			 
		_truetypeFontMap.insert( std::pair<Ceylan::ResourceID,
			Video::TwoDimensional::Text::TrueTypeFontCountedPtr>( id,
				newTrueTypeFontPtr ) ) ;
 
	}
	else
	{
	
		string message = "Resource in '" + resourcePath + "' has for type '" 
			+ resourceStringifiedType + "', which is not managed." ;
			
		send( message ) ;
	
		LogPlug::warning( message ) ;
		
		return ;
		
	}
	
	
	//send( "Resource inserted." ) ;
	
	// Reference count is 1 here.
		
}



Ceylan::ResourceID Data::ResourceManager::getIDForPath( 
	const string & resourcePath, bool emergencyStopInNotFound ) const
{

	map<string,Ceylan::ResourceID>::const_iterator it = 
		_reverseMap.find( resourcePath ) ;

	if ( it == _reverseMap.end() )
	{
		
		/*
		 * If just raising an exception, then this can lead to a segmentation
		 * fault: the exception propation may make a view constructor be
		 * aborted, which in turn may make its model constructor be aborted,
		 * which will try to deallocate this view, which is not a proper 
		 * view, causing the program to crash.
		 * Ex: in ~SingleControllerSingleViewGenericModel, with
		 * 'delete this->_view ;' and a view requesting an unknown ID at
		 * construction-time.
		 *
		 */
		
		string message = "getIDForPath: failed to look-up resource file '"
			+ resourcePath + "'." ;
			
		LogPlug::error( message ) ;
		
		if ( emergencyStopInNotFound )
			Ceylan::emergencyShutdown( message ) ;
		else	
			throw ResourceManagerException( "ResourceManager::getIDForPath: "
				"resource path '" + resourcePath 
				+ "' could not be found in archive." ) ;
	
	}
	
	return (*it).second ;	

}



ContentType Data::ResourceManager::GetContentType( 
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
	else if	( stringifiedType == "texture_2D" )
		return Data::texture_2D ;
	else if	( stringifiedType == "texture_3D" )
		return Data::texture_3D ;
	else if	( stringifiedType == "ttf_font" )
		return Data::ttf_font ;
	else
	{
	
		// Not found:
		if ( throwIfNotMatched )
			throw ResourceManagerException( "ResourceManager::GetContentType: "
				"the string '" + stringifiedType 
				+ "' could not be interpreted as a content type." ) ;
		else
			return Data::unknown ;
					
	}
		
}

	
