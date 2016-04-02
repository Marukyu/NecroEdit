#include <Client/Graphics/BitmapText.hpp>
#include "Client/Graphics/UtilitiesSf.hpp"
#include "Shared/Utils/VectorMul.hpp"
#include "Shared/Utils/StrNumCon.hpp"
#include "Shared/Utils/Utilities.hpp"

#include <iostream>
#include <algorithm>
#include <cstring>

BitmapFont * BitmapFont::generate(sf::Image & image, unsigned int charFirst, float scale, sf::Color gridColor)
{
	BitmapFont * font = new BitmapFont;

	font->x = 0;
	font->y = 0;
	font->width = 0;
	font->height = 0;
	font->spacing = 1;
	font->charFirst = charFirst;
	font->scale = scale;

	// ignore ridiculously small images (better than crashing).
	if (image.getSize().x < 2 || image.getSize().y < 2)
		return nullptr;

	// first determine font size.
	for (std::size_t x = 0; x < image.getSize().x; ++x)
	{
		if (image.getPixel(x, 1) == gridColor)
		{
			font->width = x;
			break;
		}
	}

	for (std::size_t y = 0; y < image.getSize().y; ++y)
	{
		if (image.getPixel(0, y) == gridColor)
		{
			font->height = y;
			break;
		}
	}

	// in case of no/misplaced grid lines, return invalid font.
	if (font->width == 0 || font->height == 0)
		return nullptr;

	font->charsPerLine = (image.getSize().x + 1) / (font->width + 1);

	font->cwidth.clear();
	font->charLast = font->charFirst - 1;

	// true as long as character width scanner is within image bounds.
	bool withinBounds = true;

	// determine character widths.
	for (std::size_t i = 0; withinBounds; i++)
	{
		unsigned int fx = font->x + (font->width  + font->spacing) * (i % font->charsPerLine),
					 fy = font->y + (font->height + font->spacing) * (i / font->charsPerLine);
		unsigned int off;

		for (off = 0; off < font->width; off++)
		{
			if (fx + off >= image.getSize().x)
				break;

			if (fy >= image.getSize().y)
			{
				withinBounds = false;
				break;
			}

			if (image.getPixel(fx + off, fy) == gridColor)
				break;
		}

		if (withinBounds)
		{
			font->cwidth.push_back(off);
			++font->charLast;
		}
	}

	// make all background (black) and grid pixels transparent.
	image.createMaskFromColor(sf::Color(0,0,0));
	image.createMaskFromColor(gridColor);

	return font;
}


const unsigned char formatChar = 3;

const unsigned int numVertices = 6;

BitmapText::BitmapText()
{
	myString = "";
	myColor = sf::Color::White;
	myVertices.setPrimitiveType(sf::Triangles);
	myMaxSize = sf::Vector2f(-1,-1);
	myMaxSizeScale = sf::Vector2f(1,1);
	myMaxSizeBehavior = Ignore;
	mySpacing = sf::Vector2f(1,1);
	myFixedWidth = false;
	myParseModifiers = true;
	myShadowOffset = 0;
	//myFont = nullptr;
	myVertexCacheNeedUpdate = false;
	//myTabInfo = 0;
}

void BitmapText::setFont(std::shared_ptr<const BitmapFont> font)
{
	if (myFont.lock() != font)
	{
		myFont = font;
		update();
	}
}

bool BitmapText::hasFont() const
{
	return !myFont.expired();
}

const BitmapFont & BitmapText::getFont() const
{
	return *myFont.lock();
}


void BitmapText::setText(std::string text)
{
	if (myString != text)
	{
		myString = text;
		update();
	}
}

std::string BitmapText::getText() const
{
	return myString;
}


void BitmapText::setColor(sf::Color color)
{
	if (myColor != color)
	{
		myColor = color;
		update();
	}
}

sf::Color BitmapText::getColor() const
{
	return myColor;
}


sf::FloatRect BitmapText::getRect() const
{
	return sf::FloatRect(0,0,getWidth(),getHeight());
}

