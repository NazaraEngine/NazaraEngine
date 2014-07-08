// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYMANAGER_HPP
#define NAZARA_MEMORYMANAGER_HPP

#include <Nazara/Prerequesites.hpp>
#include <cstdio>
#include <cstring>

class NAZARA_API NzMemoryManager
{
	public:
		static void* Allocate(std::size_t size, bool multi = false, const char* file = nullptr, unsigned int line = 0);

		static void Free(void* pointer, bool multi = false);

		static unsigned int GetAllocatedBlockCount();
		static std::size_t GetAllocatedSize();
		static unsigned int GetAllocationCount();

		static void NextFree(const char* file, unsigned int line);

	private:
		NzMemoryManager();
		~NzMemoryManager();

		static void Initialize();
		static void TimeInfo(char buffer[23]);
		static void Uninitialize();
};

#endif // NAZARA_MEMORYMANAGER_HPP
