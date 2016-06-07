#include <Client/Editor/LevelPanel.hpp>
#include <Client/GUI2/Internal/WidgetEvents.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <Client/GUI2/Widgets/Gradient.hpp>
#include <Client/GUI2/Widgets/Menu.hpp>
#include <SFML/Graphics/Color.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>

static constexpr float LEVEL_LIST_ENTRY_HEIGHT = 20.f;
static constexpr float SLIDER_WIDTH = 16.f;

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
	wasLevelChanged(false),
	scrollVelocity(0.f),
	messageBoxTarget(nullptr)
{
}

LevelPanel::~LevelPanel()
{
	if (deleteConfirmMessage->getParent())
	{
		deleteConfirmMessage->getParent()->remove(deleteConfirmMessage);
	}
}

void LevelPanel::init()
{
	gui2::Container::init();

	auto mainPanelContainer = gui2::GridPanel::make(1, 1);

	auto mainPanel = gui2::BorderPanel::make();

	levelListContainer = gui2::Container::make();
	levelListInnerContainer = gui2::Container::make();
	levelListContainer->add(levelListInnerContainer);

	// TODO: change placeholder button texts to proper symbols.
	buttonAddLevel = gui2::Button::make("+");
	buttonRemoveLevel = gui2::Button::make("-");
	buttonDuplicateLevel = gui2::Button::make("x2");
	buttonMoveLevelUp = gui2::Button::make("/\\");
	buttonMoveLevelDown = gui2::Button::make("\\/");

	levelListSlider = gui2::Slider::make();
	levelListSlider->setVertical(true);

	auto levelButtonGrid = gui2::GridPanel::make(5, 1);
	levelButtonGrid->add(buttonAddLevel);
	levelButtonGrid->add(buttonRemoveLevel);
	levelButtonGrid->add(buttonDuplicateLevel);
	levelButtonGrid->add(buttonMoveLevelUp);
	levelButtonGrid->add(buttonMoveLevelDown);

	dropdownMusic = gui2::Dropdown::make();
	dropdownMusic->setVerticalFlip(true);

	dropdownBoss = gui2::Dropdown::make();
	dropdownBoss->setVerticalFlip(true);

	mainPanel->add(levelListContainer, gui2::BorderPanel::Center);
	mainPanel->add(levelListSlider, gui2::BorderPanel::Right, SLIDER_WIDTH);

	mainPanel->add(levelButtonGrid, gui2::BorderPanel::Bottom, 30);
	mainPanel->add(dropdownBoss, gui2::BorderPanel::Bottom, 24);
	mainPanel->add(gui2::Text::make(" Boss:"), gui2::BorderPanel::Bottom, 24);
	mainPanel->add(dropdownMusic, gui2::BorderPanel::Bottom, 24);
	mainPanel->add(gui2::Text::make(" Music:"), gui2::BorderPanel::Bottom, 24);

	mainPanelContainer->add(gui2::Gradient::make(sf::Color(24, 24, 24, 200), sf::Color(32, 32, 32, 200)));
	mainPanelContainer->add(mainPanel);

	add(mainPanelContainer);

	deleteConfirmMessage = gui2::MessageBox::make("Are you sure you want to delete this level?", "Confirmation", {
		"Yes",
		"No" });
	setMessageBoxTarget(this);
}

void LevelPanel::setMessageBoxTarget(gui2::Container* target)
{
	if (messageBoxTarget != target)
	{
		messageBoxTarget = target;

		if (target == nullptr)
		{
			add(deleteConfirmMessage);
		}
		else
		{
			target->add(deleteConfirmMessage);
		}
	}
}

gui2::Container* LevelPanel::getMessageBoxTarget() const
{
	return messageBoxTarget;
}

void LevelPanel::setSongs(std::map<int, std::string> songs)
{
	songIDs.clear();
	std::vector<std::string> songNames;

	for (auto it = songs.begin(); it != songs.end(); ++it)
	{
		songIDs.push_back(it->first);
		songNames.push_back(it->second);
	}

	dropdownMusic->setModel(std::make_shared<gui2::DefaultMenuModel>(songNames));
}

void LevelPanel::setBosses(std::map<int, std::string> bosses)
{
	bossIDs.clear();
	std::vector<std::string> bossNames;

	for (auto it = bosses.begin(); it != bosses.end(); ++it)
	{
		bossIDs.push_back(it->first);
		bossNames.push_back(it->second);
	}

	dropdownBoss->setModel(std::make_shared<gui2::DefaultMenuModel>(bossNames));
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
	updateDropdowns();
}

