// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/TileMap.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/PhongLightingMaterial.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::TileMap
	* \brief Graphics class that represent several tiles of the same size assembled into a grid
	*  This class is far more efficient than using a sprite for every tile
	*/

	/*!
	* \brief Adds the TileMap to the rendering queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data for the instance
	*/
	void TileMap::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData, const Recti& scissorRect) const
	{
		const VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<const VertexStruct_XYZ_Color_UV*>(instanceData.data.data());

		std::size_t matCount = 0;
		std::size_t spriteCount = 0;
		for (const Layer& layer : m_layers)
		{
			renderQueue->AddSprites(instanceData.renderOrder, GetMaterial(matCount++), &vertices[4 * spriteCount], layer.tiles.size(), scissorRect);

			spriteCount += layer.tiles.size();
		}
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
	void TileMap::EnableTile(const Vector2ui& tilePos, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		PhongLightingMaterial phongMaterial(GetMaterial(materialIndex));
		NazaraAssert(phongMaterial.HasDiffuseMap(), "Material has no diffuse map");

		const Texture* diffuseMap = phongMaterial.GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTile(tilePos, unnormalizedCoords, color, materialIndex);
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
	void TileMap::EnableTiles(const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		PhongLightingMaterial phongMaterial(GetMaterial(materialIndex));
		NazaraAssert(phongMaterial.HasDiffuseMap(), "Material has no diffuse map");

		const Texture* diffuseMap = phongMaterial.GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTiles(unnormalizedCoords, color, materialIndex);
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
	void TileMap::EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectui& rect, const Color& color, std::size_t materialIndex)
	{
		NazaraAssert(materialIndex < m_layers.size(), "Material out of bounds");

		PhongLightingMaterial phongMaterial(GetMaterial(materialIndex));
		NazaraAssert(phongMaterial.HasDiffuseMap(), "Material has no diffuse map");

		const Texture* diffuseMap = phongMaterial.GetDiffuseMap();
		float invWidth = 1.f / diffuseMap->GetWidth();
		float invHeight = 1.f / diffuseMap->GetHeight();

		Rectf unnormalizedCoords(invWidth * rect.x, invHeight * rect.y, invWidth * rect.width, invHeight * rect.height);
		EnableTiles(tilesPos, tileCount, unnormalizedCoords, color, materialIndex);
	}

	/*!
	* \brief Clones this tilemap
	*/
	std::unique_ptr<InstancedRenderable> TileMap::Clone() const
	{
		return std::make_unique<TileMap>(*this);
	}

	void TileMap::MakeBoundingVolume() const
	{
		Nz::Vector2f size = GetSize();
		m_boundingVolume.Set(Vector3f(0.f), size.x*Vector3f::Right() + size.y*Vector3f::Down());
	}

	void TileMap::UpdateData(InstanceData* instanceData) const
	{
		std::size_t spriteCount = 0;
		for (const Layer& layer : m_layers)
			spriteCount += layer.tiles.size();

		instanceData->data.resize(4 * spriteCount * sizeof(VertexStruct_XYZ_Color_UV));
		VertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<VertexStruct_XYZ_Color_UV*>(instanceData->data.data());

		spriteCount = 0;
		for (const Layer& layer : m_layers)
		{
			SparsePtr<Color> colorPtr(&vertices[4 * spriteCount].color, sizeof(VertexStruct_XYZ_Color_UV));
			SparsePtr<Vector3f> posPtr(&vertices[4 * spriteCount].position, sizeof(VertexStruct_XYZ_Color_UV));
			SparsePtr<Vector2f> texCoordPtr(&vertices[4 * spriteCount].uv, sizeof(VertexStruct_XYZ_Color_UV));

			for (std::size_t tileIndex : layer.tiles)
			{
				const Tile& tile = m_tiles[tileIndex];
				NazaraAssert(tile.enabled, "Tile specified for rendering is not enabled");

				std::size_t x = tileIndex % m_mapSize.x;
				std::size_t y = tileIndex / m_mapSize.x;

				Vector3f tileLeftCorner;
				if (m_isometricModeEnabled)
					tileLeftCorner.Set(x * m_tileSize.x + m_tileSize.x/2.f * (y % 2), y/2.f * -m_tileSize.y, 0.f);
				else
					tileLeftCorner.Set(x * m_tileSize.x, y * -m_tileSize.y, 0.f);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner);
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_LeftTop);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.x * Vector3f::Right());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_RightTop);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.y * Vector3f::Down());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_LeftBottom);

				*colorPtr++ = tile.color;
				*posPtr++ = instanceData->transformMatrix.Transform(tileLeftCorner + m_tileSize.x * Vector3f::Right() + m_tileSize.y * Vector3f::Down());
				*texCoordPtr++ = tile.textureCoords.GetCorner(RectCorner_RightBottom);
			}
			spriteCount += layer.tiles.size();
		}
	}

	/*!
	* \brief Initializes the tilemap library
	* \return true If successful
	*
	* \remark Produces a NazaraError if the tilemap library failed to be initialized
	*/

	bool TileMap::Initialize()
	{
		if (!TileMapLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	/*!
	* \brief Uninitializes the tilemap library
	*/

	void TileMap::Uninitialize()
	{
		TileMapLibrary::Uninitialize();
	}

	TileMapLibrary::LibraryMap TileMap::s_library;
}
