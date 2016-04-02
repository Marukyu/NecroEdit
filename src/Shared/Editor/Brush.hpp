#ifndef SRC_SHARED_EDITOR_BRUSH_HPP_
#define SRC_SHARED_EDITOR_BRUSH_HPP_

#include <Shared/Level/Object.hpp>
#include <Shared/Level/Tile.hpp>

/**
 *
 */
class Brush
{
public:

	enum class TileMode
	{
		Ignore,
		ReplaceAny,
		ReplaceExisting,
		ReplaceEmpty,
		Erase
	};

	enum class TileMask
	{
		None = 0,

		ID = 1 << 0,
		Zone = 1 << 1,
		Torch = 1 << 2,
		Cracked = 1 << 3,

		Full = ID | Zone | Torch | Cracked
	};
	
	enum class ObjectMode
	{
		Ignore,
		Add,
		ReplaceAll,
		PlaceOnEmpty,
		ReplaceTopMost,
		ReplaceTopMostOrCreate,
		EraseAll,
		EraseTopMost
	};

	Brush();
	Brush(const Brush & brush);
	Brush & operator=(const Brush & brush);

	void setTile(Tile tile);
	Tile getTile() const;

	void setTileMode(TileMode tileMode);
	TileMode getTileMode() const;

	void setTileMask(TileMask tileMask);
	TileMask getTileMask() const;
	
	void setObject(const Object & object);
	const Object & getObject() const;

	void setObjectMode(ObjectMode objectMode);
	ObjectMode getObjectMode() const;

private:

	Tile tile;
	TileMode tileMode;
	TileMask tileMask;
	Object object;
	ObjectMode objectMode;
};

inline constexpr Brush::TileMask operator|(Brush::TileMask a, Brush::TileMask b)
{
	return Brush::TileMask(int(a) | int(b));
}

inline constexpr Brush::TileMask operator&(Brush::TileMask a, Brush::TileMask b)
{
	return Brush::TileMask(int(a) & int(b));
}

#endif
