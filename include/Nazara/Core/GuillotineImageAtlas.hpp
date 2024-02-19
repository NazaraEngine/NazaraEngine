// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
#define NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractAtlas.hpp>
#include <Nazara/Core/AbstractImage.hpp>
#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Core/Image.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API GuillotineImageAtlas : public AbstractAtlas
	{
		public:
			GuillotineImageAtlas();
			GuillotineImageAtlas(const GuillotineImageAtlas&) = delete;
			GuillotineImageAtlas(GuillotineImageAtlas&&) noexcept = default;
			~GuillotineImageAtlas() = default;

			void Clear() override;

			void Free(SparsePtr<const Rectui> rects, SparsePtr<std::size_t> layers, std::size_t count) override;

			unsigned int GetMaxLayerSize() const;
			GuillotineBinPack::FreeRectChoiceHeuristic GetRectChoiceHeuristic() const;
			GuillotineBinPack::GuillotineSplitHeuristic GetRectSplitHeuristic() const;
			AbstractImage* GetLayer(std::size_t layerIndex) const override;
			std::size_t GetLayerCount() const override;
			DataStoreFlags GetStorage() const override;

			bool Insert(const Image& image, Rectui* rect, bool* flipped, std::size_t* layerIndex) override;

			void SetMaxLayerSize(unsigned int maxLayerSize);
			void SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic);
			void SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic);

			GuillotineImageAtlas& operator=(const GuillotineImageAtlas&) = delete;
			GuillotineImageAtlas& operator=(GuillotineImageAtlas&&) noexcept = default;

		protected:
			struct Layer;

			virtual std::shared_ptr<AbstractImage> ResizeImage(const std::shared_ptr<AbstractImage>& oldImage, const Vector2ui& size) const;
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
				std::shared_ptr<AbstractImage> image;
				GuillotineBinPack binPack;
				unsigned int freedRectangles = 0;
			};

		private:
			void ProcessGlyphQueue(Layer& layer) const;

			mutable std::vector<Layer> m_layers;
			GuillotineBinPack::FreeRectChoiceHeuristic m_rectChoiceHeuristic;
			GuillotineBinPack::GuillotineSplitHeuristic m_rectSplitHeuristic;
			unsigned int m_maxLayerSize;
	};
}

#endif // NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
