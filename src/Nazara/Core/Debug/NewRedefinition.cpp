// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Config.hpp>
#if NAZARA_CORE_MANAGE_MEMORY

#include <Nazara/Core/MemoryManager.hpp>
#include <new> // Nécessaire ?

#define NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
#include <Nazara/Core/Debug/NewRedefinition.hpp>

void* operator new(std::size_t size, const char* file, unsigned int line)
{
	return Nz::MemoryManager::Allocate(size, false, file, line);
}

void* operator new[](std::size_t size, const char* file, unsigned int line)
{
	return Nz::MemoryManager::Allocate(size, true, file, line);
}

void operator delete(void* ptr, const char* file, unsigned int line) noexcept
{
	Nz::MemoryManager::NextFree(file, line);
	Nz::MemoryManager::Free(ptr, false);
}

void operator delete[](void* ptr, const char* file, unsigned int line) noexcept
{
	Nz::MemoryManager::NextFree(file, line);
	Nz::MemoryManager::Free(ptr, true);
}

#endif // NAZARA_CORE_MANAGE_MEMORY
