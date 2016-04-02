#ifndef GUI2_BUTTON_HPP
#define GUI2_BUTTON_HPP

#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class Button : public TextWidget
{
public:

	/// factory function.
	static Ptr<Button> make();

	/// factory function with button text.
	static Ptr<Button> make(std::string buttonText);

	void setActivated(bool activated);
	bool isActivated() const;

protected:

	sf::FloatRect getTextMaxBoundingBox() const;

	virtual void init();

private:

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void onTextResize();

	bool myIsActivated;
};

} // namespace gui2

#endif
