#include "Client/GUI2/Widgets/TabArea.hpp"
#include "Client/GUI2/Widgets/Button.hpp"
#include "Client/GUI2/Container.hpp"
#include "Shared/Utils/Utilities.hpp"

#include <algorithm>
#include <cmath>

namespace gui2
{

Ptr<TabArea> TabArea::make()
{
	Ptr<TabArea> widget = std::make_shared<TabArea>();
	widget->init();
	return widget;
}

void TabArea::setTabs(std::string names, const std::string & delimiter)
{
	std::vector<std::string> itemVec = splitString(names, delimiter);
	setTabs(itemVec);
}

void TabArea::setTabs(const std::vector<std::string> & names)
{
	setTabCount(names.size());

	for (std::size_t i = 0; i < names.size(); ++i)
	{
		setTabName(i, names[i]);
	}
}

void TabArea::setTabCount(std::size_t tabCount)
{
	if (tabCount > getTabCount())
	{
		insertTab(getTabCount(), tabCount - getTabCount());
	}
	else if (tabCount < getTabCount())
	{
		eraseTab(tabCount, getTabCount() - tabCount);
	}
}

std::size_t TabArea::getTabCount() const
{
	return myTabs.size();
}

void TabArea::insertTab(std::size_t index, std::size_t count)
{
	if (count == 0 || index > getTabCount())
		return;

	myTabs.insert(myTabs.begin() + index, count, TabData());

	for (std::size_t i = index; i < count; ++i)
	{
		initTab(i);
	}

	updateTabButtons();
	updateTabContainers();

	if (!myIsAnimating)
		hideInactiveTabs();
}

void TabArea::eraseTab(std::size_t index, std::size_t count)
{
	if (count == 0 || index + count > getTabCount())
		return;

	myTabs.erase(myTabs.begin() + index, myTabs.begin() + index + count);

	updateTabButtons();
	updateTabContainers();

	if (!myIsAnimating)
		hideInactiveTabs();
}

void TabArea::setTabName(std::size_t index, std::string name)
{
	if (index < getTabCount() && myTabs[index].button->getText() != name)
	{
		myTabs[index].button->setText(name);
		updateTabButtons();
	}
}

std::string TabArea::getTabName(std::size_t index) const
{
	return index < getTabCount() ? myTabs[index].button->getText() : "";
}

std::shared_ptr<Container> TabArea::tab(std::size_t index) const
{
	return index < getTabCount() ? myTabs[index].container : nullptr;
}

void TabArea::setSelectedTab(std::size_t index)
{
	// limit index to between last tab and 0, with bias towards 0.
	index = std::max<int>(0, std::min<int>(index, getTabCount() - 1));

	if (mySelectedTab != index)
	{
		myTabs[mySelectedTab].button->setActivated(false);
		mySelectedTab = index;
		myIsAnimating = true;
		myAnimClock.restart();
		myTabs[mySelectedTab].button->setActivated(true);
		showInactiveTabs();
	}
}

std::size_t TabArea::getSelectedTab()
{
	return mySelectedTab;
}

void TabArea::init()
{
	mySelectedTab = 0;
	myTabUnitOffset = 0.f;
	myButtonOffset = 0.f;
	myIsAnimating = false;
	myIsFontLoaded = false;

	myContainer = Container::make();
	myButtonContainer = Container::make();

	myContainer->setPosition(2.f, cvButtonHeight);

	link(myContainer);
	link(myButtonContainer);
}

const float TabArea::cvButtonHeight = 24.f;


void TabArea::onResize()
{
	myContainer->setSize(getSize().x - 4.f, getSize().y - cvButtonHeight - 2.f);
	myButtonContainer->setSize(getSize().x, cvButtonHeight);
	updateTabButtons();
	updateTabContainers();
	invalidateVertices();
}

void TabArea::onProcess(const WidgetEvents & events)
{
	if (!myIsFontLoaded && getDefaultFont())
	{
		myIsFontLoaded = true;
		updateTabButtons();
	}

	for (std::size_t i = 0; i < getTabCount(); ++i)
	{
		if (myTabs[i].button->isClicked())
		{
			setSelectedTab(i);
		}
	}

	if (myIsAnimating)
	{
		float animSpeed = 0.25f * (myAnimClock.restart().asSeconds() * 60.f);
		myTabUnitOffset = (myTabUnitOffset + (float)mySelectedTab * animSpeed) / (animSpeed + 1.f);

		if (std::abs(myTabUnitOffset - (float)mySelectedTab) < 0.001f)
		{
			myTabUnitOffset = mySelectedTab;
			myIsAnimating = false;
			hideInactiveTabs();
		}

		updateTabContainers();
	}
}

void TabArea::onUpdateVertexCache()
{
	clearVertices();

	BoxStyle backgroundBox;
	//backgroundBox.borderThickness = 2.f;
	vertexAddBox(sf::FloatRect(0, cvButtonHeight - 2.f, getSize().x, getSize().y - cvButtonHeight + 2.f), BoxTypeBackground, backgroundBox);
}

void TabArea::updateTabContainers()
{
	for (std::size_t i = 0; i < getTabCount(); ++i)
	{
		myTabs[i].container->setRect(sf::FloatRect(myContainer->getSize().x * (i - myTabUnitOffset), 0.f, myContainer->getSize().x, myContainer->getSize().y));
	}
}

void TabArea::updateTabButtons()
{
	if (!myIsFontLoaded)
		return;

	float buttonPos = -myButtonOffset;

	for (std::size_t i = 0; i < getTabCount(); ++i)
	{
		myTabs[i].button->loadTextFont();
		myTabs[i].button->setRect(sf::FloatRect(buttonPos, 0.f, myTabs[i].button->getTextSize().x + 10.f, cvButtonHeight));
		buttonPos += myTabs[i].button->getSize().x - 2.f;
	}
}

void TabArea::showInactiveTabs()
{
	for (std::size_t i = 0; i < getTabCount(); ++i)
	{
		myTabs[i].container->setVisible(true);
	}
}

void TabArea::hideInactiveTabs()
{
	for (std::size_t i = 0; i < getTabCount(); ++i)
	{
		myTabs[i].container->setVisible(mySelectedTab == i);
	}
}

void TabArea::initTab(std::size_t index)
{
	TabData & tab = myTabs[index];

	tab.button = Button::make();
	tab.button->setTextMaxSizeBehavior(btx::BitmapText::Ignore);
	tab.button->setActivated(mySelectedTab == index);

	tab.closeButton = Button::make();
	tab.closeButton->setVisible(false);

	tab.container = Container::make();
	tab.container->setZPosition(1);
	//tab.container->setVisible(mySelectedTab == index);

	myContainer->add(tab.container);
	myButtonContainer->add(tab.button);
}

} // namespace gui2
