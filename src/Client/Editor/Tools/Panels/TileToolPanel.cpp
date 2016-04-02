#include <Client/Editor/TileMapperFactory.hpp>
#include <Client/Editor/Tools/Panels/TileToolPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>

namespace gui2
{
class WidgetEvents;
}

gui2::Ptr<TileToolPanel> TileToolPanel::make(std::vector<Tile> tiles, const TileAppearanceManager& tileAppearance)
{
	gui2::Ptr<TileToolPanel> widget = std::make_shared<TileToolPanel>(tiles, tileAppearance);
	widget->init();
	return widget;
}

TileToolPanel::TileToolPanel(std::vector<Tile> tiles, const TileAppearanceManager& tileAppearance) :
		tileAppearance(&tileAppearance)
{
	for (Tile & tile : tiles)
	{
		if (tile.isWall())
		{
			walls.push_back(std::move(tile));
		}
		else
		{
			floors.push_back(std::move(tile));
		}
	}
}

TileToolPanel::~TileToolPanel()
{
}

const Tile& TileToolPanel::getSelectedTile() const
{
	return selectedTile;
}

void TileToolPanel::init()
{
	gui2::BorderPanel::init();

	gui2::Ptr<gui2::GridPanel> selectorContainer = gui2::GridPanel::make(1, 2);

	floorPanel = SelectionPanel::make();
	floorPanel->setTexture(tileAppearance->getTexture());
	floorPanel->setItemSize({24, 24});
	selectorContainer->add(floorPanel);
	wallPanel = SelectionPanel::make();
	wallPanel->setTexture(tileAppearance->getTexture());
	wallPanel->setItemSize({24, 48});
	selectorContainer->add(wallPanel);

	std::vector<std::string> zoneNames = { "Zone 1", "Zone 2", "Zone 3 Hot", "Zone 3 Cold", "Zone 4", "Boss" };

	auto zoneButtonContainer = gui2::GridPanel::make(2, (zoneNames.size() + 1) / 2);
	zoneButtonContainer->setMargin(1.f);

	for (std::size_t zone = 0; zone < zoneNames.size(); ++zone)
	{
		auto zoneButton = gui2::Button::make(zoneNames[zone]);
		zoneButton->setActivated((int) zone == baseTile.getZone());
		tileZoneButtons.push_back(zoneButton);
		zoneButtonContainer->add(zoneButton);
	}

	zoneButtonContainer->setGridSize(2, (tileZoneButtons.size() + 1) / 2);

	auto checkboxContainer = gui2::GridPanel::make(2, 1);
	checkboxContainer->setPadding(1.f);
	checkboxContainer->setMargin(1.f);

	tileTorchCheckbox = gui2::Checkbox::make();
	tileTorchCheckbox->setText("Torch");
	checkboxContainer->add(tileTorchCheckbox);

	tileCrackCheckbox = gui2::Checkbox::make();
	tileCrackCheckbox->setText("Cracked");
	checkboxContainer->add(tileCrackCheckbox);

	add(selectorContainer, gui2::BorderPanel::Center);
	add(zoneButtonContainer, gui2::BorderPanel::Bottom, 90.f);
	add(checkboxContainer, gui2::BorderPanel::Bottom, 30.f);

	updateSelectors();
}

void TileToolPanel::applyTileProperties(Tile & tile, const Tile & sourceTile)
{
	tile.setZone(sourceTile.getZone());
	tile.setCracked(sourceTile.isCracked());
	tile.hasTorch = sourceTile.hasTorch;
}

void TileToolPanel::updateTileProperties(std::vector<Tile>& tileList, const Tile & sourceTile)
{
	for (Tile & tile : tileList)
	{
		applyTileProperties(tile, sourceTile);
	}
}

void TileToolPanel::updateSelectors()
{
	// Disable torch for floor preview.
	Tile baseFloorTile = baseTile;
	baseFloorTile.hasTorch = false;

	updateTileProperties(floors, baseFloorTile);
	updateTileProperties(walls, baseTile);

	TileMapperFactory mapperFactory(*this->tileAppearance);

	floorPanel->setMapper(mapperFactory.generateTileMapper(floors), floors.size());
	wallPanel->setMapper(mapperFactory.generateTileMapper(walls), walls.size());

	updateSelectedTile();
}

void TileToolPanel::updateSelectedTile()
{
	if (floorPanel->hasSelection())
	{
		selectedTile = floors[floorPanel->getSelection()];
	}
	else if (wallPanel->hasSelection())
	{
		selectedTile = walls[wallPanel->getSelection()];
	}
	else
	{
		selectedTile = Tile();
	}
}

void TileToolPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	if (floorPanel->wasChanged())
	{
		wallPanel->clearSelection();
		updateSelectedTile();
	}

	if (wallPanel->wasChanged())
	{
		floorPanel->clearSelection();
		updateSelectedTile();
	}

	// Loop over zone buttons for click check.
	for (std::size_t checkedZone = 0; checkedZone < tileZoneButtons.size(); ++checkedZone)
	{
		// Check if zone button was clicked.
		if (tileZoneButtons[checkedZone]->isClicked())
		{
			// Mark clicked zone button as activated and deactivate all others.
			for (std::size_t innerZone = 0; innerZone < tileZoneButtons.size(); ++innerZone)
			{
				tileZoneButtons[innerZone]->setActivated(checkedZone == innerZone);
			}

			// Assign zone to base tile.
			baseTile.setZone(checkedZone);
			updateSelectors();
		}
	}

	if (tileTorchCheckbox->isClicked())
	{
		baseTile.hasTorch = tileTorchCheckbox->isChecked();
		updateSelectors();
	}

	if (tileCrackCheckbox->isClicked())
	{
		baseTile.setCracked(tileCrackCheckbox->isChecked());
		updateSelectors();
	}
}
