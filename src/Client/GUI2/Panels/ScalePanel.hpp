#ifndef GUI2_SCALE_PANEL_HPP
#define GUI2_SCALE_PANEL_HPP

#include "Client/GUI2/Panels/Panel.hpp"

namespace gui2
{

class ScalePanel : public Panel
{
public:

	/// factory function.
	static Ptr<ScalePanel> make();

	/// factory function: places a widget inside the panel already.
	static Ptr<ScalePanel> make(Ptr<Widget> initWidget);

protected:

	virtual void init();

private:

	void onUpdatePanel();
	void onResize();
	void update(Widget * widget);

	void onAdd(std::shared_ptr<Widget> widget);
	void onRemove(std::shared_ptr<Widget> widget);
	void onNotify(Observable & subject, int message);

};

} // namespace gui2.

#endif

