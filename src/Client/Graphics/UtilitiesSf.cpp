#include "Client/Graphics/UtilitiesSf.hpp"
#include "Shared/Utils/Utilities.hpp"
#include "Shared/Utils/MiscMath.hpp"
#include "Shared/Utils/VectorMul.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cstring>

VertexWrapper::VertexWrapper(const sf::Vertex * vertices, std::size_t vertexCount, sf::PrimitiveType primitiveType) :
		myVertices(vertices), myVertexCount(vertexCount), myPrimitiveType(primitiveType)
{
}

void VertexWrapper::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(myVertices, myVertexCount, myPrimitiveType, states);
}

sf::FloatRect viewToRect(const sf::View & view)
{
	return sf::FloatRect(view.getCenter().x - view.getSize().x / 2.0, view.getCenter().y - view.getSize().y / 2.0,
			view.getSize().x, view.getSize().y);
}

void zoomView(sf::View & view, float x, float y)
{
	view.setSize(sf::Vector2f(view.getSize().x * x, view.getSize().y * y));
}

sf::Color fadeColor(const sf::Color & color, float alpha_change)
{
	return sf::Color(color.r, color.g, color.b, color.a * alpha_change);
}

sf::Color operator*(const sf::Color & left, float right)
{
	return sf::Color(left.r * right, left.g * right, left.b * right, left.a * right);
}

sf::Color operator/(const sf::Color & left, float right)
{
	return sf::Color(left.r / right, left.g / right, left.b / right);
}

void drawClipped(const sf::Drawable & drawable, sf::RenderTarget & target, sf::RenderStates states,
		sf::IntRect clipRect)
{
	/*
	 //clipRect = expandRect(clipRect, sf::Vector2f(.5f,.5f));

	 sf::View oldView = target.getView(), clipView;

	 sf::FloatRect origRect = viewToRect(oldView),
	 origViewPort = oldView.getViewport();

	 origRect.width  /= origViewPort.width;
	 origRect.height /= origViewPort.height;

	 sf::FloatRect origClipRect (origRect.width * origViewPort.left,
	 origRect.height * origViewPort.top,
	 origRect.width * origViewPort.width,
	 origRect.height * origViewPort.height);

	 if (!origClipRect.intersects(clipRect))
	 return;

	 clipRect = rectIntersect(clipRect, origClipRect);

	 clipView.reset(clipRect);
	 clipView.setViewport(sf::FloatRect( (clipRect.left - origRect.left) / origRect.width + origViewPort.left,
	 (clipRect.top - origRect.top) / origRect.height + origViewPort.top,
	 (clipRect.width / origRect.width),
	 (clipRect.height / origRect.height) ));

	 target.setView(clipView);
	 target.draw(drawable, states);

	 target.setView(oldView);
	 */

	// TODO: Remove glIsEnabled and glGet calls in favor of manually keeping track of state.
	// check for recursion.
	if (glIsEnabled(GL_SCISSOR_TEST))
	{
		// grab original scissor box.
		GLint scRect[4];
		glGetIntegerv(GL_SCISSOR_BOX, scRect);

		sf::IntRect origClipRect(scRect[0], ((int) target.getSize().y) - scRect[1] - scRect[3], scRect[2], scRect[3]);

		// check if anything has to be drawn at all.
		if (!origClipRect.intersects(clipRect))
			return;

		// calculate new intersection rectangle.
		clipRect = rectIntersect(clipRect, origClipRect);

		// draw.
		glScissor(clipRect.left, ((int) target.getSize().y) - clipRect.top - clipRect.height, clipRect.width,
				clipRect.height);
		target.draw(drawable, states);
		glScissor(scRect[0], scRect[1], scRect[2], scRect[3]);
	}
	else
	{
		// apply scissors, draw, disable scissors.
		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect.left, ((int) target.getSize().y) - clipRect.top - clipRect.height, clipRect.width,
				clipRect.height);
		target.draw(drawable, states);
		glDisable(GL_SCISSOR_TEST);
	}
}

