#ifndef SRC_SHARED_LEVEL_LEVEL_HPP_
#define SRC_SHARED_LEVEL_LEVEL_HPP_

#include <SFML/System/Vector2.hpp>
#include <Shared/Level/Object.hpp>
#include <Shared/Level/ObjectObserver.hpp>
#include <Shared/Level/Tile.hpp>
#include <Shared/Utils/Event/EventListener.hpp>
#include <Shared/Utils/Event/EventManager.hpp>
#include <Shared/Utils/VectorComp.hpp>
#include <array>
#include <cstddef>
#include <map>
#include <memory>
#include <vector>

class Brush;

class Level
{
public:

	class TilePositionIterator
	{
	public:

		sf::Vector2i operator*() const
		{
			return position;
		}

		const sf::Vector2i * operator->() const
		{
			return &position;
		}

		TilePositionIterator & operator++()
		{
			if (level)
				level->advanceTileIterator(*this);

			return *this;
		}

		TilePositionIterator operator++(int)
		{
			auto oldThis = *this;

			if (level)
				level->advanceTileIterator(*this);

			return oldThis;
		}

		bool operator==(const TilePositionIterator & it) const
		{
			return level == it.level && position == it.position;
		}

		bool operator!=(const TilePositionIterator & it) const
		{
			return !(*this == it);
		}

	private:

		TilePositionIterator(const Level * level, sf::Vector2i position) :
				level(level),
				position(position)
		{
		}

		const Level * level;
		sf::Vector2i position;

		friend class Level;
	};

	class ObjectIterator
	{
	public:

		Object & operator*() const
		{
			return level->getObject(objectID);
		}

		Object * operator->() const
		{
			return &level->getObject(objectID);
		}

		ObjectIterator & operator++()
		{
			if (level)
				level->advanceObjectIterator(*this);

			return *this;
		}

		ObjectIterator operator++(int)
		{
			auto oldThis = *this;

			if (level)
				level->advanceObjectIterator(*this);

			return oldThis;
		}

		bool operator==(const ObjectIterator & it) const
		{
			return level == it.level && objectID == it.objectID;
		}

		bool operator!=(const ObjectIterator & it) const
		{
			return !(*this == it);
		}

	private:

		ObjectIterator(const Level * level, Object::ID objectID) :
				level(level),
				objectID(objectID)
		{
		}

		const Level * level;
		Object::ID objectID;

		friend class Level;
	};

	struct Event
	{
		enum Type
		{
			None,
			TileAdded,
			TileRemoved,
			TileChanged,
			ObjectAdded,
			ObjectRemoved,
			ObjectChanged
		};

		Event() :
				type(None),
				tilePosition(),
				objectID(0)
		{
		}

		Type type;

		sf::Vector2i tilePosition;
		Object::ID objectID;
	};

	Level();
	~Level();
	
	/**
	 * Copies the settings, contents and tiles from another level to this one.
	 */
	void assign(const Level & level);

	/**
	 * Changes the tile at the specified position.
	 */
	void setTileAt(sf::Vector2i position, Tile tile);

	/**
	 * Returns the tile at the specified position.
	 */
	Tile getTileAt(sf::Vector2i position) const;

	/**
	 * Checks if a tile exists at the specified position.
	 */
	bool hasTileAt(sf::Vector2i position) const;

	/**
	 * Creates an object with the specified type and returns its ID.
	 */
	Object::ID addObject(Object::Type type = Object::Type::None);

	/**
	 * Removes the specified object if it exists.
	 */
	void removeObject(Object::ID id);

	/**
	 * Returns a list of object IDs at the specified position.
	 */
	std::vector<Object::ID> getObjectsAt(sf::Vector2i position) const;

	/**
	 * Returns true if the object with the specified ID exists.
	 */
	bool hasObject(Object::ID id) const;

	/**
	 * Returns true if at least one object exists at the specified position.
	 */
	bool hasObjectAt(sf::Vector2i position) const;

	/**
	 * Returns a reference to the object with the specified ID.
	 */
	Object & getObject(Object::ID id) const;

	/**
	 * Applies a brush to a single tile position.
	 */
	void applyBrush(sf::Vector2i position, const Brush & brush);

	/**
	 * Sets the position at which the player starts in the level. Defaults to (0;0).
	 */
	void setPlayerSpawnPoint(sf::Vector2i playerSpawn);

	/**
	 * Returns the position at which the player starts in the level.
	 */
	sf::Vector2i getPlayerSpawnPoint() const;

	/**
	 * Sets which boss battle should generate in this level. Setting a boss overrides any tiles or objects in the level.
	 */
	void setBoss(int boss);

	/**
	 * Returns which boss this level contains (if any).
	 */
	int getBoss() const;

	/**
	 * Returns true if this level is a boss level, false otherwise.
	 */
	bool isBoss() const;

	/**
	 * Sets which music plays in this level.
	 */
	void setMusic(int music);

	/**
	 * Returns which music plays in this level.
	 */
	int getMusic() const;

	/**
	 * Returns an event listener for level-related events.
	 */
	EventListener<Event> acquireEventListener() const;

	/**
	 * Returns an iterator for the first tile in the level, iterating over all tiles in an unspecified order.
	 */
	TilePositionIterator tilesBegin() const;

	/**
	 * Returns an iterator for the one-past-last tile in the level.
	 */
	TilePositionIterator tilesEnd() const;

	/**
	 * Returns an iterator for the first object in the level, iterating over all objects in order of increasing ID.
	 */
	ObjectIterator objectsBegin() const;

	/**
	 * Returns an iterator for the one-past-last object in the level.
	 */
	ObjectIterator objectsEnd() const;

private:

	static constexpr unsigned int SECTION_SIZE = 16;

	struct Section
	{
		Section() :
				tileCount(0)
		{
		}

		std::array<Tile, SECTION_SIZE * SECTION_SIZE> tiles;
		std::vector<Object::ID> objects;
		std::size_t tileCount;
	};

	typedef std::map<sf::Vector2i, Section> SectionMap;

	void removeSectionIfEmpty(sf::Vector2i position);

	void onObjectMoved(Object::ID id, sf::Vector2i oldPosition);
	void onObjectUpdated(Object::ID id);

	void addObjectToSection(Object::ID id, sf::Vector2i position);
	void removeObjectFromSection(Object::ID id, sf::Vector2i position);

	void advanceTileIterator(TilePositionIterator & iterator) const;
	void advanceObjectIterator(ObjectIterator & iterator) const;

	static sf::Vector2i sectionToPosition(sf::Vector2i sectionPosition);
	static sf::Vector2i positionToSection(sf::Vector2i position);

	static std::size_t positionToTileIndex(sf::Vector2i position);
	static sf::Vector2i tileIndexToSubPosition(std::size_t index);

	SectionMap sections;
	std::vector<std::unique_ptr<Object> > objects;

	sf::Vector2i playerSpawn;
	int boss;
	int music;

	ObjectObserver objectObserver;
	mutable EventManager<Event> eventManager;

	friend class ObjectObserver;
	friend class Level::TilePositionIterator;
};

#endif
