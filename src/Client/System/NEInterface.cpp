#include "Client/System/NEInterface.hpp"
#include "Client/System/NEWindow.hpp"
#include "Client/GUI2/Widgets/Button.hpp"

void NEInterface::init()
{
	setTitle("NecroEdit");

	window = NEWindow::make();

	getRootContainer().add(window);

	window->loadEditor();
}

void NEInterface::onProcess(const gui2::WidgetEvents& events)
{
	if (window == nullptr || !window->isOpen())
	{
		closeWindow();
	}
}

void NEInterface::onClose()
{
	if (window != nullptr && window->isOpen())
	{
		window->requestClose();
	}
}
