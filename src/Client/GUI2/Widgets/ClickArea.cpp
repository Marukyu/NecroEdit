#include "Client/GUI2/Widgets/ClickArea.hpp"

namespace gui2
{

Ptr<ClickArea> ClickArea::make()
{
	Ptr<ClickArea> widget = std::make_shared<ClickArea>();
	widget->init();
	return widget;
}

void ClickArea::onUpdateVertexCache()
{
	// do nothing. no vertices are ever drawn, so none have to be cleared.
}

void ClickArea::init()
{
}

} // namespace gui2
