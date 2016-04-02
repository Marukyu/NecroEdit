#ifndef SRC_CLIENT_EDITOR_SELECTIONPANEL_HPP_
#define SRC_CLIENT_EDITOR_SELECTIONPANEL_HPP_

#include <Client/Game/Input.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Widgets/Slider.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace sf
{
class RenderTarget;
}

namespace gui2
{
class WidgetEvents;
}

/**
 * A panel containing all placable tiles and allowing for the selection of one tile.
 */
class SelectionPanel : public gui2::Widget
{
public:

	using ID = std::size_t;

	struct ItemEntry
	{
		std::vector<sf::Vertex> vertices;
		std::string name;
	};

	using Mapper = std::function<const ItemEntry &(ID)>;

	/**
	 * Factory function.
	 */
	static gui2::Ptr<SelectionPanel> make();

	void setSelection(ID selection);
	void clearSelection();

	ID getSelection() const;
	bool hasSelection() const;

	void setMapper(Mapper mapper, std::size_t itemCount);
	const Mapper & getMapper() const;
	std::size_t getItemCount() const;

	void setItemSize(sf::Vector2f itemSize);
	sf::Vector2f getItemSize() const;

	void setTexture(const sf::Texture * texture);
	const sf::Texture * getTexture() const;

	void update();
	
	bool wasChanged();

	SelectionPanel();

	bool isVertexRenderable() const override;
	
protected:
	
	virtual void init() override;

private:

	void onProcess(const gui2::WidgetEvents & events) override;
	void onDraw(sf::RenderTarget & target, sf::RenderStates states) const;
	void onResize() override;
	
	void onMouseDown(sf::Vector2f pos, Input button) override;

	std::size_t getRowCount() const;
	std::size_t getColumnCount() const;

	sf::FloatRect getItemRect(ID item) const;
	sf::FloatRect getShiftedItemRect(ID item) const;

	Mapper mapper;

	const sf::Texture * texture;

	bool wasSelectionChanged;
	bool selectionExists;
	std::size_t selection;
	std::size_t itemCount;
	sf::Vector2f itemSize;

	std::vector<sf::Vertex> vertices;
	
	gui2::Ptr<gui2::Slider> slider;
	float scrollVelocity;
};

#endif
