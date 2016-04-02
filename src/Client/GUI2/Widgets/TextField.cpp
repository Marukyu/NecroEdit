#include <Client/GUI2/Widgets/TextField.hpp>
#include "Shared/Utils/StrNumCon.hpp"
#include "Shared/Utils/Utilities.hpp"
#include "Shared/Utils/Clipboard.hpp"
#include "Client/Graphics/UtilitiesSf.hpp"

namespace gui2
{

const float TextField::cvMargin = 5;

Ptr<TextField> TextField::make()
{
	Ptr<TextField> widget = std::make_shared<TextField>();
	widget->init();
	return widget;
}

Ptr<TextField> TextField::make(std::string text)
{
	Ptr<TextField> widget = make();
	widget->setText(text);
	return widget;
}

void TextField::setCursorPosition(unsigned int pos)
{
	setSelectionStart(pos);
	setSelectionEnd(pos);
}
unsigned int TextField::getCursorPosition() const
{
	return getSelectionEnd();
}

void TextField::setSelectionStart(unsigned int start)
{
	if (myCursorStart != start)
	{
		myCursorStart = start;
		invalidateVertices();
	}
}
unsigned int TextField::getSelectionStart() const
{
	return myCursorStart;
}

void TextField::setSelectionEnd(unsigned int end)
{
	if (myCursorEnd != end)
	{
		myCursorEnd = end;
		invalidateVertices();
	}
}
unsigned int TextField::getSelectionEnd() const
{
	return myCursorEnd;
}

int TextField::getSelectionLength() const
{
	return myCursorEnd - myCursorStart;
}

std::string TextField::getSelectedText() const
{
	unsigned int selStart = getSelectionStart(), selEnd = getCursorPosition();

	if (selStart == selEnd)
		return "";

	if (selStart > selEnd)
		std::swap(selStart, selEnd);

	return getText().substr(selStart, selEnd - selStart);
}

void TextField::setMaxLength(unsigned int maxLength)
{
	myMaxLength = maxLength;
}
unsigned int TextField::getMaxLength() const
{
	return myMaxLength;
}

void TextField::setInputType(InputType type)
{
	myInputType = type;
}

TextField::InputType TextField::getInputType() const
{
	return myInputType;
}

void TextField::setMouseWheelEnabled(bool enabled)
{
	myUseMouseWheel = enabled;
}
bool TextField::isMouseWheelEnabled() const
{
	return myUseMouseWheel;
}

void TextField::setMouseWheelFactor(float factor)
{
	myMouseWheelFactor = factor;
}
float TextField::getMouseWheelFactor() const
{
	return myMouseWheelFactor;
}

void TextField::setReadOnly(bool readOnly)
{
	myIsReadOnly = readOnly;
}
bool TextField::isReadOnly() const
{
	return myIsReadOnly;
}

bool TextField::wasChanged()
{
	if (myWasChanged)
	{
		myWasChanged = false;
		return true;
	}
	else
		return false;
}

bool TextField::isMenuFocused() const
{
	return isFocused();
}


void TextField::init()
{
	TextWidget::init();

	myWasChanged = false;
	setText("", true);
	setTextAlignment(AlignLeft);
	setTextMaxSizeBehavior(btx::BitmapText::Downscale);
	setInputType(InputText);
	setCursorPosition(0);
	setReadOnly(false);
	setMaxLength(-1);
	setMouseWheelEnabled(true);
	setMouseWheelFactor(1.f);
	setTextPixelPerfect(true);
}

sf::FloatRect TextField::getTextMaxBoundingBox() const
{
	return expandRect(TextWidget::getTextMaxBoundingBox(), sf::Vector2f(-4, -2));
}


void TextField::eraseSelection()
{
	unsigned int selStart = getSelectionStart(), selEnd = getSelectionEnd();

	if (selStart == selEnd)
		return;

	if (selStart > selEnd)
		std::swap(selStart, selEnd);

	std::string tempStr = getText();
	tempStr.erase(tempStr.begin() + selStart, tempStr.begin() + selEnd);
	setText(tempStr);
	setCursorPosition(selStart);
}

sf::Vector2f TextField::convertPosText(sf::Vector2f pos) const
{
	return pos - getTextObject().getPosition() + getTextObject().getOrigin();
}

void TextField::onMouseDown(sf::Vector2f pos, Input button)
{
	if (button == Input(sf::Mouse::Left))
		setCursorPosition(getTextObject().findCharacterAt(convertPosText(pos)));
}

void TextField::onProcess(const WidgetEvents & events)
{
	std::string textIn;

	// add text input if control key is not pressed.
	if (!events.isControlPressed())
		textIn = events.textInput;

	if (getSelectionStart() > getText().size())
		setSelectionStart(getText().size());

	if (getSelectionEnd() > getText().size())
		setSelectionEnd(getText().size());

	if (isMousePressing())
	{
		setSelectionEnd(getTextObject().findCharacterAt(convertPosText(events.mousePosition)));
	}

	/*
	if (isMouseOver() && !myMenu.isVisible() && events.mouseButtons & EventInfo::MouseRight)
	{
		myMenu.show(convertPosition(events.mousePos) + getPosition());
	}
	*/

	// number increase/decrease.
	if (isMouseWheelEnabled() && isMouseOver() && events.mouseWheelDelta != 0)
	{
		if (getInputType() == InputInteger)
		{
			int delta = events.mouseWheelDelta;

			if (events.isShiftPressed() && events.isControlPressed())
				delta *= 1000;
			else if (events.isControlPressed())
				delta *= 100;
			else if (events.isShiftPressed())
				delta *= 10;

			setText(cNtoS(cStoI(getText()) - getMouseWheelFactor() * delta));
		}
		if (getInputType() == InputNumeric)
		{
			float delta = events.mouseWheelDelta;

			if (events.isShiftPressed() && events.isControlPressed())
				delta *= 100.f;
			else if (events.isControlPressed())
				delta *= 10.f;
			else if (events.isShiftPressed())
				delta /= 10.f;

			setText(cNtoS(cStoF(getText()) - getMouseWheelFactor() * delta));
		}
	}

	if (events.keyboardFocus && isFocused())
	{
		// text editing.
		for (unsigned int i = 0; i < events.pressedInputs.getKeyCount(); ++i)
		{
			if (events.pressedInputs.get(i).getType() == Input::Keyboard)
			{
				switch (events.pressedInputs.get(i).getCode())
				{
				case sf::Keyboard::Left:
					if (getSelectionEnd() != 0)
						setSelectionEnd(getSelectionEnd()-1);
					if (!events.isShiftPressed())
						setSelectionStart(getSelectionEnd());
					break;

				case sf::Keyboard::Right:
					if (getSelectionEnd() < getText().length())
						setSelectionEnd(getSelectionEnd()+1);
					if (!events.isShiftPressed())
						setSelectionStart(getSelectionEnd());
					break;

				case sf::Keyboard::Up:
					setSelectionEnd(0);
					if (!events.isShiftPressed())
						setSelectionStart(getSelectionEnd());
					break;

				case sf::Keyboard::Down:
					setSelectionEnd(getText().length());
					if (!events.isShiftPressed())
						setSelectionStart(getSelectionEnd());
					break;

				case sf::Keyboard::Delete:
					if (getSelectionLength() != 0)
					{
						eraseSelection();
					}
					else if (getCursorPosition() != getText().length() && !isReadOnly())
					{
						std::string tempStr = getText();
						tempStr.erase(getCursorPosition(), 1);
						setText(tempStr);
					}
					break;

				case sf::Keyboard::BackSpace:
					if (getSelectionLength() != 0)
					{
						eraseSelection();
					}
					else if (getCursorPosition() != 0 && !isReadOnly())
					{
						std::string tempStr = getText();
						tempStr.erase(getCursorPosition()-1, 1);
						setText(tempStr);
						setCursorPosition(getCursorPosition()-1);
					}
					break;

				case sf::Keyboard::C:
					if (events.isControlPressed())
					{
						setClipboardText(getSelectedText());
					}
					break;

				case sf::Keyboard::X:
					if (events.isControlPressed())
					{
						setClipboardText(getSelectedText());
						eraseSelection();
					}
					break;

				case sf::Keyboard::V:
					if (events.isControlPressed())
					{
						textIn += getClipboardText();
					}
					break;

				case sf::Keyboard::A:
					if (events.isControlPressed())
					{
						setSelectionStart(0);
						setSelectionEnd(getText().size());
					}
					break;

				default:
					break;
				}
			}
		}

		// text input.
		for (unsigned int i = 0; i < textIn.size(); ++i)
		{
			if ((unsigned char)textIn[i] >= 16 && textIn[i] != 127 && getText().length() < getMaxLength() && !isReadOnly())
			{
				// discard invalid characters.
				static std::string intChars = "0123456789-";
				static std::string floatChars = "0123456789-.";
				if (getInputType() == InputInteger && floatChars.find(textIn[i]) == std::string::npos)
					break;
				if (getInputType() == InputNumeric && intChars.find(textIn[i]) == std::string::npos)
					break;

				eraseSelection();
				std::string tempStr = getText();
				tempStr.insert(getCursorPosition(), 1, textIn[i]);
				setText(tempStr);
				setCursorPosition(getCursorPosition()+1);
			}
		}
	}

	//getTextObject().setMaxSize(getSize() - sf::Vector2f(cvMargin*2.f,0));

	/*
	// enable text zooming with alt.
	if (isMouseOver() && (events.keyModifiers & EventInfo::KeyModAlt))
	{
		myText.setMaxSize(sf::Vector2f(-1,-1));
		myText.setPosition(myText.getPosition().x + (getSize().x - cvMargin*2.f - myText.getWidth() * myText.getScale().x) * (convertPosition(events.mousePos).x / (getSize().x - cvMargin*2.f)), myText.getPosition().y);
	}
	*/

	/*
	myInputBox = generateBox(getBaseRect(), BoxDark);

	unsigned int selBoxStart = getSelectionStart(), selBoxEnd = getSelectionEnd();

	if (selBoxEnd < selBoxStart)
		std::swap(selBoxStart, selBoxEnd);

	std::vector<sf::FloatRect> curRects = getTextObject().getCursorRectMulti(selBoxStart, selBoxEnd - selBoxStart);
	myTextCursors.resize(curRects.size());

	for (std::size_t i = 0; i < curRects.size(); ++i)
	{
		myTextCursors[i].setRect( moveRect(curRects[i], getTextObject().getPosition()) );
		myTextCursors[i].setOrigin(getTextObject().getOrigin() * getTextObject().getScale());
		myTextCursors[i].setGradient(fadeColor(getTextObject().getCharacterColor(getCursorPosition()), 0.3),
								fadeColor(getTextObject().getCharacterColorB(getCursorPosition()), 0.3), GradientRect::Vertical);
		myTextCursors[i].setOutline(fadeColor(getTextObject().getCharacterColorB(getCursorPosition()), 0.75), 1.0);
	}
	*/
}

void TextField::onUpdateVertexCache()
{
	/*
	//states.transform *= getTransform();

	target.draw(myInputBox, states);
	drawText(target, states);
	if (isFocused() || myMenu.isVisible())
	{
		for (std::size_t i = 0; i < myTextCursors.size(); ++i)
			target.draw(myTextCursors[i], states);
	}
	*/

	clearVertices();

	BoxStyle backgroundBox;
	backgroundBox.pressed = true;
	backgroundBox.focused = isFocused();
	vertexAddBox(BoxTypeBackground, backgroundBox);

	if (isFocused())
	{
		unsigned int selBoxStart = getSelectionStart(), selBoxEnd = getSelectionEnd();
		if (selBoxEnd < selBoxStart)
			std::swap(selBoxStart, selBoxEnd);

		BoxStyle cursorBox;
		cursorBox.borderThickness = 1.f;

		std::vector<sf::FloatRect> cursorRects = getTextObject().getCursorRectMulti(selBoxStart, selBoxEnd - selBoxStart);

		for (std::size_t i = 0; i < cursorRects.size(); ++i)
		{
			cursorBox.color = fadeColor(getTextObject().getCharacterColor(getCursorPosition()), 0.5f);
			cursorBox.outlineColor = fadeColor(getTextObject().getCharacterColorB(getCursorPosition()), 1.f);
			vertexAddBox(moveRect(cursorRects[i], getTextObject().getPosition()), BoxTypeBackground, cursorBox);
		}
	}

	vertexAddText();
}

void TextField::onTextChange()
{
	myWasChanged = true;
	invalidateVertices();
}

} // namespace gui2
