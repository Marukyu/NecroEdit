#include "Client/GUI2/Panels/GridPanel.hpp"

namespace gui2
{

Ptr<GridPanel> GridPanel::make()
{
	Ptr<GridPanel> widget = std::make_shared<GridPanel>();
	widget->init();
	return widget;
}

Ptr<GridPanel> GridPanel::make(Ptr<Widget> initWidget)
{
	Ptr<GridPanel> widget = make();
	widget->add(initWidget);
	return widget;
}

Ptr<GridPanel> GridPanel::make(sf::Vector2u gridSize)
{
	Ptr<GridPanel> widget = make();
	widget->setGridSize(gridSize);
	return widget;
}

Ptr<GridPanel> GridPanel::make(std::size_t gridWidth, std::size_t gridHeight)
{
	Ptr<GridPanel> widget = make();
	widget->setGridSize(gridWidth, gridHeight);
	return widget;
}

void GridPanel::setGridSize(sf::Vector2u gridSize)
{
	if (myGridSize != gridSize)
	{
		myGridSize = gridSize;
		myLastPlacement.x = gridSize.x - 1;
		myLastPlacement.y = gridSize.y - 1;
		onUpdateAutoCellSize();
		onResize();
	}
}

void GridPanel::setGridSize(std::size_t width, std::size_t height)
{
	setGridSize(sf::Vector2u(width, height));
}

sf::Vector2u GridPanel::getGridSize() const
{
	return sf::Vector2u(std::max(myGridSize.x, 1u), std::max(myGridSize.y, 1u));
}

void GridPanel::setAutoCellSize(sf::Vector2f cellSize)
{
	if (myAutoCellSize != cellSize)
	{
		myAutoCellSize = cellSize;
		onUpdateAutoCellSize();
	}
}

void GridPanel::setAutoCellSize(float cellWidth, float cellHeight)
{
	setAutoCellSize(sf::Vector2f(cellWidth, cellHeight));
}

sf::Vector2f GridPanel::getAutoCellSize() const
{
	return myAutoCellSize;
}


void GridPanel::add(Ptr<Widget> widget)
{
	if (doAdd(widget))
		place(widget);
}

void GridPanel::add(Ptr<Widget> widget, sf::Vector2u pos)
{
	if (doAdd(widget))
		place(widget, pos);
}

void GridPanel::add(Ptr<Widget> widget, std::size_t x, std::size_t y)
{
	if (doAdd(widget))
		place(widget, x, y);
}

void GridPanel::place(Ptr<Widget> widget)
{
	sf::Vector2u pos = myLastPlacement;

	++pos.x;

	if (pos.x >= getGridSize().x)
	{
		pos.x = 0;
		++pos.y;

		if (pos.y >= getGridSize().y)
			pos.y = 0;
	}

	place(widget, pos);
}

void GridPanel::place(Ptr<Widget> widget, sf::Vector2u pos)
{
	if (!widget || widget->hasLinkParent())
		return;

	PanelData data;
	data.position = pos;

	myPlacements[widget] = data;

	myLastPlacement = pos;
	widget->setAutoManagingSize(false);

	update(widget);
}

void GridPanel::place(Ptr<Widget> widget, std::size_t x, std::size_t y)
{
	place(widget, sf::Vector2u(x, y));
}

bool GridPanel::update(Ptr<Widget> widget)
{
	if (!widget || widget->hasLinkParent() || !isContained(widget))
		return false;

	auto it = myPlacements.find(widget);
	if (it == myPlacements.end())
		return false;

	sf::Vector2u pos(it->second.position);
	sf::Vector2f wSize((getSize().x - 2.f * getMargin()) / getGridSize().x, (getSize().y - 2.f * getMargin()) / getGridSize().y);

	sf::Vector2f fPos(getMargin() + getPadding() + pos.x * wSize.x, getMargin() + getPadding() + pos.y * wSize.y);
	sf::Vector2f fSize(wSize.x - 2.f * getPadding(), wSize.y - 2.f * getPadding());

	if (isPixelPerfect())
	{
		widget->setPosition(sf::Vector2f(sf::Vector2i(fPos + sf::Vector2f(0.5f, 0.5f))));
		//widget->setSize(sf::Vector2f(sf::Vector2i(fSize + sf::Vector2f(0.5f, 0.5f))));
	}
	else
	{
		widget->setPosition(fPos);
	}
	widget->setSize(fSize);

	return true;
}

void GridPanel::init()
{
	Panel::init();

	setGridSize(1, 1);
}

void GridPanel::onUpdatePanel()
{
	onUpdateAutoCellSize();
	onResize();
}

bool GridPanel::overrideAutoSize() const
{
	return getAutoCellSize() != sf::Vector2f();
}

void GridPanel::onResize()
{
	for (auto it = myPlacements.begin(); it != myPlacements.end(); ++it)
	{
		update(it->first.lock());
	}
}

void GridPanel::onAdd(Ptr<Widget> widget)
{
	place(widget);
}

void GridPanel::onRemove(Ptr<Widget> widget)
{
	myPlacements.erase(widget);
}

void GridPanel::onUpdateAutoCellSize()
{
	setAutomaticSize(sf::Vector2f(getGridSize().x * (2.f * getPadding() + getAutoCellSize().x) + 2.f * getMargin(),
								  getGridSize().y * (2.f * getPadding() + getAutoCellSize().y) + 2.f * getMargin()));
}

}

