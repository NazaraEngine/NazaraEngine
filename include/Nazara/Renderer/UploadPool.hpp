// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_UPLOADPOOL_HPP
#define NAZARA_RENDERER_UPLOADPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Export.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API UploadPool
	{
		public:
			struct Allocation;

			UploadPool() = default;
			UploadPool(const UploadPool&) = delete;
			UploadPool(UploadPool&&) noexcept = default;
			~UploadPool() = default;

			virtual Allocation& Allocate(UInt64 size) = 0;
			virtual Allocation& Allocate(UInt64 size, UInt64 alignment) = 0;

			virtual void Reset() = 0;

			UploadPool& operator=(const UploadPool&) = delete;
			UploadPool& operator=(UploadPool&&) = delete;

			struct NAZARA_RENDERER_API Allocation
			{
				Allocation() = default;
				Allocation(const Allocation&) = delete;
				Allocation(Allocation&&) = default;
				~Allocation();

				Allocation& operator=(const Allocation&) = delete;
				Allocation& operator=(Allocation&&) = default;

				void* mappedPtr;
				UInt64 size;
			};
	};
}

#include <Nazara/Renderer/UploadPool.inl>

#endif // NAZARA_RENDERER_UPLOADPOOL_HPP
