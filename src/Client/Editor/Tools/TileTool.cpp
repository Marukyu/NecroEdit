#include <Client/Editor/Tools/TileTool.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <Shared/Level/Tile.hpp>
#include <memory>
#include <string>
#include <vector>

TileTool::TileTool()
{
	primaryBrush.setTileMode(Brush::TileMode::ReplaceAny);
	primaryBrush.setTileMask(Brush::TileMask::Full);
	secondaryBrush.setTileMode(Brush::TileMode::Erase);
}

TileTool::~TileTool()
{
}

void TileTool::onInit()
{
	std::vector<Tile> tiles;
	std::vector<Tile::ID> tileIDs = getEditorData().tileAppearance->getTileIDList();

	for (Tile::ID tileID : tileIDs)
	{
		Tile tile;
		tile.id = tileID;

		const std::string & tileName = getEditorData().tileAppearance->getTileName(tile);

		if (!tile.exists() || tileName.empty() || tileName[0] == '!')
		{
			continue;
		}

		tiles.push_back(tile);
	}

	settingsPanel = TileToolPanel::make(tiles, *getEditorData().tileAppearance);
}

gui2::Ptr<gui2::Widget> TileTool::getSettingsPanel() const
{
	return settingsPanel;
}

std::string TileTool::getTooltip() const
{
	return settingsPanel->getTooltip();
}

const Brush& TileTool::getPrimaryBrush() const
{
	primaryBrush.setTile(settingsPanel->getSelectedTile());
	return primaryBrush;
}

const Brush& TileTool::getSecondaryBrush() const
{
	return secondaryBrush;
}
