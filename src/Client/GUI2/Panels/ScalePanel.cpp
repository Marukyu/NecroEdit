#include "Client/GUI2/Panels/ScalePanel.hpp"

namespace gui2
{

Ptr<ScalePanel> ScalePanel::make()
{
	Ptr<ScalePanel> widget = std::make_shared<ScalePanel>();
	widget->init();
	return widget;
}

Ptr<ScalePanel> ScalePanel::make(Ptr<Widget> initWidget)
{
	Ptr<ScalePanel> widget = make();
	widget->add(initWidget);
	return widget;
}

void ScalePanel::init()
{
	Panel::init();
}

void ScalePanel::onUpdatePanel()
{
	for (Iterator it = begin(); it != end(); ++it)
	{
		update((*it).get());
	}
}

void ScalePanel::onResize()
{
	onUpdatePanel();
}

void ScalePanel::update(Widget * widget)
{
	if (!widget || widget->hasLinkParent())
		return;

	sf::Vector2f iSize(getSize() - sf::Vector2f((getPadding() + getMargin()) * 2.f, (getPadding() + getMargin()) * 2.f));
	sf::Vector2f wSize = widget->getAutomaticSize();

	if (iSize.x == 0.f || iSize.y == 0.f || wSize.x == 0.f || wSize.y == 0.f)
		return;

	float iRatio = iSize.x / iSize.y;
	float wRatio = wSize.x / wSize.y;

	if (iRatio > wRatio)
	{
		// panel is wider than widget.
		wSize = sf::Vector2f(iSize.y * wRatio, iSize.y);
	}
	else
	{
		// panel is higher than widget.
		wSize = sf::Vector2f(iSize.x, iSize.x / wRatio);
	}

	sf::Vector2f wPos(sf::Vector2f(getPadding() + getMargin(), getPadding() + getMargin()) + (iSize - wSize) / 2.f);

	if (isPixelPerfect())
	{
		widget->setPosition(sf::Vector2f(sf::Vector2i(wPos + sf::Vector2f(0.5f, 0.5f))));
		widget->setSize(sf::Vector2f(sf::Vector2i(wSize + sf::Vector2f(0.5f, 0.5f))));
	}
	else
	{
		widget->setPosition(wPos);
		widget->setSize(wSize);
	}
}

void ScalePanel::onAdd(std::shared_ptr<Widget> widget)
{
	widget->addObserver(this);
	widget->setAutoManagingSize(false);
	update(widget.get());
}

void ScalePanel::onRemove(std::shared_ptr<Widget> widget)
{
	widget->removeObserver(this);
}

void ScalePanel::onNotify(Observable & subject, int message)
{
	Panel::onNotify(subject, message);

	if (message == NotifyAutoSized)
	{
		Widget * widgetPtr = dynamic_cast<Widget*>(&subject);

		if (widgetPtr && isContained(widgetPtr))
		{
			update(widgetPtr);
		}
	}
}

} // namespace gui2
