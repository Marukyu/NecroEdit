#include "Shared/Level/Tile.hpp"

Tile::Tile(ID id, Variant variant, bool hasTorch) : id(id), variant(variant), hasTorch(hasTorch)
{
}

bool Tile::exists() const
{
	return id >= 0;
}

bool Tile::isWall() const
{
	return id >= 100;
}

void Tile::setCracked(bool cracked)
{
	int var = variant;
	
	var %= Zone1Cracked;

	if (cracked)
	{
		var += Zone1Cracked;
	}
	
	variant = (Variant)var;
}

bool Tile::isCracked() const
{
	return variant >= Zone1Cracked;
}

void Tile::setZone(int zone)
{
	bool cracked = isCracked();
	variant = (Variant)zone;
	setCracked(cracked);
}

int Tile::getZone() const
{
	return variant % Zone1Cracked;
}
