#include <Client/Editor/Tools/BrushTool.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Shared/Editor/Brush.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Level/Tile.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <array>

BrushTool::BrushTool()
{
	drawState = DrawNone;
	previewRectangle.setFillColor(sf::Color(255, 255, 255, 64));
	previewRectangle.setOutlineColor(sf::Color(255, 255, 255, 128));
	previewRectangle.setOutlineThickness(2);
}

BrushTool::~BrushTool()
{
}

void BrushTool::onMousePress(sf::Vector2i position, MouseButton button)
{
	switch (drawState)
	{
	case DrawNone:
		if (button == MouseButton::Left)
		{
			drawState = DrawPrimary;
		}
		else if (button == MouseButton::Right)
		{
			drawState = DrawSecondary;
		}
		break;

	case DrawPrimary:
		if (button == MouseButton::Right)
		{
			drawState = DrawCancelled;
		}
		break;

	case DrawSecondary:
		if (button == MouseButton::Left)
		{
			drawState = DrawCancelled;
		}
		break;

	default:
		break;
	}

	Level * level = getEditorData().level;

	if (level != nullptr && drawState != DrawCancelled && drawState != DrawNone)
	{
		level->applyBrush(position, getCurrentBrush());
	}
}

void BrushTool::onMouseRelease(sf::Vector2i position, MouseButton button)
{
	if (button == MouseButton::Left || button == MouseButton::Right)
	{
		drawState = DrawNone;
	}
}

void BrushTool::onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo)
{
	Level * level = getEditorData().level;

	if (level != nullptr && drawState != DrawCancelled && drawState != DrawNone)
	{
		const Brush & currentBrush = getCurrentBrush();

		plotBresenham(positionFrom.x, positionFrom.y, positionTo.x, positionTo.y, [=](int x, int y)
		{
			// Do not draw directly on last brush position to prevent line overdraw with additive brushes.
			if (x != positionFrom.x || y != positionFrom.y)
			{
				level->applyBrush(sf::Vector2i(x, y), currentBrush);
			}
		});
	}
}

void BrushTool::onDrawPreview(sf::RenderTarget& target, sf::RenderStates states, sf::Vector2i cursorPosition)
{
	if (getEditorData().tileAppearance == nullptr)
	{
		return;
	}

	previewVertices.clear();

	const Brush & primaryBrush = getPrimaryBrush();

	if (primaryBrush.getTileMode() != Brush::TileMode::Ignore)
	{
		auto tilePreviewVertices = getEditorData().tileAppearance->getTileVertices(primaryBrush.getTile(),
			cursorPosition);

		for (sf::Vertex & vertex : tilePreviewVertices)
		{
			// Make preview translucent.
			vertex.color.a *= 0.75f;

			// Add vertex to list.
			previewVertices.push_back(std::move(vertex));
		}
	}

	if (primaryBrush.getObjectMode() != Brush::ObjectMode::Ignore)
	{
		auto objectPreviewVertices = getEditorData().objectAppearance->getObjectVertices(primaryBrush.getObject());

		for (sf::Vertex & vertex : objectPreviewVertices)
		{
			// Offset object position (at origin) by cursor location.
			vertex.position += sf::Vector2f(cursorPosition) * TileAppearanceManager::TILE_SIZE;

			// Make preview translucent.
			vertex.color.a *= 0.75f;

			// Add vertex to list.
			previewVertices.push_back(std::move(vertex));
		}
	}

	sf::FloatRect previewRect((cursorPosition.x - 0.5f) * TileAppearanceManager::TILE_SIZE,
		(cursorPosition.y - 0.5f) * TileAppearanceManager::TILE_SIZE, TileAppearanceManager::TILE_SIZE,
		TileAppearanceManager::TILE_SIZE);

	previewRectangle.setPosition(previewRect.left, previewRect.top);
	previewRectangle.setSize(sf::Vector2f(previewRect.width, previewRect.height));

	states.texture = getEditorData().tileAppearance->getTexture();
	target.draw(previewVertices.data(), previewVertices.size(), sf::Triangles, states);

	states.texture = nullptr;
	target.draw(previewRectangle, states);
}

const Brush& BrushTool::getPrimaryBrush() const
{
	static Brush defaultBrush = Brush();
	return defaultBrush;
}

const Brush& BrushTool::getSecondaryBrush() const
{
	static Brush defaultBrush = Brush();
	return defaultBrush;
}

const Brush& BrushTool::getCurrentBrush() const
{
	return (drawState == DrawPrimary ? getPrimaryBrush() : getSecondaryBrush());
}
