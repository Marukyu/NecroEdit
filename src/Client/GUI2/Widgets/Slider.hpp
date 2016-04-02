#ifndef GUI2_SLIDER_HPP
#define GUI2_SLIDER_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"
#include "Shared/Utils/Timer.hpp"

namespace gui2
{

class Slider : public TextWidget
{
public:

	/// factory function.
	static Ptr<Slider> make();

	void setValue(float value);
	float getValue() const;

	void setBounds(float min, float max);
	float getMin() const;
	float getMax() const;

	void setVertical(bool vertical);
	bool isVertical() const;

	void displayText(bool text);
	void displayText(bool text, bool always);
	bool isDisplayingText() const;

	void alwaysDisplayText(bool text);
	bool isAlwaysDisplayingText() const;

	enum Style
	{
		StyleFull,
		StyleThin,
		StyleTransparent
	};

	enum NotifyMessagesSlider
	{
		NotifySliderValueChanged = NotifyCount
	};

	void setStyle(Style style);
	Style getStyle() const;

protected:

	virtual void init();

private:

	const static float cvSliderSize, cvSliderLineSize;

	sf::FloatRect getSliderRect() const;
	float posToValue(sf::Vector2f pos) const;

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void onResize();

	void updateSliderText();
	sf::FloatRect getTextMaxBoundingBox() const;


	float myValue, myMin, myMax;
	bool myIsVertical, myDisplayText, myAlwaysDisplayText;
	Style myStyle;

	Countdown myHighlightTimer;
};

} // namespace gui2

#endif
