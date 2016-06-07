#ifndef WOS_INTERFACE_HPP
#define WOS_INTERFACE_HPP

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Interface.hpp>
#include <Client/System/NEWindow.hpp>

class NEInterface : public gui2::Interface
{
private:

	void init() override;

	void onProcess(const gui2::WidgetEvents & events) override;

	void onClose() override;

	gui2::Ptr<NEWindow> window;
};

#endif
