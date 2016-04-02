/*
 * ITexturePacker.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#include <Client/Graphics/Packing/ITexturePacker.hpp>
#include <SFML/Graphics/Texture.hpp>


const ITexturePacker::NodeID ITexturePacker::packFailure = -1;

ITexturePacker::NodeID ITexturePacker::addOwn(std::unique_ptr<const sf::Image> && image)
{
	return add(*image);
}

bool ITexturePacker::isSmooth() const
{
	return getTexture()->isSmooth();
}



