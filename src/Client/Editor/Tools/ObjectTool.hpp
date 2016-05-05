#ifndef SRC_CLIENT_EDITOR_TOOLS_OBJECTTOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_OBJECTTOOL_HPP_

#include <Client/Editor/Tools/BrushTool.hpp>
#include <Client/Editor/Tools/Panels/ObjectToolPanel.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Shared/Editor/Brush.hpp>
#include <Shared/Level/Object.hpp>
#include <vector>

class ObjectTool : public BrushTool
{
public:
	ObjectTool(std::vector<Object::Type> objectTypes);
	virtual ~ObjectTool();

	virtual void onInit() override;

	virtual gui2::Ptr<gui2::Widget> getSettingsPanel() const override;

protected:

	virtual const Brush & getPrimaryBrush() const override;
	virtual const Brush & getSecondaryBrush() const override;

private:

	std::vector<Object::Type> objectTypes;

	mutable Brush primaryBrush;
	mutable Brush secondaryBrush;

	gui2::Ptr<ObjectToolPanel> settingsPanel;
};

#endif
