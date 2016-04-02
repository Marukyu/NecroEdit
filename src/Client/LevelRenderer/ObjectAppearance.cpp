/*
 * ObjectAppearance.cpp
 *
 *  Created on: Nov 1, 2015
 *      Author: marukyu
 */

#include <Client/Graphics/Packing/TexturePacker.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Shared/External/PugiXML/pugixml.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <memory>
#include <utility>

constexpr float TILE_SIZE = 24.f;

ObjectAppearanceManager::ObjectAppearanceManager(ITexturePacker * packer) :
		myPacker(packer)
{
}

void ObjectAppearanceManager::clearXMLObjects()
{
	myEnemies.clear();
	myItems.clear();
}

void ObjectAppearanceManager::clearTXTObjects()
{
	myTraps.clear();
	myCrates.clear();
	myChests.clear();
	myShrines.clear();
}

bool ObjectAppearanceManager::loadXMLObjects(const std::string & xmlData, const std::string & basePath)
{
	// Clear all XML-based objects.
	clearXMLObjects();

	// Create document to read necrodancer.xml's data into.
	pugi::xml_document doc;

	// Parse necrodancer.xml from parameter.
	pugi::xml_parse_result result = doc.load_buffer(xmlData.data(), xmlData.size());

	// Check if parsing was successful.
	if (!result)
	{
		return false;
	}

	// Load enemies.
	if (!loadEnemies(doc, basePath))
	{
		return false;
	}

	// Load items.
	if (!loadItems(doc, basePath + "items/"))
	{
		return false;
	}

	// Everything loaded successfully.
	return true;
}

bool ObjectAppearanceManager::loadTXTObjects(const std::string& txtData, const std::string& basePath)
{
	// Clear all TXT-based objects.
	clearTXTObjects();

	// Load traps.
	if (!loadTXTObjectsFromSection(txtData, basePath + "traps/", "Traps", myTraps))
	{
		return false;
	}

	// Load crates.
	if (!loadTXTObjectsFromSection(txtData, basePath + "entities/", "Crates", myCrates))
	{
		return false;
	}

	// Load chests.
	if (!loadTXTObjectsFromSection(txtData, basePath + "entities/", "Chests", myChests))
	{
		return false;
	}

	// Load shrines.
	if (!loadTXTObjectsFromSection(txtData, basePath + "level/", "Shrines", myShrines))
	{
		return false;
	}

	// Everything loaded successfully.
	return true;
}

bool ObjectAppearanceManager::loadEnemies(const pugi::xml_document& doc, const std::string& basePath)
{
	// Loop over all entries in the <enemies> tag.
	for (const auto & node : doc.root().child("necrodancer").child("enemies"))
	{
		// Check if the current enemy has an ID attribute.
		pugi::xml_attribute idAttribute = node.attribute("id");

		// No ID? Can't be saved in dungeon XMLs, skip this enemy.
		if (idAttribute.empty())
		{
			continue;
		}

		// Get ID from attribute.
		EnemyID id = idAttribute.as_uint();

		// Create enemy instance.
		EnemyAppearance enemy;

		// Get enemy name, fall back to node name if enemy name is nonexistent.
		enemy.name = node.attribute("name").as_string(capitalize(node.name()).c_str());

		// Get XML node containing sprite information.
		pugi::xml_node spritesheetNode = node.child("spritesheet");

		// Create image for full entity image file.
		std::unique_ptr<sf::Image> enemyImage = makeUnique<sf::Image>();

		// Load enemy image.
		if (!enemyImage->loadFromFile(basePath + spritesheetNode.child_value()))
		{
			// Load failure? Skip enemy.
			continue;
		}

		// Get cropping rectangle size.
		sf::Vector2u frameSize;
		frameSize.x = spritesheetNode.attribute("frameW").as_uint();
		frameSize.y = spritesheetNode.attribute("frameH").as_uint();

		// Create image for first-frame-cropped section.
		std::unique_ptr<sf::Image> croppedEnemyImage = makeUnique<sf::Image>();

		// Crop enemy image.
		croppedEnemyImage->create(frameSize.x, frameSize.y);
		croppedEnemyImage->copy(*enemyImage, 0, 0, sf::IntRect(0, 0, frameSize.x, frameSize.y));

		// Pack enemy image into texture packer.
		enemy.nodeID = myPacker->addOwn(std::move(croppedEnemyImage));

		// Read display offsets (default = 0).
		enemy.xOffset = spritesheetNode.attribute("xOff").as_float(0.f);
		enemy.yOffset = spritesheetNode.attribute("yOff").as_float(0.f);

		// Insert into enemy map.
		myEnemies.insert(std::make_pair(id, std::move(enemy)));
	}

	return true;
}

