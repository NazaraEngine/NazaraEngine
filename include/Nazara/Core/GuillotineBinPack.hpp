// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Implémentation originale de Jukka Jylänki (Merci de sa contribution au domaine public)
// http://clb.demon.fi/projects/even-more-rectangle-bin-packing

#pragma once

#ifndef NAZARA_CORE_GUILLOTINEBINPACK_HPP
#define NAZARA_CORE_GUILLOTINEBINPACK_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Rect.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API GuillotineBinPack
	{
		public:
			enum FreeRectChoiceHeuristic : int;
			enum GuillotineSplitHeuristic : int;

			inline GuillotineBinPack();
			inline GuillotineBinPack(const Vector2ui32& size);
			GuillotineBinPack(const GuillotineBinPack&) = default;
			GuillotineBinPack(GuillotineBinPack&&) noexcept = default;
			~GuillotineBinPack() = default;

			inline void Clear();

			void Expand(const Vector2ui32& newSize);

			inline void FreeRectangle(const Rectui32& rect);

			inline UInt32 GetHeight() const;
			inline float GetOccupancy() const;
			inline Vector2ui32 GetSize() const;
			inline UInt32 GetUsedArea() const;
			inline UInt32 GetWidth() const;

			inline bool Insert(Rectui32* rects, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
			inline bool Insert(Rectui32* rects, bool* flipped, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);
			bool Insert(Rectui32* rects, bool* flipped, bool* inserted, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod);

			bool MergeFreeRectangles();

			inline void Reset();
			inline void Reset(const Vector2ui32& size);

			GuillotineBinPack& operator=(const GuillotineBinPack&) = default;
			GuillotineBinPack& operator=(GuillotineBinPack&&) noexcept = default;

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
			void SplitFreeRectAlongAxis(const Rectui32& freeRect, const Rectui32& placedRect, bool splitHorizontal);
			void SplitFreeRectByHeuristic(const Rectui32& freeRect, const Rectui32& placedRect, GuillotineSplitHeuristic method);

			static Int32 ScoreByHeuristic(Int32 width, Int32 height, const Rectui32& freeRect, FreeRectChoiceHeuristic rectChoice);

			std::vector<Rectui32> m_freeRectangles;
			UInt32 m_height;
			UInt32 m_usedArea;
			UInt32 m_width;
	};
}

#include <Nazara/Core/GuillotineBinPack.inl>

#endif // NAZARA_CORE_GUILLOTINEBINPACK_HPP
