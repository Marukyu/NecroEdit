#ifndef GUI2_MESSAGEBOX_HPP
#define GUI2_MESSAGEBOX_HPP

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Widgets/Button.hpp>
#include <Client/GUI2/Widgets/Text.hpp>
#include <Client/GUI2/Widgets/Window.hpp>
#include <string>
#include <vector>

namespace gui2
{
class WidgetEvents;

class MessageBox : public Window
{
public:

	/// factory function.
	static Ptr<MessageBox> make();
	static Ptr<MessageBox> make(std::string message, std::string title, std::vector<std::string> buttons);

	void show();

	void setMessage(std::string message);
	std::string getMessage() const;

	void setButtons(std::vector<std::string> buttons);
	std::vector<std::string> getButtons() const;

	bool wasClosed();

	/**
	 * Returns the index of the button that was pressed on the message box, or -1 if the message box was closed using
	 * the X button.
	 */
	int getClickedButton() const;

protected:

	virtual void init() override;

private:

	void updateLayout();

	void onProcessWindow(const WidgetEvents & events) override;
	bool onClose() override;

	bool myWasClosed;
	int myClickedButton;

	Ptr<Text> myMessage;
	Ptr<Container> myButtonPanel;
	std::vector<Ptr<Button>> myButtons;
};

} // namespace gui2

#endif
