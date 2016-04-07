#include <Client/GUI2/Application.hpp>
#include <Client/GUI2/Container.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Widgets/Dropdown.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/MiscMath.hpp>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>

namespace gui2
{
class WidgetEvents;
}

namespace gui2
{

Ptr<Dropdown> Dropdown::make()
{
	Ptr<Dropdown> widget = std::make_shared<Dropdown>();
	widget->init();
	return widget;
}

void Dropdown::setText(std::string text)
{
	if (text != myInput->getText())
	{
		myInput->setText(text);
		updateSelection();
	}
}

void Dropdown::setTextSilent(std::string text)
{
	if (text != myInput->getText())
	{
		myInput->setTextSilent(text);
		updateSelection();
	}
}

std::string Dropdown::getText() const
{
	return myInput->getText();
}

void Dropdown::setSelection(int item)
{
	if (!myModel || item < 0 || item >= (int) myModel->getItemCount())
		setText("");
	else
		setText(myModel->getItem(item));
}

void Dropdown::setSelectionSilent(int item)
{
	if (!myModel || item < 0 || item >= (int) myModel->getItemCount())
		setTextSilent("");
	else
		setTextSilent(myModel->getItem(item));
}

int Dropdown::getSelection() const
{
	if (myDropdownModel)
		return myDropdownModel->getCheckboxIndex();
	else
		return -1;
}

void Dropdown::setModel(std::shared_ptr<AbstractMenuModel> model)
{
	if (myModel)
	{
		myModel->removeObserver(this);
	}

	myModel = model;

	if (myModel)
	{
		myDropdownModel = std::make_shared<DropdownModel>(myModel);
		myModel->addObserver(this);
	}
	else
	{
		myDropdownModel = nullptr;
	}

	myMenu->setModel(myDropdownModel);
}

std::shared_ptr<AbstractMenuModel> Dropdown::getModel() const
{
	return myModel;
}

void Dropdown::setVerticalFlip(bool flip)
{
	myMenu->setVerticalFlip(flip);
	myButton->setText(flip ? "1" : "0");
}

bool Dropdown::isVerticalFlipped() const
{
	return myMenu->isVerticalFlipped();
}

bool Dropdown::wasChanged()
{
	if (myWasChanged)
	{
		myWasChanged = false;
		return true;
	}
	else
		return false;
}

void Dropdown::init()
{
	myInput = TextField::make();
	myButton = Button::make();
	myMenu = Menu::make();

	link(myInput);
	link(myButton);

	myButton->setTextFont(Application::FontSymbolSmall);

	myWasChanged = false;
	setVerticalFlip(false);
}

Dropdown::DropdownModel::DropdownModel(std::shared_ptr<AbstractMenuModel> model)
{
	myCheckboxIndex = -1;
	myModel = model;
}

std::size_t Dropdown::DropdownModel::getItemCount() const
{
	return myModel->getItemCount();
}

std::string Dropdown::DropdownModel::getItem(std::size_t index) const
{
	return myModel->getItem(index);
}

bool Dropdown::DropdownModel::isEnabled(std::size_t index) const
{
	return myModel->isEnabled(index);
}

bool Dropdown::DropdownModel::isChecked(std::size_t index) const
{
	return getCheckboxIndex() >= 0 && (int) index == getCheckboxIndex();
}

bool Dropdown::DropdownModel::isSeparator(std::size_t index) const
{
	return myModel->isSeparator(index);
}

void Dropdown::DropdownModel::setCheckboxIndex(int index)
{
	if (myCheckboxIndex != index)
	{
		myCheckboxIndex = index;
		notify();
	}
}

int Dropdown::DropdownModel::getCheckboxIndex() const
{
	return myCheckboxIndex;
}

void Dropdown::onNotify(Observable & subject, int message)
{
	if (myDropdownModel)
		myDropdownModel->notify();

	updateSelection();
}

void Dropdown::onResize()
{
	float buttonWidth = getSize().y * 0.8f;

	myInput->setSize(getSize().x - buttonWidth + 2.f, getSize().y);
	myButton->setPosition(getSize().x - buttonWidth, 0.f);
	myButton->setSize(buttonWidth, getSize().y);
	myMenu->setMinimumWidth(getSize().x);
}

void Dropdown::onProcess(const WidgetEvents & events)
{
	if (myInput->wasChanged())
	{
		myWasChanged = true;
		updateSelection();
	}

	if (myButton->isClicked())
	{
		// Define unit lines for scale factor calculation.
		sf::Vector2f point00(0, 0), point10(1, 0), point01(0, 1);

		// Calculate initial position.
		sf::Vector2f menuPosition(0, isVerticalFlipped() ? 2.f : getSize().y - 2.f);

		// Put points into array to transform them all at once.
		std::array<std::reference_wrapper<sf::Vector2f>, 4> pointsToTransform = { point00, point10, point01,
				menuPosition };

		// Offset local position by dropdown position.
		menuPosition += getPosition();

		// Start at dropdown menu's parent.
		Ptr<Container> parent = getParent();

		// Keep track of last valid parent to add menu to.
		Ptr<Container> lastValidParent;

		// Traverse parent hierarchy.
		while (parent != nullptr)
		{
			sf::Transform transform = parent->getTransform() * parent->getContainerTransform();
			for (sf::Vector2f & point : pointsToTransform)
			{
				point = transform.transformPoint(point);
			}
			lastValidParent = parent;
			parent = parent->getParent();
		}

		// Calculate size from unit lines.
		sf::Vector2f scale(vectorLength(point10 - point00), vectorLength(point01 - point00));

		// Check if parent is valid.
		if (lastValidParent)
		{
			// Assign scale factor to menu.
			myMenu->setScale(scale);

			// Set menu's parent to topmost container.
			lastValidParent->add(myMenu);

			// Show at final position.
			myMenu->show(menuPosition);
		}
	}

	if (myMenu->isClicked())
	{
		setSelection(myMenu->getSelectedItem());
	}
}

void Dropdown::onUpdateVertexCache()
{
	// nothing to do.
}

void Dropdown::updateSelection()
{
	if (!myModel)
		return;

	int selection = -1;
	for (std::size_t i = 0; i < myModel->getItemCount(); ++i)
	{
		if (myModel->getItem(i) == myInput->getText())
		{
			selection = i;
			break;
		}
	}

	myDropdownModel->setCheckboxIndex(selection);
}

} // namespace gui2
