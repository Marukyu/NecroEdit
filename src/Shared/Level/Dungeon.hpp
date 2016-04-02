#ifndef SRC_SHARED_LEVEL_DUNGEON_HPP_
#define SRC_SHARED_LEVEL_DUNGEON_HPP_

#include <Shared/Level/Object.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace pugi
{
class xml_node;
}

class Level;

/**
 * A dungeon is an ordered collection of levels and boss levels.
 * 
 * It can be saved to and loaded from a NecroDancer-compatible XML file.
 */
class Dungeon
{
public:
	Dungeon();
	~Dungeon();

	/**
	 * Inserts a new level in front of the specified position. The inserted level will have the specified index. Shifts
	 * all subsequent levels forward by one index.
	 * 
	 * Does nothing if index is out of range.
	 */
	void insertLevel(std::size_t index);

	/**
	 * Removes the specified level. Shifts all subsequent levels backward by one index.
	 * 
	 * Does nothing if index is out of range.
	 */
	void removeLevel(std::size_t index);

	/**
	 * Swaps the levels at the two specified indices without altering the levels in-between.
	 * 
	 * Does nothing if either argument is out of range.
	 * 
	 * All pointers remain valid.
	 */
	void swapLevels(std::size_t indexA, std::size_t indexB);

	/**
	 * Returns the number of levels in the dungeon.
	 */
	std::size_t getLevelCount() const;

	/**
	 * Returns the level at the specified index.
	 * 
	 * Throws std::out_of_range if the index is out of range.
	 */
	Level & getLevel(std::size_t index) const;

	/**
	 * Sets which player character is assigned to this dungeon.
	 */
	void setPlayerCharacter(int character);

	/**
	 * Returns the player character assigned to this dungeon.
	 */
	int getPlayerCharacter() const;

	/**
	 * Sets whether the player will spawn with the character-specific default items.
	 */
	void setStartItemsEnabled(bool enabled);

	/**
	 * Returns whether the player will spawn with the character-specific default items.
	 */
	bool isStartItemsEnabled() const;

	/**
	 * Reads the XML file with the specified file name and loads the dungeon from it.
	 * 
	 * If an error occurs (failed to open file or incorrect file content), returns false and leaves dungeon unchanged.
	 * Otherwise, returns true.
	 */
	bool loadFromXML(const std::string & filename);

	/**
	 * Writes the XML file with the specified file name and saves the dungeon to it.
	 * 
	 * If an error occurs (failed to open file), returns false.
	 */
	bool saveToXML(const std::string & filename) const;

private:

	/**
	 * Loads all tiles from the specified XML <level> node.
	 */
	void loadTiles(const pugi::xml_node & levelNode, std::size_t levelNumber);

	/**
	 * Loads a certain type of object from the specified XML <level> node.
	 */
	void loadObjects(const pugi::xml_node & levelNode, std::size_t levelNumber, Object::Type objectType);

	/**
	 * Saves all tiles to the specified XML <level> node.
	 */
	void saveTiles(pugi::xml_node & levelNode, std::size_t levelNumber) const;

	/**
	 * Saves all objects to the specified XML <level> node.
	 */
	void saveObjects(pugi::xml_node & levelNode, std::size_t levelNumber) const;

	std::vector<std::unique_ptr<Level>> levels;

	std::string name;
	int playerCharacter;
	bool startItemsEnabled;
};

#endif
