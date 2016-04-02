#ifndef GUI2_TIMERBUTTON_HPP
#define GUI2_TIMERBUTTON_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class TimerButton : public Widget
{
public:

	/// factory function.
	static Ptr<TimerButton> make();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
