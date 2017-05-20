#include <Client/Graphics/Packing/SortingTexturePacker.hpp>
#include <Client/Graphics/Packing/TexturePacker.hpp>
#include <Client/GUI2/Application.hpp>
#include <Client/GUI2/Container.hpp>
#include <Client/GUI2/Interface.hpp>
#include <Client/GUI2/Widgets/Gradient.hpp>
#include <Client/GUI2/Widgets/Separator.hpp>
#include <Client/GUI2/Widgets/Window.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <Client/System/NEApplication.hpp>
#include <Client/System/NEWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <Shared/Utils/NamedFactory.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <cstdbool>
#include <cstddef>
#include <utility>

constexpr float TOOLBAR_BUTTON_SIZE = 32.f;
constexpr float TOOLBAR_MARGIN = 1.f;

gui2::Ptr<NEWindow> NEWindow::make()
{
	gui2::Ptr<NEWindow> widget = std::make_shared<NEWindow>();
	widget->init();
	widget->viewMultiplier = 2.f;
	return widget;
}

void NEWindow::loadEditor()
{
	initWindow();
	initResources();
	initTools();
	initPanels();
	initLevelPanel();
	initToolbar();
	initFileDialogs();
	initEditor();
	initLevel();

	updateTilePanels();
}

void NEWindow::setViewMultiplier(float viewMultiplier)
{
	if (this->viewMultiplier != viewMultiplier)
	{
		this->viewMultiplier = viewMultiplier;
		notify(NotifyContainerTransform);
	}
}

float NEWindow::getViewMultiplier() const
{
	return viewMultiplier;
}

void NEWindow::setToolByName(const std::string& toolName)
{
	if (tools.find(toolName) == tools.end())
	{
		std::unique_ptr<Tool> tool = Tool::createInstance(toolName);

		if (tool != nullptr)
		{
			tool->setEditorData(editorData);
			tool->onInit();

			if (tool->getSettingsPanel())
			{
				toolPanelContainer->add(tool->getSettingsPanel());
			}
		}

		tools.insert(std::make_pair(toolName, std::move(tool)));
	}

	if (getTool() != nullptr)
	{
		getTool()->onDisable();

		if (getTool()->getSettingsPanel() != nullptr)
		{
			getTool()->getSettingsPanel()->setVisible(false);
		}
	}

	editor->setTool(tools[toolName].get());

	if (getTool() != nullptr)
	{
		if (getTool()->getSettingsPanel() != nullptr)
		{
			getTool()->getSettingsPanel()->setVisible(true);
		}

		getTool()->setEditorData(editorData);
		getTool()->onEnable();
	}
}

Tool * NEWindow::getTool() const
{
	return editor->getTool();
}

void NEWindow::requestClose()
{
	closeConfirmMessage->show();
}

void NEWindow::initWindow()
{
	setCaption("NecroEdit");
	setMaximized(true);
	setFlags(gui2::Window::FlagMinimal);
}

void NEWindow::initResources()
{
	std::string workingDirectory = getParentApplication()->getResourceDirectory();
	gameDirectory = getParentApplication()->getGameDirectory();
	gameDataDirectory = gameDirectory + "/data/";

	texturePacker = makeUnique<SortingTexturePacker>(makeUnique<TexturePacker>(sf::Vector2u(1024, 512)));
	tileAppearance = makeUnique<TileAppearanceManager>(texturePacker.get());
	objectAppearance = makeUnique<ObjectAppearanceManager>(texturePacker.get());
	dungeon = makeUnique<Dungeon>();

	editorConfig = readFile(workingDirectory + "necroedit.res");

	tileAppearance->loadTiles(editorConfig, gameDataDirectory);
	objectAppearance->loadXMLObjects(readFile(gameDataDirectory + "necrodancer.xml"), gameDataDirectory);
	objectAppearance->loadTXTObjects(editorConfig, gameDataDirectory);

	texturePacker->pack();

	editorData.tileAppearance = tileAppearance.get();
	editorData.objectAppearance = objectAppearance.get();
	editorData.dungeon = dungeon.get();
}

void NEWindow::initTools()
{
	Tool::initializeToolFactory();
}

