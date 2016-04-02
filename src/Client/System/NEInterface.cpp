#include "Client/System/NEInterface.hpp"
#include "Client/System/NEWindow.hpp"
#include "Client/GUI2/Widgets/Button.hpp"

void NEInterface::init()
{
	setTitle("NecroEdit");
	
	gui2::Ptr<NEWindow> window = NEWindow::make();
	
	getRootContainer().add(window);
	
	window->loadEditor();
}
