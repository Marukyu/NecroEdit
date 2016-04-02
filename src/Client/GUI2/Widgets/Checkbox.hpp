#ifndef GUI2_CHECKBOX_HPP
#define GUI2_CHECKBOX_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class Checkbox : public TextWidget
{
public:

	/// factory function.
	static Ptr<Checkbox> make();

	void setChecked(bool checked);
	bool isChecked() const;

	enum NotifyMessagesCheckbox
	{
		NotifyCheckboxToggled = NotifyCount
	};

protected:

	virtual void init();

	sf::FloatRect getTextMaxBoundingBox() const;

	float getCheckboxSize() const;

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	bool myIsChecked;
};

} // namespace gui2

#endif
