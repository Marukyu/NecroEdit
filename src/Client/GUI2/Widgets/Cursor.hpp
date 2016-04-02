#ifndef GUI2_CURSOR_HPP
#define GUI2_CURSOR_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class Cursor : public Widget {

public:

	/// factory function.
	static Ptr<Cursor> make();

	void setTextureRect(sf::FloatRect texRect);
	sf::FloatRect getTextureRect() const;

	void setScale(sf::Vector2f scale);
	sf::Vector2f getScale() const;

	void setOrigin(sf::Vector2f origin);
	sf::Vector2f getOrigin() const;

	void setRotatable(bool rotatable);
	bool isRotatable() const;

	void setAngle(float angle);
	float getAngle() const;

	void setRotationOffset(float offset);
	float getRotationOffset() const;

protected:

	int getZPosition() const;

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	bool checkMouseover(sf::Vector2f pos) const;

	sf::Transformable myCursorTransform;
	sf::FloatRect myTextureRect;
	bool myIsRotatable;
	float myAngle, myRotationOffset;
	sf::Vector2f myBackPosition;

};

} // namespace gui2

#endif
