#include <Client/Editor/ObjectMapperFactory.hpp>
#include <Client/Editor/Tools/Panels/ObjectToolPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <algorithm>
#include <memory>

gui2::Ptr<ObjectToolPanel> ObjectToolPanel::make(std::vector<Object> objects,
		const ObjectAppearanceManager & objectAppearance)
{
	gui2::Ptr<ObjectToolPanel> widget = std::make_shared<ObjectToolPanel>(objects, objectAppearance);
	widget->init();
	return widget;
}

ObjectToolPanel::ObjectToolPanel(std::vector<Object> objects, const ObjectAppearanceManager & objectAppearance) :
		objectAppearance(&objectAppearance)
{
	this->objects = std::move(objects);
}

ObjectToolPanel::~ObjectToolPanel()
{
}

const Object& ObjectToolPanel::getSelectedObject() const
{
	return selectedObject;
}

void ObjectToolPanel::init()
{
	gui2::Container::init();
	
	ObjectMapperFactory mapperFactory(*objectAppearance);
	
	panel = SelectionPanel::make();
	panel->setTexture(objectAppearance->getTexture());
	auto grid = gui2::GridPanel::make(panel);
	add(grid);
	panel->setMapper(mapperFactory.generateObjectMapper(this->objects), this->objects.size());
}

void ObjectToolPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	if (panel->wasChanged())
	{
		if (panel->hasSelection())
		{
			// Copy object into selection to allow applying properties without modifying prototype array.
			selectedObject = Object(objects[panel->getSelection()]);
		}
		else
		{
			// Assign "none" type to mark object as invalid.
			selectedObject.setType(Object::Type::None);
		}
	}
}
