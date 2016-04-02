#include <Shared/External/TinyFileDialogs/tinyfiledialogs.h>
#include <Shared/Utils/MessageBox.hpp>
#include <algorithm>

MessageBox::MessageBox(std::string message, std::string title, Icon icon, Buttons buttons) :
		message(message),
		title(title),
		icon(icon),
		buttons(buttons)
{
}

MessageBox::~MessageBox()
{
}

bool MessageBox::show()
{
	std::string iconName;

	switch (icon)
	{
	case Info:
	default:
		iconName = "info";
		break;
	case Warning:
		iconName = "warning";
		break;
	case Error:
		iconName = "error";
		break;
	case Question:
		iconName = "question";
		break;
	}

	std::string dialogType;

	switch (buttons)
	{
	case Ok:
	default:
		dialogType = "ok";
		break;
	case OkCancel:
		dialogType = "okcancel";
		break;
	case YesNo:
		dialogType = "yesno";
		break;
	}

	return tinyfd_messageBox(title.c_str(), message.c_str(), dialogType.c_str(), iconName.c_str(), 1);
}

void MessageBox::setMessage(std::string message)
{
	this->message = std::move(message);
}

const std::string& MessageBox::getMessage() const
{
	return message;
}

void MessageBox::setTitle(std::string title)
{
	this->title = std::move(title);
}

const std::string& MessageBox::getTitle() const
{
	return title;
}

void MessageBox::setIcon(Icon icon)
{
	this->icon = icon;
}

MessageBox::Icon MessageBox::getIcon() const
{
	return icon;
}

void MessageBox::setButtons(Buttons buttons)
{
	this->buttons = buttons;
}

MessageBox::Buttons MessageBox::getButtons() const
{
	return buttons;
}