void NEWindow::initPanels()
{
	mainPanel = gui2::BorderPanel::make();

	mainPanel->setResizable(gui2::BorderPanel::Left, true);
	mainPanel->setMinimumSize(gui2::BorderPanel::Left, 45.f);
	mainPanel->setMaximumSize(gui2::BorderPanel::Left, 600.f);

	mainPanel->setResizable(gui2::BorderPanel::Right, true);
	mainPanel->setMinimumSize(gui2::BorderPanel::Right, 45.f);
	mainPanel->setMaximumSize(gui2::BorderPanel::Right, 200.f);

	auto mainFiller = gui2::GridPanel::make();
	mainFiller->add(gui2::Gradient::make(sf::Color(0, 0, 0), sf::Color(0, 0, 0)));
	mainFiller->add(mainPanel);
	add(mainFiller);

	toolPanelContainer = gui2::GridPanel::make();
	toolPanelContainer->add(gui2::Gradient::make(sf::Color(24, 24, 24, 200), sf::Color(32, 32, 32, 200)));
	mainPanel->add(toolPanelContainer, gui2::BorderPanel::Left, 200);

	levelPanel = LevelPanel::make();
	levelPanel->setMessageBoxTarget(this);
	mainPanel->add(levelPanel, gui2::BorderPanel::Right, 150);

	tooltip = gui2::Text::make();
	tooltip->setZPosition(10);
	tooltip->setTextColor(sf::Color(255, 255, 255, 220));
	add(tooltip);

	tooltipBackground = gui2::Gradient::make();
	tooltipBackground->setColorA(sf::Color(0, 0, 0, 150));
	tooltipBackground->setColorB(sf::Color(0, 0, 0, 150));
	tooltipBackground->setZPosition(9);
	tooltipBackground->setEnabled(false);
	add(tooltipBackground);
}

void NEWindow::initLevelPanel()
{
	levelPanel->setDungeon(dungeon.get());
	levelPanel->setSongs(generateEnumMap("Music"));
	levelPanel->setBosses(generateEnumMap("Bosses"));
}

void NEWindow::initToolbar()
{
	auto toolbarFiller = gui2::GridPanel::make();
	auto toolbar = gui2::BorderPanel::make();
	toolbar->setPadding(TOOLBAR_MARGIN);

	toolbarFiller->add(gui2::Gradient::make(sf::Color(32, 32, 32, 200), sf::Color(24, 24, 24, 200)));
	toolbarFiller->add(toolbar);
	mainPanel->add(toolbarFiller, gui2::BorderPanel::Top, TOOLBAR_BUTTON_SIZE + 2.f * TOOLBAR_MARGIN);

	initToolbarButtons("Actions", actionButtons, toolbar);

	auto separator = gui2::Separator::make();
	separator->setVisible(false);
	toolbar->add(separator, gui2::BorderPanel::Left, TOOLBAR_MARGIN * 4.f);

	initToolbarButtons("Tools", toolButtons, toolbar);
}

void NEWindow::initToolbarButtons(std::string configSection, std::vector<ToolbarButton>& targetVector,
	gui2::Ptr<gui2::BorderPanel> targetPanel)
{
	std::vector<std::string> actions = extractSection(splitString(editorConfig, "\n"), configSection);

	for (std::size_t i = 0; i < actions.size(); ++i)
	{
		std::vector<std::string> actionData = splitString(actions[i], ",");

		if (actionData.size() < 3)
		{
			continue;
		}

		ToolbarButton entry;
		entry.actionName = actionData[0];
		entry.button = gui2::Button::make(std::string(1, cStoB(actionData[1])));
		entry.button->setTextFont(gui2::Application::FontSymbolLarge);
		targetVector.push_back(entry);
		targetPanel->add(entry.button, gui2::BorderPanel::Left, TOOLBAR_BUTTON_SIZE);
	}
}

// TODO: Parse zone names from editor config and pass them to Tile Tool somehow:
// std::vector<std::string> zoneNames = extractSection(splitString(editorConfig, "\n"), "Zones");

