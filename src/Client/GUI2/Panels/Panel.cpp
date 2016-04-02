#include "Client/GUI2/Panels/Panel.hpp"

namespace gui2
{

void Panel::init()
{
	Container::init();

	setAutoManagingWidth(true);
	setAutoManagingHeight(true);

	myMargin = 0.f;
	myPadding = 0.f;
	myIsPixelPerfect = false;
}

void Panel::setMargin(float margin)
{
	if (myMargin != margin)
	{
		myMargin = margin;
		onUpdatePanel();
	}
}

float Panel::getMargin() const
{
	return myMargin;
}

void Panel::setPadding(float padding)
{
	if (myPadding != padding)
	{
		myPadding = padding;
		onUpdatePanel();
	}
}

float Panel::getPadding() const
{
	return myPadding;
}

void Panel::setPixelPerfect(bool perfect)
{
	if (myIsPixelPerfect != perfect)
	{
		myIsPixelPerfect = perfect;
		onUpdatePanel();
	}
}

bool Panel::isPixelPerfect() const
{
	return myIsPixelPerfect;
}

void Panel::onNotify(Observable & subject, int message)
{
	if (message == Container::NotifyContainerBox && !overrideAutoSize() && &subject == getParent().get())
	{
		sf::FloatRect parRect = getParent()->getContainerBoundingBox();
		setAutomaticSize(sf::Vector2f(parRect.width, parRect.height));
	}
}

void Panel::onUpdatePanel()
{

}

bool Panel::overrideAutoSize() const
{
	return false;
}

void Panel::onParent(std::shared_ptr<Container> oldParent)
{
	Container::onParent(oldParent);

	if (oldParent)
		oldParent->removeObserver(this);

	if (getParent())
	{
		getParent()->addObserver(this);

		sf::FloatRect parRect = getParent()->getContainerBoundingBox();
		if (!overrideAutoSize())
			setAutomaticSize(sf::Vector2f(parRect.width, parRect.height));
	}
}

bool Panel::isManagingFocus() const
{
	return false;
}

}
