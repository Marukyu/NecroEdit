#ifndef GUI2_CONTAINER_HPP
#define GUI2_CONTAINER_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Internal/WidgetEvents.hpp"

#include <memory>
#include <unordered_map>
#include <iterator>


namespace gui2
{

class Interface;

/**
 * a widget that can hold and manage other widgets.
 */
class Container : public Widget
{
	struct WidgetInfo;

public:

	/// factory function.
	static Ptr<Container> make();

	/// destructor.
	~Container();

	/// adds the specified widget to the container. overloaded by panels
	/// for positioning parameters. calls onAdd().
	void add(std::shared_ptr<Widget> widget);

	/// removes the specified widget from the container. does nothing if
	/// the widget is not within this container.
	void remove(std::shared_ptr<Widget> widget);

	/// returns true if the specified widget is within this container.
	bool isContained(std::shared_ptr<const Widget> widget) const;
	bool isContained(const Widget * widget) const;

	/// sets/gets the currently focused widget. null pointer means no
	/// focused widget.
	void setFocusedWidget(std::shared_ptr<Widget> widget);
	std::shared_ptr<Widget> getFocusedWidget() const;

	/// checks if the specified widget or any of its focus-pass-through
	/// parent widgets are focused.
	bool isWidgetFocused(std::shared_ptr<const Widget> widget) const;

	/// marks the specified widget's vertices for redrawing.
	void invalidateWidgetVertices(std::shared_ptr<Widget> widget);

	/// marks all widgets' vertices for redrawing.
	void invalidateAllWidgetVertices();

	/// sends the specified widget to the top/bottom of its z-position
	/// group.
	void sendWidgetToFront(std::shared_ptr<Widget> widget);
	void sendWidgetToBack(std::shared_ptr<Widget> widget);

	/// reinserts the widget to the correct z-position group.
	void updateWidgetZPosition(std::shared_ptr<Widget> widget);


	/// returns true if the widget supports returning its vertices as
	/// consistently textured triangles, or false if the special render()
	/// function should be called to draw the widget instead. (slower)
	virtual bool isVertexRenderable() const;


	/// returns the transformation this container applies to its widgets.
	virtual sf::Transform getContainerTransform() const;

	/// returns the inner bounding box for widget positioning.
	virtual sf::FloatRect getContainerBoundingBox() const;

	/// returns the inner bounding box for restricting widget rendering.
	virtual sf::FloatRect getContainerClipBox() const;

	/// returns true if this container clips widgets.
	virtual bool isClippingWidgets() const;

	/// returns true if this container manages widget focus.
	virtual bool isManagingFocus() const;


	/// automatically makes the specified observer monitor all current
	/// and future widgets in this container.
	void addWidgetObserver(Observer * observer);

	/// un-observes all widgets from the specified observer. must be
	/// called before the specified observer object is destroyed.
	void removeWidgetObserver(Observer * observer);

	/// returns true if the specified observer is already auto-monitoring
	/// this container's widgets.
	bool isWidgetObserver(Observer * observer) const;


	/// possible notification types for observers.
	enum NotifyMessagesContainer
	{
		NotifyContainerBox = NotifyCount,
		NotifyContainerTransform,

		NotifyCountContainer
	};

	/// iterator over widgets.
	class Iterator
	{
	public:

		typedef std::random_access_iterator_tag iterator_category;
		typedef std::shared_ptr<Widget> value_type;
		typedef std::ptrdiff_t difference_type;
		typedef std::shared_ptr<Widget> * pointer;
		typedef std::shared_ptr<Widget> & reference;

		inline bool operator==(const Iterator & i2) const
			{ return myIt != i2.myIt; }
		inline bool operator!=(const Iterator & i2) const
			{ return myIt != i2.myIt; }
		inline const Iterator & operator++()
			{ ++myIt; return *this;}
		inline Iterator operator++(int)
			{ Iterator old(myIt++); return old;}
		inline const Iterator & operator--()
			{ --myIt; return *this;}
		inline Iterator operator--(int)
			{ Iterator old(myIt--); return old;}
		inline void operator+=(difference_type b)
			{ myIt += b; }
		inline void operator-=(difference_type b)
			{ myIt -= b; }
		inline Iterator operator+(difference_type b)
			{ return Iterator(myIt + b); }
		inline Iterator operator-(difference_type b)
			{ return Iterator(myIt - b); }
		inline difference_type operator-(const Iterator & i2) const
			{ return myIt - i2.myIt; }
		inline value_type operator*() const
			{ return myIt->widget; }
		inline value_type operator->() const
			{ return myIt->widget; }

