// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERQUEUE_HPP
#define NAZARA_GRAPHICS_RENDERQUEUE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <vector>

namespace Nz
{
	template<typename RenderData>
	class RenderQueue
	{
		public:
			using const_iterator = const RenderData*;
			using size_type = std::size_t;

			RenderQueue() = default;
			RenderQueue(const RenderQueue&) = default;
			RenderQueue(RenderQueue&&) noexcept = default;
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
			RenderQueue& operator=(RenderQueue&&) noexcept = default;

		private:
			std::vector<RenderData> m_data;
	};
}

#include <Nazara/Graphics/RenderQueue.inl>

#endif // NAZARA_GRAPHICS_RENDERQUEUE_HPP
