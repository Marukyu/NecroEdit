/*
 * SortingTexturePacker.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_GRAPHICS_PACKING_SORTINGTEXTUREPACKER_HPP_
#define SRC_CLIENT_GRAPHICS_PACKING_SORTINGTEXTUREPACKER_HPP_

#include <Client/Graphics/Packing/ITexturePacker.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <vector>

class SortingTexturePacker : public ITexturePacker
{
public:
	typedef std::function<bool(sf::Vector2u, sf::Vector2u)> SizeComparatorFunc;

	SortingTexturePacker();
	SortingTexturePacker(std::unique_ptr<ITexturePacker> && texturePacker);
	SortingTexturePacker(SizeComparatorFunc sizeComparator);
	SortingTexturePacker(std::unique_ptr<ITexturePacker> && texturePacker, SizeComparatorFunc sizeComparator);

	virtual ~SortingTexturePacker();

	NodeID add(const sf::Image & image) override;
	NodeID addOwn(std::unique_ptr<const sf::Image> && image) override;
	void clear() override;
	bool empty() const override;

	void pack();

	sf::IntRect getImageRect(NodeID index) const override;

	const sf::Texture * getTexture() const override;

	void setSmooth(bool smooth) override;

private:

	struct QueuedImage
	{
		std::unique_ptr<const sf::Image> image;
		NodeID id;
	};

	std::unique_ptr<ITexturePacker> myPacker;
	std::vector<NodeID> myNodeMapping;

	std::vector<QueuedImage> myImageQueue;
	SizeComparatorFunc myComparator;
};

#endif
