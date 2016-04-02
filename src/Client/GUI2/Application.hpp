#ifndef GUI2_APPLICATION_HPP
#define GUI2_APPLICATION_HPP

#include <Client/Graphics/BitmapText.hpp>
#include "Client/GUI2/Interface.hpp"
#include "Client/Graphics/TexturePacker.hpp"
#include "Shared/Utils/Timer.hpp"

namespace gui2
{

/**
 * manages a graphical application with one or more interfaces (windows).
 * enforces a framerate shared by all interfaces.
 * can hold common resources that are accessible by all interfaces and their widgets.
 */
class Application
{
public:

	Application();
	virtual ~Application();

	enum DefaultFonts
	{
		FontDefault = -1,
		FontSymbolSmall = -2,
		FontSymbolLarge = -3,
	};

	/// starts the application. (invokes init().)
	int run(const std::vector<std::string> & args = std::vector<std::string>());

	/// opens an interface on the application.
	std::shared_ptr<Interface> open();

	/// closes and releases the specified interface.
	void close(std::shared_ptr<Interface> interface);

	/// closes all interfaces and stops the application.
	void exit();

	/// sets the application's framerate limit. (0 = unlimited, -1 = vsync)
	void setFramerateLimit(int limit);

	/// returns the application's framerate limit. (0 = unlimited, -1 = vsync)
	int getFramerateLimit() const;

	/// returns one of the application's loaded fonts.
	std::shared_ptr<const btx::BitmapFont> getFont(int index = FontDefault) const;

	/// returns a pointer to the application's main texture.
	const sf::Texture * getMainTexture() const;

	/// enables/disables smoothing for the application's main texture.
	void setTextureSmoothingEnabled(bool smooth);

	/// returns the current smoothing state for the application's main texture.
	bool isTextureSmoothingEnabled() const;

	/// returns the position of a white non-transparent pixel on the application's
	/// main texture.
	sf::Vector2f getWhitePixel() const;

	/// returns the rectangle of an image on the main texture, loading the image beforehand
	/// if necessary. returns default sf::IntRect() with zero coords/size on failure.
	sf::IntRect loadImage(std::string imageName);

protected:

	/// adds an image to the main texture and returns the position the image was added at.
	sf::IntRect addImageToTexture(const sf::Image & image);

	/// unloads all images and clears the main texture.
	void clearImages();

	/// automatically loads a font from an image and adds it to the main texture.
	/// warning: the image should NOT have been loaded using loadImage() first.
	/// doing so anyway will cause a duplicate entry in the texture map.
	std::shared_ptr<const btx::BitmapFont> loadFont(std::string imageName, unsigned int charFirst = 0,
			float scale = 1.f, sf::Color gridColor = sf::Color(128, 128, 128));

	/// adds/replaces a font map entry for this application.
	/// use empty pointer to unload font.
	void setFont(int index, std::shared_ptr<const btx::BitmapFont> font);

private:

	// initializes the application. must contain at least one call to open().
	// returns 0 on success, any other number on failure.
	virtual int init(const std::vector<std::string> & args);

	// generates an interface. override to specify the interface's subclass.
	virtual std::shared_ptr<Interface> makeInterface();

	// called after "clearImages()".
	virtual bool onLoadImage(const std::string & imageIdentifier, sf::Image & image);

	// called to unify image names into identifiers.
	virtual bool convertImageName(std::string & imageName);

	// holds a list of all open interfaces.
	std::vector<std::shared_ptr<Interface> > myInterfaces;

	// holds a map of all loaded image identifiers and their rectangles on the main texture.
	std::map<std::string, sf::IntRect> myImageRectCache;

	// holds a list of all fonts.
	std::map<int, std::shared_ptr<const btx::BitmapFont> > myFonts;

	// the texture packer holding the application's main texture.
	TexturePacker myTexturePacker;

	// holds the center position of a white pixel on the main texture.
	sf::Vector2f myWhitePixelPos;

	// true if a white pixel has been computed successfully, false otherwise.
	bool myHasWhitePixel;

	// manages the timestep for this application.
	FramerateTimer myFramerateTimer;

	// the application's framerate limit.
	int myFramerateLimit;
};

} // namespace gui2

#endif
