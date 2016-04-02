#ifndef GUI2_BORDER_PANEL_HPP
#define GUI2_BORDER_PANEL_HPP

#include "Client/GUI2/Panels/Panel.hpp"

namespace gui2
{

class BorderPanel : public Panel
{
public:

	/// factory function.
	static Ptr<BorderPanel> make();

	enum Orientation
	{
		Center,
		Left,
		Right,
		Top,
		Bottom,

		OrientationCount
	};

	void add(std::shared_ptr<Widget> widget, Orientation orientation);
	void add(std::shared_ptr<Widget> widget, Orientation orientation, float size);
	void addPos(std::shared_ptr<Widget> widget, Orientation orientation, std::size_t index);
	void addPos(std::shared_ptr<Widget> widget, Orientation orientation, float size, std::size_t index);

	void place(std::shared_ptr<Widget> widget, Orientation orientation);
	void place(std::shared_ptr<Widget> widget, Orientation orientation, float size);
	void placePos(std::shared_ptr<Widget> widget, Orientation orientation, std::size_t index);
	void placePos(std::shared_ptr<Widget> widget, Orientation orientation, float size, std::size_t index);

	bool update(std::shared_ptr<Widget> widget);

	void setResizable(Orientation side, bool resizable);
	bool isResizable(Orientation side) const;

	void setSideSize(Orientation side, float minSize);
	float getSideSize(Orientation side) const;

	void setMinimumSize(Orientation side, float minSize);
	float getMinimumSize(Orientation side) const;

	void setMaximumSize(Orientation side, float maxSize);
	float getMaximumSize(Orientation side) const;

protected:

	virtual void init();
	virtual void onResize();

private:

	class ResizeHandle : public Widget
	{
	public:
		/// factory function.
		static Ptr<ResizeHandle> make();

		void setParentPanel(std::shared_ptr<BorderPanel> parent);
		void setSide(Orientation side);

	private:
		void onUpdateVertexCache();
		void onProcess(const WidgetEvents & events);
		void onMouseDown(sf::Vector2f pos, Input button);

		std::weak_ptr<BorderPanel> myParentPanel;
		sf::Vector2f myClickOffset;
		float myInitialSize;
		Orientation mySide;
	};

	struct PanelData
	{
		std::weak_ptr<Widget> widget;
		float offset, size;
	};


	void updateAutoSize(Orientation orientation, std::size_t index);
	bool update(Orientation orientation, std::size_t index);
	bool updateSingle(Orientation orientation, std::size_t index);

	void updateResizeHandle(Orientation side);

	void onUpdatePanel();
	void onAdd(std::shared_ptr<Widget> widget);
	void onRemove(std::shared_ptr<Widget> widget);
	void onNotify(Observable & subject, int message);

	//void addPlacementEntry(std::shared_ptr<Widget> widget, Orientation orientation, std::size_t index);
	void removePlacementEntry(std::shared_ptr<Widget> widget);

	float getSideOffset(Orientation side) const;
	float getEffectiveSideSize(Orientation side) const;

	std::pair<Orientation, std::size_t> retrieve(const Widget * widget) const;


	std::vector<std::vector<PanelData> > myPlacements;

	struct SideSetting
	{
		SideSetting() :
			resizable(false),
			size(0.f),
			minSize(0.f),
			maxSize(-1.f),
			scale(1.f),
			resizeHandle(nullptr)
		{}

		bool resizable;
		float size, minSize, maxSize;
		float scale;
		std::shared_ptr<ResizeHandle> resizeHandle;
	};
	std::vector<SideSetting> mySideSettings;
};

} // namespace gui2.

#endif


