#ifndef SRC_SHARED_LEVEL_TILE_HPP_
#define SRC_SHARED_LEVEL_TILE_HPP_

#include <SFML/System/Vector2.hpp>

struct Tile
{
public:

	typedef int ID;

	enum Variant
	{
		Zone1,
		Zone2,
		Zone3Hot,
		Zone3Cold,
		Zone4,
		Boss,
		Zone5,
		Zone1Cracked,
		Zone2Cracked,
		Zone3HotCracked,
		Zone3ColdCracked,
		Zone4Cracked,
		BossCracked,
		Zone5Cracked,
	};

	enum Builtin : ID
	{
		Torch = -3,
		Invalid = -2,
		Erase = -1
	};

	Tile(ID id = Invalid, Variant variant = Zone1, bool hasTorch = false);

	ID id;
	Variant variant;
	bool hasTorch;
	
	bool exists() const;
	bool isWall() const;
	
	void setCracked(bool cracked);
	bool isCracked() const;
	
	void setZone(int zone);
	int getZone() const;
};

#endif
