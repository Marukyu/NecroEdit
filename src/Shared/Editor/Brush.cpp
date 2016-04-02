#include <Shared/Editor/Brush.hpp>

Brush::Brush() :
	tile(),
	tileMode(TileMode::Ignore),
	tileMask(TileMask::None),
	object(),
	objectMode(ObjectMode::Ignore)
{
}

Brush::Brush(const Brush& brush) :
			tile(brush.tile),
			tileMode(brush.tileMode),
			tileMask(brush.tileMask),
			object(brush.object),
			objectMode(brush.objectMode)
{
}

Brush& Brush::operator =(const Brush& brush)
{
	tile = brush.tile;
	tileMode = brush.tileMode;
	tileMask = brush.tileMask;
	object = Object(brush.object);
	objectMode = brush.objectMode;
	return *this;
}

void Brush::setTile(Tile tile)
{
	this->tile = tile;
}

Tile Brush::getTile() const
{
	return tile;
}

void Brush::setTileMask(TileMask tileMask)
{
	this->tileMask = tileMask;
}

void Brush::setTileMode(TileMode tileMode)
{
	this->tileMode = tileMode;
}

Brush::TileMode Brush::getTileMode() const
{
	return tileMode;
}

Brush::TileMask Brush::getTileMask() const
{
	return tileMask;
}

void Brush::setObject(const Object& object)
{
	this->object = Object(object);
}

const Object& Brush::getObject() const
{
	return object;
}

void Brush::setObjectMode(ObjectMode objectMode)
{
	this->objectMode = objectMode;
}

Brush::ObjectMode Brush::getObjectMode() const
{
	return objectMode;
}
