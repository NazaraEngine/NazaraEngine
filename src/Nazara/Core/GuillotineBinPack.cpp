// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Implémentation originale de Jukka Jylänki (Merci de sa contribution au domaine public)
// http://clb.demon.fi/projects/even-more-rectangle-bin-packing
// Je n'ai vraiment fait qu'adapter le code au moteur (Avec quelques améliorations), je n'ai aucun mérite sur le code ci-dessous

#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <algorithm>
#include <limits>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::GuillotineBinPack
	* \brief Core class that represents the "Guillotine problem", combination of the "Bin packing problem" and the "cutting stock"
	*/

	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		/*!
		* \brief Gets the score for fitting the area
		* \return Score of the fitting
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		Int32 ScoreBestAreaFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			return freeRectSize.width * freeRectSize.height - width * height;
		}

		/*!
		* \brief Gets the score for fitting the area following long side
		* \return Score of the fitting following long side
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		Int32 ScoreBestLongSideFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			Int32 leftoverHoriz = std::abs(static_cast<Int32>(freeRectSize.width) - width);
			Int32 leftoverVert = std::abs(static_cast<Int32>(freeRectSize.height) - height);
			return std::max(leftoverHoriz, leftoverVert);
		}

		/*!
		* \brief Gets the score for fitting the area following short side
		* \return Score of the fitting following short side
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		int ScoreBestShortSideFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			Int32 leftoverHoriz = std::abs(static_cast<Int32>(freeRectSize.width) - width);
			Int32 leftoverVert = std::abs(static_cast<Int32>(freeRectSize.height) - height);
			return std::min(leftoverHoriz, leftoverVert);
		}

		/*!
		* \brief Gets the worst score for fitting the area
		* \return Worst score of the fitting
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		Int32 ScoreWorstAreaFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			return -ScoreBestAreaFit(width, height, freeRectSize);
		}

		/*!
		* \brief Gets the worst score for fitting the area following long side
		* \return Worst score of the fitting following long side
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		Int32 ScoreWorstLongSideFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			return -ScoreBestLongSideFit(width, height, freeRectSize);
		}

		/*!
		* \brief Gets the worst score for fitting the area following short side
		* \return Worst score of the fitting following short side
		*
		* \param width Width
		* \param height Height
		* \param freeRectSize Free area
		*/
		Int32 ScoreWorstShortSideFit(Int32 width, Int32 height, const Rectui32& freeRectSize)
		{
			return -ScoreBestShortSideFit(width, height, freeRectSize);
		}
	}

	/*!
	* \brief Expands the content
	*
	* \param newSize New area for the expansion
	*
	* \see Expand
	*/
	void GuillotineBinPack::Expand(const Vector2ui32& newSize)
	{
		UInt32 oldWidth = m_width;
		UInt32 oldHeight = m_height;

		m_width = std::max(newSize.x, m_width);
		m_height = std::max(newSize.y, m_height);

		if (m_width > oldWidth)
			m_freeRectangles.push_back(Rectui32(oldWidth, 0, m_width - oldWidth, oldHeight));

		if (m_height > oldHeight)
			m_freeRectangles.push_back(Rectui32(0, oldHeight, m_width, m_height - oldHeight));

		// Merge rectangles if possible
		while (MergeFreeRectangles());
	}

	/*!
	* \brief Inserts rectangles in the area
	* \return true if each rectangle could be inserted
	*
	* \param rects List of rectangles
	* \param flipped List of flipped rectangles
	* \param inserted List of inserted rectangles
	* \param count Count of rectangles
	* \param merge Merge possible
	* \param rectChoice Heuristic to use to free
	* \param splitMethod Heuristic to use to split
	*/
	bool GuillotineBinPack::Insert(Rectui32* rects, bool* flipped, bool* inserted, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
	{
		StackVector<Rectui32*> remainingRects = NazaraStackVector(Rectui32*, count); // Position of the rectangle
		remainingRects.resize(count);
		for (UInt32 i = 0; i < count; ++i)
			remainingRects[i] = &rects[i];

		// Pack rectangles one at a time until we have cleared the rects array of all rectangles.
		while (!remainingRects.empty())
		{
			// Stores the penalty score of the best rectangle placement - bigger=worse, smaller=better.
			bool bestFlipped = false;
			std::size_t bestFreeRect = m_freeRectangles.size();
			std::size_t bestRect = std::numeric_limits<int>::min();
			Int32 bestScore = std::numeric_limits<Int32>::max();

			for (std::size_t i = 0; i < m_freeRectangles.size(); ++i)
			{
				Rectui32& freeRect = m_freeRectangles[i];

				for (std::size_t j = 0; j < remainingRects.size(); ++j)
				{
					Rectui32& rect = *remainingRects[j];

					// If this rectangle is a perfect match, we pick it instantly.
					if (rect.width == freeRect.width && rect.height == freeRect.height)
					{
						bestFreeRect = i;
						bestRect = j;
						bestFlipped = false;
						bestScore = std::numeric_limits<Int32>::min();
						i = m_freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
						break;
					}
					// If flipping this rectangle is a perfect match, pick that then.
					else if (flipped && rect.height == freeRect.width && rect.width == freeRect.height)
					{
						bestFreeRect = i;
						bestRect = j;
						bestFlipped = true;
						bestScore = std::numeric_limits<Int32>::min();
						i = m_freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
						break;
					}
					// Try if we can fit the rectangle upright.
					else if (rect.width <= freeRect.width && rect.height <= freeRect.height)
					{
						Int32 score = ScoreByHeuristic(static_cast<Int32>(rect.width), static_cast<Int32>(rect.height), freeRect, rectChoice);
						if (score < bestScore)
						{
							bestFreeRect = i;
							bestRect = j;
							bestFlipped = false;
							bestScore = score;
						}
					}
					// If not, then perhaps flipping sideways will make it fit?
					else if (flipped && rect.height <= freeRect.width && rect.width <= freeRect.height)
					{
						Int32 score = ScoreByHeuristic(static_cast<Int32>(rect.height), static_cast<Int32>(rect.width), freeRect, rectChoice);
						if (score < bestScore)
						{
							bestFreeRect = i;
							bestRect = j;
							bestFlipped = true;
							bestScore = score;
						}
					}
				}
			}

			// If we didn't manage to find any rectangle to pack, abort.
			if (bestScore == std::numeric_limits<Int32>::max())
			{
				// If we can do it, we mark the rectangle could be inserted
				if (inserted)
				{
					for (Rectui32* rect : remainingRects)
					{
						std::ptrdiff_t position = rect - rects;
						inserted[position] = false;
					}
				}

				return false;
			}

			// Otherwise, we're good to go and do the actual packing.
			std::ptrdiff_t position = remainingRects[bestRect] - rects;
			Rectui32& rect = *remainingRects[bestRect];
			rect.x = m_freeRectangles[bestFreeRect].x;
			rect.y = m_freeRectangles[bestFreeRect].y;

			if (bestFlipped)
				std::swap(rect.width, rect.height);

			if (flipped)
				flipped[position] = bestFlipped;

			if (inserted)
				inserted[position] = true;

			// Remove the free space we lost in the bin.
			SplitFreeRectByHeuristic(m_freeRectangles[bestFreeRect], rect, splitMethod);
			m_freeRectangles.erase(m_freeRectangles.begin() + bestFreeRect);

			// Remove the rectangle we just packed from the input list.
			remainingRects.erase(remainingRects.begin() + bestRect);

			// Perform a Rectangle Merge step if desired.
			if (merge)
				MergeFreeRectangles();

			m_usedArea += rect.width * rect.height;
		}

		return true;
	}

	/*!
	* \brief Merges free rectangles together
	* \return true if there was a merge (and thus if a merge is still possible)
	*/
	bool GuillotineBinPack::MergeFreeRectangles()
	{
		std::size_t oriSize = m_freeRectangles.size();

		// Do a Theta(n^2) loop to see if any pair of free rectangles could me merged into one.
		// Note that we miss any opportunities to merge three rectangles into one. (should call this function again to detect that)
		for (std::size_t i = 0; i < m_freeRectangles.size(); ++i)
		{
			Rectui32& firstRect = m_freeRectangles[i];

			for (std::size_t j = i+1; j < m_freeRectangles.size(); ++j)
			{
				Rectui32& secondRect = m_freeRectangles[j];

				if (firstRect.width == secondRect.width && firstRect.x == secondRect.x)
				{
					if (firstRect.y == secondRect.y + secondRect.height)
					{
						firstRect.y -= secondRect.height;
						firstRect.height += secondRect.height;
						m_freeRectangles.erase(m_freeRectangles.begin() + j);
						--j;
					}
					else if (firstRect.y + firstRect.height == secondRect.y)
					{
						firstRect.height += secondRect.height;
						m_freeRectangles.erase(m_freeRectangles.begin() + j);
						--j;
					}
				}
				else if (firstRect.height == secondRect.height && firstRect.y == secondRect.y)
				{
					if (firstRect.x == secondRect.x + secondRect.width)
					{
						firstRect.x -= secondRect.width;
						firstRect.width += secondRect.width;
						m_freeRectangles.erase(m_freeRectangles.begin() + j);
						--j;
					}
					else if (firstRect.x + firstRect.width == secondRect.x)
					{
						firstRect.width += secondRect.width;
						m_freeRectangles.erase(m_freeRectangles.begin() + j);
						--j;
					}
				}
			}
		}

		return m_freeRectangles.size() < oriSize;
	}

	/*!
	* \brief Splits the free rectangle along axis
	*
	* \param freeRect Free rectangle to split
	* \param placedRect Already placed rectangle
	* \param splitHorizontal Split horizontally (or vertically)
	*/
	void GuillotineBinPack::SplitFreeRectAlongAxis(const Rectui32& freeRect, const Rectui32& placedRect, bool splitHorizontal)
	{
		// Form the two new rectangles.
		Rectui32 bottom;
		bottom.x = freeRect.x;
		bottom.y = freeRect.y + placedRect.height;
		bottom.height = freeRect.height - placedRect.height;

		Rectui32 right;
		right.x = freeRect.x + placedRect.width;
		right.y = freeRect.y;
		right.width = freeRect.width - placedRect.width;

		if (splitHorizontal)
		{
			bottom.width = freeRect.width;
			right.height = placedRect.height;
		}
		else // Split vertically
		{
			bottom.width = placedRect.width;
			right.height = freeRect.height;
		}

		// Add the new rectangles into the free rectangle pool if they weren't degenerate.
		if (bottom.width > 0 && bottom.height > 0)
			m_freeRectangles.push_back(bottom);

		if (right.width > 0 && right.height > 0)
			m_freeRectangles.push_back(right);
	}

	/*!
	* \brief Splits the free rectangle using the heuristic
	*
	* \param freeRect Free rectangle to split
	* \param placedRect Already placed rectangle
	* \param method Method used to split
	*
	* \remark Produces a NazaraError if enumeration GuillotineSplitHeuristic is invalid
	*/
	void GuillotineBinPack::SplitFreeRectByHeuristic(const Rectui32& freeRect, const Rectui32& placedRect, GuillotineSplitHeuristic method)
	{
		// Compute the lengths of the leftover area
		const int w = freeRect.width - placedRect.width;
		const int h = freeRect.height - placedRect.height;

		// Placing placedRect into freeRect results in an L-shaped free area, which must be split into
		// two disjoint rectangles. This can be achieved with by splitting the L-shape using a single line
		// We have two choices: horizontal or vertical

		// Use the given heuristic to decide which choice to make

		bool splitHorizontal;
		switch (method)
		{
			case SplitLongerAxis:
				// Split along the longer total axis
				splitHorizontal = (freeRect.width > freeRect.height);
				break;

			case SplitLongerLeftoverAxis:
				// Split along the longer leftover axis
				splitHorizontal = (w > h);
				break;

			case SplitMaximizeArea:
				// Maximize the smaller area == minimize the larger area
				// Tries to make the rectangles more even-sized
				splitHorizontal = (placedRect.width * h <= w * placedRect.height);
				break;

			case SplitMinimizeArea:
				// Maximize the larger area == minimize the smaller area
				// Tries to make the single bigger rectangle
				splitHorizontal = (placedRect.width * h > w * placedRect.height);
				break;

			case SplitShorterAxis:
				// Split along the shorter total axis
				splitHorizontal = (freeRect.width <= freeRect.height);
				break;

			case SplitShorterLeftoverAxis:
				// Split along the shorter leftover axis
				splitHorizontal = (w <= h);
				break;

			default:
				NazaraError("split heuristic out of enum ({0:#x})", UnderlyingCast(method));
				splitHorizontal = true;
		}

		// Perform the actual split
		SplitFreeRectAlongAxis(freeRect, placedRect, splitHorizontal);
	}

	/*!
	* \brief Gets the score using heuristic
	* \return Score of the heuristic
	*
	* \param width Width
	* \param height Height
	* \param freeRect Free area
	* \param rectChoice Heuristic to get score
	*
	* \remark Produces a NazaraError if enumeration FreeRectChoiceHeuristic is invalid
	*/
	Int32 GuillotineBinPack::ScoreByHeuristic(Int32 width, Int32 height, const Rectui32& freeRect, FreeRectChoiceHeuristic rectChoice)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		switch (rectChoice)
		{
			case RectBestAreaFit:
				return ScoreBestAreaFit(width, height, freeRect);

			case RectBestLongSideFit:
				return ScoreBestLongSideFit(width, height, freeRect);

			case RectBestShortSideFit:
				return ScoreBestShortSideFit(width, height, freeRect);

			case RectWorstAreaFit:
				return ScoreWorstAreaFit(width, height, freeRect);

			case RectWorstLongSideFit:
				return ScoreWorstLongSideFit(width, height, freeRect);

			case RectWorstShortSideFit:
				return ScoreWorstShortSideFit(width, height, freeRect);
		}

		NazaraError("Rect choice heuristic out of enum ({0:#x})", UnderlyingCast(rectChoice));
		return std::numeric_limits<Int32>::max();
	}
}
