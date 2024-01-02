// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ABSTRACTATLAS_HPP
#define NAZARA_UTILITY_ABSTRACTATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/SparsePtr.hpp>

namespace Nz
{
	class AbstractImage;
	class Image;

	class NAZARA_UTILITY_API AbstractAtlas
	{
		public:
			AbstractAtlas() = default;
			AbstractAtlas(const AbstractAtlas&) = delete;
			AbstractAtlas(AbstractAtlas&&) noexcept = default;
			virtual ~AbstractAtlas();

			virtual void Clear() = 0;
			virtual void Free(SparsePtr<const Rectui> rects, SparsePtr<std::size_t> layers, std::size_t count) = 0;
			virtual AbstractImage* GetLayer(std::size_t layerIndex) const = 0;
			virtual std::size_t GetLayerCount() const = 0;
			virtual DataStoreFlags GetStorage() const = 0;
			virtual bool Insert(const Image& image, Rectui* rect, bool* flipped, std::size_t* layerIndex) = 0;

			AbstractAtlas& operator=(const AbstractAtlas&) = delete;
			AbstractAtlas& operator=(AbstractAtlas&&) noexcept = default;

			// Signals:
			NazaraSignal(OnAtlasCleared, const AbstractAtlas* /*atlas*/);
			NazaraSignal(OnAtlasLayerChange, const AbstractAtlas* /*atlas*/, AbstractImage* /*oldLayer*/, AbstractImage* /*newLayer*/);
			NazaraSignal(OnAtlasRelease, const AbstractAtlas* /*atlas*/);
	};
}

#endif // NAZARA_UTILITY_ABSTRACTATLAS_HPP