sf::Vector2f BitmapText::getSize() const
{
	return sf::Vector2f(getWidth(), getHeight());
}

float BitmapText::getWidth() const
{
	if (getText().empty())
		return 0;
	else
		return mySize.x * myMaxSizeScale.x;
}

float BitmapText::getHeight() const
{
	if (getText().empty())
	{
		if (hasFont())
			return getFont().height * getFont().scale;
		else
			return 0;
	}
	else
		return mySize.y * myMaxSizeScale.y;
}

sf::Vector2f BitmapText::getUnscaledSize() const
{
	if (getText().empty())
	{
		if (hasFont())
			return sf::Vector2f(0.f, getFont().height * getFont().scale);
		else
			return sf::Vector2f(0.f, 0.f);
	}
	else
		return mySize;
}


void BitmapText::setMaxSize(sf::Vector2f max)
{
	if (myMaxSize != max)
	{
		myMaxSize = max;
		updateMinor();
	}
}

sf::Vector2f BitmapText::getMaxSize() const
{
	return myMaxSize;
}


void BitmapText::setMaxSizeBehavior(MaxSizeBehavior behavior)
{
	if (myMaxSizeBehavior != behavior)
	{
		myMaxSizeBehavior = behavior;
		updateMinor();
	}
}

BitmapText::MaxSizeBehavior BitmapText::getMaxSizeBehavior() const
{
	return myMaxSizeBehavior;
}


void BitmapText::setClipOffset(sf::Vector2f clipOff)
{
	if (myClipOffset != clipOff)
	{
		myClipOffset = clipOff;
		updateMinor();
	}
}

sf::Vector2f BitmapText::getClipOffset() const
{
	return myClipOffset;
}


void BitmapText::setFixedWidth(bool enabled)
{
	if (myFixedWidth != enabled)
	{
		myFixedWidth = enabled;
		update();
	}
}

bool BitmapText::isFixedWidth() const
{
	return myFixedWidth;
}


void BitmapText::setSpacing(sf::Vector2f spacing)
{
	if (mySpacing != spacing)
	{
		mySpacing = spacing;
		update();
	}
}

sf::Vector2f BitmapText::getSpacing() const
{
	return mySpacing;
}

void BitmapText::setModifierParsing(bool parse)
{
	if (myParseModifiers != parse)
	{
		myParseModifiers = parse;
		update();
	}
}

bool BitmapText::isParsingModifers() const
{
	return myParseModifiers;
}


std::string BitmapText::formatPrefix()
{
	return std::string(1, formatChar);
}

std::string BitmapText::formatCode(FormatCode code)
{
	enum FormatCode { White, Red, Yellow, Green, Cyan, Blue, Pink, LightGray,
					  Gray, DarkRed, DarkYellow, DarkGreen, DarkCyan, DarkBlue, DarkPink, DarkGray,
					  Italic, Bold, ResetColor, ResetAll };
	std::string r = formatPrefix();
	switch (code)
	{
	case White: return r + "a";
	case Red: return r + "b";
	case Yellow: return r + "c";
	case Green: return r + "d";
	case Cyan: return r + "e";
	case Blue: return r + "f";
	case Pink: return r + "g";
	case LightGray: return r + "h";

	case Gray: return r + "A";
	case DarkRed: return r + "B";
	case DarkYellow: return r + "C";
	case DarkGreen: return r + "D";
	case DarkCyan: return r + "E";
	case DarkBlue: return r + "F";
	case DarkPink: return r + "G";
	case DarkGray: return r + "H";

	case Italic: return r + "i";
	case Bold: return r + "j";
	case ResetColor: return r + "r";
	case ResetAll: return r + "R";

	default: return "";
	}
}

std::string BitmapText::colorCode(unsigned char r, unsigned char g, unsigned char b)
{
	return formatPrefix() + "*" + std::string(1, num2Hex((r-15) / 16)) + std::string(1, num2Hex((g-15) / 16)) + std::string(1, num2Hex((b-15) / 16));
}


