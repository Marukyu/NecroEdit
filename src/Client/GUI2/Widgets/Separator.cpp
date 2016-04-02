#include "Client/GUI2/Widgets/Separator.hpp"

namespace gui2
{

Ptr<Separator> Separator::make()
{
	Ptr<Separator> widget = std::make_shared<Separator>();
	widget->init();
	return widget;
}

void Separator::init()
{
	setAutomaticSize(sf::Vector2f(0.f, 8.f));
}

void Separator::onUpdateVertexCache()
{
	clearVertices();
	vertexAddRectVGradient(sf::FloatRect(0.f, (getSize().y - 2.f) / 2.f, getSize().x, 2.f), sf::Color(180,180,180), sf::Color(120,120,120));
}

} // namespace gui2
