#include <Client/Editor/TileMapperFactory.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <string>

TileMapperFactory::TileMapperFactory(const TileAppearanceManager & tileAppearance) :
		tileAppearance(&tileAppearance)
{
}

TileMapperFactory::~TileMapperFactory()
{
}

SelectionPanel::Mapper TileMapperFactory::generateTileMapper(const std::vector<Tile>& tiles) const
{
	std::vector<SelectionPanel::ItemEntry> entries(tiles.size());

	for (std::size_t i = 0; i < tiles.size(); ++i)
	{
		auto array = tileAppearance->getTileVertices(tiles[i], sf::Vector2i(0, 0));
		entries[i].vertices.assign(array.begin(), array.end());
		entries[i].name = tileAppearance->getTileName(tiles[i]);
	}
	
	return generateMapper(entries);
}

static SelectionPanel::ItemEntry fallbackEntry = SelectionPanel::ItemEntry();

SelectionPanel::Mapper TileMapperFactory::generateMapper(std::vector<SelectionPanel::ItemEntry> entries) const
{
	return [entries](SelectionPanel::ID id) -> const SelectionPanel::ItemEntry &
	{
		if (id < entries.size())
		{
			return entries[id];
		}
		
		return fallbackEntry;
	};
}
