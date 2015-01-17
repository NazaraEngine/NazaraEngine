// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GUILLOTINEIMAGEATLAS_HPP
#define NAZARA_GUILLOTINEIMAGEATLAS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/AbstractImage.hpp>
#include <Nazara/Utility/Image.hpp>
#include <memory>
#include <vector>

class NAZARA_API NzGuillotineImageAtlas : public NzAbstractAtlas
{
	public:
		NzGuillotineImageAtlas();
		virtual ~NzGuillotineImageAtlas();

		void Clear();
		void Free(NzSparsePtr<const NzRectui> rects, NzSparsePtr<unsigned int> layers, unsigned int count);

		NzGuillotineBinPack::FreeRectChoiceHeuristic GetRectChoiceHeuristic() const;
		NzGuillotineBinPack::GuillotineSplitHeuristic GetRectSplitHeuristic() const;
		NzAbstractImage* GetLayer(unsigned int layerIndex) const;
		unsigned int GetLayerCount() const;
		nzUInt32 GetStorage() const;

		bool Insert(const NzImage& image, NzRectui* rect, bool* flipped, unsigned int* layerIndex);

		void SetRectChoiceHeuristic(NzGuillotineBinPack::FreeRectChoiceHeuristic heuristic);
		void SetRectSplitHeuristic(NzGuillotineBinPack::GuillotineSplitHeuristic heuristic);

	protected:
		struct Layer;

		virtual NzAbstractImage* ResizeImage(NzAbstractImage* oldImage, const NzVector2ui& size) const;
		bool ResizeLayer(Layer& layer, const NzVector2ui& size);

		struct QueuedGlyph
		{
            NzImage image;
			NzRectui rect;
			bool flipped;
		};

		struct Layer
		{
			std::vector<QueuedGlyph> queuedGlyphs;
			std::unique_ptr<NzAbstractImage> image;
			NzGuillotineBinPack binPack;
			unsigned int freedRectangles = 0;
		};

	private:
		void ProcessGlyphQueue(Layer& layer) const;

		mutable std::vector<Layer> m_layers;
		NzGuillotineBinPack::FreeRectChoiceHeuristic m_rectChoiceHeuristic;
		NzGuillotineBinPack::GuillotineSplitHeuristic m_rectSplitHeuristic;
};

#endif // NAZARA_GUILLOTINEIMAGEATLAS_HPP
