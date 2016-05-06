#include <Client/Editor/ObjectPropertyPanel.hpp>
#include <Client/GUI2/Widget.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <cstddef>
#include <memory>
#include <utility>

ObjectPropertyPanel::ObjectPropertyPanel() :
	object(nullptr)
{
}

gui2::Ptr<ObjectPropertyPanel> ObjectPropertyPanel::make()
{
	gui2::Ptr<ObjectPropertyPanel> widget = std::make_shared<ObjectPropertyPanel>();
	widget->init();
	return widget;
}

ObjectPropertyPanel::~ObjectPropertyPanel()
{
}

void ObjectPropertyPanel::setObject(Object* object)
{
	this->object = object;
}

Object* ObjectPropertyPanel::getObject() const
{
	return object;
}

static constexpr float PROPERTY_ENTRY_HEIGHT = 24.f;
static constexpr float PROPERTY_NAME_LABEL_WIDTH = 75.f;

void ObjectPropertyPanel::update()
{
	// Remove property entries from container.
	for (auto it = propertyMap.begin(); it != propertyMap.end(); ++it)
	{
		propertyPanel->remove(it->second);
	}

	// Remove all properties.
	propertyMap.clear();

	// Null object means empty panel.
	if (object != nullptr)
	{
		// Get object properties.
		auto objectPropertyMap = object->getAllProperties();

		// Iterate over object properties for widget generation.
		for (const auto & objectProperty : objectPropertyMap)
		{
			// Generate entry from property key (and object type).
			gui2::Ptr<PropertyEntry> entry = generatePropertyEntry(objectProperty.first, object->getType());

			// Null entry is returned in case of invalid property key or hidden property.
			if (entry != nullptr)
			{
				propertyMap[objectProperty.first] = entry;
			}
		}

		// Resize grid panel.
		propertyPanel->setGridSize(1, propertyMap.size());

		// Keep track of Y-position of grid cell to place entries in.
		std::size_t placementIndex = 0;

		// Place property entries in container.
		for (auto & propertyEntry : propertyMap)
		{
			propertyPanel->add(propertyEntry.second, 0, placementIndex++);
		}
	}

	// Set preferred size (width is irrelevant).
	setAutomaticSize(sf::Vector2f(PROPERTY_ENTRY_HEIGHT, PROPERTY_ENTRY_HEIGHT * (propertyMap.size() + 1)));

	// Fill entries with values.
	updateValues();

	// Set caption.
	if (object == nullptr)
	{
		caption->setEnabled(false);
		caption->setText("Properties");
	}
	else
	{
		caption->setEnabled(true);
		caption->setText(capitalize(Object::getTypeName(object->getType())) + " Properties");
	}
}

void ObjectPropertyPanel::updateValues()
{
	if (object != nullptr)
	{
		for (auto it = propertyMap.begin(); it != propertyMap.end(); ++it)
		{
			it->second->setPropertyValue(object->getPropertyString(it->first));
		}
	}
}

void ObjectPropertyPanel::init()
{
	setAutoManagingHeight(true);
	gui2::Container::init();

	propertyPanel = gui2::GridPanel::make();

	borderPanel = gui2::BorderPanel::make();
	borderPanel->add(propertyPanel, gui2::BorderPanel::Center);

	caption = gui2::Text::make();
	caption->setTextAlignment(AlignCenter);
	borderPanel->add(caption, gui2::BorderPanel::Top, PROPERTY_ENTRY_HEIGHT);

	add(borderPanel);

	update();
}

ObjectPropertyPanel::PropertyEntry::PropertyEntry()
{
}

ObjectPropertyPanel::PropertyEntry::~PropertyEntry()
{
}

void ObjectPropertyPanel::PropertyEntry::setPropertyName(std::string name)
{
	propertyName = name;
	propertyNameText->setText(propertyName + ": ");
}

std::string ObjectPropertyPanel::PropertyEntry::getPropertyName() const
{
	return propertyName;
}