std::size_t BitmapText::findCharacterAt(sf::Vector2f pos) const
{
	if (!hasFont())
		return 0;

	float vpos;
	std::size_t index = 0;

	// get vertical position right first...
	while (pos.y > getCharacterPos(index).y + getFont().height * getFont().scale * getScale().y && index < getText().length())
		++index;
	vpos = getCharacterPos(index).y;

	// now horizontal position.
	while (pos.x > (getCharacterPos(index).x + getCharacterPos(index+1).x) / 2.0 && vpos <= getCharacterPos(index).y && index < getText().length())
		++index;

	return index;
}

sf::Vector2f BitmapText::getCharacterPos(std::size_t pos) const
{
	if (pos > getText().length())
		return getCharacterPos(getText().length());

	if (pos == getText().length())
	{
		if (isCharacterRendered(pos-1))
			return getText().empty() ? sf::Vector2f(0,0) : getCharacterPos(getText().length()-1) +
				sf::Vector2f((getCharacterSize(getText().length()-1).x), 0);
		else
			return getText().empty() ? sf::Vector2f(0,0) : getCharacterPos(getText().length()-1);
	}

	if (myShadowOffset + pos*4 >= myVertices.getVertexCount())
		return getScale() * (hasFont() ? sf::Vector2f(0,getFont().height * getFont().scale) : sf::Vector2f(0,0));
	else
		return getScale() * getVertexPos(myShadowOffset + pos*numVertices);
}


std::vector<sf::FloatRect> BitmapText::getCursorRectMulti(std::size_t pos, std::size_t length) const
{
	std::vector<sf::FloatRect> ret;

	length = std::max<int>(0, std::min<int>(length, getText().length() - pos));

	if (!hasFont() || pos > getText().length())
	{
		return ret;
	}
	else if (length == 0)
	{
		ret.push_back(sf::FloatRect(getCharacterPos(pos).x, getCharacterPos(pos).y, getSpacing().x, getCharacterSize(pos).y));
		return ret;
	}

	sf::FloatRect cur;

	bool resetPos = true;

	for (std::size_t i = pos; i < pos + length; ++i)
	{
		if (resetPos)
		{
			cur.left = getCharacterPos(pos).x;
			cur.top = getCharacterPos(pos).y;
			resetPos = false;
		}

		if (getText()[i] == '\n' || i == pos + length - 1)
		{
			cur.width = getCharacterPos(i).x + getCharacterSize(i).x - cur.left;
			cur.height = getCharacterSize(i).y;
			ret.push_back(cur);
			resetPos = true;
		}
	}

	return ret;
}


sf::FloatRect BitmapText::getCursorRect(std::size_t pos, std::size_t length) const
{
	return getCursorRectMulti(pos, length).at(0);
}


sf::Color BitmapText::getCharacterColor(std::size_t pos) const
{
	if (pos >= getText().length()) return getText().empty() ? getColor() : getCharacterColor(getText().length()-1);
	if (myShadowOffset + pos*numVertices >= myVertices.getVertexCount())
		return getColor();
	else
		return myVertices[myShadowOffset + pos*numVertices].color;
}

sf::Color BitmapText::getCharacterColorB(std::size_t pos) const
{
	if (pos >= getText().length()) return getText().empty() ? getColor() * sf::Color(128,128,128) : getCharacterColorB(getText().length()-1);
	if (myShadowOffset + pos*numVertices+2 >= myVertices.getVertexCount())
		return getColor() * sf::Color(128,128,128);
	else
		return myVertices[myShadowOffset + pos*numVertices+2].color;
}


float BitmapText::getRawGlyphWidth(std::size_t pos) const
{
	if (!hasFont() || pos >= getText().length()) return 0;

	int arrPos = (unsigned char)(getText()[pos]) - (unsigned char)getFont().charFirst;

	if (arrPos >= 0 && arrPos < (int)getFont().cwidth.size())
		return getFont().cwidth[arrPos];
	else
		return 0.f;
}

