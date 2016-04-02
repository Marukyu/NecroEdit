#ifndef BITMAP_TEXT_HPP
#define BITMAP_TEXT_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

struct BitmapFont
{
	const sf::Texture * texture;	 // IN:  texture to be used for rendering.
	unsigned int x, y;				  // IN:  starting pos of glyphs.
	unsigned int width, height;		 // IN:  glyph grid size.
	unsigned int charsPerLine;		  // IN:  number glyphs per row in the grid.
	unsigned int charFirst, charLast;   // IN:  first and last glyph index.
	unsigned int spacing;			   // IN:  spacing between characters in grid. usually 1.
	float scale;						// IN:  automatically applied scale to font.
	std::vector<unsigned char> cwidth;  // OUT: individual glyph width, autoprocessed.

	// processes all font parameters from an image. the image will have to be converted to a texture or
	// placed in a texture atlas, which is then assigned to the "texture" attribute.
	static BitmapFont * generate(sf::Image & image, unsigned int charFirst = 0, float scale = 1.f, sf::Color gridColor =
			sf::Color(128, 128, 128));

private:

	BitmapFont() :
			texture(0),
			x(0),
			y(0),
			width(8),
			height(8),
			charsPerLine(16),
			charFirst(0),
			charLast(0),
			spacing(1),
			scale(1.f),
			cwidth(1)
	{
	}
};

class BitmapText : public sf::Transformable, public sf::Drawable
{
public:
	BitmapText();

	void setFont(std::shared_ptr<const BitmapFont> font);   /// change text font.
	bool hasFont() const;					   /// check if a font is available.
	const BitmapFont & getFont() const;			   /// get font to be used.

	void setText(std::string text);			 /// specify text to render.
	std::string getText() const;				/// get current text.

	void setColor(sf::Color color);			 /// set base color.
	sf::Color getColor() const;				 /// get base color.

	sf::FloatRect getRect() const;			  /// get outer rectangle.
	sf::Vector2f getSize() const;			   /// get text size.
	float getWidth() const;					 /// get text width.
	float getHeight() const;					/// get text height.

	sf::Vector2f getUnscaledSize() const;	   /// returns the text size without max-size downscaling.

	void setMaxSize(sf::Vector2f max);		  /// set maximum size of text, scale down if too large.
	sf::Vector2f getMaxSize() const;			/// get maximum text size.

	enum MaxSizeBehavior
	{
		Ignore,		 // ignores size restriction. (default)
		Downscale,	  // reduce text scale to fit text in.
		WordWrap,	   // insert newlines if text does not fit in.
		Clip			// clips the text if it does not fit.
	};
	void setMaxSizeBehavior(MaxSizeBehavior behavior);	/// specify action to take when maximum size is exceeded.
	MaxSizeBehavior getMaxSizeBehavior() const;		   /// get the blablabla from above.

	void setClipOffset(sf::Vector2f clipOff); /// moves the text to the specified offset to change the area revealed after clipping.
	sf::Vector2f getClipOffset() const;	/// moves the text to the specified offset to change the area revealed after clipping.

	void setFixedWidth(bool enabled);		   /// toggle fixed width mode.
	bool isFixedWidth() const;				  /// check if fixed width is on.

	void setSpacing(sf::Vector2f spacing);	  /// set horizontal and vertical spacing between characters.
	sf::Vector2f getSpacing() const;			/// get spacing between characters.

	void setModifierParsing(bool parse);/// toggle whether formatting modifiers should be shown as plain text or parsed. (default = on)
	bool isParsingModifers() const;			 /// check whether the above is set to true.

	static std::string formatPrefix();		  /// returns the formatting code prefix.

	enum FormatCode
	{
		White,
		Red,
		Yellow,
		Green,
		Cyan,
		Blue,
		Pink,
		LightGray,
		Gray,
		DarkRed,
		DarkYellow,
		DarkGreen,
		DarkCyan,
		DarkBlue,
		DarkPink,
		DarkGray,
		Italic,
		Bold,
		ResetColor,
		ResetAll
	};
	static std::string formatCode(FormatCode code);
	static std::string colorCode(unsigned char r, unsigned char g, unsigned char b);

	std::size_t findCharacterAt(sf::Vector2f pos) const;	/// find the chracter at a certain position.
	sf::Vector2f getCharacterPos(std::size_t pos) const;/// retrieve position of the character at a certain position.

	/// returns an array of rectangles as a text cursor at a specific position or a selection rectangle
	/// around multiple characters. multiline-compatible.
	std::vector<sf::FloatRect> getCursorRectMulti(std::size_t pos, std::size_t length) const;

	/// returns the size of a text cursor around a character. (deprecated, not multiline-compatible!)
	sf::FloatRect getCursorRect(std::size_t pos, std::size_t length) const;

	sf::Color getCharacterColor(std::size_t pos) const;		/// retrieve color of the character at a certain position.
	sf::Color getCharacterColorB(std::size_t pos) const;		/// retrieve lower gradient color of the char at pos.

	float getRawGlyphWidth(std::size_t pos) const;/// return untransformed size of the character at the given position.
	sf::Vector2f getGlyphSize(std::size_t pos) const;			 /// return size of the character at the given position.
	float getGlyphOffset(std::size_t pos) const;	/// return horizontal offset of the character at the given position.
	sf::Vector2f getCharacterSize(std::size_t pos) const;/// return size of a given character in the string, including its offset.

	void update();							  /// update vertices and size bounds.
	void updateMinor();						 /// update size bounds and shadow only.

	const std::vector<sf::Vertex> & getVertices() const;	/// returns this text's vertices.

	static void setGlobalFont(const BitmapFont * font);
	static const BitmapFont & getGlobalFont();
	static bool globalFontExists();

private:

	/// draw text to the target.
	void draw(sf::RenderTarget & target, sf::RenderStates states = sf::RenderStates::Default) const;

	/// returns the transformed position of one of the vertices.
	sf::Vector2f getVertexPos(unsigned int index) const;

	/// returns a floatrect surrounding the whole text, exlcuding the shadow.
	sf::FloatRect calculateBounds() const;

	/// actual function for minor update to avoid infinite recursion with word-wrap.
	void updateMinorImpl();

	/// is this character visibly rendered?
	bool isCharacterRendered(std::size_t pos) const;

	sf::VertexArray myVertices;

	mutable std::vector<sf::Vertex> myVertexCache;
	mutable bool myVertexCacheNeedUpdate;

	std::string myString;
	std::weak_ptr<const BitmapFont> myFont;

	std::vector<bool> myCharRenderMap;

	sf::Color myColor;		  /// default base color.

	sf::Vector2f myMaxSize;
	MaxSizeBehavior myMaxSizeBehavior;
	sf::Transform myMaxSizeTransform;
	sf::Vector2f myMaxSizeScale;
	sf::Vector2f myClipOffset;

	sf::Vector2f mySpacing;
	bool myFixedWidth;
	bool myParseModifiers;

	sf::Vector2f mySize;		/// rendered text size.
	std::size_t myShadowOffset; /// number to add to vertex index to skip shadow vertices.

	void updateVertexData();	/// recalculate the position, color and texture coords of all vertices.
	void updateMaxSizeScale();  /// downscale all vertices if maximum size is exceeded.

	static const BitmapFont * globalFont;
};

namespace btx
{
	using BitmapFont = ::BitmapFont;
	using BitmapText = ::BitmapText;
}

#endif
