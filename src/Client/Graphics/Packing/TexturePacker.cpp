/*
 * TexturePacker.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#include <Client/Graphics/Packing/TexturePacker.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <algorithm>
#include <cstddef>
#include <cstring>


const TexturePacker::NodeID TexturePacker::emptyNode = -1;
const TexturePacker::NodeID TexturePacker::stubNode = -2;

namespace priv
{
static const std::size_t zeroMemSize = 65536 * 4;
static sf::Uint8 zeroMem[zeroMemSize];
static bool zeroMemInitialized = false;
}

TexturePacker::TexturePacker() :
		myTexture(makeUnique<sf::Texture>()),
		myMinimumSize(256, 256)
{
	clear();
}

TexturePacker::TexturePacker(sf::Vector2u minimumSize) :
		myTexture(makeUnique<sf::Texture>()),
		myMinimumSize(minimumSize)
{
	clear();
}

TexturePacker::~TexturePacker()
{

}

TexturePacker::NodeID TexturePacker::add(const sf::Image & image)
{
	// reject zero-sized images.
	if (image.getSize().x == 0 || image.getSize().y == 0)
		return packFailure;

	std::shared_ptr<Node> node = myTree->add(image, myTexture->getSize());

	if (node)
	{
		// insertion successful, add lookup table entry, add image to texture, and return node ID.
		node->id = myIdCounter++;

		myLookupTable.resize(std::max<unsigned int>(myLookupTable.size(), node->id + 1));
		myLookupTable[node->id] = node;

		myTexture->update(image, node->pos.x, node->pos.y);
		return node->id;
	}
	else
	{
		// retry adding with increased texture size, if possible.
		{
			sf::Image curTexture = myTexture->copyToImage();

			if (!createTransparentTexture(myTexture->getSize() * 2u))
			{
				// free any unnecessarily allocated texture space.
				resizeToFit();
				return packFailure;
			}

			myTexture->update(curTexture, 0, 0);
		}

		return add(image);
	}
}

void TexturePacker::clear()
{
	myLookupTable.clear();
	myTree = std::make_shared<Node>();
	createTransparentTexture(myMinimumSize);
	myIdCounter = 0;
}

bool TexturePacker::empty() const
{
	return myIdCounter == 0;
}

sf::IntRect TexturePacker::getImageRect(TexturePacker::NodeID index) const
{
	if (index < int(myLookupTable.size()) && index >= 0 && myLookupTable[index])
	{
		sf::Vector2u pos = myLookupTable[index]->pos, size = myLookupTable[index]->size;
		return sf::IntRect(pos.x, pos.y, size.x, size.y);
	}
	else
		return sf::IntRect();
}

void TexturePacker::setMinimumTextureSize(sf::Vector2u minimumSize)
{
	minimumSize.x = std::max<unsigned int>(1, std::min(minimumSize.x, sf::Texture::getMaximumSize()));
	minimumSize.y = std::max<unsigned int>(1, std::min(minimumSize.y, sf::Texture::getMaximumSize()));

	if (myMinimumSize != minimumSize)
	{
		myMinimumSize = minimumSize;

		// update texture size.
		if (empty())
		{
			if (!createTransparentTexture(myMinimumSize))
			{
				return;
			}
		}
		else
		{
			if (!resizeToFit())
			{
				return;
			}
		}
	}
}

sf::Vector2u TexturePacker::getMinimumTextureSize() const
{
	return myMinimumSize;
}

const sf::Texture * TexturePacker::getTexture() const
{
	return myTexture.get();
}

void TexturePacker::setSmooth(bool smooth)
{
	myTexture->setSmooth(smooth);
}

bool TexturePacker::createTransparentTexture(sf::Vector2u size)
{
	if (!myTexture->create(size.x, size.y))
	{
		return false;
	}

	if (!priv::zeroMemInitialized)
	{
		std::memset(priv::zeroMem, 0, priv::zeroMemSize);
		priv::zeroMemInitialized = true;
	}

	if (myTexture->getSize().x * myTexture->getSize().y * 4 < priv::zeroMemSize)
	{
		myTexture->update(priv::zeroMem);
	}
	else
	{
		for (std::size_t y = 0; y < myTexture->getSize().y; ++y)
		{
			myTexture->update(priv::zeroMem, myTexture->getSize().x, 1, 0, y);
		}
	}

	return true;
}

bool TexturePacker::resizeToFit()
{
	// traverse tree and calculate bounds.
	sf::Vector2u bounds = myTree->getSizeBounds();

	sf::Vector2u size = myMinimumSize;

	// increase size to at least contain the total bounds.
	while (size.x < bounds.x || size.y < bounds.y)
		size *= (unsigned int) 2;

	// crop texture to new bounds if necessary.
	if (size != myTexture->getSize())
	{
		sf::Image curTextureSub;
		{
			sf::Image curTexture = myTexture->copyToImage();

			if (!createTransparentTexture(size))
				return false;

			curTextureSub.create(size.x, size.y);
			curTextureSub.copy(curTexture, 0, 0, sf::IntRect(0, 0, size.x, size.y), false);
		}
		myTexture->update(curTextureSub);
	}

	return true;
}

TexturePacker::Node::Node() :
		sub1(nullptr),
		sub2(nullptr),
		pos(0, 0),
		size(0, 0),
		id(emptyNode)
{
}

std::shared_ptr<TexturePacker::Node> TexturePacker::Node::add(const sf::Image & image, sf::Vector2u maxSize)
{
	// try to insert into sub-nodes, if applicable.
	if (sub1 && sub2)
	{
		// try to insert into first node.
		std::shared_ptr<Node> ret = sub1->add(image, maxSize);

		// then, if first node failed, second node.
		if (ret)
			return ret;
		else
			return sub2->add(image, maxSize);
	}
	else
	{
		// node is already occupied or is a stub node (actual zero width/height).
		if (id != emptyNode)
			return std::shared_ptr<Node>();

		// get node's effective size. can be negative.
		sf::Vector2i effSize(size.x == 0 ? (int) maxSize.x - (int) pos.x : size.x,
			size.y == 0 ? (int) maxSize.y - (int) pos.y : size.y);

		// image is too big to fit in node.
		if (!((int) image.getSize().x <= effSize.x && (int) image.getSize().y <= effSize.y))
			return std::shared_ptr<Node>();

		// image fits exactly into node.
		if (image.getSize().x == size.x && image.getSize().y == size.y)
			return shared_from_this();

		// image does fit, but not perfectly: create sub-nodes.
		sub1 = std::make_shared<Node>();
		sub2 = std::make_shared<Node>();

		// determine splitting direction: first infinite compare, then finite.
		bool finiteComp = effSize.x - (int) image.getSize().x > effSize.y - (int) image.getSize().y;

		if (size.x == 0 ? (size.y == 0 ? finiteComp : true) : (size.y == 0 ? false : finiteComp))
		{
			// left-right split.

			// left side.
			sub1->pos = pos;
			sub1->size.x = image.getSize().x;
			sub1->size.y = size.y;

			// right side (1px padding, infinitely wide node stays that way).
			sub2->pos.x = pos.x + image.getSize().x + 1;
			sub2->pos.y = pos.y;
			sub2->size.x = size.x == 0 ? 0 : (size.x - image.getSize().x - 1);
			sub2->size.y = size.y;

			// check for and mark stub node (zero-width non-infinite node).
			if (size.x != 0 && size.x <= image.getSize().x + 1)
			{
				sub2->size.x = 0;
				sub2->id = stubNode;
			}
		}
		else
		{
			// top-bottom split.

			// top side.
			sub1->pos = pos;
			sub1->size.x = size.x;
			sub1->size.y = image.getSize().y;

			// bottom side (1px padding, infinitely tall node stays that way).
			sub2->pos.x = pos.x;
			sub2->pos.y = pos.y + image.getSize().y + 1;
			sub2->size.x = size.x;
			sub2->size.y = size.y == 0 ? 0 : (size.y - image.getSize().y - 1);

			// check for and mark stub node (zero-height non-infinite node).
			if (size.y != 0 && size.y <= image.getSize().y + 1)
			{
				sub2->size.y = 0;
				sub2->id = stubNode;
			}
		}

		// attempt to insert into first sub-node.
		return sub1->add(image, maxSize);
	}
}

sf::Vector2u TexturePacker::Node::getSizeBounds(sf::Vector2u currentMax) const
{
	if (sub1)
		currentMax = sub1->getSizeBounds(currentMax);

	if (sub2)
		currentMax = sub2->getSizeBounds(currentMax);

	if (id != emptyNode && id != stubNode)
	{
		currentMax.x = std::max(currentMax.x, pos.x + size.x);
		currentMax.y = std::max(currentMax.y, pos.y + size.y);
	}

	return currentMax;
}



