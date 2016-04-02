#ifndef GUI2_INPUT_HPP
#define GUI2_INPUT_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

class TextField : public TextWidget
{
public:

	/// factory function.
	static Ptr<TextField> make();

	/// factory function: pre-entered text.
	static Ptr<TextField> make(std::string text);


	void setCursorPosition(unsigned int pos);
	unsigned int getCursorPosition() const;

	void setSelectionStart(unsigned int start);
	unsigned int getSelectionStart() const;

	void setSelectionEnd(unsigned int start);
	unsigned int getSelectionEnd() const;

	int getSelectionLength() const;

	std::string getSelectedText() const;

	enum InputType
	{
		InputText,
		InputMultiLine,
		InputNumeric,
		InputInteger
	};
	void setInputType(InputType type);
	InputType getInputType() const;

	void setMouseWheelEnabled(bool enabled);
	bool isMouseWheelEnabled() const;

	void setMouseWheelFactor(float factor);
	float getMouseWheelFactor() const;

	void setReadOnly(bool readOnly);
	bool isReadOnly() const;

	void setMaxLength(unsigned int maxLength);
	unsigned int getMaxLength() const;

	bool wasChanged();

	bool isMenuFocused() const;

	sf::FloatRect getTextMaxBoundingBox() const;

protected:

	virtual void init();

private:

	const static float cvMargin;

	enum SpecialInputs {
		KeyCodeLeft = 16,
		KeyCodeRight = 17,
		KeyCodeUp = 18,
		KeyCodeDown = 19,

		KeyCodeBackSpace = 8,
		KeyCodeDelete = 127,
		KeyCodeTab = 9,
		KeyCodeEnter = 10
	};


	void onMouseDown(sf::Vector2f pos, Input button);

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void onTextChange();

	sf::Vector2f convertPosText(sf::Vector2f pos) const;
	void eraseSelection();


	//Menu myMenu;

	//GUIBox myInputBox;
	//std::vector<GradientRect> myTextCursors;

	InputType myInputType;
	float myMouseWheelFactor;
	unsigned int myCursorStart, myCursorEnd, myMaxLength;
	bool myIsReadOnly, myUseMouseWheel, myWasChanged;
};

} // namespace gui2

#endif
