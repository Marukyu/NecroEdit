#ifndef GUI2_WIDGET_HPP
#define GUI2_WIDGET_HPP

#include <Client/Graphics/BitmapText.hpp>
#include "Client/GUI2/GUI.hpp"
#include "Client/GUI2/Internal/WidgetEvents.hpp"
#include "Client/GUI2/Generator.hpp"
#include "Shared/Utils/Observer.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <memory>
#include <set>

namespace gui2
{

class Interface;
class Container;

/**
 * base class for GUI2 widgets. do not use directly.
 * manages a drawable, interactable object held within containers.
 */
class Widget : public std::enable_shared_from_this<Widget>, public sf::Drawable, public Observable
{
public:

	/// alignment specifiers.
	enum Alignment
	{
		AlignTopLeft,
		AlignTop,
		AlignTopRight,

		AlignLeft,
		AlignCenter,
		AlignRight,

		AlignBottomLeft,
		AlignBottom,
		AlignBottomRight
	};

	/// destructor.
	virtual ~Widget();

	/// sets the widget's position to the specified value.
	void setPosition(sf::Vector2f pos);
	void setPosition(float x, float y);

	/// returns the widget's position.
	sf::Vector2f getPosition() const;

	/// sets the widget's size to the specified value.
	void setSize(sf::Vector2f size);
	void setSize(float width, float height);

	/// returns the widget's size.
	sf::Vector2f getSize() const;

	/// sets the widget's position and size to the specified rectangle.
	void setRect(sf::FloatRect rect);
	void setRect(float x, float y, float width, float height);

	/// returns the widget's axis-aligned bounding box.
	sf::FloatRect getRect() const;


	/// sets the widget's size managing behavior. set to true if widget
	/// should automatically resize horizontally/vertically to fit its
	/// content, or false to let the user or panel determine the size.
	/// some widgets enable this setting by default, others may ignore
	/// it entirely.
	void setAutoManagingSize(bool autoManage);
	void setAutoManagingWidth(bool autoManage);
	void setAutoManagingHeight(bool autoManage);

	/// returns true if the widget is automatically managing its width/
	/// height, false if the user or panel decides the respective size.
	bool isAutoManagingWidth() const;
	bool isAutoManagingHeight() const;

	/// returns the widget's automatically calculated preferred size.
	sf::Vector2f getAutomaticSize() const;


	/// enables/disables the widget's responsiveness to input.
	void setEnabled(bool enabled);

	/// checks the widget's responsiveness to input.
	bool isEnabled() const;

	/// shows/hides the widget.
	void setVisible(bool visible);

	/// checks if the widget is shown or hidden.
	bool isVisible() const;


	/// checks if the widget is focused.
	bool isFocused() const;

	/// gives focus to the widget.
	void acquireFocus();


	/// sets this widget's z-position. higher number = in front.
	void setZPosition(int zpos);

	/// returns this widget's z-position.
	int getZPosition() const;

	/// sends this widget to the topmost/bottommost position among all
	/// widgets with the same z-position in the container.
	void sendToFront();
	void sendToBack();


	/// links another widget to this widget. linked widgets inherit the
	/// linking widget's transform, parent, visibility/usability status,
	/// and z-position.
	void link(std::shared_ptr<Widget> widget);

	/// unlinks a linked widget from this widget.
	void unlink(std::shared_ptr<Widget> widget);

	/// checks if the specified widget is linked to this widget.
	bool isLinked(std::shared_ptr<Widget> widget) const;

	/// returns a set of all linked widgets.
	const std::set<std::shared_ptr<Widget> > & getLinkedWidgets() const;

	/// returns a pointer to this widget's linking parent if it is linked,
	/// and a null pointer otherwise.
	std::shared_ptr<Widget> getLinkParent() const;

	/// returns true if this widget has a link parent, false otherwise.
	bool hasLinkParent() const;

	/// checks if this widget's link parent equals the specified widget,
	/// and (if false) calls this function on its link parent.
	bool checkLinkParentRecursive(std::shared_ptr<Widget> parent) const;


	/// checks if the mouse is currently over the widget.
	bool isMouseOver() const;

	/// checks if the mouse was pressed on the widget.
	bool isMouseDown(const Input & button = sf::Mouse::Left) const;

