// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYHELPER_HPP
#define NAZARA_MEMORYHELPER_HPP

#if defined(NAZARA_COMPILER_MSVC) || defined(NAZARA_COMPILER_MINGW)

#include <malloc.h>

// with MSVC, using alloca with a size of zero returns a valid pointer
#define NAZARA_ALLOCA(size) _alloca(size)
#define NAZARA_ALLOCA_SUPPORT

#elif defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)

#include <alloca.h>

// with Clang/GCC, using alloca with a size of zero does nothing good
#define NAZARA_ALLOCA(size) alloca(((size) > 0) ? (size) : 1)
#define NAZARA_ALLOCA_SUPPORT

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
}

#include <Nazara/Core/MemoryHelper.inl>

#endif // NAZARA_MEMORYHELPER_HPP
