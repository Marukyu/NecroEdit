#include "Client/GUI2/Widgets/Text.hpp"

namespace gui2
{

Ptr<Text> Text::make()
{
	Ptr<Text> widget = std::make_shared<Text>();
	widget->init();
	return widget;
}

Ptr<Text> Text::make(std::string text)
{
	Ptr<Text> widget = make();
	widget->setText(text);
	return widget;
}

void Text::init()
{
	TextWidget::init();

	setTextAlignment(AlignLeft);
	setTextPixelPerfect(true);
}

void Text::onUpdateVertexCache()
{
	clearVertices();
	vertexAddText();
}

} // namespace gui2
