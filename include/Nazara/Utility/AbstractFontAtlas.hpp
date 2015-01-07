// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTFONTATLAS_HPP
#define NAZARA_ABSTRACTFONTATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <set>

class NzAbstractImage;
class NzFont;
class NzImage;

class NAZARA_API NzAbstractFontAtlas
{
	public:
		NzAbstractFontAtlas() = default;
		virtual ~NzAbstractFontAtlas();

		virtual void Clear() = 0;
		virtual void Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count) = 0;
		virtual NzAbstractImage* GetLayer(unsigned int layerIndex) const = 0;
		virtual unsigned int GetLayerCount() const = 0;
		virtual bool Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex) = 0;
		void RegisterFont(NzFont* font);
		void UnregisterFont(NzFont* font);

	protected:
		void NotifyCleared();

	private:
		std::set<NzFont*> m_registredFonts;
};

#endif // NAZARA_ABSTRACTFONTATLAS_HPP
