#include <Client/Editor/Tools/Panels/SpawnPointToolPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <Client/GUI2/Widget.hpp>
#include <Client/GUI2/Widgets/Menu.hpp>
#include <Client/GUI2/Widgets/Text.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Level/Object.hpp>
#include <cstddef>
#include <memory>
#include <string>

gui2::Ptr<SpawnPointToolPanel> SpawnPointToolPanel::make(Dungeon & dungeon,
	const ObjectAppearanceManager & objectAppearance)
{
	gui2::Ptr<SpawnPointToolPanel> widget = std::make_shared<SpawnPointToolPanel>(dungeon, objectAppearance);
	widget->init();
	return widget;
}

SpawnPointToolPanel::SpawnPointToolPanel(Dungeon & dungeon, const ObjectAppearanceManager & objectAppearance) :
	dungeon(&dungeon),
	level(nullptr),
	objectAppearance(&objectAppearance)
{
}

SpawnPointToolPanel::~SpawnPointToolPanel()
{
}

void SpawnPointToolPanel::update()
{
	int playerCharacterIndex = -1;
	for (std::size_t i = 0; i < playerCharacterIDs.size(); ++i)
	{
		if (playerCharacterIDs[i] == dungeon->getPlayerCharacter())
		{
			playerCharacterIndex = i;
			break;
		}
	}
	playerCharacterDropdown->setSelection(playerCharacterIndex);

	startItemsCheckbox->setChecked(dungeon->isStartItemsEnabled());
}

void SpawnPointToolPanel::setLevel(Level * level)
{
	this->level = level;
}

void SpawnPointToolPanel::init()
{
	gui2::BorderPanel::init();

	playerCharacterIDs = objectAppearance->getObjectSubtypeList(Object::Type::Internal,
		ObjectAppearanceManager::InternalCharacter);

	std::vector<std::string> characterNames;
	Object characterObject(Object::Type::Internal);
	for (auto id : playerCharacterIDs)
	{
		characterObject.setPropertyInt(Object::Property::Subtype, id);
		characterNames.push_back(objectAppearance->getObjectName(characterObject));
	}

	auto playerCharacterPanel = gui2::GridPanel::make(2, 1);
	add(playerCharacterPanel, BorderPanel::Top, 25.f);

	auto playerCharacterLabel = gui2::Text::make("Character: ");
	playerCharacterLabel->setTextAlignment(gui2::Text::AlignRight);
	playerCharacterPanel->add(playerCharacterLabel);

	playerCharacterDropdown = gui2::Dropdown::make();
	playerCharacterDropdown->setModel(std::make_shared<gui2::DefaultMenuModel>(characterNames));
	playerCharacterPanel->add(playerCharacterDropdown);

	startItemsCheckbox = gui2::Checkbox::make();
	startItemsCheckbox->setText("Start with items");
	add(startItemsCheckbox, BorderPanel::Top, 25.f);

	update();
}

void SpawnPointToolPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	if (playerCharacterDropdown->wasChanged())
	{
		int selectionIndex = playerCharacterDropdown->getSelection();
		if (selectionIndex >= 0 && (std::size_t) selectionIndex < playerCharacterIDs.size())
		{
			dungeon->setPlayerCharacter(playerCharacterIDs[selectionIndex]);
			level->firePlayerSpawnPointUpdate();
		}
	}

	if (startItemsCheckbox->isClicked())
	{
		dungeon->setStartItemsEnabled(startItemsCheckbox->isChecked());
	}
}