bool ObjectAppearanceManager::loadItems(const pugi::xml_document& doc, const std::string& basePath)
{
	// Loop over all entries in the <items> tag.
	for (const auto & node : doc.root().child("necrodancer").child("items"))
	{
		// Get item ID string from node name.
		std::string itemID = node.name();

		// Create item instance.
		ItemAppearance item;

		// Get item name from flyaway.
		std::string itemFlyaway = node.attribute("flyaway").as_string();
		std::vector<std::string> itemFlyawaySplit = splitString(itemFlyaway, "|");

		// Check if item name is correctly formatted.
		if (itemFlyawaySplit.size() >= 3)
		{
			// Item name is in the third component of the localization string, separated by pipes.
			item.name = itemFlyawaySplit[2];
		}
		else
		{
			// Item name is unavailable. Use item ID string instead, replacing underscores with spaces.
			item.name = itemID;
			std::replace(item.name.begin(), item.name.end(), '_', ' ');
		}

		// Capitalize item name properly.
		item.name = capitalize(toLowercase(item.name));

		// Create image for full item image file.
		std::unique_ptr<sf::Image> itemImage = makeUnique<sf::Image>();

		// Load item image.
		if (!itemImage->loadFromFile(basePath + node.child_value()))
		{
			// Load failure? Skip item.
			continue;
		}

		// Create cropping rectangle size.
		sf::Vector2u frameSize;

		// Check if item has size info attributes.
		if (!node.attribute("imageW").empty() && !node.attribute("imageH").empty())
		{
			// Get cropping rectangle size from attributes.
			frameSize.x = node.attribute("imageW").as_uint();
			frameSize.y = node.attribute("imageH").as_uint();
		}
		else
		{
			// Calculate cropping rectangle from image size and frame count (assume 1 frame by default).
			unsigned int frameCountX = node.attribute("numFrames").as_uint(1);

			// Divide image data by number of frames in each direction (assume 2 frames vertically, for shadow).
			frameSize.x = itemImage->getSize().x / frameCountX;
			frameSize.y = itemImage->getSize().y / 2;
		}

		// Create image for first-frame-cropped section.
		std::unique_ptr<sf::Image> croppedItemImage = makeUnique<sf::Image>();

		// Crop item image.
		croppedItemImage->create(frameSize.x, frameSize.y);
		croppedItemImage->copy(*itemImage, 0, 0, sf::IntRect(0, 0, frameSize.x, frameSize.y));

		// Pack item image into texture packer.
		item.nodeID = myPacker->addOwn(std::move(croppedItemImage));

		// Insert into item map.
		myItems.insert(std::make_pair(itemID, std::move(item)));
	}

	return true;
}

bool ObjectAppearanceManager::loadTXTObjectsFromSection(const std::string& txtData, const std::string& basePath,
		const std::string& section, std::map<ObjectID, ObjectAppearance> & target)
{
	// Create loader object.
	AppearanceLoader loader;

	// Create callback function.
	AppearanceLoader::CallbackFunction callback = [this,&target](AppearanceLoader::Appearance appearance)
	{
		// Forward function call to Manager object.
			onLoadObject(std::move(appearance), target);
		};

	// Perform load.
	return loader.loadAppearance(txtData, section, basePath, callback);
}

