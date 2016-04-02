/*
 * TexturePacker.hpp
 *
 *  Created on: Nov 25, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_GRAPHICS_PACKING_TEXTUREPACKER_HPP_
#define SRC_CLIENT_GRAPHICS_PACKING_TEXTUREPACKER_HPP_

#include <Client/Graphics/Packing/ITexturePacker.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>


class TexturePacker : public ITexturePacker
{
public:
	
	TexturePacker();
	TexturePacker(sf::Vector2u minimumSize);
	virtual ~TexturePacker();

	NodeID add(const sf::Image & image) override;
	void clear() override;
	bool empty() const override;

	sf::IntRect getImageRect(NodeID index) const override;

	const sf::Texture * getTexture() const override;

	void setSmooth(bool smooth) override;

	void setMinimumTextureSize(sf::Vector2u minimumSize);
	sf::Vector2u getMinimumTextureSize() const;

private:

	static const NodeID emptyNode, stubNode;

	struct Node : std::enable_shared_from_this<Node>
	{
		Node();

		std::shared_ptr<Node> sub1, sub2;
		sf::Vector2u pos;
		sf::Vector2u size;  // 0 = unlimited.
		NodeID id;

		std::shared_ptr<Node> add(const sf::Image & image, sf::Vector2u maxSize);
		sf::Vector2u getSizeBounds(sf::Vector2u currentMax = sf::Vector2u()) const;
	};

	bool createTransparentTexture(sf::Vector2u size);
	bool resizeToFit();

	std::shared_ptr<Node> myTree;
	std::vector<std::shared_ptr<Node> > myLookupTable;
	NodeID myIdCounter;

	std::unique_ptr<sf::Texture> myTexture;
	sf::Vector2u myMinimumSize;
};

#endif
