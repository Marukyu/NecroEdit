#ifndef SRC_CLIENT_LEVELRENDERER_LEVELRENDERER_HPP_
#define SRC_CLIENT_LEVELRENDERER_LEVELRENDERER_HPP_

#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Level/Object.hpp>
#include <Shared/Level/Tile.hpp>
#include <Shared/Utils/Event/EventListener.hpp>
#include <array>
#include <cstddef>
#include <vector>

class Dungeon;

namespace sf
{
class RenderTarget;
}

/**
 *
 */
class LevelRenderer : public sf::Drawable
{
public:
	/**
	 * Creates a renderer for the specified level using the provided appearance managers.
	 */
	LevelRenderer(const Dungeon & dungeon, const Level & level, const TileAppearanceManager & tileAppearance,
			const ObjectAppearanceManager & objectAppearance);

	virtual ~LevelRenderer();

	/**
	 * Redraws all tiles and objects that have been changed since the last call to update().
	 */
	void update();

private:

	/**
	 * Container type for tile vertices.
	 */
	typedef std::vector<sf::Vertex> TileVertexArray;

	/**
	 * Enumeration for discrete tile Z-positions/layers.
	 * 
	 * This is used to correctly draw floors below walls while still maintaining a top-down Z-order for overlapping
	 * wall tiles.
	 */
	enum TileLayer
	{
		AllLayers = -2,
		LayerNone = -1,

		Floor = 0,
		Wall,

		LayerCount
	};

	/**
	 * Override for SFML drawable.
	 */
	void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

	/**
	 * Draws a single tile layer.
	 */
	void drawTileLayer(TileLayer layer, sf::RenderTarget & target, sf::RenderStates states) const;

	/**
	 * Sets the tile vertices at the specified position in an automatically selected layer, creating the entry if it
	 * does not exist yet.
	 */
	void addOrSetTile(sf::Vector2i position, const Tile & tile);

	/**
	 * Removes the tile vertices at the specified position within the specified layer.
	 */
	void removeTile(sf::Vector2i position, TileLayer layer = AllLayers);

	/**
	 * Returns the layer of the specified tile.
	 * 
	 * If the tile does not exist, returns the "None" layer.
	 */
	TileLayer getTileLayer(sf::Vector2i position) const;

	/**
	 * Adds a tile entry with the specified position to the specified layer and returns the vertices.
	 */
	std::size_t addTileEntry(sf::Vector2i position, TileLayer layer);
	void setTileVertices(sf::Vector2i position, TileVertexArray vertices, TileLayer layer);

	/**
	 * Updates the specified object's vertex array entry with its appearance data, creating the entry if it does not
	 * exist yet.
	 */
	void addOrSetObject(const Object & object);

	/**
	 * Removes the specified object's vertex array entry.
	 */
	void removeObject(Object::ID objectID);

	void setObjectVertexCount(Object::ID objectID, std::size_t size);
	std::size_t getObjectVertexIndex(Object::ID objectID);
	std::size_t getObjectVertexCount(Object::ID objectID);

	void expandObjectVertexArray(Object::ID objectID);
	void shrinkObjectVertexArray();

	void updateSpawnPoint();

	const Level * level;
	const Dungeon * dungeon;
	const TileAppearanceManager * tileAppearance;
	const ObjectAppearanceManager * objectAppearance;

	std::array<std::vector<sf::Vector2i>, LayerCount> tilePositions;
	std::array<std::vector<TileVertexArray>, LayerCount> tileVertices;

	std::vector<std::size_t> objectVertexCounts;
	std::vector<sf::Vertex> objectVertices;

	Object spawnPointVisualizer;
	std::vector<sf::Vertex> spawnPointVertices;

	EventListener<Level::Event> eventListener;
};

#endif
