/*
 * SortingTexturePacker.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#include <Client/Graphics/Packing/SortingTexturePacker.hpp>
#include <Client/Graphics/Packing/TexturePacker.hpp>
#include <SFML/Graphics/Image.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <algorithm>

namespace priv
{

bool defaultSizeComparator(sf::Vector2u a, sf::Vector2u b)
{
	return a.x + a.y < b.x + b.y;
}

}

SortingTexturePacker::SortingTexturePacker() :
		SortingTexturePacker(makeUnique<TexturePacker>())
{
}

SortingTexturePacker::SortingTexturePacker(std::unique_ptr<ITexturePacker> && texturePacker) :
		SortingTexturePacker(std::move(texturePacker), &priv::defaultSizeComparator)
{
}

SortingTexturePacker::SortingTexturePacker(SizeComparatorFunc sizeComparator) :
		SortingTexturePacker(makeUnique<TexturePacker>(), sizeComparator)
{
}

SortingTexturePacker::SortingTexturePacker(std::unique_ptr<ITexturePacker> && texturePacker,
		SizeComparatorFunc sizeComparator) :
		myPacker(std::move(texturePacker)),
		myComparator(sizeComparator)
{
}

SortingTexturePacker::~SortingTexturePacker()
{
}

TexturePacker::NodeID SortingTexturePacker::add(const sf::Image& image)
{
	return addOwn(makeUnique<sf::Image>(image));
}

TexturePacker::NodeID SortingTexturePacker::addOwn(std::unique_ptr<const sf::Image> && image)
{
	// Get local ID for new node.
	NodeID id = myNodeMapping.size();

	// Add image to the queue.
	myImageQueue.push_back(
	{ std::move(image), id });

	// Add empty mapping entry.
	myNodeMapping.push_back(packFailure);

	// Return the local ID.
	return id;
}

void SortingTexturePacker::clear()
{
	myImageQueue.clear();
	myNodeMapping.clear();
	myPacker->clear();
}

bool SortingTexturePacker::empty() const
{
	return myImageQueue.empty() && myPacker->empty();
}

void SortingTexturePacker::pack()
{
	// Sort the images in the queue.
	std::sort(myImageQueue.begin(), myImageQueue.end(), [this](const QueuedImage & a, const QueuedImage & b)
	{
		return myComparator(b.image->getSize(), a.image->getSize());
	});

	// Pack images using the underlying texture packer.
	for (const auto & entry : myImageQueue)
	{
		// Check if node ID is within bounds.
		if (entry.id >= (NodeID)myNodeMapping.size())
		{
			continue;
		}
		
		// Pack node and add its remote ID to the mapping index.
		myNodeMapping[entry.id] = myPacker->add(*entry.image);
	}
}

sf::IntRect SortingTexturePacker::getImageRect(NodeID index) const
{
	// Check if node ID is within bounds.
	if (index >= 0 && index < (NodeID)myNodeMapping.size())
	{
		// Convert local node ID to remote node ID.
		return myPacker->getImageRect(myNodeMapping[index]);
	}
	else
	{
		return sf::IntRect();
	}
}

const sf::Texture * SortingTexturePacker::getTexture() const
{
	return myPacker->getTexture();
}

void SortingTexturePacker::setSmooth(bool smooth)
{
	myPacker->setSmooth(smooth);
}
