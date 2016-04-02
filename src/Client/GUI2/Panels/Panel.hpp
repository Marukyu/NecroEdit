#ifndef GUI2_PANEL_HPP
#define GUI2_PANEL_HPP

#include "Client/GUI2/Container.hpp"

namespace gui2
{

class Panel : public Container, protected Observer
{
public:

	void setMargin(float margin);
	float getMargin() const;

	void setPadding(float padding);
	float getPadding() const;

	void setPixelPerfect(bool perfect);
	bool isPixelPerfect() const;

protected:

	virtual void init();

	void onNotify(Observable & subject, int message);

private:

	virtual void onUpdatePanel();

	virtual bool overrideAutoSize() const;

	void onParent(std::shared_ptr<Container> oldParent);
	bool isManagingFocus() const;

	float myMargin, myPadding;
	bool myIsPixelPerfect;
};

} // namespace gui2.

#endif

