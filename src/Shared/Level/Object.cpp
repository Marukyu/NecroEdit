#include <Shared/Level/Object.hpp>
#include <Shared/Level/ObjectObserver.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <algorithm>
#include <cstddef>
#include <cstring>

Object::Object(Type type) :
		position(),
		type(type),
		observer(nullptr),
		id(0)
{
}

Object::Object(sf::Vector2i position, Type type) :
		position(position),
		type(type),
		observer(nullptr),
		id(0)
{
}

Object::Object(const Object& object) :
		position(object.position),
		type(object.type),
		properties(object.properties),
		observer(nullptr),
		id(0)
{
}

Object::~Object()
{
}

void Object::assign(const Object& object)
{
	this->type = object.type;
	this->properties = object.properties;

	// A move implies an update, so perform the move as the last operation to only cause one update.
	setPosition(object.position);
}

void Object::setPosition(sf::Vector2i position)
{
	if (this->position != position)
	{
		if (observer)
		{
			std::swap(this->position, position);
			observer->notifyMove(id, position);
		}
		else
		{
			this->position = position;
		}
	}
}

sf::Vector2i Object::getPosition() const
{
	return position;
}

void Object::setType(Type type)
{
	if (this->type != type)
	{
		this->type = type;

		if (observer)
		{
			observer->notifyUpdate(id);
		}
	}
}

Object::Type Object::getType() const
{
	return type;
}

void Object::setPropertyInt(Property prop, int value)
{
	properties[prop] = cNtoS(value);

	if (observer)
	{
		observer->notifyUpdate(id);
	}
}

void Object::setPropertyString(Property prop, std::string value)
{
	properties[prop] = value;

	if (observer)
	{
		observer->notifyUpdate(id);
	}
}

void Object::unsetProperty(Property prop)
{
	properties.erase(prop);

	if (observer)
	{
		observer->notifyUpdate(id);
	}
}

bool Object::hasProperty(Property prop) const
{
	return properties.find(prop) != properties.end();
}

int Object::getPropertyInt(Property prop) const
{
	auto it = properties.find(prop);
	return it != properties.end() ? cStoI(it->second) : 0;
}

std::string Object::getPropertyString(Property prop) const
{
	auto it = properties.find(prop);
	return it != properties.end() ? it->second : "";
}

const std::map<Object::Property, std::string>& Object::getAllProperties() const
{
	return properties;
}

Object::ID Object::getID() const
{
	return id;
}

const char* Object::getTypeName(Type type)
{
	switch (type)
	{
	case Object::Type::Trap:
		return "trap";
	case Object::Type::Enemy:
		return "enemy";
	case Object::Type::Item:
		return "item";
	case Object::Type::Chest:
		return "chest";
	case Object::Type::Crate:
		return "crate";
	case Object::Type::Shrine:
		return "shrine";
	default:
		return "";
	}
}

const char* Object::getTypeNamePlural(Type type)
{
	switch (type)
	{
	case Object::Type::Trap:
		return "traps";
	case Object::Type::Enemy:
		return "enemies";
	case Object::Type::Item:
		return "items";
	case Object::Type::Chest:
		return "chests";
	case Object::Type::Crate:
		return "crates";
	case Object::Type::Shrine:
		return "shrines";
	default:
		return "";
	}
}

const char * Object::getPropertyName(Property prop)
{
	switch (prop)
	{
	case Object::Property::ID:
		return "id";
	case Object::Property::Type:
		return "type";
	case Object::Property::Subtype:
		return "subtype";
	case Object::Property::BeatDelay:
		return "beatDelay";
	case Object::Property::Lord:
		return "lord";
	case Object::Property::BloodCost:
		return "bloodCost";
	case Object::Property::SaleCost:
		return "saleCost";
	case Object::Property::SingleChoice:
		return "singleChoice";
	case Object::Property::Color:
		return "color";
	case Object::Property::Hidden:
		return "hidden";
	case Object::Property::Contents:
		return "contents";
	default:
		return "";
	}
}

Object::Property Object::getPropertyByName(const char * propName)
{
	for (std::size_t i = 0; i < std::size_t(Property::Count); ++i)
	{
		Property property = Property(i);

		if (std::strcmp(getPropertyName(property), propName) == 0)
		{
			return property;
		}
	}
	return Property::Invalid;
}

Object::PropertyValueType Object::getPropertyValueType(Type type, Property property)
{
	switch (property)
	{
	case Object::Property::ID:
	case Object::Property::Subtype:
	case Object::Property::BeatDelay:
	case Object::Property::Color:
		return PropertyValueType::Int;

	case Object::Property::Lord:
	case Object::Property::BloodCost:
	case Object::Property::SaleCost:
	case Object::Property::SingleChoice:
	case Object::Property::Hidden:
		return PropertyValueType::Bool;

	case Object::Property::Contents:
	default:
		return PropertyValueType::String;

	case Object::Property::Type:
		// Items store their name in the "type" field, whereas other object use a numeric ID.
		return (type == Object::Type::Item ? PropertyValueType::String : PropertyValueType::Int);
	}
}

// TODO: Read from necroedit.res instead of hardcoding defaults map.
static const std::map<Object::Type, std::map<Object::Property, std::string> > defaultProperties =
{
	{
		Object::Type::Trap,
		{
			{ Object::Property::Type, "1" },
			{ Object::Property::Subtype, "0" }
		}
	},
	{
		Object::Type::Enemy,
		{
			{ Object::Property::Type, "0" },
			{ Object::Property::BeatDelay, "0" },
			{ Object::Property::Lord, "0" }
		}
	},
	{
		Object::Type::Item,
		{
			{ Object::Property::Type, "food_1" },
			{ Object::Property::BloodCost, "0" },
			{ Object::Property::SaleCost, "0" },
			{ Object::Property::SingleChoice, "0" }
		}
	},
	{
		Object::Type::Chest,
		{
			{ Object::Property::Color, "1" },
			{ Object::Property::Contents, "no_item" },
			{ Object::Property::Hidden, "0" },
			{ Object::Property::SaleCost, "0" },
			{ Object::Property::SingleChoice, "0" }
		}
	},
	{
		Object::Type::Crate,
		{
			{ Object::Property::Type, "0" },
			{ Object::Property::Contents, "no_item" }
		}
	},
	{
		Object::Type::Shrine,
		{
			{ Object::Property::Type, "0" }
		}
	},
};

const std::map<Object::Property, std::string>& Object::getDefaultProperties(Type type)
{
	static const std::map<Object::Property, std::string> emptyMap = {};

	auto it = defaultProperties.find(type);

	if (it == defaultProperties.end())
	{
		return emptyMap;
	}
	else
	{
		return it->second;
	}
}

void Object::setID(ID id)
{
	this->id = id;
}

void Object::setObserver(ObjectObserver * observer)
{
	this->observer = observer;
}

ObjectObserver * Object::getObserver() const
{
	return observer;
}
