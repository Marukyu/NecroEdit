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
	"Erase front"
};

Brush::ObjectMode ObjectToolPanel::getPrimaryMode() const
{
	return static_cast<Brush::ObjectMode>(primaryModeMenu->getSelection());
}

Brush::ObjectMode ObjectToolPanel::getSecondaryMode() const
{
	return static_cast<Brush::ObjectMode>(secondaryModeMenu->getSelection());
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

	panel = SelectionPanel::make();
	panel->setTexture(objectAppearance->getTexture());
	panel->setMapper(mapperFactory.generateObjectMapper(this->objects), this->objects.size());

	auto borderPanel = gui2::BorderPanel::make();

	borderPanel->add(panel, gui2::BorderPanel::Center);
	borderPanel->add(modePanel, gui2::BorderPanel::Bottom, 50);

	add(borderPanel);
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
