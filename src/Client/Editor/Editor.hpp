#ifndef SRC_CLIENT_EDITOR_EDITOR_HPP_
#define SRC_CLIENT_EDITOR_EDITOR_HPP_

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Widget.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <memory>

class Level;
class LevelRenderer;
class Tool;

namespace gui2
{
class WidgetEvents;
}

namespace sf
{
class RenderTarget;
}

/**
 *
 */
class Editor : public gui2::Widget
{
public:

	/**
	 * Factory function.
	 */
	static gui2::Ptr<Editor> make(const TileAppearanceManager & tileAppearance,
			const ObjectAppearanceManager & objectAppearance);

	Editor(const TileAppearanceManager & tileAppearance, const ObjectAppearanceManager & objectAppearance);

	virtual ~Editor();

	void setLevel(Level * level);
	Level * getLevel() const;

	void setTool(Tool * tool);
	Tool * getTool() const;

	sf::Vector2i getMousePositionTile() const;

	void resetCamera();

	bool isVertexRenderable() const override;

private:

	void init() override;

	sf::Transform getLevelTransform() const;
	sf::Vector2f convertMousePosition(sf::Vector2f pos) const;

	void onDraw(sf::RenderTarget & target, sf::RenderStates states) const override;

	void onProcess(const gui2::WidgetEvents & events) override;

	Level * level;
	Tool * tool;

	const TileAppearanceManager * tileAppearance;
	const ObjectAppearanceManager * objectAppearance;

	std::unique_ptr<LevelRenderer> levelRenderer;

	sf::RectangleShape tilePreviewRectangle;
	std::array<sf::Vertex, TileAppearanceManager::VERTEX_COUNT> tilePreviewVertices;

	sf::Vector2f viewCenter;

	bool lastLeftMouseDown, lastRightMouseDown;
	sf::Vector2i lastMousePosition;

	bool isDragging;
	sf::Vector2f dragPosition;

	float zoomFactor;
	float zoomVelocity;
	sf::Vector2f zoomOrigin;
};

#endif
