// Copyright (C) 2014 Jérôme Leclercq
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
		enum FreeRectChoiceHeuristic
		{
			RectBestAreaFit,
			RectBestLongSideFit,
			RectBestShortSideFit,
			RectWorstAreaFit,
			RectWorstLongSideFit,
			RectWorstShortSideFit
		};

		enum GuillotineSplitHeuristic
		{
			SplitLongerAxis,
			SplitLongerLeftoverAxis,
			SplitMaximizeArea,
			SplitMinimizeArea,
			SplitShorterAxis,
			SplitShorterLeftoverAxis
		};

		NzGuillotineBinPack();
		NzGuillotineBinPack(unsigned int width, unsigned int height);
		~NzGuillotineBinPack() = default;

		void Clear();

		void FreeRectangle(const NzRectui& rect);

		unsigned int GetHeight() const;
		float GetOccupancy() const;
		NzVector2ui GetSize() const;
		unsigned int GetWidth() const;

		bool Insert(NzRectui* rects, bool* flipped, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);

		bool MergeFreeRectangles();

		void Reset();
		void Reset(unsigned int width, unsigned int height);

	private:
		void SplitFreeRectAlongAxis(const NzRectui& freeRect, const NzRectui& placedRect, bool splitHorizontal);
		void SplitFreeRectByHeuristic(const NzRectui& freeRect, const NzRectui& placedRect, GuillotineSplitHeuristic method);

		static int ScoreByHeuristic(int width, int height, const NzRectui& freeRect, FreeRectChoiceHeuristic rectChoice);

		std::vector<NzRectui> m_freeRectangles;
		float m_occupancy;
		unsigned int m_height;
		unsigned int m_width;
};

#endif // NAZARA_GUILLOTINEBINPACK_HPP
