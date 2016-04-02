#ifndef SRC_SHARED_UTILS_MESSAGEBOX_HPP_
#define SRC_SHARED_UTILS_MESSAGEBOX_HPP_

#include <string>

class MessageBox
{
public:

	enum Icon
	{
		Info,
		Warning,
		Error,
		Question
	};

	enum Buttons
	{
		Ok,
		OkCancel,
		YesNo
	};

	MessageBox(std::string message = "", std::string title = "", Icon icon = Info, Buttons buttons = Ok);
	~MessageBox();

	bool show();

	void setMessage(std::string message);
	const std::string& getMessage() const;

	void setTitle(std::string title);
	const std::string& getTitle() const;

	void setIcon(Icon icon);
	Icon getIcon() const;

	void setButtons(Buttons buttons);
	Buttons getButtons() const;

private:

	std::string message;
	std::string title;
	Icon icon;
	Buttons buttons;
};

#endif
