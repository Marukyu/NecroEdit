#ifndef SRC_CLIENT_EDITOR_TOOLS_PANELS_SPAWNPOINTTOOLPANEL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_PANELS_SPAWNPOINTTOOLPANEL_HPP_

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Widgets/Checkbox.hpp>
#include <Client/GUI2/Widgets/Dropdown.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <vector>

class Dungeon;
namespace gui2
{
class WidgetEvents;
}

class SpawnPointToolPanel : public gui2::BorderPanel
{
public:

	static gui2::Ptr<SpawnPointToolPanel> make(Dungeon & dungeon, const ObjectAppearanceManager & objectAppearance);

	SpawnPointToolPanel(Dungeon & dungeon, const ObjectAppearanceManager & objectAppearance);
	virtual ~SpawnPointToolPanel();

	void update();
	void setLevel(Level * level);

protected:

	virtual void init() override;

private:

	void onProcessContainer(gui2::WidgetEvents & events) override;

	Dungeon * dungeon;
	Level * level;
	const ObjectAppearanceManager * objectAppearance;

	std::vector<int> playerCharacterIDs;

	gui2::Ptr<gui2::Dropdown> playerCharacterDropdown;
	gui2::Ptr<gui2::Checkbox> startItemsCheckbox;
};

#endif
