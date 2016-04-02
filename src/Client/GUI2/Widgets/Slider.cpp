#include "Client/GUI2/Widgets/Slider.hpp"
#include "Client/GUI2/Container.hpp"
#include "Shared/Utils/Utilities.hpp"

#include <sstream>

namespace gui2
{

Ptr<Slider> Slider::make()
{
	Ptr<Slider> widget = std::make_shared<Slider>();
	widget->init();
	return widget;
}

void Slider::setValue(float value)
{
	if (value < myMin)
		value = myMin;
	if (value > myMax)
		value = myMax;

	if (myValue != value)
	{
		myValue = value;
		//myWasChanged = true;
		notify(NotifySliderValueChanged);
		updateSliderText();
		invalidateVertices();
	}
}

float Slider::getValue() const
{
	return myValue;
}

void Slider::setBounds(float min, float max)
{
	if (myMin != min || myMax != max)
	{
		if (min == max)
		{
			max += 1.f;
		}
		myMin = min;
		myMax = max;
		setValue(getValue());
		invalidateVertices();
	}
}

float Slider::getMin() const
{
	return myMin;
}

float Slider::getMax() const
{
	return myMax;
}

void Slider::setVertical(bool vertical)
{
	if (myIsVertical != vertical)
	{
		myIsVertical = vertical;
		invalidateVertices();
	}
}

bool Slider::isVertical() const
{
	return myIsVertical;
}

void Slider::displayText(bool text)
{
	if (myDisplayText != text)
	{
		myDisplayText = text;
		updateSliderText();
		invalidateVertices();
	}
}

void Slider::displayText(bool text, bool always)
{
	displayText(text);
	alwaysDisplayText(always);
}

bool Slider::isDisplayingText() const
{
	return myDisplayText;
}

void Slider::alwaysDisplayText(bool text)
{
	if (myAlwaysDisplayText != text)
	{
		myAlwaysDisplayText = text;
		updateSliderText();
		invalidateVertices();
	}
}

bool Slider::isAlwaysDisplayingText() const
{
	return myAlwaysDisplayText;
}

void Slider::setStyle(Style style)
{
	if (myStyle != style)
	{
		myStyle = style;
		invalidateVertices();
	}
}

Slider::Style Slider::getStyle() const
{
	return myStyle;
}

/*
bool Slider::wasChanged()
{
	if (myWasChanged)
	{
		myWasChanged = false;
		return true;
	}
	return false;
}
*/

void Slider::init()
{
	TextWidget::init();
	setTextPixelPerfect(true);
	setTextMaxSizeBehavior(btx::BitmapText::Ignore);

	myValue = 0.f;
	myMin = 0.f;
	myMax = 1.f;
	myIsVertical = false;
	myDisplayText = false;
	myAlwaysDisplayText = false;
	myStyle = StyleFull;
	//myWasChanged = true;
}

const float Slider::cvSliderSize = 10, Slider::cvSliderLineSize = 8;

sf::FloatRect Slider::getSliderRect() const
{
	if (isVertical())
	{
		return sf::FloatRect(
			0,
			((getValue() - getMin()) / (getMax() - getMin())) * (getSize().y - cvSliderSize),
			getSize().x,
			cvSliderSize);
	}
	else
	{
		return sf::FloatRect(
			((getValue() - getMin()) / (getMax() - getMin())) * (getSize().x - cvSliderSize),
			0,
			cvSliderSize,
			getSize().y);
	}
}

float Slider::posToValue(sf::Vector2f pos) const
{
	if (isVertical())
		return getMin()+(getMax()-getMin()) * ((pos.y - cvSliderSize / 2.f) / (getSize().y - cvSliderSize));
	else
		return getMin()+(getMax()-getMin()) * ((pos.x - cvSliderSize / 2.f) / (getSize().x - cvSliderSize));
}

void Slider::onProcess(const WidgetEvents & events)
{
	if (isEnabled() && isMouseDown())
	{
		setValue(posToValue(events.mousePosition));
	}

	if (isMouseDown() || isMouseOver())
	{
		myHighlightTimer.restart(sf::milliseconds(1000));
	}

	if (isDisplayingText())
	{
		float fadeOut = isAlwaysDisplayingText() ? 1.f : std::min(myHighlightTimer.getRemainingTime().asSeconds() * 2.f, 1.f);
		if (getTextColor().a != fadeOut * 255)
		{
			setTextColor(sf::Color(255, 255, 255, fadeOut * 255));
			invalidateVertices();
		}
	}
}

void Slider::onUpdateVertexCache()
{
	clearVertices();

	float lineSize = getStyle() == StyleFull ? (isVertical() ? getSize().x : getSize().y) : cvSliderLineSize;

	BoxStyle backgroundBox;
	if (isVertical())
		vertexAddBox(sf::FloatRect((getSize().x - lineSize) / 2.f, 0, lineSize, getSize().y), BoxTypeDark, backgroundBox);
	else
		vertexAddBox(sf::FloatRect(0, (getSize().y - lineSize) / 2.f, getSize().x, lineSize), BoxTypeDark, backgroundBox);

	BoxStyle sliderBox = getDefaultBoxStyle();
	sliderBox.outlineColor.a = sliderBox.color.a = getStyle() == StyleTransparent ? 150 : 255;
	vertexAddBox(getSliderRect(), BoxTypeBasic, sliderBox);

	if (isDisplayingText())
	{
		// do this to get correct rectangle for surrounding box.
		recalculateText();

		BoxStyle textBox;
		textBox.outlineColor.a = textBox.color.a = getTextColor().a;
		vertexAddBox(expandRect(getTextRect(), sf::Vector2f(4,4)), BoxTypeDark, textBox);

		vertexAddTextured(getTextObject().getVertices());
	}
}

void Slider::onResize()
{
	recalculateText();
	invalidateVertices();
}

void Slider::updateSliderText()
{
	if (!isDisplayingText())
		return;

	std::ostringstream strm;
	strm.precision(getMax() > 100.f ? 0 : (getMax() > 10.f ? 1 : 2));
	strm << std::fixed << getValue();

	if (isAlwaysDisplayingText())
	{
		setTextAlignment(AlignLeft);
	}
	else
	{
		if (getPosition().y < 20.f)
		{
			if (getParent() && getPosition().y + getSize().y > getParent()->getContainerBoundingBox().height - 20.f)
				setTextPreciseAlignment((getValue() - getMin()) / (getMax() - getMin()), .5f);
			else
				setTextPreciseAlignment((getValue() - getMin()) / (getMax() - getMin()), 0.f);
		}
		else
			setTextPreciseAlignment((getValue() - getMin()) / (getMax() - getMin()), 1.f);
	}
	//setTextAlignment(isAlwaysDisplayingText() ? AlignLeft : AlignBottom);
	/*
	if (isAlwaysDisplayingText())
	{
		setTextAlignment(AlignLeft);
	}
	else
	{
		float valueRatio = (getValue() - getMin()) / (getMax() - getMin()) * 3.f;
		if (valueRatio < 1.f)
			setTextAlignment(AlignBottomLeft);
		else if (valueRatio < 2.f)
			setTextAlignment(AlignBottom);
		else
			setTextAlignment(AlignBottomRight);
	}
	*/
	setText(strm.str());
}

sf::FloatRect Slider::getTextMaxBoundingBox() const
{
	//float maxBoxDim = 100.f;

	if (isAlwaysDisplayingText())
		return sf::FloatRect(getSize().x + 5.f, 0.f, 40.f, getSize().y);
	else
	{
		//float relValue = (getValue() - getMin()) / (getMax() - getMin());
		//return expandRect(moveRect(getSliderRect(), sf::Vector2f(15.f - relValue * 30.f, -getSliderRect().height - 4.f)), sf::Vector2f(-4.f, 0.f));

		if (getPosition().y < 20.f)
		{
			if (getParent() && getPosition().y + getSize().y > getParent()->getContainerBoundingBox().height - 20.f)
				return expandRect(getSliderRect(), sf::Vector2f(-4.f, 0.f));
			else
				return expandRect(moveRect(getSliderRect(), sf::Vector2f(0.f, getSliderRect().height + 4.f)), sf::Vector2f(-4.f, 0.f));
		}
		else
			return expandRect(moveRect(getSliderRect(), sf::Vector2f(0.f, -getSliderRect().height - 4.f)), sf::Vector2f(-4.f, 0.f));
		//return sf::FloatRect(getSliderRect().left + (getSliderRect().width - maxBoxDim) / 2.f, -maxBoxDim - 4.f, maxBoxDim, maxBoxDim);
		//return getBaseRect();
	}
}


} // namespace gui2