void ObjectPropertyPanel::PropertyEntry::init()
{
	gui2::Container::init();
	panel = gui2::BorderPanel::make();
	propertyNameText = gui2::Text::make();
	propertyNameText->setTextAlignment(AlignRight);
	panel->add(propertyNameText, gui2::BorderPanel::Left, PROPERTY_NAME_LABEL_WIDTH);
	add(panel);
}

void ObjectPropertyPanel::PropertyEntry::addValueWidget(gui2::Ptr<gui2::Widget> widget)
{
	panel->add(widget, gui2::BorderPanel::Center);
}

gui2::Ptr<ObjectPropertyPanel::BoolProperty> ObjectPropertyPanel::BoolProperty::make()
{
	gui2::Ptr<BoolProperty> widget = std::make_shared<BoolProperty>();
	widget->init();
	return widget;
}

void ObjectPropertyPanel::BoolProperty::setPropertyValue(std::string value)
{
	currentValue = cStoI(value) != 0;
	checkbox->setChecked(currentValue);
}

std::string ObjectPropertyPanel::BoolProperty::getPropertyValue() const
{
	return checkbox->isChecked() ? "1" : "0";
}

bool ObjectPropertyPanel::BoolProperty::wasChanged()
{
	if (changed)
	{
		changed = false;
		return true;
	}
	return false;
}

void ObjectPropertyPanel::BoolProperty::init()
{
	PropertyEntry::init();

	currentValue = false;
	changed = false;
	checkbox = gui2::Checkbox::make();
	addValueWidget(checkbox);
}

void ObjectPropertyPanel::BoolProperty::onProcessContainer(gui2::WidgetEvents& events)
{
	if (currentValue != checkbox->isChecked())
	{
		currentValue = checkbox->isChecked();
		changed = true;
	}
}

gui2::Ptr<ObjectPropertyPanel::StringProperty> ObjectPropertyPanel::StringProperty::make(bool numeric)
{
	gui2::Ptr<StringProperty> widget = std::make_shared<StringProperty>();
	widget->init();
	widget->setNumeric(numeric);
	return widget;
}

void ObjectPropertyPanel::StringProperty::setPropertyValue(std::string value)
{
	input->setTextSilent(value);
}

std::string ObjectPropertyPanel::StringProperty::getPropertyValue() const
{
	return input->getText();
}

bool ObjectPropertyPanel::StringProperty::wasChanged()
{
	return input->wasChanged();
}

void ObjectPropertyPanel::StringProperty::setNumeric(bool numeric)
{
	input->setInputType(numeric ? gui2::TextField::InputInteger : gui2::TextField::InputText);
}

bool ObjectPropertyPanel::StringProperty::isNumeric() const
{
	return input->getInputType() == gui2::TextField::InputInteger;
}

void ObjectPropertyPanel::StringProperty::init()
{
	PropertyEntry::init();

	input = gui2::TextField::make();
	addValueWidget(input);
}

gui2::Ptr<ObjectPropertyPanel::PropertyEntry> ObjectPropertyPanel::generatePropertyEntry(Object::Property property,
	Object::Type objectType)
{
	gui2::Ptr<PropertyEntry> entry;

	switch (Object::getPropertyValueType(objectType, property))
	{
	case Object::PropertyValueType::String:
	default:
		entry = StringProperty::make(false);
		break;
	case Object::PropertyValueType::Int:
		entry = StringProperty::make(true);
		break;
	case Object::PropertyValueType::Bool:
		entry = BoolProperty::make();
		break;
	}

	entry->setPropertyName(Object::getPropertyName(property));

	return entry;
}

void ObjectPropertyPanel::onProcessContainer(gui2::WidgetEvents& events)
{
	for (auto & currentEntry : propertyMap)
	{
		if (currentEntry.second->wasChanged())
		{
			object->setPropertyString(currentEntry.first, currentEntry.second->getPropertyValue());
		}
	}
}
