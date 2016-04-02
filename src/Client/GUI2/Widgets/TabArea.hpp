#ifndef GUI2_TABAREA_HPP
#define GUI2_TABAREA_HPP

#include "Client/GUI2/Widget.hpp"

#include <vector>

namespace gui2
{

class Button;
class Container;

class TabArea : public Widget
{
public:

	/// factory function.
	static Ptr<TabArea> make();

	void setTabs(std::string names, const std::string & delimiter = ";");
	void setTabs(const std::vector<std::string> & names);

	void setTabCount(std::size_t tabCount);
	std::size_t getTabCount() const;

	void insertTab(std::size_t index, std::size_t count = 1);
	void eraseTab(std::size_t index, std::size_t count = 1);

	void setTabName(std::size_t index, std::string name);
	std::string getTabName(std::size_t index) const;

	std::shared_ptr<Container> tab(std::size_t index) const;

	void setSelectedTab(std::size_t index);
	std::size_t getSelectedTab();

protected:

	virtual void init();

private:

	static const float cvButtonHeight;

	void onResize();
	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void updateTabContainers();
	void updateTabButtons();
	void showInactiveTabs();
	void hideInactiveTabs();
	void initTab(std::size_t index);

	struct TabData
	{
		std::shared_ptr<Button> button;
		std::shared_ptr<Button> closeButton;
		std::shared_ptr<Container> container;
	};


	std::vector<TabData> myTabs;
	std::shared_ptr<Container> myButtonContainer;
	std::shared_ptr<Container> myContainer;
	std::size_t mySelectedTab;
	bool myIsAnimating, myIsFontLoaded;
	float myTabUnitOffset, myButtonOffset;

	sf::Clock myAnimClock;
};

} // namespace gui2

#endif
