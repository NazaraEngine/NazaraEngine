// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEBUG_MEMORYLEAKTRACKER_HPP
#define NAZARA_DEBUG_MEMORYLEAKTRACKER_HPP

#define NAZARA_DEBUG_MEMORYLEAKTRACKER

#include <Nazara/Prerequesites.hpp>
#include <cstdio>
#include <cstring>

class NAZARA_API NzMemoryManager
{
	public:
		NzMemoryManager();
		~NzMemoryManager();

		static void* Allocate(std::size_t size, bool multi, const char* file = nullptr, unsigned int line = 0);
		static void Free(void* pointer, bool multi);
		static void NextFree(const char* file, unsigned int line);

	private:
		static void EnsureInitialization();
		static void Initialize();
		static char* TimeInfo();
		static void Uninitialize();
};

NAZARA_API void* operator new(std::size_t size, const char* file, unsigned int line);
NAZARA_API void* operator new[](std::size_t size, const char* file, unsigned int line);
NAZARA_API void operator delete(void* ptr, const char* file, unsigned int line) throw();
NAZARA_API void operator delete[](void* ptr, const char* file, unsigned int line) throw();

#undef NAZARA_DEBUG_MEMORYLEAKTRACKER

#endif // NAZARA_DEBUG_MEMORYLEAKTRACKER_HPP
