#include <Client/LevelRenderer/LevelRenderer.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Shared/Level/Tile.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>

LevelRenderer::LevelRenderer(const Level & level, const TileAppearanceManager & tileAppearance,
		const ObjectAppearanceManager & objectAppearance) :
		level(&level),
		tileAppearance(&tileAppearance),
		objectAppearance(&objectAppearance),
		spawnPointVisualizer(Object::Type::Internal)
{
	eventListener = level.acquireEventListener();

	spawnPointVisualizer.setPropertyInt(Object::Property::Type, ObjectAppearanceManager::InternalCharacter);
	spawnPointVisualizer.setPropertyInt(Object::Property::Subtype, -1);

	for (auto it = level.tilesBegin(); it != level.tilesEnd(); ++it)
	{
		addOrSetTile(*it, level.getTileAt(*it));
	}

	for (auto it = level.objectsBegin(); it != level.objectsEnd(); ++it)
	{
		addOrSetObject(*it);
	}

	updateSpawnPoint();
}

LevelRenderer::~LevelRenderer()
{
}

void LevelRenderer::update()
{
	Level::Event event;

	while (eventListener.poll(event))
	{
		switch (event.type)
		{
		case Level::Event::TileAdded:
		case Level::Event::TileChanged:
			addOrSetTile(event.tilePosition, level->getTileAt(event.tilePosition));
			break;

		case Level::Event::TileRemoved:
			removeTile(event.tilePosition);
			break;

		case Level::Event::ObjectAdded:
		case Level::Event::ObjectChanged:
			if (level->hasObject(event.objectID))
			{
				addOrSetObject(level->getObject(event.objectID));
			}
			break;

		case Level::Event::ObjectRemoved:
			removeObject(event.objectID);
			break;

		case Level::Event::SpawnPointMoved:
			updateSpawnPoint();
			break;

		default:
			break;
		}
	}
}

void LevelRenderer::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.texture = tileAppearance->getTexture();

	for (std::size_t layer = 0; layer < tilePositions.size(); ++layer)
	{
		drawTileLayer(TileLayer(layer), target, states);
	}

	target.draw(spawnPointVertices.data(), spawnPointVertices.size(), sf::Triangles, states);

	target.draw(objectVertices.data(), objectVertices.size(), sf::Triangles, states);
}

void LevelRenderer::drawTileLayer(TileLayer layer, sf::RenderTarget& target, sf::RenderStates states) const
{
	for (std::vector<sf::Vertex> it: tileVertices[layer])
	{
		target.draw(&it[0], it.size(), sf::Triangles, states);
	}
}

namespace priv
{

bool tilePositionCompare(sf::Vector2i a, sf::Vector2i b)
{
	return a.y < b.y ? true : (a.y == b.y ? a.x > b.x : false);
}

}

void LevelRenderer::addOrSetTile(sf::Vector2i position, const Tile & tile)
{
	TileLayer layer = tile.isWall() ? Wall : Floor;
	setTileVertices(position, tileAppearance->getTileVertices(tile, position), layer);
}

void LevelRenderer::removeTile(sf::Vector2i position, TileLayer layer)
{
	if (layer == AllLayers)
	{
		for (std::size_t i = 0; i < LayerCount; ++i)
		{
			removeTile(position, TileLayer(i));
		}
	}
	else
	{
		auto it = std::lower_bound(tilePositions[layer].begin(), tilePositions[layer].end(), position,
			&priv::tilePositionCompare);

		if (it != tilePositions[layer].end() && *it == position)
		{
			tileVertices[layer].erase(tileVertices[layer].begin() + std::distance(tilePositions[layer].begin(), it));
			tilePositions[layer].erase(it);
		}
	}
}

LevelRenderer::TileLayer LevelRenderer::getTileLayer(sf::Vector2i position) const
{
	for (std::size_t layer = 0; layer < LayerCount; ++layer)
	{
		if (std::binary_search(tilePositions[layer].begin(), tilePositions[layer].end(), position,
			&priv::tilePositionCompare))
		{
			return TileLayer(layer);
		}
	}

	return LayerNone;
}

