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

namespace Nz
{
	class NAZARA_UTILITY_API GuillotineImageAtlas : public AbstractAtlas
	{
		public:
			GuillotineImageAtlas();
			virtual ~GuillotineImageAtlas();

			void Clear() override;

			void Free(SparsePtr<const Rectui> rects, SparsePtr<unsigned int> layers, unsigned int count) override;

			GuillotineBinPack::FreeRectChoiceHeuristic GetRectChoiceHeuristic() const;
			GuillotineBinPack::GuillotineSplitHeuristic GetRectSplitHeuristic() const;
			AbstractImage* GetLayer(unsigned int layerIndex) const override;
			std::size_t GetLayerCount() const override;
			UInt32 GetStorage() const override;

			bool Insert(const Image& image, Rectui* rect, bool* flipped, unsigned int* layerIndex) override;

			void SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic);
			void SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic);

		protected:
			struct Layer;

			virtual AbstractImage* ResizeImage(AbstractImage* oldImage, const Vector2ui& size) const;
			bool ResizeLayer(Layer& layer, const Vector2ui& size);

			struct QueuedGlyph
			{
				Image image;
				Rectui rect;
				bool flipped;
			};

			struct Layer
			{
				std::vector<QueuedGlyph> queuedGlyphs;
				std::unique_ptr<AbstractImage> image;
				GuillotineBinPack binPack;
				unsigned int freedRectangles = 0;
			};

		private:
			void ProcessGlyphQueue(Layer& layer) const;

			mutable std::vector<Layer> m_layers;
			GuillotineBinPack::FreeRectChoiceHeuristic m_rectChoiceHeuristic;
			GuillotineBinPack::GuillotineSplitHeuristic m_rectSplitHeuristic;
	};
}

#endif // NAZARA_GUILLOTINEIMAGEATLAS_HPP
