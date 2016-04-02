#ifndef SRC_CLIENT_EDITOR_TOOLS_PANELS_TILETOOLPANEL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_PANELS_TILETOOLPANEL_HPP_

#include <Client/Editor/SelectionPanel.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Widgets/Button.hpp>
#include <Client/GUI2/Widgets/Checkbox.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <Shared/Level/Tile.hpp>
#include <vector>

/**
 * Configuration panel for tile brush tools.
 */
class TileToolPanel : public gui2::BorderPanel
{
public:

	static gui2::Ptr<TileToolPanel> make(std::vector<Tile> tiles, const TileAppearanceManager & tileAppearance);

	TileToolPanel(std::vector<Tile> Tiles, const TileAppearanceManager & tileAppearance);
	virtual ~TileToolPanel();

	/**
	 * Returns a reference to the currently selected tile.
	 */
	const Tile & getSelectedTile() const;

protected:
	
	virtual void init() override;

private:

	void applyTileProperties(Tile & targetTile, const Tile & sourceTile);
	void updateTileProperties(std::vector<Tile> & tileList, const Tile & sourceTile);
	void updateSelectors();
	void updateSelectedTile();

	void onProcessContainer(gui2::WidgetEvents & events) override;

	const TileAppearanceManager * tileAppearance;

	std::vector<Tile> floors;
	std::vector<Tile> walls;
	Tile selectedTile;
	Tile baseTile;

	gui2::Ptr<SelectionPanel> floorPanel;
	gui2::Ptr<SelectionPanel> wallPanel;

	std::vector<gui2::Ptr<gui2::Button> > tileZoneButtons;
	gui2::Ptr<gui2::Checkbox> tileTorchCheckbox;
	gui2::Ptr<gui2::Checkbox> tileCrackCheckbox;
};

#endif
