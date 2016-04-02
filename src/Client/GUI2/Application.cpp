#include "Client/GUI2/Application.hpp"

namespace gui2
{

Application::Application() :
	myHasWhitePixel(false)
{
	setFramerateLimit(70);
	myTexturePacker.setSmooth(true);
}

Application::~Application()
{
	
}

int Application::run(const std::vector<std::string>& args)
{
	int initRet = init(args);

	if (initRet != 0)
	{
		return initRet;
	}
	else
	{
		while (!myInterfaces.empty())
		{
			for (auto it = myInterfaces.begin(); it != myInterfaces.end(); )
			{
				if ((*it)->isWindowOpen())
				{
					(*it)->process();
					++it;
				}
				else
					it = myInterfaces.erase(it);
			}

			// sleep if necessary.
			if (getFramerateLimit() > 0)
			{
				myFramerateTimer.tick();
			}
		}
	}

	return 0;
}

std::shared_ptr<Interface> Application::open()
{
	std::shared_ptr<Interface> newInterface = makeInterface();

	newInterface->internalInit(this);
	newInterface->init();

	myInterfaces.push_back(newInterface);
	return newInterface;
}

void Application::close(std::shared_ptr<Interface> interface)
{
	for (auto it = myInterfaces.begin(); it != myInterfaces.end(); ++it)
	{
		if (*it == interface)
		{
			(*it)->closeWindow();
			myInterfaces.erase(it);
			return;
		}
	}
}

void Application::exit()
{
	for (auto it = myInterfaces.begin(); it != myInterfaces.end(); ++it)
		(*it)->closeWindow();

	myInterfaces.clear();
}

void Application::setFramerateLimit(int limit)
{
	myFramerateLimit = limit;

	if (limit > 0)
		myFramerateTimer.setFrameTime(sf::microseconds(1000000 / limit));
}

int Application::getFramerateLimit() const
{
	return myFramerateLimit;
}

std::shared_ptr<const btx::BitmapFont> Application::getFont(int index) const
{
	auto it = myFonts.find(index);
	return it == myFonts.end() ? nullptr : it->second;
}

const sf::Texture * Application::getMainTexture() const
{
	return myTexturePacker.getTexture();
}

void Application::setTextureSmoothingEnabled(bool smooth)
{
	myTexturePacker.setSmooth(smooth);
}

bool Application::isTextureSmoothingEnabled() const
{
	return myTexturePacker.isSmooth();
}

sf::Vector2f Application::getWhitePixel() const
{
	return myWhitePixelPos;
}

sf::IntRect Application::loadImage(std::string imageName)
{
	// look up image name first.
	auto it = myImageRectCache.find(imageName);

	if (it != myImageRectCache.end())
	{
		return it->second;
	}
	else
	{
		std::string imageIdent = imageName;

		// try to convert image name to identifier.
		if (convertImageName(imageIdent))
		{
			// retry image cache lookup, this time with identifier.
			it = myImageRectCache.find(imageIdent);

			// if entry exists, return it.
			if (it != myImageRectCache.end())
			{
				return it->second;
			}

			// otherwise, load image.
			sf::Image image;

			if (!onLoadImage(imageIdent, image))
			{
				return myImageRectCache[imageName] = myImageRectCache[imageIdent] = sf::IntRect();
			}

			// add image to main texture.
			return myImageRectCache[imageName] = myImageRectCache[imageIdent] = addImageToTexture(image);
		}
		else
		{
			// name lookup failure (file does not exist).
			return myImageRectCache[imageName] = myImageRectCache[imageIdent] = sf::IntRect();
		}

	}
}


sf::IntRect Application::addImageToTexture(const sf::Image & image)
{
	TexturePacker::NodeID id = myTexturePacker.add(image);

	if (!myHasWhitePixel)
	{
		sf::Image texture = myTexturePacker.getTexture()->copyToImage();

		for (std::size_t y = 0; !myHasWhitePixel && y < texture.getSize().y; ++y)
		{
			for (std::size_t x = 0; !myHasWhitePixel && x < texture.getSize().x; ++x)
			{
				if (texture.getPixel(x, y) == sf::Color::White)
				{
					myWhitePixelPos.x = x + 0.5f;
					myWhitePixelPos.y = y + 0.5f;
					myHasWhitePixel = true;
				}
			}
		}
	}

	return myTexturePacker.getImageRect(id);
}

void Application::clearImages()
{
	myTexturePacker.clear();
	myWhitePixelPos = sf::Vector2f(0,0);
	myHasWhitePixel = false;
}

std::shared_ptr<const btx::BitmapFont> Application::loadFont(std::string imageName, unsigned int charFirst, float scale, sf::Color gridColor)
{
	std::shared_ptr<btx::BitmapFont> font;

	// load image.
	sf::Image image;
	if (!convertImageName(imageName) || !onLoadImage(imageName, image))
		throw std::runtime_error("Failed to load font image file " + imageName);

	// load font.
	if (!(font = std::shared_ptr<btx::BitmapFont>(btx::BitmapFont::generate(image, charFirst, scale, gridColor))))
		throw std::runtime_error("Failed to determine character grid for font " + imageName);

	sf::IntRect texRect = addImageToTexture(image);

	if (texRect == sf::IntRect())
		throw std::runtime_error("Could not fit font " + imageName + " into the texture atlas (image too large)");

	font->texture = myTexturePacker.getTexture();
	font->x += texRect.left;
	font->y += texRect.top;

	for (auto interface : myInterfaces)
	{
		interface->getRootContainer().onUpdateFonts();
	}

	return font;
}

void Application::setFont(int index, std::shared_ptr<const btx::BitmapFont> font)
{
	if (font)
		myFonts[index] = font;
	else
		myFonts.erase(index);
}


int Application::init(const std::vector<std::string>& args)
{
	return 0;
}

std::shared_ptr<Interface> Application::makeInterface()
{
	return std::make_shared<Interface>();
}

bool Application::onLoadImage(const std::string & imageIdentifier, sf::Image & image)
{
	return false;
}

bool Application::convertImageName(std::string & imageName)
{
	return true;
}

} // namespace gui2
