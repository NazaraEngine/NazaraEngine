// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryManager.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <new>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <pthread.h>
#endif

// The only file that does not need to include Debug.hpp

namespace Nz
{
	namespace
	{
		constexpr unsigned int s_allocatedId = 0xDEADB33FUL;
		constexpr unsigned int s_freedId = 0x4B1DUL;

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

		bool s_allocationFilling = true;
		bool s_allocationLogging = false;
		bool s_initialized = false;
		const char* s_logFileName = "NazaraMemory.log";
		thread_local const char* s_nextFreeFile = "(Internal error)";
		thread_local unsigned int s_nextFreeLine = 0;

		Block s_list =
		{
			0,
			nullptr,
			&s_list,
			&s_list,
			false,
			0,
			0
		};

		unsigned int s_allocationCount = 0;
		unsigned int s_allocatedBlock = 0;
		std::size_t s_allocatedSize = 0;

		#if defined(NAZARA_PLATFORM_WINDOWS)
		CRITICAL_SECTION s_mutex;
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
		#else
		#error Lack of implementation: Mutex
		#endif
	}
	
	/*!
	* \ingroup core
	* \class Nz::MemoryManager
	* \brief Core class that represents a manager for the memory
	*/

	/*!
	* \brief Constructs a MemoryManager object by default
	*/

	MemoryManager::MemoryManager()
	{
	}

	/*!
	* \brief Destructs a MemoryManager object and calls Unitialize
	*/

	MemoryManager::~MemoryManager()
	{
		Uninitialize();
	}

	/*!
	* \brief Allocates memory
	* \return Raw memory allocated
	*
	* \param size Size to allocate
	* \parma multi Array or not
	* \param file File of the allocation
	* \param line Line of the allocation in the file
	*/

	void* MemoryManager::Allocate(std::size_t size, bool multi, const char* file, unsigned int line)
	{
		if (!s_initialized)
			Initialize();

		#if defined(NAZARA_PLATFORM_WINDOWS)
		EnterCriticalSection(&s_mutex);
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_lock(&s_mutex);
		#endif

		Block* ptr = static_cast<Block*>(std::malloc(size+sizeof(Block)));
		if (!ptr)
		{
			char timeStr[23];
			TimeInfo(timeStr);

			FILE* log = std::fopen(s_logFileName, "a");

			if (file)
				std::fprintf(log, "%s Failed to allocate %zu bytes at %s:%u\n", timeStr, size, file, line);
			else
				std::fprintf(log, "%s Failed to allocate %zu bytes at unknown position\n", timeStr, size);

			std::fclose(log);

			throw std::bad_alloc();
		}

		ptr->array = multi;
		ptr->file = file;
		ptr->line = line;
		ptr->size = size;
		ptr->magic = s_allocatedId;

		ptr->prev = s_list.prev;
		ptr->next = &s_list;
		s_list.prev->next = ptr;
		s_list.prev = ptr;

		s_allocatedBlock++;
		s_allocatedSize += size;
		s_allocationCount++;

		if (s_allocationFilling)
		{
			UInt8* data = reinterpret_cast<UInt8*>(ptr) + sizeof(Block);
			std::memset(data, 0xFF, size);
		}

		if (s_allocationLogging)
		{
			char timeStr[23];
			TimeInfo(timeStr);

			FILE* log = std::fopen(s_logFileName, "a");

			if (file)
				std::fprintf(log, "%s Allocated %zu bytes at %s:%u\n", timeStr, size, file, line);
			else
				std::fprintf(log, "%s Allocated %zu bytes at unknown position\n", timeStr, size);

			std::fclose(log);
		}

		#if defined(NAZARA_PLATFORM_WINDOWS)
		LeaveCriticalSection(&s_mutex);
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_unlock(&s_mutex);
		#endif

		return reinterpret_cast<UInt8*>(ptr) + sizeof(Block);
	}

	/*!
	* \brief Enables the filling of the allocation
	*
	* \param allocationFilling If true, sets the rest of the allocation block to '0xFF'
	*/

	void MemoryManager::EnableAllocationFilling(bool allocationFilling)
	{
		s_allocationFilling = allocationFilling;
	}

	/*!
	* \brief Enables the logging of the allocation
	*
	* \param logAllocations If true, registers every allocation
	*/

	void MemoryManager::EnableAllocationLogging(bool logAllocations)
	{
		s_allocationLogging = logAllocations;
	}

	/*!
	* \brief Frees the pointer
	*
	* \param pointer Pointer to free
	* \param multi Array or not
	*
	* \remark If pointer is nullptr, nothing is done
	*/

