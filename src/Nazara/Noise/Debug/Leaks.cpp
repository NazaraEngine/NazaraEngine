// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Config.hpp>
#if NAZARA_NOISE_MEMORYMANAGER || defined(NAZARA_DEBUG)
#define NAZARA_DEBUG_MEMORYMANAGER_DISABLE_REDEFINITION
#include <Nazara/Core/Debug/MemoryManager.hpp>
#include <new>

void* operator new(std::size_t size)
{
	return NzMemoryManager::Allocate(size, false);
}

void* operator new[](std::size_t size)
{
	return NzMemoryManager::Allocate(size, true);
}

void operator delete(void* pointer) noexcept
{
	NzMemoryManager::Free(pointer, false);
}

void operator delete[](void* pointer) noexcept
{
	NzMemoryManager::Free(pointer, true);
}

#endif
