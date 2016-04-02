#ifndef GUI2_SCROLLCONTAINER_HPP
#define GUI2_SCROLLCONTAINER_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class ScrollContainer : public Widget
{
public:

	/// factory function.
	static Ptr<ScrollContainer> make();

protected:

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
