// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTFONTATLAS_HPP
#define NAZARA_ABSTRACTFONTATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <unordered_map>

class NzAbstractImage;
class NzImage;

class NAZARA_API NzAbstractFontAtlas
{
	public:
		class Listener;

		NzAbstractFontAtlas();
		virtual ~NzAbstractFontAtlas();

		void AddListener(Listener* font, void* userdata = nullptr) const;

		virtual void Clear() = 0;
		virtual void Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count) = 0;
		virtual NzAbstractImage* GetLayer(unsigned int layerIndex) const = 0;
		virtual unsigned int GetLayerCount() const = 0;
		virtual bool Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex) = 0;

		void RemoveListener(Listener* font) const;

		class Listener
		{
			public:
				Listener() = default;
				virtual ~Listener();

				virtual bool OnAtlasCleared(const NzAbstractFontAtlas* atlas, void* userdata);
				virtual void OnAtlasReleased(const NzAbstractFontAtlas* atlas, void* userdata);
		};

	protected:
		void NotifyCleared();

	private:
		mutable std::unordered_map<Listener*, void*> m_listeners;
		bool m_listenersLocked;
};

#endif // NAZARA_ABSTRACTFONTATLAS_HPP
