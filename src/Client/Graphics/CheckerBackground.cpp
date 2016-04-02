#include "Client/Graphics/CheckerBackground.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include <cmath>

CheckerBackground::CheckerBackground()
{
	myTexture.create(2, 2);

	std::vector<sf::Uint8> texBuf(myTexture.getSize().x * myTexture.getSize().y);
	for (unsigned int y = 0; y < myTexture.getSize().y; ++y)
		for (unsigned int x = 0; x < myTexture.getSize().x; ++x)
		{
			sf::Uint8 * cur = &texBuf[(y * myTexture.getSize().y + x) * 4];
			*(cur) = *(cur+1) = *(cur+2) = (((x % 2 == 0) ^ (y % 2 == 0)) ? 255 : 200);
			*(cur+3) = 255;
		}

	myTexture.update(texBuf.data());
	myTexture.setRepeated(true);

	myVertices.setPrimitiveType(sf::Triangles);
	myVertices.resize(12);

	for (unsigned int i = 0; i < 6; ++i)
		myVertices[i].color = sf::Color(0, 32, 64, 255);

	for (unsigned int i = 6; i < 12; ++i)
		myVertices[i].color = sf::Color(0, 32, 64, 128);

	myAnim = myAnim2 = 0.f;
}

void CheckerBackground::doStatic()
{
	myVertices[0].position = myVertices[6].position = sf::Vector2f();
	myVertices[1].position = myVertices[7].position = sf::Vector2f(getSize().x,0);
	myVertices[2].position = myVertices[8].position = sf::Vector2f(0,getSize().y);
	myVertices[3].position = myVertices[9].position = sf::Vector2f(getSize().x,0);
	myVertices[4].position = myVertices[10].position = sf::Vector2f(0,getSize().y);
	myVertices[5].position = myVertices[11].position = getSize();

	for (unsigned int i = 0; i < myVertices.getVertexCount(); ++i)
	{
		myVertices[i].texCoords = myVertices[i].position;
	}
}

void CheckerBackground::doAnim()
{
	doStatic();

	float timeFactor = myClock.restart().asSeconds();

	myAnim = std::fmod(myAnim + timeFactor * 0.17f, 2 * 3.14159265f);
	myAnim2 = std::fmod(myAnim2 + timeFactor * 0.07f, 2 * 3.14159265f);

	float animAngleSin = std::sin(myAnim), animAngleCos = std::cos(myAnim);
	float animAngleSin2 = std::sin(myAnim2), animAngleCos2 = std::cos(myAnim2);
	sf::Vector2f animCenter = getSize() / 2.f;

	for (unsigned int i = 0; i < 6; ++i)
	{
		sf::Vector2f p = myVertices[i].position, &q = myVertices[i].texCoords;
		p -= animCenter;
		q.x = p.x * animAngleCos - p.y * animAngleSin;
		q.y = p.x * animAngleSin + p.y * animAngleCos;
		q /= animAngleSin * 2.5f + 10.f;
		q += animCenter;
	}
	for (unsigned int i = 6; i < 12; ++i)
	{
		sf::Vector2f p = myVertices[i].position, &q = myVertices[i].texCoords;
		p -= animCenter;
		q.x = p.x * animAngleCos2 - p.y * animAngleSin2;
		q.y = p.x * animAngleSin2 + p.y * animAngleCos2;
		q /= animAngleSin2 * 1.5f + 12.5f;
		q += animCenter;
	}
}

void CheckerBackground::setSize(sf::Vector2f size)
{
	mySize = size;
}

sf::Vector2f CheckerBackground::getSize() const
{
	return mySize;
}

void CheckerBackground::setRect(sf::FloatRect rect)
{
	setPosition(rect.left, rect.top);
	setSize(sf::Vector2f(rect.width, rect.height));
}

sf::FloatRect CheckerBackground::getRect() const
{
	return sf::FloatRect(getPosition().x, getPosition().y, getSize().x, getSize().y);
}

void CheckerBackground::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = &myTexture;

	target.draw(myVertices, states);
}
