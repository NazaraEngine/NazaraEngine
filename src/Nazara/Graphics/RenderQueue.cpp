// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderQueue.hpp>
#include <algorithm>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	void RenderQueueInternal::Sort()
	{
		std::sort(m_orderedRenderQueue.begin(), m_orderedRenderQueue.end(), [](const RenderDataPair& lhs, const RenderDataPair& rhs)
		{
			/*
			Original code:
			if (lhs.first == rhs.first)
				return lhs.second < rhs.second;
			else
				return lhs.first < rhs.first;

			Clang seems to the the only one to prevent branching with the original code (using cmov)
			Rewriting the code with bit ops seems to prevent branching with Clang/GCC/MSVC
			*/
			bool equal = lhs.first == rhs.first;
			bool compareFirst = lhs.first < rhs.first;
			bool compareSecond = lhs.second < rhs.second;

			return (equal & compareSecond) | (!equal & compareFirst);
		});
	}
}