namespace priv
{

// intersects two lines.
inline bool getLineLineIntersection(sf::Vector2f a1, sf::Vector2f a2, sf::Vector2f b1, sf::Vector2f b2,
		sf::Vector2f & intersection)
{
	sf::Vector2f b = a2 - a1;
	sf::Vector2f d = b2 - b1;
	float crossProd = b.x * d.y - b.y * d.x;

	if (crossProd == 0.f)
		return false;

	sf::Vector2f c = b1 - a1;
	float t = (c.x * d.y - c.y * d.x) / crossProd;
	if (t < 0.f || t > 1.f)
		return false;

	float u = (c.x * b.y - c.y * b.x) / crossProd;
	if (u < 0.f || u > 1.f)
		return false;

	intersection = a1 + t * b;

	return true;
}

// intersects a line with a horizontal line.
inline bool getLineHLineIntersection(sf::Vector2f a1, sf::Vector2f a2, sf::Vector2f b1, float width,
		sf::Vector2f & intersection)
{
	float crossProd = (a1.y - a2.y) * width;

	if (crossProd == 0.f)
		return false;

	float t = ((a1.y - b1.y) * width) / crossProd;
	if (t < 0.f || t > 1.f)
		return false;

	float u = ((b1.x - a1.x) * (a2.y - a1.y) + (a1.y - b1.y) * (a2.x - a1.x)) / crossProd;
	if (u < 0.f || u > 1.f)
		return false;

	intersection = a1 + t * (a2 - a1);

	return true;
}

// intersects a line with a vertical line.
inline bool getLineVLineIntersection(sf::Vector2f a1, sf::Vector2f a2, sf::Vector2f b1, float height,
		sf::Vector2f & intersection)
{
	float crossProd = (a2.x - a1.x) * height;

	if (crossProd == 0.f)
		return false;

	float t = ((b1.x - a1.x) * height) / crossProd;
	if (t < 0.f || t > 1.f)
		return false;

	float u = ((b1.x - a1.x) * (a2.y - a1.y) + (a1.y - b1.y) * (a2.x - a1.x)) / crossProd;
	if (u < 0.f || u > 1.f)
		return false;

	intersection = a1 + t * (a2 - a1);

	return true;
}

// intersects a line with an axis-aligned rectangle, returning the intersection points.
inline void getLineRectIntersection(sf::Vector2f a1, sf::Vector2f a2, const sf::FloatRect & rect,
		std::vector<sf::Vector2f> & intersections)
{
	sf::Vector2f b1(rect.left, rect.top);
	sf::Vector2f intersect;

	if (getLineHLineIntersection(a1, a2, b1, rect.width, intersect))
		intersections.push_back(intersect);

	if (getLineHLineIntersection(a1, a2, b1 + sf::Vector2f(0, rect.height), rect.width, intersect))
		intersections.push_back(intersect);

	if (getLineVLineIntersection(a1, a2, b1, rect.height, intersect))
		intersections.push_back(intersect);

	if (getLineVLineIntersection(a1, a2, b1 + sf::Vector2f(rect.width, 0), rect.height, intersect))
		intersections.push_back(intersect);
}

// projects a point onto a line and performs a reverse linear interpolation.
inline float projectPointOntoLine(sf::Vector2f p, sf::Vector2f a1, sf::Vector2f a2)
{
	a2 -= a1;
	return dotProduct(p - a1, a2) / dotProduct(a2, a2);
}

// sorts vertices clockwise.
class VertexArranger
{
public:

	VertexArranger(const sf::Vertex & pivot) :
			myPivot(pivot)
	{
	}

	inline bool operator()(const sf::Vertex & va, const sf::Vertex & vb)
	{
		const sf::Vector2f & pivot = myPivot.position;
		sf::Vector2f a = va.position - pivot;
		sf::Vector2f b = vb.position - pivot;

		float cross = a.x * b.y - b.x * a.y;
		if (cross < 0)
			return true;
		if (cross > 0)
			return false;

		// in case of same angle, differentiate by distance.
		float d1 = a.x * a.x + a.y * a.y;
		float d2 = b.x * b.x + b.y * b.y;
		return d1 > d2;
	}

private:

