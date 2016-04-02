#ifndef GUI2_WINDOW_HPP
#define GUI2_WINDOW_HPP

#include "Client/GUI2/Container.hpp"
#include "Client/GUI2/Widgets/Button.hpp"

namespace gui2
{

class Window : public Container {

public:

	/// factory function.
	static Ptr<Window> make();

	void open();
	void openAutoRemove();
	void close();
	bool isOpen() const;

	void setCaption(std::string text);
	std::string getCaption() const;

	void setMaximized(bool maximized);
	bool isMaximized() const;

	void setEnforcedRatio(float ratio);
	float getEnforcedRatio() const;
	bool isEnforcingRatio() const;

	void setMinSize(sf::Vector2f minSize);
	sf::Vector2f getMinSize() const;
	void setMaxSize(sf::Vector2f maxSize);
	sf::Vector2f getMaxSize() const;

	enum WindowFlags {
		FlagNone		= 0,
		FlagClose		= 1 << 0,
		FlagResize		= 1 << 1,
		FlagMaximize	= 1 << 2,
		FlagMove		= 1 << 3,
		FlagMinimal		= 1 << 4,
		FlagOkay		= 1 << 5,

		DefaultFlags	= FlagClose | FlagResize | FlagMaximize | FlagMove
	};

	void setFlags(unsigned int flags);
	void addFlags(unsigned int flags);
	void removeFlags(unsigned int flags);
	bool hasFlag(unsigned int flag) const;
	unsigned int getFlags() const;

protected:

	virtual void init();

	bool isPosOverTitleBar(sf::Vector2f pos) const;
	bool isTitleBarDown() const;
	sf::FloatRect getTitleBarRect() const;
	float getTitleBarButtonWidth() const;

	sf::FloatRect getTRCornerRect() const;
	sf::FloatRect getTLCornerRect() const;
	sf::FloatRect getBLCornerRect() const;
	sf::FloatRect getBRCornerRect() const;

private:

	enum DragMode {
		DragNone,   // mouse is not down, window not being dragged.
		Dragging,   // mouse is down, window is being dragged.
		DragFail,   // mouse was clicked on window body, not dragging.

		ResizeTR,   // resizing top right corner.
		ResizeTL,   // same but for top left corner.
		ResizeBL,   // and so on...
		ResizeBR
	};

	void onResize();
	void onProcessContainer(WidgetEvents & events);
	void onUpdateVertexCachePrepend();
	void onUpdateVertexCacheAppend();
	bool checkMouseover(sf::Vector2f pos) const;

	/// called whenever the window is processed.
	virtual void onProcessWindow(const WidgetEvents & events);

	/// called whenever the window is resized.
	virtual void onResizeWindow();

	/// called whenever the window should be closed. return false to cancel closing.
	virtual bool onClose();


	// transforms a position according to the drag mode.
	sf::Vector2f convertDragPosition(sf::Vector2f pos) const;

	// returns the
	sf::Vector2f getDragResizeTranslation() const;


	const static float cvTitleBarHeight, cvBorderThickness, cvResizeGrab;
	const static int cvResizeAnimationMax;


	std::shared_ptr<Button> myButtonClose, myButtonMax;
	btx::BitmapText myCaption, mySizeReadout;


	bool myAutoRemove;

	unsigned int myFlags;
	bool myIsMaximized;
	sf::FloatRect myOriginalRect;
	sf::Vector2f myMinSize, myMaxSize;
	float myEnforcedRatio;

	DragMode myDragMode;
	sf::Vector2f myDragOffset, myResizeOrigin;

	std::vector<sf::Vertex> myResizeHandleVertices;
	int myResizeHandleAnimation;

	bool myTitleBarDownCache;
	sf::Vector2f myParSizeCache;


	friend class Decorations;
};

} // namespace gui2

#endif
