// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <algorithm>

namespace Nz
{
	template<typename RenderData>
	void RenderQueue<RenderData>::Clear()
	{
		m_data.clear();
	}

	template<typename RenderData>
	void RenderQueue<RenderData>::Insert(RenderData&& data)
	{
		m_data.emplace_back(std::move(data));
	}

	template<typename RenderData>
	template<typename IndexFunc>
	void RenderQueue<RenderData>::Sort(IndexFunc&& func)
	{
		std::sort(m_data.begin(), m_data.end(), [&](const RenderData& lhs, const RenderData& rhs)
		{
			return func(lhs) < func(rhs);
		});
	}

	template<typename RenderData>
	auto RenderQueue<RenderData>::begin() const -> const_iterator
	{
		return &m_data[0];
	}

	template<typename RenderData>
	bool RenderQueue<RenderData>::empty() const
	{
		return m_data.empty();
	}

	template<typename RenderData>
	auto RenderQueue<RenderData>::end() const -> const_iterator
	{
		return begin() + m_data.size();
	}

	template<typename RenderData>
	auto RenderQueue<RenderData>::size() const -> size_type
	{
		return m_data.size();
	}
}
