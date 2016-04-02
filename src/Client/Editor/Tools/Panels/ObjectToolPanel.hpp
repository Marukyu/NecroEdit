#ifndef SRC_CLIENT_EDITOR_TOOLS_PANELS_OBJECTTOOLPANEL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_PANELS_OBJECTTOOLPANEL_HPP_

#include <Client/Editor/SelectionPanel.hpp>
#include <Client/GUI2/Container.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Shared/Level/Object.hpp>
#include <vector>

/**
 * Configuration panel for object brush tools.
 */
class ObjectToolPanel : public gui2::Container
{
public:

	static gui2::Ptr<ObjectToolPanel> make(std::vector<Object> objects,
			const ObjectAppearanceManager & objectAppearance);

	ObjectToolPanel(std::vector<Object> objects, const ObjectAppearanceManager & objectAppearance);
	virtual ~ObjectToolPanel();

	/**
	 * Returns a reference to the currently selected object.
	 */
	const Object & getSelectedObject() const;

protected:
	
	virtual void init() override;

private:
	
	void onProcessContainer(gui2::WidgetEvents & events) override;

	const ObjectAppearanceManager * objectAppearance;

	std::vector<Object> objects;
	Object selectedObject;
	gui2::Ptr<SelectionPanel> panel;
};

#endif
