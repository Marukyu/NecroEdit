#include "Client/GUI2/Widgets/Gradient.hpp"

namespace gui2
{

Ptr<Gradient> Gradient::make(sf::Color colorA, sf::Color colorB)
{
	Ptr<Gradient> widget = std::make_shared<Gradient>();
	widget->init();
	widget->setColorA(colorA);
	widget->setColorB(colorB);
	widget->setZPosition(-1);
	return widget;
}

void Gradient::setColorA(sf::Color color)
{
	colorA = color;
	invalidateVertices();
}

void Gradient::setColorB(sf::Color color)
{
	colorB = color;
	invalidateVertices();
}

void Gradient::onUpdateVertexCache()
{
	clearVertices();
	vertexAddRectVGradient(sf::FloatRect(0.f, 0.f, getSize().x, getSize().y), colorA, colorB);
}

} // namespace gui2
