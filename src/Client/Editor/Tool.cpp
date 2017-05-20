#include <Client/Editor/Tool.hpp>
#include <Client/Editor/Tools/ObjectTool.hpp>
#include <Client/Editor/Tools/SpawnPointTool.hpp>
#include <Client/Editor/Tools/TileTool.hpp>
#include <Shared/Level/Object.hpp>
#include <algorithm>

Tool::Tool()
{
	EditorData editorData;
	editorData.tileAppearance = nullptr;
	editorData.objectAppearance = nullptr;
	editorData.level = nullptr;
	editorData.dungeon = nullptr;
	this->editorData = editorData;
}

Tool::~Tool()
{
}

void Tool::onInit()
{
}

void Tool::onEnable()
{
}

void Tool::onDisable()
{
}

void Tool::onMousePress(sf::Vector2i position, MouseButton button)
{
}

void Tool::onMouseRelease(sf::Vector2i position, MouseButton button)
{
}

void Tool::onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo)
{
}

void Tool::onDrawPreview(sf::RenderTarget& target, sf::RenderStates states, sf::Vector2i cursorPosition)
{
}

std::string Tool::getTooltip() const
{
	return "";
}

gui2::Ptr<gui2::Widget> Tool::getSettingsPanel() const
{
	return nullptr;
}

void Tool::setEditorData(EditorData editorData)
{
	this->editorData = std::move(editorData);
}

const Tool::EditorData& Tool::getEditorData() const
{
	return editorData;
}

void Tool::initializeToolFactory()
{
	Tool::registrate<TileTool>("place_tile");
	Tool::registrateFunction("place_enemy", []()
	{
		return new ObjectTool(
				{	Object::Type::Enemy});
	});
	Tool::registrateFunction("place_item", []()
	{
		return new ObjectTool(
				{	Object::Type::Item});
	});
	Tool::registrateFunction("place_misc", []()
	{
		return new ObjectTool(
				{	Object::Type::Crate, Object::Type::Chest, Object::Type::Trap, Object::Type::Shrine});
	});
	Tool::registrate<SpawnPointTool>("set_player_spawn");
}
