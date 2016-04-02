/*
 * TileAppearance.hpp
 *
 *  Created on: Oct 19, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_LEVELRENDERER_TILEAPPEARANCE_HPP_
#define SRC_CLIENT_LEVELRENDERER_TILEAPPEARANCE_HPP_

#include <Client/Graphics/Packing/ITexturePacker.hpp>
#include <Client/LevelRenderer/AppearanceLoader.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Level/Tile.hpp>
#include <array>
#include <cstddef>
#include <map>
#include <string>
#include <vector>

class TileAppearanceManager
{
public:

	static constexpr float TILE_SIZE = 24.f;

	/**
	 * Number of vertices per tile.
	 * 
	 * Sufficient to hold 2 rectangles layered on top of each other (for layered rendering, such as torches or doors).
	 */
	static constexpr unsigned int VERTEX_COUNT = 12;

	TileAppearanceManager(ITexturePacker * packer);

	/**
	 * Unloads all tiles from the appearance manager.
	 * 
	 * Does NOT clear the texture packer, this has to be done externally.
	 */
	void clear();

	/**
	 * Loads tile appearance information from memory. The tileData string has to be in a comma-separated format,
	 * see editor.txt for an example.
	 * 
	 * basePath points to the NecroDancer data directory and is expected to end with a slash.
	 * 
	 * Does NOT clear the texture packer and will duplicate existing images; clear the packer before calling this
	 * function!
	 */
	bool loadTiles(const std::string & tileData, const std::string & basePath);

	/**
	 * Returns the vertices representing the given tile at the specified position.
	 */
	std::array<sf::Vertex, VERTEX_COUNT> getTileVertices(const Tile & tile, sf::Vector2i position) const;
	
	/**
	 * Returns the number of vertices a given tile will actually use out of the fixed-size vertex array.
	 */
	std::size_t getTileActualVertexCount(const Tile & tile) const;
	
	/**
	 * Returns a list of all valid tile IDs.
	 */
	std::vector<Tile::ID> getTileIDList() const;

	/**
	 * Returns the name of the specified tile.
	 */
	const std::string & getTileName(const Tile & tile) const;
	
	/**
	 * Returns the number of distinct variants of the specified tile.
	 */
	std::size_t getTileVariantCount(const Tile & tile) const;

	/**
	 * Returns the texture of the texture packer used for loading the tiles into this appearance manager.
	 */
	const sf::Texture * getTexture() const;

private:

	/**
	 * Number of vertices per single tile.
	 * 
	 * Does not take into account layering.
	 */
	static constexpr unsigned int VERTEX_COUNT_SINGLE = 6;

	struct TileVariantAppearance
	{
	public:

		TileVariantAppearance();

		ITexturePacker::NodeID nodeID;
		float yOffset;
		Tile::ID baseTile;
		sf::Uint8 opacity;
	};

	struct TileAppearance
	{
		enum Type
		{
			Wall,
			Floor
		};

		TileAppearance();

		std::string name;
		Type type;
		std::vector<TileVariantAppearance> variants;
		Tile::ID duplicateID;
	};
	
	const TileVariantAppearance * getTileVariant(const Tile & tile) const;

	std::array<sf::Vertex, VERTEX_COUNT_SINGLE> getSingleTileVertices(const TileVariantAppearance & variant,
			sf::Vector2i position) const;

	void onLoadTile(AppearanceLoader::Appearance appearance);

	ITexturePacker * myPacker;

	std::map<Tile::ID, TileAppearance> myTiles;
};

#endif
