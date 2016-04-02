#include "Client/GUI2/Widgets/Image.hpp"

using namespace gui2;

Ptr<Image> Image::make()
{
	Ptr<Image> widget = std::make_shared<Image>();
	widget->myTexture = 0;
	return widget;
}

void Image::setTexture(const sf::Texture * tex)
{
	if (myTexture != tex)
	{
		myTexture = tex;
		update();
	}
}

const sf::Texture * Image::getTexture() const
{
	return myTexture;
}

void Image::update()
{
	if (getTexture())
		setAutomaticSize(sf::Vector2f(getTexture()->getSize()));
}

void Image::onDraw(sf::RenderTarget & target, sf::RenderStates states) const
{
	/*
	states.texture = getTexture();

	sf::VertexArray vertices;
	vertices.setPrimitiveType(sf::Triangles);
	appendVertexRect(vertices, getBaseRect(), sf::Color::White);

	target.draw(vertices, states);
	*/

	if (getTexture())
	{
		sf::Sprite spr;
		spr.setPosition(getPosition());
		spr.setTexture(*getTexture(), true);
		spr.setScale(getSize().x / getTexture()->getSize().x, getSize().y / getTexture()->getSize().y);
		target.draw(spr, states);
	}
}

bool Image::isVertexRenderable() const
{
	return false;
}

