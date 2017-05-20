/*
 * TileAppearance.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: marukyu
 */

#include <Client/LevelRenderer/TileAppearance.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <algorithm>
#include <memory>

TileAppearanceManager::TileAppearanceManager(ITexturePacker* packer) :
		myPacker(packer)
{
}

void TileAppearanceManager::clear()
{
	myTiles.clear();
}

bool TileAppearanceManager::loadTiles(const std::string & tileData, const std::string & basePath)
{
	// Remove any existing loaded tiles.
	clear();

	// Create loader object.
	AppearanceLoader loader;

	// Create callback function.
	AppearanceLoader::CallbackFunction callback = [this](AppearanceLoader::Appearance appearance)
	{
		// Forward function call to Manager object.
			onLoadTile(std::move(appearance));
		};

	// Perform load.
	return loader.loadAppearance(tileData, "Tiles", basePath + "level/", callback);
}

std::vector<sf::Vertex> TileAppearanceManager::getTileVertices(const Tile& tile,
		sf::Vector2i position) const
{
	// Get variant appearance.
	const TileVariantAppearance * variant = getTileVariant(tile);

	// Check if variant exists.
	if (variant == nullptr)
	{
		// Return empty vertices if variant does not exist.
		return {};
	}

	// Vertex vector.
	std::vector<sf::Vertex> vertices = getSingleTileVertices(*variant, position);

	// Perform multi-layer rendering checks.
	if (variant->baseTile != Tile::Invalid)
	{
		// The tile has a base tile.
		Tile baseTile = tile;

		// Create base tile according to original tile's variant info.
		baseTile.id = variant->baseTile;

		// Apply opacity to upper layer vertices.
		for (sf::Vertex & vertex : vertices)
		{
			vertex.color.a = variant->opacity;
		}

		// Get vertices for lower layer (base tile).
		std::vector<sf::Vertex> baseVertices = getTileVertices(baseTile, position);
		vertices.insert(vertices.begin(), baseVertices.begin(), baseVertices.end());
	}

	if (tile.hasTorch)
	{
		// The tile has a torch.
		Tile torchTile(Tile::Torch, Tile::Zone1, false);

		// Get vertices for upper layer (torch pseudo-tile).
		std::vector<sf::Vertex> torchVertices = getTileVertices(torchTile, position);
		vertices.insert(vertices.end(), torchVertices.begin(), torchVertices.end());
	}
	
	return vertices;
}

const TileAppearanceManager::TileVariantAppearance* TileAppearanceManager::getTileVariant(const Tile& tile) const
{
	// Get tile appearance information.
	auto it = myTiles.find(tile.id);

	// Check if appearance informaton was found.
	if (it == myTiles.end())
	{
		return nullptr;
	}

	// Get reference to tile appearance.
	const TileAppearance & appearance = it->second;

	// Check if appearance has any variants.
	if (appearance.variants.empty())
	{
		// Check if appearance is a duplicate.
		if (appearance.duplicateID != Tile::Invalid || appearance.duplicateID != tile.id)
		{
			return getTileVariant(Tile(appearance.duplicateID, tile.variant, tile.hasTorch));
		}

		// No variants or duplicate info? Return nothing.
		return nullptr;
	}

	// Get current variant.
	std::size_t varIndex = tile.variant;

	// Reduce tile variant if it is out of bounds.
	varIndex %= appearance.variants.size();

	// Return pointer to the variant.
	return &appearance.variants[varIndex];
}

std::vector<sf::Vertex> TileAppearanceManager::getSingleTileVertices(
		const TileVariantAppearance & variant, sf::Vector2i position) const
{
	// Create vertex and texture rectangles.
	sf::IntRect texRect = myPacker->getImageRect(variant.nodeID);
	sf::FloatRect vertRect(0.f, variant.yOffset, texRect.width, texRect.height);

	// Base vertex position: centered horizontally by image width, vertically by fixed tile size.
	sf::Vector2f vp(position.x * TILE_SIZE - texRect.width / 2.f, position.y * TILE_SIZE - TILE_SIZE / 2.f);

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
	
	// Contract texture corners slightly to remove visible gaps (floating point errors).
	static constexpr float textureDelta = 0.01f;
	ttl += sf::Vector2f( textureDelta,  textureDelta);
	ttr += sf::Vector2f(-textureDelta,  textureDelta);
	tbr += sf::Vector2f(-textureDelta, -textureDelta);
	tbl += sf::Vector2f( textureDelta, -textureDelta);

	// Add vertices to a vector.
	return {
		{ vp + tl, sf::Color::White, ttl },
		{ vp + tr, sf::Color::White, ttr },
		{ vp + bl, sf::Color::White, tbl },
		{ vp + tr, sf::Color::White, ttr },
		{ vp + bl, sf::Color::White, tbl },
		{ vp + br, sf::Color::White, tbr },
	};
}

