#ifndef GUI2_INTERFACE_HPP
#define GUI2_INTERFACE_HPP

#include "Client/GUI2/Container.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

namespace gui2
{

class Application;

/**
 * manages a system window that processes and renders GUI2 widgets.
 * has a single container that automatically resizes to fit the interface.
 * can only be used within the context of an application.
 */
class Interface : public std::enable_shared_from_this<Interface>
{
public:

	// constructs a new default interface. should only be called by application.
	Interface();
	virtual ~Interface();

	/// returns the root container that holds this interface's widgets.
	Container & getRootContainer();
	const Container & getRootContainer() const;

	/// returns the application this interface belongs to.
	Application & getParentApplication() const;

	/// sets the interface's window title.
	void setTitle(sf::String title);

	/// returns the interface's window title.
	sf::String getTitle() const;

	/// resizes the window to the specified size (window mode) or switches to
	/// the closest available resolution (fullscreen mode).
	void resize(sf::Vector2u size, bool fullscreen);

	/// returns this interface's window size or screen resolution.
	sf::Vector2u getSize() const;

	/// returns true if the interface is in fullscreen mode.
	bool isFullscreen() const;

	/// returns true if the interface is currently focused.
	bool isFocused() const;

	/// shows or hides the mouse cursor on this interface.
	void setMouseCursorVisible(bool visible);

	/// returns the mouse cursor's visibility state on this interface.
	bool isMouseCursorVisible() const;

protected:

	/// returns a RenderTarget corresponding to this interface's window.
	sf::RenderTarget & getRenderTarget();
	const sf::RenderTarget & getRenderTarget() const;

	// these functions are called by the controlling application to manage the
	// interface's lifetime.
	bool isWindowOpen() const;
	void openWindow();
	void closeWindow();

private:

	// initializes the window. called by the controlling application.
	virtual void init();

	// processes and renders everything.
	void process();

	// called for each event during processing.
	virtual void onEvent(const sf::Event & event);

	// called after processing, but before rendering.
	virtual void onProcess(const WidgetEvents & events);

	// called after rendering, but before myWindow.display(). use draw().
	virtual void onRender();

	// called when the window is about to close. default behavior is to call closeWindow().
	virtual void onClose();

	// container subclass with an interface onProcess() callback.
	class RootContainer : public Container
	{
	public:
		static Ptr<RootContainer> make();

		bool isClippingWidgets() const;

	protected:

		virtual void init() override;

	private:

		void onProcessContainer(const WidgetEvents & events);
	};


	// internal interface initialization actions that cannot be performed in the
	// constructor.
	void internalInit(Application * parentApp);

	// returns the closest videomode to the specified size vector.
	sf::VideoMode findGoodVideoMode(sf::Vector2u compare) const;


	// the interface's window.
	sf::RenderWindow myWindow;

	// various (self-explanatory) flags.
	bool myIsFullscreen;
	bool myIsMouseCursorVisible;
	bool myHasFocus;

	// root container widget that holds this interface's widgets.
	std::shared_ptr<Container> myRootContainer;

	// event information to be passed to the root container.
	ContainerEvents myContainerEvents;

	// parent application.
	Application * myParentApplication;

	// window size.
	sf::Vector2u myWindowSize;

	// window title.
	sf::String myWindowTitle;

	friend class Application;
	friend class Interface::RootContainer;

};

} // namespace gui2

#endif
