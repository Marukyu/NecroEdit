#ifndef SRC_CLIENT_EDITOR_OBJECTPROPERTYPANEL_HPP_
#define SRC_CLIENT_EDITOR_OBJECTPROPERTYPANEL_HPP_

#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Panels/GridPanel.hpp>
#include <Client/GUI2/Widgets/Checkbox.hpp>
#include <Client/GUI2/Widgets/Text.hpp>
#include <Client/GUI2/Widgets/TextField.hpp>
#include <Shared/Level/Object.hpp>
#include <map>
#include <string>

namespace gui2
{
class WidgetEvents;
}

class Object;

class ObjectPropertyPanel : public gui2::Container
{
public:

	static gui2::Ptr<ObjectPropertyPanel> make();

	ObjectPropertyPanel();
	virtual ~ObjectPropertyPanel();

	void setObject(Object * object);
	Object * getObject() const;

	void update();
	void updateValues();

protected:

	virtual void init() override;

private:

	class PropertyEntry : public gui2::Container
	{
	public:
		PropertyEntry();
		virtual ~PropertyEntry();

		void setPropertyName(std::string name);
		std::string getPropertyName() const;

		virtual void setPropertyValue(std::string value) = 0;
		virtual std::string getPropertyValue() const = 0;

		virtual bool wasChanged() = 0;

	protected:

		virtual void init() override;

		void addValueWidget(gui2::Ptr<gui2::Widget> widget);

	private:

		gui2::Ptr<gui2::BorderPanel> panel;
		gui2::Ptr<gui2::Text> propertyNameText;
		std::string propertyName;
	};

	class BoolProperty : public PropertyEntry
	{
	public:

		static gui2::Ptr<BoolProperty> make();

		virtual void setPropertyValue(std::string value) override;
		virtual std::string getPropertyValue() const override;

		virtual bool wasChanged() override;

	protected:

		virtual void init() override;

	private:

		void onProcessContainer(gui2::WidgetEvents & events) override;

		gui2::Ptr<gui2::Checkbox> checkbox;
		bool currentValue;
		bool changed;
	};

	class StringProperty : public PropertyEntry
	{
	public:

		static gui2::Ptr<StringProperty> make(bool numeric);

		virtual void setPropertyValue(std::string value) override;
		virtual std::string getPropertyValue() const override;

		virtual bool wasChanged() override;

		void setNumeric(bool numeric);
		bool isNumeric() const;

	protected:

		virtual void init() override;

	private:

		gui2::Ptr<gui2::TextField> input;
	};

	gui2::Ptr<PropertyEntry> generatePropertyEntry(Object::Property property, Object::Type objectType);

	void onProcessContainer(gui2::WidgetEvents & events) override;

	Object * object;

	std::map<Object::Property, gui2::Ptr<PropertyEntry>> propertyMap;

	gui2::Ptr<gui2::BorderPanel> borderPanel;
	gui2::Ptr<gui2::GridPanel> propertyPanel;
	gui2::Ptr<gui2::Text> caption;
};

#endif
