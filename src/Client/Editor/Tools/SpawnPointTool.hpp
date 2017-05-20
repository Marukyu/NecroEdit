#ifndef SRC_CLIENT_EDITOR_TOOLS_SPAWNPOINTTOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_SPAWNPOINTTOOL_HPP_

#include <Client/Editor/Tool.hpp>
#include <Client/Editor/Tools/Panels/SpawnPointToolPanel.hpp>
#include <Client/GUI2/GUI.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>

class SpawnPointTool : public Tool
{
public:
	SpawnPointTool();
	virtual ~SpawnPointTool();

	virtual void onInit() override;
	virtual void onEnable() override;

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

	/**
	 * Returns a panel for changing the dungeon's player character and toggling starting items.
	 */
	virtual gui2::Ptr<gui2::Widget> getSettingsPanel() const override;

private:

	sf::RectangleShape previewRectangle;
	gui2::Ptr<SpawnPointToolPanel> toolPanel;
};

#endif
