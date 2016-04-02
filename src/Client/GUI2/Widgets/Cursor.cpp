#include "Client/GUI2/Widgets/Cursor.hpp"
#include "Shared/Utils/MiscMath.hpp"

namespace gui2
{

Ptr<Cursor> Cursor::make()
{
	Ptr<Cursor> widget = std::make_shared<Cursor>();
	widget->init();
	return widget;
}

void Cursor::setTextureRect(sf::FloatRect texRect)
{
	if (myTextureRect != texRect)
	{
		myTextureRect = texRect;
		invalidateVertices();
	}
}

sf::FloatRect Cursor::getTextureRect() const
{
	return myTextureRect;
}

void Cursor::setScale(sf::Vector2f scale)
{
	if (myCursorTransform.getScale() != scale)
	{
		myCursorTransform.setScale(scale);
		invalidateVertices();
	}
}

sf::Vector2f Cursor::getScale() const
{
	return myCursorTransform.getScale();
}

void Cursor::setOrigin(sf::Vector2f origin)
{
	if (myCursorTransform.getOrigin() != origin)
	{
		myCursorTransform.setOrigin(origin);
		invalidateVertices();
	}
}
sf::Vector2f Cursor::getOrigin() const
{
	return myCursorTransform.getOrigin();
}

void Cursor::setRotatable(bool rotatable)
{
	if (myIsRotatable != rotatable)
	{
		myIsRotatable = rotatable;
		myCursorTransform.setRotation(getAngle());
		invalidateVertices();
	}
}

bool Cursor::isRotatable() const
{
	return myIsRotatable;
}

void Cursor::setAngle(float angle)
{
	if (myAngle != angle)
	{
		myAngle = angle;
		myCursorTransform.setRotation(angle);
		invalidateVertices();
	}
}

float Cursor::getAngle() const
{
	return myAngle;
}

void Cursor::setRotationOffset(float offset)
{
	myRotationOffset = offset;
}

float Cursor::getRotationOffset() const
{
	return myRotationOffset;
}

int Cursor::getZPosition() const
{
	return 1000;
}

void Cursor::init()
{
	myIsRotatable = false;
	myAngle = 0.f;
	myRotationOffset = 16.f;
	setZPosition(1000);
}

void Cursor::onProcess(const WidgetEvents & events)
{
	if (!isVisible())
		return;

	sf::Vector2f pos = events.mousePosition - getPosition();

	if (myCursorTransform.getPosition() != pos)
	{
		myCursorTransform.setPosition(pos);
		invalidateVertices();
	}

	if (isRotatable())
	{
		sf::Vector2f deltaVec = myBackPosition - myCursorTransform.getPosition();
		sf::Vector2f normVec = vectorLength(deltaVec) < 0.0001f ? sf::Vector2f(1, 0) : deltaVec / vectorLength(deltaVec);
		myBackPosition = myCursorTransform.getPosition() + normVec * getRotationOffset();

		float angle = radToDeg(angleBetween(myBackPosition, myCursorTransform.getPosition())) + getAngle();

		myCursorTransform.setRotation(angle);
		/*
		if (myCursorTransform.getRotation() != angle)
		{
			myCursorTransform.setRotation(angle);
			invalidateVertices();
		}
		*/
	}
}

void Cursor::onUpdateVertexCache()
{
	clearVertices();

	std::vector<sf::Vertex> vertices;

	// pos coords.
	sf::Vector2f tl(myCursorTransform.getTransform().transformPoint(sf::Vector2f(0,0)));
	sf::Vector2f tr(myCursorTransform.getTransform().transformPoint(sf::Vector2f(getTextureRect().width,0)));
	sf::Vector2f br(myCursorTransform.getTransform().transformPoint(sf::Vector2f(getTextureRect().width,getTextureRect().height)));
	sf::Vector2f bl(myCursorTransform.getTransform().transformPoint(sf::Vector2f(0,getTextureRect().height)));

	// tex coords.
	sf::Vector2f ttl(getTextureRect().left, getTextureRect().top);
	sf::Vector2f tbr(getTextureRect().left + getTextureRect().width, getTextureRect().top + getTextureRect().height);
	sf::Vector2f ttr(tbr.x, ttl.y);
	sf::Vector2f tbl(ttl.x, tbr.y);

	// add vertices.
	vertices.push_back(sf::Vertex(tl, sf::Color::White, ttl));
	vertices.push_back(sf::Vertex(tr, sf::Color::White, ttr));
	vertices.push_back(sf::Vertex(bl, sf::Color::White, tbl));
	vertices.push_back(sf::Vertex(tr, sf::Color::White, ttr));
	vertices.push_back(sf::Vertex(br, sf::Color::White, tbr));
	vertices.push_back(sf::Vertex(bl, sf::Color::White, tbl));

	vertexAddTextured(vertices);
}

bool Cursor::checkMouseover(sf::Vector2f pos) const
{
	return false;
}


} // namespace gui2
