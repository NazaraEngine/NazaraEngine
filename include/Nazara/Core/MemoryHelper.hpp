// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYHELPER_HPP
#define NAZARA_MEMORYHELPER_HPP

#if defined(NAZARA_COMPILER_MSVC) || defined(NAZARA_COMPILER_MINGW)

#include <malloc.h>

#define NAZARA_ALLOCA(size) _alloca(size)
#define NAZARA_ALLOCA_SUPPORT

#elif defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
#include <alloca.h>

#define NAZARA_ALLOCA(size) alloca(size)
#define NAZARA_ALLOCA_SUPPORT

#endif

#ifdef NAZARA_ALLOCA_SUPPORT
	#define NazaraStackAllocation(size) Nz::StackAllocation(NAZARA_ALLOCA(size))
#else
	#define NazaraStackAllocation(size) Nz::StackAllocation(Nz::OperatorNew(size))
#endif

#include <cstddef>

namespace Nz
{
	void OperatorDelete(void* ptr);
	void* OperatorNew(std::size_t size);

	template<typename T, typename... Args>
	T* PlacementNew(T* ptr, Args&&... args);

	template<typename T>
	void PlacementDestroy(T* ptr);

	class StackAllocation
	{
		public:
			explicit StackAllocation(void* stackMemory);
			~StackAllocation();

			void* GetPtr();

			operator void*();

		private:
			void* m_ptr;
	};
}

#include <Nazara/Core/MemoryHelper.inl>

#endif // NAZARA_MEMORYHELPER_HPP
