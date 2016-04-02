#ifndef SRC_CLIENT_EDITOR_TILEMAPPERFACTORY_HPP_
#define SRC_CLIENT_EDITOR_TILEMAPPERFACTORY_HPP_

#include <Client/Editor/SelectionPanel.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <Shared/Level/Tile.hpp>
#include <vector>

/**
 * Creates tile mappers for selector widgets.
 * 
 * Mappers are lists of vertex representations of tiles that can be placed inside a selector widget, allowing the user
 * to select items from a visual grid.
 */
class TileMapperFactory
{
public:
	TileMapperFactory(const TileAppearanceManager & tileAppearance);
	~TileMapperFactory();
	
	SelectionPanel::Mapper generateTileMapper(const std::vector<Tile> & tiles) const;
	
private:
	
	SelectionPanel::Mapper generateMapper(std::vector<SelectionPanel::ItemEntry> entries) const;
	
	const TileAppearanceManager * tileAppearance;
};

#endif
