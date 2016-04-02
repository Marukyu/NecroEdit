#ifndef GRADIENT_RECT_HPP
#define GRADIENT_RECT_HPP

#include <SFML/Graphics.hpp>

class GradientRect : public sf::Drawable, public sf::Transformable {
public:
	enum Direction {
		Horizontal,
		Vertical
	};

	GradientRect();
	~GradientRect();

	void setRect(sf::FloatRect rect);						   /// set position/size through rectangle.
	sf::FloatRect getRect() const;							  /// get position/size by rectangle.

	void setSize(float w, float h);							 /// set rectangle size.
	void setSize(sf::Vector2f size);							/// set rectangle size.
	sf::Vector2f getSize() const;							   /// get rectangle size.

	void setGradient(sf::Color a, sf::Color b, Direction dir);  /// set gradient properties.
	void setDirection(Direction dir);						   /// set gradient direction.
	void reverse();											 /// swaps the two colors.

	void setColorA(sf::Color color);							/// set top/left color.
	void setColorB(sf::Color color);							/// set bottom/right color.
	void setColors(sf::Color a, sf::Color b);				   /// set both colors at once.

	sf::Color getColorA() const;								/// get top/left color.
	sf::Color getColorB() const;								/// get bottom/right color.

	void setOutline(sf::Color color, float thickness);		  /// set outline properties.
	void setOutlineColor(sf::Color color);					  /// set outline color.
	void setOutlineThickness(float thickness);				  /// set outline thickness.

	void expand(float factor);								  /// apply centric expansion.

	void setSkew(float skew);								   /// skew rectangle in direction of the gradient.

private:
	sf::Vector2f my_size;
	sf::ConvexShape outline;
	sf::VertexArray fill;

	void updateVertexPos();
	void updateVertexColor();

	virtual void draw(sf::RenderTarget & target, sf::RenderStates states = sf::RenderStates::Default) const;

	sf::Color color_a, color_b;
	Direction direction;
	float my_skew;
};

#endif
