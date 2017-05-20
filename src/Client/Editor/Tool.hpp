#ifndef SRC_CLIENT_EDITOR_TOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOL_HPP_

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Widget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/NamedFactory.hpp>
#include <string>

class Dungeon;
class Level;
class ObjectAppearanceManager;
class TileAppearanceManager;
namespace sf
{
class RenderTarget;
}

/**
 * Base class for all editor tools.
 */
class Tool : public NamedFactory<Tool>
{
public:

	/**
	 * Contains pointers to various Editor-related instances.
	 */
	struct EditorData
	{
		TileAppearanceManager * tileAppearance;
		ObjectAppearanceManager * objectAppearance;
		Level * level;
		Dungeon * dungeon;
	};

	/**
	 * Enumeration for possible mouse button events that can be processed by a tool.
	 */
	enum class MouseButton
	{
		Left,
		Right
	};

	/**
	 * Default Constructor. EditorData is uninitialized at this point. Use onInit() for initializations.
	 */
	Tool();

	/**
	 * Default Destructor.
	 */
	virtual ~Tool();

	/**
	 * Called after the tool is instantiated, before any other virtual function is called. At this point, EditorData has
	 * been assigned.
	 */
	virtual void onInit();

	/**
	 * Called when the tool is selected by the user. At this point, the settings panel has been parented to the editor
	 * GUI already.
	 */
	virtual void onEnable();

	/**
	 * Called when another tool is selected by the user. At this point, the settings panel is still parented to the
	 * editor GUI.
	 */
	virtual void onDisable();

	/**
	 * Called when a mouse button is pressed on the editor's level panel.
	 */
	virtual void onMousePress(sf::Vector2i position, MouseButton button);

	/**
	 * Called when a mouse button is released from the editor's level panel.
	 */
	virtual void onMouseRelease(sf::Vector2i position, MouseButton button);

	/**
	 * Called when the mouse cursor is moved on the editor's level panel while any mouse button is held down.
	 */
	virtual void onMouseDrag(sf::Vector2i positionFrom, sf::Vector2i positionTo);

	/**
	 * Called every frame as long as the mouse cursor is on the level editor.
	 */
	virtual void onDrawPreview(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2i cursorPosition);

	/**
	 * Called every frame to determine the current tooltip/status string to be displayed in the editor.
	 */
	virtual std::string getTooltip() const;

	/**
	 * Returns the settings panel associated with this tool. This has to be a consistent and unique instance; the return
	 * value must not change between calls.
	 */
	virtual gui2::Ptr<gui2::Widget> getSettingsPanel() const;

	/**
	 * Assigns the editor information for this tool.
	 */
	void setEditorData(EditorData editorData);

	/**
	 * Returns the editor information given to this tool.
	 */
	const EditorData & getEditorData() const;

	/**
	 * Initializes the Named Factory of tools, if it is not initialized yet.
	 */
	static void initializeToolFactory();

private:

	EditorData editorData;
};

#endif
