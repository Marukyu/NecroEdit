#ifndef SRC_CLIENT_EDITOR_TOOLS_TILETOOL_HPP_
#define SRC_CLIENT_EDITOR_TOOLS_TILETOOL_HPP_

#include <Client/Editor/Tools/BrushTool.hpp>
#include <Client/Editor/Tools/Panels/TileToolPanel.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Shared/Editor/Brush.hpp>

class TileTool : public BrushTool
{
public:
	TileTool();
	virtual ~TileTool();

	virtual void onInit() override;

	virtual gui2::Ptr<gui2::Widget> getSettingsPanel() const override;

	/**
	 * Returns the tooltip for this tile tool.
	 */
	virtual std::string getTooltip() const override;

protected:

	virtual const Brush & getPrimaryBrush() const override;
	virtual const Brush & getSecondaryBrush() const override;

private:

	mutable Brush primaryBrush;
	Brush secondaryBrush;

	gui2::Ptr<TileToolPanel> settingsPanel;
};

#endif
