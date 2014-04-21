// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_DEBUG_MEMORYMANAGER_HPP
#define NAZARA_DEBUG_MEMORYMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <cstdio>
#include <cstring>

class NAZARA_API NzMemoryManager
{
	public:
		static void* Allocate(std::size_t size, bool multi, const char* file = nullptr, unsigned int line = 0);

		static void Free(void* pointer, bool multi);

		static unsigned int GetAllocatedBlockCount();
		static std::size_t GetAllocatedSize();

		static void NextFree(const char* file, unsigned int line);

	private:
		NzMemoryManager();
		~NzMemoryManager();

		static void Initialize();
		static void TimeInfo(char buffer[23]);
		static void Uninitialize();
};

NAZARA_API void* operator new(std::size_t size, const char* file, unsigned int line);
NAZARA_API void* operator new[](std::size_t size, const char* file, unsigned int line);
NAZARA_API void operator delete(void* ptr, const char* file, unsigned int line) noexcept;
NAZARA_API void operator delete[](void* ptr, const char* file, unsigned int line) noexcept;

#endif // NAZARA_DEBUG_MEMORYMANAGER_HPP

#ifndef NAZARA_DEBUG_MEMORYMANAGER_DISABLE_REDEFINITION
	#define delete NzMemoryManager::NextFree(__FILE__, __LINE__), delete
	#define new new(__FILE__, __LINE__)
#endif
