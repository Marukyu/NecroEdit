#include <Client/Editor/LevelPanel.hpp>
#include <Client/GUI2/Widgets/Gradient.hpp>
#include <SFML/Graphics/Color.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <memory>
#include <string>

static constexpr float LEVEL_LIST_ENTRY_HEIGHT = 30.f;

gui2::Ptr<LevelPanel> LevelPanel::make()
{
	gui2::Ptr<LevelPanel> widget = std::make_shared<LevelPanel>();
	widget->init();
	return widget;
}

LevelPanel::LevelPanel() :
		dungeon(nullptr),
		isAnyLevelSelected(false),
		selectedLevel(0),
		wasLevelChanged(false)
{
}

void LevelPanel::init()
{
	gui2::Container::init();

	auto mainPanel = gui2::BorderPanel::make();

	levelListContainer = gui2::Container::make();

	buttonAddLevel = gui2::Button::make("+");
	buttonRemoveLevel = gui2::Button::make("-");
	buttonDuplicateLevel = gui2::Button::make("x2");
	buttonMoveLevelUp = gui2::Button::make("/\\");
	buttonMoveLevelDown = gui2::Button::make("\\/");

	auto levelButtonGrid = gui2::GridPanel::make(5, 1);
	levelButtonGrid->add(buttonAddLevel);
	levelButtonGrid->add(buttonRemoveLevel);
	levelButtonGrid->add(buttonDuplicateLevel);
	levelButtonGrid->add(buttonMoveLevelUp);
	levelButtonGrid->add(buttonMoveLevelDown);

	mainPanel->add(levelListContainer, gui2::BorderPanel::Center);
	mainPanel->add(levelButtonGrid, gui2::BorderPanel::Bottom, 30);
	mainPanel->add(gui2::Gradient::make(sf::Color(24, 24, 24, 200), sf::Color(32, 32, 32, 200)),
		gui2::BorderPanel::Center);
	add(mainPanel);
}

void LevelPanel::setSelectedLevel(std::size_t level)
{
	if (hasSelectedLevel() && getSelectedLevel() == level)
	{
		return;
	}

	if (hasSelectedLevel())
	{
		setLevelListEntryColor(getSelectedLevel(), false);
	}

	isAnyLevelSelected = true;
	selectedLevel = level;
	setLevelListEntryColor(getSelectedLevel(), true);
	updateLevelButtons();
}

void LevelPanel::deselectLevel()
{
	if (hasSelectedLevel())
	{
		setLevelListEntryColor(getSelectedLevel(), false);
	}

	isAnyLevelSelected = false;
}

std::size_t LevelPanel::getSelectedLevel() const
{
	return selectedLevel;
}

bool LevelPanel::hasSelectedLevel() const
{
	return dungeon != nullptr && isAnyLevelSelected && selectedLevel < dungeon->getLevelCount();
}

Dungeon * LevelPanel::getDungeon() const
{
	return dungeon;
}

void LevelPanel::setDungeon(Dungeon * dungeon)
{
	this->dungeon = dungeon;
	updateDungeon();
}

void LevelPanel::updateDungeon()
{
	while (levelListEntries.size() < dungeon->getLevelCount())
	{
		std::size_t levelID = levelListEntries.size();
		levelListEntries.push_back(gui2::Text::make(" Level " + cNtoS(levelID + 1)));
		levelListContainer->add(levelListEntries.back());
		updateLevelListEntryRect(levelID);
		setLevelListEntryColor(levelID, false);
	}

	while (levelListEntries.size() > dungeon->getLevelCount())
	{
		levelListContainer->remove(levelListEntries.back());
		levelListEntries.pop_back();
	}

	updateLevelButtons();
}

bool LevelPanel::wasChanged()
{
	if (wasLevelChanged)
	{
		wasLevelChanged = false;
		return true;
	}
	return false;
}

void LevelPanel::selectLevel(std::size_t level)
{
	setSelectedLevel(level);
	wasLevelChanged = true;
}

void LevelPanel::updateLevelButtons()
{
	buttonAddLevel->setEnabled(dungeon != nullptr);
	buttonRemoveLevel->setEnabled(hasSelectedLevel());
	buttonDuplicateLevel->setEnabled(hasSelectedLevel());
	buttonMoveLevelUp->setEnabled(hasSelectedLevel() && getSelectedLevel() > 0);
	buttonMoveLevelDown->setEnabled(hasSelectedLevel() && getSelectedLevel() < dungeon->getLevelCount() - 1);
}

void LevelPanel::setLevelListEntryColor(std::size_t level, bool highlight)
{
	if (level < levelListEntries.size())
	{
		levelListEntries[level]->setTextColor(highlight ? sf::Color::White : sf::Color(128, 128, 128));
	}
}

void LevelPanel::updateLevelListEntryRect(std::size_t level)
{
	if (level < levelListEntries.size())
	{
		levelListEntries[level]->setRect(0, level * LEVEL_LIST_ENTRY_HEIGHT, getSize().x, LEVEL_LIST_ENTRY_HEIGHT);
	}
}

void LevelPanel::onResize()
{
	gui2::Container::onResize();

	for (std::size_t level = 0; level < levelListEntries.size(); ++level)
	{
		updateLevelListEntryRect(level);
	}
}

void LevelPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	for (std::size_t i = 0; i < levelListEntries.size(); ++i)
	{
		auto levelEntry = levelListEntries[i];

		if (levelEntry->isMouseDown())
		{
			selectLevel(i);
		}
	}

	if (dungeon != nullptr)
	{
		if (buttonAddLevel->isClicked())
		{
			std::size_t insertionIndex = hasSelectedLevel() ? getSelectedLevel() + 1 : dungeon->getLevelCount();
			dungeon->insertLevel(insertionIndex);

			updateDungeon();
			selectLevel(insertionIndex);
		}

		if (buttonRemoveLevel->isClicked())
		{
			if (hasSelectedLevel())
			{
				dungeon->removeLevel(getSelectedLevel());

				updateDungeon();
				selectLevel(selectedLevel == dungeon->getLevelCount() ? selectedLevel - 1 : selectedLevel);
			}
		}

		if (buttonDuplicateLevel->isClicked())
		{
			if (hasSelectedLevel())
			{
				dungeon->insertLevel(getSelectedLevel());

				const Level & sourceLevel = dungeon->getLevel(getSelectedLevel() + 1);
				Level & destLevel = dungeon->getLevel(getSelectedLevel());
				destLevel.assign(sourceLevel);

				updateDungeon();
				selectLevel(getSelectedLevel() + 1);
			}
		}

		if (buttonMoveLevelUp->isClicked())
		{
			if (hasSelectedLevel() && getSelectedLevel() > 0)
			{
				dungeon->swapLevels(getSelectedLevel(), getSelectedLevel() - 1);

				updateDungeon();
				selectLevel(getSelectedLevel() - 1);
			}
		}

		if (buttonMoveLevelDown->isClicked())
		{
			if (hasSelectedLevel() && getSelectedLevel() < dungeon->getLevelCount() - 1)
			{
				dungeon->swapLevels(getSelectedLevel(), getSelectedLevel() + 1);

				updateDungeon();
				selectLevel(getSelectedLevel() + 1);
			}
		}
	}
}
