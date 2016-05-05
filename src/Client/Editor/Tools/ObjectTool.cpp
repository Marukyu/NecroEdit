#include <Client/Editor/Tools/ObjectTool.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <algorithm>
#include <memory>
#include <string>

ObjectTool::ObjectTool(std::vector<Object::Type> objectTypes) :
		objectTypes(objectTypes)
{
}

ObjectTool::~ObjectTool()
{
}

void ObjectTool::onInit()
{
	std::vector<Object> objects;

	for (Object::Type type : objectTypes)
	{
		if (type == Object::Type::Item)
		{
			std::vector<std::string> itemNames = getEditorData().objectAppearance->getItemNameList();

			for (const std::string & itemName : itemNames)
			{
				Object item(Object::Type::Item);
				item.setPropertyString(Object::Property::Type, itemName);

				objects.push_back(std::move(item));
			}
		}
		else
		{
			Object::Property typeKey = (
					Object::Type(type) == Object::Type::Chest ? Object::Property::Color : Object::Property::Type);
			std::vector<int> objectTypeIDs = getEditorData().objectAppearance->getObjectIDList(Object::Type(type));

			if (type == Object::Type::Trap)
			{
				for (int objectTypeID : objectTypeIDs)
				{
					for (int objectSubtypeID : getEditorData().objectAppearance->getObjectSubtypeList(type,
						objectTypeID))
					{
						Object object((Object::Type(type)));
						object.setPropertyInt(typeKey, objectTypeID);
						object.setPropertyInt(Object::Property::Subtype, objectSubtypeID);

						objects.push_back(std::move(object));
					}
				}
			}
			else
			{
				for (int objectTypeID : objectTypeIDs)
				{
					Object object((Object::Type(type)));
					object.setPropertyInt(typeKey, objectTypeID);

					objects.push_back(std::move(object));
				}
			}
		}
	}

	settingsPanel = ObjectToolPanel::make(objects, *getEditorData().objectAppearance);
}

gui2::Ptr<gui2::Widget> ObjectTool::getSettingsPanel() const
{
	return settingsPanel;
}

const Brush& ObjectTool::getPrimaryBrush() const
{
	primaryBrush.setObject(settingsPanel->getSelectedObject());
	primaryBrush.setObjectMode(settingsPanel->getPrimaryMode());
	return primaryBrush;
}

const Brush& ObjectTool::getSecondaryBrush() const
{
	secondaryBrush.setObjectMode(settingsPanel->getSecondaryMode());
	return secondaryBrush;
}
