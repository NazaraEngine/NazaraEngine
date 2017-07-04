// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Implémentation originale de Jukka Jylänki (Merci de sa contribution au domaine public)
// http://clb.demon.fi/projects/even-more-rectangle-bin-packing

#pragma once

#ifndef NAZARA_GUILLOTINEBINPACK_HPP
#define NAZARA_GUILLOTINEBINPACK_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Rect.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API GuillotineBinPack
	{
		public:
			enum FreeRectChoiceHeuristic : int;
			enum GuillotineSplitHeuristic : int;

			GuillotineBinPack();
			GuillotineBinPack(unsigned int width, unsigned int height);
			GuillotineBinPack(const Vector2ui& size);
			GuillotineBinPack(const GuillotineBinPack&) = default;
			GuillotineBinPack(GuillotineBinPack&&) = default;
			~GuillotineBinPack() = default;

			void Clear();

			void Expand(unsigned int newWidth, unsigned newHeight);
			void Expand(const Vector2ui& newSize);

			void FreeRectangle(const Rectui& rect);

			unsigned int GetHeight() const;
			float GetOccupancy() const;
			Vector2ui GetSize() const;
			unsigned int GetWidth() const;

			bool Insert(Rectui* rects, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
			bool Insert(Rectui* rects, bool* flipped, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
			bool Insert(Rectui* rects, bool* flipped, bool* inserted, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);

			bool MergeFreeRectangles();

			void Reset();
			void Reset(unsigned int width, unsigned int height);
			void Reset(const Vector2ui& size);

			GuillotineBinPack& operator=(const GuillotineBinPack&) = default;
			GuillotineBinPack& operator=(GuillotineBinPack&&) = default;

			enum FreeRectChoiceHeuristic : int
			{
				RectBestAreaFit,
				RectBestLongSideFit,
				RectBestShortSideFit,
				RectWorstAreaFit,
				RectWorstLongSideFit,
				RectWorstShortSideFit
			};

			enum GuillotineSplitHeuristic : int
			{
				SplitLongerAxis,
				SplitLongerLeftoverAxis,
				SplitMaximizeArea,
				SplitMinimizeArea,
				SplitShorterAxis,
				SplitShorterLeftoverAxis
			};

		private:
			void SplitFreeRectAlongAxis(const Rectui& freeRect, const Rectui& placedRect, bool splitHorizontal);
			void SplitFreeRectByHeuristic(const Rectui& freeRect, const Rectui& placedRect, GuillotineSplitHeuristic method);

			static int ScoreByHeuristic(int width, int height, const Rectui& freeRect, FreeRectChoiceHeuristic rectChoice);

			std::vector<Rectui> m_freeRectangles;
			unsigned int m_height;
			unsigned int m_usedArea;
			unsigned int m_width;
	};
}

#endif // NAZARA_GUILLOTINEBINPACK_HPP
