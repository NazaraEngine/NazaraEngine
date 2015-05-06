// Copyright (C) 2015 Jérôme Leclercq
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

class NAZARA_API NzGuillotineBinPack
{
	public:
		enum FreeRectChoiceHeuristic : int;
		enum GuillotineSplitHeuristic : int;

		NzGuillotineBinPack();
		NzGuillotineBinPack(unsigned int width, unsigned int height);
		NzGuillotineBinPack(const NzVector2ui& size);
		NzGuillotineBinPack(const NzGuillotineBinPack&) = default;
		NzGuillotineBinPack(NzGuillotineBinPack&&) = default;
		~NzGuillotineBinPack() = default;

		void Clear();

		void Expand(unsigned int newWidth, unsigned newHeight);
		void Expand(const NzVector2ui& newSize);

		void FreeRectangle(const NzRectui& rect);

		unsigned int GetHeight() const;
		float GetOccupancy() const;
		NzVector2ui GetSize() const;
		unsigned int GetWidth() const;

		bool Insert(NzRectui* rects, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
		bool Insert(NzRectui* rects, bool* flipped, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
		bool Insert(NzRectui* rects, bool* flipped, bool* inserted, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);

		bool MergeFreeRectangles();

		void Reset();
		void Reset(unsigned int width, unsigned int height);
		void Reset(const NzVector2ui& size);

		NzGuillotineBinPack& operator=(const NzGuillotineBinPack&) = default;
		NzGuillotineBinPack& operator=(NzGuillotineBinPack&&) = default;

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
		void SplitFreeRectAlongAxis(const NzRectui& freeRect, const NzRectui& placedRect, bool splitHorizontal);
		void SplitFreeRectByHeuristic(const NzRectui& freeRect, const NzRectui& placedRect, GuillotineSplitHeuristic method);

		static int ScoreByHeuristic(int width, int height, const NzRectui& freeRect, FreeRectChoiceHeuristic rectChoice);

		std::vector<NzRectui> m_freeRectangles;
		unsigned int m_height;
		unsigned int m_usedArea;
		unsigned int m_width;
};

#endif // NAZARA_GUILLOTINEBINPACK_HPP
