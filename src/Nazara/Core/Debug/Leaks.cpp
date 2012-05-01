// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#if NAZARA_CORE_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>
#include <new>

void* operator new(std::size_t size) throw(std::bad_alloc)
{
	return NzMemoryManager::Allocate(size, false);
}

void* operator new[](std::size_t size) throw(std::bad_alloc)
{
	return NzMemoryManager::Allocate(size, true);
}

void operator delete(void* pointer) throw()
{
	NzMemoryManager::Free(pointer, false);
}

void operator delete[](void* pointer) throw()
{
	NzMemoryManager::Free(pointer, true);
}
#endif
