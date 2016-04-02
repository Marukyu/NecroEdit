#include "Client/GUI2/Widgets/Button.hpp"
#include "Shared/Utils/Utilities.hpp"

namespace gui2
{

Ptr<Button> Button::make()
{
	Ptr<Button> widget = std::make_shared<Button>();
	widget->init();
	return widget;
}

Ptr<Button> Button::make(std::string buttonText)
{
	Ptr<Button> widget = std::make_shared<Button>();
	widget->init();
	widget->setText(buttonText);
	return widget;
}

void Button::setActivated(bool activated)
{
	if (myIsActivated != activated)
	{
		myIsActivated = activated;
		invalidateVertices();
	}
}
bool Button::isActivated() const
{
	return myIsActivated;
}

void Button::init()
{
	TextWidget::init();

	setTextPixelPerfect(true);
	setTextAlignment(AlignCenter);
	setActivated(false);
}

sf::FloatRect Button::getTextMaxBoundingBox() const
{
	return expandRect(TextWidget::getTextMaxBoundingBox(), sf::Vector2f(-4, -4));
}

void Button::onProcess(const WidgetEvents & events)
{

}

void Button::onUpdateVertexCache()
{
	clearVertices();

	BoxStyle boxStyle = getDefaultBoxStyle();
	boxStyle.activated = isActivated();
	vertexAddBox(BoxTypeBasic, boxStyle);
	vertexAddText();
}

void Button::onTextResize()
{
	recalculateText();
	setAutomaticSize(getTextUnscaledSize() + sf::Vector2f(8, 8));
}


} // namespace gui2