sf::Vector2f BitmapText::getGlyphSize(std::size_t pos) const
{
	if (!hasFont()) return sf::Vector2f(0,0);

	return sf::Vector2f(getRawGlyphWidth(pos), getFont().height) * getFont().scale;
}

float BitmapText::getGlyphOffset(std::size_t pos) const
{
	if (!hasFont()) return 0;

	return (getRawGlyphWidth(pos) + getSpacing().x) * getFont().scale;
}

sf::Vector2f BitmapText::getCharacterSize(std::size_t pos) const
{
	if (!hasFont()) return sf::Vector2f(0,0);

	if (!isCharacterRendered(pos)) return (sf::Vector2f(getSpacing().x, getFont().height) * getFont().scale) * getScale() * myMaxSizeScale;
	return sf::Vector2f(getGlyphOffset(pos), getFont().height * getFont().scale) * getScale() * myMaxSizeScale;
}


static const sf::Color TextColors[] =
{
	sf::Color(255,255,255), // white
	sf::Color(255,96 ,96 ), // red
	sf::Color(255,255,96 ), // yellow
	sf::Color(96 ,255,96 ), // green
	sf::Color(96 ,255,255), // cyan
	sf::Color(96 ,96 ,255), // blue
	sf::Color(255,96 ,255), // magenta
	sf::Color(160,160,160)  // gray
};

