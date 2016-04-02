#ifndef GUI2_IMAGE_HPP
#define GUI2_IMAGE_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class Image : public Widget
{
public:

	static Ptr<Image> make();

	void setTexture(const sf::Texture * tex);
	const sf::Texture * getTexture() const;

	void onDraw(sf::RenderTarget & target, sf::RenderStates states) const;

	void update();

private:

	bool isVertexRenderable() const;

	const sf::Texture * myTexture;
};

}

#endif
