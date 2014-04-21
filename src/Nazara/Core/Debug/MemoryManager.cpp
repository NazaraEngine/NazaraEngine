// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_DEBUG_MEMORYMANAGER_DISABLE_REDEFINITION

#include <Nazara/Core/Debug/MemoryManager.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <pthread.h>
#endif

// Le seul fichier n'ayant pas à inclure Debug.hpp

namespace
{
	struct Block
	{
		std::size_t size;
		const char* file;
		Block* prev;
		Block* next;
		bool array;
		unsigned int line;
		unsigned int magic;
	};

	bool s_initialized = false;
	const unsigned int s_magic = 0x51429EE;
	const char* s_MLTFileName = "NazaraLeaks.log";
	const char* s_nextFreeFile = "(Internal error)";
	unsigned int s_nextFreeLine = 0;

	Block s_list =
	{
		0,
		nullptr,
		&s_list,
		&s_list,
		false,
		0,
		s_magic
	};
	unsigned int s_allocatedBlock = 0;
	std::size_t s_allocatedSize = 0;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	CRITICAL_SECTION s_mutex;
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
	#endif
}

NzMemoryManager::NzMemoryManager()
{
}

NzMemoryManager::~NzMemoryManager()
{
	Uninitialize();
}

void* NzMemoryManager::Allocate(std::size_t size, bool multi, const char* file, unsigned int line)
{
	if (!s_initialized)
		Initialize();

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&s_mutex);
	#endif

	Block* ptr = reinterpret_cast<Block*>(std::malloc(size+sizeof(Block)));
	if (!ptr)
	{
		// Pas d'information de temps (Car nécessitant une allocation)
		FILE* log = std::fopen(s_MLTFileName, "a");
		std::fprintf(log, "Failed to allocate memory (%zu bytes)\n", size);
		std::fclose(log);

		return nullptr; // Impossible d'envoyer une exception car cela allouerait de la mémoire avec new (boucle infinie)
	}

	ptr->array = multi;
	ptr->file = file;
	ptr->line = line;
	ptr->size = size;
	ptr->magic = s_magic;

	ptr->prev = s_list.prev;
	ptr->next = &s_list;
	s_list.prev->next = ptr;
	s_list.prev = ptr;

	s_allocatedBlock++;
	s_allocatedSize += size;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&s_mutex);
	#endif

	return reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block);
}

void NzMemoryManager::Free(void* pointer, bool multi)
{
	if (!pointer)
		return;

	Block* ptr = reinterpret_cast<Block*>(reinterpret_cast<nzUInt8*>(pointer) - sizeof(Block));
	if (ptr->magic != s_magic)
		return;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	EnterCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_lock(&s_mutex);
	#endif

	if (ptr->array != multi)
	{
		char timeStr[23];
		TimeInfo(timeStr);

		FILE* log = std::fopen(s_MLTFileName, "a");

		if (s_nextFreeFile)
		{
			if (multi)
				std::fprintf(log, "%s Warning: delete[] after new at %s:%u\n", timeStr, s_nextFreeFile, s_nextFreeLine);
			else
				std::fprintf(log, "%s Warning: delete after new[] at %s:%u\n", timeStr, s_nextFreeFile, s_nextFreeLine);
		}
		else
		{
			if (multi)
				std::fprintf(log, "%s Warning: delete[] after new at unknown position\n", timeStr);
			else
				std::fprintf(log, "%s Warning: delete after new[] at unknown position\n", timeStr);
		}

		std::fclose(log);
	}

	ptr->magic = 0; // Évitons des problèmes
	ptr->prev->next = ptr->next;
	ptr->next->prev = ptr->prev;

	s_allocatedBlock--;
	s_allocatedSize -= ptr->size;

	std::free(ptr);

	s_nextFreeFile = nullptr;
	s_nextFreeLine = 0;

	#if defined(NAZARA_PLATFORM_WINDOWS)
	LeaveCriticalSection(&s_mutex);
	#elif defined(NAZARA_PLATFORM_POSIX)
	pthread_mutex_unlock(&s_mutex);
	#endif
}

unsigned int NzMemoryManager::GetAllocatedBlockCount()
{
	return s_allocatedBlock;
}

std::size_t NzMemoryManager::GetAllocatedSize()
{
	return s_allocatedSize;
}

void NzMemoryManager::NextFree(const char* file, unsigned int line)
{
	s_nextFreeFile = file;
	s_nextFreeLine = line;
}

void NzMemoryManager::Initialize()
{
	char timeStr[23];
	TimeInfo(timeStr);

	FILE* file = std::fopen(s_MLTFileName, "w");
	std::fprintf(file, "%s ==============================\n", timeStr);
	std::fprintf(file, "%s   Nazara Memory Leak Tracker  \n", timeStr);
	std::fprintf(file, "%s ==============================\n", timeStr);
	std::fclose(file);

	if (std::atexit(Uninitialize) != 0)
	{
		static NzMemoryManager manager;
	}

	#ifdef NAZARA_PLATFORM_WINDOWS
	InitializeCriticalSection(&s_mutex);
	#endif

	s_initialized = true;
}

void NzMemoryManager::TimeInfo(char buffer[23])
{
	time_t currentTime = std::time(nullptr);
	std::strftime(buffer, 23, "%d/%m/%Y - %H:%M:%S:", std::localtime(&currentTime));
}

void NzMemoryManager::Uninitialize()
{
	#ifdef NAZARA_PLATFORM_WINDOWS
	DeleteCriticalSection(&s_mutex);
	#endif

	FILE* log = std::fopen(s_MLTFileName, "a");

	char timeStr[23];
	TimeInfo(timeStr);

	std::fprintf(log, "%s Application finished, checking leaks...\n", timeStr);

	if (s_allocatedBlock == 0)
	{
		std::fprintf(log, "%s ==============================\n", timeStr);
		std::fprintf(log, "%s        No leak detected       \n", timeStr);
		std::fprintf(log, "%s ==============================", timeStr);
	}
	else
	{
		std::fprintf(log, "%s ==============================\n", timeStr);
		std::fprintf(log, "%s    Leaks have been detected   \n", timeStr);
		std::fprintf(log, "%s ==============================\n\n", timeStr);
		std::fputs("Leak list:\n", log);

		Block* ptr = s_list.next;
		while (ptr != &s_list)
		{
			if (ptr->file)
				std::fprintf(log, "-0x%p -> %zu bytes allocated at %s:%u\n", reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block), ptr->size, ptr->file, ptr->line);
			else
				std::fprintf(log, "-0x%p -> %zu bytes allocated at unknown position\n", reinterpret_cast<nzUInt8*>(ptr) + sizeof(Block), ptr->size);

			void* pointer = ptr;
			ptr = ptr->next;

			std::free(pointer);
		}

		std::fprintf(log, "\n%u blocks leaked (%zu bytes)", s_allocatedBlock, s_allocatedSize);
	}

	std::fclose(log);
}

void* operator new(std::size_t size, const char* file, unsigned int line)
{
	return NzMemoryManager::Allocate(size, false, file, line);
}

void* operator new[](std::size_t size, const char* file, unsigned int line)
{
	return NzMemoryManager::Allocate(size, true, file, line);
}

void operator delete(void* ptr, const char* file, unsigned int line) noexcept
{
	NzMemoryManager::NextFree(file, line);
	NzMemoryManager::Free(ptr, false);
}

void operator delete[](void* ptr, const char* file, unsigned int line) noexcept
{
	NzMemoryManager::NextFree(file, line);
	NzMemoryManager::Free(ptr, true);
}