std::size_t LevelRenderer::addTileEntry(sf::Vector2i position, TileLayer layer)
{
	// Get layer-specific tile position and vertex arrays.
	auto & layerPos = tilePositions[layer];
	auto & layerVert = tileVertices[layer];

	// Get iterator to insertion position (or current tile location).
	auto it = std::lower_bound(layerPos.begin(), layerPos.end(), position, &priv::tilePositionCompare);

	// Check if a new tile needs to be inserted, or if the iterator points to the correct already existing tile.
	if (it == layerPos.end() || *it != position)
	{
		// Insert position entry.
		it = layerPos.insert(it, position);

		// Insert vertex entry based on position index.
		layerVert.insert(layerVert.begin() + std::distance(layerPos.begin(), it), TileVertexArray());
	}

	// Return tile index.
	return std::distance(layerPos.begin(), it);
}

void LevelRenderer::setTileVertices(sf::Vector2i position, TileVertexArray vertices, TileLayer layer)
{
	// Check which layer this tile exists in, if any.
	TileLayer currentLayer = getTileLayer(position);

	// If the tile exists and is in a different layer...
	if (currentLayer != LayerNone && currentLayer != layer)
	{
		// Remove it first.
		removeTile(position, currentLayer);
	}

	// Get array index for this tile, adding it if it does not exist yet.
	std::size_t tileIndex = addTileEntry(position, layer);

	// Assign vertices.
	tileVertices[layer][tileIndex] = vertices;
}

void LevelRenderer::addOrSetObject(const Object& object)
{
	std::vector<sf::Vertex> vertices = objectAppearance->getObjectVertices(object);

	setObjectVertexCount(object.getID(), vertices.size());

	std::copy(vertices.begin(), vertices.end(), objectVertices.begin() + getObjectVertexIndex(object.getID()));
}

void LevelRenderer::removeObject(Object::ID objectID)
{
	setObjectVertexCount(objectID, 0);
}

void LevelRenderer::setObjectVertexCount(Object::ID objectID, std::size_t size)
{
	std::size_t insertionIndex = getObjectVertexIndex(objectID);
	std::size_t oldSize = getObjectVertexCount(objectID);

	if (size == oldSize)
	{
		return;
	}

	expandObjectVertexArray(objectID);

	if (size > oldSize)
	{
		objectVertices.insert(objectVertices.begin() + insertionIndex + oldSize, size - oldSize, sf::Vertex());
	}
	else
	{
		objectVertices.erase(objectVertices.begin() + insertionIndex + size,
			objectVertices.begin() + insertionIndex + oldSize - size);
	}

	objectVertexCounts[objectID] = size;

	shrinkObjectVertexArray();
}

std::size_t LevelRenderer::getObjectVertexIndex(Object::ID objectID)
{
	// Check for zero ID to prevent integer underflow.
	if (objectID == 0)
	{
		return 0;
	}

	std::size_t vertexIndex = 0;
	std::size_t limit = std::min<std::size_t>(objectID, objectVertexCounts.size());

	for (std::size_t i = 0; i < limit; ++i)
	{
		vertexIndex += objectVertexCounts[i];
	}

	return vertexIndex;
}

std::size_t LevelRenderer::getObjectVertexCount(Object::ID objectID)
{
	if (objectID >= objectVertexCounts.size())
	{
		return 0;
	}
	else
	{
		return objectVertexCounts[objectID];
	}
}

void LevelRenderer::expandObjectVertexArray(Object::ID objectID)
{
	if (objectID >= objectVertexCounts.size())
	{
		objectVertexCounts.insert(objectVertexCounts.end(), objectID + 1 - objectVertexCounts.size(), 0);
	}
}

void LevelRenderer::shrinkObjectVertexArray()
{
	while (!objectVertexCounts.empty() && objectVertexCounts.back() == 0)
	{
		objectVertexCounts.pop_back();
	}
}

void LevelRenderer::updateSpawnPoint()
{
	spawnPointVisualizer.setPosition(level->getPlayerSpawnPoint());
	spawnPointVertices = objectAppearance->getObjectVertices(spawnPointVisualizer);
}