void BitmapText::update()
{
	if (!hasFont()) return;

	myVertices.clear();

	myMaxSizeScale = sf::Vector2f(1,1);

	sf::Vector2f charPos(0,0), charSize(0,getFont().height * getFont().scale), texPos(0,0), texSize(0,0), nextOff(0,0);
	sf::Color curColor = getColor();
	bool bold = false, italic = false, newline = false;

	enum EscapeMode
	{
		EscapeNone,
		EscapeHash,
		EscapeHexRed,
		EscapeHexGreen,
		EscapeHexBlue
	}
	escapeMode = EscapeNone;

	myCharRenderMap.resize(getText().length());

	for (std::size_t i = 0; i < getText().length(); ++i)
	{
		charSize.x = 0;

		unsigned int chr = (unsigned char)getText()[i];

		myCharRenderMap[i] = escapeMode == EscapeNone;

		if (isParsingModifers() && chr == formatChar && escapeMode != EscapeHash && i < getText().length() -1)
		{
			escapeMode = EscapeHash;
			myCharRenderMap[i] = false;
		}
		else if (escapeMode == EscapeHash && chr != formatChar)
		{
			escapeMode = EscapeNone;

			if (chr == 'r')					   // reset color.
				curColor = getColor();
			else if (chr == 'R')				  // reset all.
			{
				italic = false;
				curColor = getColor();
			}
			else if (chr == '*' && i < getText().length() -3)
			{										   // custom color determined by 3 hex digits.
				escapeMode = EscapeHexRed;
			}
			else if (chr >= 'a' && chr <= 'h')  // lowercase letter? bright color.
				curColor = TextColors[chr-'a'];

			else if (chr >= 'A' && chr <= 'H')  // uppercase letter? darker color.
			{
				curColor = TextColors[chr-'A'];
				curColor = sf::Color(curColor.r/2,curColor.g/2,curColor.b/2,curColor.a);
			}
			else if (chr == 'i')					   // lowercase i? italic text.
				italic = !italic;
			else if (chr == 'j')					   // lowercase j? bold text.
				bold = !bold;
		}
		else if (escapeMode == EscapeHexRed)
		{
			curColor.r = hex2Num(chr) * 16 + 15;
			escapeMode = EscapeHexGreen;
		}
		else if (escapeMode == EscapeHexGreen)
		{
			curColor.g = hex2Num(chr) * 16 + 15;
			escapeMode = EscapeHexBlue;
		}
		else if (escapeMode == EscapeHexBlue)
		{
			curColor.b = hex2Num(chr) * 16 + 15;
			escapeMode = EscapeNone;
		}
		else if (escapeMode == EscapeNone && chr == '\n')   // newline.
		{
			newline = true;
		}
		else if (escapeMode == EscapeNone)
		{												   // render character.
			unsigned int processedChar = chr - getFont().charFirst;
			charSize = getGlyphSize(i);

			texPos.x = getFont().x + (processedChar % getFont().charsPerLine) * (getFont().width+getFont().spacing);
			texPos.y = getFont().y + (processedChar / getFont().charsPerLine) * (getFont().height+getFont().spacing);
			texSize = sf::Vector2f(getRawGlyphWidth(i), getFont().height);

			nextOff.x += getGlyphOffset(i);

			if (getMaxSizeBehavior() == WordWrap && charPos.x + nextOff.x > getMaxSize().x && getMaxSize().x > 0)
				newline = true;
		}

		if (newline)
		{
			newline = false;
			charPos.x = 0;
			charPos.y += (getFont().height + getSpacing().y) * getFont().scale;
		}

		curColor.a = getColor().a;  // keep alpha value from original color.

		sf::Color colMul;
		if (bold)
			colMul = sf::Color(200,255,255);
		else
			colMul = sf::Color(128,128,128);

		sf::Vertex
		vTL (sf::Vector2f(charPos.x, charPos.y), curColor, sf::Vector2f(texPos.x, texPos.y)),
		vTR (sf::Vector2f(charPos.x+charSize.x, charPos.y), curColor, sf::Vector2f(texPos.x+texSize.x, texPos.y)),
		vBR (sf::Vector2f(charPos.x+charSize.x, charPos.y+charSize.y), curColor * colMul, sf::Vector2f(texPos.x+texSize.x,texPos.y+texSize.y)),
		vBL (sf::Vector2f(charPos.x,charPos.y+charSize.y), curColor * colMul, sf::Vector2f(texPos.x,texPos.y+texSize.y));

		myVertices.append(vTL);
		myVertices.append(vTR);
		myVertices.append(vBL);

		myVertices.append(vTR);
		myVertices.append(vBR);
		myVertices.append(vBL);

		if (italic)
		{
			myVertices[myVertices.getVertexCount()-1].position.x -= 1.5f;
			myVertices[myVertices.getVertexCount()-2].position.x -= 1.5f;
			myVertices[myVertices.getVertexCount()-3].position.x += 1.5f;
			myVertices[myVertices.getVertexCount()-4].position.x -= 1.5f;
			myVertices[myVertices.getVertexCount()-5].position.x += 1.5f;
			myVertices[myVertices.getVertexCount()-6].position.x += 1.5f;
		}

		charPos += nextOff;
		nextOff = sf::Vector2f(0,0);
	}

	// reset max size when recalculating text vertices.
	myMaxSizeScale.x = 1.f;
	myMaxSizeScale.y = 1.f;

	// allocate vertices for shadow.
	myShadowOffset = myVertices.getVertexCount();
	for (std::size_t i = 0; i < myShadowOffset; ++i)
		myVertices.append(myVertices[i]);

	// calculate bounds.
	sf::FloatRect vertBounds = calculateBounds();
	mySize.x = vertBounds.width;
	mySize.y = vertBounds.height;

	// shadow.
	for (std::size_t i = 0; i < myShadowOffset; ++i)
	{
		myVertices[i].position = myVertices[myShadowOffset+i].position + sf::Vector2f(1,1);	 // offset shadow by 1,1.
		myVertices[i].color = sf::Color(0,0,0,myVertices[myShadowOffset+i].color.a/2);		  // semi-transparent.
	}

	updateMinorImpl();
}

void BitmapText::updateMinor()
{
	if (getMaxSizeBehavior() == WordWrap)
	{
		update();
	}
	else
	{
		updateMinorImpl();
	}
}

