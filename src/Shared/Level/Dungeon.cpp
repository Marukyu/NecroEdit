#include <boost/filesystem/path.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/External/PugiXML/pugixml.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Level/Tile.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>

Dungeon::Dungeon() :
		playerCharacter(0),
		startItemsEnabled(true)
{
}

Dungeon::~Dungeon()
{
}

void Dungeon::insertLevel(std::size_t index)
{
	if (index > levels.size())
	{
		return;
	}

	levels.insert(levels.begin() + index, makeUnique<Level>());
}

void Dungeon::removeLevel(std::size_t index)
{
	if (index >= levels.size())
	{
		return;
	}

	levels.erase(levels.begin() + index);
}

void Dungeon::swapLevels(std::size_t indexA, std::size_t indexB)
{
	if (indexA == indexB || indexA >= levels.size() || indexB >= levels.size())
	{
		return;
	}
	
	std::swap(levels[indexA], levels[indexB]);
}

std::size_t Dungeon::getLevelCount() const
{
	return levels.size();
}

Level& Dungeon::getLevel(std::size_t index) const
{
	if (index >= levels.size())
	{
		throw(std::out_of_range("Level " + std::to_string(index) + " does not exist."));
	}

	return *levels[index];
}

void Dungeon::setPlayerCharacter(int character)
{
	playerCharacter = character;
}

int Dungeon::getPlayerCharacter() const
{
	return playerCharacter;
}

void Dungeon::setStartItemsEnabled(bool enabled)
{
	startItemsEnabled = enabled;
}

bool Dungeon::isStartItemsEnabled() const
{
	return startItemsEnabled;
}

static const std::vector<Object::Type> OBJECT_TYPE_ORDER = { Object::Type::Trap, Object::Type::Enemy,
		Object::Type::Item, Object::Type::Chest, Object::Type::Crate, Object::Type::Shrine };

bool Dungeon::loadFromXML(const std::string& filename)
{
	// Create string to read dungeon file from.
	std::string xmlData = readFile(filename);

	// Create document to read the dungeon's data into.
	pugi::xml_document doc;

	// Parse dungeon XML from file contents.
	pugi::xml_parse_result result = doc.load_buffer(xmlData.data(), xmlData.size());

	// Check if parsing was successful.
	if (!result)
	{
		return false;
	}

	pugi::xml_node dungeonNode = doc.root().child("dungeon");

	if (dungeonNode.empty())
	{
		return false;
	}

	levels.clear();

	int characterID = dungeonNode.attribute("character").as_int(-1);

	if (characterID >= 999)
	{
		characterID -= 1000;
		startItemsEnabled = false;
	}
	else
	{
		startItemsEnabled = true;
	}

	playerCharacter = characterID;

	name = dungeonNode.attribute("name").as_string(boost::filesystem::path(filename).stem().string().c_str());

	std::size_t levelCount = dungeonNode.attribute("numLevels").as_uint(0);

	for (std::size_t i = 0; i < levelCount; ++i)
	{
		insertLevel(i);
	}

	// Loop over all levels in the XML.
	for (const auto & levelNode : dungeonNode)
	{
		int levelID = levelNode.attribute("num").as_int(0) - 1;

		if (levelID == -1 || levelID >= int(levels.size()))
		{
			continue;
		}

		Level & level = getLevel(levelID);

		level.setBoss(levelNode.attribute("boss").as_int(-1));
		level.setMusic(levelNode.attribute("music").as_int(0));

		// Load tiles.
		loadTiles(levelNode, levelID);

		// Load objects.
		for (Object::Type type : OBJECT_TYPE_ORDER)
		{
			loadObjects(levelNode, levelID, type);
		}
	}

	return true;
}

void Dungeon::loadTiles(const pugi::xml_node& levelNode, std::size_t levelNumber)
{
	Level & level = getLevel(levelNumber);

	for (const auto & tileNode : levelNode.child("tiles"))
	{
		sf::Vector2i position(tileNode.attribute("x").as_int(0), tileNode.attribute("y").as_int(0));

		Tile tile;

		tile.id = tileNode.attribute("type").as_int(Tile::Invalid);
		tile.hasTorch = tileNode.attribute("torch").as_bool(false);
		tile.variant = Tile::Variant(tileNode.attribute("zone").as_int(0));

		if (tileNode.attribute("cracked").as_bool(false))
		{
			tile.setCracked(true);
		}

		level.setTileAt(position, tile);
	}
}