void LevelPanel::unsetSelectedLevel()
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
		levelListInnerContainer->add(levelListEntries.back());
		updateLevelListEntryRect(levelID);
		setLevelListEntryColor(levelID, false);
	}

	while (levelListEntries.size() > dungeon->getLevelCount())
	{
		levelListInnerContainer->remove(levelListEntries.back());
		levelListEntries.pop_back();
	}

	levelListInnerContainer->setSize(levelListInnerContainer->getSize().x,
		levelListEntries.size() * LEVEL_LIST_ENTRY_HEIGHT);

	updateLevelButtons();
	updateDropdowns();
	updateSlider();
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

	// TODO: Scroll to selected level if it is out of view.
}

void LevelPanel::deselectLevel()
{
	unsetSelectedLevel();
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
		levelListEntries[level]->setRect(0, level * LEVEL_LIST_ENTRY_HEIGHT, getSize().x - SLIDER_WIDTH,
			LEVEL_LIST_ENTRY_HEIGHT);
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

void LevelPanel::updateDropdowns()
{
	dropdownMusic->setEnabled(hasSelectedLevel());
	dropdownBoss->setEnabled(hasSelectedLevel());

	if (hasSelectedLevel())
	{
		// Resolve index of song entry.
		auto songIterator = std::find(songIDs.begin(), songIDs.end(), dungeon->getLevel(getSelectedLevel()).getMusic());

		// Select song if it has an entry, deselect otherwise.
		dropdownMusic->setSelectionSilent(songIterator == songIDs.end() ? -1 : (songIterator - songIDs.begin()));

		// Resolve index of boss entry.
		auto bossIterator = std::find(bossIDs.begin(), bossIDs.end(), dungeon->getLevel(getSelectedLevel()).getBoss());

		// Select boss if it has an entry, deselect otherwise.
		dropdownBoss->setSelectionSilent(bossIterator == bossIDs.end() ? -1 : (bossIterator - bossIDs.begin()));
	}
}

void LevelPanel::updateSlider()
{
	float sliderMax = levelListInnerContainer->getSize().y - levelListContainer->getSize().y;

	if (sliderMax > 0.f)
	{
		levelListSlider->setEnabled(true);
		levelListSlider->setBounds(0, sliderMax);
	}
	else
	{
		levelListSlider->setEnabled(false);
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

	if (dropdownMusic->wasChanged())
	{
		try
		{
			dungeon->getLevel(getSelectedLevel()).setMusic(songIDs.at(dropdownMusic->getSelection()));
		}
		catch (...)
		{
			// No level selected or song ID somehow out of range? Ignore.
		}
	}

	if (dropdownBoss->wasChanged())
	{
		try
		{
			dungeon->getLevel(getSelectedLevel()).setBoss(bossIDs.at(dropdownBoss->getSelection()));
		}
		catch (...)
		{
			// No level selected or boss ID somehow out of range? Ignore.
		}
	}

	updateSlider();

	if (levelListSlider->isEnabled())
	{
		if ((levelListContainer->isMouseOver() || levelListSlider->isMouseOver()) && events.mouseWheelDelta != 0)
		{
			scrollVelocity -= events.mouseWheelDelta * 5;
		}

		if (std::abs(scrollVelocity) > 0.0001f)
		{
			float newSliderValue = levelListSlider->getValue() + scrollVelocity;
			if (newSliderValue > levelListSlider->getMax())
			{
				newSliderValue = levelListSlider->getMax();
				scrollVelocity = 0.f;
			}
			else if (newSliderValue < levelListSlider->getMin())
			{
				newSliderValue = levelListSlider->getMin();
				scrollVelocity = 0.f;
			}
			levelListSlider->setValue(newSliderValue);
			scrollVelocity *= 0.75f;
		}
	}

	levelListInnerContainer->setPosition(0.f, -levelListSlider->getValue());
	levelListInnerContainer->setSize(levelListContainer->getSize().x, levelListInnerContainer->getSize().y);

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
				deleteConfirmMessage->show();
			}
		}

		if (deleteConfirmMessage->wasClosed() && deleteConfirmMessage->getClickedButton() == 0)
		{
			if (hasSelectedLevel())
			{
				dungeon->removeLevel(getSelectedLevel());

				updateDungeon();

				if (dungeon->getLevelCount() == 0)
				{
					deselectLevel();
				}
				else
				{
					selectLevel(selectedLevel == dungeon->getLevelCount() ? selectedLevel - 1 : selectedLevel);
				}
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
