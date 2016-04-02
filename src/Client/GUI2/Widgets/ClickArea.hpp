#ifndef GUI2_CLICKAREA_HPP
#define GUI2_CLICKAREA_HPP

#include "Client/GUI2/Widget.hpp"

namespace gui2
{

class ClickArea : public Widget
{
public:

	/// factory function.
	static Ptr<ClickArea> make();

protected:

	virtual void init();

private:

	void onUpdateVertexCache();
};

} // namespace gui2

#endif
