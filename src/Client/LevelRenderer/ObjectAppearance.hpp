/*
 * ObjectAppearance.hpp
 *
 *  Created on: Nov 1, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_LEVELRENDERER_OBJECTAPPEARANCE_HPP_
#define SRC_CLIENT_LEVELRENDERER_OBJECTAPPEARANCE_HPP_

#include <Client/Graphics/Packing/ITexturePacker.hpp>
#include <Client/LevelRenderer/AppearanceLoader.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <Shared/Level/Object.hpp>
#include <map>
#include <string>
#include <vector>

namespace pugi
{
class xml_document;
}

class ObjectAppearanceManager
{
public:

	/**
	 * Holds special object type indices for editor-specific graphics.
	 */
	enum InternalObjectType
	{
		// This object looks like a player character when drawn. Subtype determines the character ID.
		InternalCharacter
	};

	ObjectAppearanceManager(ITexturePacker * packer);

	/**
	 * Unloads all enemies and items from the appearance manager.
	 * 
	 * Does NOT clear the texture packer, this has to be done externally.
	 */
	void clearXMLObjects();

	/**
	 * Unloads all traps, crates, chests and shrines from the appearance manager.
	 * 
	 * Does NOT clear the texture packer, this has to be done externally.
	 */
	void clearTXTObjects();

	/**
	 * Loads enemy and item appearance information from a file. xmlData holds necrodancer.xml's content.
	 * 
	 * basePath points to the directory containing the NecroDancer data files. It is expected to end with a slash.
	 * 
	 * Does NOT clear the texture packer and will duplicate existing images; clear the packer before calling this
	 * function!
	 */
	bool loadXMLObjects(const std::string & xmlData, const std::string & basePath);

	/**
	 * Loads trap, crate, chest and shrine appearance information from a file.
	 * 
	 * basePath points to the directory containing the image files. It is expected to end with a slash.
	 * 
	 * Does NOT clear the texture packer and will duplicate existing images; clear the packer before calling this
	 * function!
	 */
	bool loadTXTObjects(const std::string & txtData, const std::string & basePath);

	/**
	 * Returns the vertices for the specified object.
	 */
	std::vector<sf::Vertex> getObjectVertices(const Object & object) const;

	/**
	 * Returns the name of the specified object.
	 */
	const std::string & getObjectName(const Object & object) const;

	/**
	 * Returns the list of valid values for the "type" field for the specified object type.
	 * 
	 * This does not work on items, as they use a string-based identification system.
	 * 
	 * For chests, the "color" field should be varied, rather than the "type" field.
	 */
	std::vector<int> getObjectIDList(Object::Type type) const;

	/**
	 * Returns a list of valid values for the "subtype" for the specified object type and object ID.
	 */
	std::vector<int> getObjectSubtypeList(Object::Type type, int objectTypeID);

	/**
	 * Returns the list of valid values for the "type" field for items.
	 */
	std::vector<std::string> getItemNameList() const;

	/**
	 * Returns the texture of the texture packer used for loading objects into this appearance manager.
	 */
	const sf::Texture * getTexture() const;

private:

	typedef unsigned int EnemyID;
	typedef unsigned int ObjectID;

	struct EnemyAppearance
	{
		std::string name;
		ITexturePacker::NodeID nodeID;
		float xOffset;
		float yOffset;
	};

	struct ItemAppearance
	{
		std::string name;
		ITexturePacker::NodeID nodeID;
	};

	struct ObjectAppearance
	{
		std::string name;
		std::vector<ITexturePacker::NodeID> nodeIDs;
	};

	struct SpriteData
	{
		SpriteData();

		sf::Vector2i position;
		sf::Vector2f offset;
		sf::Vector2f scale;
		ITexturePacker::NodeID nodeID;
		float alpha;
	};

	bool loadEnemies(const pugi::xml_document & doc, const std::string & basePath);
	bool loadItems(const pugi::xml_document & doc, const std::string & basePath);

	bool loadTXTObjectsFromSection(const std::string & txtData, const std::string & basePath,
			const std::string & section, std::map<ObjectID, ObjectAppearance> & target);

	void onLoadObject(AppearanceLoader::Appearance appearance, std::map<ObjectID, ObjectAppearance> & target);

	static void applyScaleFactor(sf::Vector2f & position, sf::Vector2f scaleFactor);
	std::vector<sf::Vertex> generateSpriteVertices(SpriteData spriteData) const;

	ITexturePacker * myPacker;

	std::map<EnemyID, EnemyAppearance> myEnemies;
	std::map<std::string, ItemAppearance> myItems;

	std::map<ObjectID, ObjectAppearance> myTraps;
	std::map<ObjectID, ObjectAppearance> myCrates;
	std::map<ObjectID, ObjectAppearance> myChests;
	std::map<ObjectID, ObjectAppearance> myShrines;

	std::map<ObjectID, ObjectAppearance> myCharacters;
};

#endif
