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
	std::vector<sf::Vertex> getTileVertices(const Tile & tile, sf::Vector2i position) const;
	
	/**
	 * Returns a list of all valid tile IDs.
	 */
	std::vector<Tile::ID> getTileIDList() const;

	/**
	 * Returns the name of the specified tile.
	 */
	const std::string & getTileName(const Tile & tile) const;
	
	/**
	 * Returns the texture of the texture packer used for loading the tiles into this appearance manager.
	 */
	const sf::Texture * getTexture() const;

private:

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

	std::vector<sf::Vertex> getSingleTileVertices(const TileVariantAppearance & variant,
			sf::Vector2i position) const;

	void onLoadTile(AppearanceLoader::Appearance appearance);

	ITexturePacker * myPacker;

	std::map<Tile::ID, TileAppearance> myTiles;
};

#endif