void NEWindow::initFileDialogs()
{
#ifdef WOS_OSX
	// On OSX, ressources and dungeons aren't in the same folder, use relative path to get to the dungeons
	std::string dungeonsFolder = gameDirectory + "/../../../dungeons/";
#else
	std::string dungeonsFolder = gameDirectory + "/dungeons/";
#endif

	// Create dungeons folder if it does not exist yet
	createDirectoryStructure(dungeonsFolder);

	openDialog.setTitle("Open dungeon file...");
	openDialog.setDefaultPath(dungeonsFolder);
	openDialog.setFilterPatterns( { "*.xml" });
	openDialog.setFilterDescription("NecroDancer Dungeon XML");

	saveDialog.setTitle("Save dungeon file...");
	saveDialog.setDefaultPath(dungeonsFolder);
	saveDialog.setFilterPatterns( { "*.xml" });
	saveDialog.setFilterDescription("NecroDancer Dungeon XML");

	newConfirmMessage = gui2::MessageBox::make("Are you sure you want to create a new dungeon?\n"
		"All unsaved changes will be lost.", "Confirmation", { "Yes", "No" });

	closeConfirmMessage = gui2::MessageBox::make("Are you sure you want to quit NecroEdit?\n"
		"All unsaved changes will be lost.", "Confirmation", { "Yes", "No" });

	errorMessage = gui2::MessageBox::make("", "NecroEdit", { "OK" });

	add(newConfirmMessage);
	add(closeConfirmMessage);
	add(errorMessage);
}

void NEWindow::initEditor()
{
	editor = Editor::make(*dungeon, *tileAppearance, *objectAppearance);
	mainPanel->add(editor, gui2::BorderPanel::Center);
}

void NEWindow::initLevel()
{
	dungeon->setPlayerCharacter(0);
	dungeon->setStartItemsEnabled(true);
	dungeon->insertLevel(0);
	levelPanel->updateDungeon();
	levelPanel->setSelectedLevel(0);
	switchToLevel(&dungeon->getLevel(0));
}

std::map<int, std::string> NEWindow::generateEnumMap(const std::string& configSection) const
{
	std::vector<std::string> enumList = extractSection(splitString(editorConfig, "\n"), configSection);
	std::map<int, std::string> enumMap;

	for (const auto & entry : enumList)
	{
		std::vector<std::string> entryData = splitString(entry, ",");

		if (entryData.size() < 2)
		{
			continue;
		}

		enumMap[cStoI(entryData[0])] = entryData[1];
	}

	return std::move(enumMap);
}

void NEWindow::updateTilePanels()
{
	floors.clear();
	walls.clear();
	std::vector<Tile::ID> tileIDs = tileAppearance->getTileIDList();

	for (Tile::ID tileID : tileIDs)
	{
		Tile tile;
		tile.id = tileID;

		const std::string & tileName = tileAppearance->getTileName(tile);

		if (!tile.exists() || tileName.empty() || tileName[0] == '!')
		{
			continue;
		}

		if (tile.isWall())
		{
			walls.push_back(tile);
		}
		else
		{
			floors.push_back(tile);
		}
	}
}

void NEWindow::updateTooltip()
{
	float tooltipMargin = 10.f;
	sf::Vector2f tooltipExpansion(3.f, 3.f);
	tooltip->setTextAlignment(gui2::Text::AlignBottomLeft);
	tooltip->setPosition(mainPanel->getSideSize(gui2::BorderPanel::Left) + tooltipMargin,
		getSize().y / getViewMultiplier() - tooltipMargin);

	std::vector<std::string> tooltips;

	if (getTool())
	{
		tooltips.push_back(getTool()->getTooltip());
	}

	if (editor->isMouseOver())
	{
		tooltips.push_back("x: " + cNtoS(editor->getMousePositionTile().x));
		tooltips.push_back("y: " + cNtoS(editor->getMousePositionTile().y));
	}

	tooltip->setText(joinStrings(tooltips, "\n", true));
	tooltipBackground->setVisible(!tooltip->getText().empty());
	tooltipBackground->setRect(moveRect(expandRect(tooltip->getTextRect(), tooltipExpansion), tooltip->getPosition()));
}

