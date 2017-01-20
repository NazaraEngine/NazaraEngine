// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Config.hpp>
#if NAZARA_UTILITY_MANAGE_MEMORY

#include <Nazara/Core/MemoryManager.hpp>
#include <new> // Nécessaire ?

void* operator new(std::size_t size)
{
	return Nz::MemoryManager::Allocate(size, false);
}

void* operator new[](std::size_t size)
{
	return Nz::MemoryManager::Allocate(size, true);
}

void operator delete(void* pointer) noexcept
{
	Nz::MemoryManager::Free(pointer, false);
}

void operator delete[](void* pointer) noexcept
{
	Nz::MemoryManager::Free(pointer, true);
}

#endif // NAZARA_UTILITY_MANAGE_MEMORY
