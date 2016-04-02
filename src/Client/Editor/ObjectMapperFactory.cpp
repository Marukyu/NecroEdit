#include <Client/Editor/ObjectMapperFactory.hpp>
#include <cstddef>
#include <string>

ObjectMapperFactory::ObjectMapperFactory(const ObjectAppearanceManager & objectAppearance) :
objectAppearance(&objectAppearance)
{
}

ObjectMapperFactory::~ObjectMapperFactory()
{
}

SelectionPanel::Mapper ObjectMapperFactory::generateObjectMapper(const std::vector<Object>& objects) const
{
	std::vector<SelectionPanel::ItemEntry> entries(objects.size());

	for (std::size_t i = 0; i < objects.size(); ++i)
	{
		auto array = objectAppearance->getObjectVertices(objects[i]);
		entries[i].vertices.assign(array.begin(), array.end());
		entries[i].name = objectAppearance->getObjectName(objects[i]);
	}
	
	return generateMapper(entries);
}

static SelectionPanel::ItemEntry fallbackEntry = SelectionPanel::ItemEntry();

SelectionPanel::Mapper ObjectMapperFactory::generateMapper(std::vector<SelectionPanel::ItemEntry> entries) const
{
	return [entries](SelectionPanel::ID id) -> const SelectionPanel::ItemEntry &
	{
		if (id < entries.size())
		{
			return entries[id];
		}
		
		return fallbackEntry;
	};
}


