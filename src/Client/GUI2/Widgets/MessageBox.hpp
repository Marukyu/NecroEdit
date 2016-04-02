#ifndef GUI2_MESSAGEBOX_HPP
#define GUI2_MESSAGEBOX_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class MessageBox : public Widget
{
public:

	/// factory function.
	static Ptr<MessageBox> make();

protected:

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