std::vector<sf::Vertex> ObjectAppearanceManager::getObjectVertices(const Object& object) const
{
	// Generate vertex array.
	std::vector<sf::Vertex> vertices;

	// Store common appearance data.
	sf::Vector2f offset(0.f, 0.f);
	sf::Vector2f scale(1.f, 1.f);
	ITexturePacker::NodeID nodeID;
	float alpha = 1.f;

	// Use object type to determine appearance data.
	if (object.getType() == Object::Type::Enemy)
	{
		auto it = myEnemies.find(object.getPropertyInt(Object::Property::Type));

		if (it == myEnemies.end())
		{
			return vertices;
		}

		const EnemyAppearance & enemy = it->second;

		offset.x = enemy.xOffset;
		offset.y = enemy.yOffset;

		nodeID = enemy.nodeID;

		if (object.getPropertyInt(Object::Property::Lord) != 0)
		{
			scale *= 2.f;
		}
	}
	else if (object.getType() == Object::Type::Item)
	{
		auto it = myItems.find(object.getPropertyString(Object::Property::Type));

		if (it == myItems.end())
		{
			return vertices;
		}

		const ItemAppearance & item = it->second;

		nodeID = item.nodeID;
	}
	else
	{
		const std::map<ObjectID, ObjectAppearance> * objectMap = nullptr;
		Object::Property typeProperty = Object::Property::Type;
		int variant = 0;

		switch (object.getType())
		{
		case Object::Type::Trap:
			objectMap = &myTraps;
			variant = object.getPropertyInt(Object::Property::Subtype);
			break;
		case Object::Type::Crate:
			objectMap = &myCrates;
			break;
		case Object::Type::Chest:
			objectMap = &myChests;
			typeProperty = Object::Property::Color;
			if (object.getPropertyInt(Object::Property::Hidden))
			{
				alpha *= 0.5f;
			}
			break;
		case Object::Type::Shrine:
			objectMap = &myShrines;
			break;
		default:
			return vertices;
		}

		auto it = objectMap->find(object.getPropertyInt(typeProperty));

		if (it == objectMap->end())
		{
			return vertices;
		}

		try
		{
			nodeID = it->second.nodeIDs.at(variant);
		} catch (...)
		{
			nodeID = ITexturePacker::packFailure;
		}
	}

	// Create vertex and texture rectangles.
	sf::IntRect texRect = myPacker->getImageRect(nodeID);
	sf::FloatRect vertRect(offset.x, offset.y, texRect.width * scale.x, texRect.height * scale.y);
	vertRect.left -= vertRect.width / 2.f;
	vertRect.top -= vertRect.height;

	// Base vertex position.
	sf::Vector2f vp(object.getPosition().x * TILE_SIZE, object.getPosition().y * TILE_SIZE);

	// Vertex corners.
	sf::Vector2f tl(vertRect.left, vertRect.top);
	sf::Vector2f tr(vertRect.left + vertRect.width, vertRect.top);
	sf::Vector2f br(vertRect.left + vertRect.width, vertRect.top + vertRect.height);
	sf::Vector2f bl(vertRect.left, vertRect.top + vertRect.height);

	// Texture corners.
	sf::Vector2f ttl(texRect.left, texRect.top);
	sf::Vector2f ttr(texRect.left + texRect.width, texRect.top);
	sf::Vector2f tbr(texRect.left + texRect.width, texRect.top + texRect.height);
	sf::Vector2f tbl(texRect.left, texRect.top + texRect.height);

	// Add vertices to array.
	vertices.push_back(sf::Vertex(vp + tl, sf::Color::White, ttl));
	vertices.push_back(sf::Vertex(vp + tr, sf::Color::White, ttr));
	vertices.push_back(sf::Vertex(vp + bl, sf::Color::White, tbl));
	vertices.push_back(sf::Vertex(vp + tr, sf::Color::White, ttr));
	vertices.push_back(sf::Vertex(vp + bl, sf::Color::White, tbl));
	vertices.push_back(sf::Vertex(vp + br, sf::Color::White, tbr));

	// Apply alpha-transparency.
	for (auto & vertex : vertices)
	{
		vertex.color.a *= alpha;
	}

	// Add extra vertices for chest/crate contents.
	if (object.getType() == Object::Type::Crate || object.getType() == Object::Type::Chest)
	{
		Object content(object.getPosition(), Object::Type::Item);

		content.setPropertyString(Object::Property::Type, object.getPropertyName(Object::Property::Contents));

		std::vector<sf::Vertex> contentVertices = getObjectVertices(content);

		for (auto & vertex : contentVertices)
		{
			vertex.color.a /= 2;
		}

		vertices.insert(vertices.end(), contentVertices.begin(), contentVertices.end());
	}

	return vertices;
}

