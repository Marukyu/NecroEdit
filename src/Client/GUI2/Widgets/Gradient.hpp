#ifndef GUI2_GRADIENT_HPP
#define GUI2_GRADIENT_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class Gradient : public Widget
{
public:

	/// factory function.
	static Ptr<Gradient> make(sf::Color colorA = sf::Color::Transparent, sf::Color colorB = sf::Color::Transparent);
	
	void setColorA(sf::Color color);
	void setColorB(sf::Color color);

private:

	void onUpdateVertexCache();
	
	sf::Color colorA;
	sf::Color colorB;
};

} // namespace gui2

#endif
