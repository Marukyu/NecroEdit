#include <Shared/Editor/Brush.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <Shared/Utils/MiscMath.hpp>
#include <algorithm>
#include <stdexcept>

class Brush;

Level::Level() :
		boss(-1),
		music(0),
		objectObserver(this)
{
}

Level::~Level()
{
}

void Level::assign(const Level& level)
{
	boss = level.boss;
	music = level.music;
	sections = level.sections;

	objects.clear();
	objects.reserve(level.objects.size());

	for (Object::ID objectID = 0; objectID < level.objects.size(); ++objectID)
	{
		if (level.objects[objectID] == nullptr)
		{
			objects.push_back(nullptr);
		}
		else
		{
			objects.push_back(makeUnique<Object>(*level.objects[objectID]));
			objects[objectID]->setID(objectID);
			objects[objectID]->setObserver(&objectObserver);
		}
	}
}

void Level::setTileAt(sf::Vector2i position, Tile tile)
{
	Section & section = sections[positionToSection(position)];

	Tile & targetTile = section.tiles[positionToTileIndex(position)];

	Event event;

	event.tilePosition = position;

	if (targetTile.exists())
	{
		if (tile.exists())
		{
			event.type = Event::TileChanged;
		}
		else
		{
			event.type = Event::TileRemoved;
			section.tileCount--;
		}
	}
	else
	{
		if (!tile.exists())
		{
			removeSectionIfEmpty(position);
			return;
		}

		event.type = Event::TileAdded;
		section.tileCount++;
	}

	targetTile = tile;

	removeSectionIfEmpty(position);

	eventManager.push(event);
}

Tile Level::getTileAt(sf::Vector2i position) const
{
	auto it = sections.find(positionToSection(position));

	if (it == sections.end())
	{
		return Tile();
	}
	else
	{
		return it->second.tiles[positionToTileIndex(position)];
	}
}

bool Level::hasTileAt(sf::Vector2i position) const
{
	return getTileAt(position).exists();
}

Object::ID Level::addObject(Object::Type type)
{
	std::unique_ptr<Object> object = makeUnique<Object>(type);

	object->setObserver(&objectObserver);

	bool hasID = false;
	Object::ID id;

	for (std::size_t i = 0; i < objects.size(); ++i)
	{
		if (objects[i] == nullptr)
		{
			hasID = true;
			id = i;
			break;
		}
	}

	if (!hasID)
	{
		id = objects.size();
		objects.push_back(nullptr);
	}

	object->setID(id);
	objects[id] = std::move(object);

	addObjectToSection(id, getObject(id).getPosition());

	Event event;
	event.type = Event::ObjectAdded;
	event.objectID = id;
	eventManager.push(event);

	return id;
}

void Level::removeObject(Object::ID id)
{
	Event event;
	event.type = Event::ObjectRemoved;
	event.objectID = id;
	eventManager.push(event);

	removeObjectFromSection(id, objects[id]->getPosition());

	if (id < objects.size())
	{
		objects[id] = nullptr;

		while (objects.back() == nullptr)
		{
			objects.pop_back();
		}
	}
}

std::vector<Object::ID> Level::getObjectsAt(sf::Vector2i position) const
{
	std::vector<Object::ID> result;

	auto it = sections.find(positionToSection(position));

	if (it != sections.end())
	{
		for (Object::ID id : it->second.objects)
		{
			if (objects[id]->getPosition() == position)
			{
				result.push_back(id);
			}
		}
	}

	return result;
}

bool Level::hasObject(Object::ID id) const
{
	return id < objects.size() && objects[id] != nullptr;
}

bool Level::hasObjectAt(sf::Vector2i position) const
{
	return !getObjectsAt(position).empty();
}

Object& Level::getObject(Object::ID id) const
{
	if (!hasObject(id))
	{
		throw std::out_of_range("Reference to non-existent object");
	}

	return *objects[id];
}

