#ifndef SRC_SHARED_LEVEL_OBJECT_HPP_
#define SRC_SHARED_LEVEL_OBJECT_HPP_

#include <SFML/System/Vector2.hpp>
#include <map>
#include <string>

class Level;
class ObjectObserver;

class Object
{
public:
	
	typedef unsigned int ID;
	
	enum class Type
	{
		None = -1,
		
		Trap,
		Enemy,
		Item,
		Chest,
		Crate,
		Shrine,
		
		TypeCount,
		
		Internal
	};
	
	enum class Property
	{
		ID,
		Type,
		Subtype,
		BeatDelay,
		Lord,
		BloodCost,
		SaleCost,
		SingleChoice,
		Color,
		Hidden,
		Contents,
		
		Count,
		Invalid
	};

	enum class PropertyValueType
	{
		Int,
		String,
		Bool
	};
	
	Object(Object && object) = default;
	Object & operator=(Object && object) = default;
	
	explicit Object(const Object & object);
	
	Object(Type type = Type::None);
	Object(sf::Vector2i position, Type type = Type::None);
	~Object();
	
	void assign(const Object & object);
	
	void setPosition(sf::Vector2i position);
	sf::Vector2i getPosition() const;
	
	void setType(Type type);
	Type getType() const;
	
	void setPropertyInt(Property prop, int value);
	void setPropertyString(Property prop, std::string value);
	void unsetProperty(Property prop);
	
	bool hasProperty(Property prop) const;
	
	int getPropertyInt(Property prop) const;
	std::string getPropertyString(Property prop) const;
	const std::map<Property, std::string> & getAllProperties() const;
	
	ID getID() const;
	
	static const char * getTypeName(Type type);
	static const char * getTypeNamePlural(Type type);
	
	static const char * getPropertyName(Property prop);
	static Property getPropertyByName(const char * propName);
	
	static PropertyValueType getPropertyValueType(Type type, Property property);
	static const std::map<Property, std::string> & getDefaultProperties(Type type);

private:
	
	void setID(ID id);
	
	void setObserver(ObjectObserver * observer);
	ObjectObserver * getObserver() const;
	
	sf::Vector2i position;
	Type type;
	std::map<Property, std::string> properties;
	
	ObjectObserver * observer;
	ID id;
	
	friend class Level;
};

#endif
