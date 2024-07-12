// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	/*!
	* \brief Constructs a GuillotineBinPack object by default
	*/
	inline GuillotineBinPack::GuillotineBinPack()
	{
		Reset();
	}

	/*!
	* \brief Constructs a GuillotineBinPack object with area
	*
	* \param size Vector2 representing the area (width, height)
	*/
	inline GuillotineBinPack::GuillotineBinPack(const Vector2ui32& size)
	{
		Reset(size);
	}

	/*!
	* \brief Clears the content
	*/
	inline void GuillotineBinPack::Clear()
	{
		m_freeRectangles.clear();
		m_freeRectangles.push_back(Rectui32(0, 0, m_width, m_height));

		m_usedArea = 0;
	}

	/*!
	* \brief Frees the rectangle
	*
	* \param rect Area to free
	*
	* \remark This method should only be called with computed rectangles by the method Insert and can produce fragmentation
	*/
	inline void GuillotineBinPack::FreeRectangle(const Rectui32& rect)
	{
		m_freeRectangles.push_back(rect);

		m_usedArea -= rect.width * rect.height;
	}

	/*!
	* \brief Gets the height
	* \return Height of the area
	*/
	inline UInt32 GuillotineBinPack::GetHeight() const
	{
		return m_height;
	}

	/*!
	* \brief Gets percentage of occupation
	* \return Percentage of the already occupied area
	*/
	inline float GuillotineBinPack::GetOccupancy() const
	{
		return static_cast<float>(m_usedArea) / (m_width * m_height);
	}

	/*!
	* \brief Resets the area
	*/
	inline void GuillotineBinPack::Reset()
	{
		m_height = 0;
		m_width = 0;

		Clear();
	}

	/*!
	* \brief Resets the area
	*
	* \param size Size of the area
	*/
	inline void GuillotineBinPack::Reset(const Vector2ui32& size)
	{
		m_height = size.y;
		m_width = size.x;

		Clear();
	}

	/*!
	* \brief Gets the size of the area
	* \return Size of the area
	*/
	inline Vector2ui32 GuillotineBinPack::GetSize() const
	{
		return Vector2ui32(m_width, m_height);
	}

	/*!
	* \brief Gets the area occupied by rects
	* \return Used area in pixels
	*/
	inline UInt32 GuillotineBinPack::GetUsedArea() const
	{
		return m_usedArea;
	}

	/*!
	* \brief Gets the width
	* \return Width of the area
	*/
	inline UInt32 GuillotineBinPack::GetWidth() const
	{
		return m_width;
	}

	/*!
	* \brief Inserts rectangles in the area
	* \return true if each rectangle could be inserted
	*
	* \param rects List of rectangles
	* \param count Count of rectangles
	* \param merge Merge possible
	* \param rectChoice Heuristic to use to free
	* \param splitMethod Heuristic to use to split
	*/
	inline bool GuillotineBinPack::Insert(Rectui32* rects, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
	{
		return Insert(rects, nullptr, nullptr, count, merge, rectChoice, splitMethod);
	}

	/*!
	* \brief Inserts rectangles in the area
	* \return true if each rectangle could be inserted
	*
	* \param rects List of rectangles
	* \param flipped List of flipped rectangles
	* \param count Count of rectangles
	* \param merge Merge possible
	* \param rectChoice Heuristic to use to free
	* \param splitMethod Heuristic to use to split
	*/
	inline bool GuillotineBinPack::Insert(Rectui32* rects, bool* flipped, UInt32 count, bool merge, FreeRectChoiceHeuristic rectChoice, GuillotineSplitHeuristic splitMethod)
	{
		return Insert(rects, flipped, nullptr, count, merge, rectChoice, splitMethod);
	}
}
