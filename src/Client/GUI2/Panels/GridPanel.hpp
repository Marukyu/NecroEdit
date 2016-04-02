#ifndef GUI2_GRID_PANEL_HPP
#define GUI2_GRID_PANEL_HPP

#include "Client/GUI2/Panels/Panel.hpp"

namespace gui2
{

class GridPanel : public Panel
{
public:

	/// factory function.
	static Ptr<GridPanel> make();

	/// factory function: single widget.
	static Ptr<GridPanel> make(Ptr<Widget> initWidget);

	/// factory function: predefined grid size.
	static Ptr<GridPanel> make(sf::Vector2u gridSize);
	static Ptr<GridPanel> make(std::size_t width, std::size_t height);

	void setGridSize(sf::Vector2u gridSize);
	void setGridSize(std::size_t width, std::size_t height);
	sf::Vector2u getGridSize() const;

	void setAutoCellSize(sf::Vector2f cellSize);
	void setAutoCellSize(float cellWidth, float cellHeight);
	sf::Vector2f getAutoCellSize() const;

	void add(Ptr<Widget> widget);
	void add(Ptr<Widget> widget, sf::Vector2u pos);
	void add(Ptr<Widget> widget, std::size_t x, std::size_t y);

	void place(Ptr<Widget> widget);
	void place(Ptr<Widget> widget, sf::Vector2u pos);
	void place(Ptr<Widget> widget, std::size_t x, std::size_t y);

	bool update(Ptr<Widget> widget);

protected:

	virtual void init();

private:

	struct PanelData
	{
		sf::Vector2u position;
	};


	bool overrideAutoSize() const;
	void onUpdatePanel();
	void onResize();

	void onAdd(Ptr<Widget> widget);
	void onRemove(Ptr<Widget> widget);

	void onUpdateAutoCellSize();


	sf::Vector2u myGridSize;
	sf::Vector2f myAutoCellSize;
	sf::Vector2u myLastPlacement;

	std::map<std::weak_ptr<Widget>, PanelData, std::owner_less<std::weak_ptr<Widget> > > myPlacements;

};

} // namespace gui2.

#endif