const std::string& ObjectAppearanceManager::getObjectName(const Object& object) const
{
	static std::string invalidName = "Invalid";

	// Use object type to determine name.
	if (object.getType() == Object::Type::Enemy)
	{
		auto it = myEnemies.find(object.getPropertyInt(Object::Property::Type));

		if (it == myEnemies.end())
		{
			return invalidName;
		}

		return it->second.name;
	}
	else if (object.getType() == Object::Type::Item)
	{
		auto it = myItems.find(object.getPropertyString(Object::Property::Type));

		if (it == myItems.end())
		{
			return invalidName;
		}

		return it->second.name;
	}
	else
	{
		const std::map<ObjectID, ObjectAppearance> * objectMap = nullptr;
		Object::Property typeProperty = Object::Property::Type;

		switch (object.getType())
		{
		case Object::Type::Trap:
			objectMap = &myTraps;
			break;
		case Object::Type::Crate:
			objectMap = &myCrates;
			break;
		case Object::Type::Chest:
			objectMap = &myChests;
			typeProperty = Object::Property::Color;
			break;
		case Object::Type::Shrine:
			objectMap = &myShrines;
			break;
		default:
			return invalidName;
		}

		auto it = objectMap->find(object.getPropertyInt(typeProperty));

		if (it == objectMap->end())
		{
			return invalidName;
		}

		return it->second.name;
	}
}

std::vector<int> ObjectAppearanceManager::getObjectIDList(Object::Type type) const
{
	std::vector<int> idList;

	// Use object type to determine name.
	if (type == Object::Type::Enemy)
	{
		for (auto it = myEnemies.begin(); it != myEnemies.end(); ++it)
		{
			idList.push_back(it->first);
		}
		return idList;
	}
	else if (type == Object::Type::Item)
	{
		// This is invalid: return empty list. Use getItemNameList() instead.
		return idList;
	}
	else
	{
		const std::map<ObjectID, ObjectAppearance> * objectMap = nullptr;

		switch (type)
		{
		case Object::Type::Trap:
			objectMap = &myTraps;
			break;
		case Object::Type::Crate:
			objectMap = &myCrates;
			break;
		case Object::Type::Chest:
			objectMap = &myChests;
			break;
		case Object::Type::Shrine:
			objectMap = &myShrines;
			break;
		default:
			// This is invalid: return empty list.
			return idList;
		}

		for (auto it = objectMap->begin(); it != objectMap->end(); ++it)
		{
			idList.push_back(it->first);
		}
		return idList;
	}
}

std::vector<int> ObjectAppearanceManager::getObjectSubtypeList(Object::Type type, int objectTypeID)
{
	std::vector<int> subtypeList;

	const std::map<ObjectID, ObjectAppearance> * objectMap = nullptr;

	switch (type)
	{
	case Object::Type::Trap:
		objectMap = &myTraps;
		break;
	case Object::Type::Crate:
		objectMap = &myCrates;
		break;
	case Object::Type::Chest:
		objectMap = &myChests;
		break;
	case Object::Type::Shrine:
		objectMap = &myShrines;
		break;
	default:
		// This is invalid: return empty list.
		return subtypeList;
	}

	auto it = objectMap->find(objectTypeID);

	if (it != objectMap->end())
	{
		for (int i = 0; i < int(it->second.nodeIDs.size()); ++i)
		{
			if (it->second.nodeIDs[i] != TexturePacker::packFailure)
			{
				subtypeList.push_back(i);
			}
		}
	}

	return subtypeList;
}

std::vector<std::string> ObjectAppearanceManager::getItemNameList() const
{
	std::vector<std::string> itemNameList;

	for (auto it = myItems.begin(); it != myItems.end(); ++it)
	{
		itemNameList.push_back(it->first);
	}

	return itemNameList;
}

const sf::Texture* ObjectAppearanceManager::getTexture() const
{
	return myPacker->getTexture();
}

void ObjectAppearanceManager::onLoadObject(AppearanceLoader::Appearance appearance,
		std::map<ObjectID, ObjectAppearance>& target)
{
	// Create object appearance entry (if necessary) and get a reference to it.
	ObjectAppearance & object = target[appearance.id];

	// Apply the object name.
	object.name = appearance.name;

	// Increase object variant array size to hold current variant.
	while (object.nodeIDs.size() <= appearance.variant)
	{
		// Add invalid ID in case of in-between null variants.
		object.nodeIDs.push_back(TexturePacker::packFailure);
	}

	// Check if at least one extra parameter exists.
	if (appearance.custom.size() >= 1)
	{
		// Get "flipped" flag from first extra parameter.
		if (appearance.custom[0] == "flipped")
		{
			appearance.image->flipHorizontally();
		}
	}

	// Use clipped image and pack it into main texture.
	object.nodeIDs[appearance.variant] = myPacker->addOwn(std::move(appearance.image));
}
