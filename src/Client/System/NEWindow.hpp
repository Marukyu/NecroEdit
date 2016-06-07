#ifndef NE_WINDOW_HPP
#define NE_WINDOW_HPP

#include <Client/Editor/Editor.hpp>
#include <Client/Editor/LevelPanel.hpp>
#include <Client/Editor/Tool.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <Client/GUI2/Widgets/Button.hpp>
#include <Client/GUI2/Widgets/MessageBox.hpp>
#include <Shared/Level/Tile.hpp>
#include <Shared/Utils/FileChooser.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Brush;
class Tool;

class NEApplication;
class TileMapperFactory;
class ObjectAppearanceManager;
class SortingTexturePacker;
class TileAppearanceManager;
class Dungeon;

class NEWindow : public gui2::Window
{
public:

	static gui2::Ptr<NEWindow> make();

	void loadEditor();

	void setViewMultiplier(float viewMultiplier);
	float getViewMultiplier() const;

	void setToolByName(const std::string & toolName);
	Tool * getTool() const;

private:

	struct ToolbarButton
	{
		gui2::Ptr<gui2::Button> button;
		std::string actionName;
	};

	void initWindow();
	void initResources();
	void initTools();
	void initPanels();
	void initLevelPanel();
	void initToolbar();
	void initFileDialogs();
	void initEditor();
	void initLevel();

	void initToolbarButtons(std::string configSection, std::vector<ToolbarButton> & targetVector,
		gui2::Ptr<gui2::BorderPanel> targetPanel);

	std::map<int, std::string> generateEnumMap(const std::string & section) const;

	void updateTilePanels();

	void onProcessWindow(const gui2::WidgetEvents & events) override;

	void switchToLevel(Level * level);

	void setCurrentDungeon(std::string currentDungeon);
	const std::string & getCurrentDungeon() const;

	void newDungeon();
	void openDungeon();
	void saveDungeon();
	void saveDungeonAs();

	NEApplication * getParentApplication() const;

	sf::Transform getContainerTransform() const override;

	std::string gameDirectory;
	std::string gameDataDirectory;
	std::string editorConfig;

	float viewMultiplier;

	std::map<std::string, std::unique_ptr<Tool> > tools;
	Tool::EditorData editorData;

	gui2::Ptr<gui2::BorderPanel> mainPanel;

	std::vector<ToolbarButton> actionButtons;
	std::vector<ToolbarButton> toolButtons;

	gui2::Ptr<gui2::GridPanel> toolPanelContainer;

	gui2::Ptr<LevelPanel> levelPanel;

	gui2::Ptr<Editor> editor;

	std::vector<Tile> floors, walls;

	std::unique_ptr<SortingTexturePacker> texturePacker;
	std::unique_ptr<TileAppearanceManager> tileAppearance;
	std::unique_ptr<ObjectAppearanceManager> objectAppearance;
	std::unique_ptr<Dungeon> dungeon;

	FileChooser openDialog;
	FileChooser saveDialog;

	std::string currentDungeonFile;

	gui2::Ptr<gui2::MessageBox> newConfirmMessage;
	gui2::Ptr<gui2::MessageBox> errorMessage;
};

#endif
