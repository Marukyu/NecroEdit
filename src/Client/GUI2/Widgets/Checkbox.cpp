#include "Client/GUI2/Widgets/Checkbox.hpp"

namespace gui2
{

Ptr<Checkbox> Checkbox::make()
{
	Ptr<Checkbox> widget = std::make_shared<Checkbox>();
	widget->init();
	return widget;
}

void Checkbox::setChecked(bool checked)
{
	if (myIsChecked != checked)
	{
		myIsChecked = checked;
		notify(NotifyCheckboxToggled);
		invalidateVertices();
	}
}
bool Checkbox::isChecked() const
{
	return myIsChecked;
}

sf::FloatRect Checkbox::getTextMaxBoundingBox() const
{
	return sf::FloatRect(getCheckboxSize() * 1.5f, 0,
						 getSize().x - getCheckboxSize() * 1.5f, getSize().y);
}

float Checkbox::getCheckboxSize() const
{
	return 16.f;
}

void Checkbox::init()
{
	TextWidget::init();

	setTextAlignment(AlignLeft);
	setChecked(false);
}

void Checkbox::onProcess(const WidgetEvents & events)
{
	if (isClicked())
		setChecked(!isChecked());
}

void Checkbox::onUpdateVertexCache()
{
	clearVertices();

	BoxStyle boxStyle = getDefaultBoxStyle();
	boxStyle.focused = isMouseOver();
	boxStyle.hovered = isChecked();
	boxStyle.activated = isChecked();

	vertexAddBox(sf::FloatRect(0, (getSize().y - getCheckboxSize()) / 2.f,
							   getCheckboxSize(), getCheckboxSize()),
				 BoxTypeBasic, boxStyle);

	vertexAddText();
}

} // namespace gui2