	/// checks if the mouse is currently pressing on the widget.
	bool isMousePressing(const Input & button = sf::Mouse::Left) const;

	/// checks if the widget was clicked in this frame.
	bool isClicked(const Input & button = sf::Mouse::Left) const;


	/// returns true if the widget supports returning its vertices as
	/// consistently textured triangles, or false if the special render()
	/// function should be called to draw the widget instead. (slower)
	virtual bool isVertexRenderable() const;

	/// returns the immediate transformation applied to this widget.
	sf::Transform getTransform() const;

	/// returns the inverse of the immediate transformation applied to this widget.
	sf::Transform getInverseTransform() const;


	/// invalidates this widget's and all parent containers' vertices.
	/// should also be called by a widget when the return value of
	/// isVertexRenderable() changes.
	void invalidateVertices();

	/// invalidates this widget's transform, but not its vertices. also
	/// notifies the parent container that the widget should be redrawn.
	void invalidateTransform();

	/// notifies the parent container that the widget should be redrawn.
	void invalidateRender();


	/// returns this widget's pretransformed triangle vertices.
	const std::vector<sf::Vertex> & getVertices() const;


	/// called once per GUI update frame. calls onProcess().
	void process(const WidgetEvents & events);


	/// returns this widget's parent container.
	std::shared_ptr<Container> getParent() const;

	/// returns the interface this widget is part of.
	std::shared_ptr<Interface> getParentInterface() const;


	/// returns the horizontal alignment component of an Alignment enum.
	static Alignment convertAlignmentHorizontal(Alignment align);

	/// returns the vertical alignment component of an Alignment enum.
	static Alignment convertAlignmentVertical(Alignment align);

	/// returns the combined alignment from horizontal and vertical components.
	static Alignment combineAlignments(Alignment horiz, Alignment vert);


	/// possible notification types for observers.
	enum NotifyMessages
	{
		NotifyMoved,
		NotifyResized,
		NotifyAutoSized,
		NotifyInvalidated,
		NotifyReparented,

		NotifyCount
	};


protected:

	/// default constructor. creates an invalid widget. use the static function "make" instead,
	/// or init() for subclass initialization.
	Widget();

	/// called after the widget is created and has a shared pointer, but before it is displayed.
	virtual void init();

	/// returns the widget's untransformed, local rectangle.
	sf::FloatRect getBaseRect() const;

	/// sets the widget's preferred size.
	void setAutomaticSize(sf::Vector2f autoSize);


	struct BoxStyle
	{
		BoxStyle();

		float borderThickness;
		bool hovered;   // mouse-over (blue).
		bool pressed;   // mouse-button (invert gradient).
		bool focused;   // currently focused (green-ish).
		bool enabled;   // interactable (opaque).
		bool activated; // enabled/switched on (bright green).
		sf::Color color;
		sf::Color outlineColor;
	};

	enum BoxType
	{
		BoxTypeBasic,
		BoxTypeDark,
		BoxTypeBackground
	};

	/// returns the automatically determined box style for this widget.
	BoxStyle getDefaultBoxStyle() const;

	/// clears the vertex cache.
	void clearVertices();

	/// adds vertices to the vertex cache, giving them a flat white texture.
	void vertexAdd(const std::vector<sf::Vertex> & vertices);

	/// adds vertices to the vertex cache, retaining their texture coordinates.
	void vertexAddTextured(const std::vector<sf::Vertex> & vertices);

	/// renders a box to the widget's vertex cache, by default the widget's bounding box.
	void vertexAddBox(BoxType type = BoxTypeBasic);
	void vertexAddBox(BoxType type, const BoxStyle & style);
	void vertexAddBox(sf::FloatRect rect, BoxType type = BoxTypeBasic);
	void vertexAddBox(sf::FloatRect rect, BoxType type, const BoxStyle & style);

	/// adds a flat/gradient rectangle to the widget's vertex cache.
	void vertexAddRect(sf::FloatRect rect, sf::Color color);
	void vertexAddRectHGradient(sf::FloatRect rect, sf::Color colorLeft, sf::Color colorRight);
	void vertexAddRectVGradient(sf::FloatRect rect, sf::Color colorTop, sf::Color colorBottom);
	void vertexAddRect(sf::FloatRect rect, sf::Color colorTL, sf::Color colorTR, sf::Color colorBR, sf::Color colorBL);

