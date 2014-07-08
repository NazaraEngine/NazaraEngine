// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Config.hpp>
#if NAZARA_UTILITY_MANAGE_MEMORY

#include <Nazara/Core/MemoryManager.hpp>
#include <new> // Nécessaire ?

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

#endif // NAZARA_UTILITY_MANAGE_MEMORY
