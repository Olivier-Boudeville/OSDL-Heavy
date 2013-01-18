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
using std::pair ;
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
 * The resource manager owns a reference on each resource, therefore when their
 * refcount is 1 then no third-party references them, and the manager may or may
 * not (if wanting to cache resources) unload them, if they are loaded.
 *
 * The reference counts have been successfully checked (see
 * testOSDLResourceManager.cc)
 *
 * Note also that counted pointers have always to be passed by value, not by
 * address, otherwise the reference count will be wrong.
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

  send( "Deleting ResourceManager, whose final state was: " + toString() ) ;

}



Audio::MusicCountedPtr Data::ResourceManager::getMusic( Ceylan::ResourceID id,
  bool oneShot )
{

  send( "Getting music whose ID is " + Ceylan::toString( id ) ) ;

  map<Ceylan::ResourceID, pair<Audio::MusicCountedPtr,bool> >::iterator it =
	_musicMap.find( id ) ;

  if ( it == _musicMap.end() )
	throw ResourceManagerException( "ResourceManager::getMusic: ID #"
	  + Ceylan::toString( id ) + " could not be found." ) ;

  pair<Audio::MusicCountedPtr,bool> targetResPair = (*it).second ;

  Audio::MusicCountedPtr res = targetResPair.first ;
  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Audio::MusicCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _musicMap.erase( it ) ;
	  _musicMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}



Audio::MusicCountedPtr Data::ResourceManager::getMusic(
  const string & musicPath, bool oneShot  )
{

  send( "Getting music whose path is '" + musicPath + "'." ) ;

  map<Ceylan::ResourceID, pair<Audio::MusicCountedPtr,bool> >::iterator it =
	_musicMap.find( getIDForPath(musicPath) ) ;

  pair<Audio::MusicCountedPtr,bool> targetResPair = (*it).second ;
  Audio::MusicCountedPtr res = targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Audio::MusicCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _musicMap.erase( it ) ;

	  Ceylan::ResourceID id = (*it).first ;
	  _musicMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}



Audio::SoundCountedPtr Data::ResourceManager::getSound( Ceylan::ResourceID id,
  bool oneShot  )
{

  send( "Getting sound whose ID is " + Ceylan::toString( id ) ) ;

  map<Ceylan::ResourceID, pair<Audio::SoundCountedPtr,bool> >::iterator it =
	_soundMap.find( id ) ;

  if ( it == _soundMap.end() )
	throw ResourceManagerException( "ResourceManager::getSound: ID #"
	  + Ceylan::toString( id ) + " could not be found." ) ;

  pair<Audio::SoundCountedPtr,bool> targetResPair = (*it).second ;
  Audio::SoundCountedPtr res = targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Audio::SoundCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _soundMap.erase( it ) ;
	  _soundMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}




Audio::SoundCountedPtr Data::ResourceManager::getSound(
  const string & soundPath, bool oneShot )
{

  send( "Getting sound whose path is '" + soundPath + "'" ) ;

  map<Ceylan::ResourceID, pair<Audio::SoundCountedPtr,bool> >::iterator it =
	_soundMap.find( getIDForPath(soundPath) ) ;

  if ( it == _soundMap.end() )
	throw ResourceManagerException(
	  "ResourceManager::getSound: sound path '" + soundPath
	  + "' could not be found." ) ;

  pair<Audio::SoundCountedPtr,bool> targetResPair = (*it).second ;
  Audio::SoundCountedPtr res = targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Audio::SoundCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _soundMap.erase( it ) ;

	  Ceylan::ResourceID id = (*it).first ;
	  _soundMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}



Video::TwoDimensional::ImageCountedPtr Data::ResourceManager::getImage(
  Ceylan::ResourceID id, bool oneShot )
{

  send( "Getting image whose ID is " + Ceylan::toString( id ) ) ;

  map<Ceylan::ResourceID,
	  pair< Video::TwoDimensional::ImageCountedPtr, bool > >::iterator it =
	_imageMap.find( id ) ;

  if ( it == _imageMap.end() )
	throw ResourceManagerException( "ResourceManager::getImage: ID #"
	  + Ceylan::toString( id ) + " could not be found." ) ;

  pair<Video::TwoDimensional::ImageCountedPtr,bool> targetResPair
	= (*it).second ;

  Video::TwoDimensional::ImageCountedPtr res = targetResPair.first ;
  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Video::TwoDimensional::ImageCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _imageMap.erase( it ) ;
	  _imageMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}



