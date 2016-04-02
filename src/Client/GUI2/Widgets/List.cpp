#include "Client/GUI2/Widgets/List.hpp"

namespace gui2
{

Ptr<List> List::make()
{
	Ptr<List> widget = std::make_shared<List>();
	widget->init();
	return widget;
}

void List::init()
{

}

void List::onProcess(const WidgetEvents & events)
{

}

void List::onUpdateVertexCache()
{

}

} // namespace gui2
