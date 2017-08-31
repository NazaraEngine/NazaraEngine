// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTATLAS_HPP
#define NAZARA_ABSTRACTATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <unordered_map>

namespace Nz
{
	class AbstractImage;
	class Image;

	class NAZARA_UTILITY_API AbstractAtlas
	{
		public:
			AbstractAtlas() = default;
			virtual ~AbstractAtlas();

			virtual void Clear() = 0;
			virtual void Free(SparsePtr<const Rectui> rects, SparsePtr<unsigned int> layers, unsigned int count) = 0;
			virtual AbstractImage* GetLayer(unsigned int layerIndex) const = 0;
			virtual std::size_t GetLayerCount() const = 0;
			virtual UInt32 GetStorage() const = 0;
			virtual bool Insert(const Image& image, Rectui* rect, bool* flipped, unsigned int* layerIndex) = 0;

			// Signals:
			NazaraSignal(OnAtlasCleared, const AbstractAtlas* /*atlas*/);
			NazaraSignal(OnAtlasLayerChange, const AbstractAtlas* /*atlas*/, AbstractImage* /*oldLayer*/, AbstractImage* /*newLayer*/);
			NazaraSignal(OnAtlasRelease, const AbstractAtlas* /*atlas*/);
	};
}

#endif // NAZARA_ABSTRACTATLAS_HPP
