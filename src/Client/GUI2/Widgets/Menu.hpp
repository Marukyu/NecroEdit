#ifndef GUI2_MENU_HPP
#define GUI2_MENU_HPP

#include "Client/GUI2/Widget.hpp"
#include "Shared/Utils/Observer.hpp"

namespace gui2
{

class Menu;

class AbstractMenuModel : public Observable
{
public:

	virtual std::size_t getItemCount() const = 0;
	virtual std::string getItem(std::size_t index) const = 0;
	virtual bool isEnabled(std::size_t index) const = 0;
	virtual bool isChecked(std::size_t index) const = 0;
	virtual bool isSeparator(std::size_t index) const = 0;
};


class DefaultMenuModel : public AbstractMenuModel
{
public:

	DefaultMenuModel();
	DefaultMenuModel(const std::string & items, const std::string & delimiter = ";", bool emptySeparators = true);
	DefaultMenuModel(const std::vector<std::string> & items);

	void setItems(const std::string & items, const std::string & delimiter = ";", bool emptySeparators = true);
	void setItems(const std::vector<std::string> & items, bool emptySeparators = true);

	void clear();

	void addItem(std::string text, bool enabled = true, bool checked = false);
	void addSeparator();
	void insertItem(std::size_t index, std::string text, bool enabled = true, bool checked = false);
	void insertSeparator(std::size_t index);
	void removeItem(std::size_t index);

	void setItem(std::size_t index, std::string text);
	void setEnabled(std::size_t index, bool enabled);
	void setChecked(std::size_t index, bool checked);
	void setSeparator(std::size_t index, bool separator);

	std::size_t getItemCount() const;
	std::string getItem(std::size_t index) const;
	bool isEnabled(std::size_t index) const;
	bool isChecked(std::size_t index) const;
	bool isSeparator(std::size_t index) const;

private:

	struct ItemData
	{
		ItemData(std::string text = "", bool enabled = true, bool checked = false, bool separator = false) :
			text(text),
			enabled(enabled),
			checked(checked),
			separator(separator)
		{}

		std::string text;
		bool enabled, checked, separator;
	};

	std::vector<ItemData> myItems;

};


class Menu : public Widget, private Observer
{
public:

	/// factory function.
	static Ptr<Menu> make();

	void show(sf::Vector2f pos);
	void show(float x, float y);

	int getSelectedItem() const;

	void setModel(std::shared_ptr<AbstractMenuModel> model);
	std::shared_ptr<AbstractMenuModel> getModel() const;

	void setMinimumWidth(float minWidth);
	float getMinimumWidth() const;

	void setVerticalFlip(bool flip);
	bool isVerticalFlipped() const;

protected:

	virtual void init();

private:

	static const float cvPadding, cvItemHeight;

	void onNotify(Observable & subject, int message);

	void onProcess(const WidgetEvents & events);
	void onUpdateVertexCache();

	void invalidateMenu();
	void updateMenu();
	void updateSize();


	std::vector<std::unique_ptr<btx::BitmapText> > myTexts;

	std::shared_ptr<AbstractMenuModel> myModel;

	bool myIsMenuInvalid;

	int mySelectedItem, myMouseOverItem;
	float myMinimumWidth;
	float myMenuWidth;
	bool myIsVerticalFlipped;

};

} // namespace gui2

#endif