void Level::applyBrush(sf::Vector2i position, const Brush& brush)
{
	bool applyToTile;

	switch (brush.getTileMode())
	{
	case Brush::TileMode::Ignore:
	default:
		applyToTile = false;
		break;

	case Brush::TileMode::ReplaceAny:
		applyToTile = true;
		break;

	case Brush::TileMode::ReplaceExisting:
		applyToTile = hasTileAt(position);
		break;

	case Brush::TileMode::ReplaceEmpty:
		applyToTile = !hasTileAt(position);
		break;

	case Brush::TileMode::Erase:
		applyToTile = hasTileAt(position);
		break;
	}

	if (applyToTile)
	{
		Tile tile;

		if (brush.getTileMode() == Brush::TileMode::Erase)
		{
			// Leave tile uninitialized/empty to erase it.
		}
		else if (brush.getTileMask() == Brush::TileMask::Full)
		{
			tile = brush.getTile();
		}
		else
		{
			tile = getTileAt(position);

			if ((brush.getTileMask() & Brush::TileMask::ID) != Brush::TileMask::None)
			{
				tile.id = brush.getTile().id;
			}

			if ((brush.getTileMask() & Brush::TileMask::Zone) != Brush::TileMask::None)
			{
				tile.setZone(brush.getTile().getZone());
			}

			if ((brush.getTileMask() & Brush::TileMask::Torch) != Brush::TileMask::None)
			{
				tile.hasTorch = brush.getTile().hasTorch;
			}

			if ((brush.getTileMask() & Brush::TileMask::Cracked) != Brush::TileMask::None)
			{
				tile.setCracked(brush.getTile().isCracked());
			}
		}

		setTileAt(position, tile);
	}

	bool eraseTopObject = false;
	bool eraseAllObjects = false;
	bool createObject = false;

	switch (brush.getObjectMode())
	{
	default:
	case Brush::ObjectMode::Ignore:
		break;

	case Brush::ObjectMode::Add:
		createObject = true;
		break;

	case Brush::ObjectMode::ReplaceAll:
		eraseAllObjects = true;
		createObject = true;
		break;

	case Brush::ObjectMode::PlaceOnEmpty:
		createObject = !hasObjectAt(position);
		break;

	case Brush::ObjectMode::ReplaceTopMost:
		eraseTopObject = true;
		createObject = hasObjectAt(position);
		break;

	case Brush::ObjectMode::ReplaceTopMostOrCreate:
		eraseTopObject = true;
		createObject = true;
		break;

	case Brush::ObjectMode::EraseAll:
		eraseAllObjects = true;
		break;

	case Brush::ObjectMode::EraseTopMost:
		eraseTopObject = true;
		break;
	}

	// Check if any objects need to be erased.
	if (eraseTopObject || eraseAllObjects)
	{
		auto existingObjects = getObjectsAt(position);

		// Check how many objects are requested to be erased.
		if (eraseAllObjects)
		{
			// Remove all objects at the brush position.
			for (auto objectID : existingObjects)
			{
				removeObject(objectID);
			}
		}
		else if (!existingObjects.empty())
		{
			// Remove topmost object only, if it exists.
			removeObject(existingObjects.back());
		}
	}

	// Check if the brush object should be placed.
	if (createObject)
	{
		Object & object = getObject(addObject());
		object.assign(brush.getObject());
		object.setPosition(position);
	}
}

void Level::setPlayerSpawnPoint(sf::Vector2i playerSpawn)
{
	if (this->playerSpawn != playerSpawn)
	{
		this->playerSpawn = playerSpawn;

		Event event;
		event.type = Event::SpawnPointMoved;
		eventManager.push(event);
	}
}

sf::Vector2i Level::getPlayerSpawnPoint() const
{
	return playerSpawn;
}

void Level::setBoss(int boss)
{
	this->boss = boss;
}

int Level::getBoss() const
{
	return boss;
}

bool Level::isBoss() const
{
	return boss != -1;
}

void Level::setMusic(int music)
{
	this->music = music;
}

int Level::getMusic() const
{
	return music;
}

EventListener<Level::Event> Level::acquireEventListener() const
{
	return eventManager.acquireListener();
}

Level::TilePositionIterator Level::tilesBegin() const
{
	// If the level is completely empty, return end iterator.
	if (sections.empty())
	{
		return tilesEnd();
	}

	// Get map iterator to first section.
	auto it = sections.begin();

	// Calculate section's top left tile coordinate.
	sf::Vector2i firstSectionTopLeft = sectionToPosition(it->first);

	// Get reference to section.
	const Section & firstSection = it->second;

	// Create iterator to be returned.
	TilePositionIterator tilePosIterator(this, firstSectionTopLeft);

	// Check if first section's first tile does not exist.
	if (!firstSection.tiles[0].exists())
	{
		// If no tile exists, advance iterator to find first valid tile in section.
		advanceTileIterator(tilePosIterator);
	}

	// Return the newly created iterator.
	return tilePosIterator;
}

Level::TilePositionIterator Level::tilesEnd() const
{
	// Return null iterator.
	return TilePositionIterator(nullptr, sf::Vector2i());
}

Level::ObjectIterator Level::objectsBegin() const
{
	// If the level has no objects, return end iterator.
	if (objects.empty())
	{
		return objectsEnd();
	}

	// Create iterator to be returned.
	ObjectIterator objectIterator(this, 0);

	// Check if object 0 does not exist.
	if (objects[0] == nullptr)
	{
		// If no object exists, advance iterator to find first valid object in the level.
		advanceObjectIterator(objectIterator);
	}

	// Return the iterator.
	return objectIterator;
}

