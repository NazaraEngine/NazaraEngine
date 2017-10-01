// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TILEMAP_HPP
#define NAZARA_TILEMAP_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <set>

namespace Nz
{
	class TileMap;

	using TileMapConstRef = ObjectRef<const TileMap>;
	using TileMapLibrary = ObjectLibrary<TileMap>;
	using TileMapRef = ObjectRef<TileMap>;

	class NAZARA_GRAPHICS_API TileMap : public InstancedRenderable
	{
		friend TileMapLibrary;
		friend class Graphics;

		public:
			struct Tile;

			inline TileMap(const Nz::Vector2ui& mapSize, const Nz::Vector2f& tileSize, std::size_t materialCount = 1);
			TileMap(const TileMap& TileMap) = default;
			TileMap(TileMap&&) = delete;
			~TileMap() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

			inline void DisableTile(const Vector2ui& tilePos);
			inline void DisableTiles();
			inline void DisableTiles(const Vector2ui* tilesPos, std::size_t tileCount);

			inline void EnableIsometricMode(bool isometric);

			inline void EnableTile(const Vector2ui& tilePos, const Rectf& coords, const Color& color = Color::White, std::size_t materialIndex = 0U);
			inline void EnableTile(const Vector2ui& tilePos, const Rectui& rect, const Color& color = Color::White, std::size_t materialIndex = 0U);
			inline void EnableTiles(const Rectf& coords, const Color& color = Color::White, std::size_t materialIndex = 0U);
			inline void EnableTiles(const Rectui& rect, const Color& color = Color::White, std::size_t materialIndex = 0U);
			inline void EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectf& coords, const Color& color = Color::White, std::size_t materialIndex = 0U);
			inline void EnableTiles(const Vector2ui* tilesPos, std::size_t tileCount, const Rectui& rect, const Color& color = Color::White, std::size_t materialIndex = 0U);

			inline const MaterialRef& GetMaterial(std::size_t index) const;
			inline std::size_t GetMaterialCount() const;
			inline const Vector2ui& GetMapSize() const;
			inline Vector2f GetSize() const;
			inline const Tile& GetTile(const Vector2ui& tilePos) const;
			inline const Vector2f& GetTileSize() const;

			inline bool IsIsometricModeEnabled() const;

			inline void SetMaterial(std::size_t index, MaterialRef material);

			inline TileMap& operator=(const TileMap& TileMap);
			TileMap& operator=(TileMap&& TileMap) = delete;

			template<typename... Args> static TileMapRef New(Args&&... args);

			struct Tile
			{
				std::size_t layerIndex = 0U;
				Color color = Color::White;
				Rectf textureCoords = Rectf::Zero();
				bool enabled = false;
			};

		private:
			void MakeBoundingVolume() const override;
			void UpdateData(InstanceData* instanceData) const override;

			static bool Initialize();
			static void Uninitialize();

			struct Layer
			{
				MaterialRef material;
				std::set<std::size_t> tiles;
			};

			std::vector<Tile> m_tiles;
			std::vector<Layer> m_layers;
			Vector2ui m_mapSize;
			Vector2f m_tileSize;
			bool m_isometricModeEnabled;

			static TileMapLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/TileMap.inl>

#endif // NAZARA_TILEMAP_HPP
