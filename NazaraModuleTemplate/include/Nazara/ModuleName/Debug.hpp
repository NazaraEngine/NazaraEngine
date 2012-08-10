// Copyright (C) 2012 AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/Config.hpp>
#if NAZARA_MODULENAME_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>

	#define delete NzMemoryManager::NextFree(__FILE__, __LINE__), delete
	#define new new(__FILE__, __LINE__)
#endif