void NEWindow::onProcessWindow(const gui2::WidgetEvents& events)
{
	for (std::size_t i = 0; i < toolButtons.size(); ++i)
	{
		if (toolButtons[i].button->isClicked())
		{
			for (std::size_t j = 0; j < toolButtons.size(); ++j)
			{
				toolButtons[j].button->setActivated(j == i);
			}

			setToolByName(toolButtons[i].actionName);
		}
	}

	for (std::size_t i = 0; i < actionButtons.size(); ++i)
	{
		if (actionButtons[i].button->isClicked())
		{
			if (actionButtons[i].actionName == "file_new")
			{
				newDungeon();
			}
			else if (actionButtons[i].actionName == "file_open")
			{
				openDungeon();
			}
			else if (actionButtons[i].actionName == "file_save")
			{
				saveDungeon();
			}
			else if (actionButtons[i].actionName == "file_save_as")
			{
				saveDungeonAs();
			}
			else if (actionButtons[i].actionName == "reset_camera")
			{
				editor->resetCamera();
			}
		}
	}

	if (levelPanel->wasChanged())
	{
		if (levelPanel->hasSelectedLevel())
		{
			switchToLevel(&dungeon->getLevel(levelPanel->getSelectedLevel()));
		}
		else
		{
			switchToLevel(nullptr);
		}
	}

	if (openDialog.isDone())
	{
		editor->resetCamera();
		switchToLevel(nullptr);

		setCurrentDungeon(openDialog.getSelectedFile());
		openDialog.clear();

		if (dungeon->loadFromXML(getCurrentDungeon()))
		{
			// Select first non-boss level to show in editor.
			for (std::size_t levelID = 0; levelID < dungeon->getLevelCount(); ++levelID)
			{
				Level & level = dungeon->getLevel(levelID);

				if (!level.isBoss())
				{
					levelPanel->updateDungeon();
					levelPanel->setSelectedLevel(levelID);
					switchToLevel(&level);
				}
			}
		}
		else
		{
			setCurrentDungeon("");
			errorMessage->setMessage("Failed to load dungeon (file may be corrupted).");
			errorMessage->show();
		}
	}

	if (saveDialog.isDone())
	{
		setCurrentDungeon(saveDialog.getSelectedFile());
		saveDialog.clear();
		saveDungeon();
	}

	if (newConfirmMessage->wasClosed() && newConfirmMessage->getClickedButton() == 0)
	{
		editor->resetCamera();
		setCurrentDungeon("");
		switchToLevel(nullptr);
		while (dungeon->getLevelCount() != 0)
		{
			dungeon->removeLevel(dungeon->getLevelCount() - 1);
		}
		initLevel();
	}

	if (closeConfirmMessage->wasClosed() && closeConfirmMessage->getClickedButton() == 0)
	{
		close();
	}

	if (events.heldInputs.contains(sf::Keyboard::LControl) && events.pressedInputs.contains(sf::Keyboard::S))
	{
		saveDungeon();
	}

	updateTooltip();
}

NEApplication* NEWindow::getParentApplication() const
{
	return dynamic_cast<NEApplication*>(&getParentInterface()->getParentApplication());
}

sf::Transform NEWindow::getContainerTransform() const
{
	sf::Transform transform;
	transform.scale(viewMultiplier, viewMultiplier);
	return transform;
}

void NEWindow::switchToLevel(Level* level)
{
	editor->setLevel(level);
	editorData.level = editor->getLevel();

	if (getTool() != nullptr)
	{
		getTool()->onDisable();
		getTool()->setEditorData(editorData);
		getTool()->onEnable();
	}
}

void NEWindow::setCurrentDungeon(std::string currentDungeon)
{
	currentDungeonFile = std::move(currentDungeon);
}

const std::string & NEWindow::getCurrentDungeon() const
{
	return currentDungeonFile;
}

void NEWindow::newDungeon()
{
	newConfirmMessage->show();
}

void NEWindow::openDungeon()
{
	openDialog.showDialog(FileChooser::OpenFile);
}

void NEWindow::saveDungeon()
{
	if (getCurrentDungeon().empty())
	{
		saveDungeonAs();
	}
	else if (!dungeon->saveToXML(getCurrentDungeon()))
	{
		errorMessage->setMessage("Failed to save dungeon (file may be in use).");
		errorMessage->show();
	}
}

void NEWindow::saveDungeonAs()
{
	saveDialog.showDialog(FileChooser::SaveFile);
}
