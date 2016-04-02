#ifndef GUI2_TEXT_WIDGET_HPP
#define GUI2_TEXT_WIDGET_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class TextWidget : public Widget
{
public:

	/// sets/gets the widget's text.
	void setText(std::string text);
	void setText(std::string text, float size);
	std::string getText() const;

	/// sets the text without calling onSetText().
	void setTextSilent(std::string text);

	/// sets/gets the text's alignment.
	void setTextAlignment(Alignment align);
	Alignment getTextAlignment() const;

	/// sets/gets the text's precise alignment.
	void setTextPreciseAlignment(sf::Vector2f align);
	void setTextPreciseAlignment(float alignX, float alignY);
	sf::Vector2f getTextPreciseAlignment() const;

	/// sets/gets the text's scaling factor.
	void setTextScale(float scale);
	float getTextScale() const;

	/// sets/gets the text's base color.
	void setTextColor(sf::Color color);
	sf::Color getTextColor() const;

	/// sets/gets the text's font.
	void setTextFont(int fontIndex);
	int getTextFont() const;

	/// sets/gets how the text should behave when the maximum size is exceeded.
	void setTextMaxSizeBehavior(btx::BitmapText::MaxSizeBehavior behavior);
	btx::BitmapText::MaxSizeBehavior getTextMaxSizeBehavior() const;

	/// sets/gets the text's rendering offset when using clipping maximum size mode.
	void setTextClipOffset(sf::Vector2f offset);
	sf::Vector2f getTextClipOffset() const;

	/// sets/gets whether the text should have its coordinates rounded.
	void setTextPixelPerfect(bool perfect);
	bool isTextPixelPerfect() const;

	/// returns the text's bounds.
	sf::Vector2f getTextPosition() const;
	sf::Vector2f getTextSize() const;
	sf::Vector2f getTextUnscaledSize() const;
	sf::FloatRect getTextRect() const;

	/// attempts to load the text's current font if it is not loaded already.
	/// calls onLoadFont() if successful.
	void loadTextFont();
	bool isTextFontLoaded() const;

protected:

	/// called when the text widget is initialized.
	virtual void init();

	/// returns the text's local bounding box, by default getBaseRect().
	virtual sf::FloatRect getTextMaxBoundingBox() const;

	/// adds the text's vertices to the cache. should be called in onUpdateVertexCache().
	void vertexAddText();

	/// accessor for the internal text object.
	const btx::BitmapText & getTextObject() const;

	/// calculates the text's position and size limits based on widget size and alignment.
	void recalculateText();

private:

	/// called everytime the text is set.
	virtual void onTextChange();

	/// called when the font is initially loaded.
	virtual void onLoadFont();

	/// called whenever the text size might change.
	virtual void onTextResize();

	/// called whenever the parent has new font data available.
	virtual void onUpdateFonts();


	sf::Vector2f myTextAlignment;
	bool myIsTextPixelPerfect;
	int myFontIndex;
	sf::Color myTextColor;

	mutable BitmapText myText;

};

} // namespace gui2.

#endif
