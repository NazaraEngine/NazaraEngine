// Copyright (C) 2014 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/Config.hpp>
#if NAZARA_MODULENAME_MEMORYMANAGER || defined(NAZARA_DEBUG)
#define NAZARA_DEBUG_MEMORYMANAGER_DISABLE_REDEFINITION
#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>
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
