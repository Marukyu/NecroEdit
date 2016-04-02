#ifndef GUI2_LIST_HPP
#define GUI2_LIST_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class List : public Widget
{
public:

	/// factory function.
	static Ptr<List> make();

protected:

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();
};

} // namespace gui2

#endif
