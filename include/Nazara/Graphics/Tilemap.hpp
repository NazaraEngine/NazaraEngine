// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TILEMAP_HPP
#define NAZARA_GRAPHICS_TILEMAP_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/VertexStruct.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <memory>

namespace Nz
{
	class MaterialInstance;

	class NAZARA_GRAPHICS_API Tilemap : public InstancedRenderable
	{
		public:
			struct Tile;

			Tilemap(const Vector2ui& mapSize, const Vector2f& tileSize, std::size_t materialCount = 1);
			Tilemap(const Tilemap&) = delete;
			Tilemap(Tilemap&&) noexcept = default;
			~Tilemap() = default;

			void BuildElement(ElementRendererRegistry& registry, const ElementData& elementData, std::size_t passIndex, std::vector<RenderElementOwner>& elements) const override;

			inline void DisableTile(const Vector2ui& tilePos);
			inline void DisableTiles();
			inline void DisableTiles(const Vector2ui* tilesPos, std::size_t tileCount);

			inline void EnableIsometricMode(bool isometric);

			inline void EnableTile(const Vector2ui& tilePos, const Rectf& coords, const Color& color = Color::White(), std::size_t materialIndex = 0U);
			inline void EnableTile(const Vector2ui& tilePos, const Rectui& rect, const Color& color = Color::White(), std::size_t materialIndex = 0U);
			inline void EnableTiles(const Rectf& coords, const Color& color = Color::White(), std::size_t materialIndex = 0U);
			inline void EnableTiles(const Rectui& rect, const Color& color = Color::White(), std::size_t materialIndex = 0U);
			inline void EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectf& coords, const Color& color = Color::White(), std::size_t materialIndex = 0U);
			inline void EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectui& rect, const Color& color = Color::White(), std::size_t materialIndex = 0U);

			inline const Vector2ui& GetMapSize() const;
			const std::shared_ptr<MaterialInstance>& GetMaterial(std::size_t i) const override;
			std::size_t GetMaterialCount() const override;
			inline const Vector2f& GetOrigin() const;
			inline Vector2f GetSize() const;
			inline const Tile& GetTile(const Vector2ui& tilePos) const;
			inline const Vector2f& GetTileSize() const;

			inline bool IsIsometricModeEnabled() const;

			void SetMaterial(std::size_t matIndex, std::shared_ptr<MaterialInstance> material);
			inline void SetOrigin(const Vector2f& origin);

			struct Tile
			{
				std::size_t layerIndex = 0U;
				Color color = Color::White();
				Rectf textureCoords = Rectf::Zero();
				bool enabled = false;
			};

			Tilemap& operator=(const Tilemap&) = delete;
			Tilemap& operator=(Tilemap&&) noexcept = default;

		private:
			Vector3ui GetTextureSize(std::size_t matIndex) const;
			inline void InvalidateVertices();
			inline void UpdateAABB();
			void UpdateVertices() const;

			struct Layer
			{
				std::shared_ptr<MaterialInstance> material;
				Bitset<UInt64> enabledTiles;
				std::size_t enabledTileCount = 0; //< cached bitset popcount
			};

			mutable std::vector<VertexStruct_XYZ_Color_UV> m_vertices;
			std::vector<Layer> m_layers;
			std::vector<Tile> m_tiles;
			Vector2f m_origin;
			Vector2f m_tileSize;
			Vector2ui m_mapSize;
			bool m_isometricModeEnabled;
			mutable bool m_shouldRebuildVertices;
	};
}

#include <Nazara/Graphics/Tilemap.inl>

#endif // NAZARA_GRAPHICS_TILEMAP_HPP
