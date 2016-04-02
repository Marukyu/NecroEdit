#ifndef UTILITIES_SF_HPP
#define UTILITIES_SF_HPP

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>


/// in-place constructable class to pass vertices to functions taking only sf::Drawable instances (such as drawClipped()).
class VertexWrapper : public sf::Drawable
{
public:

	VertexWrapper() = delete;
	VertexWrapper(const sf::Vertex * vertices, std::size_t vertexCount, sf::PrimitiveType primitiveType = sf::Triangles);

private:

	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	const sf::Vertex * myVertices;
	std::size_t myVertexCount;
	sf::PrimitiveType myPrimitiveType;

};

sf::FloatRect viewToRect(const sf::View & view);
void zoomView(sf::View & view, float x, float y);

sf::Color fadeColor(const sf::Color & color, float alpha_change);
sf::Color operator* (const sf::Color & left, float right);
sf::Color operator/ (const sf::Color & left, float right);

void drawClipped(const sf::Drawable & drawable, sf::RenderTarget & target, sf::RenderStates states, sf::IntRect clipRect);

inline void drawClipped(const sf::Drawable & drawable, sf::RenderTarget & target, sf::RenderStates states, sf::FloatRect clipRect)
{
	drawClipped(drawable, target, states, sf::IntRect(clipRect));
}

void clipVertices(std::vector<sf::Vertex> & vertices, const sf::FloatRect & clipRect);
void clipVertices(std::vector<sf::Vertex> & vertices, std::size_t startPos, std::size_t endPos, const sf::FloatRect & clipRect);
void clipVertices(sf::VertexArray & array, const sf::FloatRect & clipRect);

void appendVertexRect(sf::VertexArray & array, sf::FloatRect rect, sf::Color colorTL, sf::Color colorTR, sf::Color colorBR, sf::Color colorBL);
void appendVertexRect(sf::VertexArray & array, sf::FloatRect rect, sf::Color color);

sf::FloatRect calculateVertexBoundingBox(const sf::Vertex* vertices, std::size_t vertexCount);

void fitVerticesToRectangle(sf::Vertex * vertices, std::size_t vertexCount, const sf::FloatRect & rect);
void fitVerticesToRectangle(std::vector<sf::Vertex> & vertices, const sf::FloatRect & rect);

#endif
