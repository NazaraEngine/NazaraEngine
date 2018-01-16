// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de #pragma once car ce fichier est prévu pour être inclus plusieurs fois

#include <Nazara/Core/Config.hpp>
#if NAZARA_CORE_MANAGE_MEMORY

#ifndef NAZARA_DEBUG_NEWREDEFINITION_HPP
#define NAZARA_DEBUG_NEWREDEFINITION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MemoryManager.hpp>

NAZARA_CORE_API void* operator new(std::size_t size, const char* file, unsigned int line);
NAZARA_CORE_API void* operator new[](std::size_t size, const char* file, unsigned int line);
NAZARA_CORE_API void operator delete(void* ptr, const char* file, unsigned int line) noexcept;
NAZARA_CORE_API void operator delete[](void* ptr, const char* file, unsigned int line) noexcept;

#endif // NAZARA_DEBUG_NEWREDEFINITION_HPP

#ifndef NAZARA_DEBUG_NEWREDEFINITION_DISABLE_REDEFINITION
	#define delete MemoryManager::NextFree(__FILE__, __LINE__), delete
	#define new new(__FILE__, __LINE__)
#endif

#endif // NAZARA_CORE_MANAGE_MEMORY