void BitmapText::updateMinorImpl()
{
	myMaxSizeScale.x = 1.f;
	myMaxSizeScale.y = 1.f;

	myMaxSizeTransform = sf::Transform::Identity;

	if (getMaxSizeBehavior() == Downscale)
	{
		bool msDownscale = false;

		if ((mySize.x * getScale().x > getMaxSize().x && getMaxSize().x > 0.0001f))
		{
			msDownscale = true;
			myMaxSizeScale.x = getMaxSize().x / (mySize.x * getScale().x);
		}
		if ((mySize.y * getScale().y > getMaxSize().y && getMaxSize().y > 0.0001f))
		{
			msDownscale = true;
			myMaxSizeScale.y = getMaxSize().y / (mySize.y * getScale().y);
		}

		if (msDownscale)
		{
			myMaxSizeTransform.scale(myMaxSizeScale);
		}
	}

	myVertexCacheNeedUpdate = true;
}

const std::vector<sf::Vertex> & BitmapText::getVertices() const
{
	if (myVertexCacheNeedUpdate)
	{
		myVertexCache.resize(myVertices.getVertexCount());
		std::memcpy(&myVertexCache[0], &myVertices[0], myVertices.getVertexCount() * sizeof(sf::Vertex));

		if (myMaxSizeBehavior == Clip)
		{
			if (myClipOffset != sf::Vector2f(0,0))
			{
				for (sf::Vertex & vertex : myVertexCache)
					vertex.position -= myClipOffset;
			}
			clipVertices(myVertexCache, sf::FloatRect(0, 0, getMaxSize().x, getMaxSize().y));
		}

		sf::Transform transform = getTransform();

		if (myMaxSizeBehavior == Downscale)
		{
			transform *= myMaxSizeTransform;
		}

		for (sf::Vertex & vertex : myVertexCache)
			vertex.position = transform.transformPoint(vertex.position);

		myVertexCacheNeedUpdate = false;
	}
	return myVertexCache;
}



const BitmapFont * BitmapText::globalFont;

void BitmapText::setGlobalFont(const BitmapFont * font)
{
	globalFont = font;
}

const BitmapFont & BitmapText::getGlobalFont()
{
	return *globalFont;
}

bool BitmapText::globalFontExists()
{
	return globalFont != 0;
}


void BitmapText::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	if (myMaxSizeBehavior == Downscale)
		states.transform *= myMaxSizeTransform;

	if (hasFont())
	{
		states.texture = getFont().texture;
	}

	if (myMaxSizeBehavior == Clip)
	{
		sf::Transform origTransform = states.transform;
		states.transform.translate(myClipOffset);
		drawClipped(myVertices, target, states, origTransform.transformRect(sf::FloatRect(0, 0, getMaxSize().x, getMaxSize().y)));
	}
	else
		target.draw(myVertices, states);
}

sf::Vector2f BitmapText::getVertexPos(unsigned int index) const
{
	return myMaxSizeTransform.transformPoint(myVertices[index].position);
}


sf::FloatRect BitmapText::calculateBounds() const
{
	// adapted code from sfml source.
	if (myVertices.getVertexCount() > myShadowOffset)
	{
		float left   = myVertices[myShadowOffset].position.x;
		float top	= myVertices[myShadowOffset].position.y;
		float right  = myVertices[myShadowOffset].position.x;
		float bottom = myVertices[myShadowOffset].position.y;

		for (std::size_t i = myShadowOffset + 1; i < myVertices.getVertexCount(); ++i)
		{
			sf::Vector2f position = myVertices[i].position;

			// update left and right.
			if (position.x < left)
				left = position.x;
			else if (position.x > right)
				right = position.x;

			// update top and bottom.
			if (position.y < top)
				top = position.y;
			else if (position.y > bottom)
				bottom = position.y;
		}

		return sf::FloatRect(left, top, right - left, bottom - top);
	}
	else
	{
		return sf::FloatRect();
	}
}


bool BitmapText::isCharacterRendered(std::size_t pos) const
{
	if (pos >= myCharRenderMap.size())
		return false;
	return myCharRenderMap[pos];
}
