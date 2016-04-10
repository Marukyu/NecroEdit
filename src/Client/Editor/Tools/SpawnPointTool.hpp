#ifndef SRC_CLIENT_EDITOR_TOOLS_SPAWNPOINTTOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_SPAWNPOINTTOOL_HPP_

#include <Client/Editor/Tool.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>

class SpawnPointTool : public Tool
{
public:
	SpawnPointTool();
	virtual ~SpawnPointTool();

	/**
	 * Moves the player spawn point to the clicked tile.
	 */
	virtual void onMousePress(sf::Vector2i position, MouseButton button) override;

	/**
	 * Moves the player spawn point to the dragged tile.
	 */
	virtual void onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo) override;

	/**
	 * Renders a preview of the player character to the render target.
	 */
	virtual void onDrawPreview(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2i cursorPosition)
			override;

private:

	sf::RectangleShape previewRectangle;
};

#endif
