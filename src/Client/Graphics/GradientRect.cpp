#include "Client/Graphics/GradientRect.hpp"

GradientRect::GradientRect()
{
	fill.resize(4);
	fill.setPrimitiveType(sf::TrianglesStrip);
	outline.setPointCount(4);
	outline.setFillColor(sf::Color(0,0,0,0));
	outline.setPosition(0, 0);
	setDirection(Vertical);
	my_skew = 0;
}

GradientRect::~GradientRect()
{

}

void GradientRect::setRect(sf::FloatRect rect)
{
	setSize(rect.width, rect.height);
	setPosition(rect.left, rect.top);
	updateVertexPos();
}

sf::FloatRect GradientRect::getRect() const
{
	return sf::FloatRect(getPosition().x, getPosition().y, my_size.x, my_size.y);
}

void GradientRect::setGradient(sf::Color a, sf::Color b, Direction dir)
{
	color_a = a;
	color_b = b;
	direction = dir;
	updateVertexColor();
}

void GradientRect::setOutline(sf::Color color, float thickness)
{
	outline.setOutlineColor(color);
	outline.setOutlineThickness(thickness);
}

void GradientRect::setSize(float w, float h)
{
	setSize(sf::Vector2f(w,h));
}

sf::Vector2f GradientRect::getSize() const
{
	return my_size;
}

void GradientRect::setSize(sf::Vector2f size)
{
	my_size = size;
	updateVertexPos();
}

void GradientRect::reverse()
{
	const sf::Color temp = color_a;
	color_a = color_b;
	color_b = temp;
	updateVertexColor();
}
void GradientRect::setColorA(sf::Color color)
{
	color_a = color;
	updateVertexColor();
}
void GradientRect::setColorB(sf::Color color)
{
	color_b = color;
	updateVertexColor();
}
void GradientRect::setColors(sf::Color a, sf::Color b)
{
	color_a = a;
	color_b = b;
	updateVertexColor();
}

sf::Color GradientRect::getColorA() const
{
	return color_a;
}
sf::Color GradientRect::getColorB() const
{
	return color_b;
}

void GradientRect::setOutlineColor(sf::Color color)
{
	outline.setOutlineColor(color);
}
void GradientRect::setOutlineThickness(float thickness)
{
	outline.setOutlineThickness(thickness);
}

void GradientRect::setDirection(Direction dir)
{
	direction = dir;
	updateVertexColor();
}

void GradientRect::expand(float factor)
{
	sf::FloatRect new_rect = getRect();

	new_rect.left -= getRect().width * (factor-1);
	new_rect.top -= getRect().height * (factor-1);
	new_rect.width += getRect().width * (factor-1) * 2.0;
	new_rect.height += getRect().height * (factor-1) * 2.0;

	setRect(new_rect);
}

void GradientRect::setSkew(float skew)
{
	my_skew = skew;
	updateVertexPos();
}

void GradientRect::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(fill, states);
	/* if (outline.getOutlineThickness() != 0) */ target.draw(outline, states);
}


void GradientRect::updateVertexPos()
{
	fill[0].position = sf::Vector2f(0, 0);
	fill[1].position = sf::Vector2f(my_size.x, 0);
	fill[2].position = sf::Vector2f(0, my_size.y);
	fill[3].position = sf::Vector2f(my_size.x, my_size.y);
	if (my_skew != 0.0)
	{
		if (direction == Vertical)
		{
			fill[0].position += sf::Vector2f(my_skew, 0);
			fill[1].position += sf::Vector2f(my_skew, 0);
			fill[2].position -= sf::Vector2f(my_skew, 0);
			fill[3].position -= sf::Vector2f(my_skew, 0);
		}
		else if (direction == Horizontal) {
			fill[0].position += sf::Vector2f(0, my_skew);
			fill[1].position -= sf::Vector2f(0, my_skew);
			fill[2].position += sf::Vector2f(0, my_skew);
			fill[3].position -= sf::Vector2f(0, my_skew);
		}
	}

	outline.setPoint(0, fill[0].position);
	outline.setPoint(1, fill[1].position);
	outline.setPoint(2, fill[3].position);
	outline.setPoint(3, fill[2].position);
}

void GradientRect::updateVertexColor()
{
	fill[0].color = color_a;
	fill[3].color = color_b;

	if (direction == Horizontal) {
		fill[1].color = color_b;
		fill[2].color = color_a;
	}
	else if (direction == Vertical) {
		fill[1].color = color_a;
		fill[2].color = color_b;
	}
}