Video::TwoDimensional::ImageCountedPtr Data::ResourceManager::getImage(
  const string & imagePath, bool oneShot )
{

  send( "Getting image whose path is '" + imagePath + "'" ) ;

  map<Ceylan::ResourceID,
	  pair< Video::TwoDimensional::ImageCountedPtr, bool > >::iterator it =
	_imageMap.find( getIDForPath(imagePath) ) ;

  if ( it == _imageMap.end() )
	throw ResourceManagerException( "ResourceManager::getImage: image '"
	  + imagePath + "' could not be found." ) ;

  pair<Video::TwoDimensional::ImageCountedPtr,bool> targetResPair
	= (*it).second ;

  Video::TwoDimensional::ImageCountedPtr res = targetResPair.first ;
  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Video::TwoDimensional::ImageCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _imageMap.erase( it ) ;
	  Ceylan::ResourceID id = (*it).first ;
	  _imageMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load() ;

  return res ;

}



Video::OpenGL::TextureCountedPtr Data::ResourceManager::getTexture(
  Ceylan::ResourceID id, bool uploadWanted, bool oneShot )
{

  send( "Getting texture whose ID is " + Ceylan::toString( id ) ) ;

  map<Ceylan::ResourceID,
	  pair< Video::OpenGL::TextureCountedPtr, bool > >::iterator it =
	_textureMap.find( id ) ;

  if ( it == _textureMap.end() )
  {

	LogPlug::fatal( "Data::ResourceManager::getTexture for ID failed." ) ;

	throw ResourceManagerException( "ResourceManager::getTexture: ID #"
	  + Ceylan::toString( id ) + " could not be found." ) ;

  }


  Video::OpenGL::TextureCountedPtr res = ((*it).second).first ;

  // Ensures a returned resource is always loaded:
  res->load() ;

  // Maybe wanting to have it uploaded directly to the video card?
  if ( uploadWanted && ( ! res->wasUploaded() ) )
	res->upload() ;

  return res ;

}



Video::OpenGL::TextureCountedPtr Data::ResourceManager::getTexture(
  const string & texturePath, bool uploadWanted, bool oneShot )
{

  send( "Getting texture whose path is '" + texturePath + "'" ) ;

  map<Ceylan::ResourceID,
	  pair< Video::OpenGL::TextureCountedPtr, bool > >::iterator it =
	_textureMap.find( getIDForPath(texturePath) ) ;

  if ( it == _textureMap.end() )
  {

	LogPlug::fatal( "Data::ResourceManager::getTexture for file '"
	  + texturePath + "' failed." ) ;

	throw ResourceManagerException( "ResourceManager::getTexture: texture '"
	  + texturePath + "' could not be found." ) ;

  }

  pair<Video::OpenGL::TextureCountedPtr,bool> targetResPair = (*it).second ;
  Video::OpenGL::TextureCountedPtr res = targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Video::OpenGL::TextureCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _textureMap.erase( it ) ;
	  Ceylan::ResourceID id = (*it).first ;
	  _textureMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

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
  bool uploadWanted, bool oneShot )
{

  send( "Getting texture from surface "
	+ sourceSurface.toString( Ceylan::low ) ) ;

  Video::OpenGL::TextureCountedPtr resPtr = new Video::OpenGL::GLTexture(
	sourceSurface, flavour ) ;

  // Maybe wanting to have it uploaded directly to the video card?
  if ( uploadWanted && ( ! resPtr->wasUploaded() ) )
	resPtr->upload() ;

  _maxID++ ;

  Ceylan::ResourceID resID = _maxID ;

  // Could be purgeable as well:
  _textureMap.insert( pair< Ceylan::ResourceID,
	pair<Video::OpenGL::TextureCountedPtr,bool> >(
	  resID, std::make_pair( resPtr, /* purgeable */ false ) ) ) ;

  return std::pair<Video::OpenGL::TextureCountedPtr,Ceylan::ResourceID>(
	resPtr, resID ) ;

}