Level::ObjectIterator Level::objectsEnd() const
{
	// Return null iterator.
	return ObjectIterator(nullptr, 0);
}

void Level::onObjectMoved(Object::ID id, sf::Vector2i oldPosition)
{
	removeObjectFromSection(id, oldPosition);
	onObjectUpdated(id);
	addObjectToSection(id, getObject(id).getPosition());
}

void Level::onObjectUpdated(Object::ID id)
{
	Event event;

	event.type = Event::ObjectChanged;
	event.objectID = id;

	eventManager.push(event);
}

void Level::addObjectToSection(Object::ID id, sf::Vector2i position)
{
	// Create section if it does not currently exist.
	Section & section = sections[positionToSection(position)];

	auto & objs = section.objects;
	auto location = std::lower_bound(objs.begin(), objs.end(), id);
	if (location == objs.end() || *location != id)
	{
		objs.insert(location, id);
	}
}

void Level::removeObjectFromSection(Object::ID id, sf::Vector2i position)
{
	auto it = sections.find(positionToSection(position));

	if (it != sections.end())
	{
		auto & objs = it->second.objects;
		auto location = std::lower_bound(objs.begin(), objs.end(), id);
		if (location != objs.end() && *location == id)
		{
			objs.erase(location);
		}
	}

	removeSectionIfEmpty(position);
}

void Level::removeSectionIfEmpty(sf::Vector2i position)
{
	auto it = sections.find(positionToSection(position));

	if (it != sections.end() && it->second.tileCount == 0 && it->second.objects.size() == 0)
	{
		sections.erase(it);
	}
}

void Level::advanceTileIterator(TilePositionIterator& iterator) const
{
	// Get current section of tile.
	sf::Vector2i sectionPos = positionToSection(iterator.position);

	// Get map iterator to current section.
	auto it = sections.find(sectionPos);

	// Calculate section's top left tile coordinate.
	sf::Vector2i sectionTopLeft = sectionToPosition(it->first);

	// Get reference to section.
	const Section & section = it->second;

	// Find next valid tile in section.
	for (std::size_t i = positionToTileIndex(iterator.position) + 1; i < SECTION_SIZE * SECTION_SIZE; ++i)
	{
		// Check if tile exists.
		if (section.tiles[i].exists())
		{
			// Assign found tile.
			iterator.position = sectionTopLeft + tileIndexToSubPosition(i);
			return;
		}
	}

	// If no more tiles exist in the current section, check next section.
	it++;

	// Check if the iterator's current section was the last one.
	if (it == sections.end())
	{
		// Set to end iterator.
		iterator = tilesEnd();
		return;
	}

	// Recalculate section's top left tile coordinate.
	sectionTopLeft = sectionToPosition(it->first);

	// Get updated reference to section.
	const Section & newSection = it->second;

	// Assign iterator to top-left corner of section.
	iterator.position = sectionTopLeft;

	// Check if first section's first tile exists.
	if (!newSection.tiles[0].exists())
	{
		// If no tile exists, recursively advance iterator to find first valid tile in section.
		advanceTileIterator(iterator);
	}
}

void Level::advanceObjectIterator(ObjectIterator& iterator) const
{
	// Repeat until one of the conditions within the loop is triggered.
	while (true)
	{
		// Increment iterator.
		iterator.objectID++;

		// Check if iterator is out of bounds.
		if (iterator.objectID >= objects.size())
		{
			iterator = objectsEnd();
			break;
		}

		// Check if object with current iterator ID exists.
		if (objects[iterator.objectID] != nullptr)
		{
			break;
		}
	}
}

sf::Vector2i Level::sectionToPosition(sf::Vector2i sectionPosition)
{
	return sf::Vector2i(sectionPosition.x * SECTION_SIZE, sectionPosition.y * SECTION_SIZE);
}

sf::Vector2i Level::positionToSection(sf::Vector2i position)
{
	return sf::Vector2i(floorDivP<int>(position.x, SECTION_SIZE), floorDivP<int>(position.y, SECTION_SIZE));
}

std::size_t Level::positionToTileIndex(sf::Vector2i position)
{
	return sgnMod<int>(position.x, SECTION_SIZE) + sgnMod<int>(position.y, SECTION_SIZE) * SECTION_SIZE;
}

sf::Vector2i Level::tileIndexToSubPosition(std::size_t index)
{
	return sf::Vector2i(index % SECTION_SIZE, index / SECTION_SIZE);
}
