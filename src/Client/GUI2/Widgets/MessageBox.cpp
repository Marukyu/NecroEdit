#include <Client/GUI2/Widgets/MessageBox.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>

namespace gui2
{

static constexpr float EDGE_SPACING = 10.f;
static constexpr float MESSAGE_BUTTON_SPACING = 20.f;
static constexpr float BUTTON_WIDTH = 100.f;
static constexpr float BUTTON_HEIGHT = 24.f;
static constexpr float BUTTON_SPACING = 5.f;

Ptr<MessageBox> MessageBox::make()
{
	Ptr<MessageBox> widget = std::make_shared<MessageBox>();
	widget->init();
	return widget;
}

Ptr<MessageBox> MessageBox::make(std::string message, std::string title, std::vector<std::string> buttons)
{
	Ptr<MessageBox> widget = std::make_shared<MessageBox>();
	widget->init();
	widget->setMessage(message);
	widget->setCaption(title);
	widget->setButtons(buttons);
	return widget;
}

void MessageBox::show()
{
	updateLayout();

	if (getParent() != nullptr)
	{
		setPosition((getParent()->getContainerBoundingBox().width - getSize().x) / 2,
			(getParent()->getContainerBoundingBox().height - getSize().y) / 2);
	}

	setVisible(true);
	sendToFront();
	acquireFocus();

	invalidateVertices();
}

void MessageBox::setMessage(std::string message)
{
	myMessage->setText(message);

	updateLayout();
}

std::string MessageBox::getMessage() const
{
	return myMessage->getText();
}

void MessageBox::setButtons(std::vector<std::string> buttons)
{
	for (auto button : myButtons)
	{
		myButtonPanel->remove(button);
	}

	myButtons.clear();

	std::size_t i = 0;
	for (auto buttonText : buttons)
	{
		auto button = Button::make(std::move(buttonText));
		button->setRect(i * (BUTTON_WIDTH + BUTTON_SPACING), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
		myButtons.push_back(button);
		myButtonPanel->add(button);
		++i;
	}

	myButtonPanel->setSize(myButtons.size() * (BUTTON_WIDTH + BUTTON_SPACING) - BUTTON_SPACING, BUTTON_HEIGHT);

	updateLayout();
}

std::vector<std::string> MessageBox::getButtons() const
{
	std::vector<std::string> buttonTexts;
	for (auto button : myButtons)
	{
		buttonTexts.push_back(button->getText());
	}
	return buttonTexts;
}

bool MessageBox::wasClosed()
{
	if (myWasClosed)
	{
		myWasClosed = false;
		return true;
	}
	else
	{
		return false;
	}
}

int MessageBox::getClickedButton() const
{
	return myClickedButton;
}

void MessageBox::init()
{
	Window::init();

	setVisible(false);

	myMessage = Text::make();
	add(myMessage);

	myButtonPanel = Container::make();
	add(myButtonPanel);

	setFlags(FlagClose | FlagMove);

	myWasClosed = false;
	myClickedButton = -1;
}

void MessageBox::updateLayout()
{
	float width = std::max(myMessage->getTextUnscaledSize().x, myButtonPanel->getSize().x) + 2 * EDGE_SPACING;
	float height = myMessage->getTextUnscaledSize().y + myButtonPanel->getSize().y + MESSAGE_BUTTON_SPACING
		+ 2 * EDGE_SPACING;

	setSize(width, height);
	myMessage->setPosition(EDGE_SPACING, EDGE_SPACING);
	myMessage->setSize(getSize().x - EDGE_SPACING * 2, myMessage->getTextUnscaledSize().y);
	myButtonPanel->setPosition((getSize().x - myButtonPanel->getSize().x) / 2,
		getSize().y - myButtonPanel->getSize().y - EDGE_SPACING);
}

void MessageBox::onProcessWindow(const WidgetEvents& events)
{
	for (std::size_t i = 0; i < myButtons.size(); ++i)
	{
		if (myButtons[i]->isClicked())
		{
			myWasClosed = true;
			myClickedButton = i;
			close();
		}
	}
}

bool MessageBox::onClose()
{
	myWasClosed = true;
	myClickedButton = -1;
	return true;
}

} // namespace gui2