	const sf::Vertex & myPivot;
};

// clips the vertex triangle to the specified rectangle and returns a convex polygon of the clipped triangle.
// returns false if the triangle is already clipped, or true if the triangle has been clipped to the rectangle.
bool clipVertexTriangle(const sf::Vertex & v1, const sf::Vertex & v2, const sf::Vertex & v3, const sf::FloatRect & rect,
		std::vector<sf::Vertex> & newVertices)
{
	float rect_right = rect.left + rect.width, rect_bottom = rect.top + rect.height;

	// vertices beyond edge of rectangle?
	bool v1l = v1.position.x < rect.left,	v2l = v2.position.x < rect.left,	v3l = v3.position.x < rect.left;
	bool v1r = v1.position.x > rect_right,	v2r = v2.position.x > rect_right,	v3r = v3.position.x > rect_right;
	bool v1t = v1.position.y < rect.top,	v2t = v2.position.y < rect.top,		v3t = v3.position.y < rect.top;
	bool v1b = v1.position.y > rect_bottom,	v2b = v2.position.y > rect_bottom,	v3b = v3.position.y > rect_bottom;

	// vertices inside of rectangle?
	bool v1in = !(v1l || v1r || v1t || v1b);
	bool v2in = !(v2l || v2r || v2t || v2b);
	bool v3in = !(v3l || v3r || v3t || v3b);

	// all vertices within clipping rectangle, nothing to do.
	if (v1in && v2in && v3in)
		return false;

	// all vertices beyond one of the rectangle's edges, "erase" vertices.
	if ((v1l && v2l && v3l) || (v1r && v2r && v3r) || (v1t && v2t && v3t) || (v1b && v2b && v3b))
		return true;

	// copy all input vertices that lie inside the rectangle.
	if (v1in)
		newVertices.push_back(v1);
	if (v2in)
		newVertices.push_back(v2);
	if (v3in)
		newVertices.push_back(v3);

	// intersection point vector.
	std::vector<sf::Vector2f> intersections;

	// intersect line v1-v2.
	if (!(v1in && v2in))
	{
		getLineRectIntersection(v1.position, v2.position, rect, intersections);
		for (sf::Vector2f intersect : intersections)
		{
			float proj = projectPointOntoLine(intersect, v1.position, v2.position); // this should always be between 0.0 and 1.0.
			float invProj = 1 - proj;
			newVertices.push_back(
					sf::Vertex(intersect, v1.color * invProj + v2.color * proj,
							v1.texCoords * invProj + v2.texCoords * proj));
		}
		intersections.clear();
	}

	// intersect line v2-v3.
	if (!(v2in && v3in))
	{
		getLineRectIntersection(v2.position, v3.position, rect, intersections);
		for (sf::Vector2f intersect : intersections)
		{
			float proj = projectPointOntoLine(intersect, v2.position, v3.position);
			float invProj = 1 - proj;
			newVertices.push_back(
					sf::Vertex(intersect, v2.color * invProj + v3.color * proj,
							v2.texCoords * invProj + v3.texCoords * proj));
		}
		intersections.clear();
	}

	// intersect line v1-v3.
	if (!(v1in && v3in))
	{
		getLineRectIntersection(v1.position, v3.position, rect, intersections);
		for (sf::Vector2f intersect : intersections)
		{
			float proj = projectPointOntoLine(intersect, v1.position, v3.position);
			float invProj = 1 - proj;
			newVertices.push_back(
					sf::Vertex(intersect, v1.color * invProj + v3.color * proj,
							v1.texCoords * invProj + v3.texCoords * proj));
		}
		intersections.clear();
	}

	// check if triangle contains rectangle's top left corner.
	if (v1l || v2l || v3l || v1t || v2t || v3t)
	{
		float b1, b2, b3;
		sf::Vector2f corner = sf::Vector2f(rect.left, rect.top);
		if (getBarycentricCoords(corner, v1.position, v2.position, v3.position, b1, b2, b3)
				&& !(b1 < 0.f || b1 > 1.f || b2 < 0.f || b2 > 1.f || b3 < 0.f || b3 > 1.f))
		{
			newVertices.push_back(
					sf::Vertex(corner, v1.color * b1 + v2.color * b2 + v3.color * b3,
							v1.texCoords * b1 + v2.texCoords * b2 + v3.texCoords * b3));
		}
	}

	// check if triangle contains rectangle's top right corner.
	if (v1r || v2r || v3r || v1t || v2t || v3t)
	{
		float b1, b2, b3;
		sf::Vector2f corner = sf::Vector2f(rect_right, rect.top);
		if (getBarycentricCoords(corner, v1.position, v2.position, v3.position, b1, b2, b3)
				&& !(b1 < 0.f || b1 > 1.f || b2 < 0.f || b2 > 1.f || b3 < 0.f || b3 > 1.f))
		{
			newVertices.push_back(
					sf::Vertex(corner, v1.color * b1 + v2.color * b2 + v3.color * b3,
							v1.texCoords * b1 + v2.texCoords * b2 + v3.texCoords * b3));
		}
	}

	// check if triangle contains rectangle's bottom right corner.
	if (v1r || v2r || v3r || v1b || v2b || v3b)
	{
		float b1, b2, b3;
		sf::Vector2f corner = sf::Vector2f(rect_right, rect_bottom);
		if (getBarycentricCoords(corner, v1.position, v2.position, v3.position, b1, b2, b3)
				&& !(b1 < 0.f || b1 > 1.f || b2 < 0.f || b2 > 1.f || b3 < 0.f || b3 > 1.f))
		{
			newVertices.push_back(
					sf::Vertex(corner, v1.color * b1 + v2.color * b2 + v3.color * b3,
							v1.texCoords * b1 + v2.texCoords * b2 + v3.texCoords * b3));
		}
	}

	// check if triangle contains rectangle's bottom left corner.
	if (v1l || v2l || v3l || v1b || v2b || v3b)
	{
		float b1, b2, b3;
		sf::Vector2f corner = sf::Vector2f(rect.left, rect_bottom);
		if (getBarycentricCoords(corner, v1.position, v2.position, v3.position, b1, b2, b3)
				&& !(b1 < 0.f || b1 > 1.f || b2 < 0.f || b2 > 1.f || b3 < 0.f || b3 > 1.f))
		{
			newVertices.push_back(
					sf::Vertex(corner, v1.color * b1 + v2.color * b2 + v3.color * b3,
							v1.texCoords * b1 + v2.texCoords * b2 + v3.texCoords * b3));
		}
	}

	// now sort vertices clockwise.
	if (!newVertices.empty())
		std::sort(newVertices.begin() + 1, newVertices.end(), VertexArranger(newVertices[0]));

	return true;
}
}

