#ifndef GUI2_TOOLTIP_HPP
#define GUI2_TOOLTIP_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class Tooltip : public Widget
{
public:

	/// factory function.
	static Ptr<Tooltip> make();

protected:

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
