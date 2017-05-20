#include <Client/Editor/Editor.hpp>
#include <Client/Editor/Tool.hpp>
#include <Client/GUI2/Internal/WidgetEvents.hpp>
#include <Client/LevelRenderer/LevelRenderer.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Mouse.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <cmath>

gui2::Ptr<Editor> Editor::make(Dungeon & dungeon, const TileAppearanceManager & tileAppearance,
		const ObjectAppearanceManager & objectAppearance)
{
	gui2::Ptr<Editor> widget = std::make_shared<Editor>(dungeon, tileAppearance, objectAppearance);
	widget->init();
	return widget;
}

Editor::Editor(Dungeon & dungeon, const TileAppearanceManager & tileAppearance, const ObjectAppearanceManager & objectAppearance) :
		dungeon(&dungeon),
		level(nullptr),
		tool(nullptr),
		tileAppearance(&tileAppearance),
		objectAppearance(&objectAppearance),
		levelRenderer(nullptr),
		lastLeftMouseDown(false),
		lastRightMouseDown(false),
		isDragging(false),
		zoomFactor(1.f),
		zoomVelocity(0.f)
{
	tilePreviewRectangle.setOutlineThickness(2);
}

Editor::~Editor()
{
}

void Editor::setLevel(Level * level)
{
	if (this->level != level)
	{
		this->level = level;

		if (level == nullptr)
		{
			levelRenderer = nullptr;
		}
		else
		{
			levelRenderer = makeUnique<LevelRenderer>(*dungeon, *level, *tileAppearance, *objectAppearance);
		}
	}
}

Level * Editor::getLevel() const
{
	return level;
}

void Editor::setTool(Tool * tool)
{
	this->tool = tool;
}

Tool * Editor::getTool() const
{
	return tool;
}

sf::Vector2i Editor::getMousePositionTile() const
{
	return lastMousePosition;
}

void Editor::resetCamera()
{
	float tileSize = TileAppearanceManager::TILE_SIZE;
	viewCenter = getLevel() != nullptr ? sf::Vector2f(getLevel()->getPlayerSpawnPoint()) * tileSize : sf::Vector2f();
	zoomFactor = 1;
	zoomVelocity = 0;
}

bool Editor::isVertexRenderable() const
{
	return false;
}

void Editor::init()
{
	Widget::init();

	setZPosition(-1);
}

sf::Vector2f Editor::convertMousePosition(sf::Vector2f pos) const
{
	return getLevelTransform().getInverse().transformPoint(pos);
}

sf::Transform Editor::getLevelTransform() const
{
	sf::Transform transform;
	transform.translate(sf::Vector2f(sf::Vector2i(getSize() / 2.f)) - viewCenter);
	transform.scale(sf::Vector2f(zoomFactor, zoomFactor));
	return transform;
}

void Editor::onDraw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.transform *= getLevelTransform();

	if (levelRenderer != nullptr)
	{
		target.draw(*levelRenderer, states);
	}

	if (getTool() != nullptr && isMouseOver())
	{
		getTool()->onDrawPreview(target, states, lastMousePosition);
	}
}

void Editor::onProcess(const gui2::WidgetEvents& events)
{
	sf::Vector2f mousePos = convertMousePosition(events.mousePosition);

	bool shiftLeftMouseDown = isMouseDown(sf::Mouse::Left) && events.isShiftPressed();
	bool ctrlLeftMouseDown = isMouseDown(sf::Mouse::Left) && events.isControlPressed();

	bool leftMouseDown = isMouseDown(sf::Mouse::Left) && !shiftLeftMouseDown && !ctrlLeftMouseDown;
	bool rightMouseDown = isMouseDown(sf::Mouse::Right) || shiftLeftMouseDown;
	bool middleMouseDown = isMouseDown(sf::Mouse::Middle) || ctrlLeftMouseDown;

	if (middleMouseDown)
	{
		if (isDragging)
		{
			viewCenter = dragPosition - events.mousePosition;
		}
		else if (events.mouseFocus)
		{
			dragPosition = events.mousePosition + viewCenter;
			isDragging = true;
		}
	}
	else if (isDragging)
	{
		isDragging = false;
	}

	if (events.mouseFocus && events.mouseWheelDelta != 0)
	{
		zoomOrigin = mousePos;

		static constexpr float ZOOM_FACTOR_VELOCITY = 0.05f;
		zoomVelocity += events.mouseWheelDelta * ZOOM_FACTOR_VELOCITY;
	}

	static constexpr float ZOOM_VELOCITY_EPSILON = 0.000001f;
	if (std::abs(zoomVelocity) > ZOOM_VELOCITY_EPSILON)
	{
		zoomFactor *= 1 + zoomVelocity;
		viewCenter -= zoomOrigin * (zoomFactor / (1 + zoomVelocity) - zoomFactor);

		if (isDragging)
		{
			// Recalculate middle click drag position.
			dragPosition = events.mousePosition + viewCenter;
		}

		static constexpr float ZOOM_FACTOR_FRICTION = 0.75f;
		zoomVelocity *= ZOOM_FACTOR_FRICTION;
	}

	mousePos = convertMousePosition(events.mousePosition);

	sf::Vector2i tileAtMousePos = sf::Vector2i(std::floor(mousePos.x / TileAppearanceManager::TILE_SIZE + 0.5f),
		std::floor(mousePos.y / TileAppearanceManager::TILE_SIZE + 0.5f));

	if (getTool() != nullptr)
	{
		if (leftMouseDown && !lastLeftMouseDown)
		{
			getTool()->onMousePress(tileAtMousePos, Tool::MouseButton::Left);
		}

		if (!leftMouseDown && lastLeftMouseDown)
		{
			getTool()->onMouseRelease(tileAtMousePos, Tool::MouseButton::Left);
		}

		if (rightMouseDown && !lastRightMouseDown)
		{
			getTool()->onMousePress(tileAtMousePos, Tool::MouseButton::Right);
		}

		if (!rightMouseDown && lastRightMouseDown)
		{
			getTool()->onMouseRelease(tileAtMousePos, Tool::MouseButton::Right);
		}

		if (leftMouseDown || rightMouseDown)
		{
			getTool()->onMouseDrag(lastMousePosition, tileAtMousePos);
		}
	}

	lastLeftMouseDown = leftMouseDown;
	lastRightMouseDown = rightMouseDown;
	lastMousePosition = tileAtMousePos;

	if (levelRenderer != nullptr)
	{
		levelRenderer->update();
	}
}
