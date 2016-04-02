#include <Shared/Level/Level.hpp>
#include <Shared/Level/ObjectObserver.hpp>

void ObjectObserver::notifyMove(Object::ID id, sf::Vector2i oldPosition)
{
	level->onObjectMoved(id, oldPosition);
}

void ObjectObserver::notifyUpdate(Object::ID id)
{
	level->onObjectUpdated(id);
}

ObjectObserver::ObjectObserver(Level* level) :
		level(level)
{
}
