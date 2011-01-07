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


#ifndef OSDL_RESOURCE_MANAGER_H_
#define OSDL_RESOURCE_MANAGER_H_


#include "OSDLDataCommon.h"     // for DataException, ResourceID


#include "OSDLMusic.h"          // for MusicCountedPtr
#include "OSDLSound.h"          // for SoundCountedPtr
#include "OSDLImage.h"          // for ImageCountedPtr
#include "OSDLGLTexture.h"      // for TextureCountedPtr
#include "OSDLTrueTypeFont.h"   // for TrueTypeFontCountedPtr


#include "Ceylan.h"             // for ResourceID


#include <string>
#include <list>
#include <map>
#include <utility>              // for std::pair



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
	 * This type does not reflect the encoding (ex: no '.wav' or '.ogg'), as a
	 * given encoding can be used in different contexts (ex: as a sound or as a
	 * music), and as the actual encoding is generally automatically determined
	 * by the back-end (ex: SDL_mixer) based on the file content rather than
	 * based on its extension.
	 *
	 * Therefore we have usage-based types (ex: '.music', '.sound') rather than
	 * encoding-based extensions.
	 *
	 */
	enum ContentType
	{


	  // For pure texts:
	  text,


	  // For raw data, not corresponding to any other content type:
	  data,


	  /*
	   * For musics (longer sounds, at most one of them playing at any time):
	   *
	   */
	  music,


	  // For sounds (usually special-effects):
	  sound,


	  // For images; are automatically converted to the display format:
	  image,



	  /*
	   * For OpenGL textures, which are not converted to the display
	   * format.
	   *
	   * Only the supported flavours are listed below.
	   *
	   */

	  // For textures to be used in 2D:
	  texture_2D,


	  // For textures to be used in 3D:
	  texture_3D,


	  // For True-type fonts:
	  ttf_font,


	  // For data whose content type is not known:
	  unknown

	} ;




	/**
	 * This ResourceManager manages resources on behalf of the developer.
	 *
	 * The manager is able to keep track of resources, to load them and to make
	 * them available to any number of clients.
	 *
	 * To do so, the manager reads a resource map, usually from an XML file in
	 * an OSDL archive, and based on it maintains a set of dictionaries, each of
	 * them corresponding to a kind of resource, like images, sounds, etc.
	 *
	 * A key of such a dictionary is a resource identifier, which is expected to
	 * correspond to a resource this dictionary manages.
	 *
	 * Each associated value is a Ceylan::LoadableWithContent instance, which
	 * records the path of the resource file and, if loaded, the resource
	 * itself.
	 *
	 * @note When the resource archive is modified (ex: a resource is added),
	 * the code loading from the archive must be recompiled, as resource
	 * identifiers are likely to change.
	 *
	 * @see create-OSDL-archive.sh to understand how resources are indexed in an
	 * archive.
	 *
	 */
	class OSDL_DLL ResourceManager : public Ceylan::Object
	{


	public:



	  /**
	   * Creates a resource manager, which will manage resources listed in the
	   * resource map, whose filename is specified.
	   *
	   * @param resourceMapFilename the filename of the XML resource map to use;
	   * typically "resource-map.xml" if using the create-OSDL-archive.sh script
	   * with default settings.
	   *
	   */
	  explicit ResourceManager(
		const std::string & resourceMapFilename =
		"OSDLResourceMap.xml" ) ;



	  /// Virtual destructor.
	  virtual ~ResourceManager() throw() ;




	  /**
	   * Returns a counter pointer referencing the specified music.
	   *
	   * @param id the resource identifier for that music.
	   *
	   * @throw ResourceManagerException if the music could not be found.
	   *
	   */
	  virtual Audio::MusicCountedPtr getMusic(
		Ceylan::ResourceID id ) ;



	  /**
	   * Returns a counter pointer referencing the specified music.
	   *
	   * @param musicPath the music path that will be looked-up in the reverse
	   * resource map, resolved to a resource identifier which will be then used
	   * to return a corresponding counted music pointer.
	   *
	   * @throw ResourceManagerException if the music could not be found.
	   *
	   */
	  virtual Audio::MusicCountedPtr getMusic(
		const std::string & musicPath ) ;




	  /**
	   * Returns a counter pointer referencing the specified sound.
	   *
	   * @param id the resource identifier for that sound.
	   *
	   * @throw ResourceManagerException if the sound could not be
	   * found.
	   *
	   */
	  virtual Audio::SoundCountedPtr getSound(
		Ceylan::ResourceID id ) ;



	  /**
	   * Returns a counter pointer referencing the specified sound.
	   *
	   * @param soundPath the sound path that will be looked-up in the reverse
	   * resource map, resolved to a resource identifier which will be then used
	   * to return a corresponding counted sound pointer.
	   *
	   * @throw ResourceManagerException if the sound could not be found.
	   *
	   */
	  virtual Audio::SoundCountedPtr getSound(
		const std::string & soundPath ) ;



	  /**
	   * Returns a counter pointer referencing the specified image.
	   *
	   * @param id the resource identifier for that image.
	   *
	   * @throw ResourceManagerException if the image could not be found.
	   *
	   */
	  virtual Video::TwoDimensional::ImageCountedPtr
		getImage( Ceylan::ResourceID id ) ;



	  /**
	   * Returns a counter pointer referencing the specified image.
	   *
	   * @param imagePath the image path that will be looked-up in the reverse
	   * resource map, resolved to a resource identifier which will be then used
	   * to return a corresponding counted image pointer.
	   *
	   * @throw ResourceManagerException if the image could not be found.
	   *
	   */
	  virtual Video::TwoDimensional::ImageCountedPtr getImage(
		const std::string & imagePath ) ;



	  /**
	   * Returns a counter pointer referencing the specified texture.
	   *
	   * @param id the resource identifier for that texture.
	   *
	   * @param uploadWanted if true, the method will ensure that this texture
	   * is available in the video card before returning.
	   *
	   * @throw ResourceManagerException if the texture could not be found.
	   *
	   * @note The returned texture has not been uploaded yet.
	   *
	   */
	  virtual Video::OpenGL::TextureCountedPtr getTexture(
		Ceylan::ResourceID id, bool uploadWanted = true ) ;



	  /**
	   * Returns a counter pointer referencing the specified texture.
	   *
	   * @param texturePath the texture path that will be looked-up in the
	   * reverse resource map, resolved to a resource identifier which will be
	   * then used to return a corresponding counted texture pointer.
	   *
	   * @param uploadWanted if true, the method will ensure that this texture
	   * is available in the video card before returning.
	   *
	   * @throw ResourceManagerException if the texture could not be found.
	   *
	   * @note The returned texture has not been uploaded yet.
	   *
	   */
	  virtual Video::OpenGL::TextureCountedPtr getTexture(
		const std::string & texturePath,
		bool uploadWanted = true ) ;



	  /**
	   * Returns a pair made of a counted pointer to a texture created from
	   * specified surface, and of the ID of the entry corresponding to that
	   * texture in this manager.
	   *
	   * The texture can be used directly, whereas the ID is to be used later to
	   * discard it, once become useless.
	   *
	   * @see discardTexture
	   *
	   * @param sourceSurface the surface, whose ownership is taken, from which
	   * the texture will be made and referenced.
	   *
	   * @param flavour the texture flavour that should be used.
	   *
	   * @param uploadWanted if true, the method will ensure that this texture
	   * is available in the video card before returning.
	   *
	   * @throw ResourceManagerException if the operation failed.
	   *
	   */
	  virtual std::pair<Video::OpenGL::TextureCountedPtr,
		Ceylan::ResourceID> getTextureFrom(
		  Video::Surface & sourceSurface,
		  Video::OpenGL::GLTexture::TextureFlavour flavour,
		  bool uploadWanted = true ) ;




	  /**
	   * Returns a counter pointer referencing the specified TrueType font.
	   *
	   * @param id the resource identifier for that font.
	   *
	   * @param pointSize the targeted point size; it must be specified as
	   * otherwise, since a returned resource must always be loaded (due to
	   * conventions), it would be loaded with the default point size which is
	   * seldom the one wanted, thus it would have to be unloaded and then
	   * reloaded with the correct size, when wanting to use it for real.
	   *
	   * @throw ResourceManagerException if the font could not be found.
	   *
	   */
	  virtual Video::TwoDimensional::Text::TrueTypeFontCountedPtr
		getTrueTypeFont( Ceylan::ResourceID id,
		  Video::TwoDimensional::Text::PointSize pointSize ) ;



	  /**
	   * Returns a counter pointer referencing the specified TrueType font.
	   *
	   * @param fontPath the font path that will be looked-up in the reverse
	   * resource map, resolved to a resource identifier which will be then used
	   * to return a corresponding counted font pointer.
	   *
	   * @param pointSize the targeted point size; it must be specified as
	   * otherwise, since a returned resource must always be loaded (due to
	   * conventions), it would be loaded with the default point size which is
	   * seldom the one wanted, thus it would have to be unloaded and then
	   * reloaded with the correct size, when wanting to use it for real.
	   *
	   * @throw ResourceManagerException if the font could not be found.
	   *
	   */
	  virtual Video::TwoDimensional::Text::TrueTypeFontCountedPtr
		getTrueTypeFont( const std::string & fontPath,
		  Video::TwoDimensional::Text::PointSize pointSize ) ;




	  /**
	   * Discards permanently the specified texture entry from this manager.
	   *
	   * @note Useful for resources like generated textures that cannot be
	   * unloaded (as they could not be then reloaded from file) and that we
	   * want nevertheless to be able to get rid of, when finished with them, so
	   * that their memory can be released.
	   *
	   * @throw ResourceManagerException if not such resource exists.
	   *
	   */
	  virtual void discardTexture( Ceylan::ResourceID textureId ) ;



	  /**
	   * Requests all resources only referenced by this manager to unload their
	   * content.
	   *
	   */
	  virtual void purge() ;



	  /**
	   * Outputs the list of all currently loaded resources.
	   *
	   * @note Only for debugging purpose.
	   *
	   */
	  virtual void displayLoadedResources() ;



	  /**
	   * Returns an user-friendly description of the state of this object.
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
	   * Returns the content type corresponding to the specified stringified
	   * version.
	   *
	   * @param stringifiedType the string to interpret.
	   *
	   * @param throwIfNotMatched if true, if the context type could not be
	   * determined, throws a ResourceManagerException rather than returning the
	   * 'unknown' content type.
	   *
	   */
	  static ContentType GetContentType(
		const std::string & stringifiedType,
		bool throwIfNotMatched = true ) ;




	protected:



	  /**
	   * Registers a new resource, as specified by the XML resource entry.
	   *
	   */
	  void registerResource(
		const Ceylan::XML::XMLParser::XMLTree & resourceXMLEntry ) ;



	  /**
	   * Returns the resource identifier corresponding to specified path, based
	   * on the reverse resource map.
	   *
	   * @param resourcePath the path of the specified resource in archive.
	   *
	   * @param emergencyStopInNotFound performs an emergency stop in the
	   * look-up of the specified ID failed, as usually the raising of an
	   * exception here will make a view constructor fail, leading to a
	   * segmentation fault.
	   *
	   * @throw ResourceManagerException if the path could not be resolved.
	   *
	   */
	  Ceylan::ResourceID getIDForPath(
		const std::string & resourcePath,
		bool emergencyStopInNotFound = true ) const ;



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
	   * Each key of the map is a resource identifier, which is expected to
	   * correspond to a music resource.
	   *
	   * Each associated value is a counted pointer on a
	   * Ceylan::LoadableWithContent instance (here, a music), which records the
	   * path of the resource file and, if loaded, the resource itself.
	   *
	   */
	  std::map<Ceylan::ResourceID, Audio::MusicCountedPtr> _musicMap ;



	  /**
	   * Dictionary for sound resources.
	   *
	   * Each key of the map is a resource identifier, which is expected to
	   * correspond to a sound resource.
	   *
	   * Each associated value is a counted pointer on a
	   * Ceylan::LoadableWithContent instance (here, a sound), which records the
	   * path of the resource file and, if loaded, the resource itself.
	   *
	   */
	  std::map<Ceylan::ResourceID, Audio::SoundCountedPtr> _soundMap ;



	  /**
	   * Dictionary for image resources.
	   *
	   * Each key of the map is a resource identifier, which is expected to
	   * correspond to a image resource.
	   *
	   * Each associated value is a counted pointer on a
	   * Ceylan::LoadableWithContent instance (here, a image), which records the
	   * path of the resource file and, if loaded, the resource itself.
	   *
	   */
	  std::map<Ceylan::ResourceID,
		Video::TwoDimensional::ImageCountedPtr> _imageMap ;


	  /**
	   * Dictionary for texture resources (all kinds: 2D, 3D, etc.).
	   *
	   * Each key of the map is a resource identifier, which is expected to
	   * correspond to a texture resource.
	   *
	   * Each associated value is a counted pointer on a
	   * Ceylan::LoadableWithContent instance (here, a texture), which records
	   * the path of the resource file and, if loaded, the resource itself.
	   *
	   */
	  std::map<Ceylan::ResourceID,
		Video::OpenGL::TextureCountedPtr> _textureMap ;



	  /**
	   * Dictionary for TrueType fonts.
	   *
	   * Each key of the map is a resource identifier, which is expected to
	   * correspond to a TrueType font.
	   *
	   * Each associated value is a counted pointer on a
	   * Ceylan::LoadableWithContent instance (here, a TrueType font), which
	   * records the path of the resource file and, if loaded, the resource
	   * itself.
	   *
	   */
	  std::map<Ceylan::ResourceID,
		Video::TwoDimensional::Text::TrueTypeFontCountedPtr>
		_truetypeFontMap ;



	  /**
	   * Reverse dictionary: allows to convert a specified resource path into a
	   * resource identifier.
	   *
	   * This is useful when the resource path is to be established at runtime
	   * rather than at compile time (ex: if wanting to load a locale-dependent
	   * image, the simplest way is to forge the path at runtime based on the
	   * selected locale, rather than trying to determine and guess the
	   * corresponding identifier statically, for example with a 'switch
	   * (currentLocale) {...}'), and also to ensure that regardless of the
	   * resource package being used (ex: editor or game package), a shared
	   * resource (ex: a splash screen) will be referenced uniquely, by path
	   * name (whereas that image could be associated to different resource
	   * identifiers depending on the archive being read).
	   *
	   * Each key of the map is a resource path, each value is a resource
	   * identifier which then can be given back to a get* method.
	   *
	   */
	  std::map<std::string,Ceylan::ResourceID> _reverseMap ;


#pragma warning( pop )


	  /**
	   * Records the current highest resource identifier already allocated.
	   *
	   * Useful, once having read the resource file, to know from which ID
	   * dynamically added resources can have their own ID.
	   *
	   */
	  Ceylan::ResourceID _maxID ;



	private:



	  /**
	   * Copy constructor made private to ensure that it will never be called.
	   *
	   * The compiler should complain whenever this undefined constructor is
	   * called, implicitly or not.
	   *
	   */
	  explicit ResourceManager( const ResourceManager & source ) ;



	  /**
	   * Assignment operator made private to ensure that it will never be
	   * called.
	   *
	   * The compiler should complain whenever this undefined operator is
	   * called, implicitly or not.
	   *
	   */
	  ResourceManager & operator = (
		const ResourceManager & source ) ;



	} ;


  }

}



#endif // OSDL_RESOURCE_MANAGER_H_
