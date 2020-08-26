#include "Client/GUI2/Application.hpp"
#include "Client/GUI2/Widgets/Menu.hpp"
#include "Shared/Utils/Utilities.hpp"
#include "Shared/Utils/MiscMath.hpp"

namespace gui2
{

DefaultMenuModel::DefaultMenuModel()
{

}

DefaultMenuModel::DefaultMenuModel(const std::string & items, const std::string & delimiter, bool emptySeparators)
{
	setItems(items, delimiter, emptySeparators);
}

DefaultMenuModel::DefaultMenuModel(const std::vector<std::string> & items)
{
	setItems(items);
}

void DefaultMenuModel::setItems(const std::string & items, const std::string & delimiter, bool emptySeparators)
{
	std::vector<std::string> itemVec = splitString(items, delimiter);
	setItems(itemVec, emptySeparators);
}

void DefaultMenuModel::setItems(const std::vector<std::string> & items, bool emptySeparators)
{
	clear();

	for (const auto & item : items)
	{
		if (emptySeparators && item.empty())
			addSeparator();
		else
			addItem(item);
	}
}

void DefaultMenuModel::clear()
{
	myItems.clear();

	notify();
}

void DefaultMenuModel::addItem(std::string text, bool enabled, bool checked)
{
	myItems.push_back(ItemData(text, enabled, checked, false));

	notify();
}

void DefaultMenuModel::addSeparator()
{
	myItems.push_back(ItemData("", false, false, true));

	notify();
}

void DefaultMenuModel::insertItem(std::size_t index, std::string text, bool enabled, bool checked)
{
	if (index <= myItems.size())
	{
		myItems.insert(myItems.begin() + index, ItemData(text, enabled, checked, false));
		notify();
	}
}

void DefaultMenuModel::insertSeparator(std::size_t index)
{
	if (index <= myItems.size())
	{
		myItems.insert(myItems.begin() + index, ItemData("", false, false, true));
		notify();
	}
}

void DefaultMenuModel::removeItem(std::size_t index)
{
	if (index < myItems.size())
	{
		myItems.erase(myItems.begin() + index);
		notify();
	}
}

void DefaultMenuModel::setItem(std::size_t index, std::string text)
{
	if (index < myItems.size())
	{
		myItems[index].text = text;
		notify();
	}
}

void DefaultMenuModel::setEnabled(std::size_t index, bool enabled)
{
	if (index < myItems.size())
	{
		myItems[index].enabled = enabled;
		notify();
	}
}

void DefaultMenuModel::setChecked(std::size_t index, bool checked)
{
	if (index < myItems.size())
	{
		myItems[index].checked = checked;
		notify();
	}
}

void DefaultMenuModel::setSeparator(std::size_t index, bool separator)
{
	if (index < myItems.size())
	{
		myItems[index].separator = separator;
		notify();
	}
}

std::size_t DefaultMenuModel::getItemCount() const
{
	return myItems.size();
}

std::string DefaultMenuModel::getItem(std::size_t index) const
{
	if (index < myItems.size())
		return myItems[index].text;
	return "";
}

bool DefaultMenuModel::isEnabled(std::size_t index) const
{
	if (index < myItems.size())
		return myItems[index].enabled;
	return false;
}

bool DefaultMenuModel::isChecked(std::size_t index) const
{
	if (index < myItems.size())
		return myItems[index].checked;
	return false;
}

bool DefaultMenuModel::isSeparator(std::size_t index) const
{
	if (index < myItems.size())
		return myItems[index].separator;
	return false;
}


Ptr<Menu> Menu::make()
{
	Ptr<Menu> widget = std::make_shared<Menu>();
	widget->init();
	return widget;
}

void Menu::show(sf::Vector2f pos)
{
	updateMenu();
	setVisible(true);

	if (isVerticalFlipped())
		setPosition(pos.x, pos.y - getSize().y * getScale().y);
	else
		setPosition(pos);

	myScrollVelocity = 0.f;
	myScrollPosition = 0.f;
	myScrollLimitMax = std::max(-getPosition().y, 0.f);
	// TODO: calculate minimum scroll limit

	acquireFocus();
}

void Menu::show(float x, float y)
{
	show(sf::Vector2f(x, y));
}

int Menu::getSelectedItem() const
{
	return mySelectedItem;
}

void Menu::setModel(std::shared_ptr<AbstractMenuModel> model)
{
	if (myModel)
		myModel->removeObserver(this);

	myModel = model;

	if (myModel)
		myModel->addObserver(this);

	invalidateMenu();
}

std::shared_ptr<AbstractMenuModel> Menu::getModel() const
{
	return myModel;
}

void Menu::setMinimumWidth(float minWidth)
{
	if (myMinimumWidth != minWidth)
	{
		myMinimumWidth = minWidth;
		updateSize();
	}
}

float Menu::getMinimumWidth() const
{
	return myMinimumWidth;
}

void Menu::setVerticalFlip(bool flip)
{
	myIsVerticalFlipped = flip;
}

bool Menu::isVerticalFlipped() const
{
	return myIsVerticalFlipped;
}

void Menu::init()
{
	setModel(std::make_shared<DefaultMenuModel>());
	setZPosition(500);
}

void Menu::onNotify(Observable & subject, int message)
{
	invalidateMenu();
}

const float Menu::cvPadding = 2.f;
const float Menu::cvItemHeight = 18.f;

void Menu::onProcess(const WidgetEvents & events)
{
	if (isVisible())
	{
		updateMenu();

		if (isMouseOver() && events.mouseWheelDelta)
		{
			myScrollVelocity += events.mouseWheelDelta * 10.f;
		}

		if (std::abs(myScrollVelocity) > 0.0001f)
		{
			float oldScrollPosition = myScrollPosition;
			myScrollPosition = clamp(myScrollLimitMin, myScrollPosition + myScrollVelocity, myScrollLimitMax);
			setPosition(getPosition().x, getPosition().y + myScrollPosition - oldScrollPosition);
			myScrollVelocity *= 0.75f;
		}

		int newMouseOverItem = -1;

		if (isMouseOver())
		{
			for (std::size_t i = 0; i < myTexts.size(); ++i)
			{
				float nextY = (i == myTexts.size() - 1) ? getSize().y - cvPadding : myTexts[i + 1]->getPosition().y;
				float curY = myTexts[i]->getPosition().y;
				if (events.mousePosition.y < nextY && events.mousePosition.y >= curY)
				{
					newMouseOverItem = i;
					break;
				}
			}
		}

		if (myMouseOverItem != newMouseOverItem)
		{
			myMouseOverItem = newMouseOverItem;
			invalidateVertices();
		}
	}

	if (isClicked() && myMouseOverItem >= 0 && myModel->isEnabled(myMouseOverItem))
	{
		mySelectedItem = myMouseOverItem;
		myMouseOverItem = -1;
		setVisible(false);
	}

	if (isVisible() && !isFocused())
	{
		mySelectedItem = -1;
		setVisible(false);
	}
}

void Menu::onUpdateVertexCache()
{
	clearVertices();

	BoxStyle backgroundBox;
	backgroundBox.pressed = true;
	backgroundBox.focused = true;
	vertexAddBox(BoxTypeDark, backgroundBox);

	if (myMouseOverItem != -1 && myModel && myModel->isEnabled(myMouseOverItem) && !myModel->isSeparator(myMouseOverItem))
	{
		BoxStyle highlightBox;
		highlightBox.color = sf::Color(125, 175, 225);
		highlightBox.outlineColor = highlightBox.color * sf::Color(200,200,200);
		highlightBox.pressed = isMouseDown();
		vertexAddBox(sf::FloatRect(cvPadding, myTexts[myMouseOverItem]->getPosition().y - cvPadding, getSize().x - cvPadding * 2.f, cvItemHeight + cvPadding), BoxTypeBasic, highlightBox);
	}

	BoxStyle checkBox;
	checkBox.activated = true;

	for (std::size_t i = 0; i < myModel->getItemCount(); ++i)
	{
		if (myModel->isSeparator(i))
		{
			vertexAddRectVGradient(sf::FloatRect(cvPadding * 2.f, myTexts[i]->getPosition().y, getSize().x - cvPadding * 4.f, cvPadding), sf::Color(180,180,180), sf::Color(120,120,120));
		}
		else
		{
			if (myModel->isChecked(i))
			{
				float checkboxSize = 14.f;
				checkBox.hovered = (myMouseOverItem == (int)i);
				vertexAddBox(sf::FloatRect(3.f * cvPadding, myTexts[i]->getPosition().y + (cvItemHeight - checkboxSize - cvPadding) / 2.f, checkboxSize, checkboxSize), BoxTypeBasic, checkBox);
			}

			vertexAddTextured(myTexts[i]->getVertices());
		}
	}
}

void Menu::invalidateMenu()
{
	invalidateVertices();
	myIsMenuInvalid = true;
}

void Menu::updateMenu()
{
	if (myIsMenuInvalid)
	{
		myMenuWidth = 0.f;
		float yPos = cvPadding * 2.f;

		myTexts.resize(myModel->getItemCount());

		for (std::size_t i = 0; i < myModel->getItemCount(); ++i)
		{
			if (!myTexts[i])
			{
				myTexts[i].reset(new btx::BitmapText);
			}

			btx::BitmapText & text = *myTexts[i];

			if (!text.hasFont())
			{
				text.setFont(getDefaultFont(Application::FontDefault));
			}


			text.setPosition(25.f, yPos);

			if (myModel->isSeparator(i))
			{
				text.setText("");
				yPos += cvPadding * 2.f;
			}
			else
			{
				text.setText(myModel->getItem(i));
				yPos += cvItemHeight;
			}

			text.setColor(myModel->isEnabled(i) ? sf::Color::White : sf::Color(128, 128, 128));

			myMenuWidth = std::max(text.getPosition().x + text.getSize().x + cvPadding * 4.f, myMenuWidth);

			yPos += cvPadding;
		}

		setSize(getSize().x, yPos);
		updateSize();

		myIsMenuInvalid = false;
	}
}

void Menu::updateSize()
{
	setSize(std::max(myMenuWidth, myMinimumWidth), getSize().y);
}



} // namespace gui2
