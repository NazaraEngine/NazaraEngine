// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RenderQueue.hpp>
#include <Nazara/Core/Error.hpp>

namespace Nz
{
	template<typename RenderData>
	void RenderQueue<RenderData>::Clear()
	{
		m_orderedRenderQueue.clear();
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
		m_orderedRenderQueue.clear();
		m_orderedRenderQueue.reserve(m_data.size());

		std::size_t dataIndex = 0;
		for (const RenderData& renderData : m_data)
			m_orderedRenderQueue.emplace_back(func(renderData), dataIndex++);

		RenderQueueInternal::Sort();
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::const_iterator RenderQueue<RenderData>::begin() const
	{
		return const_iterator(this, 0);
	}

	template<typename RenderData>
	bool RenderQueue<RenderData>::empty() const
	{
		return m_orderedRenderQueue.empty();
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::const_iterator RenderQueue<RenderData>::end() const
	{
		return const_iterator(this, m_orderedRenderQueue.size());
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::size_type RenderQueue<RenderData>::size() const
	{
		return m_orderedRenderQueue.size();
	}

	template<typename RenderData>
	const RenderData& RenderQueue<RenderData>::GetData(std::size_t i) const
	{
		NazaraAssert(i < m_orderedRenderQueue.size(), "Cannot dereference post-end iterator");

		return m_data[m_orderedRenderQueue[i].second];
	}

	
	template<typename RenderData>
	RenderQueue<RenderData>::const_iterator::const_iterator(const RenderQueue* queue, std::size_t nextId) :
	m_nextDataId(nextId),
	m_queue(queue)
	{
	}

	template<typename RenderData>
	RenderQueue<RenderData>::const_iterator::const_iterator(const const_iterator& it) :
	m_nextDataId(it.m_nextDataId),
	m_queue(it.m_queue)
	{
	}

	template<typename RenderData>
	const RenderData& RenderQueue<RenderData>::const_iterator::operator*() const
	{
		return m_queue->GetData(m_nextDataId);
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::const_iterator& RenderQueue<RenderData>::const_iterator::operator=(const const_iterator& it)
	{
		m_nextDataId = it.m_nextDataId;
		m_queue = it.m_queue;

		return *this;
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::const_iterator& RenderQueue<RenderData>::const_iterator::operator++()
	{
		++m_nextDataId;

		return *this;
	}

	template<typename RenderData>
	typename RenderQueue<RenderData>::const_iterator RenderQueue<RenderData>::const_iterator::operator++(int)
	{
		return iterator(m_queue, m_nextDataId++);
	}

	template<typename RenderData>
	bool RenderQueue<RenderData>::const_iterator::operator==(const typename RenderQueue<RenderData>::const_iterator& rhs) const
	{
		NazaraAssert(m_queue == rhs.m_queue, "Cannot compare iterator coming from different queues");

		return m_nextDataId == rhs.m_nextDataId;
	}

	template<typename RenderData>
	bool RenderQueue<RenderData>::const_iterator::operator!=(const typename RenderQueue<RenderData>::const_iterator& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename RenderData>
	void RenderQueue<RenderData>::const_iterator::swap(typename RenderQueue<RenderData>::const_iterator& rhs)
	{
		NazaraAssert(m_queue == rhs.m_queue, "Cannot swap iterator coming from different queues");

		using std::swap;

		swap(m_nextDataId, rhs.m_nextDataId);
	}
}