Video::TwoDimensional::Text::TrueTypeFontCountedPtr
Data::ResourceManager::getTrueTypeFont( Ceylan::ResourceID id,
  Text::PointSize pointSize, bool oneShot )
{

  send( "Getting TrueTypeFont whose ID is " + Ceylan::toString( id ) ) ;

  map<ResourceID, pair< Text::TrueTypeFontCountedPtr, bool > >::iterator it =
	_truetypeFontMap.find( id ) ;

  if ( it == _truetypeFontMap.end() )
	throw ResourceManagerException( "ResourceManager::getTrueTypeFont: ID #"
	  + Ceylan::toString( id ) + " could not be found." ) ;

  pair<Text::TrueTypeFontCountedPtr,bool> targetResPair = (*it).second ;
  Video::TwoDimensional::Text::TrueTypeFontCountedPtr res =
	targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Text::TrueTypeFontCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _truetypeFontMap.erase( it ) ;
	  _truetypeFontMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load( pointSize ) ;

  return res ;

}



Video::TwoDimensional::Text::TrueTypeFontCountedPtr
Data::ResourceManager::getTrueTypeFont( const string & fontPath,
  Text::PointSize pointSize, bool oneShot )
{

  send( "Getting TrueTypeFont whose path is '" + fontPath + "'" ) ;

  map<ResourceID, pair< Text::TrueTypeFontCountedPtr, bool > >::iterator it =
	_truetypeFontMap.find( getIDForPath(fontPath) ) ;

  if ( it == _truetypeFontMap.end() )
	throw ResourceManagerException( "ResourceManager::getTrueTypeFont: "
	  "font '" + fontPath + "' could not be found." ) ;

  pair<Text::TrueTypeFontCountedPtr,bool> targetResPair = (*it).second ;
  Video::TwoDimensional::Text::TrueTypeFontCountedPtr res =
	targetResPair.first ;

  bool purgeable = targetResPair.second ;

  /*
   * Whether or not it was purgeable, a 'one-shot' get will not change this
   * status. So:
   *
   */
  if ( ! oneShot )
  {

	/*
	 * Not one-shot get here, so unmark it as purgeable if it was:
	 *
	 * (i.e. the only update case is to go from purgeable to non-purgeable
	 * because the get was not a one-shot one)
	 *
	 */
	if ( purgeable )
	{

	  pair<Text::TrueTypeFontCountedPtr,bool> newResPair =
		std::make_pair( res, /* purgeable */ false ) ;

	  // Swaps previous and new entries (not at the same place, though):
	  _truetypeFontMap.erase( it ) ;

	  Ceylan::ResourceID id = (*it).first ;
	  _truetypeFontMap.insert( std::make_pair( id, newResPair ) ) ;

	}
	// else: neither one-shot nor marked as purgeable: leave it as is.

  }

  // Ensures a returned resource is always loaded:
  res->load( pointSize ) ;

  return res ;

}



void Data::ResourceManager::discardTexture( Ceylan::ResourceID textureId )
{

  send( "Discarding texture whose ID is " + Ceylan::toString( textureId ) ) ;

  map< Ceylan::ResourceID, pair<Video::OpenGL::TextureCountedPtr,bool> >::
	iterator it = _textureMap.find( textureId ) ;

  if ( it == _textureMap.end() )
	throw ResourceManagerException( "ResourceManager::discardTexture: "
	  "no texture with ID #" + Ceylan::toString( textureId )
	  + " found." ) ;

  /*
   * Ensures the texture is removed from video card and deleted (regardless of
   * purgeable status):
   *
   */
  _textureMap.erase( it ) ;

}



void Data::ResourceManager::tidy()
{

  /*
   * Unloads content from all entries which are referenced only by this manager
   * and are marked as purgeable:
   */

  for ( map< Ceylan::ResourceID, pair<Audio::MusicCountedPtr,bool> >::iterator
		  it = _musicMap.begin(); it != _musicMap.end(); it++ )
  {

	Audio::MusicCountedPtr resPtr = ((*it).second).first ;
	bool purgeable = ((*it).second).second ;

	if ( ( resPtr.getReferenceCount() == 1 ) && purgeable )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID, pair<Audio::SoundCountedPtr,bool> >::iterator
		  it = _soundMap.begin(); it != _soundMap.end(); it++ )
  {

	Audio::SoundCountedPtr resPtr = ((*it).second).first ;
	bool purgeable = ((*it).second).second ;

	if ( ( resPtr.getReferenceCount() == 1 ) && purgeable )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
		  pair<Video::TwoDimensional::ImageCountedPtr,bool> >::iterator it =
		  _imageMap.begin(); it != _imageMap.end(); it++ )
  {

	Video::TwoDimensional::ImageCountedPtr resPtr = ((*it).second).first ;
	bool purgeable = ((*it).second).second ;

	if ( ( resPtr.getReferenceCount() == 1 ) && purgeable )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
		  pair<Video::OpenGL::TextureCountedPtr,bool> >::iterator it =
		  _textureMap.begin(); it != _textureMap.end(); it++ )
  {

	Video::OpenGL::TextureCountedPtr resPtr = ((*it).second).first ;
	bool purgeable = ((*it).second).second ;

	if ( ( resPtr.getReferenceCount() == 1 ) && purgeable )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
	  pair<Video::TwoDimensional::Text::TrueTypeFontCountedPtr,bool> >::iterator
	  it = _truetypeFontMap.begin(); it != _truetypeFontMap.end(); it++ )
  {

	Video::TwoDimensional::Text::TrueTypeFontCountedPtr resPtr
	  = ((*it).second).first ;
	bool purgeable = ((*it).second).second ;

	if ( ( resPtr.getReferenceCount() == 1 ) && purgeable )
	  resPtr->unload() ;

  }

  /*
	Ceylan::checkpoint( "ResourceManager is now tidied, new state: " ) ;
	displayLoadedResources() ;
  */

}



