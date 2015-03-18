// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Implémentation originale de Jukka Jylänki (Merci de sa contribution au domaine public)
// http://clb.demon.fi/projects/even-more-rectangle-bin-packing
// Je n'ai vraiment fait qu'adapter le code au moteur (Avec quelques améliorations), je n'ai aucun mérite sur le code ci-dessous

#include <Nazara/Core/GuillotineBinPack.hpp>
#include <Nazara/Core/Config.hpp>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <Nazara/Core/Debug.hpp>

namespace
{
	int ScoreBestAreaFit(int width, int height, const NzRectui& freeRectSize)
	{
		return freeRectSize.width * freeRectSize.height - width * height;
	}

	int ScoreBestLongSideFit(int width, int height, const NzRectui& freeRectSize)
	{
		int leftoverHoriz = std::abs(static_cast<int>(freeRectSize.width - width));
		int leftoverVert = std::abs(static_cast<int>(freeRectSize.height - height));
		int leftover = std::max(leftoverHoriz, leftoverVert);

		return leftover;
	}

	int ScoreBestShortSideFit(int width, int height, const NzRectui& freeRectSize)
	{
		int leftoverHoriz = std::abs(static_cast<int>(freeRectSize.width - width));
		int leftoverVert = std::abs(static_cast<int>(freeRectSize.height - height));
		int leftover = std::min(leftoverHoriz, leftoverVert);

		return leftover;
	}

	int ScoreWorstAreaFit(int width, int height, const NzRectui& freeRectSize)
	{
		return -ScoreBestAreaFit(width, height, freeRectSize);
	}

	int ScoreWorstLongSideFit(int width, int height, const NzRectui& freeRectSize)
	{
		return -ScoreBestLongSideFit(width, height, freeRectSize);
	}

	int ScoreWorstShortSideFit(int width, int height, const NzRectui& freeRectSize)
	{
		return -ScoreBestShortSideFit(width, height, freeRectSize);
	}
}

NzGuillotineBinPack::NzGuillotineBinPack()
{
	Reset();
}

NzGuillotineBinPack::NzGuillotineBinPack(unsigned int width, unsigned int height)
{
	Reset(width, height);
}

NzGuillotineBinPack::NzGuillotineBinPack(const NzVector2ui& size)
{
	Reset(size);
}

void NzGuillotineBinPack::Clear()
{
	m_freeRectangles.clear();
	m_freeRectangles.push_back(NzRectui(0, 0, m_width, m_height));

	m_usedArea = 0;
}

void NzGuillotineBinPack::Expand(unsigned int newWidth, unsigned newHeight)
{
	unsigned int oldWidth = m_width;
	unsigned int oldHeight = m_height;

	m_width = std::max(newWidth, m_width);
	m_height = std::max(newHeight, m_height);

	if (m_width > oldWidth)
		m_freeRectangles.push_back(NzRectui(oldWidth, 0, m_width - oldWidth, oldHeight));

	if (m_height > oldHeight)
		m_freeRectangles.push_back(NzRectui(0, oldHeight, m_width, m_height - oldHeight));

	// On va ensuite fusionner les rectangles tant que possible
	while (MergeFreeRectangles());
}

void NzGuillotineBinPack::Expand(const NzVector2ui& newSize)
{
	Expand(newSize.x, newSize.y);
}

void NzGuillotineBinPack::FreeRectangle(const NzRectui& rect)
{
	///DOC: Cette méthode ne devrait recevoir que des rectangles calculés par la méthode Insert et peut provoquer de la fragmentation
	m_freeRectangles.push_back(rect);

	m_usedArea -= rect.width * rect.height;
}

unsigned int NzGuillotineBinPack::GetHeight() const
{
	return m_height;
}

float NzGuillotineBinPack::GetOccupancy() const
{
	return static_cast<float>(m_usedArea)/(m_width*m_height);
}

NzVector2ui NzGuillotineBinPack::GetSize() const
{
	return NzVector2ui(m_width, m_height);
}

unsigned int NzGuillotineBinPack::GetWidth() const
{
	return m_width;
}

