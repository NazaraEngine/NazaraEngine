// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERELEMENTPOOL_HPP
#define NAZARA_GRAPHICS_RENDERELEMENTPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderElementOwner.hpp>
#include <NazaraUtils/MemoryPool.hpp>

namespace Nz
{
	class RenderElementOwner;

	class NAZARA_GRAPHICS_API RenderElementPoolBase
	{
		public:
			RenderElementPoolBase() = default;
			RenderElementPoolBase(const RenderElementPoolBase&) = delete;
			RenderElementPoolBase(RenderElementPoolBase&&) = delete;
			virtual ~RenderElementPoolBase();

			virtual void Clear() = 0;

			virtual void Free(std::size_t index) = 0;

			RenderElementPoolBase& operator=(const RenderElementPoolBase&) = delete;
			RenderElementPoolBase& operator=(RenderElementPoolBase&&) = delete;
	};

	template<typename T>
	class RenderElementPool final : public RenderElementPoolBase
	{
		public:
			RenderElementPool();
			RenderElementPool(const RenderElementPool&) = delete;
			RenderElementPool(RenderElementPool&&) = delete;
			~RenderElementPool() = default;

			template<typename... Args> RenderElementOwner Allocate(Args&&... args);

			void Clear() override;

			void Free(std::size_t index) override;

			RenderElementPool& operator=(const RenderElementPool&) = delete;
			RenderElementPool& operator=(RenderElementPool&&) = delete;

		private:
			MemoryPool<T> m_pool;
	};
}

#include <Nazara/Graphics/RenderElementPool.inl>

#endif // NAZARA_GRAPHICS_RENDERELEMENTPOOL_HPP
