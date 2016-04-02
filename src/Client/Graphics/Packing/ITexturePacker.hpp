/*
 * ITexturePacker.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_GRAPHICS_PACKING_ITEXTUREPACKER_HPP_
#define SRC_CLIENT_GRAPHICS_PACKING_ITEXTUREPACKER_HPP_

#include <SFML/Config.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <memory>

namespace sf
{
class Image;
class Texture;
}


class ITexturePacker
{
public:

	typedef sf::Int32 NodeID;

	static const NodeID packFailure;

	ITexturePacker()
	{
	}
	virtual ~ITexturePacker()
	{
	}

	virtual NodeID add(const sf::Image & image) = 0;
	virtual NodeID addOwn(std::unique_ptr<const sf::Image> && image);
	virtual void clear() = 0;
	virtual bool empty() const = 0;

	virtual sf::IntRect getImageRect(NodeID index) const = 0;

	virtual const sf::Texture * getTexture() const = 0;

	virtual void setSmooth(bool smooth) = 0;
	bool isSmooth() const;
};


#endif
