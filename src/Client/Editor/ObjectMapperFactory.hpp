#ifndef SRC_CLIENT_EDITOR_OBJECTMAPPERFACTORY_HPP_
#define SRC_CLIENT_EDITOR_OBJECTMAPPERFACTORY_HPP_

#include <Client/Editor/SelectionPanel.hpp>
#include <Client/LevelRenderer/ObjectAppearance.hpp>
#include <Shared/Level/Object.hpp>
#include <vector>

/**
 * Creates object mappers for selector widgets.
 * 
 * Mappers are lists of vertex representations of objects that can be placed inside a selector widget, allowing the user
 * to select items from a visual grid.
 */
class ObjectMapperFactory
{
public:
	ObjectMapperFactory(const ObjectAppearanceManager & objectAppearance);
	~ObjectMapperFactory();

	SelectionPanel::Mapper generateObjectMapper(const std::vector<Object> & objects) const;

private:

	SelectionPanel::Mapper generateMapper(std::vector<SelectionPanel::ItemEntry> entries) const;

	const ObjectAppearanceManager * objectAppearance;
};

#endif