	void MemoryManager::Free(void* pointer, bool multi)
	{
		if (!pointer)
			return;

		Block* ptr = reinterpret_cast<Block*>(static_cast<UInt8*>(pointer) - sizeof(Block));
		if (ptr->magic != s_allocatedId)
		{
			char timeStr[23];
			TimeInfo(timeStr);

			FILE* log = std::fopen(s_logFileName, "a");

			const char* error = (ptr->magic == s_freedId) ? "double-delete" : "possible delete of dangling pointer";
			if (s_nextFreeFile)
				std::fprintf(log, "%s Warning: %s at %s:%u\n", timeStr, error, s_nextFreeFile, s_nextFreeLine);
			else
				std::fprintf(log, "%s Warning: %s at unknown position\n", timeStr, error);

			std::fclose(log);
			return;
		}

		#if defined(NAZARA_PLATFORM_WINDOWS)
		EnterCriticalSection(&s_mutex);
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_lock(&s_mutex);
		#endif

		if (ptr->array != multi)
		{
			char timeStr[23];
			TimeInfo(timeStr);

			FILE* log = std::fopen(s_logFileName, "a");

			const char* error = (multi) ? "delete[] after new" : "delete after new[]";
			if (s_nextFreeFile)
				std::fprintf(log, "%s Warning: %s at %s:%u\n", timeStr, error, s_nextFreeFile, s_nextFreeLine);
			else
				std::fprintf(log, "%s Warning: %s at unknown position\n", timeStr, error);

			std::fclose(log);
		}

		ptr->magic = s_freedId;
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;

		s_allocatedBlock--;
		s_allocatedSize -= ptr->size;

		if (s_allocationFilling)
		{
			UInt8* data = reinterpret_cast<UInt8*>(ptr) + sizeof(Block);
			std::memset(data, 0xFF, ptr->size);
		}

		std::free(ptr);

		s_nextFreeFile = nullptr;
		s_nextFreeLine = 0;

		#if defined(NAZARA_PLATFORM_WINDOWS)
		LeaveCriticalSection(&s_mutex);
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_unlock(&s_mutex);
		#endif
	}

	/*!
	* \brief Gets the number of allocated blocks
	* \return Number of allocated blocks
	*/

	unsigned int MemoryManager::GetAllocatedBlockCount()
	{
		return s_allocatedBlock;
	}

	/*!
	* \brief Gets the allocated size
	* \return Size of total allocation
	*/

	std::size_t MemoryManager::GetAllocatedSize()
	{
		return s_allocatedSize;
	}

	/*!
	* \brief Gets the number of allocations
	* \return Number of allocations
	*/

	unsigned int MemoryManager::GetAllocationCount()
	{
		return s_allocationCount;
	}

	/*!
	* \brief Checks whether the filling of allocation is enabled
	* \return true if it is filling
	*/

	bool MemoryManager::IsAllocationFillingEnabled()
	{
		return s_allocationFilling;
	}

	/*!
	* \brief Checks whether the logging of allocation is enabled
	* \return true if it is logging
	*/

	bool MemoryManager::IsAllocationLoggingEnabled()
	{
		return s_allocationLogging;
	}

	/*!
	* \brief Sets the next free
	*
	* \param file Name of the file
	* \param line Line in the file
	*/

	void MemoryManager::NextFree(const char* file, unsigned int line)
	{
		s_nextFreeFile = file;
		s_nextFreeLine = line;
	}

	/*!
	* \brief Initializes the MemoryManager
	*/

	void MemoryManager::Initialize()
	{
		char timeStr[23];
		TimeInfo(timeStr);

		FILE* file = std::fopen(s_logFileName, "w");
		std::fprintf(file, "%s ==============================\n", timeStr);
		std::fprintf(file, "%s   Nazara Memory Leak Tracker  \n", timeStr);
		std::fprintf(file, "%s ==============================\n", timeStr);
		std::fclose(file);

		if (std::atexit(Uninitialize) != 0)
		{
			static MemoryManager manager;
		}

		#ifdef NAZARA_PLATFORM_WINDOWS
		InitializeCriticalSection(&s_mutex);
		//#elif defined(NAZARA_PLATFORM_POSIX) is already done in the namespace
		#endif

		s_initialized = true;
	}

	/*!
	* \brief Gets the time
	*
	* \param buffer Buffer to set the time in
	*/

	void MemoryManager::TimeInfo(char buffer[23])
	{
		time_t currentTime = std::time(nullptr);
		std::strftime(buffer, 23, "%d/%m/%Y - %H:%M:%S:", std::localtime(&currentTime));
	}

	/*!
	* \brief Uninitializes the MemoryManager
	*/

	void MemoryManager::Uninitialize()
	{
		#ifdef NAZARA_PLATFORM_WINDOWS
		DeleteCriticalSection(&s_mutex);
		#elif defined(NAZARA_PLATFORM_POSIX)
		pthread_mutex_destroy(&s_mutex);
		#endif

		FILE* log = std::fopen(s_logFileName, "a");

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
					std::fprintf(log, "-0x%s -> %zu bytes allocated at %s:%u\n", reinterpret_cast<UInt8*>(ptr) + sizeof(Block), ptr->size, ptr->file, ptr->line);
				else
					std::fprintf(log, "-0x%s -> %zu bytes allocated at unknown position\n", reinterpret_cast<UInt8*>(ptr) + sizeof(Block), ptr->size);

				void* pointer = ptr;
				ptr = ptr->next;

				std::free(pointer);
			}

			std::fprintf(log, "\n%u blocks leaked (%zu bytes)", s_allocatedBlock, s_allocatedSize);
		}

		std::fclose(log);
	}
}
