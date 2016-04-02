#ifndef GUI2_FILLCONTAINER_HPP
#define GUI2_FILLCONTAINER_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class FillContainer : public Widget
{
public:

	/// factory function.
	static Ptr<FillContainer> make();

protected:

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
