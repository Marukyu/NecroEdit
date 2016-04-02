#ifndef SRC_SHARED_LEVEL_OBJECTOBSERVER_HPP_
#define SRC_SHARED_LEVEL_OBJECTOBSERVER_HPP_

#include <Shared/Level/Object.hpp>

class ObjectObserver
{
public:
	
	void notifyMove(Object::ID id, sf::Vector2i oldPosition);
	
	void notifyUpdate(Object::ID id);
	
private:
	
	ObjectObserver(Level * level);
	
	Level * level;
	
	friend class Level;
};

#endif /* SRC_SHARED_LEVEL_OBJECTOBSERVER_HPP_ */