void clipVertices(std::vector<sf::Vertex> & vertices, const sf::FloatRect & clipRect)
{
	clipVertices(vertices, 0, vertices.size(), clipRect);
}

void clipVertices(std::vector<sf::Vertex> & vertices, std::size_t startPos, std::size_t endPos,
		const sf::FloatRect & clipRect)
{
	std::vector<sf::Vertex> newVertices;

	for (std::size_t i = startPos; i < endPos;)
	{
		// create polygon from triangle-rectangle intersection.
		if (priv::clipVertexTriangle(vertices[i], vertices[i + 1], vertices[i + 2], clipRect, newVertices))
		{
			// no or too few vertices returned? remove this triangle.
			if (newVertices.size() < 3)
			{
				vertices.erase(vertices.begin() + i, vertices.begin() + i + 3);
				endPos -= 3;
			}
			else
			{
				// insert new vertices for triangles.
				std::size_t insCount = (newVertices.size() - 3) * 3;

				if (insCount != 0)
					vertices.insert(vertices.begin() + i, insCount, sf::Vertex());
				endPos += insCount;

				// otherwise, convert polygon to triangles.
				for (std::size_t j = 1; j < newVertices.size() - 1; ++j)
				{
					const sf::Vertex & v1 = newVertices[0], &v2 = newVertices[j], &v3 = newVertices[j + 1];

					vertices[i] = v1;
					vertices[i + 1] = v2;
					vertices[i + 2] = v3;
					i += 3;
				}
			}
			newVertices.clear();
		}
		else
		{
			// skip vertices, leave them as they are.
			i += 3;
		}
	}
}

void clipVertices(sf::VertexArray & array, const sf::FloatRect & clipRect)
{
	std::vector<sf::Vertex> vertVec(array.getVertexCount());
	std::memcpy(&vertVec[0], &array[0], array.getVertexCount() * sizeof(sf::Vertex));

	clipVertices(vertVec, clipRect);

	array.resize(vertVec.size());
	std::memcpy(&array[0], &vertVec[0], vertVec.size() * sizeof(sf::Vertex));
}

