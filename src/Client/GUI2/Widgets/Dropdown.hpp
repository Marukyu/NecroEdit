#ifndef GUI2_DROPDOWN_HPP
#define GUI2_DROPDOWN_HPP

#include <Client/GUI2/Widgets/TextField.hpp>
#include "Client/GUI2/Widgets/Button.hpp"
#include "Client/GUI2/Widgets/Menu.hpp"

namespace gui2
{

class Dropdown : public Widget, private Observer
{
public:

	/// factory function.
	static Ptr<Dropdown> make();

	void setText(std::string text);
	void setTextSilent(std::string text);
	std::string getText() const;

	void setSelection(int item);
	void setSelectionSilent(int item);
	int getSelection() const;

	void setModel(std::shared_ptr<AbstractMenuModel> model);
	std::shared_ptr<AbstractMenuModel> getModel() const;

	void setVerticalFlip(bool flip);
	bool isVerticalFlipped() const;

	bool wasChanged();

protected:

	virtual void init();

private:

	// pass-through menu model filter that only modifies the menu's checkboxes.
	class DropdownModel : public AbstractMenuModel
	{
	public:

		DropdownModel(std::shared_ptr<AbstractMenuModel> model);

		std::size_t getItemCount() const;
		std::string getItem(std::size_t index) const;
		bool isEnabled(std::size_t index) const;
		bool isChecked(std::size_t index) const; // < this one is changed.
		bool isSeparator(std::size_t index) const;

		void setCheckboxIndex(int index);
		int getCheckboxIndex() const;

	private:

		std::shared_ptr<AbstractMenuModel> myModel;

		int myCheckboxIndex;

		friend class Dropdown;
	};


	void onNotify(Observable & subject, int message);

	void onResize();
	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void updateSelection();


	std::shared_ptr<TextField> myInput;
	std::shared_ptr<Button> myButton;
	std::shared_ptr<Menu> myMenu;

	std::shared_ptr<AbstractMenuModel> myModel;
	std::shared_ptr<DropdownModel> myDropdownModel;

	bool myWasChanged;
};

} // namespace gui2

#endif