std::vector<Tile::ID> TileAppearanceManager::getTileIDList() const
{
	std::vector<Tile::ID> tileList;
	
	for (auto it = myTiles.begin(); it != myTiles.end(); ++it)
	{
		tileList.push_back(it->first);
	}
	
	return tileList;
}

const std::string& TileAppearanceManager::getTileName(const Tile& tile) const
{
	auto it = myTiles.find(tile.id);
	if (it == myTiles.end())
	{
		it = myTiles.find(Tile::Builtin::Invalid);
		if (it == myTiles.end())
		{
			static std::string invalidName = "Invalid";
			return invalidName;
		}
	}
	return it->second.name;
}

const sf::Texture* TileAppearanceManager::getTexture() const
{
	return myPacker->getTexture();
}

TileAppearanceManager::TileVariantAppearance::TileVariantAppearance() :
		nodeID(ITexturePacker::packFailure),
		yOffset(0.f),
		baseTile(Tile::Invalid),
		opacity(255)
{
}

TileAppearanceManager::TileAppearance::TileAppearance() :
		name(),
		type(Wall),
		variants(),
		duplicateID(Tile::Invalid)
{
}

void TileAppearanceManager::onLoadTile(AppearanceLoader::Appearance appearance)
{
	// Create tile appearance entry (if necessary) and get a reference to it.
	TileAppearance & tile = myTiles[appearance.id];

	// Apply the tile name.
	tile.name = appearance.name;

	// Detect the tile type from the ID (0-99 are floors, 100+ are walls).
	tile.type = appearance.id < 100 ? TileAppearance::Floor : TileAppearance::Wall;

	// Check if the current tile is a duplicate.
	if (appearance.isDuplicate)
	{
		// Mark tile as duplicate.
		tile.duplicateID = appearance.duplicateID;
	}
	else
	{
		// Mark tile as non-duplicate.
		tile.duplicateID = Tile::Invalid;

		// Increase tile variant array size to hold current variant.
		tile.variants.resize(std::max<unsigned int>(tile.variants.size(), appearance.variant + 1));

		// Get current tile variant.
		TileVariantAppearance & variant = tile.variants[appearance.variant];

		// Assign default vertical offset based on tile type.
		variant.yOffset = tile.type == TileAppearance::Floor ? 0.f : -15.f;

		// Assign no base tile as default.
		variant.baseTile = Tile::Invalid;

		// Assign fully opaque color as default.
		variant.opacity = 255;

		// Check if tile variant already had clipping information or needs to be clipped manually.
		if (appearance.isClipped)
		{
			// Directly use loaded image and pack it into main texture.
			variant.nodeID = myPacker->addOwn(std::move(appearance.image));

			// Check if first extra parameter exists.
			if (appearance.custom.size() >= 1)
			{
				// Get yOffset from first extra parameter.
				variant.yOffset = cStoF(appearance.custom[0]);
			}

			// Check if second extra parameter exists.
			if (appearance.custom.size() >= 2)
			{
				// Get base tile from second extra parameter.
				variant.baseTile = cStoI(appearance.custom[1]);
			}

			// Check if third extra parameter exists.
			if (appearance.custom.size() >= 3)
			{
				// Get tile opacity from third extra parameter.
				variant.opacity = cStoUB(appearance.custom[2]);
			}
		}
		else
		{
			// Create cropping sub-rectangle.
			sf::IntRect tileRect;

			// No cropping information specified: get defaults by type.
			if (tile.type == TileAppearance::Floor)
			{
				// Most floor images are 26x26.
				// Ignore the border pixels and take the 24x24 center.
				tileRect = sf::IntRect(1, 1, TILE_SIZE, TILE_SIZE);
			}
			else if (tile.type == TileAppearance::Wall)
			{
				// Most wall images are 24x48 (or multiples thereof).
				// A vertical offset is specified to ensure the correct rendering of walls.
				tileRect = sf::IntRect(0, 0, TILE_SIZE, 2.f * TILE_SIZE);
			}

			// Check if image size is sufficient for clipping rectangle.
			if (tileRect.left + tileRect.width > (int) appearance.image->getSize().x
					|| tileRect.top + tileRect.height > (int) appearance.image->getSize().y)
			{
				return;
			}

			// Create and crop image.
			std::unique_ptr<sf::Image> clippedTileImage = makeUnique<sf::Image>();
			clippedTileImage->create(tileRect.width, tileRect.height);
			clippedTileImage->copy(*appearance.image, 0, 0, tileRect);

			// Use clipped image and pack it into main texture.
			variant.nodeID = myPacker->addOwn(std::move(clippedTileImage));
		}
	}
}