void Dungeon::loadObjects(const pugi::xml_node& levelNode, std::size_t levelNumber, Object::Type objectType)
{
	Level & level = getLevel(levelNumber);

	for (const auto & objectNode : levelNode.child(Object::getTypeNamePlural(objectType)))
	{
		sf::Vector2i position(objectNode.attribute("x").as_int(0), objectNode.attribute("y").as_int(0));

		Object & object = level.getObject(level.addObject(objectType));

		object.setPosition(position);

		for (auto attrib = objectNode.attributes_begin(); attrib != objectNode.attributes_end(); ++attrib)
		{
			// Get name and (string) value of current property.
			Object::Property propertyKey = Object::getPropertyByName(attrib->name());

			// Ignore unknown attributes (including position).
			if (propertyKey == Object::Property::Invalid)
			{
				continue;
			}

			const char * propertyValue = attrib->value();

			// Assign property as string. Conversion into number is done later if necessary.
			object.setPropertyString(propertyKey, propertyValue);
		}
	}
}

bool Dungeon::saveToXML(const std::string& filename) const
{
	// Create document to generate the dungeon's data in.
	pugi::xml_document doc;

	// Create dungeon node.
	pugi::xml_node dungeonNode = doc.root().append_child("dungeon");

	// Add dungeon attributes.
	dungeonNode.append_attribute("character").set_value(playerCharacter + (isStartItemsEnabled() ? 0 : 1000));
	dungeonNode.append_attribute("name").set_value(boost::filesystem::path(filename).stem().string().c_str());
	dungeonNode.append_attribute("numLevels").set_value((int) levels.size());

	// Add levels.
	for (std::size_t levelID = 0; levelID < levels.size(); ++levelID)
	{
		// Select current level.
		Level & level = getLevel(levelID);

		// Create level node.
		pugi::xml_node levelNode = dungeonNode.append_child("level");

		// Add level attributes.
		levelNode.append_attribute("bossNum").set_value(level.getBoss());
		levelNode.append_attribute("music").set_value(level.getMusic());
		levelNode.append_attribute("num").set_value((int) levelID + 1);

		// Save various components (tiles and objects) of the level.
		saveTiles(levelNode, levelID);
		saveObjects(levelNode, levelID);
	}

	// Save document to file.
	return doc.save_file(filename.c_str());
}

void Dungeon::saveTiles(pugi::xml_node& levelNode, std::size_t levelNumber) const
{
	// Create tiles node.
	pugi::xml_node tilesNode = levelNode.append_child("tiles");

	Level & level = getLevel(levelNumber);
	sf::Vector2i playerSpawn = level.getPlayerSpawnPoint();

	// Loop over all tiles in the level.
	for (auto it = level.tilesBegin(); it != level.tilesEnd(); ++it)
	{
		// Create tile node.
		pugi::xml_node tileNode = tilesNode.append_child("tile");

		// Get tile at current iterator position.
		const Tile & tile = level.getTileAt(*it);

		// Assign attributes.
		tileNode.append_attribute("x").set_value(it->x - playerSpawn.x);
		tileNode.append_attribute("y").set_value(it->y - playerSpawn.y);
		tileNode.append_attribute("type").set_value(tile.id);
		tileNode.append_attribute("zone").set_value(tile.getZone());
		tileNode.append_attribute("torch").set_value(tile.hasTorch ? 1 : 0);
		tileNode.append_attribute("cracked").set_value(tile.isCracked() ? 1 : 0);
	}
}

void Dungeon::saveObjects(pugi::xml_node& levelNode, std::size_t levelNumber) const
{
	std::vector<pugi::xml_node> objectNodes;

	for (Object::Type type : OBJECT_TYPE_ORDER)
	{
		if ((int) type < 0)
		{
			continue;
		}
		objectNodes.resize(std::max<std::size_t>(objectNodes.size(), (std::size_t) type + 1));
		objectNodes[(std::size_t) type] = levelNode.append_child(Object::getTypeNamePlural(type));
	}

	Level & level = getLevel(levelNumber);
	sf::Vector2i playerSpawn = level.getPlayerSpawnPoint();

	// Loop over all objects in the level.
	for (auto it = level.objectsBegin(); it != level.objectsEnd(); ++it)
	{
		if ((int) it->getType() >= (int) objectNodes.size() || (int) it->getType() < 0)
		{
			continue;
		}

		// Create object node.
		pugi::xml_node objectNode = objectNodes[(std::size_t) it->getType()].append_child(
			Object::getTypeName(it->getType()));

		// Assign common attributes.
		objectNode.append_attribute("x").set_value(it->getPosition().x - playerSpawn.x);
		objectNode.append_attribute("y").set_value(it->getPosition().y - playerSpawn.y);

		// Assign properties.
		for (auto itProp = it->getAllProperties().begin(); itProp != it->getAllProperties().end(); ++itProp)
		{
			const char * propertyName = Object::getPropertyName(itProp->first);
			objectNode.append_attribute(propertyName).set_value(itProp->second.c_str());
		}
	}
}
