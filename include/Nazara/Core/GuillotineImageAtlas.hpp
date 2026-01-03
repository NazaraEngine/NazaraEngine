// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
#define NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractAtlas.hpp>
#include <Nazara/Core/AbstractImage.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Core/Image.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API GuillotineImageAtlas : public AbstractAtlas
	{
		public:
			inline GuillotineImageAtlas(PixelFormat pixelFormat, UInt32 initialLayerSize = 512);
			GuillotineImageAtlas(const GuillotineImageAtlas&) = delete;
			GuillotineImageAtlas(GuillotineImageAtlas&&) noexcept = default;
			~GuillotineImageAtlas() = default;

			void Clear() override;

			void Free(SparsePtr<const Rectui> rects, SparsePtr<std::size_t> layers, std::size_t count) override;

			inline UInt32 GetMaxLayerSize() const;
			inline GuillotineBinPack::FreeRectChoiceHeuristic GetRectChoiceHeuristic() const;
			inline GuillotineBinPack::GuillotineSplitHeuristic GetRectSplitHeuristic() const;
			AbstractImage* GetLayer(std::size_t layerIndex) const override;
			std::size_t GetLayerCount() const override;
			inline PixelFormat GetPixelFormat() const;
			DataStoreFlags GetStorage() const override;

			bool Insert(const Image& image, Rectui* rect, bool* flipped, std::size_t* layerIndex) override;

			inline void SetMaxLayerSize(UInt32 maxLayerSize);
			inline void SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic);
			inline void SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic);

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
			PixelFormat m_pixelFormat;
			UInt32 m_initialLayerSize;
			UInt32 m_maxLayerSize;
	};
}

#include <Nazara/Core/GuillotineImageAtlas.inl>

#endif // NAZARA_CORE_GUILLOTINEIMAGEATLAS_HPP