	private:

		inline Iterator(std::vector<WidgetInfo>::const_iterator it) :
			myIt(it)
		{}

		std::vector<WidgetInfo>::const_iterator myIt;

		friend class Container;
	};

	inline Iterator begin() const
	{
		return Iterator(myWidgets.begin());
	}
	inline Iterator end() const
	{
		return Iterator(myWidgets.end());
	}

protected:

	/// initializes the container. must be called by subclass init() if overridden.
	virtual void init();

	/// adds a widget to the container without invoking onAdd(). returns true if
	/// widget was added successfully, false otherwise.
	bool doAdd(std::shared_ptr<Widget> widget);

	// called when the application's fonts are updated.
	virtual void onUpdateFonts();

	// called whenever the container's parent is changed.
	virtual void onParent(std::shared_ptr<Container> oldParent);

private:

	// invoked when add() is called.
	virtual void onAdd(std::shared_ptr<Widget> widget);

	// invoked when remove() is called.
	virtual void onRemove(std::shared_ptr<Widget> widget);

	// overridable processing function. called by onProcess() after processing
	// this container's widgets, but before updating their vertices.
	virtual void onProcessContainer(WidgetEvents & events);

	// renders the container. TODO: description and stuff.
	virtual void onDraw(sf::RenderTarget & target, sf::RenderStates states) const;

	// adds vertices to the container's vertex cache with untransformed vertices.
	// prepend/append determine whether the vertices will be inserted before or
	// after the vertices of the contained widgets.
	// do NOT call clearVertices() within these functions.
	virtual void onUpdateVertexCachePrepend();
	virtual void onUpdateVertexCacheAppend();

	// builds a vertex cache for this container's vertex-renderable widgets.
	void onUpdateVertexCache();

	// prevents unnecessary vertex cache updates on mouse actions.
	virtual void onMouseOver();
	virtual void onMouseAway();
	virtual void onMouseDown(sf::Vector2f pos, Input button);
	virtual void onMouseUp(sf::Vector2f pos, Input button);

	// called on resize, updates all widget vertices and notifies for container
	// box change.
	virtual void onResize();

	// processes this widget. subclasses should override onProcessContainer().
	void onProcess(const WidgetEvents & events);

	// called by getParentInterface() on unparented widgets.
	// returns the parent interface for root containers.
	virtual std::shared_ptr<Interface> onGetParentInterface() const;


	// returns the parent interface's font.
	//virtual std::shared_ptr<const btx::Font> onGetFont(int index) const;


	// constructs a container linked to an interface.
	void setParentInterface(std::shared_ptr<Interface> interface);

	// moves the widget at the specified index to the destination index,
	// updating the vertex cache and widget index map correctly.
	void moveWidget(std::size_t sourcePos, std::size_t destPos);


	void debug() const;


	struct WidgetInfo
	{
		WidgetInfo(std::shared_ptr<Widget> widget) :
			widget(widget),
			cachedVertexCount(0),
			verticesInvalid(true)
		{}

		std::shared_ptr<Widget> widget;
		std::size_t cachedVertexCount;
		bool verticesInvalid;
	};

	// a vector holding this container's widgets.
	std::vector<WidgetInfo> myWidgets;

	// maps each widget pointer to its index in the widget vector.
	std::map<std::weak_ptr<const Widget>, std::size_t, std::owner_less<std::weak_ptr<const Widget> > > myWidgetIndexMap;

	// true if all widgets in this container are vertex-renderable.
	bool myIsVertexRenderable;

	// true if all widgets' vertices are invalid.
	bool myAllVerticesInvalid;

	// number of vertices before/after the widget vertices.
	std::size_t myVertexCountPrepend, myVertexCountAppend;

	// a pointer to this container's parent interface.
	std::weak_ptr<Interface> myParentInterface;

	// a pointer to the currently focused widget.
	std::weak_ptr<Widget> myFocusedWidget;

	// a list of pointers to current widget observers.
	std::set<Observer*> myWidgetObservers;

	friend class Interface;
	friend class Application;
};

} // namespace gui2

#endif