void Data::ResourceManager::purge()
{

  // Unloads everything which is not referenced by anyone else:

  for ( map< Ceylan::ResourceID, pair<Audio::MusicCountedPtr,bool> >::iterator
		  it = _musicMap.begin(); it != _musicMap.end(); it++ )
  {

	Audio::MusicCountedPtr resPtr = ((*it).second).first ;

	if ( resPtr.getReferenceCount() == 1 )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID, pair<Audio::SoundCountedPtr,bool> >::iterator
		  it = _soundMap.begin(); it != _soundMap.end(); it++ )
  {

	Audio::SoundCountedPtr resPtr = ((*it).second).first ;

	if ( resPtr.getReferenceCount() == 1 )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
		  pair<Video::TwoDimensional::ImageCountedPtr,bool> >::iterator it =
		  _imageMap.begin(); it != _imageMap.end(); it++ )
  {

	Video::TwoDimensional::ImageCountedPtr resPtr = ((*it).second).first ;

	if ( resPtr.getReferenceCount() == 1 )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
		  pair<Video::OpenGL::TextureCountedPtr,bool> >::iterator it =
		  _textureMap.begin(); it != _textureMap.end(); it++ )
  {

	Video::OpenGL::TextureCountedPtr resPtr = ((*it).second).first ;

	if ( resPtr.getReferenceCount() == 1 )
	  resPtr->unload() ;

  }


  for ( map< Ceylan::ResourceID,
		  pair<Video::TwoDimensional::Text::TrueTypeFontCountedPtr,bool> >::iterator
		  it = _truetypeFontMap.begin(); it != _truetypeFontMap.end(); it++ )
  {

	Video::TwoDimensional::Text::TrueTypeFontCountedPtr resPtr
	  = ((*it).second).first ;

	if ( resPtr.getReferenceCount() == 1 )
	  resPtr->unload() ;

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
		  pair< Audio::MusicCountedPtr, bool> >::const_iterator it =
		  _musicMap.begin(); it != _musicMap.end(); it++ )
  {

	Audio::MusicCountedPtr ptr = ((*it).second).first ;
	if ( ptr->hasContent() )
	{

	  bool purgeable = ((*it).second).second ;

	  string purgeText ;

	  if ( purgeable )
		purgeText = "marked as purgeable" ;
	  else
		purgeText = "not marked as purgeable" ;

	  loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
		+ ": " + ptr->toString(level)
		+ ", a resource which is " + purgeText
		+ " and whose reference count is "
		+ Ceylan::toString( ptr.getReferenceCount() ) ) ;

	}

  }


  for ( map<Ceylan::ResourceID,
		  pair< Audio::SoundCountedPtr, bool> >::const_iterator it =
		  _soundMap.begin(); it != _soundMap.end(); it++ )
  {

	Audio::SoundCountedPtr ptr = ((*it).second).first ;
	if ( ptr->hasContent() )
	{

	  bool purgeable = ((*it).second).second ;

	  string purgeText ;

	  if ( purgeable )
		purgeText = "marked as purgeable" ;
	  else
		purgeText = "not marked as purgeable" ;


	  loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
		+ ": " + ptr->toString(level)
		+ ", a resource which is " + purgeText
		+ " and whose reference count is "
		+ Ceylan::toString( ptr.getReferenceCount() ) ) ;

	}

  }


  for ( map<Ceylan::ResourceID,
		  pair< Video::TwoDimensional::ImageCountedPtr, bool> >::const_iterator
		  it = _imageMap.begin(); it != _imageMap.end(); it++ )
  {

	Video::TwoDimensional::ImageCountedPtr ptr = ((*it).second).first ;
	if ( ptr->hasContent() )
	{

	  bool purgeable = ((*it).second).second ;

	  string purgeText ;

	  if ( purgeable )
		purgeText = "marked as purgeable" ;
	  else
		purgeText = "not marked as purgeable" ;


	  loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
		+ ": " + ptr->toString(level)
		+ ", a resource which is " + purgeText
		+ " and whose reference count is "
		+ Ceylan::toString( ptr.getReferenceCount() ) ) ;

	}

  }


  for ( map<Ceylan::ResourceID,
		  pair< Video::OpenGL::TextureCountedPtr, bool> >::const_iterator it =
		  _textureMap.begin(); it != _textureMap.end(); it++ )
  {

	Video::OpenGL::TextureCountedPtr ptr = ((*it).second).first ;
	if ( ptr->hasContent() )
	{

	  bool purgeable = ((*it).second).second ;

	  string purgeText ;

	  if ( purgeable )
		purgeText = "marked as purgeable" ;
	  else
		purgeText = "not marked as purgeable" ;


	  loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
		+ ": " + ptr->toString(level)
		+ ", a resource which is " + purgeText
		+ " and whose reference count is "
		+ Ceylan::toString( ptr.getReferenceCount() ) ) ;

	}

  }


  for ( map<Ceylan::ResourceID,
		  pair< Text::TrueTypeFontCountedPtr, bool> >::const_iterator it =
		  _truetypeFontMap.begin(); it != _truetypeFontMap.end(); it++ )
  {

	Text::TrueTypeFontCountedPtr ptr = ((*it).second).first ;
	if ( ptr->hasContent() )
	{

	  bool purgeable = ((*it).second).second ;

	  string purgeText ;

	  if ( purgeable )
		purgeText = "marked as purgeable" ;
	  else
		purgeText = "not marked as purgeable" ;

	  loadedList.push_back( "ID #" + Ceylan::toString( (*it).first )
		+ ": " + ptr->toString(level)
		+ ", a resource which is " + purgeText
		+ " and whose reference count is "
		+ Ceylan::toString( ptr.getReferenceCount() ) ) ;

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
			  pair< Audio::MusicCountedPtr, bool > >::const_iterator it
			  = _musicMap.begin(); it != _musicMap.end(); it++ )
	  {

		Audio::MusicCountedPtr ptr = ((*it).second).first ;

		bool purgeable = ((*it).second).second ;

		string purgeText ;

		if ( purgeable )
		  purgeText = "marked as purgeable" ;
		else
		  purgeText = "not marked as purgeable" ;

		musics.push_back( ptr->toString(level)
		  + " (ID #" + Ceylan::toString( (*it).first )
		  + ", which is " + purgeText
		  + " and whose reference count is "
		  + Ceylan::toString( ptr.getReferenceCount() )
		  + ")" ) ;

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
			  pair<  Audio::SoundCountedPtr, bool> >::const_iterator it =
			  _soundMap.begin(); it != _soundMap.end(); it++ )
	  {

		Audio::SoundCountedPtr ptr = ((*it).second).first ;

		bool purgeable = ((*it).second).second ;

		string purgeText ;

		if ( purgeable )
		  purgeText = "marked as purgeable" ;
		else
		  purgeText = "not marked as purgeable" ;

		sounds.push_back( ptr->toString(level)
		  + " (ID #" + Ceylan::toString( (*it).first )
		  + ", which is " + purgeText
		  + " and whose reference count is "
		  + Ceylan::toString( ptr.getReferenceCount() )
		  + ")" ) ;

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
		pair< Video::TwoDimensional::ImageCountedPtr, bool> >::const_iterator it
			  = _imageMap.begin(); it != _imageMap.end(); it++ )
	  {

		Video::TwoDimensional::ImageCountedPtr ptr = ((*it).second).first ;

		bool purgeable = ((*it).second).second ;

		string purgeText ;

		if ( purgeable )
		  purgeText = "marked as purgeable" ;
		else
		  purgeText = "not marked as purgeable" ;

		images.push_back( ptr->toString(level)
		  + " (ID #" + Ceylan::toString( (*it).first )
		  + ", which is " + purgeText
		  + " and whose reference count is "
		  + Ceylan::toString( ptr.getReferenceCount() )
		  + ")" ) ;

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
			  pair< Video::OpenGL::TextureCountedPtr, bool> >::const_iterator
			  it = _textureMap.begin(); it != _textureMap.end(); it++ )
	  {

		Video::OpenGL::TextureCountedPtr ptr = ((*it).second).first ;

		bool purgeable = ((*it).second).second ;

		string purgeText ;

		if ( purgeable )
		  purgeText = "marked as purgeable" ;
		else
		  purgeText = "not marked as purgeable" ;

		textures.push_back( ptr->toString(level)
		  + " (ID #" + Ceylan::toString( (*it).first )
		  + ", which is " + purgeText
		  + " and whose reference count is "
		  + Ceylan::toString( ptr.getReferenceCount() )
		  + ")" ) ;

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
			  pair< Text::TrueTypeFontCountedPtr, bool> >::const_iterator it =
			  _truetypeFontMap.begin(); it != _truetypeFontMap.end();
			it++ )
	  {

		Text::TrueTypeFontCountedPtr ptr = ((*it).second).first ;

		bool purgeable = ((*it).second).second ;

		string purgeText ;

		if ( purgeable )
		  purgeText = "marked as purgeable" ;
		else
		  purgeText = "not marked as purgeable" ;

		trueTypeFonts.push_back( ptr->toString(level)
		  + " (ID #" + Ceylan::toString( (*it).first )
		  + ", which is " + purgeText
		  + " and whose reference count is "
		  + Ceylan::toString( ptr.getReferenceCount() )
		  + ")" ) ;

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

  if ( resourceType == Data::unknown )
	throw ResourceManagerException(
	  "ResourceManager::registerResource: the resource whose path is '"
	  + resourcePath + "' is not of a known content type. "
	  "Forgot to post-process it, or to remove this file?" ) ;

  // By default, unloaded resources are not purgeable:
  bool purgeable = false ;

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

	_musicMap.insert( std::pair<Ceylan::ResourceID,
				std::pair< Audio::MusicCountedPtr, bool > >(
				  id, std::make_pair( newMusicPtr, purgeable ) ) ) ;

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

	_soundMap.insert( std::pair<Ceylan::ResourceID,
				std::pair< Audio::SoundCountedPtr, bool > >(
				  id, std::make_pair( newSoundPtr, purgeable ) ) ) ;

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
				std::pair< Video::TwoDimensional::ImageCountedPtr, bool > >(
				  id, std::make_pair( newImagePtr, purgeable ) ) ) ;

  }
  else if ( resourceType == Data::texture_2D )
  {

	Video::OpenGL::TextureCountedPtr newTexturePtr =
	  new Video::OpenGL::GLTexture( resourcePath,
		/* flavour */ Video::OpenGL::GLTexture::For2D,
		/* preload */ false ) ;

	_textureMap.insert( std::pair<Ceylan::ResourceID,
				std::pair< Video::OpenGL::TextureCountedPtr, bool > >(
				  id, std::make_pair( newTexturePtr, purgeable ) ) ) ;

  }
  else if ( resourceType == Data::texture_3D )
  {

	Video::OpenGL::TextureCountedPtr newTexturePtr =
	  new Video::OpenGL::GLTexture( resourcePath,
		/* flavour */ Video::OpenGL::GLTexture::For3D,
		/* preload */ false ) ;

	_textureMap.insert( std::pair<Ceylan::ResourceID,
				std::pair< Video::OpenGL::TextureCountedPtr, bool > >(
				  id, std::make_pair( newTexturePtr, purgeable ) ) ) ;

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
				std::pair< Video::TwoDimensional::Text::TrueTypeFontCountedPtr,
						   bool > >(
				  id, std::make_pair( newTrueTypeFontPtr, purgeable ) ) ) ;

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
	 * If just raising an exception, then this can lead to a segmentation fault:
	 * the exception propation may make a view constructor be aborted, which in
	 * turn may make its model constructor be aborted, which will try to
	 * deallocate this view, which is not a proper view, causing the program to
	 * crash.
	 *
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
  else if ( stringifiedType == "data" )
	return Data::data ;
  else if ( stringifiedType == "sound" )
	return Data::sound ;
  else if ( stringifiedType == "music" )
	return Data::music ;
  else if ( stringifiedType == "image" )
	return Data::image ;
  else if ( stringifiedType == "texture_2D" )
	return Data::texture_2D ;
  else if ( stringifiedType == "texture_3D" )
	return Data::texture_3D ;
  else if ( stringifiedType == "ttf_font" )
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
