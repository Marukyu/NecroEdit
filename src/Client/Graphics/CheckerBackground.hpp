#ifndef CHECKER_BACKGROUND_HPP
#define CHECKER_BACKGROUND_HPP

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Texture.hpp>

class CheckerBackground : public sf::Drawable, public sf::Transformable
{
public:

	CheckerBackground();

	void doStatic();
	void doAnim();

	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;

	void setRect(sf::FloatRect rect);
	sf::FloatRect getRect() const;

private:

	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	sf::Texture myTexture;
	sf::VertexArray myVertices;
	float myAnim, myAnim2;
	sf::Vector2f mySize;

	sf::Clock myClock;
};

#endif
