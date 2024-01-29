// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Disable the tile at position tilePos, disabling rendering at this location
	*
	* \param tilePos Position of the tile to disable
	*
	* \see DisableTiles
	*/
	inline void Tilemap::DisableTile(const Vector2ui& tilePos)
	{
		NazaraAssert(tilePos.x < m_mapSize.x && tilePos.y < m_mapSize.y, "Tile position is out of bounds");

		std::size_t tileIndex = tilePos.y * m_mapSize.x + tilePos.x;
		Tile& tile = m_tiles[tileIndex];
		if (tile.enabled)
		{
			tile.enabled = false;

			Layer& layer = m_layers[tile.layerIndex];
			layer.enabledTiles.Reset(tileIndex);
			layer.enabledTileCount = layer.enabledTiles.Count();
		}

		InvalidateVertices();
	}

	/*!
	* \brief Disable all tiles
	*/
	inline void Tilemap::DisableTiles()
	{
		for (Tile& tile : m_tiles)
			tile.enabled = false;

		for (Layer& layer : m_layers)
		{
			layer.enabledTiles.Reset();
			layer.enabledTileCount = 0;
		}

		InvalidateVertices();
	}

	/*!
	* \brief Disable tileCount tiles at positions contained at tilesPos location, disabling rendering at those locations
	*
	* This is equivalent to calling tileCount times DisableTile with the positions contained at tilesPos
	*
	* \param tilesPos Pointer to a valid array of at least tileCount positions
	* \param tileCount Number of tiles to disable
	*
	* \remark if tileCount is zero, this is a no-op and the value of tilesPos is not used
	*
	* \see DisableTile
	*/
	inline void Tilemap::DisableTiles(const Vector2ui* tilesPos, std::size_t tileCount)
	{
		NazaraAssert(tilesPos || tileCount == 0, "Invalid tile position array with a non-zero tileCount");

		for (std::size_t i = 0; i < tileCount; ++i)
		{
			NazaraAssert(tilesPos->x < m_mapSize.x&& tilesPos->y < m_mapSize.y, "Tile position is out of bounds");

			std::size_t tileIndex = tilesPos->y * m_mapSize.x + tilesPos->x;
			Tile& tile = m_tiles[tileIndex];

			if (tile.enabled)
			{
				tile.enabled = false;

				Layer& layer = m_layers[tile.layerIndex];
				layer.enabledTiles.Reset(tileIndex);
			}

			tilesPos++;
		}

		for (Layer& layer : m_layers)
			layer.enabledTileCount = layer.enabledTiles.Count();

		if (tileCount > 0)
			InvalidateVertices();
	}

	/*!
	* \brief Enable/Disable isometric mode
	*
	* If enabled, every odd line will overlap by half the tile size with the upper line
	*
	* \param isometric Should the isometric mode be enabled for this Tilemap
	*
	* \see IsIsometricModeEnabled
	*/
	inline void Tilemap::EnableIsometricMode(bool isometric)
	{
		m_isometricModeEnabled = isometric;

		InvalidateVertices();
	}

	/*!
	* \brief Enable and sets the tile at position tilePos
	*
	* Setup the tile at position tilePos using color, normalized coordinates coords and materialIndex
	*
	* \param tilePos Position of the tile to enable
	* \param coords Normalized coordinates ([0..1]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \see EnableTiles
	*/
	inline void Tilemap::EnableTile(const Vector2ui& tilePos, const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(tilePos.x < m_mapSize.x&& tilePos.y < m_mapSize.y, "Tile position is out of bounds");
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		std::size_t tileIndex = tilePos.y * m_mapSize.x + tilePos.x;
		Tile& tile = m_tiles[tilePos.y * m_mapSize.x + tilePos.x];

		if (!tile.enabled)
		{
			Layer& layer = m_layers[materialIndex];
			layer.enabledTiles.UnboundedSet(tileIndex);
			layer.enabledTileCount = layer.enabledTiles.Count();
		}
		else if (materialIndex != tile.layerIndex)
		{
			Layer& oldLayer = m_layers[tile.layerIndex];
			Layer& newLayer = m_layers[materialIndex];

			oldLayer.enabledTiles.Reset(tileIndex);
			oldLayer.enabledTileCount = oldLayer.enabledTiles.Count();

			newLayer.enabledTiles.UnboundedSet(tileIndex);
			newLayer.enabledTileCount = newLayer.enabledTiles.Count();
		}

		tile.enabled = true;
		tile.color = color;
		tile.textureCoords = coords;
		tile.layerIndex = materialIndex;

		InvalidateVertices();
	}

	/*!
	* \brief Enable and sets the tile at position tilePos
	*
	* Setup the tile at position tilePos using color, unnormalized coordinates rect and materialIndex
	*
	* \param tilePos Position of the tile to enable
	* \param coords Unnormalized coordinates ([0..size]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \remark The material at [materialIndex] must have a valid diffuse map before using this function,
	*         as the size of the material diffuse map is used to compute normalized texture coordinates before returning.
	*
	* \see EnableTiles
	*/
	inline void Tilemap::EnableTile(const Vector2ui& tilePos, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		Vector2ui textureSize(GetTextureSize(materialIndex));
		float invWidth = 1.f / textureSize.x;
		float invHeight = 1.f / textureSize.y;

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTile(tilePos, unnormalizedCoords, color, materialIndex);
	}

	/*!
	* \brief Enable and sets all the tiles
	*
	* Setup all tiles using color, normalized coordinates coords and materialIndex
	*
	* \param coords Normalized coordinates ([0..1]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \remark The material at [materialIndex] must have a valid diffuse map before using this function,
	*         as the size of the material diffuse map is used to compute normalized texture coordinates before returning.
	*
	* \see EnableTile
	*/
	inline void Tilemap::EnableTiles(const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		for (Layer& layer : m_layers)
			layer.enabledTiles.Reset();

		std::size_t tileIndex = 0;
		for (Tile& tile : m_tiles)
		{
			tile.enabled = true;
			tile.color = color;
			tile.textureCoords = coords;
			tile.layerIndex = materialIndex;

			m_layers[materialIndex].enabledTiles.UnboundedSet(tileIndex++);
		}

		m_layers[materialIndex].enabledTileCount = m_layers[materialIndex].enabledTiles.Count();

		InvalidateVertices();
	}

	/*!
	* \brief Enable and sets all the tiles
	*
	* Setup all tiles using color, unnormalized coordinates coords and materialIndex
	*
	* \param coords Unnormalized coordinates ([0..size]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \remark The material at [materialIndex] must have a valid diffuse map before using this function,
	*         as the size of the material diffuse map is used to compute normalized texture coordinates before returning.
	*
	* \see EnableTile
	*/
	inline void Tilemap::EnableTiles(const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		Vector2ui textureSize(GetTextureSize(materialIndex));
		float invWidth = 1.f / textureSize.x;
		float invHeight = 1.f / textureSize.y;

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTiles(unnormalizedCoords, color, materialIndex);
	}

	/*!
	* \brief Enable and sets tileCount tiles at positions contained at tilesPos location, enabling rendering at those locations
	*
	* Setup all tiles using color, normalized coordinates coords and materialIndex
	*
	* \param tilesPos Pointer to a valid array of at least tileCount positions
	* \param tileCount Number of tiles to enable
	* \param coords Normalized coordinates ([0..1]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \see EnableTile
	*/
	inline void Tilemap::EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(tilesPos || tileCount == 0, "Invalid tile position array with a non-zero tileCount");
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		for (std::size_t i = 0; i < tileCount; ++i)
		{
			NazaraAssert(tilesPos->x < m_mapSize.x&& tilesPos->y < m_mapSize.y, "Tile position is out of bounds");

			std::size_t tileIndex = tilesPos->y * m_mapSize.x + tilesPos->x;
			Tile& tile = m_tiles[tileIndex];

			if (!tile.enabled)
			{
				Layer& layer = m_layers[materialIndex];
				layer.enabledTiles.UnboundedSet(tileIndex);
				layer.enabledTileCount = layer.enabledTiles.Count();
			}
			else if (materialIndex != tile.layerIndex)
			{
				Layer& oldLayer = m_layers[tile.layerIndex];
				Layer& newLayer = m_layers[materialIndex];

				oldLayer.enabledTiles.Reset(tileIndex);
				oldLayer.enabledTileCount = oldLayer.enabledTiles.Count();

				newLayer.enabledTiles.UnboundedSet(tileIndex);
				newLayer.enabledTileCount = newLayer.enabledTiles.Count();
			}

			tile.enabled = true;
			tile.color = color;
			tile.textureCoords = coords;
			tile.layerIndex = materialIndex;
			tilesPos++;
		}

		for (Layer& layer : m_layers)
			layer.enabledTileCount = layer.enabledTiles.Count();

		if (tileCount > 0)
			InvalidateVertices();
	}

	/*!
	* \brief Enable and sets tileCount tiles at positions contained at tilesPos location, enabling rendering at those locations
	*
	* Setup all tiles using color, unnormalized coordinates coords and materialIndex
	*
	* \param tilesPos Pointer to a valid array of at least tileCount positions
	* \param tileCount Number of tiles to enable
	* \param coords Unnormalized coordinates ([0..size]) used to specify which region of the material textures will be used
	* \param color The multiplicative color applied to the tile
	* \param materialIndex The material which will be used for rendering this tile
	*
	* \remark The material at [materialIndex] must have a valid diffuse map before using this function,
	*         as the size of the material diffuse map is used to compute normalized texture coordinates before returning.
	*
	* \see EnableTile
	*/
	inline void Tilemap::EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssertFmt(materialIndex < m_layers.size(), "material index out of bounds ({0} >= {1})", materialIndex, m_layers.size());

		Vector2ui textureSize(GetTextureSize(materialIndex));
		float invWidth = 1.f / textureSize.x;
		float invHeight = 1.f / textureSize.y;

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTiles(tilesPos, tileCount, unnormalizedCoords, color, materialIndex);
	}

	/*!
	* \brief Gets the tilemap size (i.e. number of tiles in each dimension)
	* \return Number of tiles in each dimension
	*
	* \see GetSize
	* \see GetTileSize
	*/
	inline const Vector2ui& Tilemap::GetMapSize() const
	{
		return m_mapSize;
	}

	/*!
	* \brief Gets the tilemap origin
	* \return Tilemap origin
	*/
	inline const Vector2f& Tilemap::GetOrigin() const
	{
		return m_origin;
	}

	/*!
	* \brief Returns the size of the tilemap in units (which is equivalent to GetMapSize() * GetTileSize())
	* \return Maximum size in units occupied by this tilemap
	*
	* \see GetMapSize
	* \see GetTileSize
	*/
	inline Vector2f Tilemap::GetSize() const
	{
		return Vector2f(m_mapSize) * m_tileSize;
	}

	/*!
	* \brief Returns informations about a particular tile
	*
	* \param tilePos Position of the tile to get (enabled or not)
	*
	* \return Maximum size in units occupied by this tilemap
	*/
	inline const Tilemap::Tile& Tilemap::GetTile(const Vector2ui& tilePos) const
	{
		NazaraAssert(tilePos.x < m_mapSize.x&& tilePos.y < m_mapSize.y, "Tile position is out of bounds");

		return m_tiles[tilePos.y * m_mapSize.x + tilePos.x];
	}

	/*!
	* \brief Gets the tile size (i.e. number of units occupied by a tile in each dimension)
	* \return Tile size in each dimension
	*
	* \see GetMapSize
	* \see GetSize
	*/
	inline const Vector2f& Tilemap::GetTileSize() const
	{
		return m_tileSize;
	}

	/*!
	* \brief Gets the actual state of the isometric mode
	* \return True if the isometric mode is enabled
	*
	* \see EnableIsometricMode
	*/
	inline bool Tilemap::IsIsometricModeEnabled() const
	{
		return m_isometricModeEnabled;
	}

	inline void Tilemap::SetOrigin(const Vector2f& origin)
	{
		m_origin = origin;

		InvalidateVertices();
		UpdateAABB();
	}

	inline void Tilemap::InvalidateVertices()
	{
		m_shouldRebuildVertices = true;
		OnElementInvalidated(this);
	}

	inline void Tilemap::UpdateAABB()
	{
		Vector2f size = GetSize();
		InstancedRenderable::UpdateAABB(Rectf(-m_origin * size, size));
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
