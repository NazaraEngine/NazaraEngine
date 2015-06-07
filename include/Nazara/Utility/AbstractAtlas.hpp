// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTATLAS_HPP
#define NAZARA_ABSTRACTATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <unordered_map>

class NzAbstractImage;
class NzImage;

class NAZARA_API NzAbstractAtlas
{
	public:
		NzAbstractAtlas() = default;
		virtual ~NzAbstractAtlas();

		virtual void Clear() = 0;
		virtual void Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count) = 0;
		virtual NzAbstractImage* GetLayer(unsigned int layerIndex) const = 0;
		virtual unsigned int GetLayerCount() const = 0;
		virtual nzUInt32 GetStorage() const = 0;
		virtual bool Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex) = 0;

		NazaraSignal(OnAtlasCleared, const NzAbstractAtlas*); //< Args: me
		NazaraSignal(OnAtlasLayerChange, const NzAbstractAtlas*, NzAbstractImage*, NzAbstractImage*); //< Args: me, oldLayer, newLayer
		NazaraSignal(OnAtlasRelease, const NzAbstractAtlas*); //< Args: me
};

#endif // NAZARA_ABSTRACTATLAS_HPP