bool NzGuillotineBinPack::Insert(NzRectui* rects, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
{
	return Insert(rects, nullptr, nullptr, count, merge, rectChoice, splitMethod);
}

bool NzGuillotineBinPack::Insert(NzRectui* rects, bool* flipped, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
{
	return Insert(rects, flipped, nullptr, count, merge, rectChoice, splitMethod);
}

bool NzGuillotineBinPack::Insert(NzRectui* rects, bool* flipped, bool* inserted, unsigned int count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
{
	std::vector<NzRectui*> remainingRects(count); // La position du rectangle
	for (unsigned int i = 0; i < count; ++i)
		remainingRects[i] = &rects[i];

	// Pack rectangles one at a time until we have cleared the rects array of all rectangles.
	while (!remainingRects.empty())
	{
		// Stores the penalty score of the best rectangle placement - bigger=worse, smaller=better.
		bool bestFlipped;
		int bestFreeRect;
		int bestRect;
		int bestScore = std::numeric_limits<int>::max();

		for (std::size_t i = 0; i < m_freeRectangles.size(); ++i)
		{
			NzRectui& freeRect = m_freeRectangles[i];

			for (std::size_t j = 0; j < remainingRects.size(); ++j)
			{
				NzRectui& rect = *remainingRects[j];

				// If this rectangle is a perfect match, we pick it instantly.
				if (rect.width == freeRect.width && rect.height == freeRect.height)
				{
					bestFreeRect = i;
					bestRect = j;
					bestFlipped = false;
					bestScore = std::numeric_limits<int>::min();
					i = m_freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
					break;
				}
				// If flipping this rectangle is a perfect match, pick that then.
				else if (rect.height == freeRect.width && rect.width == freeRect.height)
				{
					bestFreeRect = i;
					bestRect = j;
					bestFlipped = true;
					bestScore = std::numeric_limits<int>::min();
					i = m_freeRectangles.size(); // Force a jump out of the outer loop as well - we got an instant fit.
					break;
				}
				// Try if we can fit the rectangle upright.
				else if (rect.width <= freeRect.width && rect.height <= freeRect.height)
				{
					int score = ScoreByHeuristic(rect.width, rect.height, freeRect, rectChoice);
					if (score < bestScore)
					{
						bestFreeRect = i;
						bestRect = j;
						bestFlipped = false;
						bestScore = score;
					}
				}
				// If not, then perhaps flipping sideways will make it fit?
				else if (rect.height <= freeRect.width && rect.width <= freeRect.height)
				{
					int score = ScoreByHeuristic(rect.height, rect.width, freeRect, rectChoice);
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
		if (bestScore == std::numeric_limits<int>::max())
		{
			// Si nous le pouvons, on marque les rectangles n'ayant pas pu être insérés
			if (inserted)
			{
				for (NzRectui* rect : remainingRects)
				{
					unsigned int position = rect - rects;
					inserted[position] = false;
				}
			}

			return false;
		}

		// Otherwise, we're good to go and do the actual packing.
		unsigned int position = remainingRects[bestRect] - rects;
		NzRectui& rect = *remainingRects[bestRect];
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

bool NzGuillotineBinPack::MergeFreeRectangles()
{
	///DOC: Renvoie true s'il y a eu fusion (et donc si une fusion est encore possible)
	std::size_t oriSize = m_freeRectangles.size();

	// Do a Theta(n^2) loop to see if any pair of free rectangles could me merged into one.
	// Note that we miss any opportunities to merge three rectangles into one. (should call this function again to detect that)
	for (std::size_t i = 0; i < m_freeRectangles.size(); ++i)
	{
		NzRectui& firstRect = m_freeRectangles[i];

		for (std::size_t j = i+1; j < m_freeRectangles.size(); ++j)
		{
			NzRectui& secondRect = m_freeRectangles[j];

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

void NzGuillotineBinPack::Reset()
{
	m_height = 0;
	m_width = 0;

	Clear();
}

void NzGuillotineBinPack::Reset(unsigned int width, unsigned int height)
{
	m_height = height;
	m_width = width;

	Clear();
}

void NzGuillotineBinPack::Reset(const NzVector2ui& size)
{
	Reset(size.x, size.y);
}

void NzGuillotineBinPack::SplitFreeRectAlongAxis(const NzRectui& freeRect, const NzRectui& placedRect, bool splitHorizontal)
{
	// Form the two new rectangles.
	NzRectui bottom;
	bottom.x = freeRect.x;
	bottom.y = freeRect.y + placedRect.height;
	bottom.height = freeRect.height - placedRect.height;

	NzRectui right;
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

void NzGuillotineBinPack::SplitFreeRectByHeuristic(const NzRectui& freeRect, const NzRectui& placedRect, GuillotineSplitHeuristic method)
{
	// Compute the lengths of the leftover area.
	const int w = freeRect.width - placedRect.width;
	const int h = freeRect.height - placedRect.height;

	// Placing placedRect into freeRect results in an L-shaped free area, which must be split into
	// two disjoint rectangles. This can be achieved with by splitting the L-shape using a single line.
	// We have two choices: horizontal or vertical.

	// Use the given heuristic to decide which choice to make.

	bool splitHorizontal;
	switch (method)
	{
		case SplitLongerAxis:
			// Split along the longer total axis.
			splitHorizontal = (freeRect.width > freeRect.height);
			break;

		case SplitLongerLeftoverAxis:
			// Split along the longer leftover axis.
			splitHorizontal = (w > h);
			break;

		case SplitMaximizeArea:
			// Maximize the smaller area == minimize the larger area.
			// Tries to make the rectangles more even-sized.
			splitHorizontal = (placedRect.width * h <= w * placedRect.height);
			break;

		case SplitMinimizeArea:
			// Maximize the larger area == minimize the smaller area.
			// Tries to make the single bigger rectangle.
			splitHorizontal = (placedRect.width * h > w * placedRect.height);
			break;

		case SplitShorterAxis:
			// Split along the shorter total axis.
			splitHorizontal = (freeRect.width <= freeRect.height);
			break;

		case SplitShorterLeftoverAxis:
			// Split along the shorter leftover axis.
			splitHorizontal = (w <= h);
			break;

		default:
			NazaraError("Split heuristic out of enum (0x" + NzString::Number(method, 16) + ')');
			splitHorizontal = true;
	}

	// Perform the actual split.
	SplitFreeRectAlongAxis(freeRect, placedRect, splitHorizontal);
}

int NzGuillotineBinPack::ScoreByHeuristic(int width, int height, const NzRectui& freeRect, FreeRectChoiceHeuristic rectChoice)
{
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

	NazaraError("Rect choice heuristic out of enum (0x" + NzString::Number(rectChoice, 16) + ')');
	return std::numeric_limits<int>::max();
}