	/// returns one of the parent application's loaded fonts.
	std::shared_ptr<const BitmapFont> getDefaultFont() const;
	std::shared_ptr<const BitmapFont> getDefaultFont(int index) const;

	/// the widget's vertex cache.
	std::vector<sf::Vertex> myVertexCache;

private:

	/// called once per GUI update frame.
	virtual void onProcess(const WidgetEvents & events);

	/// draws the widget to a render target. default implementation:
	/// renders the widget's vertices to the render target.
	virtual void onDraw(sf::RenderTarget & target, sf::RenderStates states) const;

	/// fills this widget's vertex cache with untransformed vertices.
	/// they are automatically transformed afterwards.
	virtual void onUpdateVertexCache();

	/// called when the mouse enters this widget's clickable area.
	/// invalidates vertices by default.
	virtual void onMouseOver();

	/// called when the mouse leaves this widget's clickable area.
	/// invalidates vertices by default.
	virtual void onMouseAway();

	/// called when a mouse button is pressed down on the widget.
	/// invalidates vertices by default.
	virtual void onMouseDown(sf::Vector2f pos, Input button);

	/// called when a mouse button that was pressed on the widget is released.
	/// invalidates vertices by default.
	virtual void onMouseUp(sf::Vector2f pos, Input button);

	/// called when a mouse button is pressed down and released on the widget.
	/// does nothing by default.
	virtual void onClick(sf::Vector2f pos, Input button);

	/// called when the widget receives focus.
	/// invalidates vertices by default.
	virtual void onFocus();

	/// called when the widget loses focus.
	/// invalidates vertices by default.
	virtual void onUnfocus();

	/// called whenever the widget's size is set.
	/// invalidates vertices by default.
	virtual void onResize();

	/// called whenever the widget's parent is changed.
	virtual void onParent(std::shared_ptr<Container> oldParent);

	/// called when the application's fonts are updated.
	virtual void onUpdateFonts();

	/// returns true if the specified local mouse position is considered
	/// within the widget's mouseover/click area.
	virtual bool checkMouseover(sf::Vector2f pos) const;

	/// returns one of the parent application's loaded fonts.
	//virtual std::shared_ptr<const btx::Font> onGetFont(int index) const;

	// called by getParentInterface() on unparented widgets. only relevant for containers.
	virtual std::shared_ptr<Interface> onGetParentInterface() const;


	// draws the widget to a render target. calls onDraw().
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	// sets this widget's parent container. called by the container's add() function.
	void setParent(std::shared_ptr<Container> parent);

	// sets this widget's parent link widget. called by another widget in link().
	void setLinkParent(std::shared_ptr<Widget> linkParent);

	// updates this widget's vertex cache if necessary and clears the "need update" flag.
	void updateVertexCache();


	typedef sf::Uint8 MouseBitmask;

	// converts a mouse button to a unique bitmask.
	static MouseBitmask convertMouseButtonBitmask(Input button);


	// widget position.
	sf::Vector2f myPosition;

	// widget size.
	sf::Vector2f mySize;

	// automatic size management flags.
	bool myIsAutoManagingWidth, myIsAutoManagingHeight;

	// automatic
	sf::Vector2f myAutomaticSize;

	// widget responsive status.
	bool myIsEnabled;

	// widget visibility status.
	bool myIsVisible;

	// widget z-position.
	int myZPosition;

	// list of widgets linked to this widget.
	std::set<std::shared_ptr<Widget> > myLinkedWidgets;

	// widget that this widget is linked to, if it exists.
	std::weak_ptr<Widget> myLinkParent;

	// event status flags. MouseDown and Clicked hold bits for each mosue button.
	bool myIsMouseOver;
	MouseBitmask myIsMouseDown, myIsClicked;

	// the widget's cached transformation matrix/inverse and validity flags.
	mutable sf::Transform myTransform, myInverseTransform;
	mutable bool myIsTransformInvalid, myIsInverseTransformInvalid;

	// true if vertex cache needs updating.
	bool myIsVertexCacheInvalid;

	// pointer to this widget's parent.
	std::weak_ptr<Container> myParent;


	friend class Interface;
	friend class Container;

	template <typename T> friend std::shared_ptr<T> make();

};

} // namespace gui2

#endif
