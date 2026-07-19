// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADOWATLAS_HPP
#define NAZARA_GRAPHICS_SHADOWATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Rect.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class GpuDevice;
	class Texture;

	class NAZARA_GRAPHICS_API ShadowAtlas
	{
		public:
			ShadowAtlas(GpuDevice& renderDevice, UInt32 atlasSize);
			ShadowAtlas(const ShadowAtlas&) = delete;
			ShadowAtlas(ShadowAtlas&&) = delete;
			~ShadowAtlas() = default;

			void Clear();

			inline std::optional<Rectf> GetNormalizedRect(std::size_t shadowIndex) const;
			inline std::optional<Rectui32> GetRect(std::size_t shadowIndex) const;
			inline const std::shared_ptr<Texture>& GetTexture() const;

			inline bool IsEmpty() const;

			void Pack();

			inline std::size_t Register(UInt32 maxSize, std::size_t count = 1);
			inline std::size_t Register(std::initializer_list<UInt32> maxSizes);

			ShadowAtlas& operator=(const ShadowAtlas&) = delete;
			ShadowAtlas& operator=(ShadowAtlas&&) = delete;

		private:
			static constexpr UInt32 InvalidPosition = MaxValue();

			struct Entry
			{
				Rectui32 rect;
			};

			std::shared_ptr<Texture> m_atlasTexture;
			std::vector<Entry> m_entries;
			GuillotineBinPack m_binPacker;
			UInt32 m_atlasSize;
			GpuDevice& m_renderDevice;
	};
}

#include <Nazara/Graphics/ShadowAtlas.inl>

#endif // NAZARA_GRAPHICS_SHADOWATLAS_HPP
