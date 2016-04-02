#include <Client/Editor/SelectionPanel.hpp>
#include <Client/Graphics/UtilitiesSf.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Mouse.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <memory>

namespace gui2
{
class WidgetEvents;
}

static constexpr float MARGIN = 2.f;
static constexpr float SLIDER_WIDTH = 16.f;

gui2::Ptr<SelectionPanel> SelectionPanel::make()
{
	gui2::Ptr<SelectionPanel> widget = std::make_shared<SelectionPanel>();
	widget->init();
	return widget;
}

SelectionPanel::SelectionPanel() :
		mapper([](ID id)
		{
			static ItemEntry invalidEntry;
			return invalidEntry;
		}),
		texture(nullptr),
		wasSelectionChanged(false),
		selectionExists(false),
		selection(0),
		itemCount(0),
		itemSize(24.f, 24.f),
		scrollVelocity(0)
{
}

void SelectionPanel::init()
{
	slider = gui2::Slider::make();
	link(slider);
	slider->setVertical(true);
	update();
}

void SelectionPanel::setSelection(ID selection)
{
	this->selection = selection;
	this->selectionExists = true;
}

void SelectionPanel::clearSelection()
{
	this->selectionExists = false;
}

SelectionPanel::ID SelectionPanel::getSelection() const
{
	return selection;
}

bool SelectionPanel::hasSelection() const
{
	return selectionExists;
}

void SelectionPanel::setMapper(Mapper mapper, std::size_t itemCount)
{
	this->mapper = mapper;
	this->itemCount = itemCount;
	update();
}

std::size_t SelectionPanel::getItemCount() const
{
	return itemCount;
}

const SelectionPanel::Mapper & SelectionPanel::getMapper() const
{
	return mapper;
}

void SelectionPanel::setItemSize(sf::Vector2f itemSize)
{
	this->itemSize = itemSize;
	update();
}

sf::Vector2f SelectionPanel::getItemSize() const
{
	return itemSize;
}

void SelectionPanel::update()
{
	setAutomaticSize(
		sf::Vector2f(getColumnCount() * (getItemSize().x + MARGIN) + MARGIN,
			getRowCount() * (getItemSize().y + MARGIN) + MARGIN));

	slider->setRect(getSize().x - SLIDER_WIDTH, 0, SLIDER_WIDTH, getSize().y);

	if (getAutomaticSize().y > getSize().y)
	{
		slider->setEnabled(true);
		slider->setBounds(0, getAutomaticSize().y - getSize().y);
	}
	else
	{
		slider->setEnabled(false);
		slider->setValue(0);
	}

	vertices.clear();

	std::vector<sf::Vertex> itemVertices;

	for (ID i = 0; i < getItemCount(); ++i)
	{
		itemVertices = mapper(i).vertices;
		fitVerticesToRectangle(itemVertices, getItemRect(i));
		vertices.insert(vertices.end(), itemVertices.begin(), itemVertices.end());
	}
}

bool SelectionPanel::isVertexRenderable() const
{
	return false;
}

void SelectionPanel::onProcess(const gui2::WidgetEvents& events)
{
	if (slider->isEnabled())
	{
		if ((isMouseOver() || slider->isMouseOver()) && events.mouseWheelDelta != 0)
		{
			scrollVelocity -= events.mouseWheelDelta * 5;
		}
		
		if (std::abs(scrollVelocity) > 0.0001f)
		{
			float newSliderValue = slider->getValue() + scrollVelocity;
			if (newSliderValue > slider->getMax())
			{
				newSliderValue = slider->getMax();
				scrollVelocity = 0.f;
			}
			else if (newSliderValue < slider->getMin())
			{
				newSliderValue = slider->getMin();
				scrollVelocity = 0.f;
			}
			slider->setValue(newSliderValue);
			scrollVelocity *= 0.75f;
		}
	}
}

void SelectionPanel::onDraw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	
	sf::Transform origTransform = states.transform;
	
	states.transform.translate(0, -slider->getValue());
	states.texture = this->texture;

	if (!vertices.empty())
	{
		drawClipped(VertexWrapper(&vertices[0], vertices.size(), sf::Triangles), target, states,
			origTransform.transformRect(getBaseRect()));
	}

	if (hasSelection())
	{
		states.texture = nullptr;

		sf::FloatRect selectRect = getItemRect(getSelection());
		sf::RectangleShape selectBox;
		selectBox.setPosition(sf::Vector2f(selectRect.left, selectRect.top));
		selectBox.setSize(sf::Vector2f(selectRect.width, selectRect.height));
		selectBox.setFillColor(sf::Color(255, 255, 255, 48));
		selectBox.setOutlineColor(sf::Color(255, 255, 255, 220));
		selectBox.setOutlineThickness(2.f);
		drawClipped(selectBox, target, states, origTransform.transformRect(getBaseRect()));
	}
}

void SelectionPanel::onResize()
{
	update();
}

std::size_t SelectionPanel::getRowCount() const
{
	return (getItemCount() + getColumnCount() - 1) / getColumnCount();
}

std::size_t SelectionPanel::getColumnCount() const
{
	return std::max<std::size_t>(1, (getSize().x - MARGIN - SLIDER_WIDTH) / (getItemSize().x + MARGIN));
}

void SelectionPanel::setTexture(const sf::Texture* texture)
{
	this->texture = texture;
}

const sf::Texture* SelectionPanel::getTexture() const
{
	return texture;
}

void SelectionPanel::onMouseDown(sf::Vector2f pos, Input button)
{
	if (button == sf::Mouse::Left)
	{
		for (ID i = 0; i < getItemCount(); ++i)
		{
			if (getShiftedItemRect(i).contains(pos))
			{
				if (!hasSelection() || getSelection() != i)
				{
					setSelection(i);
					wasSelectionChanged = true;
				}
				break;
			}
		}
	}
}

sf::FloatRect SelectionPanel::getItemRect(ID item) const
{
	std::size_t indexX = item % getColumnCount();
	std::size_t indexY = item / getColumnCount();

	sf::FloatRect rect(indexX * (getItemSize().x + MARGIN) + MARGIN, indexY * (getItemSize().y + MARGIN) + MARGIN,
		getItemSize().x, getItemSize().y);

	return rect;
}

sf::FloatRect SelectionPanel::getShiftedItemRect(ID item) const
{
	return moveRect(getItemRect(item), sf::Vector2f(0, -slider->getValue()));
}

bool SelectionPanel::wasChanged()
{
	if (wasSelectionChanged)
	{
		wasSelectionChanged = false;
		return true;
	}
	return false;
}
