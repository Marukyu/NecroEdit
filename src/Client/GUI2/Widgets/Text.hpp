#ifndef GUI2_TEXT_HPP
#define GUI2_TEXT_HPP

#include "Client/GUI2/Widget.hpp"
#include "Client/GUI2/Widgets/TextWidget.hpp"

namespace gui2
{

/**
 * displays one or more lines of text. see TextWidget for functions.
 */
class Text : public TextWidget
{

public:

	/// factory function.
	static Ptr<Text> make();

	/// factory function: predefined text.
	static Ptr<Text> make(std::string text);

protected:

	virtual void init();

private:

	void onUpdateVertexCache();
};

} // namespace gui2

#endif
