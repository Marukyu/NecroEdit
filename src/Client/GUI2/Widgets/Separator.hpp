#ifndef GUI2_SEPARATOR_HPP
#define GUI2_SEPARATOR_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class Separator : public Widget
{
public:

	/// factory function.
	static Ptr<Separator> make();

protected:

	virtual void init();

private:

	void onUpdateVertexCache();
};

} // namespace gui2

#endif
