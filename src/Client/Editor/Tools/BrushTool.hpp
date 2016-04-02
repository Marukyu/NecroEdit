#ifndef SRC_CLIENT_EDITOR_TOOLS_BRUSHTOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_BRUSHTOOL_HPP_

#include <Client/Editor/Tool.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class Brush;

class BrushTool : public Tool
{
public:
	BrushTool();
	virtual ~BrushTool();

	/**
	 * Applies the brush to the clicked tile. Primary brush is applied on left click, secondary brush on right click.
	 */
	virtual void onMousePress(sf::Vector2i position, MouseButton button) override;

	/**
	 * Stops applying the brush.
	 */
	virtual void onMouseRelease(sf::Vector2i position, MouseButton button) override;

	/**
	 * Applies the brush to all newly dragged-over tiles using the Bresenham line algorithm.
	 */
	virtual void onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo) override;

	/**
	 * Renders the primary brush's preview to the editor render target.
	 */
	virtual void onDrawPreview(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2i cursorPosition)
			override;

protected:

	/**
	 * Returns the primary brush (used for left mouse button).
	 */
	virtual const Brush & getPrimaryBrush() const;

	/**
	 * Returns the secondary brush (used for right mouse button).
	 */
	virtual const Brush & getSecondaryBrush() const;

private:

	/**
	 * Returns the current brush according to the draw state.
	 */
	const Brush & getCurrentBrush() const;

	enum DrawState
	{
		DrawNone,          // No mouse button held, so not drawing.
		DrawPrimary,       // Left mouse button held, drawing primary.
		DrawSecondary,     // Right mouse button held, drawing secondary.
		DrawCancelled      // Both mouse buttons pressed, drawing cancelled until all buttons released.
	};

	DrawState drawState;

	sf::RectangleShape previewRectangle;
	std::vector<sf::Vertex> previewVertices;
};

#endif
