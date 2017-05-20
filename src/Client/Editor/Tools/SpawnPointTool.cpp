#include <Client/Editor/Tools/Panels/SpawnPointToolPanel.hpp>
#include <Client/Editor/Tools/SpawnPointTool.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Client/LevelRenderer/TileAppearance.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <Shared/Level/Dungeon.hpp>
#include <Shared/Level/Level.hpp>
#include <Shared/Level/Object.hpp>
#include <vector>

SpawnPointTool::SpawnPointTool()
{
	previewRectangle.setFillColor(sf::Color(255, 255, 255, 64));
	previewRectangle.setOutlineColor(sf::Color(255, 255, 255, 128));
	previewRectangle.setOutlineThickness(2);
}

SpawnPointTool::~SpawnPointTool()
{
}

void SpawnPointTool::onInit()
{
	toolPanel = SpawnPointToolPanel::make(*getEditorData().dungeon, *getEditorData().objectAppearance);
	onEnable();
}

void SpawnPointTool::onEnable()
{
	toolPanel->setLevel(getEditorData().level);
	toolPanel->update();
}

void SpawnPointTool::onMousePress(sf::Vector2i position, MouseButton button)
{
	if (getEditorData().level)
	{
		getEditorData().level->setPlayerSpawnPoint(position);
	}
}

void SpawnPointTool::onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo)
{
	if (getEditorData().level)
	{
		getEditorData().level->setPlayerSpawnPoint(positionTo);
	}
}

void SpawnPointTool::onDrawPreview(sf::RenderTarget& target, sf::RenderStates states, sf::Vector2i cursorPosition)
{
	// Create spawn point marker object.
	Object spawnPointPreview(Object::Type::Internal);
	spawnPointPreview.setPosition(cursorPosition);
	spawnPointPreview.setPropertyInt(Object::Property::Type, ObjectAppearanceManager::InternalCharacter);
	spawnPointPreview.setPropertyInt(Object::Property::Subtype, getEditorData().dungeon->getPlayerCharacter());

	// Get marker vertices.
	std::vector<sf::Vertex> vertices = getEditorData().objectAppearance->getObjectVertices(spawnPointPreview);

	for (sf::Vertex & vertex : vertices)
	{
		// Make preview translucent.
		vertex.color.a *= 0.75f;
	}

	// Calculate preview rectangle.
	sf::FloatRect previewRect((cursorPosition.x - 0.5f) * TileAppearanceManager::TILE_SIZE,
		(cursorPosition.y - 0.5f) * TileAppearanceManager::TILE_SIZE, TileAppearanceManager::TILE_SIZE,
		TileAppearanceManager::TILE_SIZE);

	previewRectangle.setPosition(previewRect.left, previewRect.top);
	previewRectangle.setSize(sf::Vector2f(previewRect.width, previewRect.height));

	// Render preview rectangle.
	states.texture = nullptr;
	target.draw(previewRectangle, states);

	// Render marker preview.
	states.texture = getEditorData().objectAppearance->getTexture();
	target.draw(vertices.data(), vertices.size(), sf::Triangles, states);
}

gui2::Ptr<gui2::Widget> SpawnPointTool::getSettingsPanel() const
{
	return toolPanel;
}