void appendVertexRect(sf::VertexArray & array, sf::FloatRect rect, sf::Color colorTL, sf::Color colorTR,
		sf::Color colorBR, sf::Color colorBL)
{
	sf::Vertex vTL(sf::Vector2f(rect.left, rect.top), colorTL), vTR(sf::Vector2f(rect.left + rect.width, rect.top),
			colorTR), vBR(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), colorBR), vBL(
			sf::Vector2f(rect.left, rect.top + rect.height), colorBL);

	if (array.getPrimitiveType() == sf::Triangles)
	{
		array.append(vTL);
		array.append(vTR);
		array.append(vBL);
		array.append(vTR);
		array.append(vBR);
		array.append(vBL);
	}
	else if (array.getPrimitiveType() == sf::TrianglesStrip)
	{
		array.append(vTR);
		array.append(vTL);
		array.append(vBR);
		array.append(vBL);
	}
	else if (array.getPrimitiveType() == sf::Quads)
	{
		array.append(vTL);
		array.append(vBL);
		array.append(vBR);
		array.append(vTR);
	}
	else if (array.getPrimitiveType() == sf::Lines)
	{
		array.append(vTL);
		array.append(vBL);
		array.append(vBL);
		array.append(vBR);
		array.append(vBR);
		array.append(vTR);
		array.append(vTR);
		array.append(vTL);
	}
	else if (array.getPrimitiveType() == sf::LinesStrip)
	{
		array.append(vTL);
		array.append(vBL);
		array.append(vBR);
		array.append(vTR);
		array.append(vTL);
	}
}

void appendVertexRect(sf::VertexArray & array, sf::FloatRect rect, sf::Color color)
{
	return appendVertexRect(array, rect, color, color, color, color);
}

sf::FloatRect calculateVertexBoundingBox(const sf::Vertex* vertices, std::size_t vertexCount)
{
	bool needInitialValues = true;
	float left = 0, right = 0, top = 0, bottom = 0;
	
	for (std::size_t i = 0; i < vertexCount; ++i)
	{
		sf::Vector2f pos = vertices[i].position;
		
		if (needInitialValues || pos.x < left)
		{
			left = pos.x;
		}
		
		if (needInitialValues || pos.x > right)
		{
			right = pos.x;
		}
		
		if (needInitialValues || pos.y < top)
		{
			top = pos.y;
		}
		
		if (needInitialValues || pos.y > bottom)
		{
			bottom = pos.y;
		}
		
		needInitialValues = false;
	}
	
	return sf::FloatRect(left, top, right - left, bottom - top);
}

namespace priv
{

sf::FloatRect fitAspectRatio(sf::Vector2f innerSize, sf::FloatRect outerRect)
{
	sf::Vector2f outerSize(outerRect.width, outerRect.height);

	if (innerSize.x == 0.f || innerSize.y == 0.f || outerSize.x == 0.f || outerSize.y == 0.f)
		return sf::FloatRect();

	float outerRatio = outerSize.x / outerSize.y;
	float innerRatio = innerSize.x / innerSize.y;

	if (outerRatio > innerRatio)
	{
		// Outer is wider than inner.
		innerSize = sf::Vector2f(outerSize.y * innerRatio, outerSize.y);
	}
	else
	{
		// Outer is taller than inner.
		innerSize = sf::Vector2f(outerSize.x, outerSize.x / innerRatio);
	}

	sf::Vector2f innerPos(sf::Vector2f(outerSize - innerSize) / 2.f);
	sf::Vector2f outerPos(outerRect.left, outerRect.top);
	
	return sf::FloatRect(outerPos + innerPos, innerSize);
}

}

void fitVerticesToRectangle(sf::Vertex* vertices, std::size_t vertexCount, const sf::FloatRect& rect)
{
	sf::FloatRect boundingBox = calculateVertexBoundingBox(vertices, vertexCount);
	sf::FloatRect fitBox = priv::fitAspectRatio(sf::Vector2f(boundingBox.width, boundingBox.height), rect);

	sf::Vector2f preTranslation(-boundingBox.left, -boundingBox.top);
	sf::Vector2f scaling(fitBox.width / boundingBox.width, fitBox.height / boundingBox.height);
	sf::Vector2f postTranslation(fitBox.left, fitBox.top);
	
	for (std::size_t i = 0; i < vertexCount; ++i)
	{
		vertices[i].position += preTranslation;
		vertices[i].position *= scaling;
		vertices[i].position += postTranslation;
	}
}

void fitVerticesToRectangle(std::vector<sf::Vertex>& vertices, const sf::FloatRect& rect)
{
	fitVerticesToRectangle(vertices.data(), vertices.size(), rect);
}
