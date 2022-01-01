// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ABSTRACTATLAS_HPP
#define NAZARA_UTILITY_ABSTRACTATLAS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

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
			virtual void Free(SparsePtr<const Rectui> rects, SparsePtr<unsigned int> layers, unsigned int count) = 0;
			virtual AbstractImage* GetLayer(unsigned int layerIndex) const = 0;
			virtual std::size_t GetLayerCount() const = 0;
			virtual DataStoreFlags GetStorage() const = 0;
			virtual bool Insert(const Image& image, Rectui* rect, bool* flipped, unsigned int* layerIndex) = 0;

			AbstractAtlas& operator=(const AbstractAtlas&) = delete;
			AbstractAtlas& operator=(AbstractAtlas&&) noexcept = default;

			// Signals:
			NazaraSignal(OnAtlasCleared, const AbstractAtlas* /*atlas*/);
			NazaraSignal(OnAtlasLayerChange, const AbstractAtlas* /*atlas*/, AbstractImage* /*oldLayer*/, AbstractImage* /*newLayer*/);
			NazaraSignal(OnAtlasRelease, const AbstractAtlas* /*atlas*/);
	};
}

#endif // NAZARA_UTILITY_ABSTRACTATLAS_HPP
