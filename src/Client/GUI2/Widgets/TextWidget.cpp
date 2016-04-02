#include "Client/GUI2/Widgets/TextWidget.hpp"
#include "Client/GUI2/Application.hpp"
#include "Client/Graphics/UtilitiesSf.hpp"
#include "Shared/Utils/VectorMul.hpp"

namespace gui2
{

void TextWidget::init()
{
	setTextAlignment(AlignCenter);
	myIsTextPixelPerfect = false;
	myFontIndex = Application::FontDefault;
	myTextColor = sf::Color::White;
	setTextMaxSizeBehavior(btx::BitmapText::Downscale);
}

void TextWidget::setText(std::string text)
{
	setTextSilent(text);
	onTextChange();
}

void TextWidget::setText(std::string text, float size)
{
	setTextScale(size);
	setText(text);
}

std::string TextWidget::getText() const
{
	return myText.getText();
}

void TextWidget::setTextSilent(std::string text)
{
	if (myText.getText() != text)
	{
		myText.setText(text);
		invalidateVertices();
		onTextResize();
	}
}

void TextWidget::setTextAlignment(Alignment align)
{
	/*
	if (myTextAlignment != align)
	{
		myTextAlignment = align;
		invalidateVertices();
		onTextResize();
	}
	*/

	sf::Vector2f alignp;

	switch (convertAlignmentHorizontal(align))
	{
	case AlignLeft:
	default:
		alignp.x = 0.f;
		break;

	case AlignCenter:
		alignp.x = 0.5f;
		break;

	case AlignRight:
		alignp.x = 1.f;
		break;
	}

	switch (convertAlignmentVertical(align))
	{
	case AlignTop:
	default:
		alignp.y = 0.f;
		break;

	case AlignCenter:
		alignp.y = 0.5f;
		break;

	case AlignBottom:
		alignp.y = 1.f;
		break;
	}

	setTextPreciseAlignment(alignp);
}

Widget::Alignment TextWidget::getTextAlignment() const
{
	sf::Vector2f alignp = getTextPreciseAlignment() * 3.f;

	Alignment horiz = AlignCenter, vert = AlignCenter;

	if (alignp.x < 1.f)
		horiz = AlignLeft;
	else if (alignp.x < 2.f)
		horiz = AlignCenter;
	else
		horiz = AlignRight;

	if (alignp.y < 1.f)
		vert = AlignTop;
	else if (alignp.y < 2.f)
		vert = AlignCenter;
	else
		vert = AlignBottom;

	return combineAlignments(horiz, vert);
}

void TextWidget::setTextPreciseAlignment(sf::Vector2f align)
{
	if (myTextAlignment != align)
	{
		myTextAlignment = align;
		invalidateVertices();
		onTextResize();
	}
}

void TextWidget::setTextPreciseAlignment(float alignX, float alignY)
{
	setTextPreciseAlignment(sf::Vector2f(alignX, alignY));
}

sf::Vector2f TextWidget::getTextPreciseAlignment() const
{
	return myTextAlignment;
}

void TextWidget::setTextScale(float scale)
{
	if (myText.getScale().x != scale || myText.getScale().y != scale)
	{
		myText.setScale(scale, scale);
		myText.updateMinor();
		invalidateVertices();
		onTextResize();
	}
}

float TextWidget::getTextScale() const
{
	return myText.getScale().x;
}

void TextWidget::setTextColor(sf::Color color)
{
	if (myTextColor != color)
	{
		myTextColor = color;
		invalidateVertices();
		onTextResize();
	}
}

sf::Color TextWidget::getTextColor() const
{
	return myTextColor;
}

void TextWidget::setTextFont(int fontIndex)
{
	if (myFontIndex != fontIndex)
	{
		myFontIndex = fontIndex;
		myText.setFont(getDefaultFont(fontIndex));
		invalidateVertices();
		onTextResize();
	}
}

int TextWidget::getTextFont() const
{
	return myFontIndex;
}

void TextWidget::setTextMaxSizeBehavior(btx::BitmapText::MaxSizeBehavior behavior)
{
	if (myText.getMaxSizeBehavior() != behavior)
	{
		myText.setMaxSizeBehavior(behavior);
		invalidateVertices();
		onTextResize();
	}
}

btx::BitmapText::MaxSizeBehavior TextWidget::getTextMaxSizeBehavior() const
{
	return myText.getMaxSizeBehavior();
}

void TextWidget::setTextClipOffset(sf::Vector2f offset)
{
	if (myText.getClipOffset() != offset)
	{
		myText.setClipOffset(offset);
		invalidateVertices();
		onTextResize();
	}
}

sf::Vector2f TextWidget::getTextClipOffset() const
{
	return myText.getClipOffset();
}

void TextWidget::setTextPixelPerfect(bool perfect)
{
	if (myIsTextPixelPerfect != perfect)
	{
		myIsTextPixelPerfect = perfect;
		invalidateVertices();
		onTextResize();
	}
}

bool TextWidget::isTextPixelPerfect() const
{
	return myIsTextPixelPerfect;
}

sf::Vector2f TextWidget::getTextPosition() const
{
	return myText.getPosition();
}

sf::Vector2f TextWidget::getTextSize() const
{
	return myText.getSize() * myText.getScale();
}

sf::Vector2f TextWidget::getTextUnscaledSize() const
{
	return myText.getUnscaledSize() * myText.getScale();
}

sf::FloatRect TextWidget::getTextRect() const
{
	sf::Vector2f pos = getTextPosition(), size = getTextSize();
	return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

void TextWidget::loadTextFont()
{
	if (!myText.hasFont())
	{
		myText.setFont(getDefaultFont(myFontIndex));

		if (myText.hasFont())
		{
			invalidateVertices();
			onLoadFont();
		}
	}
}

bool TextWidget::isTextFontLoaded() const
{
	return myText.hasFont();
}


sf::FloatRect TextWidget::getTextMaxBoundingBox() const
{
	return getBaseRect();
}

void TextWidget::vertexAddText()
{
	recalculateText();
	vertexAddTextured(myText.getVertices());
}

const btx::BitmapText & TextWidget::getTextObject() const
{
	return myText;
}

void TextWidget::onTextChange()
{

}

void TextWidget::onLoadFont()
{
	onTextResize();
}

void TextWidget::onTextResize()
{

}

void TextWidget::onUpdateFonts()
{
	loadTextFont();
}


void TextWidget::recalculateText()
{
	// perform text initialization.
	loadTextFont();

	myText.setColor(isEnabled() ? getTextColor() : fadeColor(getTextColor(), 0.6f));

	sf::FloatRect boundingBox = getTextMaxBoundingBox();
	sf::Vector2f pos(boundingBox.left, boundingBox.top);

	myText.setMaxSize(sf::Vector2f(boundingBox.width, boundingBox.height));
	sf::Vector2f textSize(getTextSize());

	pos.x += (boundingBox.width - textSize.x) * getTextPreciseAlignment().x;
	pos.y += (boundingBox.height - textSize.y) * getTextPreciseAlignment().y;

	/*
	switch (convertAlignmentHorizontal(getTextAlignment()))
	{
	case AlignLeft:
	default:
		break;

	case AlignCenter:
		pos.x += (boundingBox.width - textSize.x) / 2.f;
		break;

	case AlignRight:
		pos.x += boundingBox.width - textSize.x;
		break;
	}

	switch (convertAlignmentVertical(getTextAlignment()))
	{
	case AlignTop:
	default:
		break;

	case AlignCenter:
		pos.y += (boundingBox.height - textSize.y) / 2.f;
		break;

	case AlignBottom:
		pos.y += boundingBox.height - textSize.y;
		break;
	}
	*/

	if (isTextPixelPerfect())
		pos = sf::Vector2f(sf::Vector2i(pos + sf::Vector2f(0.5f, 0.5f)));

	if (pos != myText.getPosition())
	{
		myText.setPosition(pos);
		myText.updateMinor();
	}
}

} // namespace gui2

