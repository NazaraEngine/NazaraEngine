// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TileMap.hpp>
#include <Nazara/Core/Error.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a TileMap object, containing mapSize tileSize-sized tiles
	*
	* \param mapSize Number of tiles in each dimension, must be
	* \param tileSize Size of each tile of the TileMap
	* \param materialCount The maximum number of differents Material this TileMap will use
	*
	* \remark When constructed, a TileMap has no tile active and will not be rendered
	* To use it, you have to enable some tiles.
	*
	* \remark The default material is used for every material requested
	*/
	inline TileMap::TileMap(const Nz::Vector2ui& mapSize, const Nz::Vector2f& tileSize, std::size_t materialCount) :
	m_tiles(mapSize.x * mapSize.y),
	m_layers(materialCount),
	m_mapSize(mapSize),
	m_tileSize(tileSize)
	{
		NazaraAssert(m_tiles.size() != 0U, "Invalid map size");
		NazaraAssert(m_tileSize.x != 0U && m_tileSize.y != 0U, "Invalid tile size");
		NazaraAssert(m_layers.size() != 0U, "Invalid material count");

		for (Layer& layer : m_layers)
			layer.material = Material::GetDefault();

		InvalidateBoundingVolume();
	}

	/*!
	* \brief Disable the tile at position tilePos, disabling rendering at this location
	*
	* \param tilePos Position of the tile to disable
	*
	* \see DisableTiles
	*/
	inline void TileMap::DisableTile(const Vector2ui& tilePos)
	{
		NazaraAssert(tilePos.x < m_mapSize.x && tilePos.y < m_mapSize.y, "Tile position is out of bounds");

		std::size_t tileIndex = tilePos.y * m_mapSize.x + tilePos.x;
		Tile& tile = m_tiles[tileIndex];
		tile.enabled = false;

		m_layers[tile.layerIndex].tiles.erase(tileIndex);

		InvalidateInstanceData(1U << tile.layerIndex);
	}

	/*!
	* \brief Disable all tiles
	*/
	inline void TileMap::DisableTiles()
	{
		for (Tile& tile : m_tiles)
			tile.enabled = false;

		for (Layer& layer : m_layers)
			layer.tiles.clear();

		InvalidateInstanceData(0xFFFFFFFF);
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
	inline void TileMap::DisableTiles(const Vector2ui* tilesPos, std::size_t tileCount)
	{
		NazaraAssert(tilesPos || tileCount == 0, "Invalid tile position array with a non-zero tileCount");

		UInt32 invalidatedLayers = 0;

		for (std::size_t i = 0; i < tileCount; ++i)
		{
			NazaraAssert(tilesPos->x < m_mapSize.x && tilesPos->y < m_mapSize.y, "Tile position is out of bounds");

			std::size_t tileIndex = tilesPos->y * m_mapSize.x + tilesPos->x;
			Tile& tile = m_tiles[tileIndex];
			tile.enabled = false;

			m_layers[tile.layerIndex].tiles.erase(tileIndex);

			invalidatedLayers |= 1U << tile.layerIndex;

			tilesPos++;
		}

		if (tileCount > 0)
			InvalidateInstanceData(invalidatedLayers);
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
	inline void TileMap::EnableTile(const Vector2ui& tilePos, const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(tilePos.x < m_mapSize.x && tilePos.y < m_mapSize.y, "Tile position is out of bounds");
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		UInt32 invalidatedLayers = 1U << materialIndex;

		std::size_t tileIndex = tilePos.y * m_mapSize.x + tilePos.x;
		Tile& tile = m_tiles[tilePos.y * m_mapSize.x + tilePos.x];

		if (!tile.enabled)
			m_layers[materialIndex].tiles.insert(tileIndex);
		else if (materialIndex != tile.layerIndex)
		{
			m_layers[materialIndex].tiles.erase(tileIndex);
			m_layers[tile.layerIndex].tiles.insert(tileIndex);

			invalidatedLayers |= 1U << tile.layerIndex;
		}

		tile.enabled = true;
		tile.color = color;
		tile.textureCoords = coords;
		tile.layerIndex = materialIndex;

		InvalidateInstanceData(invalidatedLayers);
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
	inline void TileMap::EnableTile(const Vector2ui& tilePos, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");
		NazaraAssert(m_layers[materialIndex].material->HasDiffuseMap(), "Material has no diffuse map");

		Texture* diffuseMap = m_layers[materialIndex].material->GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

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
	inline void TileMap::EnableTiles(const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		for (Layer& layer : m_layers)
			layer.tiles.clear();

		std::size_t tileIndex = 0;
		for (Tile& tile : m_tiles)
		{
			tile.enabled = true;
			tile.color = color;
			tile.textureCoords = coords;
			tile.layerIndex = materialIndex;

			m_layers[materialIndex].tiles.insert(tileIndex++);
		}

		InvalidateInstanceData(0xFFFFFFFF);
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
	inline void TileMap::EnableTiles(const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		Texture* diffuseMap = m_layers[materialIndex].material->GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

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
	inline void TileMap::EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectf& coords, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(tilesPos || tileCount == 0, "Invalid tile position array with a non-zero tileCount");
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		UInt32 invalidatedLayers = 1U << materialIndex;

		for (std::size_t i = 0; i < tileCount; ++i)
		{
			NazaraAssert(tilesPos->x < m_mapSize.x && tilesPos->y < m_mapSize.y, "Tile position is out of bounds");

			std::size_t tileIndex = tilesPos->y * m_mapSize.x + tilesPos->x;
			Tile& tile = m_tiles[tileIndex];

			if (!tile.enabled)
				m_layers[materialIndex].tiles.insert(tileIndex);
			else if (materialIndex != tile.layerIndex)
			{
				m_layers[materialIndex].tiles.erase(tileIndex);
				m_layers[tile.layerIndex].tiles.insert(tileIndex);

				invalidatedLayers |= 1U << tile.layerIndex;
			}

			tile.enabled = true;
			tile.color = color;
			tile.textureCoords = coords;
			tile.layerIndex = materialIndex;
			tilesPos++;
		}

		if (tileCount > 0)
			InvalidateInstanceData(invalidatedLayers);
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
	inline void TileMap::EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");
		NazaraAssert(m_layers[materialIndex].material->HasDiffuseMap(), "Material has no diffuse map");

		Texture* diffuseMap = m_layers[materialIndex].material->GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTiles(tilesPos, tileCount, unnormalizedCoords, color, materialIndex);
	}

	/*!
	* \brief Gets the material at position index used by the TileMap
	*
	* \param index Index of the material to query
	*
	* \return Material at index
	*/
	inline const MaterialRef& TileMap::GetMaterial(std::size_t index) const
	{
		NazaraAssert(index < m_layers.size(), "Material out of bounds");

		return m_layers[index].material;
	}

	/*!
	* \brief Gets the maximum material count this TileMap can use
	* \return Material count
	*/
	inline std::size_t TileMap::GetMaterialCount() const
	{
		return m_layers.size();
	}

	/*!
	* \brief Gets the tilemap size (i.e. number of tiles in each dimension)
	* \return Number of tiles in each dimension
	*
	* \see GetSize
	* \see GetTileSize
	*/
	inline const Vector2ui& TileMap::GetMapSize() const
	{
		return m_mapSize;
	}

	/*!
	* \brief Returns the size of the tilemap in units (which is equivalent to GetMapSize() * GetTileSize())
	* \return Maximum size in units occupied by this tilemap
	*
	* \see GetMapSize
	* \see GetTileSize
	*/
	inline Vector2f TileMap::GetSize() const
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
	inline const TileMap::Tile& TileMap::GetTile(const Vector2ui& tilePos) const
	{
		NazaraAssert(tilePos.x < m_mapSize.x && tilePos.y < m_mapSize.y, "Tile position is out of bounds");

		return m_tiles[tilePos.y * m_mapSize.x + tilePos.x];
	}

	/*!
	* \brief Gets the tile size (i.e. number of units occupied by a tile in each dimension)
	* \return Tile size in each dimension
	*
	* \see GetMapSize
	* \see GetSize
	*/
	inline const Vector2f& TileMap::GetTileSize() const
	{
		return m_tileSize;
	}

	/*!
	* \brief Sets a material of the TileMap
	*
	* \param index Index of the material to change
	* \param material Material for the TileMap
	*/
	inline void TileMap::SetMaterial(std::size_t index, MaterialRef material)
	{
		NazaraAssert(index < m_layers.size(), "Material out of bounds");

		m_layers[index].material = std::move(material);
	}

	/*!
	* \brief Sets the current TileMap with the content of the other one
	* \return A reference to this
	*
	* \param TileMap The other TileMap
	*/
	inline TileMap& TileMap::operator=(const TileMap& TileMap)
	{
		InstancedRenderable::operator=(TileMap);

		m_layers = TileMap.m_layers;
		m_mapSize = TileMap.m_mapSize;
		m_tiles = TileMap.m_tiles;
		m_tileSize = TileMap.m_tileSize;

		// We do not copy final vertices because it's highly probable that our parameters are modified and they must be regenerated
		InvalidateBoundingVolume();
		InvalidateInstanceData(0xFFFFFFFF);

		return *this;
	}

	/*!
	* \brief Creates a new TileMap from the arguments
	* \return A reference to the newly created TileMap
	*
	* \param args Arguments for the TileMap
	*/
	template<typename... Args>
	TileMapRef TileMap::New(Args&&... args)
	{
		std::unique_ptr<TileMap> object(new TileMap(std::forward<Args>(args)...));
		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
