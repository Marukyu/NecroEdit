#include <Client/Editor/ObjectMapperFactory.hpp>
#include <Client/Editor/Tools/Panels/ObjectToolPanel.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <Client/GUI2/Widgets/Menu.hpp>
#include <Client/GUI2/Widgets/Text.hpp>
#include <algorithm>
#include <memory>
#include <string>

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

static const std::vector<std::string> BRUSH_MODE_NAMES = {
	"Ignore",
	"Add",
	"Replace all",
	"Place on empty",
	"Replace front",
	"Place/replace front",
	"Erase all",
	"Erase front" };

Brush::ObjectMode ObjectToolPanel::getPrimaryMode() const
{
	return static_cast<Brush::ObjectMode>(primaryModeMenu->getSelection());
}

Brush::ObjectMode ObjectToolPanel::getSecondaryMode() const
{
	return static_cast<Brush::ObjectMode>(secondaryModeMenu->getSelection());
}

std::string ObjectToolPanel::getTooltip() const
{
	return hoveredObject.getType() != Object::Type::None ? objectAppearance->getObjectName(hoveredObject) : "";
}

void ObjectToolPanel::init()
{
	gui2::Container::init();

	ObjectMapperFactory mapperFactory(*objectAppearance);

	auto modePanel = gui2::GridPanel::make(2, 2);
	modePanel->setMargin(1.f);

	primaryModeMenu = gui2::Dropdown::make();
	primaryModeMenu->setModel(std::make_shared<gui2::DefaultMenuModel>(BRUSH_MODE_NAMES));
	primaryModeMenu->setSelection(static_cast<int>(Brush::ObjectMode::ReplaceAll));
	primaryModeMenu->setVerticalFlip(true);

	secondaryModeMenu = gui2::Dropdown::make();
	secondaryModeMenu->setModel(std::make_shared<gui2::DefaultMenuModel>(BRUSH_MODE_NAMES));
	secondaryModeMenu->setSelection(static_cast<int>(Brush::ObjectMode::EraseAll));
	secondaryModeMenu->setVerticalFlip(true);

	modePanel->add(gui2::Text::make("Left click:"));
	modePanel->add(gui2::Text::make("Right click:"));
	modePanel->add(primaryModeMenu);
	modePanel->add(secondaryModeMenu);

	selectionPanel = SelectionPanel::make();
	selectionPanel->setTexture(objectAppearance->getTexture());
	selectionPanel->setMapper(mapperFactory.generateObjectMapper(this->objects), this->objects.size());

	propertyPanel = ObjectPropertyPanel::make();

	auto borderPanel = gui2::BorderPanel::make();

	borderPanel->add(selectionPanel, gui2::BorderPanel::Center);
	borderPanel->add(propertyPanel, gui2::BorderPanel::Bottom);
	borderPanel->add(modePanel, gui2::BorderPanel::Bottom, 50);

	add(borderPanel);
}

void ObjectToolPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	if (selectionPanel->wasChanged())
	{
		if (selectionPanel->hasSelection())
		{
			// Copy object into selection to allow applying properties without modifying prototype array.
			selectedObject = Object(objects[selectionPanel->getSelection()]);

			// Set property defaults for missing property keys.
			addDefaultPropertiesToObject();

			// Update property panel.
			propertyPanel->setObject(&selectedObject);
			propertyPanel->update();
		}
		else
		{
			// Assign "none" type to mark object as invalid.
			selectedObject.setType(Object::Type::None);

			// Update property panel.
			propertyPanel->setObject(nullptr);
			propertyPanel->update();
		}
	}

	if (selectionPanel->hasHoveredItem())
	{
		hoveredObject = Object(objects[selectionPanel->getHoveredItem()]);
	}
	else
	{
		hoveredObject.setType(Object::Type::None);
	}
}

void ObjectToolPanel::addDefaultPropertiesToObject()
{
	for (const auto & property : Object::getDefaultProperties(selectedObject.getType()))
	{
		if (!selectedObject.hasProperty(property.first))
		{
			selectedObject.setPropertyString(property.first, property.second);
		}
	}
}
