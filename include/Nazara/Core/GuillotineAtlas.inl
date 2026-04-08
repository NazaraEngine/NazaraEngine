// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline GuillotineAtlas::GuillotineAtlas(PixelFormat pixelFormat, UInt32 initialLayerSize) :
	m_rectChoiceHeuristic(GuillotineBinPack::RectBestAreaFit),
	m_rectSplitHeuristic(GuillotineBinPack::SplitMinimizeArea),
	m_pixelFormat(pixelFormat),
	m_initialLayerSize(initialLayerSize),
	m_maxLayerSize(std::max<UInt32>(initialLayerSize, 16384u))
	{
		NazaraAssertMsg(m_initialLayerSize != 0, "initial layer size must be positive (%u)", initialLayerSize);
	}

	inline UInt32 GuillotineAtlas::GetMaxLayerSize() const
	{
		return m_maxLayerSize;
	}

	inline GuillotineBinPack::FreeRectChoiceHeuristic GuillotineAtlas::GetRectChoiceHeuristic() const
	{
		return m_rectChoiceHeuristic;
	}

	inline GuillotineBinPack::GuillotineSplitHeuristic GuillotineAtlas::GetRectSplitHeuristic() const
	{
		return m_rectSplitHeuristic;
	}

	inline PixelFormat GuillotineAtlas::GetPixelFormat() const
	{
		return m_pixelFormat;
	}

	inline void GuillotineAtlas::SetMaxLayerSize(UInt32 maxLayerSize)
	{
		m_maxLayerSize = maxLayerSize;
	}

	inline void GuillotineAtlas::SetRectChoiceHeuristic(GuillotineBinPack::FreeRectChoiceHeuristic heuristic)
	{
		m_rectChoiceHeuristic = heuristic;
	}

	inline void GuillotineAtlas::SetRectSplitHeuristic(GuillotineBinPack::GuillotineSplitHeuristic heuristic)
	{
		m_rectSplitHeuristic = heuristic;
	}
}
