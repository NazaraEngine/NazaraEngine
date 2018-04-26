// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERQUEUE_HPP
#define NAZARA_RENDERQUEUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class RenderQueueInternal
	{
		public:
			using Index = Nz::UInt64;

			RenderQueueInternal() = default;
			~RenderQueueInternal() = default;

		protected:
			using RenderDataPair = std::pair<Index, std::size_t>;

			void Sort();

			std::vector<RenderDataPair> m_orderedRenderQueue;
	};

	template<typename RenderData>
	class RenderQueue : public RenderQueueInternal
	{
		public:
			class const_iterator;
			friend const_iterator;
			using size_type = std::size_t;

			RenderQueue() = default;
			RenderQueue(const RenderQueue&) = default;
			RenderQueue(RenderQueue&&) = default;
			~RenderQueue() = default;

			void Clear();

			void Insert(RenderData&& data);

			template<typename IndexFunc> void Sort(IndexFunc&& func);

			// STL API
			inline const_iterator begin() const;
			inline bool empty() const;
			inline const_iterator end() const;
			inline size_type size() const;

			RenderQueue& operator=(const RenderQueue&) = default;
			RenderQueue& operator=(RenderQueue&&) = default;

		private:
			const RenderData& GetData(std::size_t i) const;

			std::vector<RenderData> m_data;
	};
	
	template<typename RenderData>
	class RenderQueue<RenderData>::const_iterator : public std::iterator<std::forward_iterator_tag, const RenderData>
	{
		friend RenderQueue;

		public:
			const_iterator(const const_iterator& it);

			const RenderData& operator*() const;

			const_iterator& operator=(const const_iterator& it);
			const_iterator& operator++();
			const_iterator operator++(int);

			bool operator==(const const_iterator& rhs) const;
			bool operator!=(const const_iterator& rhs) const;

			void swap(const_iterator& rhs);

		private:
			const_iterator(const RenderQueue* queue, std::size_t nextId);

			std::size_t m_nextDataId;
			const RenderQueue* m_queue;
	};

}

#include <Nazara/Graphics/RenderQueue.inl>

#endif // NAZARA_RENDERQUEUE_HPP
